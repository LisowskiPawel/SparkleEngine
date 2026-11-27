# RHI: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Remove PTLAS and its exclusive vendor surface completely, retaining an explicit two-backend, multiqueue, parallel-recording RHI.

**Retain:** D3D12 and Vulkan, standard BLAS/TLAS/refit, per-worker command pools/allocators and contexts, queue submission/synchronization/retirement, shader targets, RenderDoc/PIX, DRED/Aftermath/Vulkan breadcrumbs and marker lifetime.

**Target flow:** Immutable pass inputs -> per-worker native recording contexts -> queue-owned submission/dependencies -> fences -> resource/descriptor/allocator retirement. RHI does not depend on Renderer.

Dominant execution owner for: RH-01, RH-02, RH-03, RH-04, RH-05, RH-06, RH-07, RH-08, RH-09, TP-05. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **372 files / 37,814 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **26 files / 2,332 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| RH-03 | 2 files / 264 lines |

## Stage 0: freeze the actual responsibility and smaller target

Trace every Ptlas descriptor/service/packer/provider through Renderer strategy, Editor settings and shader/build capabilities. Prove whether D3D12NvapiRayTracingProvider and acquired headers have any non-PTLAS consumer. List shared TLAS/BLAS descriptor/update helpers and native marker/context owners that must survive. Capture standard AS counts and skinned refit behavior.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Engine/RHI/Public/RayTracing/RhiPtlasService.h](../../../Engine/RHI/Public/RayTracing/RhiPtlasService.h)
- [Engine/RHI/Private/D3D12/RayTracing/D3D12NvapiRayTracingProvider.cpp](../../../Engine/RHI/Private/D3D12/RayTracing/D3D12NvapiRayTracingProvider.cpp)
- [Engine/Renderer/Private/RayTracing/Acceleration](../../../Engine/Renderer/Private/RayTracing/Acceleration)
- [Engine/Editor/Private/Settings/Rendering/RenderingPtlasSettingsSection.cpp](../../../Engine/Editor/Private/Settings/Rendering/RenderingPtlasSettingsSection.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Remove PTLAS vertical integration and its settings

Decisions: RH-03.

Delete PTLAS contracts, operation layout/packing, D3D12/Vulkan services, renderer partition/strategy/build state, capability/settings/UI controls and shader-side exclusive flags. Make standard AS behavior direct; remove alternate provider selection instead of keeping disabled stubs. Retain BLAS/TLAS builds/refits, barriers, multiqueue scheduling and concurrent native recording.

**Exit evidence:** No PTLAS admission/settings/commands/build membership remains. Standard static and skinned scenes load/render with both APIs and validation; parallel command contexts remain independent. Delayed GPU completion cannot reuse AS scratch/resources early. No backend-wide serialization or unconditional queue idle is introduced.

## Stage 2: Close the native NVAPI dependency chain

Decisions: TP-05.

When the last-consumer trace from stage 1 closes, remove the PTLAS-only native provider and NVAPI recipe/acquisition/cache identity/build inputs/notices that are no longer consumed. ThirdParty owns retained-library patch qualification, not this provider retirement. Keep Streamline/DLSS, Aftermath/DRED and generic NVIDIA debugger launch separate.

**Exit evidence:** Clean configure and both API targets do not require removed NVAPI/PTLAS inputs; final runtime dependency inventory excludes the retired provider. Existing source caches are not recursively purged as part of this source cut. Native GPU-crash symbols/markers remain usable with the new binary identity.

## Stage 3: Review the retained thread and queue boundary after cuts

Decisions: RH-01, RH-02, RH-04, RH-05, RH-06, RH-07, RH-08, RH-09.

Remove only adapters/derived state left without a consumer by the completed cuts. Explain native allocator/list ownership, descriptor lifetime, target mappings and each cross-queue dependency. Keep backend-specific conversions/workarounds with their backend and neutral invariants with RHI contracts. This stage has no authority to replace the owned RHI with a library or remove a native queue.

**Exit evidence:** Exercise parallel recording, more than one queue where the retained frame uses it, resize/reload/shutdown, captures and a controlled native fault. Record unexercised hardware/provider modes honestly. Reviewer can follow one resource from allocation through multiple submissions to deferred destruction on either API.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
