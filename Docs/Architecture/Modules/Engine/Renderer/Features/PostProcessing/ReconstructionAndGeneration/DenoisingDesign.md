# Denoising And Upscaling Depth

[Docs](../../../../../../../README.md) / [Up](ReconstructionAndGenerationDesign.md) / [Implementation](ImplementationPlan.md)

**Status:** selected qualification/extension target; `REC-D0` open. Existing Linear/DLSS SR/RR source is not accepted quality, temporal or product evidence.
**Purpose:** `PGE-02..05/08..13`; explain and measure how noisy lighting becomes a stable output without hiding estimator defects.

## Required Products

| Path | Required result and limit |
| --- | --- |
| Linear and DLSS SR | Complete the existing release obligation for correct inputs, actual optimal/render/output extents, device/API support, reset, state/lifetime and visible quality/cost |
| DLSS RR | Qualify the pinned SDK's denoise/reconstruction contract and output ownership; current NativeAA RR followed by SR is source behavior, not proof that the SDK endorses that composition |
| Classical temporal denoiser | One selected NRD RELAX diffuse/specular profile, integrated through existing lighting/reconstruction ownership after release; no blanket NRD-family or new NRI/RHI abstraction |
| Owned neural denoiser | The separate [fixed diffuse-indirect CNN](../../../../../../CrossModule/NeuralGraphics/NeuralGraphicsDesign.md), with owned data/model/kernel conformance; it does not substitute for diffuse/specular temporal denoising or SDK integration proof |

Existing DIR-6/IND-6 portable reconstruction remains required under its frozen contract; RELAX can replace that implementation after release once equivalent supported consumers and failures pass. Do not add two production classical denoisers to tick separate plans. Frame generation, additional vendor upscalers, temporal AA frameworks and generic filter/plugin graphs are unselected.

## Primary Contract And Sampling Inputs

Inspected 2026-10-25: [NRD](https://github.com/NVIDIA-RTX/NRD/tree/e06852bd545973cb224eb28820b891205cfc1624), [Streamline SR](https://github.com/NVIDIA-RTX/Streamline/blob/625db07d4bca5abc67d781c42af7e7fb0761f1b6/docs/ProgrammingGuideDLSS.md) and [RR](https://github.com/NVIDIA-RTX/Streamline/blob/625db07d4bca5abc67d781c42af7e7fb0761f1b6/docs/ProgrammingGuideDLSS_RR.md). These are research pins, not a mandate to upgrade the locally pinned SDK. REC-D0 matches actual installed headers and release-specific guide.

Freeze a signal table for each actual consumer: diffuse/specular radiance and demodulation, hit-distance definition/units, normal space/packing, roughness convention, linear/view depth, motion sign/scale/space/jitter, camera matrices, exposure/pre-exposure and frame/viewport/extent/generation. ReSTIR-selected paths do not automatically supply a denoiser-valid hit distance. Derive the actual guide estimator from the selected SDK contract and transport; never divide geometric hit distance by a radiance PDF or copy an incompatible selected-ray value.

RR albedos/normals/roughness are material signals, not lighting or arbitrary GBuffer aliases. Verify motion from current/previous surface and camera, sky motion, disocclusion and invalid history. Upscaler jitter normalization must agree with input extents. Reject deliberate sign/unit/space/exposure/roughness mutations before interpreting image quality.

## Output Ownership And Lifetime

The selected release-specific target is combined RR reconstruction directly producing output extent. REC-D0 freezes the actual mode/optimal extent, settings compatibility and tags; one producer owns the resolved output and no later SR evaluation follows RR. A denoise-only mode is not selected merely to preserve the current sequence. Any required change updates the existing dossier's AC-IRU/output contract, selectors and every caller in one reviewed slice.

Renderer owns signal products/policy, feature-local adapters own NRD/Streamline dispatch setup, graph resources express actual guide/history/workspace access and all queues retire generations. RHI provides native execution/resources, not denoiser state or settings. Import only the selected NRD shader/runtime dependencies through existing ShaderSystem/build/package mechanisms; no parallel SDK renderer, light database or NRI layer.

Budget: existing requested/provider settings and composition hook, necessary typed guides, one RELAX private state owner, existing status/debug/export paths. Account for every guide/history/intermediate allocation, bytes/pixel, barriers and old/new overlap. Remove unused guide targets/writes when no selected consumer needs them, preserving any other consumer. Do not rely on a disabled branch to justify unconditional resource cost.

## REC-D0 And Measurement Card

The rendering/native implementer and signal reviewer freeze actual SDK/profile/license/eligibility, transport guide derivations, topology, reset/failure policy, ABI/lifetime and numeric gates. Existing release SR/RR qualification may execute in DSP-1 and DSP-3 now; RELAX production is later scope. The owned model retains its own discovery and REL-11/M3 dependencies.

Compare raw estimator, classical denoise, SR-only and RR in meaningful compatible cells; assess the owned denoiser only on its qualified diffuse-indirect component. Use identical raw inputs or matched sampler identity, fixed exposure/output and independent high-sample references. Freeze spatial error, temporal trail/flicker/response, disocclusion recovery, native correctness, practical whole-frame cost and peak memory before candidate results. Include motion, thin alpha geometry, specular detail, light/emissive changes and resize/cut/provider switching. Temporal smoothing cannot compensate for biased/wrong raw transport.

## Acceptance And Controlled Failures

| Criterion | Required falsifier/evidence |
| --- | --- |
| `AC-REC-01` signals | Named guide semantics with analytic motion/depth/normal/exposure and deliberate wrong-guide counterexamples; correct producer identity |
| `AC-REC-02` SR/RR | Actual release SDK/device/API tuple, extents/tags/constants, single output producer, NativeAA/scaled paths and original AC-IRU proof |
| `AC-REC-03` RELAX | Real diffuse/specular temporal reconstruction, raw input retained, admitted guide/hit-distance validity and independent quality/sequence comparison |
| `AC-REC-04` lifetime/failures | Two Views, discontinuities/resize, replacement/shutdown, missing SDK, allocation/evaluation/device failure and truthful requested/active status |
| `AC-REC-05` value/delivery | Held-out quality/temporal/time/memory frontier, native artifacts, manifested dependencies and documented reproduction |

`FM-REC-01`: wrong guide frame/unit/layout -> reject before evaluation, demonstrate a deliberately altered input. `FM-REC-02`: stale history/extent/provider -> invalidate before scheduling, test a cut/resize. `FM-REC-03`: selected SDK unavailable/evaluation failed -> existing explicit refusal/failure with selection intact; no silent Linear or copy output. `FM-REC-04`: in-flight resource replacement -> completion-safe retirement, checked with delayed work.

Evidence lives in `reconstruction/denoising-upscaling/` under the existing workload artifact root. First-release FCR-REN-10 and local later criteria retain separate verdicts; no NRD/owned-model result can close SR/RR by association.

## Check And Risk Ownership

| Check | Defect-detecting obligation |
| --- | --- |
| CHK-REC-01 | guide/extent/topology conformance and deliberate wrong-input controls; retain the associated AC and FM row identities in the candidate report. |
| CHK-REC-02 | motion/disocclusion/reset/two-View/SDK and completion faults; retain the associated AC and FM row identities in the candidate report. |
| CHK-REC-03 | compatible held-out spatial/temporal full-cost/package replay; retain the associated AC and FM row identities in the candidate report. |

RISK-REC-01: invalid guides hide transport errors or double reconstruction. The feature implementer and subject reviewer own mitigation: preserve raw outputs and freeze the installed SDK topology. A failed required falsifier/value band stops the stage; do not hide it by expanding scope or lowering a post-hoc threshold. The local acceptance table remains conjunctive, and the report records every supported API/domain cell separately.
