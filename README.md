# SRT protocol draft work

Initial editorial work for extensions implemented by Robotweax SRT 0.3.2.
These documents are **unsubmitted working drafts**, not IETF standards,
IANA allocations, or a claim of independent cryptographic review.

## Start here

- [Scope and source baseline](docs/scope.md)
- [Requirement traceability](docs/traceability.md)
- [Open decisions and submission gates](docs/decisions.md)
- [Multicast dispatch draft](drafts/draft-robotweax-srt-multicast-00.xml)
- [Session authentication draft](drafts/draft-robotweax-srt-authentication-00.xml)

The first drafting pass concentrates on multicast and authentication. Base-SRT
corrections and delivery/group extensions are tracked separately so experimental
features do not silently redefine ordinary SRT. Draft names and organizational
author entries are placeholders pending editor confirmation.

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

Run the IETF `idnits` tool against the rendered text before submission. The
structural check is not a substitute for xml2rfc or idnits. No command in this
repository submits a draft. Generated output belongs in `build/`.

## Contribution rules

Distinguish implemented behavior, proposed requirements, and unresolved design.
Use immutable source references and do not promote self-interoperability into
independent interoperability. Record protocol changes in the decision register;
do not silently change existing versioned wire formats. Do not copy upstream
draft prose without preserving applicable attribution and rights notices.
