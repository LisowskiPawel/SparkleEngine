# Editor: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Use the [shared stage protocol](README.md#stage-protocol) and [frozen design and check inputs](../../../build/validation/portfolio-editor-stage0-20261128/REPORT.md). The remaining gates are listed in execution order.

**Execution order: 02 of 21 (first focused visit).** Continue with Stages 1?2 in Wave 1. Resume / dependency: Stages 3-4 in Wave 6, with GameFramework stage 3 and its loading contract. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Keep a polished scene-inspection and editing frontend with a direct path from immutable observations to game-owned edits and approved persistence.

**Retain:** Outliner, live transforms/visibility/material variants/cameras/lights, render/effect controls, undo/redo and drag coalescing, console execution/history, compile/reload, screenshot/capture launch, preferences, custom title bar and existing generated icons. Maintainer saves persist approved fields; reviewer changes remain in-session.

**Target flow:** UI reads immutable world views -> typed edits reach the game-thread owner -> accepted state publishes to Renderer; explicit maintainer save writes the narrow level schema.

Dominant execution owner for: ED-01, ED-02, ED-03, ED-04, ED-05, ED-06, ED-07, ED-08, ED-09, ED-10, ED-11, ED-12, ED-13, ED-14, ED-15, ED-16, ED-17, ED-18, ED-19, ED-20, ED-21, ED-22, ED-23, ED-24. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **Remaining frontend:** narrow selection/query contracts and frame data to the outliner, selected property controls, rendering options and console execution/history. Keep custom chrome/icons and the accepted controls.
- **One edit/save path:** immutable world observations feed the UI; typed edits are applied by the game owner. Maintainer persistence uses source-qualified identities and one narrow level writer. Reviewer edits stay in-session. Keep undo/coalescing and asynchronous preparation without copying the world or creating a second authoring model.

**Structural acceptance:** follow selection -> observation -> edit -> publication -> undo and maintainer save through real callers. Confirm package edits reset, no retired query/update work runs per frame, and added ED-23 code is charged to the footprint. Reject a UI-owned world mirror or a compatibility persistence layer.

**Reference application:** [P1](README.md#reference-choices-and-limits) informs frontend/private implementation separation, not an Unreal property/reflection or asset-editor framework. The local immutable-view and thread contracts remain authoritative.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **152 files / 12,005 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **24 files / 2,394 lines**; assigned shared spans: **35 lines in 2 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| ED-24 | 2 files / 497 lines |

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| ED-24; stage 3 | Reviewer-bundle scene writes and package authoring path. Close Save action/capability/writability derivation, path resolution and writer dispatch in shipped hosts so runtime edits cannot mutate supplied files. Delete any package-only persistence path after maintainer saves use one projection. | ED-22/23 explicit writable-source saves, live edits/undo, per-user settings and reload/reset; do not delete the shared level writer. |
| ED-23; stage 3 | Do not grow a second save/session/persistence framework while adding the approved overrides: consolidate source-qualified identity, projection and writer callers; remove obsolete save-field mirrors. | This is accepted new implementation work, not a deletion credit. Preserve correct identity, undo, explicit maintainer saves and reviewer in-session edits. |

## Stage 1: Delete mesh, texture and shader inspection subsystems

Decisions: ED-12, ED-13, ED-14. The [retirement receipt](../../../build/validation/portfolio-editor-stage1-20261128/REPORT.md) and [DLL follow-up](../../../build/validation/portfolio-editor-stage1-followup-20261128/REPORT.md) bind the completed work. Finish qualification against the current source/build and matching cooked inputs.

1. Inspect a staged reviewer package for required runtime binaries and the absence of retired inspection files and sidecars. Finish selected-stack qualification with BuildPackaging when that identity is available.
2. Exercise the remaining view and lifetime cases on D3D12 and Vulkan. Check D3D12 GBuffer Diffuse, atlas growth and stale handles, and resize, minimize, map change and close during delayed viewport readback or texture upload. Show independent worker overlap with the frozen settings. Observe native replay and any required provider cells. Keep the final external consumer of the exported memory snapshot unresolved until its owner supplies evidence.
3. Reject malformed reflection through the real shader reload owner and confirm that the accepted generation remains usable.
4. Rebuild the frozen pre-cut source for the original visual reference and collect matched candidate images, load/frame percentiles and allocation observations on the frozen map matrix. Apply the [recorded sampling rules and bounds](../../../build/validation/portfolio-editor-stage0-20261128/CHECKS.md#inputs-and-numeric-bounds-fixed-before-a-candidate). Record unsupported cells explicitly and retain their gates. Once the outstanding cells pass, remove this stage and repair the queue links.

## Stage 2: Remove property search and console completion

Decisions: ED-19, ED-20. The [Stage 2 receipt](../../../build/validation/portfolio-editor-stage2-20261128/REPORT.md) records the cut, build and passing checks. Finish the remaining interactive qualification on the current candidate.

1. Exercise the retained Details controls with camera, light, sky, static and skeletal mesh, and variant selections. Confirm typed edit and publication through the selected controls, selection change ending a drag, separate entities with the same interaction key, generation replacement, and initial variant undo.
2. Verify the retained Outliner and Settings search widgets, console filter/copy/clear, keyboard Up/Down with draft restoration, and visible command failures. Exercise live compile with and without symbols and confirm explicit reload through the actual shader owner. The [frozen checks](../../../build/validation/portfolio-editor-stage0-20261128/CHECKS.md#stage-2) specify the unobserved input cases.

## Stage 3: Bound maintainer persistence and forbid package authoring

Decisions: ED-22, ED-23, ED-24.

Refresh the prepared source/reimport identity and run the [frozen transform, visibility and material-variant example](../../../build/validation/portfolio-editor-stage0-20261128/example.json) through the real importer, cook, Editor edit and reload flow. Capture the current baseline before changing the save path. The source audit records that Save All currently projects lighting, sky and the active camera description.

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
