# MeshCooker: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Keep a small checked mesh-writing library; consume removal of obsolete payloads without pretending that merging the library removes mesh preparation.

**Retain:** Distinct cohesive library, geometry/index validation, skeletal data and tangent frames, stable resource reuse and parallel caller preparation.

**Target flow:** Checked imported geometry -> narrow mesh record writer -> cooked resource -> level-owned runtime mesh -> Renderer/RHI.

Dominant execution owner for: MC-01. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **7 files / 377 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **0 files / 0 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

## Stage 0: freeze the actual responsibility and smaller target

Trace public writer/build input to SourceImporters, SceneCooker and runtime mesh readers. Separate morph payloads from skeleton joints/weights and from ordinary static vertices. Measure only this owner, keeping generated/binary geometry out of authored code counts.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Cooking/MeshCooker](../../../Tools/Cooking/MeshCooker)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Reconcile retained mesh contract with vertical removals

Decisions: MC-01.

Participate in GameFramework morph and SceneCooker single-product/animation stages: remove only exclusive morph/duplicate record fields and adapters when those owners migrate all readers. Keep direct checked writing, required skin/previous-frame inputs and resource sharing. Collapse forwarding copies only where it removes an actual representation; do not merge the selected library for cosmetic file savings.

**Exit evidence:** Static and skinned retained meshes preserve indices, tangent frames, joint weights and resource identity. Invalid input leaves accepted products unchanged. Multiple independent meshes still prepare in parallel. No independent net saving is promised; any deleted mesh payload is credited once to its system owner.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
