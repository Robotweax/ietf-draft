# SRT baseline comparison and editorial disposition

The submitted draft and supplemental working source are pinned in the
[baseline manifest](../audit/baselines/manifest.json). The supplemental source is
[Haivision commit 461a1fea](https://github.com/Haivision/srt-rfc/blob/461a1fea85baaa4a928fa634d30c846dfd047ab8/draft-sharabayko-srt.md),
whose raw bytes match Git blob `e74908bb3c8f680f42bb76926fef2f79e6c3d083`.
The source commit is dated 22 September 2025. The hosted page's January 2024 date
is not used to identify it. The submitted baseline remains
[draft-sharabayko-srt-01](https://www.ietf.org/archive/id/draft-sharabayko-srt-01.txt).
Original notices are retained in both local snapshots.

This is a complete numbered-section editorial inventory, with corresponding
supplemental headings and an action for the Robotweax revision. It is not a
completed clause-by-clause conformance certification. **Retain boundary** means
no base-wire change is proposed here; it does not assert byte-identical prose
or prove implementation compliance. Nested rows inherit the parent issue where
appropriate. The source map and traceability report carry implementation evidence.

**Material differences and reconciliation findings**

- The supplemental source adds GCM-related DATA/KM fields and expands negotiation
  and failure descriptions. Its encryption narrative remains CTR-focused, and
  the declared Cipher/Auth value ranges conflict with their added enumerations.
  The base revision should resolve these inconsistencies and supply a complete
  GCM IV/AAD/tag contract using Robotweax's independently checked fixtures as
  evidence, without claiming that Robotweax originated GCM support.
- Rendezvous cookie-contest pseudocode and its explanatory states are expanded.
  Reconcile those corrections in the base draft; they do not enlarge the current
  multicast or authentication-v2 topology scope.
- The supplemental clock description still motivates a receive-time baseline.
  The audited multicast caller samples during attachment. Make that implementation
  difference explicit and decide the future profile contract rather than treating
  the two timings as interchangeable.
- Group type reservation does not specify multicast dispatch, admission, shared
  keys or repair election. Those belong in the extension document. Authentication,
  Control/Sensor delivery and provisional identity/probe records likewise need
  explicit scope and negotiation instead of being silently folded into base SRT.

**Section inventory**

Submitted section numbers are stable references. Supplemental numbers can change
because new subheadings were introduced; source line numbers avoid assuming the
same numbering. The local [submitted text](../audit/baselines/draft-sharabayko-srt-01.txt)
and [supplemental source](../audit/baselines/haivision-461a1fea.md.txt) can be inspected
at the recorded lines. Full machine-readable rows are in
[the inventory](../audit/baselines/submitted-sections.json).

| Submitted section | Topic | Submitted / supplemental line | Disposition | Revision action |
|---|---|---|---|---|
| 1 | Introduction | 156 / 182 | Scope | Separate base SRT from opt-in extensions; preserve attribution. |
| 1.1 | Motivation | 173 / 184 | Scope | Separate base SRT from opt-in extensions; preserve attribution. |
| 1.2 | Secure Reliable Transport Protocol | 229 / 219 | Scope | Separate base SRT from opt-in extensions; preserve attribution. |
| 2 | Terms and Definitions | 298 / 270 | Clarify | Separate wire version, library version, cipher and delivery profile. |
| 3 | Packet Structure | 311 / 283 | Retain boundary | Keep base header/namespace structure; add explicit extension references. |
| 3.1 | Data Packets | 375 / 343 | Reconcile | GCM tag/header treatment; multicast destination identity and authenticated parity are extension-specific. |
| 3.2 | Control Packets | 456 / 409 | Retain boundary | Keep base header/namespace structure; add explicit extension references. |
| 3.2.1 | Handshake | 535 / 472 | Reconcile | Namespace/version negotiation and rejection; list provisional Robotweax records separately. |
| 3.2.1.1 | Handshake Extension Message | 760 / 611 | Reconcile | Resolve Control REXMITFLG exception and explicitly enumerate negotiated flags. |
| 3.2.1.1.1 | Handshake Extension Message Flags | 800 / 644 | Reconcile | Resolve Control REXMITFLG exception and explicitly enumerate negotiated flags. |
| 3.2.1.2 | Key Material Extension Message | 859 / 679 | Reconcile | GCM cipher/Auth values and failure outcomes; keep authentication envelopes separately negotiated. |
| 3.2.1.3 | Stream ID Extension Message | 875 / 716 | Retain boundary | Stream ID remains routing/application metadata, not proof of identity. |
| 3.2.1.4 | Group Membership Extension | 921 / 745 | Extend separately | Specify multicast dispatch and other group topologies without treating reserved type 4 as a complete protocol. |
| 3.2.2 | Key Material | 983 / 796 | Reconcile | GCM cipher/Auth values and failure outcomes; keep authentication envelopes separately negotiated. |
| 3.2.3 | Keep-Alive | 1181 / 952 | Retain boundary | No new base encoding proposed; specify any authenticated-control wrapper separately. |
| 3.2.4 | ACK (Acknowledgment) | 1237 / 997 | Clarify | ACK/NAK validation and per-member feedback; ACP1 coverage is a separate profile. |
| 3.2.5 | NAK (Negative Acknowledgement or Loss Report) | 1356 / 1095 | Clarify | ACK/NAK validation and per-member feedback; ACP1 coverage is a separate profile. |
| 3.2.6 | Congestion Warning | 1415 / 1148 | Retain boundary | No new base encoding proposed; specify any authenticated-control wrapper separately. |
| 3.2.7 | Shutdown | 1461 / 1189 | Retain boundary | No new base encoding proposed; specify any authenticated-control wrapper separately. |
| 3.2.8 | ACKACK (Acknowledgement of Acknowledgement) | 1498 / 1227 | Clarify | ACK/NAK validation and per-member feedback; ACP1 coverage is a separate profile. |
| 3.2.9 | Message Drop Request | 1548 / 1267 | Clarify | Map drop request semantics to Live/Control/Sensor profiles and sender deadlines. |
| 3.2.10 | Peer Error | 1629 / 1327 | Reconcile | Namespace/version negotiation and rejection; list provisional Robotweax records separately. |
| 4 | SRT Data Transmission and Control | 1669 / 1367 | Scope | Separate base state machines from negotiated extension state machines. |
| 4.1 | Stream Multiplexing | 1691 / 1378 | Extend separately | Add dispatch group routing and source admission in multicast document. |
| 4.2 | Data Transmission Modes | 1701 / 1388 | Extend separately | Control and Sensor need named delivery rules; multicast scope remains Live Message. |
| 4.2.1 | Message Mode | 1718 / 1398 | Extend separately | Control and Sensor need named delivery rules; multicast scope remains Live Message. |
| 4.2.2 | Buffer Mode | 1763 / 1426 | Extend separately | Control and Sensor need named delivery rules; multicast scope remains Live Message. |
| 4.3 | Handshake Messages | 1772 / 1434 | Reconcile | Adopt reviewed negotiation/error clarifications; keep authentication proof and multicast admission transitions separate. |
| 4.3.1 | Caller-Listener Handshake | 1965 / 1536 | Reconcile | Adopt reviewed negotiation/error clarifications; keep authentication proof and multicast admission transitions separate. |
| 4.3.1.1 | The Induction Phase | 1971 / 1542 | Reconcile | Adopt reviewed negotiation/error clarifications; keep authentication proof and multicast admission transitions separate. |
| 4.3.1.2 | The Conclusion Phase | 2049 / 1590 | Reconcile | Adopt reviewed negotiation/error clarifications; keep authentication proof and multicast admission transitions separate. |
| 4.3.2 | Rendezvous Handshake | 2094 / 1648 | Reconcile | Review corrected cookie contest and state descriptions; multicast/v2 Caller-Listener scope does not inherit Rendezvous. |
| 4.3.2.1 | Serial Handshake Flow | 2140 / 1789 | Reconcile | Review corrected cookie contest and state descriptions; multicast/v2 Caller-Listener scope does not inherit Rendezvous. |
| 4.3.2.2 | Parallel Handshake Flow | 2270 / 1876 | Reconcile | Review corrected cookie contest and state descriptions; multicast/v2 Caller-Listener scope does not inherit Rendezvous. |
| 4.4 | SRT Buffer Latency | 2434 / 1982 | Clarify | Separate negotiated delivery latency from application deadlines and peer-idle timeout. |
| 4.5 | Timestamp-Based Packet Delivery | 2469 / 2007 | Decision | Specify setup sample versus ingress capture, timestamp delta, rollover and group clock origin. |
| 4.5.1 | Packet Delivery Time | 2566 / 2082 | Decision | Specify setup sample versus ingress capture, timestamp delta, rollover and group clock origin. |
| 4.5.1.1 | TSBPD Time Base Calculation | 2610 / 2110 | Decision | Specify setup sample versus ingress capture, timestamp delta, rollover and group clock origin. |
| 4.6 | Too-Late Packet Drop | 2652 / 2143 | Decision | Specify unsent/repair retirement and late DATA behavior per profile; do not generalize pressure-test outcomes. |
| 4.7 | Drift Management | 2749 / 2208 | Clarify | Reconcile drift estimation and initialized versus measured RTT; preserve base behavior unless explicitly changed. |
| 4.8 | Acknowledgement and Lost Packet Handling | 2811 / 2261 | Extend separately | Document ARQ-first/tail probes and multicast repair election; retain baseline ACK/NAK interoperability. |
| 4.8.1 | Packet Acknowledgement (ACKs, ACKACKs) | 2844 / 2287 | Extend separately | Document ARQ-first/tail probes and multicast repair election; retain baseline ACK/NAK interoperability. |
| 4.8.2 | Packet Retransmission (NAKs) | 2894 / 2316 | Extend separately | Document ARQ-first/tail probes and multicast repair election; retain baseline ACK/NAK interoperability. |
| 4.9 | Bidirectional Transmission Queues | 2984 / 2374 | Retain boundary | No new base queue wire format; describe directional keys and failure independently. |
| 4.10 | Round-Trip Time Estimation | 2989 / 2378 | Clarify | Distinguish valid RTT samples from initial estimates, including retry and standby probes. |
| 5 | SRT Packet Pacing and Congestion Control | 3048 / 2432 | Decision | Specify aggregate multicast/repair congestion limits; per-member pacing is insufficient as a group policy. |
| 5.1 | SRT Packet Pacing and Live Congestion Control (LiveCC) | 3066 / 2449 | Decision | Specify aggregate multicast/repair congestion limits; per-member pacing is insufficient as a group policy. |
| 5.1.1 | Configuring Maximum Bandwidth | 3116 / 2489 | Decision | Specify aggregate multicast/repair congestion limits; per-member pacing is insufficient as a group policy. |
| 5.1.2 | SRT's Default LiveCC Algorithm | 3203 / 2561 | Decision | Specify aggregate multicast/repair congestion limits; per-member pacing is insufficient as a group policy. |
| 5.2 | File Transfer Congestion Control (FileCC) | 3283 / 2638 | Retain boundary | Control uses FileCC with distinct delivery semantics; audit controller compatibility before any algorithm change. |
| 5.2.1 | SRT's Default FileCC Algorithm | 3289 / 2644 | Retain boundary | Control uses FileCC with distinct delivery semantics; audit controller compatibility before any algorithm change. |
| 5.2.1.1 | Slow Start | 3336 / 2683 | Retain boundary | Control uses FileCC with distinct delivery semantics; audit controller compatibility before any algorithm change. |
| 5.2.1.2 | Congestion Avoidance | 3441 / 2773 | Retain boundary | Control uses FileCC with distinct delivery semantics; audit controller compatibility before any algorithm change. |
| 5.2.1.3 | Link Capacity and Receiving Rate Estimation | 3660 / 2960 | Retain boundary | Control uses FileCC with distinct delivery semantics; audit controller compatibility before any algorithm change. |
| 6 | Encryption | 3701 / 2993 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 6.1 | Overview | 3708 / 2999 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 6.1.1 | Encryption Scope | 3718 / 3006 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 6.1.2 | AES Counter | 3728 / 3013 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 6.1.3 | Stream Encrypting Key (SEK) | 3757 / 3032 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 6.1.4 | Key Encrypting Key (KEK) | 3772 / 3045 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 6.1.5 | Key Material Exchange | 3788 / 3057 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 6.1.6 | KM Refresh | 3827 / 3082 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 6.2 | Encryption Process | 3876 / 3113 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 6.2.1 | Generating the Stream Encrypting Key | 3878 / 3115 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 6.2.2 | Encrypting the Payload | 3906 / 3140 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 6.3 | Decryption Process | 3928 / 3156 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 6.3.1 | Restoring the Stream Encrypting Key | 3930 / 3158 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 6.3.2 | Decrypting the Payload | 3962 / 3186 | Reconcile | Complete GCM algorithm contract beyond added field codes; separately specify shared group schedule and authentication-v2 coverage. |
| 7 | Best Practices and Configuration Tips for Data Transmission via SRT | 3984 / 3202 | Scope | Update deployment examples only for supported profile combinations; separate loopback evidence from WAN/scale claims. |
| 7.1 | Live Streaming | 3986 / 3204 | Scope | Update deployment examples only for supported profile combinations; separate loopback evidence from WAN/scale claims. |
| 7.2 | File Transmission | 4045 / 3236 | Scope | Update deployment examples only for supported profile combinations; separate loopback evidence from WAN/scale claims. |
| 7.2.1 | File Transmission in Buffer Mode | 4071 / 3246 | Scope | Update deployment examples only for supported profile combinations; separate loopback evidence from WAN/scale claims. |
| 7.2.2 | File Transmission in Message Mode | 4099 / 3254 | Scope | Update deployment examples only for supported profile combinations; separate loopback evidence from WAN/scale claims. |
| 8 | Security Considerations | 4119 / 3263 | Decision | State shared-key insider, revocation, replay, downgrade and amplification limits; independent security review remains open. |
| 9 | IANA Considerations | 4176 / 3300 | Decision | Coordinate identifier ownership and collision migration; provisional values are not allocations. |
| A | Packet Sequence List Coding | 4356 / 3337 | Retain boundary | No new sequence-list encoding proposed; preserve 31-bit serial validation and extension repair behavior. |
| B | SRT Access Control | 4396 / 3368 | Retain boundary | Keep access-control text distinct from cryptographic authentication and authorization. |
| B.1 | General Syntax | 4429 / 3386 | Retain boundary | Keep access-control text distinct from cryptographic authentication and authorization. |
| B.2 | Standard Keys | 4455 / 3409 | Retain boundary | Keep access-control text distinct from cryptographic authentication and authorization. |
| B.3 | Examples | 4526 / 3448 | Retain boundary | Keep access-control text distinct from cryptographic authentication and authorization. |
| C | Changelog | 4550 / 3464 | Editorial | Preserve historical provenance; add an accurate revision changelog. |
| C.1 | Since draft-sharabayko-mops-srt-00 | 4552 / 3466 | Editorial | Preserve historical provenance; add an accurate revision changelog. |
| C.2 | Since draft-sharabayko-mops-srt-01 | 4577 / 3478 | Editorial | Preserve historical provenance; add an accurate revision changelog. |
| C.3 | Since draft-sharabayko-srt-00 | 4610 / 3489 | Editorial | Preserve historical provenance; add an accurate revision changelog. |

**Supplemental headings without a numbered submitted counterpart**

- Line 1548: The Induction Request.
- Line 1569: The Induction Response.
- Line 1592: The Conclusion Request.
- Line 1628: The Conclusion Response.
- Line 1658: Cookie Contest.
- Line 1698: The Waving State.
- Line 1740: Conclusion.
- Line 1773: Initiated.
- Line 3304: Contributors.
- Line 3325: Acknowledgments.
- Line 3499: Since draft-sharabayko-srt-01.

Front matter, authors, contributors, acknowledgments and references also require
editorial/rights review. Existing source authorship is not authorization to name
new draft editors. No IANA request, outreach or submission is performed here.

**Work remaining after this comparison**

Translate the reconciliation actions into reviewed base text and check each
changed requirement against implementation and interoperability evidence. The
inventory closes the missing-section tracking gap, not the outstanding normative
algorithm, security or compatibility decisions. See the
[decision package](decision-package-2026-11-06.md) for recommended choices.
