# Initial independent wire vectors

`initial-wire-vectors.json` freezes outputs of `tools/wire_vectors.py`.
Inputs are artificial public fixtures; the embedded-NUL password is not a secret.
All multi-byte wire integers use network order. The generator does not call,
import or link Robotweax implementation code.

Coverage: two 64-byte multicast CONFIG records (4-byte extension header and
60-byte content), authentication-v2 salt/PBKDF2 output, caller/listener proof
content, KM request/response envelopes, and parity authentication input/tag.
All MAC inputs are included so another implementation can reproduce each step.

The 16-byte KM body and FEC body are synthetic opaque inputs. They are not valid
end-to-end KM or FEC transactions. A matching cryptographic result does not mean
that a transport decoder should admit either body. No AES wrap/GCM vector or
runtime replay-state sequence is supplied in this first corpus.

Run `python3 tools/wire_vectors.py` to compare independently computed outputs
with the frozen file. Use `--openssl /path/to/openssl-3` for a separate CLI
PBKDF2/HMAC cross-check. `--write` deliberately replaces the expected fixture;
use it only after reviewing intended input/format changes, never to hide a
mismatch. These are formula-level checks, not independent peer interoperability
or proof of correct behavior in the C++ implementation.

## Subsequent C++ verification

The frozen formula-level vectors now match the pinned C++ codec/authentication
implementation through `conformance/adapter.cpp`. See `evidence/run-manifest.json`
for executed evidence. The original synthetic bodies remain intentionally
synthetic. `valid-km-transaction.json` is a separate, implementation-originated
valid AES-128-CTR KM body with independent PBKDF2 input construction and RFC3394
unwrap/rewrap checks using Python cryptography 41.0.3. It is not independent peer
interoperability or a comprehensive key-rotation corpus.

## Six-case crypto matrix

`crypto-matrix.json` extends valid KM/DATA calculations to CTR and GCM with all
three AES key sizes. Independently computed Python KEK, RFC3394, IV, ciphertext
and GCM-tag values match the new pinned C++ adapter output. This matrix supersedes
the earlier statement that valid-transaction coverage is AES-128-CTR only.
Post-rotation DATA verification is not part of this matrix yet.
