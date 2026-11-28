# AssetCooker: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

**Execution order: 12 of 21 (first focused visit).** Start: Stages 0-1 in Wave 4. Resume / dependency: Stage 2 within SceneCooker stage 2; stage 3 after TextureCooker stage 2, in Wave 5. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Make selected Showcase asset preparation a short explicit parallel workflow, with one shader sequencer and one texture request contract.

**Retain:** Incremental source/output identity, bounded parallel asset/texture work, accepted-output publication, selected scenes and supervised TextureCooker process.

**Target flow:** Launcher selects Showcase/profile/scopes -> AssetCooker resolves explicit prepared scene inputs -> importer/cooker work -> TextureCooker request stage -> complete accepted publication. Launcher invokes ShaderCompiler separately.

Dominant execution owner for: AC-01, AC-02, AC-03, AC-04, AC-05, AC-06, AC-07. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **One product grammar:** express Showcase, retained profiles/scopes and explicit source paths directly in command parsing and preparation. Remove multi-project/ALL/positional inference and extension probing from downstream requests and results, not just the CLI. Launcher owns direct ShaderCompiler invocation; delete the duplicate shader stage and its dependency/result bookkeeping.
- **One bounded preparation flow:** use the retained importer/cooker boundaries and supervised TextureCooker stage. Simplify per-stage request/result intermediates and duplicate publication state while preserving incremental identity, bounded parallel work, cancellation, child settlement and accepted-output publication. Keep used cook-assets/cook-textures routes.

**Structural acceptance:** trace maintainer full and incremental/no-change preparation from Launcher to accepted assets/textures; test failure/cancel without corrupting accepted output. Inventory the smaller CLI, stage sequence and state holders. A general project/stage registry with one selected project or duplicate shader orchestration fails.

**Reference application:** [P3 and P4](README.md#reference-choices-and-limits) support explicit composition and delegation of compiler mechanics. These are local cook-orchestration choices, not an imported asset pipeline or task service.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **30 files / 1,550 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **1 files / 24 lines**; assigned shared spans: **35 lines in 1 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| AC-03 | 7 files / 285 lines |
| AC-04 | 2 files / 462 lines |

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| AC-03; stage 1 | Unused extra AssetCooker subset/category interfaces. Identify actual extra interface before deletion; prune command registrations, category enum/planner/executor switches and caller-specific state only after a retained caller migration. | Launcher actively calls cook-assets/cook-textures; keep them or update all callers in one change. No blanket category deletion. |
| AC-04; stage 1 | ALL/multi-project/permissive positional CLI. Retire alternate tokens/aliases/project enumeration, argument-escaping/reordering complexity, duplicated profile/root derivation, help/validation and Launcher caller assumptions after one explicit grammar is live. | Showcase profiles/root contracts, shared tool startup, noninteractive calls and parallel scene work. |
| AC-05; stage 1 | AssetCooker shader-stage wrapper. Retire shader runner/source/CMake dependency plus cook-shaders/full-project promises, categories and executor branches unique to that wrapper; update help/tests/callers. | Launcher -> ShaderCompiler sequence already used today, ShaderCompiler executable, Cook All and Editor compile/reload. |
| AC-07; stage 2 | Extension-probing scene-source resolution. Remove extension candidate tables/ambiguity/fallback branches after all level/catalog/preparation references name supported inputs explicitly. Close tool path/admission/build/cook callers and old output keys. | Contained relocatable paths, deterministic missing-source errors and each fixed Knight recipe constituent. |

## Stage 0: freeze the actual responsibility and smaller target

Trace CLI, CookProjectContent, ProjectCookPlanner/source probing, scene/texture stages and actual Launcher commands. Launcher calls cook-assets and cook-textures today; AC-03 must not delete those active endpoints blindly. Identify its independent cook-project/cook-shaders contract and the single-scene Knight preparation handoff.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Cooking/AssetCooker/Private/Cli/AssetCookerCli.cpp](../../../Tools/Cooking/AssetCooker/Private/Cli/AssetCookerCli.cpp)
- [Tools/Cooking/AssetCooker/Private/Planning/ProjectCookPlanner.cpp](../../../Tools/Cooking/AssetCooker/Private/Planning/ProjectCookPlanner.cpp)
- [Tools/Launcher/SparkleLauncher/Private/Cook/CookOperationProcessRequests.cpp](../../../Tools/Launcher/SparkleLauncher/Private/Cook/CookOperationProcessRequests.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Implementation prompt:**

```text
Prepare Stage 0 of Docs/Implementation/PortfolioReduction/12_AssetCooker.md: freeze the actual responsibility and smaller target.
Read this module's outcome, retained contracts, source entry points and Stage 0 questions; read Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md, Docs/Roadmap.md and Docs/Implementation/PortfolioReduction/README.md. Refresh current HEAD/working-tree identities and respect concurrent work. Audit the named retirement closure/participation in this plan, including narrowed retained capabilities; prepare a transitive retire/retain-with-consumer/unresolved manifest and rerun it after each layer disappears. Do not implement the production refactor in this stage.

Acceptance criteria:
- Specify this plan's Adapt the surviving design contract: vanished assumption, concrete smaller API/data/control flow, caller changes, selected reference lesson and local limits. Budget all replacement/caller code and name the retained behavior/failure check.
- an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.
- Map each proposed changed function to one cohesive responsibility/owner and an intent-based name; distinguish workflow orchestration from implementation mechanisms. Specify callers/contracts/files/build outputs to retire and the sole target path; no legacy/backward-compatibility reader, alias, migration, disabled stub or old/new implementation may be planned.
- Define stage-specific success, invalid-input/failure/cancellation/lifetime checks and measured bounds before coding; retain all approved parallel/data-oriented/game/render/RHI contracts. No unsupported performance or qualification claim.

Failure modes to reject:
- The feature disappears but surviving contracts retain obsolete generality, or an upstream framework is copied without a local invariant.
- Historical AssetCooker paths, footprint counts or old receipts are treated as current without tracing actual callers/build inputs.
- Mixed retained/retired responsibilities or CPU/GPU reader lifetimes are omitted, leaving no safe complete-system cut or measurable replacement budget.
- A generic wrapper/framework, serial execution, retained compatibility path or source relocation is substituted for a smaller complete design. A first-order-only dependency scan is insufficient.

Deliver the concrete design/ownership map, bounded change and net footprint budget, planned checks and open conditions in ignored stage evidence. Keep CodeStyle's function/namespace/layout rules explicit in the design; permanent test-only additions are not authorized. Do not close an unproven implementation stage or change a recorded decision.
```

## Stage 1: Remove project/category/argument generality and duplicate shader sequencing

Decisions: AC-03, AC-04, AC-05.

**Implementation prompt:**

```text
Implement Stage 1 of Docs/Implementation/PortfolioReduction/12_AssetCooker.md: Remove project/category/argument generality and duplicate shader sequencing.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Fix the sole product to Showcase and use a small explicit CLI. Retire ALL/discovery, permissive positional permutations and unused fine-grained subsets; retain/update the active cook-assets/cook-textures callers together. Remove AssetCooker-owned shader stage and narrow cook-project/cook-shaders promises; Launcher already sequences ShaderCompiler. Keep one working Cook All and noninteractive Editor/Launcher integration.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- CLI validation/help and Launcher callers agree on one grammar; full/asset/texture/shader actions reach the correct retained owners. Failed stages preserve accepted outputs. No second project discovery or duplicate shader runner remains.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Removing category CLI generality breaks Launcher cook-assets/cook-textures or Cook All.
- Both Launcher and AssetCooker still sequence shaders, or unsupported positional/ALL aliases remain.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 2: Resolve explicit sources and one prepared scene product

Decisions: AC-07.

The project cook planner now admits exact sources and checks their bindings through the shared level parser. The remaining Stage 2 work is to consume the qualified Knight preparation and migrate the planner to the final prepared-scene identities.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/12_AssetCooker.md: Resolve explicit sources and one prepared scene product.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Participation boundary: execute the shared one-product migration inside **SceneCooker stage 2**, which owns the complete vertical cut. This stage owns its explicit resolver/project-recipe details and verifies its consumers; it does not start a second composition migration. Close both affected stages against the same qualified source identity when their distinct exits pass.



With SceneCooker stage 2 and Showcase, consume the fixed Knight preparation and one prepared scene per level. Migrate the planner's binding validation, cook requests and output identities together. Preserve stable saved identities and relocatable contained paths.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Every nonempty retained level resolves deterministically; missing/ambiguous/escaping inputs fail clearly. Single Knight product contains base/Curtains/animation, not just the FBX. Incremental invalidation includes each required source, texture and preparation setting.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- An extensionless fallback still guesses a source, or single-product Knight loses constituent inputs.
- One constituent change does not invalidate the prepared product, or escaping paths enter the cook.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 3: Consolidate parallel preparation/publication

Decisions: AC-01, AC-02, AC-06.

**Implementation prompt:**

```text
Implement Stage 3 of Docs/Implementation/PortfolioReduction/12_AssetCooker.md: Consolidate parallel preparation/publication.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Remove redundant orchestration/state surfaced by stages 1/2. Retain parallel independent preparation, one supervised TextureCooker request-file stage, bounded cancellation/settlement and complete publication. One operation/process never implies one worker or global serial cooking.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Check full then no-op/one-input-changed cook, interrupted child, invalid source and delayed readers. Existing accepted scene/texture outputs remain valid on failure; no-op avoids actual preparation without trusting a past success flag. Account for end-to-end source reduction across Launcher/AssetCooker, not just moved functions.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- One operation or child process becomes serial cooking, or request deduplication races.
- Cancellation/child failure publishes a partial set or destroys the previous accepted output.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
