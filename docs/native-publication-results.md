# Native publication and timestamp capture — 2026-11-06

Two instrumented native loopback scenarios passed. All changes are local and
uncommitted; no external write or submission occurred.

## Observed transition order

| Barrier | Member state | Group state | Runtime | Joined |
|---|---|---|---|---|
| Before runtime | CONNECTING | CONNECTING | No | No |
| Before join | CONNECTING | CONNECTING | Yes | No |
| Joined, before publication | CONNECTING | CONNECTING | Yes | Yes |
| Published | CONNECTED | CONNECTED | Yes | Yes |

Public API DATA delivery passed after successful publication. In the failure
scenario, an unavailable interface was injected immediately before native join;
the OS join failed after runtime installation, connect returned an error, and
neither joined nor published was observed. The first attempt configured the
interface too early and failed before this boundary; its log remains available
and is not counted as a successful failure-path test.

## Timing observation

The callback captured the exact local timing argument entering attach_runtime.
A controlled 60 ms pre-attachment pause produced sample-minus-barrier intervals
of 61,185 microseconds on success and 61,089 on failure. Both samples were taken
after barrier release and paired with the unchanged peer handshake timestamp.
Thus the caller path's variable named handshake_arrival_microseconds represents
a setup-time sample at runtime attachment, not a retained kernel/UDP arrival
timestamp. These numbers characterize this run, not latency guarantees.

The draft should describe that observed rule accurately. Whether a future
interoperable profile should require earlier ingress timestamp capture, and how
it accounts for setup scheduling delay, remains a protocol-design decision.
This experiment alone does not establish a security or delivery defect.

## Provenance and limits

The probe links a separate instrumented 0.3.2 source export. All original tracked
regular files were byte-verified against the Git object; only connection.cpp
changes in the instrumented copy. The patch adds observations/barriers and a
capture of the existing timing arguments. The failure hook changes test socket
configuration before the real native join. No release source or public API was
modified. Review the [patch](../evidence/native-instrumentation.patch),
[log](../evidence/native-publication.log),
[manifest](../evidence/run-manifest.json), and
[reproduction instructions](../conformance/native/README.md).

This closes the narrow source-order question for plaintext synchronous
Caller/Listener on this host. It does not qualify every connection mode,
asynchronous readiness, encrypted key readiness, arbitrary teardown races,
physical-network timing or resource scale. Native network timing is real;
barriers make the observed transition ordering controlled, not the whole network
deterministic.

## Expanded admission matrix

The subsequent native matrix passes all 12 combinations: synchronous and
asynchronous connect, plaintext and AES-256 CTR/GCM, successful join and late
native join failure. See the [initial matrix log](../evidence/native-admission-matrix.log)
and [verified matrix log](../evidence/native-admission-verified.log).
The latter additionally asserts successful key-state queries and that asynchronous
connect returns while the caller remains held before runtime attachment.
Its [stderr](../evidence/native-admission-verified-stderr.log) retains kernel buffer
warnings and expected injected connection failures.

Successful cases retain the transition order above and deliver exact public-API
DATA. Failed asynchronous calls initially return a member handle; the group then
leaves CONNECTING without becoming CONNECTED. Synchronous calls return an error.
After asynchronous admission the harness switches receive back to blocking mode
for the DATA check; this does not qualify asynchronous DATA or epoll readiness.
Encrypted DATA is sent only after all connected sender members report
SNDKMSTATE=SECURED, with a bounded three-second wait.

The first matrix run observed RCVKMSTATE=UNSECURED at CONNECTED publication in
its synchronous GCM success case. The verified rerun observed SECURED in all four
encrypted success cases. This is a scheduling-dependent observation, not a
required state transition or a claim that every connection exposes the interval.
In particular, successful publication alone must not be presented as evidence of
completed group-key acknowledgement. Receive-key status can also reflect installed
handshake material; it is not a substitute for the sender acknowledgement check.
A controlled KM-loss/delay experiment remains necessary to characterize that
boundary reproducibly. No failure of eventual encrypted DATA delivery was observed.

These runs extend the initial scope to async connect and encrypted admission on
this host. They do not cover epoll notification order, deterministic KM delay,
teardown races, other operating systems or physical multicast networks. The
60 ms timing-sample assertion passed in all matrix cases. No release source was
changed; the additional assertions are in the local probe only.

## Controlled KM loss and epoll readiness

A further run passes all 16 cases: the previous 12 admission cases and four
controlled-loss cases (sync/async connect × AES-256 CTR/GCM). The
[verified stdout](../evidence/native-key-delay-verified.log) and
[stderr](../evidence/native-key-delay-verified-stderr.log) are retained, along with
an [initial observation run](../evidence/native-key-delay.log).

The listener channel's existing test send hook decodes outgoing datagrams and
suppresses runtime user-defined KMREQ packets (subtype 3), returning successful
local send completion to simulate packet loss. Handshake extensions and all
other packets continue through native UDP. The hook is installed before connect
and remains active until after publication and the pending-key checks. This is
controlled loss followed by normal protocol retry, not delayed delivery of a
saved datagram. Each verified case intercepted one KMREQ before release.

Assertions pass in all four controlled cases:

- Receiver group epoll reports no IN/OUT/ERR at the three prepublication barriers.
- After publication it reports OUT; the receiver remains CONNECTED while KMREQ
  is suppressed. The sender member reports SNDKMSTATE=SECURING.
- Both sender and receiver group report OUT while the sender key is SECURING.
- Releasing the hook allows the normal retry and acknowledgement to reach
  SNDKMSTATE=SECURED within the three-second test budget.
- After sending one payload, receiver IN is reported within three seconds; the
  public receive API returns exact bytes and draining the payload clears IN.

The receive key was also observed UNSECURED at publication in all four cases,
consistent with the earlier scheduling-dependent observation. OUT is therefore
not evidence of completed multicast group-key acknowledgement. Applications and
specification text must distinguish admission, API readiness, and acknowledged
group-key installation. These tests do not attempt DATA while the key is pending
and do not establish what that call would return or how such DATA would be queued.

Epoll checks use level-triggered group registrations and sampled state at barriers;
they do not prove the order of every internal notification, edge-triggered behavior,
member-socket notification behavior, or concurrent waiter/teardown behavior.
The control covers key availability and barrier order, not OS scheduling. Broader
key-loss/timeout policy and failure/teardown epoll behavior remain open. No new
wire fields or protocol requirements are inferred from these API observations.

## Pending-key DATA, failed join and receive/close

The lifecycle extension passes all 16 cases in the
[verified stdout](../evidence/native-lifecycle-verified.log), with
[stderr retained](../evidence/native-lifecycle-verified-stderr.log).
It extends the four controlled KM-loss cases and six late-join-failure cases;
it does not add 16 new independent scenarios.

In each of the four encrypted controlled-loss cases, a nonblocking sender accepts
a 20-byte payload (including its trailing NUL) while SNDKMSTATE is still SECURING.
Immediately permitting KMREQ retry then allows key acknowledgement and exact
receipt of that payload within the 500 ms receive budget. The subsequent ordinary
payload also passes the existing IN/exact-read/drain checks. This proves API
acceptance before acknowledgement and eventual delivery under this short loss
schedule. It does not locate the payload in a particular queue or establish
whether its first datagram was sent before key readiness. Prolonged loss, queue
pressure, deadline expiry and delivery guarantees remain untested.

All six failed joins (sync/async × plaintext/CTR256/GCM256) report group BROKEN
and level-triggered IN|OUT|ERR (13). No readiness bits are present at the held
prepublication barriers. OUT can therefore also accompany terminal error; an
application must inspect the error/state rather than treating OUT alone as
successful connection. This is an API observation, not a wire requirement.

After draining DATA in the four controlled-loss cases, a separate thread calls
blocking receive. Its future remains pending for 50 ms; closing that receiver
group then lets the future finish within one second with SRT_ERROR and
SRT_ESCLOSED (1005). This is one concurrent receive/close schedule. The harness
has no internal receive-entry barrier and does not prove every scheduler
interleaving, simultaneous close/send behavior, epoll waiter cancellation or
race freedom. The group handle is removed from cleanup ownership after closing.

The initial assertion incorrectly named error 1005 as ECONNLOST rather than
ESCLOSED; the resulting failed run is retained in
[stdout](../evidence/native-lifecycle-error-expectation.log) and
[stderr](../evidence/native-lifecycle-error-expectation-stderr.log). Correcting
that harness expectation produced the passing run. No implementation defect or
release-source modification is claimed. The
[manifest](../evidence/run-manifest.json) records all runs and probe hashes.

## One-second key loss, queue pressure and epoll close

The expanded suite passes 20/20 cases: the existing 16 plus four pressure cases
(sync/async admission × AES-256 CTR/GCM). See
[verified stdout](../evidence/native-pressure-verified.log),
[verified stderr](../evidence/native-pressure-verified-stderr.log), and the
[initial observation run](../evidence/native-pressure-initial.log).

Each new case configures listener SNDBUF to 48,000 bytes before admission, holds
runtime KMREQ as before, and submits nonblocking 1,316-byte messages. A bounded
loop stops at the first error (at most 4,096 submissions). All four runs accepted
32 messages, then returned SRT_EASYNCSND (6001), with sender group OUT cleared.
The asserted requirement is positive bounded acceptance followed by EASYNCSND
and no readiness flags; 32 is an observed capacity for this fixture, not a
protocol limit or general memory bound.

The hold then continues for one second without releasing KMREQ. All four cases
intercepted 11 requests in this run, remained CONNECTED, and retained sender
SNDKMSTATE=SECURING. Receiver group readiness remained OUT without IN. Assertions
require at least two suppressed requests; the exact retry count is not a timing
contract. This establishes a one-second loss window, not behavior under indefinite
loss or the eventual key timeout policy.

With KMREQ still suppressed and DATA undelivered, the harness starts a blocking
receive and a level-triggered epoll wait interested in IN|ERR (500 ms timeout).
Both futures are pending after the 50 ms observation interval. Closing the
receiver group yields SRT_ERROR/SRT_ESCLOSED for receive and one ERR epoll event,
with both futures completing within the subsequent one-second check. These
results are asserted in all four cases. This extends close coverage to a pending
epoll waiter with queue pressure and unacknowledged keys. Futures do not supply
internal entry barriers; arbitrary scheduler interleavings remain unqualified.

No keys are released in the pressure cases and no delivery/recovery claim is
made for their queued messages. Recovery after sustained loss, packet deadline
expiry, blocked-send cancellation, other queue sizes, edge-triggered polling,
resource-leak checks and stress across many repetitions remain open. Existing
short-loss recovery cases still pass separately. No release source was changed.

## Three-second loss: recovery and peer-idle expiry

The native suite now passes 28/28 cases: the previous 20, four sustained-loss
recovery cases, and four peer-idle-expiry cases (each sync/async × AES-256 CTR/GCM).
See [stdout](../evidence/native-recovery-verified.log) and
[stderr](../evidence/native-recovery-verified-stderr.log).

Both additions fill the same 48,000-byte configured send buffer, observe
EASYNCSND, and suppress KMREQ for three seconds. Recovery cases explicitly set
PEERIDLETIMEO=10,000 ms on listener and receiver. At release, both groups remain
CONNECTED and the sender remains SECURING. Normal KM retry and acknowledgement
reach SECURED. A 1,500 ms nonblocking drain observes 32 messages of 1,316 zero
bytes in each run, matching the count accepted before backpressure. Each payload
is checked byte-for-byte; the count is recorded and constrained not to exceed
acceptance, rather than asserting that all pressure fixtures must deliver 32.
Because these pressure messages are identical, this is not a per-message identity
or duplication proof. A distinct fresh marker is then accepted and received
exactly. This demonstrates recovery for this fixture, not a universal delivery
promise for arbitrary deadlines or prolonged outages.

Expiry cases instead set PEERIDLETIMEO=2,000 ms on both endpoints. At the end of
the three-second hold, each receiver group is BROKEN and reports IN|OUT|ERR.
Keys are not released in these cases. The initial attempt incorrectly combined
that shorter idle horizon with an expectation of survival for recovery; its
[stdout](../evidence/native-recovery-idle-initial.log) and
[stderr](../evidence/native-recovery-idle-initial-stderr.log) are retained with
exit code 1. Separating the two fixtures resolves the test expectation; no
implementation defect is inferred.

Source tracing distinguishes key retry from peer-idle expiry. The runtime retry
block computes a cadence from RTT, bounded by a peer-idle-derived ceiling, and
retries pending material; it contains no independent elapsed-KM deadline. The
runtime's idle check expires elapsed time since last peer activity. Source-map
claims MC-D01/MC-D02 pin these blocks to the immutable release. The existing
focused tests independently pass: [two retry tests](../evidence/key-retry-focused.log)
and [one last-valid-packet idle test](../evidence/peer-idle-focused.log).
These support the narrow code-path explanation, not a claim that no other
termination condition exists or that active unrelated traffic always preserves
a pending key exchange. A dedicated interoperable maximum KM-completion policy
remains an open design decision; the two-second idle fixture must not be described
as a two-second cryptographic handshake timeout.
