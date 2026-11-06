# Requirement traceability

All Robotweax paths below are relative to the immutable
[0.3.2 source](https://github.com/Robotweax/srt/tree/235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1).
“Reported” means recorded by that release, not rerun for this repository.

| ID | Feature / base-draft location | Implementation authority | Draft action | Evidence / gap |
|---|---|---|---|---|
| BASE-01 | HSv5 flags, section 3.2.1.1 | docs/control-profile.md | Resolve base MUST-set REXMITFLG versus Control omission | Explicit profile exception or implementation change undecided |
| BASE-02 | TSBPD, drift and RTT, sections 4.5, 4.7, 4.10 | docs/release-notes-0.3.2.md; docs/live-timing.md | Distinguish initialized RTT from valid measurement | Reported timing regressions; extract exact cases |
| BASE-03 | Drop/recovery, sections 4.6, 4.8 | docs/sender-drop-semantics.md; docs/protocol-edge-cases.md | Reconcile first-transmission deadline, ACK validation and rollover | Clause-by-clause audit pending |
| AEAD-01 | Encryption, sections 3.2.2 and 6; newer working copy | docs/aes-gcm-contract.md; compat/robotweax-0.2-aead-fixtures.json | Freeze negotiation, IV/AAD, tag, retransmission and lifetime | Existing fixture corpus; independent validation pending |
| GROUP-01 | Group membership, section 3.2.1.4 | docs/connection-groups.md | Specify Broadcast/Backup sequence, clocks, join/failover and deduplication | Per-topology evidence must be extracted |
| MC-01 | Reserved group type 4 | include/robotweax/srt/connection_group.hpp; src/connection_group.cpp | Document 60-byte dispatch record and validation | Exact field layout inspected; vectors pending |
| MC-02 | New dispatch transport | docs/connection-groups.md | Admission, group destination ID, feedback and repair | Reported lifecycle, epoll and media tests; independent peer absent |
| MC-03 | Group encryption | docs/encryption.md | Positional shared keys, late joins, wrap and receiver-free progression | Reported CTR/GCM tests; no sender identity or leave revocation |
| MC-04 | Congestion and deployment | docs/multicast-media.md | Aggregate repair budget, heterogeneous receivers, network qualification | Per-member cap exists; aggregate policy and physical fan-out unresolved |
| AUTH-01 | Session authentication v2 | docs/session-authentication.md; include/robotweax/srt/session_authentication.hpp | Specify 0x7f10 negotiation, proofs, counters and parity | Existing primitive/provider tests; independent crypto review absent |
| AUTH-02 | ACP1 versions 3–6 | docs/experimental-acp1.md | Separate experimental envelope and topology binding specification | Reported Python vector/tamper/replay tests; review outstanding |
| PROFILE-01 | Control delivery | docs/control-profile.md | Specify control-v1, FileCC, ordered delivery and no TTL | No application execution/exactly-once guarantee |
| PROFILE-02 | Sensor delivery | docs/packet-filter.md | Specify fec-sensor-v1, FEC-only samples and bounded gap retirement | Lossy delivery; not a reliable command channel |
| EXT-01 | Path identity 0x7f11 | docs/group-path-identifiers.md | Encoding, bilateral admission and member lifecycle | Labels are not authorization |
| EXT-02 | Standby RTT 0x7f12 | docs/standby-rtt-foundation.md | Distinct handshake/runtime namespaces, request/echo and budgets | Correlation is not authentication or DATA health |
| EXT-03 | Identity 0x7f13 | docs/implementation-identity.md | Optional disclosure, unknown records, collision handling | Provisional number; independent implementer feedback pending |
| REC-01 | ARQ-first/tail probes, sections 4.8 and 5 | docs/arq-first-fec.md; docs/socket-options.md | Optional sender algorithm, pacing and cold-start limits | Reported measured-RTT UDP tests; no general deadline/bandwidth claim |
| EXP-01 | Compiled controllers and filters | docs/experimental-controllers.md; docs/experimental-filters.md | Specify selected named wire algorithms only if promoted | Private provider API is out of scope |

Next evidence pass: map each row to exact test names and immutable result
artifacts, record platform/topology/cipher, then add captured wire vectors.
Missing evidence remains missing; a passing same-code peer pair is not a second
independent implementation.
