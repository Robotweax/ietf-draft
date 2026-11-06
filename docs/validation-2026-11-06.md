# Initial document validation — 2026-11-06

Validated source baseline: commit `3e4f9581c41d4e205c609f7293d2893c203b03f0`.

- `python3 tools/check.py`: passed for both RFCXML documents and local Markdown link targets.
- `make render XML2RFC=/private/tmp/ietf-draft-venv/bin/xml2rfc`: passed, exit 0, using xml2rfc 3.31.0 in an isolated environment.
- Both authentication and multicast sources generated text and HTML successfully through xml2rfc's normal validation/rendering pipeline.
- Remote Git tree inspection confirmed all ten initial source/support files were present.

The rendering environment emitted LibreSSL/urllib3 compatibility and unwritable default cache-directory warnings. There were no fatal rendering errors. No remote reference fetching is needed by the inline reference records in this seed. Generated output is intentionally untracked and reproducible through the Makefile.

Not completed: idnits; browser visual inspection of rendered HTML; independent technical/cryptographic review; source-to-test evidence extraction; independent wire vectors; interoperability or multicast deployment testing. Author metadata and draft names remain editorial placeholders. These checks do not establish protocol correctness or submission readiness, and no IETF submission has been made.
