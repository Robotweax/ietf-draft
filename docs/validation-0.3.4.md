# SRT 0.3.4 editorial validation

The source and editorial checks apply to the four working drafts. Runtime,
network interoperability and cryptographic qualification were outside this pass.

On 20 November 2026, the structural checks passed for all four XML sources and
local Markdown links. The audit verified both tagged release deltas, including
all 143 changed paths for 0.3.4 and seven claims with pinned source anchors.
Historical evidence retained its original version bindings.

xml2rfc 3.31.0 rendered each draft to text and HTML. Idnits 3.3.0 checked each
XML and text file in normal and submission mode with public reference lookups.
All 16 checks returned zero errors, warnings and comments. The initial check
interpreted an uppercase socket policy name as a normative keyword. The final
draft describes that configuration in ordinary language.
[Final results and input hashes](../evidence/release-0.3.4/idnits-online-summary.json).
