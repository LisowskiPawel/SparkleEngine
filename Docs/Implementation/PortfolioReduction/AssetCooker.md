# AssetCooker: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Make selected Showcase asset preparation a short explicit parallel workflow, with one shader sequencer and one texture request contract.

**Retain:** Incremental source/output identity, bounded parallel asset/texture work, accepted-output publication, selected scenes and supervised TextureCooker process.

**Target flow:** Launcher selects Showcase/profile/scopes -> AssetCooker resolves explicit prepared scene inputs -> importer/cooker work -> TextureCooker request stage -> complete accepted publication. Launcher invokes ShaderCompiler separately.

Dominant execution owner for: AC-01, AC-02, AC-03, AC-04, AC-05, AC-06, AC-07. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **30 files / 1,550 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **1 files / 24 lines**; assigned shared spans: **35 lines in 1 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| AC-03 | 7 files / 285 lines |
| AC-04 | 2 files / 462 lines |

## Stage 0: freeze the actual responsibility and smaller target

Trace CLI, CookProjectContent, ProjectCookPlanner/source probing, scene/texture stages and actual Launcher commands. Launcher calls cook-assets and cook-textures today; AC-03 must not delete those active endpoints blindly. Identify its independent cook-project/cook-shaders contract and the single-scene Knight preparation handoff.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Cooking/AssetCooker/Private/Cli/AssetCookerCli.cpp](../../../Tools/Cooking/AssetCooker/Private/Cli/AssetCookerCli.cpp)
- [Tools/Cooking/AssetCooker/Private/Planning/ProjectCookPlanner.cpp](../../../Tools/Cooking/AssetCooker/Private/Planning/ProjectCookPlanner.cpp)
- [Tools/Launcher/SparkleLauncher/Private/Cook/CookOperationProcessRequests.cpp](../../../Tools/Launcher/SparkleLauncher/Private/Cook/CookOperationProcessRequests.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Remove project/category/argument generality and duplicate shader sequencing

Decisions: AC-03, AC-04, AC-05.

Fix the sole product to Showcase and use a small explicit CLI. Retire ALL/discovery, permissive positional permutations and unused fine-grained subsets; retain/update the active cook-assets/cook-textures callers together. Remove AssetCooker-owned shader stage and narrow cook-project/cook-shaders promises; Launcher already sequences ShaderCompiler. Keep one working Cook All and noninteractive Editor/Launcher integration.

**Exit evidence:** CLI validation/help and Launcher callers agree on one grammar; full/asset/texture/shader actions reach the correct retained owners. Failed stages preserve accepted outputs. No second project discovery or duplicate shader runner remains.

## Stage 2: Resolve explicit sources and one prepared scene product

Participation boundary: execute the shared one-product migration inside **SceneCooker stage 2**, which owns the complete vertical cut. This stage owns its explicit resolver/project-recipe details and verifies its consumers; it does not start a second composition migration. Close both affected stages against the same qualified source identity when their distinct exits pass.

Decisions: AC-07.

Replace extension probing with explicit supported paths. With SceneCooker stage 2/Showcase, consume the fixed Knight recipe and one prepared scene per level. Update all source admission/catalog/cook references together; preserve stable identity and relocatable contained paths. Remove old ambiguity/probing policy after consumers migrate, with no compatibility aliases.

**Exit evidence:** Every nonempty retained level resolves deterministically; missing/ambiguous/escaping inputs fail clearly. Single Knight product contains base/Curtains/animation, not just the FBX. Incremental invalidation includes each required source, texture and preparation setting.

## Stage 3: Consolidate parallel preparation/publication

Decisions: AC-01, AC-02, AC-06.

Remove redundant orchestration/state surfaced by stages 1/2. Retain parallel independent preparation, one supervised TextureCooker request-file stage, bounded cancellation/settlement and complete publication. One operation/process never implies one worker or global serial cooking.

**Exit evidence:** Check full then no-op/one-input-changed cook, interrupted child, invalid source and delayed readers. Existing accepted scene/texture outputs remain valid on failure; no-op avoids actual preparation without trusting a past success flag. Account for end-to-end source reduction across Launcher/AssetCooker, not just moved functions.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
