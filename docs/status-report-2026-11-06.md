# Robotweax SRT IETF draft status report

Status date: 6 November 2026

The local project has two unsubmitted working drafts and a substantial, reproducible evidence base for selected Robotweax SRT 0.3.2 functionality. Both drafts have now been consolidated, and a concrete decision package, six proposed base replacement clauses, a combination matrix and a key-lifecycle proposal are ready for protocol-design review. It is not yet a complete revision of the public SRT specification, a submission-ready package, or an independently interoperable protocol specification.

The current continuation is local and uncommitted. External commits, pushes, outreach and IETF submission require explicit user instruction. No release implementation changes were made during this audit; native probes use a separate instrumented source export.

**Scope and deliverables**

The implementation baseline is tag `v0.3.2`, commit `235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1`. The recorded public specification baseline is `draft-sharabayko-srt-01`; the supplemental Haivision source is now pinned to commit `461a1fea85baaa4a928fa634d30c846dfd047ab8` with verified Git blob identity. This report summarizes local evidence rather than a fresh check of external publication status.

| Workstream | Current status |
|---|---|
| Multicast dispatch draft | RFCXML working draft covering dispatch, admission, repair, timing and key lifecycle, with implementation findings and unresolved policy choices |
| Session authentication draft | RFCXML working draft for version 2; ACP1 versions 3–6 remain a separate scope decision |
| Base SRT revision | All 84 numbered submitted sections/appendices mapped to the supplemental source; six priority replacement blocks prepared; remaining clauses and clause-level review are outstanding |
| Other extensions | Control/Sensor profiles, path labels, standby RTT and optional implementation identity are tracked, not fully specified by the two drafts |
| Evidence and reproducibility | Source ledger, frozen vectors, C++ adapters, native probe, retained logs and hashed execution manifest |
| Generated documents | Both drafts rendered successfully to text and HTML |

See the [scope](scope.md), [multicast draft](../drafts/draft-robotweax-srt-multicast-00.xml), [authentication draft](../drafts/draft-robotweax-srt-authentication-00.xml), and [traceability](traceability.md).

**Validation completed**

| Evidence layer | Recorded result |
|---|---|
| Latest native suite | 28/28 cases passed, covering admission, key loss, pressure, recovery, idle expiry and selected close behavior |
| Focused retry and idle tests | 3/3 passed |
| Earlier selected release regressions | Authentication 15/15; multicast 22/22; stale response 1/1; timing 7/7 |
| Crypto matrix | CTR and GCM at 128/192/256 bits; independent Python KEK, wrapping, IV and initial ciphertext/tag calculations matched all six cases |
| Serialized handshake/runtime adapter | Four accepted and two rejected cases, including sequence/source admission and injected-clock deadlines |
| Evidence integrity | 38 pinned source claims, baseline hashes and 84 section mappings verified; log, fixture and tracked harness hashes passed |
| Document validation | Structural XML, local links and idnits passed; both drafts rendered; visual layout review remains open |

These are different, partly overlapping evidence layers, not one aggregate conformance score. The native suite uses loopback multicast and an instrumented copy of the pinned implementation. It is not independent peer interoperability or physical-network qualification. Rendering reports a nonfatal Python LibreSSL/urllib3 warning; idnits 3.3.0 now reports zero findings in normal and submission modes for XML/text with online reference lookups. See [submission readiness](submission-readiness-2026-11-06.md); draft names/publication route and visual review remain open.

**Findings that affect the specification**

- Native multicast membership is installed before CONNECTED publication. Tested join failures remain unpublished and later report BROKEN with group epoll IN|OUT|ERR.
- CONNECTED and epoll OUT do not establish group-key acknowledgement. Controlled KMREQ loss leaves sender key state SECURING while both are visible.
- Nonblocking DATA can be accepted before key readiness. Short-loss recovery delivers the test payload. With a configured 48,000-byte send buffer, pressure cases accepted 32 messages before EASYNCSND and cleared sender OUT; this is a fixture result, not a protocol capacity limit.
- Three-second KM loss recovers under a ten-second peer-idle horizon. Each recovery run observed 32 matching buffered payloads and exact delivery of a distinct fresh marker. Identical pressure payloads do not prove individual message identity or absence of offsetting loss/duplication.
- The same loss with a two-second peer-idle horizon expires the receiver. Retry cadence and peer-idle expiry are distinct mechanisms; these tests do not establish a separate maximum key-completion deadline.
- Closing a receiver with receive and epoll futures pending produces ESCLOSED and an ERR event in the tested schedules. This is bounded concurrency coverage, not proof of race freedom.
- Caller timing uses a local runtime-attachment sample paired with the peer timestamp. A controlled setup pause enters that sample; it is not a retained UDP-ingress timestamp. The multicast timestamp-delta field remains unused on this path.

The [native results](native-publication-results.md), [crypto matrix results](matrix-and-handoff-results.md), and [execution manifest](../evidence/run-manifest.json) retain scope, reproduction details and unsuccessful attempts. Investigated harness expectations and sandbox failures are distinguished from implementation defects; no release defect has been established by these passes.

**Remaining decisions and readiness gates**

The [decision register](decisions.md) remains open. Highest-priority work is to resolve the clock baseline and delta semantics; multicast key lifecycle and trust assumptions; identifier allocation and collision policy; congestion/resource limits; supported extension combinations; and whether a separate pending-key deadline is needed. Evidence of current behavior does not approve new wire requirements.

Before an initial submission, confirm draft names, scope, attribution and applicable IPR notices; the author/contact is already confirmed; use the pinned supplemental baseline to reconcile contradictions and references; review IANA wording; run idnits; and inspect the final text/HTML. An initial draft can identify open issues, but the current package still needs that editorial pass.

Independent cryptographic review, an independent peer implementation, physical multicast deployment/scale testing, cross-platform/provider coverage, exhaustive replay/rollover cases and broader teardown stress remain maturity gaps. They should not be represented as completed merely because local tests pass.

**Recommended next sequence**

1. Review the [decision package](decision-package-2026-11-06.md). Both drafts now separate descriptive behavior, evidence summaries and proposed open decisions; detailed chronology remains in evidence reports.
2. Review the [proposed base clauses](proposed-base-clauses.md) for GCM, handshake acceptance, Control REXMITFLG, clock sampling and namespaces, plus ordinary directional key lifecycle, then complete remaining replacement text using the [84-section baseline map](baseline-comparison.md). Clause-level conformance is not established.
3. Review the [combination matrix](extension-combination-matrix.md) and [key-lifecycle proposal](proposed-key-lifecycle.md), especially unqualified combinations, historical repair eligibility and serial/exhaustion policy.
4. Resolve the remaining [submission-readiness gates](submission-readiness-2026-11-06.md): draft names/publication route, metadata, reference classification, protocol decisions and visual review. Automated checks now pass.
5. Prepare concrete requests for independent security and interoperability review. Any outreach or publication remains subject to explicit user instruction.

Latest decision follow-up: [version 00 disposition](initial-draft-disposition.md).
Four shared-schedule and two directional-key focused tests passed. The claim ledger
now has 38 source claims and eight proposal mappings. Author/contact details are confirmed and applied: Marc Cymontkowski, Robotweax GmbH,
marc@robotweax.ai. Draft names and publication route remain provisional.
