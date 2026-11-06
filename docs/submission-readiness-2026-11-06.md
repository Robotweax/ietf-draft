# SRT draft submission readiness

Status: local editorial checks pass; the package is not yet ready for submission.
No upload, external commit, outreach or submission was performed. Public package
and reference metadata were downloaded for local checks.

| Check | Result | Scope |
|---|---|---|
| idnits 3.3.0, submission mode | Zero errors, warnings and comments | Both RFCXML and rendered text, with online reference lookups |
| idnits 3.3.0, normal mode | Zero errors, warnings and comments | Both RFCXML and rendered text, with online reference lookups |
| Offline rerun | Zero errors; unresolved-reference warnings/comments remain in normal mode | Offline results cannot determine current remote reference state |
| xml2rfc 3.31.0 | Both text and HTML outputs generated | A nonfatal Python LibreSSL/urllib3 environment warning remains |
| Local structural/link checks | Passed | Draft anchors, local documentation links and required sections |
| Source/evidence checks | Passed | 38 source claims, 84 baseline section mappings and eight proposal mappings |
| Generated HTML structure | Passed | Unique IDs and 96 multicast / 95 authentication nonempty fragment links; no missing targets |
| Rendered text | Reviewed | Introduction, contents, references, security/IANA sections and confirmed author block; eight multicast and seven authentication pages |
| Browser visual layout | Open | Browser policy blocked local file URLs; no visual layout approval is claimed |

The [online idnits summary](../evidence/idnits-online-summary.json),
[offline summary](../evidence/idnits-offline-summary.json),
[render inspection](../evidence/render-readiness.json) and
[readiness manifest](../evidence/readiness-manifest.json) record input hashes,
results and tool provenance. Submission mode alone was insufficient: the broader
normal-mode pass found missing Introduction headings, which have been fixed.
Initial/pre-fix logs remain in evidence for transparency.

**Editorial changes completed**

Both drafts now open with an Introduction while retaining their status anchor
and clear unsubmitted/incomplete status. Running titles distinguish multicast
from session authentication. The submitted SRT reference now lists all five
source authors. Authentication cites the primitive definitions in
[RFC 8018](https://www.rfc-editor.org/info/rfc8018),
[RFC 2104](https://www.rfc-editor.org/info/rfc2104) and
[RFC 3394](https://www.rfc-editor.org/info/rfc3394).
Byte-array prose no longer resembles unresolved bracketed citations. No wire
layout, cryptographic parameter, acceptance rule or implementation was changed.

**Remaining gates before an initial submission**

| Gate | Required resolution | Owner or next action |
|---|---|---|
| Author identity | Confirmed: Marc Cymontkowski, Robotweax GmbH, marc@robotweax.ai | Supplied explicitly by the user; applied to both drafts |
| Publication metadata | Confirm draft names, intended status and Independent Submission versus intended IETF participation route | User/editor decision; current Experimental/independent metadata is provisional |
| Rights and attribution | Confirm contribution rights, author consent and applicable notices for submitted text | Editor review; a tool pass does not establish this |
| Reference classification | Decide which dependencies become normative as descriptive text becomes a complete specification | Editorial pass after protocol scope is agreed; current informative classification is explicit |
| Protocol decision disposition | Resolve or visibly defer clock sampling/delta, Control REXMITFLG exception, namespace governance, historical repair eligibility and position exhaustion | Review P01–P07 and the decision register |
| Completeness and claims | Keep unqualified extension combinations and security/interoperability limits explicit; decide what belongs in version 00 | Editorial/protocol review; independent peer and security review remain maturity gaps |
| Visual layout | Open both generated HTML files in a permitted local viewer and inspect representative content, references and navigation | Pending; static checks are not a substitute |
| Final freshness | Regenerate and rerun checks on the exact intended submission files after metadata or protocol edits | Local authoring step; current results bind to recorded hashes |
| External submission authorization | Explicit user instruction to submit/publish | Not authorized |

An initial draft may retain clearly identified open issues. That does not turn
these files into a complete conformance specification or establish security
review. The automated pass is evidence about document checks, not approval of
protocol choices or permission to publish.

**Reproduction**

The checker is installed temporarily at `/private/tmp/srt-idnits` from the official
npm package with lifecycle scripts disabled. Tool version and package-lock hash
are recorded. To reproduce in an isolated directory, install
`@ietf-tools/idnits@3.3.0`, render the drafts, then run:

```sh
python3 tools/check_nits.py --idnits /path/to/idnits
python3 tools/check_nits.py --idnits /path/to/idnits --online
```

The default is offline. The online option permits reference metadata lookups; it
does not upload or submit the drafts. The wrapper runs both modes and both formats,
retains raw JSON/stderr and exits nonzero for checker errors. See the
[official idnits documentation](https://github.com/ietf-tools/idnits) for checker
scope. Online zero-findings results do not replace the manual gates above.
