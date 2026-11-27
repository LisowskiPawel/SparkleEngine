# Launcher: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Keep the developer control surface and reviewer Quick Start; remove the machinery that turns ordinary operations into an environment-management product.

**Retain:** Scene browsing/previews, build/cook/sync/clean, CLI automation, configured debugger launch, launch readiness, explicit path overrides, independent concurrent operations and the current visual character. Preserve the four ordered Clean scopes: All; Content; Native compilation artifacts; Third-party compilation artifacts.

**Target flow:** GUI/CLI request -> one typed operation owner -> CMake/tool process or bounded worker work -> actual artifact observation -> UI result. History is not readiness authority.

Dominant execution owner for: LA-01, LA-02, LA-03, LA-05, LA-06, LA-07, LA-08, LA-09, LA-10, LA-11, LA-12, LA-13, LA-14, LA-15, LA-16, LA-19, LA-20, LA-21, LA-22, LA-23, LA-24, LA-25, LA-26, LA-27, LA-28, AP-07, RV-01.

Delegated complete systems: LA-04 -> [BuildPackaging](BuildPackaging.md); LA-17 -> [BuildPackaging](BuildPackaging.md); LA-18 -> [BuildPackaging](BuildPackaging.md). Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **205 files / 19,662 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **8 files / 639 lines**; assigned shared spans: **239 lines in 4 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| LA-03 | 35 files / 3,924 lines |
| LA-11 | 8 files / 595 lines |
| LA-15 | 7 files / 1,077 lines |
| LA-19 | 5 files / 494 lines |
| LA-21 | 3 files / 667 lines |
| LA-23 | 3 files / 716 lines |
| LA-24 | 6 files / 778 lines |
| LA-25 | 18 files / 1,773 lines |
| LA-28 | 2 files / 521 lines |
| AP-07 | 10 files / 1,725 lines |

## Stage 0: freeze the actual responsibility and smaller target

Trace BuildWorkspaceOperations/Planner/Executor, CMakeWorkflowProcessRequests, LauncherOperationService and CookedContentReadiness together. Classify each record as input, owned execution state, native process lifetime or observed artifact fact. Capture current pages at 100/150/200% DPI before replacing wrappers. Inventory the exact mixed GPU-crash functions against the protected native collector.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Launcher/SparkleLauncher/Private/Build/BuildWorkspacePlanner.cpp](../../../Tools/Launcher/SparkleLauncher/Private/Build/BuildWorkspacePlanner.cpp)
- [Tools/Launcher/SparkleLauncher/Private/Build/CMakeWorkflowProcessRequests.cpp](../../../Tools/Launcher/SparkleLauncher/Private/Build/CMakeWorkflowProcessRequests.cpp)
- [Tools/Launcher/SparkleLauncher/Private/Cook/CookedContentReadiness.cpp](../../../Tools/Launcher/SparkleLauncher/Private/Cook/CookedContentReadiness.cpp)
- [Tools/Launcher/SparkleLauncher/Private/Gui/GpuCrash/LauncherGpuCrashPanel.cpp](../../../Tools/Launcher/SparkleLauncher/Private/Gui/GpuCrash/LauncherGpuCrashPanel.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Retire optional shell, installer and self-replacement workflows

Decisions: LA-11, LA-13, LA-16, LA-28.

Delete rich shell dashboard/activity presentation, HostToolInstaller, shadow/live-copy bootstrap and self-rebuild/restart, and the dry-run command/report path. Update every CLI/UI caller and build entry. Keep ordinary launch, command help/errors, accurate destructive targets and internal execution planning. Rebuilding Launcher now uses a closed executable; Editor restart is separate.

**Exit evidence:** Build standalone Launcher; run retained CLI actions and startup from its canonical path. Help exposes no removed command; no background shadow generation/restart/install action remains. Retained Clean paths stay contained; existing accepted outputs survive unrelated failures.

## Stage 2: Delete managed post-crash incident workflow end to end

Decisions: LA-15, AP-07.

Own this cross-module removal through Application and CMake diagnostics scripts. Remove ReadGpuCrashState/ReadGpuCrashCollectionState, incident polling/list/catalog/inspection/recovery UI and commands, and otherwise unused application incident-report transport. Split mixed functions before deleting. Keep configured external tool launch and Editor crash-mode startup; retain native callbacks, dump/marker writing and bounded collection completion.

**Exit evidence:** Produce an exact removed-function versus retained-native-function list. An opt-in native fault on each supported backend yields usable matched dump/breadcrumb/marker/symbol data; ordinary map launch requires no crash monitor. A successful startup alone does not pass this stage. Application/RHI protected owners sign off through the same revision-bound receipt.

## Stage 3: Replace build/status bookkeeping with a small execution flow

Decisions: LA-03, LA-19, LA-20, LA-21, LA-23, LA-24.

After BuildPackaging stage 1, remove duplicate build freshness signatures/file-list scans, automatic incompatible-cache wipes, stored operation-history browser/records, expanded setup/repair cards and per-dependency sync selection UI. Keep one explicit request -> configure/build -> settlement -> observation flow. CMake/Ninja owns graph invalidation; minimal compiler/generator/profile/SDK identity rejects incompatible caches. Sync the enabled pinned set as one request with parallel independent work. Reuse actual native/cooked/shader/dependency contracts, not a second readiness database or blanket recursive hash service.

**Exit evidence:** A valid externally built/cooked product is recognized after Launcher restart without operation history. Missing/stale/incomplete products are not ready. Added/removed source and changed SDK/recipe invalidate correctly. Independent jobs overlap; conflicting output writers are ordered. Cancellation joins workers/processes; no history record is needed for correctness. LA-03 replacement plus callers must be measurably smaller than its retired responsibility.

## Stage 4: Use Qt ownership directly and close visual/workflow parity

Decisions: LA-25, LA-22, LA-26, LA-27.

Replace unnecessary wrapper widgets/manual layout/model duplication with standard Qt controls, layouts and existing model/delegate facilities. Retain one compact theme, existing previews and visual hierarchy. Avoid a replacement generic widget/service framework. Keep concurrent operations, scene-ready child handshake and path override contracts; native crash readiness validation remains distinct from post-crash monitoring.

**Exit evidence:** Compare retained pages/states at the three DPI settings and exercise keyboard focus/errors. Build/cook/sync/clean and Editor/Game launch work from source and prepared package. No developer SDK absence blocks reviewer runtime readiness; actual scene readiness still detects startup failure. Record final source/API/state/branch reduction and remaining largest files.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
