# Shader assets and rendering data: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Retain the selected shader/math/data capabilities with clear consumer ownership; consume system removals without creating duplicate shader routes.

**Retain:** Inline queries and RT pipelines used by the frame, shader parameter/layout contracts, wireframe/instance views, retained material/subsurface math and built-in environment images selected by SH-03. No independent shader-asset purge is approved.

**Target flow:** Owned shader registrations and shared typed layouts -> compiler contracts -> cooked code/reflection -> matching Renderer/RHI consumers.

Dominant execution owner for: SH-01, SH-02, SH-03. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **no independent authored-code baseline in this selection; shader/data/docs are separate**. Assigned cross-module whole-file retirement candidates: **0 files / 0 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

## Stage 0: freeze the actual responsibility and smaller target

Trace source files to registrations, parameter layouts, generated/cooked outputs and retained runtime consumers. Note only one sky EXR is in the current default cook; preserve the agreed capability without claiming all four images are wired. Reference and morph deletions are owned by Renderer/GameFramework, not a second local task.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Engine/Assets/Shaders](../../../Engine/Assets/Shaders)
- [Engine/Assets/Textures/Sky](../../../Engine/Assets/Textures/Sky)
- [Engine/Renderer/Private/Passes/GBuffer/RayTracing/RayTracingGBufferShaderRegistrations.cpp](../../../Engine/Renderer/Private/Passes/GBuffer/RayTracing/RayTracingGBufferShaderRegistrations.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Reconcile retained shader/data contracts after owned system cuts

Decisions: SH-01, SH-02, SH-03.

Consume Renderer Reference/PTLAS/presentation and GameFramework morph removals; eliminate only their exclusive includes/fields/registrations and regenerate code/layout products. Keep HLSL/Slang/DXIL/SPIR-V paths and useful shared functions with one authority. Remove orphan forwarding math only after actual last-consumer proof, not because a scene was removed.

**Exit evidence:** Both shader targets compile with ABI/reflection checks and retained debug correlation. Raster/ray GBuffer and material/instance/debug views render correctly. Report zero independent deletion allowance here; cross-module shader savings belong to their system owners.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
