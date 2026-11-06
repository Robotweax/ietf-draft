# Open decisions and publication gates

No design choice in this register is implicitly approved by the initial drafts.

| ID | Decision / action | Completion criterion |
|---|---|---|
| D01 | Editors, author contacts, draft names and intended stream | Named editors approve metadata; coordinate continuation with existing SRT authors or use separate individual drafts |
| D02 | Base text provenance | Pin supplemental working-copy revision and complete section-by-section comparison |
| D03 | Identifier governance | Coordinate handshake/runtime values, group type and profile names; define registry/allocation policy and collision migration |
| D04 | Multicast resource/congestion policy | Define aggregate original/repair limits, receiver limits, feedback amplification controls, slow receiver policy and deployment scope |
| D05 | Multicast clock and rollover semantics | Specify origin-delta calculation, serial arithmetic, admission cursor, retained repair history, wrap and empty-group behavior with vectors |
| D06 | Multicast key lifecycle | Specify generation boundaries, joining keys, failed generation, late confirmation and retention; review nonce uniqueness and same-packet retransmission |
| D07 | Multicast trust model | Decide whether shared-key insider forgery and no leave revocation are acceptable; otherwise design a new version |
| D08 | Interface hints | Define host-local IPv6 interface-index interpretation; never assume sender index selects receiver interface |
| D09 | Authentication design review | Independent review of PBKDF, transcript coverage, direction separation, replay, shared-secret identity and absent forward secrecy |
| D10 | ACP1 scope | Decide whether versions 3–6 remain a separate Experimental document; do not silently replace v2 or call ACP1 supported security |
| D11 | Profile semantics | Resolve REXMITFLG discrepancy and formalize Control/Sensor rejection and delivery rules |
| D12 | Extension combinations | Explicit matrix for topology, cipher, authentication, filters and profiles, including fail-closed behavior |
| D13 | Conformance corpus | Independently calculated vectors plus malformed, duplicate, loss, reorder, replay, rollover and lifetime tests |
| D14 | Interoperability and deployment | Seek independent peer implementation; separately qualify multicast physical network and scale |

## Initial Internet-Draft posting gates

- Confirm editors, contacts, names, scope, attribution and applicable IPR notices.
- Make status and incompleteness explicit; resolve accidental contradictions.
- Render with xml2rfc; inspect text/HTML; run idnits and review warnings.
- Check references, required sections and IANA wording; submit through Datatracker.

An initial draft may contain open issues. Independent interoperability and a
completed crypto review are not administrative prerequisites for posting, but
they are maturity requirements before making broader support/security claims.
No submission or outreach is authorized by repository validation commands.

## Subsequent publication

For an Informational/Experimental independent RFC, follow the
[ISE process](https://www.rfc-editor.org/authors/ise/ise-checklist/).
For IETF standards-track work, seek an appropriate community venue and consensus;
an individual upload does not confer adoption. Follow the current
[submission instructions](https://datatracker.ietf.org/submit/tool-instructions/).
