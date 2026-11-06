# Proposed SRT base revision clauses

These are reviewable replacement and insertion blocks, not an approved base draft.
Uppercase MUST, MUST NOT and SHOULD express the proposed requirement strength.
They do not assert IETF consensus or silently modify Robotweax SRT 0.3.2.
Targets refer to the submitted draft's numbering; supplemental source anchors
and immutable implementation authorities are recorded in the
[clause ledger](../audit/proposed-clauses.json). See also the
[baseline comparison](baseline-comparison.md) and
[decision package](decision-package-2026-11-06.md).

**P01 GCM fields and protected DATA**

Targets: sections 3.1, 3.2.2 and 6. Insert the algorithm block alongside the
existing CTR algorithm, not as a replacement for CTR. Replace contradictory
Cipher/Auth range descriptions with the suite-pair definition below. Preserve
historical reserved code descriptions separately.

> For the encrypted suites defined by this profile, the accepted Key Material
> Cipher and Authentication pairs are AES-CTR `(2, 0)` and AES-GCM `(4, 1)`.
> A receiver MUST reject an unsupported or mixed pair before installing or
> acknowledging the supplied key material. These KM values are distinct from
> the handshake Encryption Field, which identifies the advertised cipher family
> and key size; that field does not by itself select CTR versus GCM.
>
> For AES-GCM, let S be the 31-bit DATA sequence number, N the 16-byte salt,
> H the clear 16-byte DATA header and P the plaintext. The 12-byte IV is
> `N[0:12] XOR (eight zero bytes || uint32_be(S))`. The sequence word's high
> bit is zero. AAD consists of all four header words in network byte order,
> with only the retransmission bit forced to zero. The protected DATA body is
> the ciphertext followed by the complete 16-byte GCM tag.
>
> A receiver MUST authenticate the protected body and header before publishing
> plaintext into reliability ordering or application delivery. Truncated tags,
> invalid tags and inconsistent protected lengths MUST NOT publish plaintext.
> A single authentication failure does not by itself require terminating the
> connection; an authentic retransmission may recover the packet.
>
> A retransmission MUST preserve the ciphertext, tag, key selector and every
> authenticated header field except the normalized retransmission bit. The
> sender MUST NOT create different protected inputs using the same key and IV.
> Payload budgeting MUST include the full tag after accounting for network,
> SRT and any negotiated recovery-header overhead.

Compatibility: this spells out the pinned Robotweax GCM profile and corrects
incomplete supplemental descriptions. It does not add a new code point or turn
GCM on by default. The earlier host-order-AAD/CTR-style-IV preview is incompatible
and is not an alternate accepted encoding. Adoption into a common base revision
still requires coordination with other implementers. Initial DATA calculations
are independently checked for all three AES key sizes; independent post-rotation
DATA and broad cross-implementation coverage remain open. Directional/group key
lifecycle and FEC reconstruction need their own complete clauses.

**P02 Handshake acceptance and cipher selection**

Targets: sections 3.2.1 and 4.3.1. Add explicit acceptance rules without conflating
public API option numbers, HSv5 Encryption Field values and KM suite bytes.

> A successful Caller/Listener CONCLUSION response MUST contain the required
> handshake response extension. The responder validates the proposed transport
> configuration before accepting it. Acceptance is not a request for a second
> round of parameter negotiation. Where an optional controller echo is present,
> its identity MUST agree with the accepted proposal. A profile that requires
> explicit confirmation, including `control-v1`, MUST define and enforce that
> confirmation; omission is not evidence of acceptance for that profile.
>
> For encrypted operation under this GCM profile, mode selection follows the
> tables below. AUTO is a local selection policy, not an additional on-wire
> cipher suite. Incompatible selected modes MUST fail establishment rather
> than silently change to another encrypted mode. Provider unavailability or
> an incoherent transport configuration MUST fail before installing a GCM key.

| Caller selection | Listener AUTO | Listener CTR | Listener GCM |
|---|---|---|---|
| AUTO | CTR | CTR | Reject |
| CTR | CTR | CTR | Reject |
| GCM | GCM | Reject | GCM |

| Rendezvous initiator selection | Responder AUTO | Responder CTR | Responder GCM |
|---|---|---|---|
| AUTO | CTR | CTR | Reject |
| CTR | CTR | CTR | Reject |
| GCM | Reject | Reject | GCM |

These tables describe encrypted-mode selection; they do not authorize plaintext
fallback. Robotweax's optional plaintext behavior for certain ordinary bundles
must be specified separately from enforced-encryption profiles. Control/GCM and
Sensor/GCM require a secret and enforced encryption. Authentication v2 also
forbids downgrade. Do not apply the general optional behavior to those profiles.

Compatibility: preserve accepted ordinary FileCC behavior while making explicit
profiles fail closed. Exact error precedence can depend on the first incompatible
property, so the proposal does not promise one universal rejection code. The
Rendezvous table is the pinned GCM contract, not an expansion of multicast or
authentication-v2 topology support. The existing source/evidence does not validate
every cell across independent implementations.

**P03 Control REXMITFLG exception**

Target: section 3.2.1.1.1. Proposed replacement for the unconditional flag rule;
retain the surrounding base flag definitions.

> REXMITFLG MUST be set for the base profiles described here, except where an
> explicitly negotiated versioned profile defines an exception. For
> `control-v1`, both peers MUST select that exact CONFIG congestion-controller
> identity and the responder MUST explicitly confirm it. Within this profile,
> absence of REXMITFLG MUST NOT disable NAK-based recovery, retransmission-timeout
> recovery or interpretation of the retransmission bit on DATA. Control senders
> continue to mark retransmitted DATA. Absence of REXMITFLG alone MUST NOT be
> used to infer that a peer selected `control-v1`.

Compatibility: an explicit exception matches the existing Control contract and
does not relax the base rule for unnamed peers. The alternative is to change the
implementation's advertisement and retest compatibility; that is a different
proposal, not implemented here. This clause does not define the complete Control
profile: ordered Message delivery, no finite TTL, disabled TSBPD/TLPKTDROP, FileCC
pacing, profile mismatch handling and application execution acknowledgements still
belong in its dedicated specification. The general exception mechanism itself
requires editorial agreement before adoption.

**P04 Clock sampling and admission boundary**

Targets: section 4.5.1.1 and the multicast extension's clock section. This is a
clarification plus a profile-specific insertion; it is not a blanket change to
all base SRT clock initialization.

> A receive-clock definition MUST identify both the peer handshake timestamp
> used as its reference and the local event at which the corresponding local
> time is sampled. A sampled setup time and a retained packet-ingress time MUST
> NOT be described as interchangeable. The selected sample establishes the
> initial time base before subsequent drift and wrap processing.
>
> In the Robotweax 0.3.2 multicast caller path, the local sample is taken during
> runtime attachment. The initial base is this sample minus the peer handshake
> timestamp. Setup scheduling delay is consequently included. Native receive
> membership and ingress routes are installed before CONNECTED publication.
> The multicast sender emits zero timestamp-origin delta, and this receiver
> path does not apply the announced delta. Connection publication does not
> establish completion of group-key acknowledgement.

Compatibility: the first paragraph requires precise specification; the second is
an implementation description suitable for a compatibility profile. It does not
claim the source implements new normative ingress capture. Recommendation:
retain that descriptive contract for 0.3.2 and design any ingress-time mapping as
an explicit future change. Requiring zero on reception would change today's
nonzero-delta handling and is not proposed as an implemented rejection rule.
Serial wrap, admission cursor, group-origin and retention rules remain necessary
before claiming a complete independent multicast implementation.

**P05 Identifier namespaces and negotiation scope**

Targets: sections 3.2.1, 3.2 and 9. Insert a namespace table and the following
interpretation rules; allocation policy remains an open decision.

> A protocol identifier MUST be interpreted in its enclosing namespace. A
> handshake CONFIG type, a runtime user-defined subtype, a group type, a suite
> byte and an implementation's API enum are not interchangeable identifiers.
> Reuse of the same integer in two namespaces does not imply the same wire
> structure or authorization. Capability acceptance MUST follow the applicable
> extension's negotiation rules rather than being inferred from an API value
> or from the numerical value of an unrelated field.
>
> The provisional Robotweax values below document an implementation. They MUST
> NOT be represented as IANA allocations or as a private-use range assigned by
> this document. A future assignment or collision resolution requires an
> explicit migration design; it MUST NOT silently reinterpret deployed bytes.

| Namespace | Value | Current meaning |
|---|---|---|
| Handshake CONFIG | `0x7f10` | Session authentication |
| Handshake CONFIG | `0x7f11` | Path identifier |
| Handshake CONFIG | `0x7f12` | Standby RTT capability |
| Handshake CONFIG | `0x7f13` | Implementation identity |
| Handshake CONFIG | `0x7f14` | Multicast dispatch |
| Runtime user-defined subtype | `0x7f03` | Authenticated KMREQ |
| Runtime user-defined subtype | `0x7f04` | Authenticated KMRSP |
| Runtime user-defined subtype | `0x7f05` | Experimental ACP1 control envelope |
| Runtime user-defined subtype | `0x7f12` | Standby RTT probe |
| Group type | `4` | Multicast reservation in the base draft; not an allocation of the dispatch CONFIG record |
| CONFIG congestion-controller extension | Type `6`, string `control-v1` | Versioned Control identity; not an added extension type number |

Compatibility: this inventory changes no wire value. It makes existing reuse
explicit and leaves allocation/collision handling unresolved. Unknown extension
handling cannot be reduced to one universal ignore/reject rule: optional identity,
required protected profiles and malformed encodings have different contracts.
Their individual specifications must state those rules. API state values and
crypto-option numbers are deliberately not assigned protocol meanings here.

**Review and integration plan**

1. Review P01/P02 with the base-draft editors and cipher implementers before
   adopting the GCM contract as common base text. Local outreach is not performed.
2. Decide whether to adopt P03's explicit Control exception or change the
   implementation advertisement. Preserve this choice in the decision register.
3. Keep P04's implementation description distinct from any future clock-policy
   change. Decide zero-delta reception and key-completion policy separately.
4. Review P05 with a namespace/collision proposal and extension-specific
   unknown-record rules. Naming numbers does not allocate them.
5. Integrate approved text into a complete base revision, add missing lifecycle
   and combination clauses, and run document/submission checks. These five blocks
   alone are not a submission-ready SRT replacement.

**P06 Extension combinations**

The [combination matrix](extension-combination-matrix.md) separates contract
restrictions from local executed coverage and unresolved combinations. Apply all
relevant topology, delivery, cipher, authentication and filter constraints; no
passing feature implies support for its entire cross-product.

**P07 Multicast key lifecycle**

The [lifecycle proposal](proposed-key-lifecycle.md) records positional generation,
initial confirmation versus later rotation, cache behavior, idle advancement,
retransmission and membership trust. Serial ambiguity, position exhaustion and
historical repair eligibility remain explicit decisions.

**P08 Ordinary directional key lifecycle**

Target: section 6.1.6, with startup clarification in 6.1.5. This proposed block
applies to the Robotweax ordinary confirmed encrypted-data path, not shared
multicast schedules or optional-encryption modes that intentionally continue
without a decrypting peer.

> Transmit and receive directions maintain separate key state. After the initial
> encrypted handshake, each endpoint prepares independently generated transmit
> material and waits for its matching runtime key confirmation before releasing
> new DATA. Retained handshake material for legacy receive compatibility MUST
> NOT be cloned into the endpoint's opposite transmit direction. Local queue
> acceptance and CONNECTED publication do not constitute that confirmation.
>
> The sender preannounces a successor key and retries its KM request until the
> matching acknowledgement is accepted. Ordinary successor activation is
> acknowledgement-gated. If confirmation is absent at the refresh boundary,
> the current key may continue only within the implementation's bounded overrun
> and per-key sequence budget; afterwards new DATA pauses. Already protected
> retransmissions retain their original key selector and protected bytes.
>
> For the audited readiness predicate, let R be the effective refresh interval,
> C the positions already consumed on the active key and L the per-key sequence
> limit. After initial confirmation, a prepared but unacknowledged rotation
> permits a new DATA operation only while `C + 1 < min(2 * R, L)` once the
> ordinary refresh threshold is reached. This is a strict boundary, not a
> guarantee that exactly two refresh periods of packets will be delivered.
> A matching response can enable the successor; an acknowledged old body MUST
> NOT acknowledge different pending material. Peer-idle expiry remains separate
> from this position-based rotation bound.

Compatibility: this documents the pinned direction-separation behavior and its
startup exchange. It does not claim every other SRT implementation uses the same
local scheduling policy. Packet retirement/sequence skips, adaptive preannouncement,
receive-key retention and optional fallback paths still need complete treatment
before this becomes a complete base-key specification. Multicast instead uses
P07's shared positional schedule and later switches without waiting for every
member; Broadcast/Backup do not inherit that multicast rule.

Focused source tests for distinct initial keys and bounded overrun have passed.
They are implementation tests, not independently calculated post-rotation DATA
vectors or a proof of every possible lifetime transition.
