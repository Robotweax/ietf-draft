# Local audit validation — 2026-11-06

All work in this continuation is local and uncommitted. No external write, push,
comment, outreach or IETF submission was performed.

## Checks completed

- Structural XML and local Markdown-link checks passed.
- Evidence verifier passed for 24 claims, exact source anchors, file SHA-256
  hashes and named test definitions against immutable SRT commit
  `235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1`.
- Both updated drafts rendered to text and HTML with xml2rfc 3.31.0, exit 0.
  The local Python environment emits a LibreSSL/urllib3 compatibility warning;
  rendering succeeded with a writable temporary reference cache.
- Full src/include symbol search for timestamp_origin_delta_microseconds found
  the declaration, decoder, encoder and zero-emitting announcement construction;
  no runtime consumer of the delta was found.

## Evidence limits

No C++ tests were compiled or executed. Source-inspected tests are not pass
results. No independent vectors, complete protocol conformance audit, idnits
run, HTML visual inspection, crypto review or network-scale test is claimed.
Remaining integration boundaries are listed in the source-audit report.

## Local artifacts

- `docs/source-audit-2026-11-06.md`: readable findings and claim ledger.
- `audit/source-map.json`: machine-readable immutable evidence.
- `tools/check_audit.py`: offline verification against an existing SRT Git repo.
- `drafts/`: revised RFCXML sources.
- `build/`: generated text/HTML, untracked by design.
- `audit/editorial-changes.patch`: differences from the initial local source
  seed for README, decisions and the two draft sources. New audit artifacts are
  not included in that patch and must be retained separately.

The local folder is an authoring copy inside the existing workspace, not a
new Git checkout. It preserves the original repository's unrelated changes.

## Integration and vector continuation

The evidence ledger now contains 31 verified claim anchors. Two multicast
records and five authentication cases were independently constructed with
Python standard-library byte packing and cryptographic primitives. Frozen
outputs matched regeneration. OpenSSL 3 CLI PBKDF2 and all five HMAC results
matched the Python outputs. See the integration follow-up for scope limits;
the KM/FEC bodies are synthetic, not complete protocol transactions.

Both updated RFCXML sources were rendered again after the integration additions.
No C++ protocol test or C++ vector adapter was run. This continuation also made
no Git commits or external writes.
