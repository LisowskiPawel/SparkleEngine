# Showcase content and conversion scripts: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: Stages 2?3 remain. The [shared stage protocol](README.md#stage-protocol) applies.

**Execution order: 04 of 21.** Run Stage 2 within SceneCooker Stage 2 and Stage 3 in Wave 5. Follow the [stage queue](README.md#suggested-execution-order).

## Outcome and ownership

Keep ten deliberate portfolio scenes and reproducible preparation, with only the Knight Modern Sponza variant and no general runtime content composition.

**Retain:** Empty, CornellBox, classic Sponza, BistroExterior, BistroInteriorWine, ModernSponzaKnight, CesiumMan, LPSHead, DamagedHelmet and ABeautifulGame; Cornell/LPS conversions and required attribution. Retain base/Curtains/Knight source dependencies.

**Target flow:** Pinned source/rights and fixed conversion/preparation -> explicit level scene reference -> cooked bundle -> Launcher scene selection -> Editor/Game and bounded regression examples.

Dominant execution owner for: PR-01, PR-02, PR-03, PR-04, PR-05, PR-07, PR-08, PR-11, PR-12, PR-15, PR-16. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **Ten explicit scene entries:** adopt explicit prepared-scene paths and the single Knight product when its SceneCooker stage lands. Remove runtime plural-asset metadata and catalog adapters that no retained map needs; keep the selected maps' source/rights dependencies.
- **Preparation is concrete:** keep Cornell/LPS conversion and the fixed Knight source recipe with checked units/identity. Source dependencies may be plural even though the runtime product is one scene. Do not replace retired composition with a generic converter framework, or require reviewers to acquire/cook content.

**Structural acceptance:** reproduce all selected scene references from the narrowed catalog and launch prepared Editor/Game maps; confirm Knight animation and required source attribution. Check active products/previews for retired entries. Removing Base/Curtains data still needed by Knight or booking raw-asset bytes as authored-code reduction fails.

**Reference application:** [P3](README.md#reference-choices-and-limits) informs explicit input/product composition; the selected scenes and their fixed preparation follow Sparkle's scope, not another repository's sample asset system.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **15 files / 762 physical code/script lines**. No assigned whole-file or shared-span source retirement remains. Budget the fixed preparation and all changed callers in the SceneCooker stage 2 vertical change; count moved/replacement code and report asset bytes separately from native code savings.

## Stage 2: Reproduce one prepared Knight product and explicit scene paths

Decisions: PR-08.

The current Knight FBX cooks without skin weights, a skeleton or a clip. Recover the skin from the matching Body, Shield and Sword data in the publisher’s Arnold Maya ASCII file. Prove the joint bind transforms and motion through the existing importer before migrating the prepared scene. The FBX and Maya ASCII data are already in the acquired Knight pack.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/04_Showcase.md: Reproduce one prepared Knight product and explicit scene paths.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Participation boundary: execute the shared one-product migration inside **SceneCooker stage 2**, which owns the complete vertical cut. This stage owns its explicit resolver/project-recipe details and verifies its consumers; it does not start a second composition migration. Close both affected stages against the same qualified source identity when their distinct exits pass.



Implement the small project-owned fixed preparation entry consumed by SceneCooker stage 2. Inputs are Base/main_sponza/NewSponza_Main_glTF_003.gltf, AddOns/Curtains/pkg_a_curtains/NewSponza_Curtains_glTF.gltf and AddOns/Knight/pkg_e_knight_anim/Exports/FBX/Knight_USD_002.fbx. Recover the missing skin bindings from Scenes/knight_ARNOLD_ANIM_002.rnd.ma in that same Knight pack during offline preparation. Keep the routine readable and specific. Update all retained level inputs to the explicit source/prepared-scene contracts selected by AssetCooker.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Knight reproduces complete composition/animation after clean preparation. A changed input is recooked and a missing one fails usefully. Import/cook/resource identity tests cover source-unit transforms and stable saved object references. The runtime contains one scene product per level.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Knight preparation is not reproducible or loses animation/material/light/transform identity.
- A new general recipe language/runtime add-on path replaces the old composition instead of a fixed preparation routine.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 3: Close selected-scene walkthrough and acquisition boundaries

Decisions: PR-01, PR-02, PR-03, PR-04, PR-05, PR-11, PR-12, PR-15, PR-16.

**Implementation prompt:**

```text
Implement Stage 3 of Docs/Implementation/PortfolioReduction/04_Showcase.md: Close selected-scene walkthrough and acquisition boundaries.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Remove unused script/catalog helpers exposed by the smaller set; retain the existing CornellBox/LPSHead reproducibility routes and source-authored camera/light/tangent semantics. Set intentional defaults and explain each selected scene purpose, controls and limits without adding new demos or features.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- All promised map/product/API combinations load from prepared output and source preparation remains reproducible for the maintainer. Rights/notice review is explicit and public export restrictions remain until resolved. Existing runtime/image defects are tracked separately from source-route success.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- A selected source/converter/default is removed merely to reduce bytes.
- A map runs only with raw source/cook tools, or source-route success is misreported as image/redistribution qualification.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
