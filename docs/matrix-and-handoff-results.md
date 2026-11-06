# Crypto matrix and deterministic handoff results

Local, uncommitted continuation against SRT commit
`235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1`.

## Executed checks

| Coverage | Result |
|---|---|
| CTR: 128/192/256-bit | All three valid KM, DATA round-trip, authenticated rotation and stale acknowledged-body checks passed |
| GCM: 128/192/256-bit | All three valid KM, DATA round-trip, tag-tamper rejection, retransmission-bit normalization, authenticated rotation and stale-body checks passed |
| Independent calculations | All six KEK, wrap/unwrap, IV and ciphertext/tag results matched Python cryptography |
| Serialized handshake to runtime | Four accepted cases: old/wrapping sender timestamp × normal/delayed arrival |
| Rejection | Two cases: refused membership and inconsistent announced group ID |
| Receive admission and timing | Pre-join sequence and foreign source rejected; valid sequence delivered at its calculated deadline, not one microsecond before |
| Group receive clock | Actual ConnectionRuntime installs shared receive-clock state |

The handshake adapter serializes/decodes messages, runs real caller/listener
HandshakeMachine objects, checks repeated CONCLUSION announcement stability, and
passes their confirmed announcement and peer timestamp into ConnectionRuntime.
It uses an injected clock with no sleeps or network timing dependencies.
The runtime input assembly is a deliberate test seam modeled on attach_runtime;
it does not invoke the production connection coordinator, admission callback
scheduler, native multicast membership join, or CONNECTED publication path.
Therefore this is not yet the full deterministic public-API admission test
identified in the roadmap. The existing real-UDP multicast regressions remain
complementary evidence, not a replacement for those missing assertions.

## Reproduce and inspect

Use [the adapter instructions](../conformance/README.md), then capture the new
adapter output as `evidence/adapter-matrix.log`. Run:

```sh
python3 conformance/check_crypto_matrix.py
```

That Python environment needs cryptography (tested with 41.0.3). The matrix
fixture includes public deterministic password-derived KEKs and SEKs solely for
reproduction. CTR has no wire authentication tag; its fixture tag is null.
`--write` intentionally regenerates the frozen fixture and should never be used
to conceal an unexplained mismatch.

See [the matrix fixture](../vectors/crypto-matrix.json),
[adapter log](../evidence/adapter-matrix.log) and
[hashed execution manifest](../evidence/run-manifest.json).

## Findings and remaining decisions

No implementation defect was established by this pass. Arrival delay shifts the
initial receiver time base as the current algorithm specifies; this is not proof
that all scheduling delays are correctly accounted for by the native coordinator.
The multicast timestamp-delta field remains unused by this path. Next, test the
production coordinator's timestamp-capture and publish ordering deterministically
before asserting full admission/clock handoff. Decide zero-only delta semantics
and pre-join delivery policy separately from these implementation observations.

The six-mode fixture proves the initial DATA packet in each mode/key size and
KM rotation exchange. It does not independently recompute post-rotation DATA or
exhaustive replay/counter boundaries. Independent peer interoperability,
cryptographic review and physical-network qualification remain open.
