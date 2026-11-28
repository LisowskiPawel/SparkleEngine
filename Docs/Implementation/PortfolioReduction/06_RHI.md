# RHI: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

**Execution order: 06 of 21 (first focused visit).** Start: Stages 0-2 in Wave 2. Resume / dependency: Stage 3 in Wave 7 after final renderer and selected build changes. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Remove PTLAS and its exclusive vendor surface completely, retaining an explicit two-backend, multiqueue, parallel-recording RHI.

**Retain:** D3D12 and Vulkan, standard BLAS/TLAS/refit, per-worker command pools/allocators and contexts, queue submission/synchronization/retirement, shader targets, RenderDoc/PIX, DRED/Aftermath/Vulkan breadcrumbs and marker lifetime.

**Target flow:** Immutable pass inputs -> per-worker native recording contexts -> queue-owned submission/dependencies -> fences -> resource/descriptor/allocator retirement. RHI does not depend on Renderer.

Dominant execution owner for: RH-01, RH-02, RH-03, RH-04, RH-05, RH-06, RH-07, RH-08, RH-09, TP-05. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **Standard acceleration structures:** after PTLAS retirement, reshape public descriptors, capability records, update/build requests and both backend translations around retained BLAS/TLAS/refit. Remove provider tags, partition data and fallback dispatch used only by PTLAS; update Renderer callers directly. Shader-target enumeration and both graphics APIs are intentional surviving alternatives.
- **Native ownership stays visible:** after the exclusive NVAPI route is gone, connect each retained SDK at its actual consumer. Keep per-worker recording contexts, queue-specific submission, explicit dependencies and fence retirement. Trim only synchronization/state with no retained lifetime; semantic breadcrumbs and native fault completion are retained consumers.

**Structural acceptance:** follow static and skinned TLAS build/refit through each API, including concurrent recording, cross-queue use and delayed retirement; show the reduced neutral/backend API and SDK closure. Reject an empty generic provider layer, a global recording lock or removal of shared Aftermath/Streamline inputs.

**Reference application:** [P3](README.md#reference-choices-and-limits) informs explicit backend/SDK targets. Keep Sparkle's owned RHI and lifetime semantics; no NVRHI substitution or copied optional-provider matrix.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **372 files / 37,814 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **26 files / 2,332 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| RH-03 | 2 files / 264 lines |

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| RH-03; stage 1 | PTLAS services/contracts/packing and renderer strategies. Close RHI descriptors/limits/feature capability, D3D12/Vulkan calls/extension negotiation/function pointers, Renderer partition state/build strategies, Editor/settings/CVar persistence and shader/cook/build providers. Delete exclusive helpers after this chain is gone. | Standard BLAS/TLAS/refit and their shared geometry/scratch/sync rules; retained Vulkan/D3D12 functions, multiple queues and per-worker contexts. |
| TP-05; stage 2 | PTLAS-exclusive native NVAPI integration. Trace ThirdParty/nvidia-nvapi/Dependency.cmake, SparkleNVAPI imported target, SPARKLE_RHI_D3D12_NVAPI_* options, FetchDependencies, SourceDependencyFacts/SourceDependencyState, identity headers/libs, deployment/source-package metadata/notices and provider initialization/shutdown. | Aftermath, Streamline/DLSS, PIX/RenderDoc and standard DXR remain; retire SDK only when all real consumers are gone. |

## Retained neutral and native vocabulary

Neutral APIs use `RenderPipeline`, `GraphicsPipelineDesc/ComputePipelineDesc`, `Create*Pipeline`, `SetPipeline`, `renderTarget(s)` and `depthStencil`; PSO/RTV/DSV stay native. Use `GpuMesh/GpuMeshCache` and `commandContext`, not `GPU*/cmd`. `WorkspaceRoot` includes extracted products; `RepositoryRoot` refers to source/provenance.

## Stage 0: freeze the actual responsibility and smaller target

Trace every Ptlas descriptor/service/packer/provider through Renderer strategy, Editor settings and shader/build capabilities. Prove whether D3D12NvapiRayTracingProvider and acquired headers have any non-PTLAS consumer. List shared TLAS/BLAS descriptor/update helpers and native marker/context owners that must survive. Capture standard AS counts and skinned refit behavior.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Engine/RHI/Public/RayTracing/RhiPtlasService.h](../../../Engine/RHI/Public/RayTracing/RhiPtlasService.h)
- [Engine/RHI/Private/D3D12/RayTracing/D3D12NvapiRayTracingProvider.cpp](../../../Engine/RHI/Private/D3D12/RayTracing/D3D12NvapiRayTracingProvider.cpp)
- [Engine/Renderer/Private/RayTracing/Acceleration](../../../Engine/Renderer/Private/RayTracing/Acceleration)
- [Engine/Editor/Private/Settings/Rendering/RenderingPtlasSettingsSection.cpp](../../../Engine/Editor/Private/Settings/Rendering/RenderingPtlasSettingsSection.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Implementation prompt:**

```text
Prepare Stage 0 of Docs/Implementation/PortfolioReduction/06_RHI.md: freeze the actual responsibility and smaller target.
Read this module's outcome, retained contracts, source entry points and Stage 0 questions; read Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md, Docs/Roadmap.md and Docs/Implementation/PortfolioReduction/README.md. Refresh current HEAD/working-tree identities and respect concurrent work. Audit the named retirement closure/participation in this plan, including narrowed retained capabilities; prepare a transitive retire/retain-with-consumer/unresolved manifest and rerun it after each layer disappears. Do not implement the production refactor in this stage.

Acceptance criteria:
- Specify this plan's Adapt the surviving design contract: vanished assumption, concrete smaller API/data/control flow, caller changes, selected reference lesson and local limits. Budget all replacement/caller code and name the retained behavior/failure check.
- an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.
- Map each proposed changed function to one cohesive responsibility/owner and an intent-based name; distinguish workflow orchestration from implementation mechanisms. Specify callers/contracts/files/build outputs to retire and the sole target path; no legacy/backward-compatibility reader, alias, migration, disabled stub or old/new implementation may be planned.
- Define stage-specific success, invalid-input/failure/cancellation/lifetime checks and measured bounds before coding; retain all approved parallel/data-oriented/game/render/RHI contracts. No unsupported performance or qualification claim.

Failure modes to reject:
- The feature disappears but surviving contracts retain obsolete generality, or an upstream framework is copied without a local invariant.
- Historical RHI paths, footprint counts or old receipts are treated as current without tracing actual callers/build inputs.
- Mixed retained/retired responsibilities or CPU/GPU reader lifetimes are omitted, leaving no safe complete-system cut or measurable replacement budget.
- A generic wrapper/framework, serial execution, retained compatibility path or source relocation is substituted for a smaller complete design. A first-order-only dependency scan is insufficient.

Deliver the concrete design/ownership map, bounded change and net footprint budget, planned checks and open conditions in ignored stage evidence. Keep CodeStyle's function/namespace/layout rules explicit in the design; permanent test-only additions are not authorized. Do not close an unproven implementation stage or change a recorded decision.
```

## Stage 1: Remove PTLAS vertical integration and its settings

Decisions: RH-03.

**Implementation prompt:**

```text
Implement Stage 1 of Docs/Implementation/PortfolioReduction/06_RHI.md: Remove PTLAS vertical integration and its settings.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Delete PTLAS contracts, operation layout/packing, D3D12/Vulkan services, renderer partition/strategy/build state, capability/settings/UI controls and shader-side exclusive flags. Make standard AS behavior direct; remove alternate provider selection instead of keeping disabled stubs. Retain BLAS/TLAS builds/refits, barriers, multiqueue scheduling and concurrent native recording.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- No PTLAS admission/settings/commands/build membership remains. Standard static and skinned scenes load/render with both APIs and validation; parallel command contexts remain independent. Delayed GPU completion cannot reuse AS scratch/resources early. No backend-wide serialization or unconditional queue idle is introduced.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- PTLAS calls/settings/provider selection survive as disabled branches or an old cooked/shader capability.
- Standard BLAS/TLAS/refit, per-worker command contexts, queue synchronization or scratch retirement is damaged.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 2: Close the native NVAPI dependency chain

Decisions: TP-05.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/06_RHI.md: Close the native NVAPI dependency chain.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
When the last-consumer trace from stage 1 closes, remove the PTLAS-only native provider, SparkleNVAPI target, SPARKLE_RHI_D3D12_NVAPI_* options, dependency-fact/generated-header/catalog entries and NVAPI recipe/acquisition/cache identity/build inputs/notices that are no longer consumed. ThirdParty owns retained-library patch qualification, not this provider retirement. Keep Streamline/DLSS, Aftermath/DRED and generic NVIDIA debugger launch separate.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Clean configure and both API targets do not require removed NVAPI/PTLAS inputs; final runtime dependency inventory excludes the retired provider. Existing source caches are not recursively purged as part of this source cut. Native GPU-crash symbols/markers remain usable with the new binary identity.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- NVAPI is removed before its actual last consumer, or acquisition/build/notices still expect it.
- The cut accidentally removes Aftermath, Streamline/DLSS, standard DXR or generic NVIDIA tool launch.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 3: Review the retained thread and queue boundary after cuts

Decisions: RH-01, RH-02, RH-04, RH-05, RH-06, RH-07, RH-08, RH-09.

**Implementation prompt:**

```text
Implement Stage 3 of Docs/Implementation/PortfolioReduction/06_RHI.md: Review the retained thread and queue boundary after cuts.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Remove only adapters/derived state left without a consumer by the completed cuts. Explain native allocator/list ownership, descriptor lifetime, target mappings and each cross-queue dependency. Keep backend-specific conversions/workarounds with their backend and neutral invariants with RHI contracts. This stage has no authority to replace the owned RHI with a library or remove a native queue.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Exercise parallel recording, more than one queue where the retained frame uses it, resize/reload/shutdown, captures and a controlled native fault. Record unexercised hardware/provider modes honestly. Reviewer can follow one resource from allocation through multiple submissions to deferred destruction on either API.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- A helper consolidation serializes command recording/submission or reuses descriptors/allocators before every queue completes.
- Markers, target mappings or native diagnostics become unusable although compilation still passes.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
