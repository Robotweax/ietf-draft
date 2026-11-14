# 0.3.3 editorial validation

Executed 2026-11-14 against the updated working drafts and pinned source delta.
The [validation manifest](../evidence/validation-0.3.3.json) binds current XML,
rendered TXT/HTML, the release ledger, validation tools and logs to SHA-256 hashes.
Generated renders remain excluded from Git and are reproducible with `make render`.

| Check | Result |
|---|---|
| Structural XML, local document links | Passed for all three drafts |
| Historical audit | 38 original claim anchors, evidence hashes, 84 baseline sections and eight proposal mappings still pass |
| Release delta | Tag-to-commit peeling, all 332 changed paths and 12 source-traced claims pass |
| RFCXML render | xml2rfc 3.31.0 produced text and HTML for all three drafts |
| Idnits | 3.3.0; 12 checks (three drafts × XML/TXT × normal/submission), all zero errors, warnings and comments |
| Rendered output | Text inspected; static HTML internal fragments and author/contact fields verified |
| Whitespace | `git diff --check` passed |

Reproduce source verification with:

```sh
python3 tools/check.py
python3 tools/check_audit.py --source-repo /path/to/robotweax-srt
make render XML2RFC=/path/to/xml2rfc
python3 tools/check_nits.py --idnits /path/to/idnits --online \
  --evidence-dir evidence/release-0.3.3
```

The runner now discovers all three drafts and accepts a separate evidence directory
so new runs need not overwrite historical 0.3.2 results. Online mode performs public
reference lookups, not submission. Current raw output and logs are in
[evidence/release-0.3.3](../evidence/release-0.3.3).

The first idnits normal-mode run rejected the hybrid heading “Introduction and
Scope”; changing it to “Introduction” resolved the finding. That original output
is preserved under `initial-nits`. Python emitted an environment-level urllib3 /
LibreSSL warning during rendering; rendering succeeded. The first renderer run
also warned about its default cache path; the recorded run used a writable
explicit cache. Browser visual inspection was not performed.

No 0.3.3 runtime, native-network or provider conformance suite was executed for this
editorial pass. Historical readiness/idnits manifests remain 0.3.2 snapshots;
passing these editorial checks does not establish protocol completeness,
independent interoperability or security qualification.
