# GameFramework and scene runtime: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Replace general loading/resource orchestration with an owned small runtime scene path while preserving typed ECS, parallel work and safe game/render publication.

**Retain:** Basic Knight/Cesium skeleton playback and skinning, material variants, authored instance groups, typed ECS, immutable Editor read views, incremental render identity/publication and in-process asynchronous map switching. Separate game/render threads and data-oriented parallel ranges are mandatory.

**Target flow:** One prepared scene -> bounded parallel read/decode -> level-owned reusable resources and typed world -> parallel system ranges -> immutable Editor/render publication -> generation/fence retirement.

Dominant execution owner for: GF-01, GF-02, GF-03, GF-04, GF-05, GF-06, GF-07, GF-08, GF-09, GF-10, GF-11, GF-12, GF-15, GF-16, RV-02, RV-03.

Delegated complete systems: GF-13 -> [SceneCooker](SceneCooker.md); GF-14 -> [SceneCooker](SceneCooker.md). Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **219 files / 12,223 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **8 files / 355 lines**; assigned shared spans: **1 lines in 1 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| GF-06 | 8 files / 617 lines |
| GF-08 | 4 files / 614 lines |
| GF-09 | 7 files / 397 lines |
| GF-12 | 17 files / 1,092 lines |
| GF-15 | 16 files / 651 lines |
| GF-16 | 1 files / 159 lines |

## Stage 0: freeze the actual responsibility and smaller target

Trace SceneLoadExecutor/Run/PackageBuilder, SceneLoadWorldStaging, World/Resources, GameWorldSystems and immutable read-view builders to Renderer consumers. Record who owns world mutation, each task argument and resource handles. Inspect base/Curtains/Knight identity rebasing with SceneCooker before deleting runtime composition. Preserve read lifetime independently of whole-storage copy removal.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Engine/GameFramework/Private/Level/Loading/SceneLoadExecutor.cpp](../../../Engine/GameFramework/Private/Level/Loading/SceneLoadExecutor.cpp)
- [Engine/GameFramework/Private/Level/Loading/SceneLoadRun.cpp](../../../Engine/GameFramework/Private/Level/Loading/SceneLoadRun.cpp)
- [Engine/GameFramework/Private/World/Loading/SceneLoadWorldStaging.cpp](../../../Engine/GameFramework/Private/World/Loading/SceneLoadWorldStaging.cpp)
- [Engine/GameFramework/Private/World/Systems/GameWorldSystems.cpp](../../../Engine/GameFramework/Private/World/Systems/GameWorldSystems.cpp)
- [Engine/Renderer/Private/Scene/Preparation/RenderScenePreparationRanges.cpp](../../../Engine/Renderer/Private/Scene/Preparation/RenderScenePreparationRanges.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Remove morph targets throughout the pipeline

Decisions: GF-02.

Own the complete morph removal through SourceImporters, cooked scene/mesh contracts, world weight storage, Renderer payloads/shaders, animation weight channels, UI and build lists. Regenerate disposable cooked/shader output in place. Keep skeleton joints/weights, pose evaluation, GPU skinning, previous-frame deformation and RT acceleration updates. Do not remove a shared deformation file wholesale when skeletal consumers remain.

**Exit evidence:** Knight/Cesium import/cook/playback and moving geometry are correct on both APIs. Morph admission is explicitly unsupported; no morph-only fields/weights/registration/shader payloads or hidden default remain. Parallel animation/extraction jobs and array bounds remain valid.

## Stage 2: Replace loading/staging/resource orchestration as one lifecycle

Decisions: GF-06, GF-08, GF-09, GF-15.

Remove cross-pool per-asset chains, whole-world rollback staging and shared cross-scene store abstraction together. Use one load coordinator with bounded parallel asset/chunk jobs and level-owned deduplicated mesh/material/texture arrays. Keep the accepted-data boundary and explicit empty/loading failure state; whole previous-world rollback is not retained. Quiesce only readers of retiring generations; keep UI/loading-frame and independent workers running. Simplify world-system scheduling around data-oriented ranges without reducing parallel transforms/animation/extraction.

**Exit evidence:** Test cancel/reload/close at read, decode, world install and pending-render handoff. No partial world, use-after-free, deadlock, global recording lock or duplicated per-instance geometry. Knight/Bistro memory/load throughput and game/render overlap are measured. All retired coordinators/state/forwarders disappear with callers; a renamed layer stack is not completion.

## Stage 3: Simplify level description and immutable view publication

Decisions: GF-12, GF-16, GF-10, GF-11.

Consume the already-delivered one-scene runtime contract from SceneCooker stage 2; do not repeat its composition deletion. Narrow the remaining level parser/writer to one scene reference plus kept camera/light/sky and Editor-approved override fields. Maintain one grammar, owned representations and useful errors; no old/new schema versions. Reduce unnecessary whole-storage view clones with small immutable views/stable references or a proven smaller existing storage path. Keep one publication authority and safe borrowed lifetimes, not shared mutable view state.

**Exit evidence:** Corrupt/escaping/missing inputs fail before unsafe publication. Maintainer save/load works with Editor stage 3 and old disposable output is regenerated. A camera/property change has measured bounded copy work; no serial UI/render lock replaces it. Stable render identities and animated deltas survive switch/reload.

## Stage 4: Close reduced runtime path and explain ownership

Decisions: GF-01, GF-03, GF-04, GF-05, GF-07, RV-02, RV-03.

Consume SceneCooker-prepared Ozz runtime data and single prepared Knight scene; remove runtime offline-builder/old multi-asset adapters only in the stages owned by SceneCooker. Consolidate remaining neutral data contracts and component/resource iteration with current typed ECS. Keep intentionally retained material-variant/authored-instance capabilities even where current inputs do not exercise them.

**Exit evidence:** Trace a Knight joint and an edited object from cooked identity through game jobs, immutable view, render extraction, GPU use and retirement. Run Empty, Knight, CesiumMan and Bistro switching under both APIs with cancellation/failure. Publish net source/state/storage reduction; retained feature support is demonstrated or its qualification stays open.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
