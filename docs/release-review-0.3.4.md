# SRT 0.3.4 protocol review

Robotweax published v0.3.4 on 20 November 2026. The annotated tag
`559c0c4a0ba0ddbb0d65a62eebbe127f5187f2a5` resolves to release commit
`87742777290e6a77e38672ca35701f7184b75526`. The comparison starts at the
0.3.3 release commit `69bd5328bf654ebbed894c77fd30ae5b2dfba9d7` and spans
143 paths. The submitted SRT baseline remains draft-sharabayko-srt-01. Its
HSv5 extension mechanism provides the context for two provisional CONFIG
commands. [Public SRT draft](https://datatracker.ietf.org/doc/html/draft-sharabayko-srt-01)
and [release announcement](https://github.com/Robotweax/srt/releases/tag/v0.3.4).

| Change | Consequence for the drafts |
|---|---|
| MPEG-TS null packet deletion | New experimental draft describes bilateral negotiation, exact restoration, DATA/FEC byte accounting and rejection semantics. Released CONFIG 0x7f19 supersedes a development-only assignment that overlapped fixed-source multicast. |
| Pinned-key peer authentication | Authentication draft adds the distinct, default-off OpenSSL exchange, its transcript coverage, KM publication gate and security limits. CONFIG 0x7f16 and hybrid runtime subtype 0x7f16 use distinct namespaces. |
| Setup validation | Bound-attempt checks precede metadata and crypto changes. Inconsistent setup input is ignored under the existing deadline. Preserve pre-binding rejection and permitted Caller/Listener socket-ID changes. |
| Group admission | Build-time default of 64 retained members counts pending and connected members together. The configured range is 1–65536. The coordinated multicast sender also enforces its own capacity. |
| Hybrid freshness | Schedule the next query for known policy from first enqueue of the confirming request. This local scheduling change consumes elapsed round-trip time from the refresh interval. |
| Sparse traffic and Linux UDP batching | After 20 ms without datagrams, buffered sparse channels can wait for the earliest protocol deadline, capped at 10 ms. Pending KM and due deadlines retain the short poll. Experimental batching and segmentation change local UDP operations. None introduces a peer-negotiated record. |
| Epoll wake, file receive, build and buffer changes | Local API, file-handle lifetime and performance changes. The new wake call brings the C export count to 86. ABI 0.3 and compatible API 1.5.7 continue. The draft series gains no transport message for these operations. |

The [source ledger](../audit/release-0.3.4.json) identifies the release files
and the clauses they support. Open specification work includes independent
NPD and pinned-key vectors, full pinned-key transcript and timeout state
machines, cryptographic review and identifier coordination. Existing issues
around multicast group trust, compatible KM replay and physical-network
qualification remain open.

The release reports final-tag CI passing on its second attempt and records
three corrections without direct observing tests (expected-tag erasure, batch
receive checks and group enrolment rollback). It also distinguishes earlier
NPD qualification using obsolete identifiers from later checks using 0x7f19.
These are upstream results. This project update checks the source delta and
drafts. [Upstream qualification](https://github.com/Robotweax/srt/blob/87742777290e6a77e38672ca35701f7184b75526/packaging/qualification-0.3.4.md).
