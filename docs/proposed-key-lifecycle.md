# Proposed SRT multicast key lifecycle

P07 describes the pinned shared multicast schedule and proposes specification
boundaries. It does not replace ordinary directional-key rotation with multicast
rules. Source anchors are in the [clause ledger](../audit/proposed-clauses.json).
Normative adoption, identifier governance and independent security review remain
open. The algorithm below records implemented behavior; explicit proposals are
identified at the end.

**State and sequence interpretation**

The dispatch sender owns a shared salt, cipher mode, key length and schedule.
Members retain independent KEKs and KM exchanges. One mode and key length are
required across the encrypted group. Let R be the effective refresh interval,
A the preannouncement interval, F the 64-bit local frontier position and Q its
31-bit sequence number. The implementation clamps R to `[2, 2^30]` and A to at
most `floor((R - 1) / 2)`. F starts at zero at the configured sequence origin.
Generation is `g = floor(F / R)`; even/odd selector follows generation parity.
Generation numbers and F are local state, not additional wire fields.

For a candidate sequence S, compute `u = (S - Q) mod 2^31`. The signed distance
d is u when u is below `2^30`, otherwise `u - 2^31`. For d greater than zero,
advance F by d and set Q to S. Otherwise map to `max(0, F + d)` without changing
the frontier. This is the actual signed-distance mapping, including the clamp
for a position before the origin. It is not a standalone packet-admission rule;
source, sequence and reliability validation still apply. Exactly half a sequence
space is ambiguous for independent serial interpretation and is a negative
distance in this implementation. A future specification must constrain admissible
jumps rather than infer chronology from arbitrary 31-bit values.

**Generation cache and failure handling**

Initialization obtains a random salt and generation-zero key before marking the
schedule initialized. Four slots are indexed by `generation mod 4`, each carrying
a valid bit and full generation number. Replacing a slot erases its old key only
after random generation succeeds. Cache capacity is not a promise that four
consecutive historical generations have been populated.

On a requested generation, an exact valid slot is returned. On a slot miss, the
implementation rejects a generation more than one ahead of its newest generated
generation, or one for which `generation + 4 <= newest_generation`. Otherwise it
creates the requested key. This admits some missing generations within the
retained numeric window; it is more precise than saying that only the next key
can ever be generated. After an idle jump, a missing intervening generation did
not necessarily encrypt any original packet. Do not interpret generating a new
key for a previously unused position as recovering historical ciphertext.

For unused sequence advancement, the implementation requires an initialized
schedule, a positive packet count below the half-range, and a nonnegative signed
distance from the frontier. It checks addition against the 64-bit position limit,
prepares the target generation, and only then commits the new F and Q. It does
not generate every skipped key. Failure leaves that frontier uncommitted.
The ordinary `position_of` advance does not contain the same explicit 64-bit
overflow check; this proposal therefore does not claim complete position-exhaustion
handling. Arithmetic hardening and exhaustion policy remain review items.

**Member lifecycle**

| Event | Implemented transition | Specification consequence |
|---|---|---|
| Member admission | Attach only an initialized compatible schedule, before an independent transmit key has been confirmed | Do not replace an already secured member's key underneath its receiver |
| Initial announcement | Install current generation; inside the announcement window also install successor and advertise both | A late join can receive two generations; it has no backward-access guarantee |
| Initial group-key wait | Member remains unable to send group DATA until initial confirmation | CONNECTED and application queue acceptance are not key acknowledgement |
| Sequence reaches announcement window | Ensure successor, rebuild KM with both selectors and mark rotation prepared; sender state may become SECURING | SECURING after initial confirmation is different from initial-key waiting |
| Later generation boundary | Select the generation from sequence position, without waiting for every member's acknowledgement | Do not import ordinary acknowledgement-gated rotation into multicast |
| Receiver lacks generation | Under enforced encryption, unusable DATA is not delivered; repair can recover after key installation, subject to packet lifetime | No universal delivery guarantee when key exchange is later than the deadline |
| Matching KM response | Validate authenticated envelope when enabled, then semantic body acknowledgement; current pending material can become secured | Authentication-counter acceptance alone does not confirm a new key |
| Duplicate/stale response | Known acknowledged body may be idempotent; it does not acknowledge different pending material | Preserve separate replay and key-state decisions |
| Member leaves | Control/session state closes; previously learned traffic keys remain known | Leave is not cryptographic revocation |

A receiver discards the ordinary handshake-key clone as an alternate multicast
DATA key. An admitted group member has the shared traffic key and can forge
traffic authentic under it. Neither source filtering nor GCM proves sender-only
identity within that trust domain.

**Originals, repairs and rotation**

The group sequence identifies the key generation for both original multicast
DATA and unicast repair. DATA names the dispatch group in its destination field.
GCM authenticates the header with only the retransmission bit normalized; repeats
must preserve the protected representation and other authenticated fields.
Cipher/nonce derivation and key retention must be considered together: two paths
must not create different protected inputs for the same key/IV.

Ordinary non-shared sessions instead use direction-specific keys and gate a
successor switch on acknowledgement, with their documented bounded continuation
on the active key. That algorithm requires a separate base-key clause. The
multicast rule above is not an exception that can be inferred merely from an
API group handle: Broadcast and Backup keep independent member crypto.

**Retry and expiry**

Pending KM is retried using the runtime's RTT-dependent cadence. Peer-idle expiry
uses the last peer-activity time. They do not define a separate elapsed-KM deadline.
The native three-second fixtures demonstrate recovery under a ten-second idle
horizon and expiry under a two-second horizon, not a universal key-completion
bound or delivery guarantee. Queue backpressure and application timeout policies
remain separate from the wire key-generation rule.

**Proposed requirements still requiring a decision**

1. Specify an admissible serial-distance/position domain and fail-closed behavior
   at ambiguity or exhaustion. Do not label the current mapping fully overflow-safe.
2. Specify historical repair eligibility after idle jumps, including whether an
   unpopulated generation may be created and how send history proves eligibility.
3. Preserve initial-confirmation gating and explicitly document positional later
   rotation, with a clear late-key/packet-deadline outcome.
4. State the trusted-member security model, lack of leave revocation and lack of
   backward/forward membership secrecy. Stronger guarantees require a new design.
5. Decide whether a separate configurable pending-key deadline is required, with
   no duration inferred from the local fixtures.

The current evidence includes selected generation/idle-failure regressions, initial
cipher vectors, rotation exchange, and native KM-loss cases. Independent
post-rotation DATA vectors, exhaustive wrap/retention/exhaustion transitions and
an independent peer remain gaps. This lifecycle draft is review material, not a
claim that all these requirements have already been implemented or tested.
