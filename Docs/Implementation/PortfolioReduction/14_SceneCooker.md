# SceneCooker: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

**Execution order: 14 of 21 (first focused visit).** Start: Stages 0-2 in Wave 5, following SourceImporters stage 2. Resume / dependency: Stage 3 in Wave 5 after SourceImporters stage 3 and MeshCooker/MaterialCooker. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Prepare the exact runtime scene and animation products offline, eliminating runtime authoring/general composition without removing parallel loading or playback.

**Retain:** Distinct library boundary, checked scene metadata, source cameras/lights, selected skeleton/clip data, reusable resource identities and correct dependency records.

**Target flow:** Checked imported sources + fixed Showcase preparation -> one flattened checked scene/ready Ozz runtime records -> scene registry/manifest -> parallel runtime loading.

Dominant execution owner for: GF-13, GF-14, SC-01, SC-02, SC-03. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **Prepared runtime data:** own source-to-runtime Ozz construction and the flat assembly of project-declared inputs. Emit one scene with ready runtime skeleton/clip data. Update runtime loading, manifests and source dependency identity together. Showcase data owns asset identities and paths. Cooking needs only the declared inputs and product identity.
- **One checked scene contract:** narrow indices, resource references, metadata and writer/parser intermediates after morph/composition cuts. Keep reusable within-scene resources, authored cameras/lights and skeletal rebasing correct. Conditional curve/resampler removal follows native animation equivalence; retain the minimum required single working path if it fails.

**Structural acceptance:** reproduce Knight from admitted sources, check units/joints/material/camera/light rebasing and run Knight/Cesium animation through threaded loading and rendering. Confirm GameFramework no longer constructs offline Ozz data or interprets a source list. Flattening by duplication or hiding composition in runtime adapters fails.

**Reference application:** [P1 and P3](README.md#reference-choices-and-limits) guide explicit ownership and dependency placement. The fixed recipe and runtime representation follow Sparkle's decisions; no Unreal asset database or scene framework is introduced.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **30 files / 1,430 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **0 files / 0 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| GF-13 | 6 files / 604 lines |
| GF-14 | 6 files / 536 lines |
| SC-03 | 2 files / 162 lines |

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| GF-13; stage 1 | Runtime Ozz offline construction. Move needed preparation to SceneCooker; remove GameFramework runtime builder code, includes/adapters and ozz_animation_offline link in Engine/GameFramework/CMakeLists.txt. Trace runtime deployment/source-package attribution after linkage changes. | Ozz offline target stays for cooking; ozz runtime/base, parallel animation evaluation, skeleton/clip identity and correct Knight/Cesium motion remain. |
| SC-03; stage 1 | Over-general/duplicated cook curve resampling. Remove unused curve evaluation/baking adapters and intermediate records after IM-08/09 and cook-time Ozz preparation establish one path. Check skeleton/clip writer/readers and previous runtime builder links. | Minimal required TRS/quaternion sampling and correct bind pose/timing; Ozz prep remains in cooker and parallel evaluation remains. |
| GF-14; stage 2 | General runtime multi-asset scene composition. Retire plural level asset list/parser/writer fields, loader aggregation/fan-out, runtime add-on/composite identity and extra scene-store paths only after fixed offline Knight preparation migrates all producers/consumers. | Base/Curtains/Knight source assembly, one prepared product, parallel reads/decode, all rebased camera/light/material/joint references and saved object identity. |

## Stage 0: freeze the actual responsibility and smaller target

Read CookedSceneBuild, instance/metadata writers and animation bake/resample flow, plus GameFramework runtime Ozz construction. Before flattening Knight, list every rebased mesh/material/node/camera/light/skeleton/clip index, texture reference and coordinate/unit transform. Record the current separate base/Curtains/Knight products and their loaded result.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Cooking/SceneCooker/Public/CookedSceneBuild.h](../../../Tools/Cooking/SceneCooker/Public/CookedSceneBuild.h)
- [Tools/Cooking/SceneCooker/Private/Instances/CookedSceneInstanceBuilder.cpp](../../../Tools/Cooking/SceneCooker/Private/Instances/CookedSceneInstanceBuilder.cpp)
- [Tools/Cooking/SceneCooker/Private/Metadata/CookedSceneMetadataBuilder.cpp](../../../Tools/Cooking/SceneCooker/Private/Metadata/CookedSceneMetadataBuilder.cpp)
- [Engine/GameFramework/Private/World](../../../Engine/GameFramework/Private/World)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Implementation prompt:**

```text
Prepare Stage 0 of Docs/Implementation/PortfolioReduction/14_SceneCooker.md: freeze the actual responsibility and smaller target.
Read this module's outcome, retained contracts, source entry points and Stage 0 questions; read Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md, Docs/Roadmap.md and Docs/Implementation/PortfolioReduction/README.md. Refresh current HEAD/working-tree identities and respect concurrent work. Audit the named retirement closure/participation in this plan, including narrowed retained capabilities; prepare a transitive retire/retain-with-consumer/unresolved manifest and rerun it after each layer disappears. Do not implement the production refactor in this stage.

Acceptance criteria:
- Specify this plan's Adapt the surviving design contract: vanished assumption, concrete smaller API/data/control flow, caller changes, selected reference lesson and local limits. Budget all replacement/caller code and name the retained behavior/failure check.
- an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.
- Map each proposed changed function to one cohesive responsibility/owner and an intent-based name; distinguish workflow orchestration from implementation mechanisms. Specify callers/contracts/files/build outputs to retire and the sole target path; no legacy/backward-compatibility reader, alias, migration, disabled stub or old/new implementation may be planned.
- Define stage-specific success, invalid-input/failure/cancellation/lifetime checks and measured bounds before coding; retain all approved parallel/data-oriented/game/render/RHI contracts. No unsupported performance or qualification claim.

Failure modes to reject:
- The feature disappears but surviving contracts retain obsolete generality, or an upstream framework is copied without a local invariant.
- Historical SceneCooker paths, footprint counts or old receipts are treated as current without tracing actual callers/build inputs.
- Mixed retained/retired responsibilities or CPU/GPU reader lifetimes are omitted, leaving no safe complete-system cut or measurable replacement budget.
- A generic wrapper/framework, serial execution, retained compatibility path or source relocation is substituted for a smaller complete design. A first-order-only dependency scan is insufficient.

Deliver the concrete design/ownership map, bounded change and net footprint budget, planned checks and open conditions in ignored stage evidence. Keep CodeStyle's function/namespace/layout rules explicit in the design; permanent test-only additions are not authorized. Do not close an unproven implementation stage or change a recorded decision.
```

## Stage 1: Move Ozz preparation into cooking and narrow resampling

Decisions: GF-13, SC-03.

**Implementation prompt:**

```text
Implement Stage 1 of Docs/Implementation/PortfolioReduction/14_SceneCooker.md: Move Ozz preparation into cooking and narrow resampling.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Own the cross-module change through imported/cooked contracts, Ozz dependency linkage and GameFramework playback. Cook ready runtime skeleton/animation data once; keep only sampling needed by retained curves/clips. Retire runtime offline builders, the GameFramework ozz_animation_offline link and duplicate conversion, preserving names/associations and parallel preparation/evaluation. Do not invent versioned compatibility readers: regenerate disposable cooked data.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Knight/Cesium import/cook/playback, duration, joint association, bind pose and animated RT geometry match defined controls. If IM-08/IM-09 equivalence fails, retain required sampling. Runtime no longer links/calls unused Ozz offline builders; cold load time and memory are measured. Cook-time additions are subtracted from runtime deletions.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Offline Ozz construction remains in runtime, or cook/runtime copies both become permanent supported paths.
- Required Knight/Cesium curves, joints/timing/rotation or motion/RT data change; source metadata alone is treated as playback proof.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 2: Deliver one prepared Knight scene and remove runtime multi-asset composition

Decisions: GF-14.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/14_SceneCooker.md: Deliver one prepared Knight scene and remove runtime multi-asset composition.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Own the vertical cut through Showcase preparation, AssetCooker, runtime level/loader and relevant Editor saved identities. Use Showcase's declared Base, Curtains and Knight inputs through asset-independent cooking code. Flatten and rebase all references and preserve skeletal animation, imported cameras/lights, materials/UVs, units and authored transforms. Runtime levels then reference one prepared scene product. Remove multi-asset document and loading branches after all retained maps migrate.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- A reproduced Knight product contains complete environment/curtains/animated character and loads on both APIs. Resource/index/material/light counts, transforms and animation are checked; name collisions cannot silently remap a bone/material. A changed constituent input invalidates the combined product. No runtime multi-asset route survives as a fallback; one product still loads/decode in parallel.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Flattened Knight drops base/Curtains, rebases a node/material/bone incorrectly or loses source units/cameras/lights.
- Runtime still assembles asset lists, a constituent change is missed, or saved object identity silently remaps.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 3: Close checked metadata/library ownership

Decisions: SC-01, SC-02.

**Implementation prompt:**

```text
Implement Stage 3 of Docs/Implementation/PortfolioReduction/14_SceneCooker.md: Close checked metadata/library ownership.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Keep cohesive scene-writing and imported-camera/light metadata contracts. Remove builders/record copies whose only consumers were morph/old composition/runtime preparation. Coordinate with MeshCooker/MaterialCooker without merging their explicitly retained libraries or adding a general asset serialization layer.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Malformed/truncated/index-invalid data is rejected before accepted publication; runtime readers use exactly the cooked layout produced here. Reviewer can trace source object/joint identity to one scene record and the corresponding runtime resource.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Metadata pruning drops retained camera/light/material associations or invalidates runtime bounds.
- Moved builders introduce a second serialization model or duplicate records without net simplification.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
