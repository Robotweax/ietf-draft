# SRT draft decision package

The recommendations below are prepared for review, not approved protocol changes.
They preserve the distinction between documenting Robotweax SRT 0.3.2 and designing
a future interoperable profile. The [baseline comparison](baseline-comparison.md)
and [native evidence](native-publication-results.md) provide the supporting context.

| Decision | Recommended choice | Alternative and consequence | Effect on compatibility |
|---|---|---|---|
| Document structure | Keep base corrections, multicast, authentication v2 and other delivery/group extensions separately scoped. | One combined replacement is possible, but requires completing all extension definitions and coordinating the base text at once. | Existing SRT remains the baseline; opt-in profiles stay explicit. |
| Clock initialization and delta | Describe 0.3.2's attachment-time sample and emitted zero delta accurately. Leave a future ingress-capture rule explicit and unresolved. | Require ingress capture or reject nonzero delta now; that would describe behavior the current audited path does not implement. | A changed receive rule needs compatibility analysis and potentially a new negotiated profile. No silent decoder change. |
| Key lifecycle and trust | Specify the current positional shared-key design, trusted-member threat model and lack of leave revocation. Complete retention/rollover rules before conformance claims. | Add sender-only authentication or revocation; this is a new security design, not wording cleanup. | Stronger membership isolation likely requires new negotiation and wire behavior. |
| Key completion deadline | Keep peer-idle expiry separate from pending-key completion. Describe a separately configurable local policy as an open proposal, without choosing a universal duration. | A fixed protocol deadline simplifies expectations but can break high-RTT/loss deployments and is not justified by the current fixtures. | A local timeout can affect liveness without changing bytes; a peer-visible guarantee requires shared semantics and new evidence. |
| Identifiers | Inventory each handshake/runtime namespace and provisional value; coordinate assignments and define collision migration before recommending broad deployment. | Treat current numbers as permanently allocated; unsupported by the evidence. | Migration may require explicit dual-version handling; never reinterpret an existing value silently. |
| Congestion and resources | Require a concrete aggregate original/repair budget and bounded feedback/receiver policy in the proposed multicast profile. Leave numerical defaults open until measured. | Rely only on member limits; this does not specify aggregate behavior. | May restrict operational scope and pacing; it need not change dispatch bytes, but peer expectations must be reviewed. |
| Authentication v2 and ACP1 | Document v2's actual coverage; keep ACP1 in a separate experimental document and commission independent review of both. | Silently strengthen v2 or describe ACP1 as full transport security; neither matches the current contract. | Cryptographic transcript/KDF changes require an explicit new version. |
| Supported combinations | Publish an explicit topology × delivery × cipher × authentication × filter matrix, with unsupported and unverified cells distinguished. | Infer combinations from isolated passing features; that would overstate support. | Reject unsupported profiles explicitly; verification status alone does not determine runtime rejection. |

**Concrete editorial changes prepared**

The multicast draft now has a dedicated admission/clock section, separates key
confirmation from CONNECTED and queue acceptance, and distinguishes peer-idle
expiry from KM retry. Both drafts have short evidence summaries instead of a
chronological test diary. Authentication open issues acknowledge completed
formula-level vectors, while retaining the gap to an independent complete peer.
No new wire identifier, algorithm, timeout or rejection rule has been adopted.

**Proposed combination matrix starting point**

| Area | Current evidence | Treatment in next revision |
|---|---|---|
| Multicast Live Message Caller/Listener | Native plaintext and AES-256 CTR/GCM sync/async cases; separate crypto matrix at all AES key sizes | Scope as implemented; distinguish native AES-256 evidence from component-level other sizes |
| Multicast with File/Stream, Rendezvous or packet-filter FEC | Excluded by current multicast scope | State exclusion and pin exact rejection paths before claiming exhaustive rejection coverage |
| Authentication v2 Caller/Listener | Formula vectors, selected release regressions and KM/replay cases | Specify v2 separately; do not equate this with a complete multicast-plus-authentication qualification |
| ACP1 topologies | Version-specific documentation; ACP1 disabled in the recorded suite | Separate experimental work; no completed security/interoperability claim |
| Control/Sensor and ancillary extensions | Traceability exists; two drafts do not specify them fully | Add dedicated contracts and combination/rejection evidence before inclusion |

**Recommended next execution**

Five [proposed replacement clauses](proposed-base-clauses.md) now address the most
consequential discrepancies: GCM field ranges/algorithm coverage, handshake
negotiation, Control REXMITFLG, clock sampling and identifier namespaces. Their [source ledger](../audit/proposed-clauses.json) pins the authorities; each
clause identifies its compatibility effect and unresolved choice. These clauses
remain proposals awaiting review. Do not defer this editorial work to another broad test expansion.

The [combination matrix](extension-combination-matrix.md) and
[key-lifecycle proposal](proposed-key-lifecycle.md) are now prepared. Review their
explicit unknowns and algorithm boundaries, using targeted tests only where a
proposed clause requires new evidence. Finally perform idnits,
metadata/reference review and rendered-document inspection. The author/contact is confirmed as Marc Cymontkowski, Robotweax GmbH,
marc@robotweax.ai. Draft names and publication route still need confirmation
before any submission. No external outreach or publication is authorized by this package.
