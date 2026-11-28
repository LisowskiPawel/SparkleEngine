# Launcher: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: active; only remaining delivery work is listed below. The [shared stage protocol](README.md#stage-protocol) applies. [Design checkpoint](../../../build/validation/portfolio-reduction/launcher-stage0/REPORT.md) | [Retirement evidence](../../../build/validation/portfolio-reduction/launcher-stage1/REPORT.md).

**Execution order: 03 of 21.** Finish Stage 1's remaining cleanup, then resume Stage 2 in Wave 3 after BuildPackaging stage 1; stages 3-4 in Wave 6. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Keep the developer control surface and reviewer Quick Start; remove the machinery that turns ordinary operations into an environment-management product.

**Retain:** Scene browsing/previews, build/cook/sync/clean, CLI automation, configured debugger launch, launch readiness, explicit path overrides, independent concurrent operations and the current visual character. Preserve the four ordered Clean scopes: All; Content; Native compilation artifacts; Third-party compilation artifacts.

**Target flow:** GUI/CLI request -> one typed operation owner -> CMake/tool process or bounded worker work -> actual artifact observation -> UI result. History is not readiness authority.

Dominant execution owner for: LA-01, LA-02, LA-03, LA-05, LA-06, LA-07, LA-08, LA-09, LA-10, LA-11, LA-12, LA-13, LA-14, LA-15, LA-16, LA-19, LA-20, LA-21, LA-22, LA-23, LA-24, LA-25, LA-26, LA-27, LA-28, AP-07, RV-01.

Delegated complete systems: LA-04 -> [BuildPackaging](09_BuildPackaging.md); LA-17 -> [BuildPackaging](09_BuildPackaging.md); LA-18 -> [BuildPackaging](09_BuildPackaging.md). Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **Fixed product and build policy:** GUI and CLI create the same typed requests for the selected suite, Showcase, clang-cl and Ninja Multi-Config. Remove obsolete project/compiler/generator choices from request/state models and callers when BuildPackaging delivers that policy; retain six profiles, both APIs and explicit paths. Build/cook orchestration should read as preparation, execution, observation and result, with process/filesystem mechanics owned below it.
- **Artifact facts and Qt:** compute readiness from actual accepted outputs/dependencies, not remembered operations or a replacement history database. Use standard Qt ownership, controls, layouts and models for the retained pages. Model only real operation conflicts; independent jobs remain concurrent. Debug-tool launch hands off without managed incident polling, while native fault collection remains owned by the engine.

**Structural acceptance:** trace one GUI and one CLI build/cook/launch request through the same owner; restart with externally prepared outputs; show the reduced request/state/control-flow inventory and visual parity. A one-option selector, mirrored readiness store or wrapper chain retained only to preserve old call sites fails.

**Reference application:** [P6 and P3](README.md#reference-choices-and-limits) guide Qt reuse and explicit target composition. Apply Sparkle's actual artifact and operation lifetimes; do not introduce a UI framework or universal job service.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Current containing scope: **194 files / 18,079 physical code/build-script lines**, bound to the [retirement receipt](../../../build/validation/portfolio-reduction/launcher-stage1/REPORT.md). Uncompleted allocations from the original [ledger](README.md#footprint-and-size-targets): **2 whole files / 239 lines**, plus **167 shared lines in 3 files**. These remain conditional gross candidates, not guaranteed net savings; include replacements/callers and moved code. Data-only changes do not count as native code savings.

Historical overlapping replacement baselines from 25 November (refresh before each remaining stage; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| LA-03 | 35 files / 3,924 lines |
| LA-15 | 7 files / 1,077 lines |
| LA-19 | 5 files / 494 lines |
| LA-21 | 3 files / 667 lines |
| LA-23 | 3 files / 716 lines |
| LA-24 | 6 files / 778 lines |
| LA-25 | 18 files / 1,773 lines |
| AP-07 | 10 files / 1,725 lines |

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| LA-15, AP-07; stage 2 | Launcher incident polling/catalog/inspection/recovery and extra Application incident presentation transport. Trace Public/SparkleLauncher/GpuCrashIncident.h, LauncherCore.cmake, LauncherGpuCrashPanel/OperationService, OpenGpuCrash.ps1 -> GpuCrashIncident.ps1 and extra manifest/state readers. Remove only the managed inspection chain and its script/deployment/UI callers; split mixed native collection files by function. | Native GpuCrashIncidentSink/Files portions required for dump persistence, marker sidecars, callbacks, matching native/shader inputs and bounded collection settlement; external debugger launch and readiness. |
| LA-19; stage 3 | Build/State duplicate source-list freshness/signature authority. Trace BuildFilesFreshness, BuildFreshnessSignature, BuildWorkspaceStateFiles and their planner/readiness/cache users. Remove only bookkeeping replaced by native CMake/Ninja dependency/regeneration and minimal incompatible-cache identity. | Actual artifact availability, source/recipe/SDK invalidation and cache incompatibility errors; no history-based readiness. |
| LA-20; stage 3 | NativeBuildOutputReset automatic toolchain-change wipe path. Remove reset plan/step/state triggers, stale reset reason/flags, UI status and old toolchain-difference branches; retain a helper only if another selected Clean workflow truly consumes it. | Explicit controlled native Clean, source/download caches, real incompatible-cache rejection and task/process settlement. |
| LA-21; stage 3 | Stored multi-run history and history browser. Trace LauncherActivityPanel, operation/session serialization, activity row/time/log readers, GUI signals and shell activity consumers. Retire only history storage/presentation after current progress/failure consumers have direct ownership. | Active concurrent operation state, cancellation/settlement, useful current logs and artifact-derived readiness for external builds/cooks/dependencies. |
| LA-23; stage 3 | Expanded compiler/SDK/dependency setup/repair dashboard. Retire detail-only cards/models, install/repair actions, duplicated readiness facts and update signals. Recheck generated dependency facts and compiler/generator fields after BuildPackaging changes. | Concise actionable readiness, selected tool/SDK paths, required setup observations and source workspace preparation. |
| LA-24; stage 3 | Per-dependency code-sync selection UI/commands. Remove selection widgets/state, public operation variants and CLI/help aliases; make one enabled pinned-set request. Audit SPARKLE_SYNC_SOURCE_DEPENDENCY and per-recipe/cache helpers by internal consumers before removing any. | Internal bounded parallel per-dependency execution, admitted-source identity/locks, incremental cache reuse and dependency-specific failures. |
| LA-25; stage 4 | Custom Qt wrappers/manual layout and duplicated style/state mechanisms. Follow wrapper types to object ownership, signal wiring, models/delegates, duplicated roles, artwork/font/icon helpers and CMake/deployment components. Remove a helper or Qt component only after the replacement has no remaining use. | Current retained appearance/previews/theme, focus/DPI/accessibility and Qt6::Widgets; this is not wholesale Qt/icon removal. |
| LA-09 / LA-10; stage 3 | Replace the old CleanScope variants and every name/UI/CLI/planner/executor/preview/path mapping with exactly All; Content; Native compilation artifacts; Third-party compilation artifacts, in that order. Audit MaintenanceOperations, MaintenanceOperationPlanner, MaintenanceCleanSteps/CleanExecution/ActionCleanPaths and their state. All is the union of the three selected generated-output scopes; obsolete independent log/workspace-state/source-cache/pristine choices and aliases are not retained. | Preserve containment, exact target reporting, accepted source/settings, required readiness state and concurrent-operation conflict handling. Do not wipe source/download caches or historical evidence as compilation artifacts. |

## Outstanding qualification from earlier UI delivery

Launcher scene-view qualification retains physical fractional/mixed-DPI, assistive-technology and broader GUI/GPU/profile/capture/package checks as **UNVERIFIED** in the [bound delivery record](../../../build/validation/launcher-item-views/candidate-20261122/REPORT.md). Its required temporary cleanup, recorded with that evidence, remains open after automatic approval review rejected deletion; this adds no release prerequisite.

## Stage 1: Remove temporary validation probes

The production retirement and bounded controls passed; only required temporary cleanup remains. Remove the Stage 1 probe sources/build tree, controlled fixtures and probe setup/CLI scripts listed in the [cleanup receipt](../../../build/validation/portfolio-reduction/launcher-stage1/temporary-cleanup.json), then verify their absence and prune this stage and its queue entry. Automatic approval review rejected both cleanup commands as “blocked by policy”; no more specific reason was supplied. Existing user state and historical validation are outside this cleanup. This does not reopen the delivered production work or qualify later stages.

## Stage 2: Delete managed post-crash incident workflow end to end

Decisions: LA-15, AP-07.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/03_Launcher.md: Delete managed post-crash incident workflow end to end.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Own this cross-module removal through Application and CMake diagnostics scripts. Remove ReadGpuCrashState/ReadGpuCrashCollectionState, incident polling/list/catalog/inspection/recovery UI and commands, and otherwise unused application incident-report transport. Split mixed functions before deleting. Keep configured external tool launch and Editor crash-mode startup; retain native callbacks, dump/marker writing and bounded collection completion.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Produce an exact removed-function versus retained-native-function list. An opt-in native fault on each supported backend yields usable matched dump/breadcrumb/marker/symbol data; ordinary map launch requires no crash monitor. A successful startup alone does not pass this stage. Application/RHI protected owners sign off through the same revision-bound receipt.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Incident UI retirement also removes native callbacks, breadcrumb/marker sidecars or collection waits.
- Ordinary startup succeeds but native fault data cannot resolve this build/shader/source identity.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 3: Replace build/status bookkeeping with a small execution flow

Decisions: LA-03, LA-09, LA-10, LA-19, LA-20, LA-21, LA-23, LA-24.

**Implementation prompt:**

```text
Implement Stage 3 of Docs/Implementation/PortfolioReduction/03_Launcher.md: Replace build/status bookkeeping with a small execution flow.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
After BuildPackaging stage 1, remove duplicate build freshness signatures/file-list scans, automatic incompatible-cache wipes, stored operation-history browser/records, expanded setup/repair cards and per-dependency sync selection UI. Keep one explicit request -> configure/build -> settlement -> observation flow. CMake/Ninja owns graph invalidation; minimal compiler/generator/profile/SDK identity rejects incompatible caches. Sync the enabled pinned set as one request with parallel independent work. Replace the old CleanScope enum/name/UI/CLI/planner/path branches with exactly the four approved scopes in order; remove obsolete scope state and aliases while preserving contained generated-output targets and source/download data. Reuse actual native/cooked/shader/dependency contracts, not a second readiness database or blanket recursive hash service.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- A valid externally built/cooked product is recognized after Launcher restart without operation history. Missing/stale/incomplete products are not ready. Added/removed source and changed SDK/recipe invalidate correctly. Independent jobs overlap; conflicting output writers are ordered. Cancellation joins workers/processes; no history record is needed for correctness. Clean exposes exactly the four approved scopes in order, with no old-scope aliases or accidental source/cache/evidence deletion. LA-03 replacement plus callers must be measurably smaller than its retired responsibility.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- A remembered successful operation marks missing/incomplete/stale artifacts ready.
- A new state/signature database replaces the old one, independent operations serialize, or cancellation fails to settle children.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 4: Use Qt ownership directly and close visual/workflow parity

Decisions: LA-25, LA-22, LA-26, LA-27.

**Implementation prompt:**

```text
Implement Stage 4 of Docs/Implementation/PortfolioReduction/03_Launcher.md: Use Qt ownership directly and close visual/workflow parity.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Replace unnecessary wrapper widgets/manual layout/model duplication with standard Qt controls, layouts and existing model/delegate facilities. Retain one compact theme, existing previews and visual hierarchy. Avoid a replacement generic widget/service framework. Keep concurrent operations, scene-ready child handshake and path override contracts; native crash readiness validation remains distinct from post-crash monitoring.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Compare retained pages/states at the three DPI settings and exercise keyboard focus/errors. Build/cook/sync/clean and Editor/Game launch work from source and prepared package. No developer SDK absence blocks reviewer runtime readiness; actual scene readiness still detects startup failure. Record final source/API/state/branch reduction and remaining largest files.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Standard Qt implementation changes selected appearance, previews, focus or supported-DPI states.
- The handshake reports process creation as scene success, or reviewer launch requires development tools.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
