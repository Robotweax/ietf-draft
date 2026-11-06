# Pinned implementation conformance results — 2026-11-06

All work remains local and uncommitted. No external writes or IETF submission.

## Results

| Check | Result |
|---|---|
| Frozen Python vectors versus pinned C++ | Exact proof, KM/FEC envelope and IPv4/IPv6 dispatch matches |
| Selected malformed/replay adapter cases | Passed reserved-byte rejection, MAC tamper rejection, identical KM retry, stale KM and repeated FEC rejection |
| Valid KM and rotation adapter | Passed wrapped key install, authenticated response, rotation and stale acknowledged-body behavior |
| Independent valid-KM check | PBKDF2-derived KEK and RFC3394 unwrap/rewrap recover the expected deterministic SEK |
| Deterministic timing components | Passed old sender origins, deadline readiness, delayed arrival and 32-bit timestamp rollover |
| Upstream authentication filter | 15/15 passed with local UDP access |
| Upstream multicast filter | 22/22 passed, no skips |
| Upstream stale response regression | 1/1 passed |
| Upstream timing filter | 7/7 passed |
| CMake adapter recipe | Configured, built and ran; output identical to directly linked adapter |

See [the execution manifest](../evidence/run-manifest.json) for log hashes and
platform/compiler details, and [reproduction instructions](../conformance/README.md).
Both builds used immutable SRT commit
`235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1`, OpenSSL, Release and AES-GCM enabled.
ACP1 remained off. The new adapter is separate from the unchanged source export.

## Failures investigated

Two authentication tests failed at `srt_bind` in the sandbox. The original log
is retained; a rerun with local UDP permission passed all 15. The first adapter
attempt requested rotation before reaching its announcement position; the test
was corrected to consume two positions with refresh=3/preannouncement=1. This
was a harness error, not an implementation defect. OpenSSL's `enc` command did
not support the attempted wrapping mode, so the independent wrap cross-check
uses Python cryptography 41.0.3 instead. No unsupported CLI result is counted as
successful evidence.

## What remains unproven

The timing adapter is component-level, not a deterministic complete multicast
admission exercise. Existing loopback late-join cases wait before the next batch;
there is still no immediate-join deadline guarantee. Physical network fan-out,
heterogeneous receiver behavior, cross-provider and cross-platform matrices,
independent peers, full crypto review and exhaustive negative/replay coverage
remain open. The valid transaction currently covers AES-128-CTR only; the generic
regression profile's GCM coverage is separate evidence.

## Next work

Add deterministic full admission/clock handoff tests and extend the transaction
corpus to all key sizes and GCM. Then translate proven state transitions into
normative draft text, keeping unresolved wire-policy choices explicit.
