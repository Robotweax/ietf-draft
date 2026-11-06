# Clock and KM integration follow-up — 2026-11-06

This is a local source audit, not a test-execution report. It extends the initial 24-claim ledger to 31 claims.

| ID | Confirmed behavior | Source | Remaining boundary |
|---|---|---|---|
| MC-B01 | Group origin is initialized once from a local fallback; accepted members reuse the admitted group origin. | [src/compat/connection.cpp:66](https://github.com/Robotweax/srt/blob/235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1/src/compat/connection.cpp#L66) | Full peer handshake timestamp emission and delayed-admission path still need targeted review. |
| MC-B02 | Runtime Live setup passes handshake arrival and peer timestamp, then shares the receive group clock. | [src/compat/transport_runtime.cpp:1593](https://github.com/Robotweax/srt/blob/235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1/src/compat/transport_runtime.cpp#L1593) | Cross-host late-join timing needs a dedicated deterministic test, not only successful eventual delivery. |
| MC-B03 | Initial TSBPD base is handshake-arrival microseconds minus peer handshake timestamp. | [src/timing.cpp:356](https://github.com/Robotweax/srt/blob/235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1/src/timing.cpp#L356) | Generic timing unit test is not multicast late-join qualification. |
| MC-B04 | Caller initializes inbound sequence from multicast announcement before installing runtime. | [src/compat/connection.cpp:1884](https://github.com/Robotweax/srt/blob/235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1/src/compat/connection.cpp#L1884) | Inspect packet arrival between announcement generation and completed join; delivery of intervening stream history is not guaranteed. |
| AUTH-B01 | Runtime authenticates private KM envelopes, rewrites subtype to ordinary KM, and rejects bare KM in protected sessions. | [src/compat/transport_runtime.cpp:4012](https://github.com/Robotweax/srt/blob/235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1/src/compat/transport_runtime.cpp#L4012) | Authentication-layer counters may commit before downstream semantic admission. Define rejection and retransmission behavior at both layers. |
| AUTH-B02 | KMRSP semantic processing accepts acknowledged-history bodies idempotently; otherwise requires exact pending KM bytes before marking secured. | [src/crypto.cpp:1381](https://github.com/Robotweax/srt/blob/235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1/src/crypto.cpp#L1381) | Bounded history success is not new pending-key acknowledgement; do not describe all successful returns as rotation advancement. |
| AUTH-B03 | Runtime forwards opened KMRSP to acknowledge_key_material with cloning disabled. | [src/compat/transport_runtime.cpp:4137](https://github.com/Robotweax/srt/blob/235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1/src/compat/transport_runtime.cpp#L4137) | Full runtime counter exhaustion/failure teardown and all optional-encryption paths remain outside this focused pass. |

## Interpretation

The receive clock is initialized through the ordinary handshake timing path, then connected to group receive-clock state. This explains why a separate multicast timestamp delta is not applied; it does not prove that every delayed-admission or wrap scenario is correct. The multicast late-join test explicitly sleeps 50 ms (plain) or 200 ms (encrypted) before sending the next batch, so it should not be cited as evidence of immediate join readiness or exact first-packet deadlines. See `tests/test_multicast_dispatch.cpp:589`.

Authenticated KM processing has two layers. The envelope checks direction, MAC and counters before exposing the body. Semantic processing then accepts a bounded already-acknowledged body idempotently, or checks exact equality with the pending body before marking that pending exchange acknowledged. The stale-response test exercises the crypto-session semantic layer; it is not a full authenticated-network replay test.

## Independent vector scope

The new vector generator uses Python standard-library struct, hashlib and hmac, without loading Robotweax code. Frozen fixtures cover IPv4/IPv6 dispatch records, a version-2 salt/key, caller/listener proofs, KM request/response envelopes and a parity MAC. An OpenSSL CLI cross-check independently invoked PBKDF2 and all five HMAC inputs and matched the outputs. Python and CLI may share cryptographic-library ancestry, so this is a separate invocation/serialization check, not independent cryptographic implementation assurance.

The KM body is intentionally synthetic and not a valid SRT key-material transaction. These fixtures check cryptographic byte construction, not KM semantic acceptance, wrapped-key correctness, DATA encryption or interoperability. IPv6 index 7 is an illustrative local index; it has no cross-host meaning. No C++ conformance comparison was run.

## Next concrete work

Add a small adapter that feeds these frozen vectors into the pinned C++ codec/authentication implementation, then add a valid wrapped-KM transaction fixture and full retry/replay sequences. Complete deterministic delayed-admission/late-join clock cases and document the pre-join packet boundary. Keep the zero-only delta proposal and any other wire changes pending explicit protocol decisions.
