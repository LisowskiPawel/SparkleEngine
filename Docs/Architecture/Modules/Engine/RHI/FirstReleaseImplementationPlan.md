# First Release RHI And GPU Execution Plan

[Docs](../../../../README.md) / [Up](RHIDesign.md)

Make D3D12 and Vulkan execute the same admitted GPU contracts and fail safely when requirements are missing.

**Status:** implementation plan; ordered backend closure, not native-validation or release evidence

**Snapshot:** 2026-09-08; portfolio projection **45/100**; all six `FCR-RHI-*` families are `Blocked`

## Work This Plan

| What to do | Working route |
| --- | --- |
| First action | [RHI-0 backend matrix](#rhi-0--freeze-the-service-and-backend-matrix): match each Renderer request to a real service and supported native tuple. |
| Result to reach | An explicit capability/service matrix and narrow native checks; later phases close resources, submission, ray tracing and device failure. |
| Start / stop | Do not infer parity from shared names. Missing native support or a required unavailable check blocks that cell; Renderer policy stays in Renderer. |

## `RHI-0` — Freeze The Service And Backend Matrix

**Goal:** reconcile every public RHI service and every Renderer consumer into an admitted D3D12/Vulkan/capability matrix before changing implementation.

**Non-goals:** making both backends textually identical, exposing native vocabulary to Renderer, or filling matrix cells with assumed support.

**Required work:**

- inventory public services, resource and handle types, descriptors, pipelines, command operations, queues, present, interop, capture, timing, ray tracing, and diagnostics;
- trace owner, lifetime, creation/destruction, producer/consumer, CMake membership, selector, requested/active state, unsupported route, and native lowering;
- map each service to feature-local `AC-RHI*`, `FM-RHI*`, and `CHK-RHI*`; repair the Architecture contract first if any admitted cell lacks one;
- freeze hardware/driver/toolchain/provider and feature-level prerequisites for the release candidate;
- classify divergence as shared invariant, intentionally backend-specific mechanism, unsupported, or defect.

**Failure modes to control:** public service has one backend only but reports support; implementation method has no consumer; Renderer branches on native API; invalid handle behavior is undefined; capability query differs from actual creation.

**Phase exit criteria:** every admitted service has a current source route, both-backend disposition, capability/failure behavior, and mapped check; unknown cells block later phases.

## `RHI-1` — Device, Resources, Descriptors, And Lifetime

**Goal:** close `FCR-RHI-01` and the resource/lifecycle base of `FCR-RHI-06` with one published backend aggregate and completion-safe ownership.

**Non-goals:** Renderer resource policy in RHI, a generic lifetime framework without consumers, or relying on process exit for cleanup.

**Required work:**

- preserve owner-thread aggregate creation/publication and reverse-order teardown; make partial-create cleanup explicit;
- close buffer/texture/view/sampler/descriptor/upload/readback/pipeline resource creation and invalid-input/allocation behavior for admitted uses;
- define handle/generation, native ownership, mapped memory, descriptor range, state initialization, debug naming, and queue-use retirement;
- make capability queries and creation failure agree; keep unsupported behavior explicit and safe;
- measure retained/high-water memory and descriptor usage for release workloads.

**Failure modes to control:** partial device publishes; descriptor out of bounds; freed resource remains in flight; mapped range misuse; allocation failure corrupts prior state; dependent outlives device; native object leaks.

**Phase exit criteria:** all admitted resource/descriptor/lifetime criteria pass on both backends; partial creation and allocation/invalid-handle failures settle cleanly; memory and live-object records are retained.

## `RHI-2` — Command Recording, Synchronization, Resize, And Present

**Goal:** close `FCR-RHI-02` from frame begin through recording leases, submission tokens, completion, resize, present, and retirement.

**Non-goals:** Renderer pass scheduling in RHI, unconditional queue waits, or treating a responsive process as a correct frame loop.

**Required work:**

- define frame/submission/completion identities, legal owner/thread states, command-list lease lifetime, batch ordering, queue dependencies, and serial control;
- close resource state/barrier lowering, signal/wait error propagation, completion-driven recycling, frames-in-flight bounds, and cancellation/shutdown settlement;
- handle zero-size/minimized window, swapchain out-of-date/suboptimal, resize generation, acquire/present failure, and device-loss separation;
- expose the native capability, compatible format/color-space activation, static metadata, recreation, and fallback mechanics consumed by Renderer `DSP-7`; `FCR-REN-26` retains HDR feature ownership;
- ensure Renderer graph order lowers without hidden RHI policy and native markers preserve causality;
- measure CPU submit/present, waits, pacing, and resource high-water without changing the production route.

**Failure modes to control:** token reused across generation; wait cycle; allocator/list reset before completion; resize destroys in-flight back buffer; present failure loops; incompatible HDR format/color-space or failed metadata leaves false active state; cancellation partially submits; shutdown hangs.

**Phase exit criteria:** serial and normal frame paths, resize/minimize/restore, queue ordering, failure, shutdown, and pacing checks pass on both backends with clean native validation.

## `RHI-3` — Paired Backend Lowering

**Goal:** close `FCR-RHI-03` by proving that common contracts produce equivalent declared results while backend-specific mechanics remain contained.

**Non-goals:** identical native command streams, hiding a real backend limitation, or branching feature semantics in backend code.

**Required work:**

- compare every admitted resource, descriptor, pipeline, command, state/barrier, presentation, interop, and diagnostic operation cell-by-cell;
- close missing lowering and normalize error/capability truth at the shared boundary without lowest-common-denominator leakage;
- validate descriptor lifetime, format/state mapping, queue family/ownership, swapchain differences, compiler failures, and debug naming;
- retain backend-specific algorithms only behind the common invariant and document why they differ;
- execute matching deterministic micro-oracles plus representative Renderer paths with D3D12 debug layer/DRED and Vulkan validation.

**Failure modes to control:** one backend silently no-ops; format/state maps differently; validation suppression hides defect; capability advertised before dependency; fallback changes feature result; backend fix changes shared semantics.

**Phase exit criteria:** every included matrix cell is `PASS` or explicitly excluded before scope freeze; deterministic outputs and failures agree within owned tolerances; native validation is clean or has an accepted, scoped disposition.

## `RHI-4` — Ray-Tracing And Acceleration-Structure Execution

**Goal:** close `FCR-RHI-04` for admitted BLAS/TLAS, update/refit/partitioned policy, inline query, native pipeline, shader table, and trace commands.

**Non-goals:** moving scene/TLAS selection policy out of Renderer, duplicating effect semantics for inline and RGS, or claiming unsupported hardware paths.

**Required work:**

- freeze geometry/instance/generation/SBT contribution identity and ownership across build, update, replacement, and completion;
- close scratch/result/operation-buffer sizing, bounds, state/synchronization, compaction/retirement as applicable, and allocation failure;
- keep one semantic Renderer effect contract with thin inline/native-pipeline traversal adapters;
- make requested/active capability and classic/refit/partitioned choices observable, including safe unsupported behavior;
- compare deterministic hit/miss/instance/material results and native validation across backends/routes; measure build/update/traversal and memory.

**Failure modes to control:** stale BLAS address; SBT index aliases instance; update used after topology change; operation buffer overrun; scratch freed early; unsupported path silently falls back; inline/RGS result diverges.

**Phase exit criteria:** all admitted ray criteria/failures pass for both APIs and traversal routes; identity and lifetime ledgers reconcile; costs and capability boundaries are recorded.

## `RHI-5` — Diagnostics, Capture, Device Loss, And Recovery

**Goal:** close `FCR-RHI-05` and `FCR-RHI-06` with bounded observation, truthful recoverable swapchain events, and terminal device-loss handling.

**Non-goals:** an internal performance-dashboard product, automatic device resurrection, unbounded logging, or collecting private content without intent.

**Required work:**

- close timing validity, message/drop state, memory/live objects, object naming, texture capture/readback/encoding, privacy, and external-tool correlation;
- ensure observers are bounded and selectable, report unavailable/invalid data, and are erased or disabled as the Shipping contract requires;
- distinguish resize/out-of-date recovery from terminal device loss; settle queues/dependents and prevent post-loss use;
- collect DRED or Vulkan fault/context data with candidate/frame/object identity and safe user-facing export;
- exercise partial create, queue wait failure, capture failure, device removal/loss, teardown, and repeated recoverable swapchain events.

**Failure modes to control:** invalid timestamp presented as zero; diagnostic ring silently overwrites; capture reads wrong generation; observer stalls frame; device loss retried forever; post-loss native call; diagnostics leak private path/data.

**Phase exit criteria:** feature checks pass on both APIs; recoverable versus terminal paths are unmistakable; native objects settle; diagnostic artifacts identify candidate/frame without privacy breach; external profiler handoff is demonstrated where applicable.

## `RHI-6` — Candidate Closure

**Goal:** bind all RHI claims to one package candidate, hardware/driver matrix, and paired-backend evidence index.

**Non-goals:** rerunning broad work without a missing claim or accepting one backend on behalf of the other.

**Required work:** audit all six FCR reports, run only missing cross-boundary and package-relative checks, verify native validation artifacts, confirm capability/selectors and exclusions, and check invalidation after any code/provider/driver/config change.

**Failure modes to control:** evidence uses different binary or shader/content; driver/tool version missing; backend failure hidden by auto-selection; unavailable hardware marked pass; debug-only route differs from Shipping package.

**Phase exit criteria:** each RHI FCR has a complete candidate-bound decision, paired matrices and limitations are explicit, and all Renderer dependencies have a stable accepted contract.

## Reference And Ownership

<details>
<summary>Exact owners, source snapshots and related contracts</summary>

**Responsibility:** sequence the neutral RHI contract, both native lowerings, GPU execution, diagnostics, and device-failure closure

**Orchestrator:** [First Release Implementation Plan](../../../CrossModule/FirstRelease/ImplementationPlan.md)

**Architecture owner:** [Render Hardware Interface](RHIDesign.md) and its [feature dossiers](Features/RHIFeatureGuide.md)

</details>

## Outcome And Sequence

Deliver one narrow Renderer-facing RHI whose D3D12 and Vulkan implementations truthfully lower the admitted resource, execution, presentation, ray-tracing, diagnostics, and failure contracts.

```mermaid
flowchart LR
    R0[RHI-0<br/>matrix and baseline] --> R1[RHI-1<br/>device and resources]
    R1 --> R2[RHI-2<br/>submission and present]
    R2 --> R3[RHI-3<br/>paired lowering]
    R3 --> R4[RHI-4<br/>ray execution]
    R4 --> R5[RHI-5<br/>diagnostics and loss]
    R5 --> R6[RHI-6<br/>candidate closure]
```

| Phase | Primary families | Required Architecture route |
| --- | --- | --- |
| `RHI-0` | all six | [Capability Inventory](CapabilityInventory.md), feature acceptance sections, Renderer consumers |
| `RHI-1` | `FCR-RHI-01`, resource portion of `06` | [Device And Resources](Features/DeviceAndResources/DeviceAndResourcesDesign.md) |
| `RHI-2` | `FCR-RHI-02` | [Command Submission](Features/PipelineAndExecution/PipelineAndExecutionDesign.md#command-submission-and-synchronization), [Presentation](Features/PresentationAndInterop/PresentationAndInteropDesign.md#presentation) |
| `RHI-3` | `FCR-RHI-03` | [Backend Selection And Capabilities](Features/DeviceAndResources/DeviceAndResourcesDesign.md#backend-selection-and-device-capabilities) plus all admitted contracts |
| `RHI-4` | `FCR-RHI-04` | [Ray Tracing](Features/PipelineAndExecution/PipelineAndExecutionDesign.md#ray-tracing) |
| `RHI-5` | `FCR-RHI-05`, `FCR-RHI-06` | [Diagnostics And Capture](Features/DiagnosticsAndCapture/DiagnosticsAndCaptureDesign.md), [Device Lifecycle](Features/DeviceAndResources/DeviceAndResourcesDesign.md#device-lifecycle-and-failure-recovery) |
| `RHI-6` | all six | candidate reports and package matrix |

Use the [common phase contract](../../../CrossModule/FirstRelease/ImplementationPlan.md#phase-card-contract). The [NVRHI Programming Guide](https://github.com/NVIDIA-RTX/NVRHI/blob/8e8c36e37558acec333204619b95d9d2fcdc4a79/doc/ProgrammingGuide.md), [DirectX Graphics Samples](https://github.com/microsoft/DirectX-Graphics-Samples), and [Khronos Vulkan Samples](https://github.com/KhronosGroup/Vulkan-Samples) provide reference questions and native oracles. They do not authorize a wrapper migration, API imitation, or claim of local parity.
