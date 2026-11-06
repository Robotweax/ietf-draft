# Pinned C++ conformance adapter

The adapter compares the independent frozen vectors with Robotweax SRT 0.3.2
codec and authentication functions. It also exercises authenticated valid KM,
rotation, stale-body idempotency and deterministic timing components. Deterministic
randomness and public fixture secrets are used only inside the test executable.
This is not an independent peer implementation.

## Reproduce

Export commit `235cbdb6c0b0d1950b3e74d623ffc21108bf3ef1` from an existing SRT
Git repository into an empty temporary source directory. Keep that export
unchanged. From this draft working directory, replace the paths below:

```sh
mkdir -p /tmp/srt-fixtures
python3 conformance/make_fixture_header.py /tmp/srt-fixtures/fixtures.hpp
cmake -S conformance -B /tmp/srt-conformance-build \
  -DSRT_SOURCE=/path/to/pinned-export \
  -DFIXTURE_HEADER_DIR=/tmp/srt-fixtures \
  -DCMAKE_BUILD_TYPE=Release
cmake --build /tmp/srt-conformance-build --target srt_draft_adapter robotweax_srt_tests --parallel 4
/tmp/srt-conformance-build/srt_draft_adapter
/tmp/srt-conformance-build/srt/robotweax_srt_tests --include session_authentication
/tmp/srt-conformance-build/srt/robotweax_srt_tests --include multicast_dispatch
/tmp/srt-conformance-build/srt/robotweax_srt_tests --include crypto_session_stale_response_cannot_release_a_pending_rotation
/tmp/srt-conformance-build/srt/robotweax_srt_tests --include tsbpd_clock
```

OpenSSL 3 development libraries are required; set `OPENSSL_ROOT_DIR` if needed.
The initial run used AppleClang 21, C++20, Release, OpenSSL 3.6.5, and enabled
AES-GCM. ACP1 remained disabled. Network cases require permission to bind local
UDP and join loopback multicast. Record skipped tests rather than counting them
as passes. Capture each command's exit status separately.

`check_valid_km.py` expects `evidence/adapter.log` containing the adapter output.
It parses the deterministic AES-128-CTR KM body, derives KEK from the final eight
salt bytes, unwraps with RFC3394, checks the known SEK and rewraps exactly. Run
with a Python environment containing `cryptography` (tested with 41.0.3):

```sh
python3 conformance/check_valid_km.py
```

`--write` regenerates the valid-KM fixture and is for intentional, reviewed
changes only. Ordinary verification compares with the frozen fixture.

## Coverage limits

- Frozen multicast codec and authentication byte comparisons are now tested
  against the pinned C++ implementation, including selected malformed/replay cases.
- The valid KM fixture originates from that implementation; separate key-wrap
  verification does not make the full transaction independently authored.
- Clock cases model old sender origins, arrival delay and rollover without wall
  clock sleeps. They do not execute the complete multicast admission state machine
  or prove wire-time timestamp capture. Existing loopback late-join tests include
  settling delays and are complementary evidence.
- Loopback fan-out is not physical-LAN/WAN scale, independent interoperability,
  full security review or exhaustive conformance.

## Expanded matrix and handoff continuation

The adapter now compiles `admission_handoff.cpp` as well as `adapter.cpp`.
It covers all six CTR/GCM × 16/24/32-byte key cases, initial DATA, GCM tampering
and retransmission normalization, and six serialized handshake/runtime cases.
Capture output in `evidence/adapter-matrix.log` and run
`python3 conformance/check_crypto_matrix.py` with Python cryptography installed.
See [the detailed handoff boundary](../docs/matrix-and-handoff-results.md): the
production coordinator and native join are deliberately not exercised.
Historical evidence logs retain their older, narrower adapter scope.
