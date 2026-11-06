# Native multicast publication probe

This probe exercises public Caller/Listener APIs through the real connection
coordinator and native IPv4 multicast join. It links a **separately instrumented**
export of SRT 0.3.2. It does not modify the immutable export or the user's SRT
checkout. It is not an unmodified-release binary qualification run.

`instrument.py` checks the exact connection.cpp source hash, refuses to overwrite
an existing destination, copies the source, and adds observation/barrier calls.
The complete patch is retained in `evidence/native-instrumentation.patch`.
No production algorithm or protocol bytes are replaced. Probe callbacks pause
the caller and inspect state. For the failure case only, the callback substitutes
an unavailable receiver interface immediately before native join, causing the
real OS membership operation to fail after runtime attachment.

## Reproduce

From the local draft directory, using a fresh destination:

```sh
python3 conformance/native/instrument.py \
  --source /path/to/verified-032-export \
  --destination /tmp/srt-native-instrumented
cmake -S conformance/native -B /tmp/srt-native-build \
  -DSRT_SOURCE=/tmp/srt-native-instrumented -DCMAKE_BUILD_TYPE=Release
cmake --build /tmp/srt-native-build --target native_probe --parallel 4
/tmp/srt-native-build/native_probe
```

Set OPENSSL_ROOT_DIR if necessary. Local UDP binding and loopback multicast must
be permitted. The probe uses 239.255.133.99:15999, TTL 0, with an explicit loopback
interface. The failure case assumes 192.0.2.123 is not a local interface address;
if the host assigns it, use a verified unassigned local-interface selector and
record the fixture change. Tests have bounded five-second barriers and a
three-second connection timeout; a timeout is failure, not a skipped pass.

## Assertions

- Before runtime attachment: member/group CONNECTING, no runtime, no join.
- Before native join: still CONNECTING, runtime exists, no join.
- After successful join but before publication: still CONNECTING, runtime and
  native receive-group membership exist.
- After publication: member/group CONNECTED, runtime and join exist, exact DATA
  is delivered through the public API.
- Native join failure after runtime installation: no joined/published callback,
  connect returns an error, no CONNECTED publication observed at the barriers.
- The runtime receives a local timing sample taken after the controlled 60 ms
  barrier, paired with the original peer handshake timestamp. It is not a saved
  UDP-ingress time carried through that pause.

The barriers establish order without polling for transient states. The deliberate
60 ms pause tests timing-sample provenance; it does not establish a network latency
bound. This run covers plaintext, synchronous Caller/Listener APIs on macOS
ARM64. Asynchronous API publication, encrypted join readiness, teardown races,
other platforms and wide-area timing remain separate qualification work.

## Expanded matrix

The current executable runs 12 cases: sync/async connect × plaintext/CTR256/GCM256
× successful/failed native join. Async connect must return before the first
barrier is released; late failure is checked through subsequent group state.
Every receive-key query must succeed. Encrypted success cases wait up to three
seconds for sender member SNDKMSTATE=SECURED before sending DATA. Receive mode
is restored to blocking after admission for the payload assertion. Key-state
values at publication are observations, not asserted timing requirements.

Capture stdout and stderr separately to avoid interleaved records. Historical
logs retain their original scope; see the expanded results in
[the report](../../docs/native-publication-results.md). Deterministic key delay,
epoll readiness, and teardown races remain untested by this probe.

## Controlled key loss and epoll assertions

The executable now adds four successful encrypted cases with runtime KMREQ loss,
for 16 cases total. An RAII hook on the listener DatagramChannel suppresses only
decoded user-defined subtype-3 KMREQ while its atomic hold flag is set; all other
packets use the channel's native UDP socket. Removing the hold permits normal
retry. Hook removal is synchronized with sends and precedes context destruction.

The added cases assert level-triggered group epoll state at admission barriers,
OUT while sender SNDKMSTATE is SECURING, eventual SECURED after loss ends, and IN
for DATA followed by no IN after draining it. Socket and epoll resources are closed
locally. No packet is sent externally (loopback interface, multicast TTL 0).
Capture stdout and stderr separately. Pending-key DATA, edge-triggered events,
failed-join epoll events, and concurrent teardown are outside this test.

## Lifecycle extension

The current 16-case executable also checks failed-join BROKEN/IN|OUT|ERR in all
six failure cases. The four controlled-loss cases use nonblocking send to submit
one 20-byte payload while the sender is SECURING, release KMREQ loss, and require
exact delivery within a 500 ms receive budget. They then run the ordinary DATA
readiness check and close the receiver while a receive future is pending. The
future must return SRT_ERROR/SRT_ESCLOSED within one second of close. The preceding
50 ms wait observes a pending future; it is not an internal receive-entry barrier.
These changes supersede the earlier exclusions for this narrow pending-key DATA,
failed-join event and receive/close schedule only. Prolonged key loss, queue
pressure, epoll cancellation and arbitrary teardown races remain open.

## Queue pressure and pending epoll close

The current executable runs 20 cases. Four new cases configure SNDBUF=48,000
before admission and submit 1,316-byte nonblocking messages while KMREQ is held,
with a 4,096-attempt cap. They require positive acceptance followed by EASYNCSND
and cleared sender OUT. They then hold KMREQ for another second, require multiple
suppressed retries while CONNECTED/SECURING, and close the receiver with receive
and IN|ERR epoll futures pending. Expected results are ESCLOSED and one ERR event.
The epoll call has a 500 ms timeout; completion after close is checked within one
second. No pressure-case key recovery or queued-message delivery is asserted.
The 32 accepted messages and 11 suppressed retries in the recorded run are
fixture observations, not fixed protocol requirements.

## Sustained-loss recovery versus idle expiry

The executable now runs 28 cases, adding four three-second recovery cases and
four three-second idle-expiry cases. Both use pressure and held runtime KMREQ.
Recovery explicitly configures PEERIDLETIMEO=10,000 ms, releases KMREQ after the
hold, drains for 1,500 ms with nonblocking receive, checks zero-filled payloads
and a bounded count, then requires exact delivery of a distinct marker. Expiry
uses PEERIDLETIMEO=2,000 ms and requires receiver BROKEN/IN|OUT|ERR without releasing
keys. Neither interval is a new protocol timeout requirement. The source audit
also executes the existing retry-cadence, UDP-acceptance and last-valid-packet
idle tests; their logs and anchors are tracked separately.
