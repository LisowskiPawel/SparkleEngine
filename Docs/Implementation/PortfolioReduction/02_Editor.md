# Editor: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

**Execution order: 02 of 21 (first focused visit).** Start: Stages 0-2 in Wave 1. Resume / dependency: Stages 3-4 in Wave 6, with GameFramework stage 3 and its loading contract. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Keep a polished scene-inspection/editing frontend while deleting optional asset/shader tools and making the remaining state flow readable.

**Retain:** Outliner, live transforms/visibility/material variants/cameras/lights, render/effect controls, undo/redo and drag coalescing, console execution/history, compile/reload, screenshot/capture launch, preferences, custom title bar and existing generated icons. Maintainer saves persist approved fields; reviewer changes remain in-session.

**Target flow:** UI reads immutable world views -> typed edits reach the game-thread owner -> accepted state publishes to Renderer; explicit maintainer save writes the narrow level schema.

Dominant execution owner for: ED-01, ED-02, ED-03, ED-04, ED-05, ED-06, ED-07, ED-08, ED-09, ED-10, ED-11, ED-12, ED-13, ED-14, ED-15, ED-16, ED-17, ED-18, ED-19, ED-20, ED-21, ED-22, ED-23, ED-24. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **Remaining frontend:** after inspector/search/completion removal, narrow panel registration, selection/query contracts and frame data to the outliner, selected property controls, rendering options and console execution/history. Delete diagnostic-only request plumbing; do not retain a general inspector protocol with empty providers. Keep custom chrome/icons and the accepted controls.
- **One edit/save path:** immutable world observations feed the UI; typed edits are applied by the game owner. Maintainer persistence uses source-qualified identities and one narrow level writer. Reviewer edits stay in-session. Keep undo/coalescing and asynchronous preparation without copying the world or creating a second authoring model.

**Structural acceptance:** follow selection -> observation -> edit -> publication -> undo and maintainer save through real callers. Confirm package edits reset, no retired query/update work runs per frame, and added ED-23 code is charged to the footprint. Reject a UI-owned world mirror or a compatibility persistence layer.

**Reference application:** [P1](README.md#reference-choices-and-limits) informs frontend/private implementation separation, not an Unreal property/reflection or asset-editor framework. The local immutable-view and thread contracts remain authoritative.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **152 files / 12,005 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **24 files / 2,394 lines**; assigned shared spans: **35 lines in 2 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| ED-12 | 2 files / 49 lines |
| ED-13 | 2 files / 49 lines |
| ED-19 | 14 files / 1,293 lines |
| ED-24 | 2 files / 497 lines |

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| ED-12; stage 1 | Mesh inspection/preview UI and exclusive diagnostic producers. Close collectors/panel models/formatting and preview-only GPU buffers/shaders/textures/capture wiring through Application/Renderer; remove unused icon/menu/registration/build/deployment entries. Check shared diagnostic formatting against ED-13. | Ordinary meshes/materials, world extraction, retained signal views, native GPU captures and parallel upload/preparation. |
| ED-13; stage 1 | Texture inspection UI and exclusive diagnostic producers. Follow selected-texture state, thumbnail/preview exports, residency-formatting helpers, readback consumers and Application/Renderer APIs to their final users; remove orphan helpers after ED-12 too. | Actual texture residency/cache/upload, material/default textures, capture readback with retained users and runtime memory/lifetime correctness. |
| ED-14; stage 1 | Shader inspector/list/export frontend. Retire ShaderToolsPanel/ShaderArtifactInspection UI-only adapters, menu/status/preference fields and exclusive Application export APIs; coordinate with ShaderCompiler report removal and Reference exports. | Editor compile/reload, compiler failures, runtime reflection/ABI and exact debugger code/symbol/source artifacts. |
| ED-19; stage 2 | Per-property search/filter mechanism. Remove inspector query state, recursive property search/visibility metadata, filter callbacks/widgets and copied filtered views with no remaining user. | Outliner selection/search where separately retained, fixed property sections, useful text input and game-owned edits. |
| ED-20; stage 2 | Console completion functions/UI. Retire CompleteConsoleLine/CompleteConsoleCVarName, completion result/state/cursor paths and callback wiring in Core/Editor only when exclusive. | Console registration/execution/help/errors, history and CVar get/set; Launcher operation history is a different system. |
| ED-24; stage 3 | Reviewer-bundle scene writes and package authoring path. Close Save action/capability/writability derivation, path resolution and writer dispatch in shipped hosts so runtime edits cannot mutate supplied files. Delete any package-only persistence path after maintainer saves use one projection. | ED-22/23 explicit writable-source saves, live edits/undo, per-user settings and reload/reset; do not delete the shared level writer. |
| ED-23; stage 3 | Do not grow a second save/session/persistence framework while adding the approved overrides: consolidate source-qualified identity, projection and writer callers; remove obsolete save-field mirrors. | This is accepted new implementation work, not a deletion credit. Preserve correct identity, undo, explicit maintainer saves and reviewer in-session edits. |

## Stage 0: freeze the actual responsibility and smaller target

Trace AssetInspection and ShaderTools panels to their Application/Renderer producers before cutting. Separate console completion from console execution/history. For ED-23, inspect existing source/reimport identity and LevelWorldProjection: Save All currently saves lighting/sky/active-camera description, not every object edit. Freeze screenshots and a transform/visibility/material-variant edit/reload example.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Engine/Editor/Private/AssetInspection](../../../Engine/Editor/Private/AssetInspection)
- [Engine/Editor/Private/Viewport](../../../Engine/Editor/Private/Viewport)
- [Engine/GameFramework/Private/Level/LevelWorldProjection.cpp](../../../Engine/GameFramework/Private/Level/LevelWorldProjection.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Implementation prompt:**

```text
Prepare Stage 0 of Docs/Implementation/PortfolioReduction/02_Editor.md: freeze the actual responsibility and smaller target.
Read this module's outcome, retained contracts, source entry points and Stage 0 questions; read Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md, Docs/Roadmap.md and Docs/Implementation/PortfolioReduction/README.md. Refresh current HEAD/working-tree identities and respect concurrent work. Audit the named retirement closure/participation in this plan, including narrowed retained capabilities; prepare a transitive retire/retain-with-consumer/unresolved manifest and rerun it after each layer disappears. Do not implement the production refactor in this stage.

Acceptance criteria:
- Specify this plan's Adapt the surviving design contract: vanished assumption, concrete smaller API/data/control flow, caller changes, selected reference lesson and local limits. Budget all replacement/caller code and name the retained behavior/failure check.
- an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.
- Map each proposed changed function to one cohesive responsibility/owner and an intent-based name; distinguish workflow orchestration from implementation mechanisms. Specify callers/contracts/files/build outputs to retire and the sole target path; no legacy/backward-compatibility reader, alias, migration, disabled stub or old/new implementation may be planned.
- Define stage-specific success, invalid-input/failure/cancellation/lifetime checks and measured bounds before coding; retain all approved parallel/data-oriented/game/render/RHI contracts. No unsupported performance or qualification claim.

Failure modes to reject:
- The feature disappears but surviving contracts retain obsolete generality, or an upstream framework is copied without a local invariant.
- Historical Editor paths, footprint counts or old receipts are treated as current without tracing actual callers/build inputs.
- Mixed retained/retired responsibilities or CPU/GPU reader lifetimes are omitted, leaving no safe complete-system cut or measurable replacement budget.
- A generic wrapper/framework, serial execution, retained compatibility path or source relocation is substituted for a smaller complete design. A first-order-only dependency scan is insufficient.

Deliver the concrete design/ownership map, bounded change and net footprint budget, planned checks and open conditions in ignored stage evidence. Keep CodeStyle's function/namespace/layout rules explicit in the design; permanent test-only additions are not authorized. Do not close an unproven implementation stage or change a recorded decision.
```

## Stage 1: Delete mesh, texture and shader inspection subsystems

Decisions: ED-12, ED-13, ED-14.

**Implementation prompt:**

```text
Implement Stage 1 of Docs/Implementation/PortfolioReduction/02_Editor.md: Delete mesh, texture and shader inspection subsystems.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Remove panels, preview rendering/tool actions, selection models and their exclusive diagnostic collectors. Follow dependencies into Application/Renderer/Core and remove now-unconsumed producers, registrations and includes in the same stage. Renderer-owned Reference overlay removal belongs to Renderer stage 1. Keep ordinary runtime mesh/texture/shader data, residency, useful errors, signal views and external native captures.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Editor starts and renders retained maps without retired panels/preview paths. Compile/reload and native capture still work. Search confirms old panel producers have no retained consumer; removal does not serialize asset preparation or damage GPU resource lifetime. Build after regenerating source membership.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- An optional panel survives through hidden actions/collectors or retired build membership.
- Removing its producer breaks retained signal views, shader reload, asynchronous residency or native capture.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 2: Remove property search and console completion

Decisions: ED-19, ED-20.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/02_Editor.md: Remove property search and console completion.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Delete per-property query plumbing and CompleteConsoleLine/CompleteConsoleCVarName plus completion-only UI/state. Retain fixed property sections, outliner selection, input parsing, command/CVar execution, console history, help and useful failures. Remove redundant view/filter copies rather than replacing search with another generic query layer.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Fixed inspectors still expose every kept control. Commands and CVars execute, retain history and report errors; completion/search UI and exclusive producers are gone. Retained immutable view and game-thread mutation boundaries pass focused edit checks.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Search/completion removal also deletes console history, command execution or kept properties.
- Fixed inspectors cannot expose retained controls, or edits bypass game-thread ownership.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 3: Bound maintainer persistence and forbid package authoring

Decisions: ED-22, ED-23, ED-24.

**Implementation prompt:**

```text
Implement Stage 3 of Docs/Implementation/PortfolioReduction/02_Editor.md: Bound maintainer persistence and forbid package authoring.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
With GameFramework stage 3, retain one explicit save projection and small writer for camera/light/sky plus approved per-object transform/visibility/material overrides. Reuse source-qualified cooked node/object identity; runtime EntityId or pointer values are not persistent keys. Reject/reset overrides when source identity no longer matches. Keep supplied reviewer data read-only through product capability/writability; reviewer edits stay in-session. No sidecar authoring database, compatibility migration or new user-role framework.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- An explicit maintainer save/reload restores the selected fields and undo semantics remain correct. Reordered/replaced source identities cannot silently edit another object. Failed save preserves accepted document bytes. A relocated reviewer bundle supports all live edits but writes no scene data; reload restores defaults. Record ED-23 added lines separately: this approved new behavior is not deletion savings.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Saved runtime IDs/indices silently target another object after source changes.
- Reviewer edits modify supplied scene files, or a failed maintainer save corrupts accepted data.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 4: Consolidate retained frontend ownership

Decisions: ED-01, ED-08, ED-10, ED-17, ED-18.

**Implementation prompt:**

```text
Implement Stage 4 of Docs/Implementation/PortfolioReduction/02_Editor.md: Consolidate retained frontend ownership.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Collapse forwarding-only controllers and duplicate derived view state exposed by the cuts, using existing Editor composition owners. Keep custom title/hit testing, icon catalog/lifetime and the working console, preferences and undo command set. Coordinate GF-09 loading/empty failure UI without adding a second world-lifecycle owner.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Native window interaction, DPI, icons, selection/undo and in-process map switching work on both APIs. UI observes snapshots and edits execute on the world owner; no render/global-world lock is introduced. Specialist can trace one edit and one saved property without walking obsolete diagnostic layers.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Wrapper cleanup changes the retained title bar/icons/DPI interaction or drops undo coalescing.
- Map failure exposes a partial world or a new UI/render lock serializes observations and edits.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
