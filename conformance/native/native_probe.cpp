#include "srt/srt.h"
#include "robotweax/srt/codec.hpp"
#include "compat/socket_registry.hpp"
#include "compat/group_registry.hpp"
#include "compat/transport_runtime.hpp"
#include <arpa/inet.h>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>
#include <thread>
#include <algorithm>
using namespace robotweax::srt::compat;
using namespace std::chrono_literals;
struct Event{std::string stage;int state;int group_state;bool runtime;bool joined;unsigned peer_timestamp;long long observed_us;int receive_key_state=-1;bool key_query_ok=false;};
std::mutex gate;std::condition_variable cv;std::vector<Event> events;size_t released=0;bool timed_out=false;std::atomic<bool> inject_join_failure{false};std::atomic<SRTSOCKET> target_group{SRT_INVALID_SOCK};long long sampled_epoch=0;unsigned sampled_peer=0;long long released_epoch=0;
extern "C" void srt_draft_native_probe(const char* stage,void* ptr,unsigned timestamp)noexcept{
 auto& socket=*static_cast<SocketRecord*>(ptr);Event e;e.stage=stage;e.peer_timestamp=timestamp;
 SRTSOCKET group;
 {std::lock_guard lock(socket.mutex);e.state=socket.state;e.runtime=bool(socket.runtime);group=socket.group_id;}
 int key_size=sizeof(e.receive_key_state);e.key_query_ok=srt_getsockflag(socket.protocol_socket_id,SRTO_RCVKMSTATE,&e.receive_key_state,&key_size)==0;
 e.group_state=srt_getsockstate(group);e.joined=false;
 if(auto record=GroupRegistry::instance().find(group)){std::lock_guard lock(record->mutex);e.joined=record->dispatch && record->dispatch->joined;}
 e.observed_us=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
 std::unique_lock lock(gate);events.push_back(e);size_t id=events.size();cv.notify_all();
 if(e.stage!="published" && !cv.wait_for(lock,5s,[&]{return released>=id;}))timed_out=true;
 lock.unlock();
 if(inject_join_failure && e.stage=="before_join"){
  std::lock_guard socket_lock(socket.mutex);
  socket.public_options.multicast_interface=robotweax::srt::IpEndpoint{.address={192,0,2,123}};
  socket.public_options.multicast_interface_set=true;
 }
}
extern "C" void srt_draft_native_timing(void* ptr,unsigned long long arrival,unsigned peer,long long origin)noexcept{
 auto& socket=*static_cast<SocketRecord*>(ptr);SRTSOCKET group;
 {std::lock_guard lock(socket.mutex);group=socket.group_id;}
 if(group==target_group){std::lock_guard lock(gate);sampled_epoch=origin+static_cast<long long>(arrival);sampled_peer=peer;}
}
void require(bool value,const char* msg){if(!value)throw std::runtime_error(msg);}
sockaddr_in address(const char* ip,unsigned short port){sockaddr_in a{};a.sin_family=AF_INET;a.sin_port=htons(port);require(inet_pton(AF_INET,ip,&a.sin_addr)==1,"address parse");return a;}
template<class T>void option(SRTSOCKET s,SRT_SOCKOPT o,const T& value){require(srt_setsockflag(s,o,&value,sizeof(value))==0,"socket option");}
struct Handles{std::vector<SRTSOCKET> list;SRTSOCKET keep(SRTSOCKET h){require(h!=SRT_INVALID_SOCK,"socket create");list.push_back(h);return h;}~Handles(){for(auto h:list)(void)srt_close(h);}};
int admit(void*,SRTSOCKET child,int,const sockaddr*,const char*){uint64_t domain=77;const char key[]="native-probe";if(srt_setsockflag(child,SRTO_ROBOTWEAX_GROUPDOMAIN,&domain,sizeof(domain)))return -1;return srt_setsockflag(child,SRTO_ROBOTWEAX_MULTICAST,key,sizeof(key)-1);}
void crypto_options(SRTSOCKET socket,int mode){
 if(!mode)return;
 const char pass[]="sixteen-char-key";
 require(srt_setsockflag(socket,SRTO_PASSPHRASE,pass,sizeof(pass)-1)==0,"passphrase");
 int length=32;option(socket,SRTO_PBKEYLEN,length);option(socket,SRTO_CRYPTOMODE,mode);
}
void wait_sender_key(SRTSOCKET group){
 auto record=GroupRegistry::instance().find(group);require(bool(record),"sender group record");
 auto deadline=std::chrono::steady_clock::now()+3s;
 for(;;){
  (void)GroupRegistry::instance().state(group);std::vector<SRTSOCKET> members;
  {std::lock_guard lock(record->mutex);for(auto& member:record->members)if(member.public_data.sockstate==SRTS_CONNECTED)members.push_back(member.public_data.id);}
  bool ready=!members.empty();
  for(auto member:members){int state=-1,size=sizeof(state);ready=ready&&srt_getsockflag(member,SRTO_SNDKMSTATE,&state,&size)==0&&state==SRT_KM_S_SECURED;}
  if(ready)return;
  require(std::chrono::steady_clock::now()<deadline,"sender group key readiness timeout");std::this_thread::sleep_for(2ms);
 }
}
// Loss injection uses the existing channel test seam. Other datagrams use native UDP.
struct KeyDelay {
 std::shared_ptr<DatagramChannel> channel;
 std::atomic<bool> hold{false};std::atomic<unsigned> dropped{0};
 static robotweax::srt::UdpIoResult send(std::span<const std::byte> bytes,robotweax::srt::IpEndpoint peer,void* context)noexcept{
  auto& self=*static_cast<KeyDelay*>(context);auto decoded=robotweax::srt::decode_packet(bytes);
  if(self.hold && decoded && decoded.packet.kind==robotweax::srt::PacketKind::control && decoded.packet.control.type==robotweax::srt::ControlType::user_defined && decoded.packet.control.subtype==robotweax::srt::key_material_request_subtype){++self.dropped;return {.bytes_transferred=bytes.size(),.peer=peer};}
  return self.channel->socket.send_to(bytes,peer);
 }
 void attach(SRTSOCKET listener){auto record=SocketRegistry::instance().find(listener);{std::lock_guard lock(record->mutex);channel=record->channel;}require(bool(channel),"listener channel");hold=true;channel->set_send_hook_for_testing(send,this);}
 ~KeyDelay(){if(channel)channel->set_send_hook_for_testing(nullptr,nullptr);}
};
struct Poll {
 int id=srt_epoll_create();
 ~Poll(){if(id>=0)(void)srt_epoll_release(id);}
 void add(SRTSOCKET socket){int flags=SRT_EPOLL_IN|SRT_EPOLL_OUT|SRT_EPOLL_ERR;require(id>=0 && srt_epoll_add_usock(id,socket,&flags)==0,"epoll add");}
 int sample(SRTSOCKET socket){SRT_EPOLL_EVENT items[4];int n=srt_epoll_uwait(id,items,4,0);require(n>=0,"epoll sample");int flags=0;for(int i=0;i<n;++i)if(items[i].fd==socket)flags|=items[i].events;return flags;}
};
int sender_key(SRTSOCKET group){auto record=GroupRegistry::instance().find(group);require(bool(record),"sender group");SRTSOCKET member=SRT_INVALID_SOCK;{std::lock_guard lock(record->mutex);require(record->members.size()==1,"one sender member");member=record->members.front().public_data.id;}int state=-1,size=sizeof(state);require(srt_getsockflag(member,SRTO_SNDKMSTATE,&state,&size)==0,"sender key query");return state;}
void run(bool fail_join,bool asynchronous,int mode,bool delay_keys=false,bool pressure=false,bool recover=false,bool idle_expiry=false){
 std::cout<<"CASE api="<<(asynchronous?"async":"sync")<<" crypto="<<mode<<" key_bytes="<<(mode?32:0)<<" idle_expiry="<<idle_expiry<<" recover="<<recover<<" pressure="<<pressure<<" delay_keys="<<delay_keys<<" fail_join="<<fail_join<<'\n';
 {std::lock_guard lock(gate);events.clear();released=0;timed_out=false;sampled_epoch=0;released_epoch=0;inject_join_failure=fail_join;}
 Handles sockets;auto listener=sockets.keep(srt_create_socket());auto multicast=address("239.255.133.99",15999);auto loopback=address("127.0.0.1",0);int ttl=0;
 if(pressure){int buffer=48000;option(listener,SRTO_SNDBUF,buffer);}
 if(recover){int idle=idle_expiry?2000:10000;option(listener,SRTO_PEERIDLETIMEO,idle);}
 crypto_options(listener,mode);option(listener,SRTO_ROBOTWEAX_MCAST_GROUP,multicast);option(listener,SRTO_ROBOTWEAX_MCAST_IFACE,loopback);option(listener,SRTO_ROBOTWEAX_MCAST_TTL,ttl);
 require(srt_listen_callback(listener,admit,nullptr)==0,"listen callback");auto local=loopback;require(srt_bind(listener,reinterpret_cast<sockaddr*>(&local),sizeof(local))==0,"bind");require(srt_listen(listener,4)==0,"listen");int size=sizeof(local);require(srt_getsockname(listener,reinterpret_cast<sockaddr*>(&local),&size)==0,"getsockname");
 KeyDelay delay;if(delay_keys)delay.attach(listener);
 auto receiver=sockets.keep(srt_create_group(ROBOTWEAX_SRT_GTYPE_MULTICAST));target_group=receiver;if(recover){int idle=idle_expiry?2000:10000;option(receiver,SRTO_PEERIDLETIMEO,idle); }crypto_options(receiver,mode);bool blocking=!asynchronous;option(receiver,SRTO_RCVSYN,blocking);auto interface=loopback;option(receiver,SRTO_ROBOTWEAX_MCAST_IFACE,interface);int timeout=3000;option(receiver,SRTO_CONNTIMEO,timeout);option(receiver,SRTO_RCVTIMEO,timeout);
 Poll receiver_poll;receiver_poll.add(receiver);
 auto future=std::async(std::launch::async,[&]{auto endpoint=srt_prepare_endpoint(nullptr,reinterpret_cast<const sockaddr*>(&local),sizeof(local));return srt_connect_group(receiver,&endpoint,1);});
 bool assertions=true;auto verify=[&](bool b){assertions=assertions&&b;};
 const std::vector<std::string> expected=fail_join?std::vector<std::string>{"before_runtime","before_join"}:std::vector<std::string>{"before_runtime","before_join","joined_before_publish","published"};
 for(size_t i=0;i<expected.size();++i){
  std::unique_lock lock(gate);if(!cv.wait_for(lock,5s,[&]{return events.size()>i;})){assertions=false;released=100;cv.notify_all();break;}
  const auto e=events[i];{int flags=receiver_poll.sample(receiver);verify(flags==(i==3?SRT_EPOLL_OUT:0));std::cout<<"EPOLL barrier="<<e.stage<<" receiver_flags="<<flags<<'\n';}verify(e.stage==expected[i]);verify(e.key_query_ok);verify(e.runtime==(i>0));verify(e.joined==(i>=2));verify(e.state==(i==3?SRTS_CONNECTED:SRTS_CONNECTING));verify(i==3 || e.group_state!=SRTS_CONNECTED);
  std::cout<<(fail_join?"FAILURE_PATH ":"SUCCESS_PATH ")<<e.stage<<" state="<<e.state<<" group="<<e.group_state<<" runtime="<<e.runtime<<" joined="<<e.joined<<" peer_timestamp="<<e.peer_timestamp<<" receive_key_state="<<e.receive_key_state<<" observed_us="<<e.observed_us<<'\n';
  if(i==0){
   if(asynchronous)verify(future.wait_for(100ms)==std::future_status::ready);
   // Hold the real coordinator before sampling, without a timing-sensitive poll.
   cv.wait_for(lock,60ms,[]{return false;});
   released_epoch=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
  }
  released=i+1;cv.notify_all();
 }
 auto result=future.get();
 if(asynchronous && fail_join){auto deadline=std::chrono::steady_clock::now()+3s;while(srt_getsockstate(receiver)==SRTS_CONNECTING && std::chrono::steady_clock::now()<deadline)std::this_thread::sleep_for(2ms);verify(srt_getsockstate(receiver)!=SRTS_CONNECTING && srt_getsockstate(receiver)!=SRTS_CONNECTED);}
 {std::lock_guard lock(gate);verify(!timed_out);verify(events.size()==expected.size());
 verify(!events.empty());if(!events.empty()){verify(sampled_epoch>=released_epoch && sampled_peer==events.front().peer_timestamp);
 verify(sampled_epoch-events.front().observed_us>=50000);
 std::cout<<"TIMING sample_minus_barrier_us="<<sampled_epoch-events.front().observed_us<<" sample_after_release="<<(sampled_epoch>=released_epoch)<<'\n';}}
 verify(fail_join&&!asynchronous?result==SRT_ERROR:result!=SRT_ERROR);
 if(fail_join){SRT_EPOLL_EVENT item;int count=srt_epoll_uwait(receiver_poll.id,&item,1,1000);require(count==1 && item.fd==receiver && item.events==(SRT_EPOLL_IN|SRT_EPOLL_OUT|SRT_EPOLL_ERR) && srt_getsockstate(receiver)==SRTS_BROKEN,"failed join BROKEN and IN/OUT/ERR");std::cout<<"JOIN_FAILED state="<<srt_getsockstate(receiver)<<" count="<<count<<" flags="<<(count>0?item.events:0)<<'\n';}
 if(!fail_join && assertions){auto sender=sockets.keep(srt_accept(listener,nullptr,nullptr));if(delay_keys){
  auto deadline=std::chrono::steady_clock::now()+3s;while(delay.dropped==0 && std::chrono::steady_clock::now()<deadline)std::this_thread::sleep_for(2ms);
  require(delay.dropped>0,"KM request intercepted");Poll sender_poll;sender_poll.add(sender);int pending=sender_key(sender);
  std::cout<<"KEY_HELD dropped="<<delay.dropped<<" sender_key="<<pending<<" receiver_flags="<<receiver_poll.sample(receiver)<<" sender_flags="<<sender_poll.sample(sender)<<'\n';
  require(pending==SRT_KM_S_SECURING,"sender must await held group KM");require(receiver_poll.sample(receiver)==SRT_EPOLL_OUT && sender_poll.sample(sender)==SRT_EPOLL_OUT,"OUT before key readiness");require(srt_getsockstate(receiver)==SRTS_CONNECTED,"receiver connected while key held");
  bool nonblocking=false;option(sender,SRTO_SNDSYN,nonblocking);
  if(pressure){
   char payload[1316]{};int accepted=0,last=0,error=0;
   for(int i=0;i<4096;++i){last=srt_sendmsg(sender,payload,sizeof(payload),-1,1);if(last<0){error=srt_getlasterror(nullptr);break;}require(last==sizeof(payload),"full pressure message");++accepted;}
   std::cout<<"PRESSURE accepted="<<accepted<<" result="<<last<<" error="<<error<<" sender_flags="<<sender_poll.sample(sender)<<'\n';
   require(accepted>0 && last==SRT_ERROR && error==SRT_EASYNCSND && sender_poll.sample(sender)==0,"bounded pressure rejects with EASYNCSND and clears OUT");
   auto held_until=std::chrono::steady_clock::now()+(recover?3s:1s);while(std::chrono::steady_clock::now()<held_until)std::this_thread::sleep_for(10ms);
   std::cout<<"PROLONGED dropped="<<delay.dropped<<" sender_key="<<sender_key(sender)<<" receiver_state="<<srt_getsockstate(receiver)<<" receiver_flags="<<receiver_poll.sample(receiver)<<'\n';
   if(idle_expiry){require(delay.dropped>=2 && srt_getsockstate(receiver)==SRTS_BROKEN && receiver_poll.sample(receiver)==(SRT_EPOLL_IN|SRT_EPOLL_OUT|SRT_EPOLL_ERR),"2-second idle expiry under 3-second KM loss");require(assertions,"idle admission invariants");std::cout<<"PASS peer idle expiry under sustained key loss"<<'\n';return;}
   require(delay.dropped>=2 && sender_key(sender)==SRT_KM_S_SECURING && srt_getsockstate(receiver)==SRTS_CONNECTED && receiver_poll.sample(receiver)==SRT_EPOLL_OUT,"repeated KM loss remains connected and securing");
   if(recover){
    require(srt_getsockstate(sender)==SRTS_CONNECTED,"sender alive within configured idle horizon");delay.hold=false;wait_sender_key(sender);
    bool nonblocking_receive=false;option(receiver,SRTO_RCVSYN,nonblocking_receive);int received_count=0;auto drain_until=std::chrono::steady_clock::now()+1500ms;
    while(std::chrono::steady_clock::now()<drain_until){char data[1500]{};int got=srt_recvmsg(receiver,data,sizeof(data));if(got>0){require(got==sizeof(payload) && std::all_of(data,data+got,[](char c){return c==0;}),"pressure payload bytes");++received_count;}else{require(got==SRT_ERROR && srt_getlasterror(nullptr)==SRT_EASYNCRCV,"nonblocking drain empty");std::this_thread::sleep_for(2ms);}}
    std::cout<<"RECOVERY accepted="<<accepted<<" received="<<received_count<<" sender_key="<<sender_key(sender)<<" sender_flags="<<sender_poll.sample(sender)<<'\n';
    require(received_count<=accepted,"no extra pressure messages");const char marker[]="after sustained key loss";require(srt_sendmsg(sender,marker,sizeof(marker),-1,1)==sizeof(marker),"fresh marker accepted after recovery");bool blocking_receive=true;option(receiver,SRTO_RCVSYN,blocking_receive);char data[1500]{};int got=srt_recvmsg(receiver,data,sizeof(data));require(got==sizeof(marker) && std::string(data)==marker,"fresh marker exact after recovery");
    require(assertions,"recovery admission invariants");std::cout<<"PASS sustained key loss recovery and fresh DATA"<<'\n';return;
   }
   bool blocking=true;option(receiver,SRTO_RCVSYN,blocking);
   int flags=SRT_EPOLL_IN|SRT_EPOLL_ERR;require(srt_epoll_update_usock(receiver_poll.id,receiver,&flags)==0,"pending epoll interest");
   auto read_wait=std::async(std::launch::async,[&]{char data[1500];int result=srt_recvmsg(receiver,data,sizeof(data));return std::pair{result,result<0?srt_getlasterror(nullptr):0};});
   auto poll_wait=std::async(std::launch::async,[&]{SRT_EPOLL_EVENT item;int count=srt_epoll_uwait(receiver_poll.id,&item,1,500);return std::pair{count,count>0?item.events:(count<0?srt_getlasterror(nullptr):0)};});
   bool read_pending=read_wait.wait_for(50ms)==std::future_status::timeout;bool poll_pending=poll_wait.wait_for(0ms)==std::future_status::timeout;
   require(srt_close(receiver)==0,"pressure receiver close");std::erase(sockets.list,receiver);
   require(read_wait.wait_for(1s)==std::future_status::ready && poll_wait.wait_for(1s)==std::future_status::ready,"pressure waiters bounded");auto read_result=read_wait.get();auto poll_result=poll_wait.get();
   std::cout<<"PRESSURE_CLOSE read_pending="<<read_pending<<" poll_pending="<<poll_pending<<" read="<<read_result.first<<" read_error="<<read_result.second<<" poll="<<poll_result.first<<" poll_detail="<<poll_result.second<<'\n';
   require(read_pending && poll_pending && read_result.first==SRT_ERROR && read_result.second==SRT_ESCLOSED && poll_result.first==1 && poll_result.second==SRT_EPOLL_ERR,"close completes receive and epoll with expected results");require(assertions,"pressure admission invariants");std::cout<<"PASS prolonged key loss pressure and close assertions"<<'\n';return;
  }
  const char early[]="pending key payload";int sent=srt_sendmsg(sender,early,sizeof(early),-1,1);int send_error=sent==SRT_ERROR?srt_getlasterror(nullptr):0;
  std::cout<<"EARLY_SEND result="<<sent<<" error="<<send_error<<" key="<<sender_key(sender)<<'\n';
  require(sent==sizeof(early) && sender_key(sender)==SRT_KM_S_SECURING,"pending-key send accepted while securing");
  delay.hold=false;wait_sender_key(sender);
  if(sent>0){bool blocking=true;option(receiver,SRTO_RCVSYN,blocking);int early_timeout=500;option(receiver,SRTO_RCVTIMEO,early_timeout);char received[64]{};int got=srt_recvmsg(receiver,received,sizeof(received));std::cout<<"EARLY_RECEIVE result="<<got<<" error="<<(got<0?srt_getlasterror(nullptr):0)<<" exact="<<(got==sizeof(early)&&std::string(received)==early)<<'\n';require(got==sizeof(early)&&std::string(received)==early,"pending-key payload exact after recovery");option(receiver,SRTO_RCVTIMEO,timeout);}

  std::cout<<"KEY_RELEASED sender_key="<<sender_key(sender)<<" receiver_flags="<<receiver_poll.sample(receiver)<<" sender_flags="<<sender_poll.sample(sender)<<'\n';
 }else if(mode)wait_sender_key(sender);bool blocking_receive=true;option(receiver,SRTO_RCVSYN,blocking_receive);const char text[]="native join ready";require(srt_sendmsg(sender,text,sizeof(text),-1,1)==sizeof(text),"send after published");if(delay_keys){int flags=SRT_EPOLL_IN|SRT_EPOLL_ERR;require(srt_epoll_update_usock(receiver_poll.id,receiver,&flags)==0,"poll receive interest");SRT_EPOLL_EVENT item;require(srt_epoll_uwait(receiver_poll.id,&item,1,3000)==1 && item.fd==receiver && item.events==SRT_EPOLL_IN,"IN for delivered DATA");std::cout<<"DATA_READY receiver_flags="<<item.events<<'\n';}char output[64]{};require(srt_recvmsg(receiver,output,sizeof(output))==sizeof(text),"receive after native join");require(std::string(output)==text,"payload exact");if(delay_keys){require(receiver_poll.sample(receiver)==0,"IN clears after DATA drained");
 auto waiting=std::async(std::launch::async,[&]{char data[64];int result=srt_recvmsg(receiver,data,sizeof(data));return std::pair{result,result<0?srt_getlasterror(nullptr):0};});
 require(waiting.wait_for(50ms)==std::future_status::timeout,"receive pending before close");require(srt_close(receiver)==0,"close receiver with pending receive");std::erase(sockets.list,receiver);
 require(waiting.wait_for(1s)==std::future_status::ready,"receive completes after close");auto closed=waiting.get();std::cout<<"CLOSE_WAIT result="<<closed.first<<" error="<<closed.second<<'\n';require(closed.first==SRT_ERROR && closed.second==SRT_ESCLOSED,"closed receive reports socket closed");
 }}
 require(assertions,"native publication ordering or join failure invariant");
 std::cout<<"PASS "<<(fail_join?"native join failure never publishes CONNECTED":"native join completes before CONNECTED and delivers DATA")<<'\n';
}
int main(){try{require(srt_startup()==0,"startup");for(bool asynchronous:{false,true})for(int mode:{0,1,2})for(bool failure:{false,true})run(failure,asynchronous,mode);for(bool asynchronous:{false,true})for(int mode:{1,2})run(false,asynchronous,mode,true);for(bool asynchronous:{false,true})for(int mode:{1,2})run(false,asynchronous,mode,true,true);for(bool asynchronous:{false,true})for(int mode:{1,2})run(false,asynchronous,mode,true,true,true);for(bool asynchronous:{false,true})for(int mode:{1,2})run(false,asynchronous,mode,true,true,true,true);require(srt_cleanup()==0,"cleanup");}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';(void)srt_cleanup();return 1;}}
