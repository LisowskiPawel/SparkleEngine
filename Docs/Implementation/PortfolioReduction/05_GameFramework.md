# GameFramework and scene runtime: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

**Execution order: 05 of 21 (first focused visit).** Start: Stages 0-1 in Wave 2. Resume / dependency: Stages 2-3 in Wave 6 after prepared scenes/animation; stage 4 in Wave 7 after Application. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Replace general loading/resource orchestration with an owned small runtime scene path while preserving typed ECS, parallel work and safe game/render publication.

**Retain:** Basic Knight/Cesium skeleton playback and skinning, material variants, authored instance groups, typed ECS, immutable Editor read views, incremental render identity/publication and in-process asynchronous map switching. Separate game/render threads and data-oriented parallel ranges are mandatory.

**Target flow:** One prepared scene -> bounded parallel read/decode -> level-owned reusable resources and typed world -> parallel system ranges -> immutable Editor/render publication -> generation/fence retirement.

Dominant execution owner for: GF-01, GF-02, GF-03, GF-04, GF-05, GF-06, GF-07, GF-08, GF-09, GF-10, GF-11, GF-12, GF-15, GF-16, RV-02, RV-03.

Delegated complete systems: GF-13 -> [SceneCooker](14_SceneCooker.md); GF-14 -> [SceneCooker](14_SceneCooker.md). Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **One level lifecycle:** replace cross-pool chains, replacement-world staging and shared cross-scene stores together with one coordinator, bounded parallel asset jobs and level-owned reusable arrays/handles. Remove obsolete commit/rollback, store-acquisition and composition protocols from the surviving callers. An empty/loading failure state does not remove partial-load cleanup, admission bounds, physical settlement or generation/fence retirement.
- **Smaller world contracts:** one prepared scene feeds typed ECS and parallel transform/animation/extraction ranges. Move Ozz offline preparation and Knight source composition to SceneCooker, then remove runtime-only preparation arguments/intermediates. Narrow the existing level grammar and published read-view contents; preserve immutable lifetime and incremental resource/identity publication without whole-storage copying where safe.

**Structural acceptance:** draw the sole load -> world -> Editor/render handoff and teardown flow; exercise rapid switch/cancel/failure and moving Knight/Cesium. Show fewer state holders and task adapters with bounded memory and retained parallel overlap. A one-item runtime composer, disguised staging world or mutable shared read view fails.

**Reference application:** [P1](README.md#reference-choices-and-limits) informs ownership boundaries; the target storage/load/thread design follows Sparkle's accepted decisions, not an imported gameplay/ECS framework.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

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

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| GF-02; stage 1 | Morph targets/weights/animation support. Trace imported/cooked schema fields, mesh/scene serialization/validators, blend-weight jobs/storage, components/views/deltas, shader bindings/layouts/includes/registrations and debug/UI/CVar fields. Remove exclusive adapters and regenerate disposable contracts. | Skeletal joint/weight/pose/skinning and previous-frame/RT geometry data; shared deformation helpers stay while used. |
| GF-06; stage 2 | Unnecessary world-system scheduling/orchestration machinery. Retire duplicate compiled system graphs/invoker wrappers/state mirrors only where the direct data-oriented range/dependency owner replaces them. Check Application scheduler/task adapters against frame, loading and Launcher consumers. | Parallel transforms/animation/extraction, deterministic merges, typed ECS, native task dependencies and separate game/render threads. |
| GF-08; stage 2 | Cross-pool per-asset read/decode handoff machinery. Close SceneLoadRun/WorkState/PackageBuilder chains, chained task/callback/result records, I/O-to-background adapters and duplicate settlement state. Re-audit each task extension after all domain users migrate. | One load coordinator with bounded parallel I/O/decode jobs, physical settlement, owned source bytes, cancellation and failure cleanup. |
| GF-09; stage 2 | Whole-world staging/rollback transaction. Retire SceneLoadWorldStaging, staged-world generation/commit/rollback wrappers, old-world preservation callbacks, duplicate stores/snapshots and Editor status branches unique to restoration. | Empty/loading failure behavior, partial-construction cleanup, stale-generation checks with live concurrent readers, immutable handoff and fence-based retirement. |
| GF-15; stage 2 | Shared cross-scene resource-store abstraction. Retire global/cross-level store maps, ownership/refcount adapters, resource-key mirrors and acquisition/release callbacks superseded by level-owned arrays. Check Renderer caches/handles and async upload readers before removing shared abstractions. | Within-level mesh/material/texture deduplication, stable handles, data-oriented parallel access and delayed CPU/GPU resource destruction. |
| GF-16; stage 3 | Unnecessary whole-storage read-view clones. Replace the actual clone/copy expression and duplicate snapshot backing only after tracing borrowed Editor and Renderer readers, component spans, invalidation and generation lifetimes. | Immutable publication with stable references/ranges; the whole containing view/store is not a deletion quote and no racing mutable fallback is allowed. |
| GF-12; stage 3 | Remove the general level grammar/intermediate parser models, plural composition aliases, duplicated dispatch/writer/projection fields and obsolete defaults while implementing the single prepared-scene contract. | Retain camera/light/sky and approved per-object maintainer fields, one writer, precise errors and safe parallel publication. |

## Stage 0: freeze the actual responsibility and smaller target

Trace SceneLoadExecutor/Run/PackageBuilder, SceneLoadWorldStaging, World/Resources, GameWorldSystems and immutable read-view builders to Renderer consumers. Record who owns world mutation, each task argument and resource handles. Inspect base/Curtains/Knight identity rebasing with SceneCooker before deleting runtime composition. Preserve read lifetime independently of whole-storage copy removal.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Engine/GameFramework/Private/Level/Loading/SceneLoadExecutor.cpp](../../../Engine/GameFramework/Private/Level/Loading/SceneLoadExecutor.cpp)
- [Engine/GameFramework/Private/Level/Loading/SceneLoadRun.cpp](../../../Engine/GameFramework/Private/Level/Loading/SceneLoadRun.cpp)
- [Engine/GameFramework/Private/World/Loading/SceneLoadWorldStaging.cpp](../../../Engine/GameFramework/Private/World/Loading/SceneLoadWorldStaging.cpp)
- [Engine/GameFramework/Private/World/Systems/GameWorldSystems.cpp](../../../Engine/GameFramework/Private/World/Systems/GameWorldSystems.cpp)
- [Engine/Renderer/Private/Scene/Preparation/RenderScenePreparationRanges.cpp](../../../Engine/Renderer/Private/Scene/Preparation/RenderScenePreparationRanges.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Implementation prompt:**

```text
Prepare Stage 0 of Docs/Implementation/PortfolioReduction/05_GameFramework.md: freeze the actual responsibility and smaller target.
Read this module's outcome, retained contracts, source entry points and Stage 0 questions; read Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md, Docs/Roadmap.md and Docs/Implementation/PortfolioReduction/README.md. Refresh current HEAD/working-tree identities and respect concurrent work. Audit the named retirement closure/participation in this plan, including narrowed retained capabilities; prepare a transitive retire/retain-with-consumer/unresolved manifest and rerun it after each layer disappears. Do not implement the production refactor in this stage.

Acceptance criteria:
- Specify this plan's Adapt the surviving design contract: vanished assumption, concrete smaller API/data/control flow, caller changes, selected reference lesson and local limits. Budget all replacement/caller code and name the retained behavior/failure check.
- an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.
- Map each proposed changed function to one cohesive responsibility/owner and an intent-based name; distinguish workflow orchestration from implementation mechanisms. Specify callers/contracts/files/build outputs to retire and the sole target path; no legacy/backward-compatibility reader, alias, migration, disabled stub or old/new implementation may be planned.
- Define stage-specific success, invalid-input/failure/cancellation/lifetime checks and measured bounds before coding; retain all approved parallel/data-oriented/game/render/RHI contracts. No unsupported performance or qualification claim.

Failure modes to reject:
- The feature disappears but surviving contracts retain obsolete generality, or an upstream framework is copied without a local invariant.
- Historical GameFramework paths, footprint counts or old receipts are treated as current without tracing actual callers/build inputs.
- Mixed retained/retired responsibilities or CPU/GPU reader lifetimes are omitted, leaving no safe complete-system cut or measurable replacement budget.
- A generic wrapper/framework, serial execution, retained compatibility path or source relocation is substituted for a smaller complete design. A first-order-only dependency scan is insufficient.

Deliver the concrete design/ownership map, bounded change and net footprint budget, planned checks and open conditions in ignored stage evidence. Keep CodeStyle's function/namespace/layout rules explicit in the design; permanent test-only additions are not authorized. Do not close an unproven implementation stage or change a recorded decision.
```

## Stage 1: Remove morph targets throughout the pipeline

Decisions: GF-02.

**Implementation prompt:**

```text
Implement Stage 1 of Docs/Implementation/PortfolioReduction/05_GameFramework.md: Remove morph targets throughout the pipeline.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Own the complete morph removal through SourceImporters, cooked scene/mesh contracts, world weight storage, Renderer payloads/shaders, animation weight channels, UI and build lists. Regenerate disposable cooked/shader output in place. Keep skeleton joints/weights, pose evaluation, GPU skinning, previous-frame deformation and RT acceleration updates. Do not remove a shared deformation file wholesale when skeletal consumers remain.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Knight/Cesium import/cook/playback and moving geometry are correct on both APIs. Morph admission is explicitly unsupported; no morph-only fields/weights/registration/shader payloads or hidden default remain. Parallel animation/extraction jobs and array bounds remain valid.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Morph removal deletes shared skeletal weights, pose/previous-frame data or skinning code.
- Old morph payloads/shaders still cook/load, or the new representation is accepted only through a legacy reader.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 2: Replace loading/staging/resource orchestration as one lifecycle

Decisions: GF-06, GF-08, GF-09, GF-15.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/05_GameFramework.md: Replace loading/staging/resource orchestration as one lifecycle.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Remove cross-pool per-asset chains, whole-world rollback staging and shared cross-scene store abstraction together. Use one load coordinator with bounded parallel asset/chunk jobs and level-owned deduplicated mesh/material/texture arrays. Keep the accepted-data boundary and explicit empty/loading failure state; whole previous-world rollback is not retained. Quiesce only readers of retiring generations; keep UI/loading-frame and independent workers running. Simplify world-system scheduling around data-oriented ranges without reducing parallel transforms/animation/extraction.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Test cancel/reload/close at read, decode, world install and pending-render handoff. No partial world, use-after-free, deadlock, global recording lock or duplicated per-instance geometry. Knight/Bistro memory/load throughput and game/render overlap are measured. All retired coordinators/state/forwarders disappear with callers; a renamed layer stack is not completion.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- A single load owner becomes a serial asset loop or world jobs lose their parallel ranges.
- World/resource cleanup frees live CPU/GPU references, duplicates geometry per instance or stops the whole scheduler.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 3: Simplify level description and immutable view publication

Decisions: GF-12, GF-16, GF-10, GF-11.

**Implementation prompt:**

```text
Implement Stage 3 of Docs/Implementation/PortfolioReduction/05_GameFramework.md: Simplify level description and immutable view publication.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Consume the already-delivered one-scene runtime contract from SceneCooker stage 2; do not repeat its composition deletion. Narrow the remaining level parser/writer to one scene reference plus kept camera/light/sky and Editor-approved override fields. Maintain one grammar, owned representations and useful errors; no old/new schema versions. Reduce unnecessary whole-storage view clones with small immutable views/stable references or a proven smaller existing storage path. Keep one publication authority and safe borrowed lifetimes, not shared mutable view state.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Corrupt/escaping/missing inputs fail before unsafe publication. Maintainer save/load works with Editor stage 3 and old disposable output is regenerated. A camera/property change has measured bounded copy work; no serial UI/render lock replaces it. Stable render identities and animated deltas survive switch/reload.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Parser/view simplification preserves two grammars/storage copies or breaks approved saved fields.
- A smaller view races mutable world storage, loses render identity, or serializes Editor/render readers.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 4: Close reduced runtime path and explain ownership

Decisions: GF-01, GF-03, GF-04, GF-05, GF-07, RV-02, RV-03.

**Implementation prompt:**

```text
Implement Stage 4 of Docs/Implementation/PortfolioReduction/05_GameFramework.md: Close reduced runtime path and explain ownership.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Consume SceneCooker-prepared Ozz runtime data and single prepared Knight scene; remove runtime offline-builder/old multi-asset adapters only in the stages owned by SceneCooker. Consolidate remaining neutral data contracts and component/resource iteration with current typed ECS. Keep intentionally retained material-variant/authored-instance capabilities even where current inputs do not exercise them.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Trace a Knight joint and an edited object from cooked identity through game jobs, immutable view, render extraction, GPU use and retirement. Run Empty, Knight, CesiumMan and Bistro switching under both APIs with cancellation/failure. Publish net source/state/storage reduction; retained feature support is demonstrated or its qualification stays open.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Runtime still builds offline Ozz data or follows hidden multi-asset fallback routes.
- Knight animation/RT updates fail, or kept variants/instance capability are silently removed because current content lacks them.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
