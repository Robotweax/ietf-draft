# Scope and baseline

Review date: 2026-11-14. Current delta: [0.3.3 release review](release-review-0.3.3.md).

## Immutable implementation baseline

Current release: Robotweax SRT `v0.3.3`, annotated tag object
`29c741d35f89a412926c374d6ccb15fa1270833c`, peeled commit
`69bd5328bf654ebbed894c77fd30ae5b2dfba9d7`.

Historical 0.3.2 audit: `235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1` is the
**annotated tag object**, not a commit; it peels to
`e45d0edd346e5c427d85e4c3b1ac605c81a23397`. Earlier ledgers called the tag
`implementation_commit`. Git resolves its file references to that commit, so
those hashes and test provenance are preserved rather than relabeled as 0.3.3.

Project version 0.3.3, ABI 0.3, compatible API 1.5.7 and HSv5 are separate axes.
Unreleased branches and adjacent projects remain outside this baseline.

## Specification baselines

- Submitted: [draft-sharabayko-srt-01](https://datatracker.ietf.org/doc/html/draft-sharabayko-srt-01),
  2021-09-07, expired individual Informational draft, Independent Submission stream.
- Supplemental: [Haivision source at commit 461a1fea85baaa4a928fa634d30c846dfd047ab8](https://github.com/Haivision/srt-rfc/blob/461a1fea85baaa4a928fa634d30c846dfd047ab8/draft-sharabayko-srt.md), committed 2025-09-22. The snapshot matches Git blob `e74908bb3c8f680f42bb76926fef2f79e6c3d083`. This is the revision cited by Robotweax's GCM contract. It is now the explicit supplemental baseline; identity with the hosted page displaying 2024-01-08 is not asserted. See [snapshot provenance](../audit/baselines/manifest.json) and the [section comparison](baseline-comparison.md).

The submitted draft reserves multicast group type 4 but does not specify this
dispatch protocol. AES-GCM is not exclusively a Robotweax invention. Existing
group-membership fields are not a complete bonding specification.

## Document plan

1. Base revision, coordinated with existing authors: correct ambiguities,
   reconcile GCM and handshake changes, clarify extensibility and group behavior.
2. Experimental multicast draft: dispatch and recovery, lifecycle, key schedule,
   resource/congestion behavior and deployment limits.
3. Authentication draft: first document session authentication v2; evaluate ACP1
   separately before deciding whether to integrate its versions 3 through 6.
4. Separate experimental hybrid-control draft: v7 component context, separate
   exchange, recovery controls, bounded fallback/return and admission limitations.
5. Subsequent delivery/group extensions: Control, Sensor, path labels, standby
   RTT and optional implementation identity. These are tracked, not yet specified
   in these draft sources.

ARQ-first and tail probes need algorithm/compatibility text, not a new handshake
field. Provider APIs, scheduler internals, backend selection, C ABI, installers
and samples are implementation documentation, except where a selected provider
defines a distinct on-wire algorithm.

## Evidence boundary

The historical 0.3.2 package includes source inspection, independent formula-level
vectors, selected release regressions, crypto/handshake adapters and an instrumented
native loopback suite. See the [status report](status-report-2026-11-06.md) and
[native results](native-publication-results.md) for counts and limits. The baseline
comparison assigns an editorial disposition to all 84 numbered submitted sections
and appendices; clause-level conformance and replacement base text remain work.
Independent interoperability, physical-network scale and cryptographic review
remain open. Structural checks and rendering do not establish protocol conformance.

The 0.3.3 pass reviews release/source deltas and updates editorial text. It does
not rerun the 0.3.2 runtime suite. Release-reported qualification remains attributed
to the release, not claimed as independent testing by this draft project.
