#include "robotweax/srt/crypto.hpp"
#include "robotweax/srt/session_authentication.hpp"
#include "robotweax/srt/connection_group.hpp"
#include "robotweax/srt/timing.hpp"
#include "fixtures.hpp"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
using namespace robotweax::srt;
using Bytes=std::vector<std::byte>;
void check(bool v,const char* label){if(!v)throw std::runtime_error(label);}
void ok(Error e,const char* label){check(e==Error::none,label);}
Bytes unhex(std::string_view s){Bytes b;for(size_t i=0;i<s.size();i+=2)b.push_back(std::byte(std::stoul(std::string(s.substr(i,2)),nullptr,16)));return b;}
std::string hex(std::span<const std::byte> b){std::string s;for(auto x:b){unsigned v=std::to_integer<unsigned>(x);s+="0123456789abcdef"[v>>4];s+="0123456789abcdef"[v&15];}return s;}
void matches(std::span<const std::byte> b,std::string_view expected,const char* label){check(hex(b)==expected,label);}
// Deterministic randomness only for this test process; all primitives delegate.
struct Provider final:CryptoProvider{
 unsigned next;CryptoProvider& p=default_crypto_provider();
 explicit Provider(unsigned n):next(n){}
 Error random_bytes(std::span<std::byte> d)noexcept override{for(auto& x:d)x=std::byte(next++ & 255U);return Error::none;}
 Error pbkdf2_hmac_sha1(std::span<const std::byte>a,std::span<const std::byte>b,uint32_t n,std::span<std::byte>o)noexcept override{return p.pbkdf2_hmac_sha1(a,b,n,o);}
 Error hmac_sha256(std::span<const std::byte>a,std::span<const std::byte>b,std::span<std::byte,32>o)noexcept override{return p.hmac_sha256(a,b,o);}
 Error wrap_key(std::span<const std::byte>a,std::span<const std::byte>b,std::span<std::byte>o,size_t&n)noexcept override{return p.wrap_key(a,b,o,n);}
 Error unwrap_key(std::span<const std::byte>a,std::span<const std::byte>b,std::span<std::byte>o,size_t&n)noexcept override{return p.unwrap_key(a,b,o,n);}
 Error make_aes_ctr_cipher(std::span<const std::byte> k,std::unique_ptr<PayloadCipher>&c)noexcept override{return p.make_aes_ctr_cipher(k,c);}
 bool supports_aes_gcm()const noexcept override{return p.supports_aes_gcm();}
 Error make_aes_gcm_cipher(std::span<const std::byte>k,std::unique_ptr<AuthenticatedPayloadCipher>&c)noexcept override{return p.make_aes_gcm_cipher(k,c);}
 void secure_erase(std::span<std::byte>b)noexcept override{p.secure_erase(b);}
};
void establish(SessionAuthentication&c,SessionAuthentication&l){ok(c.start(true),"caller start");ok(l.start(false),"listener start");auto pass=unhex(password_hex);ok(l.establish(pass,c.parameters(),0x05060708,0x01020304),"listener establish");ok(c.establish(pass,l.parameters(),0x01020304,0x05060708),"caller establish");}
void auth_vectors(){
 Provider cp(0),lp(32);SessionAuthentication c(cp),l(lp);establish(c,l);
 auto body=unhex(synthetic_km_body_hex);SessionAuthenticationParameters proof;
 ok(l.handshake_proof(false,body,proof),"server proof");matches(proof.proof,listener_proof_tag,"listener proof vector");check(c.verify_handshake(false,body,proof),"verify server proof");
 ok(c.handshake_proof(true,body,proof),"caller proof");matches(proof.proof,caller_proof_tag,"caller proof vector");
 Bytes wire(512);size_t n=0;std::span<const std::byte> opened;
 ok(c.seal(false,body,wire,n),"request seal");matches(std::span(wire).first(n),caller_kmreq_content,"request vector");
 auto saved=Bytes(wire.begin(),wire.begin()+n);check(l.open(false,saved,opened),"open request");check(l.open(false,saved,opened),"identical KM retry");
 ok(l.seal(true,body,wire,n),"response seal");matches(std::span(wire).first(n),listener_kmrsp_content,"response vector");check(c.open(true,std::span(wire).first(n),opened),"open response");
 auto tamper=saved;tamper.back()^=std::byte{1};check(!l.open(false,tamper,opened),"tamper rejected");
 auto newer=body;newer[0]^=std::byte{1};ok(c.seal(false,newer,wire,n),"newer request");check(l.open(false,std::span(wire).first(n),opened),"newer accepted");check(!l.open(false,saved,opened),"stale request rejected");
 DataHeader h{.sequence=SequenceNumber{9},.message_number=0,.boundary=MessageBoundary::solo,.timestamp=PacketTimestamp{1234},.destination_socket_id=0x05060708};
 auto parity=unhex(fec_body_hex);ok(c.seal_fec(h,parity,wire,n),"FEC seal");matches(std::span(wire).first(n),caller_fec_content,"FEC vector");check(l.open_fec(h,std::span(wire).first(n),opened),"FEC open");check(!l.open_fec(h,std::span(wire).first(n),opened),"FEC replay rejected");
 std::cout<<"PASS frozen authentication vectors and KM/FEC retry distinction\n";
}
void multicast_vectors(){
 for(auto expected:{ipv4_dispatch,ipv6_dispatch}){
  auto bytes=unhex(expected);auto ext=decode_extension(bytes);check(bool(ext),"extension decode");auto decoded=decode_multicast_dispatch(ext.extension);check(bool(decoded),"dispatch decode");
  check(decoded.dispatch.group_id==0x40000042 && decoded.dispatch.initial_sequence==0x7ffffffd && decoded.dispatch.timestamp_origin_delta_microseconds==0,"dispatch values");
  Bytes encoded(64);auto result=encode_multicast_dispatch(decoded.dispatch,encoded);check(bool(result),"dispatch encode");matches(encoded,expected,"dispatch exact bytes");
  bytes[61]=std::byte{1};auto invalid=decode_extension(bytes);check(bool(invalid)&&!decode_multicast_dispatch(invalid.extension),"reserved tail rejected");
 }
 std::cout<<"PASS frozen IPv4/IPv6 dispatch decode/encode and reserved-byte rejection\n";
}
void valid_km(CryptoMode mode, size_t key_length){
 Provider cp(0),lp(32);SessionAuthentication c(cp),l(lp);establish(c,l);
 Provider keys(80);CryptoConfiguration cfg{.passphrase="conformance fixture password",.mode=mode,.enable_aes_gcm=true,.key_length=key_length,.refresh_rate_packets=3,.preannouncement_packets=1};CryptoSession sender(cfg,keys),receiver(cfg);
 ok(sender.start_initiator(),"KM start");auto req=sender.pending_key_material();Bytes original(req.begin(),req.end());
 Bytes wire(512);size_t n=0;std::span<const std::byte> body;ok(c.seal(false,original,wire,n),"valid KM envelope");check(l.open(false,std::span(wire).first(n),body),"valid KM open");ok(receiver.accept_key_material(body,false),"valid wrapped KM accepted");
 auto reply=receiver.key_material_response();check(std::equal(reply.begin(),reply.end(),original.begin(),original.end()),"KM echo");
 ok(l.seal(true,reply,wire,n),"valid KMRSP seal");check(c.open(true,std::span(wire).first(n),body),"valid KMRSP open");ok(sender.acknowledge_key_material(body,false),"semantic acknowledge");check(sender.ready_to_send_data(),"sender ready");
 if(mode==CryptoMode::aes_ctr && key_length==16)std::cout<<"KM_BODY_HEX "<<hex(original)<<"\n";
 const auto mode_name=mode==CryptoMode::aes_gcm?"gcm":"ctr";
 std::cout<<"KM_CASE "<<mode_name<<" "<<key_length<<" "<<hex(original)<<"\n";
 const Bytes clear=unhex("00112233445566778899aabbccddeeff10203040");Bytes cipher(clear.size()),opened(clear.size());std::array<std::byte,16>tag{};EncryptionKey selected{};
 DataHeader data{.sequence=SequenceNumber{100},.message_number=7,.boundary=MessageBoundary::solo,.in_order=true,.encryption_key=sender.active_sender_key(),.timestamp=PacketTimestamp{123456},.destination_socket_id=0x05060708};
 if(mode==CryptoMode::aes_gcm){
  ok(sender.seal(data,clear,cipher,tag,selected),"GCM DATA seal");auto bad=tag;bad[0]^=std::byte{1};check(receiver.open(data,cipher,bad,opened)!=Error::none,"GCM tag tamper");ok(receiver.open(data,cipher,tag,opened),"GCM DATA open");
  auto repeat=data;repeat.retransmitted=true;ok(receiver.open(repeat,cipher,tag,opened),"GCM retransmission-bit normalization");
 }else{ok(sender.encrypt(data.sequence,clear,cipher,selected),"CTR DATA encrypt");ok(receiver.decrypt(selected,data.sequence,cipher,opened),"CTR DATA decrypt");}
 check(opened==clear,"DATA roundtrip");std::cout<<"DATA_CASE "<<mode_name<<" "<<key_length<<" "<<hex(clear)<<" "<<hex(cipher)<<" "<<hex(tag)<<"\n";
 ok(sender.note_data_packet_sent(),"first consumed position");ok(sender.note_data_packet_sent(),"second consumed position");ok(sender.prepare_rotation(),"prepare rotation");auto pending=sender.pending_key_material();Bytes successor(pending.begin(),pending.end());check(!successor.empty() && successor!=original,"successor differs");
 ok(sender.acknowledge_key_material(original,false),"old body idempotent success");matches(sender.pending_key_material(),hex(successor),"old body did not replace pending material");check(sender.sender_state()==CryptoState::securing,"old response did not confirm rotation");
 ok(c.seal(false,successor,wire,n),"rotation seal");check(l.open(false,std::span(wire).first(n),body),"rotation open");ok(receiver.accept_key_material(body,false),"rotation install");ok(l.seal(true,receiver.key_material_response(),wire,n),"rotation response");check(c.open(true,std::span(wire).first(n),body),"rotation response open");ok(sender.acknowledge_key_material(body,false),"rotation confirm");
 std::cout<<"PASS "<<mode_name<<"-"<<key_length*8<<" valid KM, DATA, authenticated rotation and stale-body semantic check\n";
}
void late_clock(){
 // Component-level model, not real multicast admission or shared clock wiring.
 for(uint64_t sender_age:{9000000ULL,3900000000ULL}){
  TsbpdClock late(1000000,PacketTimestamp{uint32_t(sender_age)},120000);
  check(late.delivery_time(PacketTimestamp{uint32_t(sender_age+20000)})==1140000,"late-join origin mapping");
  check(!late.ready(PacketTimestamp{uint32_t(sender_age+20000)},1139999),"not early");check(late.ready(PacketTimestamp{uint32_t(sender_age+20000)},1140000),"deadline ready");
 }
 TsbpdClock wrap(1000000,PacketTimestamp{0xfffff000},120000);check(wrap.delivery_time(PacketTimestamp{0x1000})==1128192,"join near timestamp wrap");
 TsbpdClock delayed(1050000,PacketTimestamp{9000000},120000);check(delayed.delivery_time(PacketTimestamp{9020000})==1190000,"arrival delay shifts initial base");
 std::cout<<"PASS deterministic late-join clock component cases (not full network admission)\n";
}
void admission_handoff();
int main(){try{auth_vectors();multicast_vectors();for(auto mode:{CryptoMode::aes_ctr,CryptoMode::aes_gcm})for(size_t size:{16U,24U,32U})valid_km(mode,size);late_clock();admission_handoff();}catch(const std::exception&e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
