# NVIDIA NRC Delivery Contract

[Docs](../../../../../../../../README.md) / [Up](../RadianceCachingDesign.md) / [Implementation](ImplementationPlan.md)

**Status:** selected later target; `NRC-D0` open; implementation blocked until SHaRC is fully accepted.
**Reason:** `PGE-02/03/05/08..13`; compare neural radiance caching against an understood, delivered classical cache and uncached transport.

## Product And Attribution

An engineer selects NRC in the existing indirect-lighting path, obtains actual online-trained cache-assisted indirect output, and can explain when its quality, response and total cost justify it over SHaRC. NVIDIA owns the SDK network/training/inference internals. Sparkle owns the transport coupling, data semantics, scheduling, native integration, failure behavior and comparative evidence. The [owned neural denoiser](../../../../../../../CrossModule/NeuralGraphics/NeuralGraphicsDesign.md) remains required evidence for personally implemented model/training/kernel work.

Initial scope: the same diffuse continuation as SHaRC, followed by one rough-specular profile after the shared-domain proof. Exclude arbitrary model loading, online model persistence, distributed training, custom training infrastructure and changes to the raw Reference estimator. NRC is a biased acceleration mode, not ground truth.

## Primary Research

Inspected 2026-10-25: [NRC binary/header distribution](https://github.com/NVIDIA-RTX/NRC/tree/7de30e4eb2a1a27118eb9a6acf5f715d757240c0), [RTXGI integration guide](https://github.com/NVIDIA-RTX/RTXGI/blob/711dd7ad26d990902b0fe33f27ad02dd44a395bb/Docs/NrcGuide.md), and [the original research](https://research.nvidia.com/labs/rtr/publication/muller2021nrc/). The SDK guide describes online radiance learning with separate update/query tracing and resolve, and API-specific libraries. Research performance is not a Sparkle measurement.

NRC-D0 verifies installed headers/binaries, API-specific device features, GPU/driver/runtime requirements, redistribution and SDK status. The inspected current guide mentions CUDA-runtime dependencies; freeze actual package bytes rather than assuming the existing Streamline dependency supplies NRC. Do not auto-upgrade a working SDK from moving-main documentation.

## Transport And Online Learning Semantics

Freeze these as a reviewed rule-to-shader/API ledger before implementation:

1. Map the existing continuation state to SDK surface attributes, radiance/reflectance, position/direction, path throughput and event records. Document units, handedness and roughness convention.
2. Define update/query vertex accounting for misses, emission, direct NEE, sampling PDFs, roulette and finite path depth. Use the library's propagation contract exactly; a resolve contribution must not repeat radiance already accumulated by the path.
3. Define the query termination heuristic and which diffuse/rough-specular events are eligible. Show how the cached tail is composed with pre-termination throughput. Include a zero-query/forced-full-path control; a cache must not change the raw reference mode.
4. Record the SDK-prescribed ordering of begin-frame, update/query tracing, prediction/training, resolve and end-frame for the pinned release. Prove resource hazards, native command-state restoration and context retirement; do not assume SHaRC's pass order applies to NRC.
5. Separate cache cold start/adaptation from image denoiser and reservoir history. Freeze reset/retrain policy for light/material/geometry/scene-scale changes, camera relocation, resize, pause and two Views. Report training/adaptation transients, not only converged still images.

## Owners And Budget

Existing indirect lighting owns the feature and typed lobe results; FrameGraph owns ordering/lifetimes. API-specific SDK interop stays inside dedicated existing-style external-provider adapters; native handles do not enter Renderer feature headers or general RHI policy. Use the accepted [Renderer/RHI boundary](../../../../../../../Decisions/RendererRhiBoundary.md).

Budget: reuse the SHaRC-era selection/composition/status seams, existing transport/material/light data and frame completion. One feature-local context per admitted View/device; byte-account context/workspace/model state and replacement overlap. Zero new generic tensor graphs, cache registry, scheduler, shadow Scene or configuration framework. Native callback/interop additions need a consumer, lifetime, failure and defect-detecting check.

## NRC-D0 And Measurement Card

Research/feasibility may run early; production requires all `AC-SHARC-01..05`, including documented reproduction, plus release/M3 inputs. The implementer and transport/native reviewers freeze exact SDK calls, supported device/API matrix, source/rights/package ledger and numeric gates before source changes.

Compare uncached, SHaRC and NRC on a common diffuse domain before adding rough specular. Use the existing workload scene/repeat/uncertainty rules, fixed settings and separate cold/warm runs. Freeze maximum acceptable raw-reference error, bias, temporal lag, peak memory and practical whole-frame improvement. Include training, update/query rays, prediction/resolve, barriers and adaptation warm-up in cost; report convergence time and break-even frame count. An SDK inference timer alone is insufficient.

## Acceptance And Controlled Failures

| Criterion | Check and artifact |
| --- | --- |
| `AC-NRC-01` coupling | Independent minimal diffuse transport, forced full-path and zero-query controls; contribution/event ledger catches double count and incorrect throughput |
| `AC-NRC-02` learning | Online update changes real predictions; frozen-update/perturbed-input ablations, cold start, light toggle and material/geometry edit sequences expose adaptation and bias |
| `AC-NRC-03` domain | Common-domain SHaRC/uncached comparison first; separately qualified rough-specular cases, no claim for sharp/delta/media transport outside scope |
| `AC-NRC-04` native/value | D3D12/Vulkan on actually eligible tuples, validation/capture, all-queue context retirement, total quality/time/memory frontier and held-out dynamics |
| `AC-NRC-05` delivery | Actual active state, verified SDK/package identity/rights, supported/unsupported behavior and one documented reproduction of the comparison |

`FM-NRC-01`: missing/wrong SDK or unsupported device -> preflight refusal without false active state. `FM-NRC-02`: invalid training/query records or non-finite prediction -> stop publication and diagnose the producer. `FM-NRC-03`: allocation/evaluation/device failure -> controlled operation failure; no automatic relabeling as SHaRC. `FM-NRC-04`: stale scene/context or shutdown while in flight -> invalidate and retire through real completion. Inject each at its owning boundary and retain the smallest counterexample.

A cache miss/declined termination continues uncached tracing under the algorithm; it is distinct from SDK failure. If SDK eligibility or evidence cannot be obtained, the required target remains BLOCKED, not completed by a stub or an exclusion. Verdicts belong in the candidate's `radiance-cache/nrc/` report under the existing workload artifact root.

## Check And Risk Ownership

| Check | Defect-detecting obligation |
| --- | --- |
| CHK-NRC-01 | vertex/event/throughput coupling and full-path controls; retain the associated AC and FM row identities in the candidate report. |
| CHK-NRC-02 | frozen updates, cold adaptation, invalid records and context/SDK faults; retain the associated AC and FM row identities in the candidate report. |
| CHK-NRC-03 | common-domain SHaRC/uncached frontier, native/package and reproduction; retain the associated AC and FM row identities in the candidate report. |

RISK-NRC-01: SDK eligibility, opaque internal cost and adaptation bias. The feature implementer and subject reviewer own mitigation: probe actual tuple and measure full online workload. A failed required falsifier/value band stops the stage; do not hide it by expanding scope or lowering a post-hoc threshold. The local acceptance table remains conjunctive, and the report records every supported API/domain cell separately.
