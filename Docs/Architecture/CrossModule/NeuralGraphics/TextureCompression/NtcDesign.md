# NVIDIA Neural Texture Compression

[Docs](../../../../README.md) / [Up](../NeuralGraphicsDesign.md) / [Implementation](ImplementationPlan.md)

**Status:** selected post-release target; `NTC-D0` open; no local codec/cook/runtime support is claimed.
**Purpose:** `PGE-03/04/08..13`; demonstrate a real memory-versus-shader-work tradeoff across correlated PBR textures, with truthful SDK attribution.

## Observable Product And Bounded Modes

An author cooks one qualified material set and selects a scene whose material textures are decoded through NVIDIA NTC at sample time. The reviewer compares it with the same material using current BCn textures. Deliver offline compression/publication plus a real on-sample shader path on qualified D3D12/Vulkan tuples. An on-load decode alone does not satisfy this selected neural-shading target.

Retain BCn for deliberate baseline/unsupported materials. Select one NTC profile per cooked material set; no global latent-texture service, generic codec plugin API, standalone texture explorer or new material editor. Feedback-driven sparse streaming, runtime training, OptiX, per-frame recompression and replacement of unrelated texture classes are excluded.

## Primary Sources And Feasibility

Inspected 2026-10-25: [RTXNTC](https://github.com/NVIDIA-RTX/RTXNTC/tree/0ffc32685134c50d6bd09689112fa756052c1b8c), [quality settings](https://github.com/NVIDIA-RTX/RTXNTC/blob/0ffc32685134c50d6bd09689112fa756052c1b8c/docs/SettingsAndQuality.md), and [on-sample integration](https://github.com/NVIDIA-RTX/RTXNTC/blob/0ffc32685134c50d6bd09689112fa756052c1b8c/docs/integration/InferenceOnSample.md). These distinguish offline compression, latent/weight data and shader decode, with mode-specific compiler/device requirements. Verify licenses and actual installed requirements in NTC-D0.

The inspected README warns that the D3D12 LinAlg route depends on preview components and is not a shipping route. Freeze a supported non-preview implementation first; do not require Developer Mode, a preview Agility upgrade or pretend all cooperative-vector paths are available. A qualified integer/fallback path can establish correctness; a separate supported acceleration profile must prove its actual cost. No SDK installation or platform upgrade is part of this planning pass.

## Material And Sampling Semantics

The existing [material contract](../../../Modules/Engine/Renderer/Features/GeometryAndResources/GeometryMaterialsAndGBuffer.md) remains the decoded truth. Freeze a per-texture-set table: channel meanings, scalar ranges, linear/sRGB transforms, normal encoding, alpha cutoff, mip dimensions, UV/wrap mode, filtering and decoder profile. Correlated-channel coding must not change a channel's physical meaning.

The decoder's texel result is not automatically a filtered texture sample. Define the chosen mip/LOD/derivative and stochastic filtering method, sample/PDF/seed/temporal behavior and anisotropic/minification limits. Check it independently against the baseline's filtering target. Use one decoder helper from raster and ray material sampling; the ray path needs a declared ray-cone/LOD policy, not invented screen derivatives.

Normal angular error, roughness/metallic bias and alpha-cutout coverage need separate controls; PSNR cannot certify shaded equivalence. The first profile compresses base color, normal and roughness/metallic/AO in one bounded material set, with emission and alpha-sensitive textures retained as ordinary textures until individually qualified. A masked material therefore remains supported without compressing its coverage channel.

## Existing Owners And Data Lifetime

The existing content cooker invokes a pinned offline NTC compressor; Assets publishes immutable latent/weight/channel/mip metadata with hashes and original input identities. Renderer MaterialCache and existing texture residency own upload, descriptor binding and activation. ShaderSystem cooks the fixed decoder/profile. RHI provides existing buffers/textures/bindings and only a justified neutral capability if the selected hardware instruction path requires it.

Budget: one cooked material encoding discriminator, existing material publication/descriptor path, one shared sampling helper and existing capability/status presentation. No CPU image/latent copies beyond the offline transform and actual upload boundary; no parallel material records, new renderer, generic graph IR or second residency manager. Old/new data retire after all queues; report the complete package and resident/workspace bytes, not latent bitrate alone.

NVIDIA owns codec/model/decoder algorithms. Sparkle owns channel/format integration, sampling correctness, publication/lifetime, material-path coupling and analysis. This does not replace the owned denoiser's model/kernel evidence.

## NTC-D0 And Pre-Result Measurements

The Tools/Renderer implementer with material/native reviewers freezes source rights, installed CUDA compression environment, DXC/device/API matrix, exact compressor/decode release, profile and filtering before production. Gate after release, qualified materials/raw references and classical study input; source compression and runtime eligibility are separate.

Freeze quality/error bands on licensed train/validation material sets and a sealed held-out set before choosing bitrate. Record compression wall time, reproducibility policy, model/latent/file hashes, total cook/package/resident/upload bytes, decode/whole-frame time and peak replacement memory. Compare identical content, mips, camera, output, lighting and filtering target against current BCn; measure both memory-pressure and shader-bound scenes. Existing workload repeat/CI rules apply; freeze practical effect and quality limits before seeing held-out results.

## Acceptance, Failure And Evidence

| Criterion | Falsifier and retained result |
| --- | --- |
| `AC-NTC-01` artifact | Reject wrong channel/mip/profile/length/hash; two frozen-input cook runs obey the declared reproducibility policy; no silent format migration |
| `AC-NTC-02` decoder | Independent decoded texel/channel fixtures, normal angular/scalar error, sRGB and extreme-value controls on both APIs |
| `AC-NTC-03` sampling | Magnification/minification, grazing UVs, mip transitions and moving camera; raster/ray LOD/filtering parity within the declared domain |
| `AC-NTC-04` value | Held-out shaded/raw and temporal quality, memory-versus-time frontier, all weights/latents/descriptors/upload/decoder work and full-frame cost |
| `AC-NTC-05` product | Actual NTC sampling in material shaders, truthful unsupported/refusal path, safe replacement, manifested rights/runtime bytes and documented reproduction |

`FM-NTC-01`: corrupt/mismatched cooked set -> reject before GPU publication. `FM-NTC-02`: unsupported selected decoder -> explicit material admission failure with reason; never claim BCn output as NTC. `FM-NTC-03`: allocation/upload/compile failure -> retain a valid prior generation only under existing publication policy, report failure. `FM-NTC-04`: stale descriptors/weights -> prevent scheduling and retire safely. Inject these controls in the owning cook/runtime paths.

Reports and assets live in `neural-textures/ntc/` beneath the existing workload artifact root; large source/training sets and codec build outputs stay out of repository source. All local acceptance cells are future portfolio requirements, not current readiness or a first-release feature-family expansion.

## Check And Risk Ownership

| Check | Defect-detecting obligation |
| --- | --- |
| CHK-NTC-01 | artifact/channel/decoder conformance; retain the associated AC and FM row identities in the candidate report. |
| CHK-NTC-02 | LOD/filtering/motion and corrupt/unsupported/replacement controls; retain the associated AC and FM row identities in the candidate report. |
| CHK-NTC-03 | held-out shaded quality and complete bytes-versus-frame-cost frontier; retain the associated AC and FM row identities in the candidate report. |

RISK-NTC-01: filtering/normal distortion or decode cost outweighs memory saving. The feature implementer and subject reviewer own mitigation: freeze channel/filter contract and include shader-bound losing cases. A failed required falsifier/value band stops the stage; do not hide it by expanding scope or lowering a post-hoc threshold. The local acceptance table remains conjunctive, and the report records every supported API/domain cell separately.
