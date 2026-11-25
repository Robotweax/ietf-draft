# SRT protocol draft work

Editorial work for extensions implemented by Robotweax SRT 0.3.5, with earlier release evidence preserved.
These documents are **unsubmitted working drafts**, not IETF standards,
IANA allocations, or a claim of independent cryptographic review.

## Current release review

The [0.3.5 release review](docs/release-review-0.3.5.md) maps the release
to the working drafts. See the [delta ledger](audit/release-0.3.5.json)
and [editorial validation](docs/validation-0.3.5.md). The
[0.3.4 review](docs/release-review-0.3.4.md) records the previous baseline.

## Audit and publication status

This repository contains working drafts, proposals and audit evidence. Repository
commits do not submit an Internet-Draft. Further external commits, pushes, review
comments, outreach and submissions require explicit user instruction.
See the [source audit](docs/source-audit-2026-11-06.md) and
[machine-readable evidence](audit/source-map.json). Verify references with:

```sh
python3 tools/check_audit.py --source-repo /path/to/robotweax-srt
```

See also the [integration follow-up](docs/integration-audit-2026-11-06.md) and
[vector scope](vectors/README.md).

```sh
python3 tools/wire_vectors.py
# Optional separate crypto-tool invocation:
python3 tools/wire_vectors.py --openssl /path/to/openssl-3
```

The [C++ conformance adapter](conformance/README.md) and
[execution manifest](evidence/run-manifest.json) record the subsequent pinned
implementation checks. Earlier audit reports retain their historical no-test-run
status; the manifest and claim ledger now contain executed evidence.

Historical 0.3.2: [conformance results](docs/conformance-results-2026-11-06.md).

Historical 0.3.2 continuation: [crypto matrix and handoff results](docs/matrix-and-handoff-results.md).

Historical 0.3.2 native-path evidence: [publication and timestamp capture](docs/native-publication-results.md).

Historical 0.3.2 editorial pass: [pinned baseline comparison](docs/baseline-comparison.md) and
[decision package](docs/decision-package-2026-11-06.md), with
[proposed base clauses](docs/proposed-base-clauses.md),
[combination matrix](docs/extension-combination-matrix.md), and
[key-lifecycle proposal](docs/proposed-key-lifecycle.md).

Earlier reports retain their status at the time of writing; statements that work
was local/uncommitted are historical snapshots. The author details are confirmed,
while draft names, publication route and protocol decisions remain provisional.

Historical 0.3.2 validation: [submission readiness](docs/submission-readiness-2026-11-06.md).

Recommended next decisions: [version 00 disposition](docs/initial-draft-disposition.md).

## Start here

- [Scope and source baseline](docs/scope.md)
- [Requirement traceability](docs/traceability.md)
- [Open decisions and submission gates](docs/decisions.md)
- [Multicast dispatch draft](drafts/draft-robotweax-srt-multicast-00.xml)
- [Session authentication draft](drafts/draft-robotweax-srt-authentication-00.xml)
- [MPEG-TS null packet deletion draft](drafts/draft-robotweax-srt-null-packet-deletion-00.xml)
- [Experimental hybrid control draft](drafts/draft-robotweax-srt-hybrid-control-00.xml)

The drafts cover multicast, authentication, hybrid control and null packet deletion. Base-SRT
corrections and delivery/group extensions are tracked separately so experimental
features do not silently redefine ordinary SRT. Draft names remain provisional. The author/contact is Marc Cymontkowski,
Robotweax GmbH, marc@robotweax.ai, as confirmed by the user.

## Validate and render

Python 3 is sufficient for the repository structural checks:

```sh
python3 tools/check.py
```

For RFCXML schema validation and rendering, install the authoring tools in an
isolated environment and run:

```sh
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt
make render XML2RFC=.venv/bin/xml2rfc
```

Run the IETF `idnits` tool against XML and rendered text before submission.
The runner discovers all working drafts and can retain release-specific evidence:

```sh
python3 tools/check_nits.py --idnits /path/to/idnits --online \
  --evidence-dir evidence/release-0.3.5
```

The `--online` flag permits reference metadata lookups; it does not submit documents. The
structural check is not a substitute for xml2rfc or idnits. No command in this
repository submits a draft. Generated output belongs in `build/`.

## Contribution rules

Distinguish implemented behavior, proposed requirements, and unresolved design.
Use immutable source references and do not promote self-interoperability into
independent interoperability. Record protocol changes in the decision register;
do not silently change existing versioned wire formats. Do not copy upstream
draft prose without preserving applicable attribution and rights notices.
