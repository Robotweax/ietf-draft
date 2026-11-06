# Scope and baseline

Review date: 2026-11-06.

## Immutable implementation baseline

Robotweax SRT tag `v0.3.2`, commit
`235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1`:
https://github.com/Robotweax/srt/tree/235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1

Project version 0.3.2, ABI 0.3, compatible API 1.5.7 and HSv5 are independent
version axes. API enumeration values do not by themselves negotiate capabilities.
The source review used this tag, not the older local development checkout.
Unreleased aggregation branches and adjacent projects are outside this baseline.

## Specification baselines

- Submitted: [draft-sharabayko-srt-01](https://datatracker.ietf.org/doc/html/draft-sharabayko-srt-01),
  2021-09-07, expired individual Informational draft, Independent Submission stream.
- Supplemental: [Haivision working copy](https://haivision.github.io/srt-rfc/draft-sharabayko-srt.html),
  displaying 2024-01-08 at review. It includes AES-GCM and handshake clarifications.
  This mutable page must be replaced with a verified immutable revision for the
  final reconciliation. Robotweax's GCM contract cites upstream draft revision
  `461a1fea85baaa4a928fa634d30c846dfd047ab8`; do not assume it is the page revision.

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
4. Subsequent delivery/group extensions: Control, Sensor, path labels, standby
   RTT and optional implementation identity. These are tracked, not yet specified
   in the two initial draft sources.

ARQ-first and tail probes need algorithm/compatibility text, not a new handshake
field. Provider APIs, scheduler internals, backend selection, C ABI, installers
and samples are implementation documentation, except where a selected provider
defines a distinct on-wire algorithm.

## Evidence boundary

This seed is based on documentation and selected source inspection. It does not
report new test executions, independent protocol implementations, WAN benchmarks,
multicast scale qualification, or independent cryptographic review. Repository
validation checks document structure only. The initial drafts are intentionally
incomplete and identify their missing normative material.
