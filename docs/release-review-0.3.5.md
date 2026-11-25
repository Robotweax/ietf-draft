# SRT 0.3.5 protocol review

Reviewed on 25 November 2026. The release changes when receivers report a
single missing packet and how senders schedule packets. Peers continue to use
the existing DATA, NAK and handshake formats. The four experimental documents
keep their existing wire identifiers and draft revision numbers.

| Source | Annotated tag | Peeled commit |
|---|---|---|
| v0.3.4 | 559c0c4a0ba0ddbb0d65a62eebbe127f5187f2a5 | 87742777290e6a77e38672ca35701f7184b75526 |
| v0.3.5 | d7b6ba7959593db1d37bac8178c6bff26ecfc5b0 | fd9dac31675b12022579d28a748abb0839004cde |

The [release announcement](https://github.com/Robotweax/srt/releases/tag/v0.3.5)
was published on 23 November at 22:58 UTC. The [source ledger](../audit/release-0.3.5.json)
pins the complete 86-path diff and the implementation anchors used for this review.

## Implications for the base SRT draft

| Submitted draft section | 0.3.5 observation and editorial disposition |
|---|---|
| 3.2.5 and 4.8.2, loss reporting and retransmission | On an eligible route the receiver can defer immediate feedback for one missing sequence until the next nonblocking read already budgeted for this receive slice. Only one recheck can be pending per channel. If the missing DATA passes normal admission and fills the gap, the initial NAK can be avoided. A different packet, failed admission, empty read or end of slice causes the pending feedback to be evaluated. Describe this as a receiver policy. Peers cannot rely on a fixed holdoff, extra timer, new control subtype or indefinite suppression. |
| 5.1, Live pacing | Continuous packet schedules retain fractional microseconds across sends. A fresh schedule rounds its first interval upward. Mobile timer coalescing may group deadlines for queued new Live DATA. The setting is local, with no negotiation or change to DATA timestamps or rate units. |
| 5.2.1, default FileCC | When the loss response lengthens the sending period by 3%, the implementation retains the fraction instead of rounding each increase up to a whole microsecond. At a 2-microsecond period, the intermediate value is 2.06 microseconds. The receiver still uses ordinary loss feedback. |

The one-gap predicate requires the new sequence to be two ahead of the previous
highest sequence, an empty receive loss list, effective ARQ level “always”, and
either a disabled retransmit flag or zero packet reordering tolerance. Matching
the destination socket, missing sequence and peer only identifies a candidate.
Authentication and packet admission still determine whether the gap is filled.
Ingress-only multicast routes skip this optimization and wake the owning channel
to service ACK/NAK deadlines. The existing multicast repair budgets and NPD
restoration rules therefore need no new wire fields. See the
[submitted SRT draft](https://datatracker.ietf.org/doc/html/draft-sharabayko-srt-01).

## Release scope and evidence

- Android and iOS default to timer pacing for sub-millisecond waits. The legacy
  policy remains selectable at build time. Experimental Live coalescing accepts
  0–10,000 microseconds, defaults to zero and requires timer pacing when enabled.
  The release's Android CPU observation concerns a Galaxy A56 at 2–8 Mbit/s.
  Battery energy was outside the measurement.
- Linux and macOS coarse-timer recovery starts probing after 5 ms. Four punctual
  probes restore timer waits. Repeated lateness or closely spaced episodes can
  increase probe spacing to 100 ms. These are implementation scheduling values.
- Empty socket-option strings clear the packet filter or select the Live
  congestion controller under the existing option-setting restrictions.
  Single-stream srt-live-transmit URI validation rejects invalid groupconnect
  settings before opening endpoint sockets. Group harness diagnostics and the
  targeted reference-peer retry affect test execution.
- Windows shared-library process-exit handling, batch receive checks and group
  admission cleanup affect local lifecycle or validation. The source adds tests
  for receive-batch bounds, peer storage clearing and expected authentication
  tag erasure. The pinned-key, NPD, ACP1, hybrid and UDP batching features
  retain their experimental status. ABI 0.3, API 1.5.7 and 86 C exports continue.
- Upstream reports final-tag CI and signed Windows packages. The earlier security
  delta has its own pre-freeze revision. Local evidence for this editorial pass
  covers source comparison and draft validation. Independent network, mobile
  performance and cryptographic qualification remain outside that evidence.
  [Upstream security delta](https://github.com/Robotweax/srt/blob/fd9dac31675b12022579d28a748abb0839004cde/docs/security-delta-0.3.5.md).

See the [editorial validation record](validation-0.3.5.md).
