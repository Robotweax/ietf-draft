# Recommended disposition for the initial SRT drafts

Recommendation: make version 00 a precise description of the selected 0.3.2
profiles with explicit open issues. Do not use editorial cleanup to introduce
new clock rules, key deadlines, identifier assignments or rejection behavior.
The recommendations below are not approved protocol changes. They identify what
can be carried honestly in an initial draft and what requires another decision.

| Decision | Recommended version 00 treatment | What remains open | Consequence of choosing a stronger rule |
|---|---|---|---|
| D03 identifiers | List each namespace and deployed provisional value; state that no allocation is made | Allocation policy, collision discovery and migration | A new assignment can require versioned or dual-format migration |
| D04 congestion/resources | State current bounded tally behavior and explicitly limit deployment claims | Aggregate repair budget, feedback amplification and slow-receiver policy | Numeric mandatory limits need rationale and deployment evidence |
| D05/D15 clocks | Describe attachment-time sampling and zero-emitted, unused delta | Ingress-time capture, nonzero reception semantics and serial ambiguity | New sampling or rejection rules change behavior and require compatibility analysis |
| D06 historical repair | Describe four cache slots, positional generation and actual miss handling | Prove repair eligibility from retained send history; define missing-generation policy | Do not assert that cache capacity guarantees recovery of four prior generations |
| D06 exhaustion | State the checked unused-advance path and the distinct ordinary frontier path | Complete arithmetic/exhaustion rule and implementation assessment | A fail-closed requirement must not be claimed implemented without corresponding guards and tests |
| D07 trust | Explicitly scope multicast to a shared trusted-member domain | Whether stronger sender-only authentication or membership revocation is required | Stronger guarantees need a new security design, not rewording |
| D08 interfaces | State local override precedence and host-local scope-index meaning | A portable receiver selection policy | Do not interpret the sender's interface index as the receiver's interface |
| D09 authentication | Document actual v2 coverage and limitations; retain the independent-review gap | Transcript, replay/exhaustion, password derivation and topology review | A changed cryptographic construction requires explicit versioning |
| D10 ACP1 | Keep a separate experimental document/workstream | Full version-specific definitions and independent review | Do not fold versions 3–6 into v2 or claim complete transport authentication |
| D11 Control | Keep P03's explicit `control-v1` exception as a proposed base correction | Adopt the exception or change and retest the implementation advertisement | The exception must never be inferred solely from an absent REXMITFLG |
| D12 combinations | Publish contract restrictions and tested/unqualified distinctions | Combined multicast/authentication, ancillary extensions and profile-specific cells | Do not claim universal support or rejection from isolated feature tests |
| D13/D14 evidence | Retain exact fixture/platform limits and independent-peer gap | Post-rotation DATA vectors, lifetime boundaries, scale and interoperability | Passing local checks do not certify the entire protocol |
| D16 key deadline | Keep peer-idle expiry and pending-key retry separate | Whether an independent local/negotiated key-completion deadline is needed | A fixed duration would create a new liveness contract unsupported by the current fixtures |

**Text that can advance without changing protocol behavior**

The GCM algorithm description and field corrections, precise cipher negotiation
scope, namespace inventory, and the separate directional/multicast lifecycle
explanations can be developed as source-backed proposals. Adoption into common
base SRT still needs editorial agreement. Existing source names, reserved values
and public API enums must retain their separate meanings.

The four focused shared-schedule tests now pass in the pinned build: generation
mapping, sequence wrap, unused advancement, and failure atomicity. Two additional
focused checks pass for initial direction separation and bounded ordinary-key
overrun. These support their named transitions; they do not cover arbitrary
cache misses, 64-bit exhaustion or every combination. See the execution manifest
and claim ledger rather than treating six passing tests as a lifecycle proof.

**Remaining practical decisions**

The user confirmed Marc Cymontkowski, Robotweax GmbH, marc@robotweax.ai as the
author/contact; both drafts now carry those details. Draft names and intended
publication route still need confirmation. The current Experimental/independent metadata remains provisional.
Reference classification should be finalized with the intended scope: a descriptive
snapshot can label its source references informative, whereas a complete normative
profile must identify every dependency needed to implement it.

The automated document checks remain passed for the current XML/text files.
Visual review is still open because browser policy blocked local file previews.
No protocol decision in this table is silently treated as accepted, and no
outreach, publication or external commit is authorized.
