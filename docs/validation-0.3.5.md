# SRT 0.3.5 editorial validation

The validation scope covers release provenance, draft structure, rendering and idnits.

On 25 November 2026 the structural XML and local Markdown link checks passed.
The source audit verified both annotated tags, their peeled commits, the full
86-path delta and six claims against the release source. Earlier release ledgers
also passed against their original pins.

xml2rfc 3.31.0 rendered each of the four drafts to text and HTML. Idnits 3.3.0
checked XML and text in normal and submission modes with public reference lookups.
All 16 runs reported zero errors, warnings and comments.
[Idnits results and input hashes](../evidence/release-0.3.5/idnits-online-summary.json).

The review used source inspection for pacing and loss feedback. Runtime and
physical-device measurements are attributed to their upstream sources.
