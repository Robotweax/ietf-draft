# SRT extension combination matrix

This matrix is a review proposal against Robotweax SRT 0.3.2, not a declaration
that every cross-product has been tested. It separates the release contract from
executed local evidence. The pinned [source ledger](../audit/proposed-clauses.json)
identifies the authorities for P06; detailed test records remain in the
[execution manifest](../evidence/run-manifest.json).

Apply all applicable rows. A configuration needs a valid topology, delivery
bundle, cipher and security policy; one matching row does not override a
restriction in another. **Excluded** means a release restriction or explicit
profile exclusion. **Unqualified** means evidence or specification is missing,
not necessarily that the implementation rejects the setting. There is no default
rule that unspecified combinations are supported or rejected.

| Topology and delivery | Cipher selection | Authentication and filter constraints | Contract status | Local evidence boundary |
|---|---|---|---|---|
| Individual Caller/Listener, Live Message | Ordinary plaintext/CTR; explicit GCM extension | Ordinary or explicit session-authentication v2; Live GCM permits the built-in recovery filters | Documented configuration family | Selected v2 and crypto cases; not every cipher/filter/authentication cross-product |
| Individual Caller/Listener, File Stream | Ordinary plaintext/CTR; explicit GCM extension | GCM File bundle excludes FEC; v2 requires secret and enforced encryption | Documented configuration family | No new native File qualification in this draft audit |
| Individual Caller/Listener, Control Message | CTR or explicit GCM as configured | Exact `control-v1` confirmation; no packet filter; Control/GCM requires secret and enforcement; v2 is separately opt-in | Documented configuration family | Source/contract reviewed; complete Control combination suite not rerun here |
| Individual Caller/Listener, Sensor Message | Cipher-dependent Sensor contract; explicit GCM available | `fec-sensor-v1` required; Sensor/GCM requires secret and enforcement; v2 is separately opt-in | Documented configuration family | Native multicast results do not qualify Sensor; full Sensor matrix remains separate |
| Individual Rendezvous | Ordinary cipher selection; GCM explicit on both peers | Session authentication v2 alone excluded; experimental ACP1 v4 requires its gate and prerequisites | Documented ordinary/experimental families | No native Rendezvous extension qualification in this audit |
| Broadcast/Backup Caller/Listener groups | Ordinary policy or explicit GCM, per member | Live with TSBPD; independent member crypto; ACP1 group profile uses v5 and prerequisites | Documented ordinary/experimental families | Multicast shared-key tests are not evidence for bonding crypto or failover |
| Broadcast/Backup Group Rendezvous | Experimental policy | ACP1 required on both groups; version 6; Live Message; matched group types and policy | Experimental and separately gated | ACP1 disabled in the recorded audit suite |
| Multicast Caller/Listener, Live Message | Plaintext, CTR or GCM; one mode/key length across admitted members | Packet-filter FEC excluded; exactly one receiver control link; sender is listener side | Documented experimental multicast profile | Native plaintext/CTR256/GCM256, sync/async; other AES sizes have component evidence |
| Multicast with File, Control, Sensor or Rendezvous | Any | Violates the multicast delivery/topology scope | Excluded | Contracts and selected rejection paths; not an exhaustive negative cross-product |
| Multicast plus authentication v2 or ACP1 | Any | Must satisfy group admission and version/topology binding independently | Unqualified by this matrix; do not infer support or rejection | No combined native qualification here; ACP1 documentation explicitly covers ordinary/Broadcast/Backup topologies, not a multicast endorsement |

**Security prerequisites**

- V2 requires explicit enablement, a passphrase and enforced encryption. It does
  not select GCM and it does not authenticate all transport controls.
- ACP1 requires the experimental build gate, required policy, SESSIONAUTH and
  encryption prerequisites. Versions 3, 4, 5 and 6 bind different topology
  contexts; they are not fallback alternatives for one connection.
- GCM selection does not authenticate peer identity against another holder of a
  shared secret. Multicast group-key holders share the documented insider risk.
- Plaintext behavior in ordinary optional-encryption bundles must not be imported
  into Control/GCM, Sensor/GCM or session-authenticated configurations.

**Ancillary extension rules**

| Extension | Required treatment | Qualification status |
|---|---|---|
| Path identifiers | Apply group/path matching and required-policy rules; labels do not authorize admission | Source/contract mapping exists; full combined matrix remains unqualified |
| Standby RTT | Separate handshake capability from runtime probe subtype; a successful probe is not DATA health or authentication | Not covered by the native multicast readiness suite |
| Implementation identity | Preserve its optional disclosure and unknown-record rules; do not use as authorization | Exact combination behavior needs dedicated clause review |
| ARQ-first and tail probes | Preserve group exclusions and profile-specific recovery rules | Group exclusion is source-traced; no universal pacing/deadline guarantee |
| FEC | Use the selected delivery/cipher-specific recovery format; multicast listener FEC is rejected | Formula parity and selected crypto tests do not certify every geometry |

**Rejection evidence and open cells**

The group option path rejects non-Live or disabled-TSBPD configurations after
ordinary socket validation. The multicast admission path explicitly rejects an
enabled packet filter. The receiver connect path rejects multiple endpoints,
an occupied receiver or nonzero endpoint weight. These are specific source
anchors, not proof that every invalid order of option-setting has been tested.

The next evidence work should close named cells, especially multicast with v2,
ACP1 multicast exclusion/support, Control/Sensor profile combinations and bonding
key separation. For each cell record both peers' settings, role, cipher/key size,
build gates, expected accept/reject result, and whether the result was observed
or only inferred from a contract. Do not expand the published profile by taking
the union of isolated passing features.
