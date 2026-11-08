# Roadmap

[Docs](README.md) · [Goals](Goals.md)

**Next: recover current Sponza radiance.** Freeze one camera and the source/content/settings candidate, inspect Lit and Reference raw/display output, and repair the first wrong input or rendering owner. [Reference recovery](Architecture/ReferencePathTracer.md#next-work) owns the actions. Analytic-emitter repair F01 remains separate.

This is the delivery order, not an acceptance report. Keep one implementation task and one independent discovery/decision item active. Reuse applicable results; reopen only claims invalidated by changed inputs.

## Release work

| Order | Remaining result and exit | Reference |
| --- | --- | --- |
| `FR-03` / `REL-04` | Correct required rendering, failure behavior and lifetime. Qualify raw Reference before using it as an oracle. | Feature order below |
| `FR-04` / `REL-05` | Selected maps have frozen cameras, correct content/materials, qualified references, temporal inspection and resolved blocking visual defects. | [Content](Architecture/Content.md), [Reference](Architecture/ReferencePathTracer.md) |
| `FR-05` / `REL-06/07` | Realtime >=30 FPS on the declared support matrix, bounded memory, stability, native safety, D3D12/Vulkan parity and whole-frame review. Freeze the measurement protocol before results. | [Validation](Engineering/Validation.md), [studies](Architecture/PerformanceStudies.md) |
| `FR-06` / `REL-08/09` | Freeze repaired package bytes and reproduce consumer/source journeys. Join applicable feature, symbol, failure and package results for those bytes. | [Build and delivery](Architecture/BuildAndRun.md#delivery-work) |
| `FR-07` / `REL-10` | Close exact redistribution rights, Windows certificate/signing/timestamping, signed-byte verification, publication and download verification. | [Release inputs](Architecture/BuildAndRun.md#release-inputs) |
| `FR-08` / `REL-11` | Resolve release blockers and complete finite maintainer closeout before starting the advanced program. | [Release limits](Architecture/BuildAndRun.md#release-limits) |

Prepare a needed map artifact or unsigned qualification archive when its technical inputs exist. Feature proof can consume it before formal release approval; final candidate reconciliation binds the applicable observations to the actual bytes. This does not waive an earlier formal gate.

### Feature order within FR-03

1. Repair a demonstrated content, shader, frame or native-lifetime blocker, then return to rendering.
2. Recover Reference radiance and qualify its raw oracle; finish the remaining analytic-emitter, identity, API, lifecycle and ShippingEditor checks.
3. Prove [frame/scene execution](Architecture/Renderer.md): admission, publication, graph dependencies, barriers, aliasing and retirement (`FS-0..3`).
4. Prove [geometry/material correspondence](Architecture/Materials.md) and [ray inputs](Architecture/RayTracing.md) (`GR-0..4`).
5. Prove [raw direct/indirect transport](Architecture/Lighting.md), temporal validity and disocclusion (`LGT-0..2`, `DSP-1`).
6. Finish [exposure](Architecture/Display.md) and [SR/RR](Architecture/Reconstruction.md) input/reset/native checks (`DSP-2/3`).
7. Prove one tone/encoding/output path, then required [decals](Architecture/Materials.md#decals), grading, lens effects and HDR/SDR recovery (`GR-5`, `DSP-4..7`).
8. Qualify current controls, captures and latency consumers. Join the final whole-frame and product results (`FS-4`, `GR-6`, `LGT-4`, `DSP-8`, `RD-6`).

Check affected APIs and cost as each slice lands. Display processing and denoising cannot hide a wrong raw mean. RELAX and later renderer extensions stay outside first-release prerequisites.

### Earlier results and open blockers

- Product PD-1..4/internal FR-02 evidence retains its original unsigned `a0f34c8b...` candidate. That archive carried `S1` / `ISSUE-3`; changed publication/source code does not qualify replacement bytes. `FCR-PROD-06` and new-archive proof remain open; package removal was excluded.
- Earlier foundation and ownership source work is delivered. Reuse its bounded results; do not restart module-wide implementations. Historical temporary-output cleanup rejection supplies no deletion pass and is not rendering work.
- Accepted `PTD-00-R1` discovery and bounded F05 controls remain historical inputs. Current radiance, full raw-oracle and release-map adoption proof remain open.
- Final map/performance/native/adoption/publication checks remain unaccepted. Missing hardware, rights or required evidence is a named blocker; documentation cleanup changes none of those results.

[Original local artifacts](References.md#earlier-local-results) retain their candidate identities. A missing receipt or deleted report cannot be reconstructed as a pass from a summary.

## After REL-11

| Milestone | Deliverable and prerequisite |
| --- | --- |
| `M0` | Reuse applicable obtain/build/run proof from the released candidate. |
| `M1/M2` | Freeze Bistro/San Miguel inputs, cameras and lineage; qualify content and raw references (`CPIX-0..3`). Qualified inputs may feed work before final case assembly. [Content](Architecture/Content.md#content-to-pixel) |
| `M3` | Three causal classical studies and a difficult incident, with reproducible offline analysis and local failure/native/quality proof (`WS-S0..3`). This technical handoff admits owned-model work; documented reproduction closes at M6. [Studies](Architecture/PerformanceStudies.md) |
| `M4` | Qualify the diffuse-indirect reference component, scene-separated data and one fixed training recipe (`NG-S0..2`). Beauty is not a training label. [Neural graphics](Architecture/NeuralGraphics.md) |
| `M5` | Check export/operators, FP32 kernels and one runtime; measure optimizations and local quality/cost/failure/package gates (`NG-S3..6a`). |
| `M5R` | Complete the finite renderer-depth wave below using each subject's own qualified inputs. |
| `M6` | Join CPIX, classical, neural and M5R technical packets into three cases; perform one documented rehearsal. Close `WS-S4` and `NG-S6b` using actual observations. [Portfolio](Architecture/Portfolio.md) |
| `M7` | Challenge a consequential decision, use the result in a second real context, observe explanation/learning, then revise and retest (`KT-0..3`). External participation is optional and credited honestly. |

### Renderer depth wave

1. Raw [direct/indirect](Architecture/Lighting.md) estimator/reuse and admitted light/path increments (`DIR`, `IND-1..5`), followed by raw [base/anisotropic/coat](Architecture/Materials.md) proof (`MAT-S0..3`).
2. [SR/RR and RELAX](Architecture/Reconstruction.md) (`REC-S0..5`), then the affected direct/indirect/material guide, temporal, native, package and rehearsal joins (`DIR/IND-6/8`, `MAT-S4/5`). The later owned CNN is not a prerequisite.
3. [PTLAS](Architecture/RayTracing.md#ptlas), [Q0/Q1/Q2 volumetrics](Architecture/Volumetrics.md), [SHaRC then NRC](Architecture/Lighting.md#radiance-caches), and [NTC](Architecture/NeuralGraphics.md#neural-texture-compression).

Those final subjects depend on their own qualified AS/transport/material/native/measurement inputs. PTLAS hardware failure does not block ready volumetric/cache/texture work. NTC requires the M3 handoff and does not wait for NRC. **Full SHaRC technical, value, package and reproduction closure is required before NRC production.**

Qualify individual features, then the selected combined hero configuration. A failed or unavailable required cell remains open; it is not silently dropped. Add depth sections to the same three cases instead of creating a project per SDK.
