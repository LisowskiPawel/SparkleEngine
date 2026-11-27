# ToolSupport: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Remove a general project/content browser model while retaining one shared tool startup and the exact Showcase acquisition/layout contract.

**Retain:** Shared startup/errors/progress and source/output format contracts, retained scene identities and required source/download/error explanations. Keep parallel tool work and actual artifact observation.

**Target flow:** Showcase explicit catalog and product roots -> retained source/pack dependency facts -> Launcher/cook consumers. Runtime readiness reads prepared outputs, not acquisition history.

Dominant execution owner for: TS-01, TS-02, TS-03, TS-04. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **20 files / 1,428 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **0 files / 0 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| TS-02 | 5 files / 524 lines |
| TS-03 | 8 files / 1,053 lines |
| TS-04 | 6 files / 760 lines |

## Stage 0: freeze the actual responsibility and smaller target

Trace ProjectLevelCatalog reader/validator, Core root helpers, Launcher project and content resolution, cook planning and CMake project discovery. Separate family/variant presentation from actual base/Curtains/Knight dependency edges. Existing catalog still contains Candles until Showcase stage 1; do not confuse checkout data with the ten-map target.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Support/ToolSupport/Public/ProjectLevelCatalog.h](../../../Tools/Support/ToolSupport/Public/ProjectLevelCatalog.h)
- [Tools/Support/ToolSupport/Private/Projects/ProjectLevelCatalog.cpp](../../../Tools/Support/ToolSupport/Private/Projects/ProjectLevelCatalog.cpp)
- [Tools/Support/ToolSupport/Private/Projects/ProjectLevelCatalogValidator.cpp](../../../Tools/Support/ToolSupport/Private/Projects/ProjectLevelCatalogValidator.cpp)
- [Tools/Support/ToolSupport/Private/SourceAssetPaths.cpp](../../../Tools/Support/ToolSupport/Private/SourceAssetPaths.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Retire general multi-project discovery as one cross-module system

Decisions: TS-02.

Fix user-product selection to Showcase across ToolSupport, Core/product composition, Launcher, cookers, CMake and packaging. Remove project enumeration/selection/fallback state and duplicate discovery. Keep internal Engine/shared shader domains, standalone Launcher and required source/cooked root distinction; they are not extra user projects.

**Exit evidence:** Source and relocated reviewer products identify Showcase without scanning for another project. Explicit path overrides work; a missing developer source catalog does not block a prepared runtime. No hidden ALL/project selector remains in retained entry points; roots have one authority with Core stage 2.

## Stage 2: Flatten presentation and delete unused admission branches

Decisions: TS-03, TS-04.

Use explicit retained scene entries, removing unneeded family/add-on variant presentation fields and their UI/parser branches. Preserve actual Knight/base/Curtains preparation dependencies until its fixed recipe owns them, plus source availability/manual-download errors and integrity checks needed by retained packs. Delete policy only after its last consumer vanishes; removed catalog data alone is not proof of dead code.

**Exit evidence:** All ten selected scenes resolve/cook/launch and acquisition failures identify the real missing input. Dependency graph contains required Knight inputs, no retired Candles/future add-on route. Tool startup/progress remains shared and thread-safe rather than copied into each executable.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
