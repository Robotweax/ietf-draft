"""Create a separate instrumented export; refuse an unexpected source version."""
import argparse,hashlib,shutil,difflib
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--source',required=True);p.add_argument('--destination',required=True);args=p.parse_args()
src=Path(args.source);dst=Path(args.destination)
original=(src/'src/compat/connection.cpp').read_text()
assert hashlib.sha256(original.encode()).hexdigest()=='f3543b2006bc7e6e2b3bce9c4cefd028423b56f9bbb074a28c242ec448918192'
assert not dst.exists(), 'Use a new empty destination; do not overwrite source'
shutil.copytree(src,dst)
changed=original.replace('namespace robotweax::srt::compat {','extern "C" void srt_draft_native_probe(const char*, void*, unsigned) noexcept;\nextern "C" void srt_draft_native_timing(void*, unsigned long long, unsigned, long long) noexcept;\n\nnamespace robotweax::srt::compat {',1)
needle='        if (attach_runtime(socket_, setup_.channel, origin_,'
assert changed.count(needle)==1
changed=changed.replace(needle,'        if (protocol.has_peer_multicast_dispatch()) srt_draft_native_probe("before_runtime", &socket_, peer_handshake_timestamp.value());\n'+needle)
needle='        if (attach_dispatch_receiver(socket_, protocol) == SRT_ERROR) {'
assert changed.count(needle)==1
changed=changed.replace(needle,'        if (protocol.has_peer_multicast_dispatch()) srt_draft_native_probe("before_join", &socket_, peer_handshake_timestamp.value());\n'+needle)
needle='        publish_connected_state(socket_, SRTS_CONNECTED);'
assert changed.count(needle)==1
changed=changed.replace(needle,'        if (protocol.has_peer_multicast_dispatch()) srt_draft_native_probe("joined_before_publish", &socket_, peer_handshake_timestamp.value());\n'+needle+'\n        if (protocol.has_peer_multicast_dispatch()) srt_draft_native_probe("published", &socket_, peer_handshake_timestamp.value());')
needle='    std::shared_ptr<ConnectionRuntime> runtime;\n    std::optional<ConnectionRuntime::Configuration> configuration;'
assert changed.count(needle)==1
changed=changed.replace(needle,'    srt_draft_native_timing(&socket, handshake_arrival_microseconds, peer_handshake_timestamp.value(), std::chrono::duration_cast<std::chrono::microseconds>(origin.time_since_epoch()).count());\n'+needle)
(dst/'src/compat/connection.cpp').write_text(changed)
(root:=Path(__file__).resolve().parents[2]/'evidence').mkdir(exist_ok=True)
(root/'native-instrumentation.patch').write_text(''.join(difflib.unified_diff(original.splitlines(True),changed.splitlines(True),fromfile='a/src/compat/connection.cpp',tofile='b/src/compat/connection.cpp')))
print('Created instrumented export:',dst)
