# Open decisions and publication gates

No design choice in this register is implicitly approved by the initial drafts.

| ID | Decision / action | Completion criterion |
|---|---|---|
| D01 | Editors, author contacts, draft names and intended stream | Author/contact confirmed and applied; confirm draft names/route and coordinate continuation with existing SRT authors or use separate individual drafts |
| D02 | Base text reconciliation | Supplemental source pinned and all 84 numbered submitted sections mapped; complete replacement clauses and clause-level compatibility review |
| D03 | Identifier governance | Coordinate handshake/runtime values, group type and profile names; define registry/allocation policy and collision migration |
| D04 | Multicast resource/congestion policy | Define aggregate original/repair limits, receiver limits, feedback amplification controls, slow receiver policy and deployment scope |
| D05 | Multicast clock and rollover semantics | Specify origin-delta calculation, serial arithmetic, admission cursor, retained repair history, wrap and empty-group behavior with vectors |
| D06 | Multicast key lifecycle | Specify generation boundaries, joining keys, failed generation, late confirmation and retention; review nonce uniqueness and same-packet retransmission |
| D07 | Multicast trust model | Decide whether shared-key insider forgery and no leave revocation are acceptable; otherwise design a new version |
| D08 | Interface hints | Define host-local IPv6 interface-index interpretation; never assume sender index selects receiver interface |
| D09 | Authentication design review | Independent review of PBKDF, transcript coverage, direction separation, replay, shared-secret identity and absent forward secrecy |
| D10 | ACP1 scope | Decide whether versions 3–6 remain a separate Experimental document; do not silently replace v2 or call ACP1 supported security |
| D11 | Profile semantics | Resolve REXMITFLG discrepancy and formalize Control/Sensor rejection and delivery rules |
| D12 | Extension combinations | Explicit matrix for topology, cipher, authentication, filters and profiles, including fail-closed behavior |
| D13 | Conformance corpus | Independently calculated vectors plus malformed, duplicate, loss, reorder, replay, rollover and lifetime tests |
| D14 | Interoperability and deployment | Seek independent peer implementation; separately qualify multicast physical network and scale |
| D15 | Clock capture event | Decide attachment-time versus ingress-time semantics without misdescribing 0.3.2 |
| D16 | Pending-key deadline | Decide whether a separate key-completion policy is needed; do not equate it with peer-idle expiry |

## Initial Internet-Draft posting gates

- Author/contact confirmed; confirm names of the drafts, scope, attribution and applicable IPR notices.
- Make status and incompleteness explicit; resolve accidental contradictions.
- Render with xml2rfc; inspect text/HTML; run idnits and review warnings.
- Check references, required sections and IANA wording; submit through Datatracker.

An initial draft may contain open issues. Independent interoperability and a
completed crypto review are not administrative prerequisites for posting, but
they are maturity requirements before making broader support/security claims.
No submission or outreach is authorized by repository validation commands.

## Subsequent publication

For an Informational/Experimental independent RFC, follow the
[ISE process](https://www.rfc-editor.org/authors/ise/ise-checklist/).
For IETF standards-track work, seek an appropriate community venue and consensus;
an individual upload does not confer adoption. Follow the current
[submission instructions](https://datatracker.ietf.org/submit/tool-instructions/).

## Local audit findings — 2026-11-06

See [the claim ledger](source-audit-2026-11-06.md). D05 now explicitly includes the zero-emitted, runtime-unused timestamp delta; D04 includes per-request member RTT versus a shared expiry window; D06 distinguishes four key-cache slots from historical-generation availability. D09 must distinguish idempotent KM retries from the FEC bitmap. These are observations, not approved wire changes. No remote files were modified by this continuation.

## Timing sample decision from native probe

D15 (open): define whether the interoperable clock baseline uses native ingress
capture or the current caller setup-time sample, and specify treatment of
admission/scheduler delay. The instrumented native run confirms the current
behavior; no protocol change is approved or implemented by this observation.
See [native publication results](native-publication-results.md).

## Key readiness after admission

The native matrix distinguishes CONNECTED publication from sender group-key
acknowledgement. A scheduling-dependent GCM observation shows that publication
can precede a secured receive-key status; eventual DATA passed after sender
acknowledgement. Controlled runtime KMREQ loss now reproduces CONNECTED and group epoll OUT
while sender SNDKMSTATE remains SECURING in four encrypted cases. Level-triggered
IN follows subsequent DATA and clears when drained. A subsequent extension verifies one pending-key DATA submission followed by
short-loss recovery, BROKEN/IN|OUT|ERR after failed join, and socket-closed errors
when closing a receiver with a receive future pending. Four further cases establish nonblocking backpressure during a one-second
KM-loss window and close completion for pending receive/epoll futures. Three-second recovery and idle-expiry cases now distinguish configured
peer-idle horizons from key completion. A dedicated maximum KM-completion
policy, delivery across arbitrary deadlines and broader teardown interleavings
remain open. See the expanded
[native results](native-publication-results.md); no new normative wire requirement
is inferred from the observed scheduling interval.

## KM completion and peer-idle policy

Do not specify PEERIDLETIMEO as an independent KM-completion timeout. MC-D01/D02
and the native recovery/expiry fixtures distinguish retry cadence from expiry
since last peer activity. Whether an interoperable profile needs a separate
maximum pending-key interval requires an explicit design decision; the observed
three-second fixtures do not resolve it.

## Consolidated editorial review

The [decision package](decision-package-2026-11-06.md) presents recommendations,
alternatives and compatibility consequences. Recommendations are not approved
wire changes. The [baseline comparison](baseline-comparison.md) pins source
provenance and records section dispositions; it does not certify every clause.

## Proposed base clauses

[P01–P05](proposed-base-clauses.md) provide replacement/insertion text for GCM,
handshake acceptance, the Control REXMITFLG exception, clock sampling and namespace
interpretation. All are marked proposed, not approved. Their source anchors are
verified by the audit checker. No implementation or deployed wire rule changed.

## Combination and lifecycle review

[P06](extension-combination-matrix.md) separates supported contract families,
source-confirmed exclusions and unqualified combinations. [P07](proposed-key-lifecycle.md)
distinguishes multicast positional rotation from ordinary acknowledgement-gated
rotation and documents the cache-miss rule precisely. D06 still needs historical
repair eligibility and complete serial/exhaustion policy; no new rejection rule
or wire change is approved by these descriptions.

## Submission readiness pass

The [readiness report](submission-readiness-2026-11-06.md) records passing normal
and submission-mode idnits results after editorial fixes. Author/contact metadata is now confirmed. Intended route/names, rights review, reference classification and
protocol decisions remain open. Local browser policy blocked visual HTML review;
static text/HTML checks passed. No external submission is authorized.

## Recommended version 00 disposition

The [disposition table](initial-draft-disposition.md) assigns concrete initial-draft
wording and remaining decisions without adopting wire changes. P08 adds ordinary
directional key lifecycle separately from multicast. Four shared-schedule and
two directional-key focused regressions passed; their limits remain explicit.
Author/contact details were explicitly supplied and applied to both drafts:
Marc Cymontkowski, Robotweax GmbH, marc@robotweax.ai. Draft names and route remain
provisional.
