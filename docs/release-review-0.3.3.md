# Robotweax SRT 0.3.3: IETF draft impact review

Reviewed 2026-11-14. The [published release](https://github.com/Robotweax/srt/releases/tag/v0.3.3)
adds fixed-source/coordinated multicast, separately gated hybrid control and
receive-key/lifecycle hardening. It is a substantive protocol-documentation delta,
not a version-number substitution. This pass updates unsubmitted working text;
it does not adopt new normative requirements or submit an Internet-Draft.

## Source identity and review scope

| Baseline | Annotated tag object | Peeled release commit |
|---|---|---|
| 0.3.2 | `235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1` | `e45d0edd346e5c427d85e4c3b1ac605c81a23397` |
| 0.3.3 | `29c741d35f89a412926c374d6ccb15fa1270833c` | `69bd5328bf654ebbed894c77fd30ae5b2dfba9d7` |

The 0.3.3 GitHub release was published at 2026-11-14 07:29:44 UTC, neither draft
nor prerelease. The remote tag object and its target match local Git objects.
Earlier audit ledgers called the 0.3.2 tag object a commit. Git peels it for file
access, so the prior hashes remain valid; historical ledgers and logs are preserved.
New text uses the actual 0.3.3 commit. The source archive product commit is another
identity and is not substituted for the final release commit.

[Release metadata](../evidence/release-0.3.3-metadata.json) and the
[delta ledger](../audit/release-0.3.3.json) record 332 changed paths, 12 source-traced
impact claims, file hashes, anchors and destinations. The inventory is complete for
the release-to-release Git diff. Its broad path categories are triage, not a claim
of line-by-line security review of every changed file. Focused review covered
release contracts, source/dispatch codecs and handshake negotiation, receive-key
selection, authentication version gates, repair budgets and hybrid wire/state
contracts. The official SRT draft and supplemental baseline remain pinned as in
[scope](scope.md); this pass does not rebase them or claim new IETF allocations.

## Changes and editorial disposition

| Release change | Draft action | Compatibility and remaining boundary |
|---|---|---|
| Fixed multicast source | Add exact 32-byte CONFIG `0x7f15` layout, bilateral opt-in, context validation and source routing to multicast draft | Existing 60-byte `0x7f14` stays unchanged. One-sided selection fails. Native C API IPv4 only; IPv6 engine encoding is not deployment qualification |
| Coordinated sender | Add sender-owned emission/history, queue/member bounds, ACK eligibility, stale-ISN rejection and detachment semantics | Uses independent-source wire negotiation; sender-local option. Filters, fragmented messages and session-authentication profiles excluded |
| Ordinary repair budget | Replace the stale assertion that there is no aggregate cap | Positive group MAXBW caps unicast repeats at 4×, SRT bytes excluding IP/UDP; burst max(1500 bytes, 100 ms), initially empty. Nonpositive group rates stay uncapped |
| Coordinated budget | Describe separate wire-byte accounting | 4× original wire rate, one IP-MTU burst initially full; includes IP/UDP and canonical unicast originals/repairs. Controls/KM excluded. No fairness or congestion-safety conclusion |
| Receive-key generation routing | Add accepted-position, companion-generation and half-range retirement rules | No new DATA/KM wire format. GCM tries eligible alternatives after authentication failure; CTR retains an ambiguity policy. Current keys are not expired by the retired-key lifetime rule |
| Failed authenticated open | Record preserved encrypted input and no published unauthenticated output | Local failure-atomicity change; no changed cipher, IV, tag length or authentication negotiation |
| Hybrid exchange/control | Add separate `draft-robotweax-srt-hybrid-control-00.xml`; scope it in both existing drafts | Both build gates default-off. v7 is a component context, rejected by native handshake. Separate runtime exchange `0x7f17`; inner recovery `0x7f16` inside ACP1 `0x7f05` |
| Hybrid fallback and return | Describe pairwise credentials, bounded policy/replay handling, committed overlap and live catch-up; provisioned startup waits for confirmed unicast policy before CONNECTED and joins multicast afterward | Sample may discard unread media; no lossless/decoder-continuity promise. No public hybrid C API |
| Listener close, profile revalidation, route retirement, epoll alignment and Windows timer period | Record as implementation/lifecycle corrections, not invented protocol fields | Timer policy is not a scheduling or latency guarantee. Route identity strengthens pre-session admission checks |
| Packaging/API | Retain project/ABI/API distinction | 0.3.3 / ABI 0.3 / API 1.5.7; release reports 85 retained C exports. Matching headers/library needed for new options |

The updated [multicast draft](../drafts/draft-robotweax-srt-multicast-00.xml) and
[authentication draft](../drafts/draft-robotweax-srt-authentication-00.xml) keep
version-specific observations explicit. The new
[hybrid draft](../drafts/draft-robotweax-srt-hybrid-control-00.xml) is an editorial
inventory with explicit remaining derivation/state-machine gaps, not a complete
interoperable specification. All filenames stay `-00` because these drafts have
not been submitted; product releases and Internet-Draft revisions are independent.

## Security and conformance boundaries

- No announcement-authentication claim: ordinary v2–v6 proofs do not bind the
  dispatch/source records. Hybrid binds that context after join/filter setup;
  it does not retroactively authenticate initial admission.
- Shared DATA keys still allow insider forgery, lack departure revocation and do
  not provide sender-only identity. Pairwise control credentials do not change
  those DATA properties. Hybrid/ACP1 remain outside supported security claims.
- The release's security delta reviews its named earlier source and references
  final qualification separately. Neither that report nor this editorial pass is
  independent whole-protocol cryptographic review.
- Historical 0.3.2 runtime/vector successes, 28 instrumented native scenarios and
  idnits results are retained as 0.3.2 evidence. No runtime, network, provider or
  conformance suite was rerun against 0.3.3 here. The release reports final CI,
  signing and package qualification; this review did not download/requalify them.
- Independent post-rotation DATA vectors, complete hybrid state machines,
  interoperability, physical multicast/IGMP, fan-out, feedback/congestion analysis
  and long-horizon compatible KM replay remain open. Release acceptance is not
  evidence that these draft gates have closed.

## Next decisions

1. Review the new fixed-source and budget descriptions against the intended
   interoperable profile, especially different byte accounting and burst rules.
2. Decide which hybrid semantics belong in a future independently implementable
   specification; complete per-kind validity, transcript, expiry/retry, policy and
   credential-lifetime rules before declaring it complete.
3. Extend the independent corpus at named gaps: receive-generation transitions,
   missing-companion cases, retirement/wrap and new codec/state transitions.
4. Resolve identifier policy and intended publication route. Keep any stronger
   handshake authentication or membership revocation as explicit future design.

Validation results are recorded in [validation-0.3.3.md](validation-0.3.3.md).
