#include "robotweax/srt/handshake_datagram.hpp"
#include "compat/transport_runtime.hpp"
#include "compat/group_registry.hpp"
#include <array>
#include <iostream>
#include <stdexcept>
using namespace robotweax::srt;
using namespace robotweax::srt::compat;
namespace {
void require(bool b,const char* why){if(!b)throw std::runtime_error(why);}
struct Admission {bool allowed=true;bool wrong=false;uint32_t cursor=8000;};
bool group(const GroupMembership& peer,GroupMembership& local,void* ctx)noexcept{
 auto& c=*static_cast<Admission*>(ctx);if(!c.allowed || peer.type!=GroupType::multicast)return false;
 local={.group_id=0x40000042,.type=GroupType::multicast};return true;
}
bool dispatch(MulticastDispatch& out,void* ctx)noexcept{
 auto& c=*static_cast<Admission*>(ctx);out={.group_id=c.wrong?0x40000043U:0x40000042U,.multicast_group={.address={239,255,133,11},.port=10000},.initial_sequence=c.cursor};return true;
}
uint32_t cookie(const Handshake&,void*)noexcept{return 0x12345678;}
HandshakeDatagramDecodeResult transmit(const HandshakeAction& action,uint32_t timestamp){
 std::array<std::byte,1500> bytes{};auto encoded=encode_handshake_datagram(action,PacketTimestamp{timestamp},0,bytes);require(bool(encoded),"handshake encode");auto decoded=decode_handshake_datagram(std::span(bytes).first(encoded.bytes_written));require(bool(decoded),"handshake decode");return decoded;
}
uint64_t read_clock(void* p)noexcept{return *static_cast<uint64_t*>(p);}
void scenario(uint32_t sender_time,uint64_t extra_delay,bool allowed,bool wrong){
 Admission policy{allowed,wrong};
 HandshakeMachine caller({.role=ConnectionRole::caller,.local_socket_id=101,.initial_sequence=SequenceNumber{123},.has_group_membership=true,.group_membership={.group_id=0x40000001,.type=GroupType::multicast},.group_membership_policy={.admit_multicast_dispatch=true}});
 HandshakeMachine listener({.role=ConnectionRole::listener,.local_socket_id=102,.initial_sequence=SequenceNumber{8000},.group_membership_negotiator=group,.group_membership_context=&policy,.group_membership_policy={.admit_multicast_dispatch=true},.multicast_dispatch_provider=dispatch,.multicast_dispatch_context=&policy,.cookie_generator=cookie});
 auto induction=transmit(caller.start().values[0],0);auto reply=transmit(listener.receive(induction.message).values[0],sender_time);auto conclusion=caller.receive(reply.message);
 auto request=transmit(conclusion.values[0],100);auto response=listener.receive(request.message);
 if(!allowed || wrong){
  // A refusal is terminal on listener or on caller after processing response.
  if(listener.state()!=HandshakeState::rejected){auto denied=transmit(response.values[0],sender_time);(void)caller.receive(denied.message);}
  require(listener.state()==HandshakeState::rejected || caller.state()==HandshakeState::rejected,"admission mismatch rejected");return;
 }
 auto accepted=transmit(response.values[0],sender_time);(void)caller.receive(accepted.message);
 require(caller.state()==HandshakeState::connected && caller.has_peer_multicast_dispatch(),"caller dispatch admission");
 auto retry=listener.receive(request.message);auto repeated=transmit(retry.values[0],sender_time);
 require(repeated.message.multicast_dispatch.initial_sequence==8000 && repeated.message.multicast_dispatch.group_id==0x40000042,"duplicate conclusion keeps announcement");
 uint64_t now=1000000+extra_delay;const uint64_t arrival=now;
 SocketOptions options;require(options.set(SocketOption::tsbpd_mode,1)==Error::none,"enable TSBPD");
 auto group_record=std::make_shared<GroupRecord>();group_record->category=GroupCategory::dispatch;
 // Deliberate integration seam: emulate the production attach_runtime inputs.
 // Does not invoke native join, listener callback scheduling or socket publication.
 ConnectionRuntime runtime({.group=group_record,.peer=IpEndpoint::loopback(9000),.local_socket_id=101,.peer_socket_id=102,.initial_sequence=SequenceNumber{123},.peer_initial_sequence=SequenceNumber{caller.peer_multicast_dispatch().initial_sequence},.has_distinct_peer_initial_sequence=true,.options=options,.negotiated_options={.receive_tsbpd=true,.receive_delay_milliseconds=120},.origin={},.handshake_arrival_microseconds=arrival,.peer_handshake_timestamp=accepted.control.timestamp,.now_function=read_clock,.now_context=&now});
 require(group_record->receive_clock!=nullptr,"runtime installed shared receive clock");
 const std::array<std::byte,1> payload{std::byte{'x'}};PacketView packet;packet.kind=PacketKind::data;packet.data={.sequence=SequenceNumber{7999},.message_number=1,.boundary=MessageBoundary::solo,.in_order=true,.timestamp=PacketTimestamp{sender_time+20000},.destination_socket_id=0x40000042};packet.payload=payload;
 runtime.process_packet(packet,IpEndpoint::loopback(9000));require(!runtime.has_buffered_receive_data(),"prejoin sequence excluded");
 packet.data.sequence=SequenceNumber{8000};runtime.process_packet(packet,IpEndpoint::loopback(9001));require(!runtime.has_buffered_receive_data(),"foreign endpoint excluded");
 runtime.process_packet(packet,IpEndpoint::loopback(9000));std::array<std::byte,8> output{};
 now=arrival+140000-1;auto early=runtime.receive_message(output,false,0);require(early.bytes==0,"runtime withheld early DATA");
 now++;auto delivered=runtime.receive_message(output,false,0);require(delivered.status==MessageIoStatus::success && delivered.bytes==1 && output[0]==std::byte{'x'},"runtime exact delivery deadline");
 require(delivered.first_sequence==SequenceNumber{8000},"announcement cursor honored");
}
}
void admission_handoff(){
 for(uint32_t age:{9000000U,0xfffff000U})for(uint64_t delay:{0ULL,50000ULL})scenario(age,delay,true,false);
 scenario(9000000,0,false,false);scenario(9000000,0,true,true);
 std::cout<<"PASS six deterministic serialized-admission/runtime-handoff cases (native join and publication excluded)\n";
}
