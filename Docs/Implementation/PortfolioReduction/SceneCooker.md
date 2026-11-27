# SceneCooker: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Prepare the exact runtime scene and animation products offline, eliminating runtime authoring/general composition without removing parallel loading or playback.

**Retain:** Distinct library boundary, checked scene metadata, source cameras/lights, selected skeleton/clip data, reusable resource identities and correct dependency records.

**Target flow:** Checked imported sources + fixed Showcase preparation -> one flattened checked scene/ready Ozz runtime records -> scene registry/manifest -> parallel runtime loading.

Dominant execution owner for: GF-13, GF-14, SC-01, SC-02, SC-03. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **30 files / 1,430 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **0 files / 0 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| GF-13 | 6 files / 604 lines |
| GF-14 | 6 files / 536 lines |
| SC-03 | 2 files / 162 lines |

## Stage 0: freeze the actual responsibility and smaller target

Read CookedSceneBuild, instance/metadata writers and animation bake/resample flow, plus GameFramework runtime Ozz construction. Before flattening Knight, list every rebased mesh/material/node/camera/light/skeleton/clip index, texture reference and coordinate/unit transform. Record the current separate base/Curtains/Knight products and their loaded result.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Cooking/SceneCooker/Public/CookedSceneBuild.h](../../../Tools/Cooking/SceneCooker/Public/CookedSceneBuild.h)
- [Tools/Cooking/SceneCooker/Private/Instances/CookedSceneInstanceBuilder.cpp](../../../Tools/Cooking/SceneCooker/Private/Instances/CookedSceneInstanceBuilder.cpp)
- [Tools/Cooking/SceneCooker/Private/Metadata/CookedSceneMetadataBuilder.cpp](../../../Tools/Cooking/SceneCooker/Private/Metadata/CookedSceneMetadataBuilder.cpp)
- [Engine/GameFramework/Private/World](../../../Engine/GameFramework/Private/World)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Move Ozz preparation into cooking and narrow resampling

Decisions: GF-13, SC-03.

Own the cross-module change through imported/cooked contracts, Ozz dependency linkage and GameFramework playback. Cook ready runtime skeleton/animation data once; keep only sampling needed by retained curves/clips. Retire runtime offline builders and duplicate conversion, preserving names/associations and parallel preparation/evaluation. Do not invent versioned compatibility readers: regenerate disposable cooked data.

**Exit evidence:** Knight/Cesium import/cook/playback, duration, joint association, bind pose and animated RT geometry match defined controls. If IM-08/IM-09 equivalence fails, retain required sampling. Runtime no longer links/calls unused Ozz offline builders; cold load time and memory are measured. Cook-time additions are subtracted from runtime deletions.

## Stage 2: Deliver one prepared Knight scene and remove runtime multi-asset composition

Decisions: GF-14.

Own the vertical cut through Showcase fixed preparation, AssetCooker, runtime level/loader and relevant Editor saved identities. Use a small project-owned fixed recipe over base, Curtains and Knight; no general recipe language, streaming framework or runtime add-on registry. Flatten/rebase all references and preserve skeletal animation, imported cameras/lights, materials/UVs, units and authored transforms. Runtime levels then reference one prepared scene product; remove general multi-asset document/list/loading branches after all retained maps migrate.

**Exit evidence:** A reproduced Knight product contains complete environment/curtains/animated character and loads on both APIs. Resource/index/material/light counts, transforms and animation are checked; name collisions cannot silently remap a bone/material. A changed constituent input invalidates the combined product. No runtime multi-asset route survives as a fallback; one product still loads/decode in parallel.

## Stage 3: Close checked metadata/library ownership

Decisions: SC-01, SC-02.

Keep cohesive scene-writing and imported-camera/light metadata contracts. Remove builders/record copies whose only consumers were morph/old composition/runtime preparation. Coordinate with MeshCooker/MaterialCooker without merging their explicitly retained libraries or adding a general asset serialization layer.

**Exit evidence:** Malformed/truncated/index-invalid data is rejected before accepted publication; runtime readers use exactly the cooked layout produced here. Reviewer can trace source object/joint identity to one scene record and the corresponding runtime resource.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
