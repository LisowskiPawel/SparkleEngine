# ToolSupport: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

**Execution order: 10 of 21 (first focused visit).** Start: Stages 0-1 in Wave 4. Resume / dependency: Stage 2 at the end of Wave 5 after the fixed Knight recipe and source users settle. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Remove a general project/content browser model while retaining one shared tool startup and the exact Showcase acquisition/layout contract.

**Retain:** Shared startup/errors/progress and source/output format contracts, retained scene identities and required source/download/error explanations. Keep parallel tool work and actual artifact observation.

**Target flow:** Showcase explicit catalog and product roots -> retained source/pack dependency facts -> Launcher/cook consumers. Runtime readiness reads prepared outputs, not acquisition history.

Dominant execution owner for: TS-01, TS-02, TS-03, TS-04. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **One Showcase catalog:** remove general project enumeration and family/group/admission presentation layers from catalog schemas, loaders, Launcher models and cook requests. Represent the retained scenes and real constituent source/pack dependencies directly. Fixed user-project scope does not erase internal Engine/shared-shader roots or Knight's multiple preparation inputs.
- **Neutral tool contracts:** keep one explicit startup/path/error/progress boundary used by actual tools. Remove project-policy mirrors and forwarding-only adapters; leave process supervision and domain publication with their owners. Read runtime readiness from actual prepared outputs, independently of source acquisition state.

**Structural acceptance:** trace scene selection to acquisition/preparation and reviewer launch, including missing Knight inputs and absent development SDKs. Show a flatter catalog/API with retained actionable failures. A renamed general project registry, acquisition history used as runtime readiness or new universal tool service fails.

**Reference application:** [P1 and P3](README.md#reference-choices-and-limits) guide focused contracts and explicit composition. Use Sparkle's existing typed records and artifact contract, not a general project/asset database.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **20 files / 1,428 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **0 files / 0 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| TS-02 | 5 files / 524 lines |
| TS-03 | 8 files / 1,053 lines |
| TS-04 | 6 files / 760 lines |

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| TS-02; stage 1 | General project discovery/selection. Retire ToolSupport catalog/project enumerators, Core/product root fallback, Launcher selectors, cooker ALL paths, CMake Projects.cmake project discovery and package project matrices together. | Only Showcase is a user project; internal Engine/shared shader scopes, standalone Launcher and real root overrides remain. |
| TS-03; stage 2 | Scene-family/variant presentation framework. Remove Family/VariantKind parsing/fields/enums, derived row grouping, selection models/delegates and unused GUI/CLI formatting while flattening retained scene entries. | Actual pack dependency/acquisition edges needed for Knight preparation and retained content readiness, not a duplicated browser model. |
| TS-04; stage 2 | Unused catalog admission/capability policy. Review validator/reader fields, unsupported/download/game capability branches and duplicated reasons against retained packs; remove field producers and consumers together only when none remains. | Manual-source/acquisition/integrity/required-input errors and necessary pack parent relationships; entry removal alone is not proof of dead policy. |

## Stage 0: freeze the actual responsibility and smaller target

Trace ProjectLevelCatalog reader/validator, Core root helpers, Launcher project and content resolution, cook planning and CMake project discovery. Separate family/variant presentation from actual base/Curtains/Knight dependency edges. Existing catalog still contains Candles until Showcase stage 1; do not confuse checkout data with the ten-map target.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Support/ToolSupport/Public/ProjectLevelCatalog.h](../../../Tools/Support/ToolSupport/Public/ProjectLevelCatalog.h)
- [Tools/Support/ToolSupport/Private/Projects/ProjectLevelCatalog.cpp](../../../Tools/Support/ToolSupport/Private/Projects/ProjectLevelCatalog.cpp)
- [Tools/Support/ToolSupport/Private/Projects/ProjectLevelCatalogValidator.cpp](../../../Tools/Support/ToolSupport/Private/Projects/ProjectLevelCatalogValidator.cpp)
- [Tools/Support/ToolSupport/Private/SourceAssetPaths.cpp](../../../Tools/Support/ToolSupport/Private/SourceAssetPaths.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Implementation prompt:**

```text
Prepare Stage 0 of Docs/Implementation/PortfolioReduction/10_ToolSupport.md: freeze the actual responsibility and smaller target.
Read this module's outcome, retained contracts, source entry points and Stage 0 questions; read Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md, Docs/Roadmap.md and Docs/Implementation/PortfolioReduction/README.md. Refresh current HEAD/working-tree identities and respect concurrent work. Audit the named retirement closure/participation in this plan, including narrowed retained capabilities; prepare a transitive retire/retain-with-consumer/unresolved manifest and rerun it after each layer disappears. Do not implement the production refactor in this stage.

Acceptance criteria:
- Specify this plan's Adapt the surviving design contract: vanished assumption, concrete smaller API/data/control flow, caller changes, selected reference lesson and local limits. Budget all replacement/caller code and name the retained behavior/failure check.
- an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.
- Map each proposed changed function to one cohesive responsibility/owner and an intent-based name; distinguish workflow orchestration from implementation mechanisms. Specify callers/contracts/files/build outputs to retire and the sole target path; no legacy/backward-compatibility reader, alias, migration, disabled stub or old/new implementation may be planned.
- Define stage-specific success, invalid-input/failure/cancellation/lifetime checks and measured bounds before coding; retain all approved parallel/data-oriented/game/render/RHI contracts. No unsupported performance or qualification claim.

Failure modes to reject:
- The feature disappears but surviving contracts retain obsolete generality, or an upstream framework is copied without a local invariant.
- Historical ToolSupport paths, footprint counts or old receipts are treated as current without tracing actual callers/build inputs.
- Mixed retained/retired responsibilities or CPU/GPU reader lifetimes are omitted, leaving no safe complete-system cut or measurable replacement budget.
- A generic wrapper/framework, serial execution, retained compatibility path or source relocation is substituted for a smaller complete design. A first-order-only dependency scan is insufficient.

Deliver the concrete design/ownership map, bounded change and net footprint budget, planned checks and open conditions in ignored stage evidence. Keep CodeStyle's function/namespace/layout rules explicit in the design; permanent test-only additions are not authorized. Do not close an unproven implementation stage or change a recorded decision.
```

## Stage 1: Retire general multi-project discovery as one cross-module system

Decisions: TS-02.

**Implementation prompt:**

```text
Implement Stage 1 of Docs/Implementation/PortfolioReduction/10_ToolSupport.md: Retire general multi-project discovery as one cross-module system.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Fix user-product selection to Showcase across ToolSupport, Core/product composition, Launcher, cookers, CMake and packaging. Remove project enumeration/selection/fallback state and duplicate discovery. Keep internal Engine/shared shader domains, standalone Launcher and required source/cooked root distinction; they are not extra user projects.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Source and relocated reviewer products identify Showcase without scanning for another project. Explicit path overrides work; a missing developer source catalog does not block a prepared runtime. No hidden ALL/project selector remains in retained entry points; roots have one authority with Core stage 2.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Showcase-only policy leaves project/ALL selectors or duplicate root discovery alive elsewhere.
- Internal Engine/shared-shader/standalone Launcher boundaries are mistaken for extra projects and deleted.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 2: Flatten presentation and delete unused admission branches

Decisions: TS-03, TS-04.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/10_ToolSupport.md: Flatten presentation and delete unused admission branches.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Use explicit retained scene entries, removing unneeded family/add-on variant presentation fields and their UI/parser branches. Preserve actual Knight/base/Curtains preparation dependencies until its fixed recipe owns them, plus source availability/manual-download errors and integrity checks needed by retained packs. Delete policy only after its last consumer vanishes; removed catalog data alone is not proof of dead code.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- All ten selected scenes resolve/cook/launch and acquisition failures identify the real missing input. Dependency graph contains required Knight inputs, no retired Candles/future add-on route. Tool startup/progress remains shared and thread-safe rather than copied into each executable.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Flattened presentation drops real Knight/base/Curtains preparation or useful acquisition failures.
- Unused policy remains behind hidden fields, or shared startup/path/progress code is copied into each tool.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
