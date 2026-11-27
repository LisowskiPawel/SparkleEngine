# Renderer: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Keep one focused real-time path-traced renderer and its owned frame graph; remove the independent offline/reference renderer and unused presentation modes across the repository.

**Retain:** Frame graph, raster and ray-traced GBuffer, inline/ray-pipeline shader paths, ReSTIR indirect, TLAS refit, DLSS SR/RR, subsurface, alpha/two-sided parity, automatic/manual exposure and signal views. Preserve separate render-thread ownership, parallel preparation/recording and queue overlap.

**Target flow:** Game-owned immutable scene deltas -> parallel render preparation -> owned frame graph -> native RHI contexts/queues -> checked real-time output and fence retirement.

Dominant execution owner for: AP-03, RE-01, RE-02, RE-03, RE-04, RE-05, RE-06, RE-07, RE-08, RE-09, RE-10, RE-11, RE-12, RE-13, RE-14, RE-15. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **619 files / 36,806 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **53 files / 3,095 lines**; assigned shared spans: **57 lines in 1 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| AP-03 | 2 files / 310 lines |
| RE-07 | 2 files / 521 lines |
| RE-13 | 8 files / 132 lines |

## Stage 0: freeze the actual responsibility and smaller target

Map every ReferencePathTracer type, render mode, session/accumulation/checkpoint/export setting, shader registration and Application/Editor caller. Classify shared EXR/readback/capture/lighting code by its remaining real-time consumer; do not remove TinyEXR or general readback by folder label. Freeze representative real-time raw signals/camera/settings before touching mode selection.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Engine/Renderer/Private/Passes/Lighting/ReferencePathTracer](../../../Engine/Renderer/Private/Passes/Lighting/ReferencePathTracer)
- [Engine/Application/Private/Editor/ReferencePathTracer](../../../Engine/Application/Private/Editor/ReferencePathTracer)
- [Engine/Editor/Private/Viewport/ReferencePathTracer](../../../Engine/Editor/Private/Viewport/ReferencePathTracer)
- [Engine/Assets/Shaders/Passes/Lighting/ReferencePathTracer](../../../Engine/Assets/Shaders/Passes/Lighting/ReferencePathTracer)
- [Engine/Application/Private/Editor/Capture/ImageEncoding.cpp](../../../Engine/Application/Private/Editor/Capture/ImageEncoding.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Delete the independent Reference renderer as one system

Decisions: RE-04, AP-03.

Own cross-module deletion of sessions, scheduling/accumulation/convergence/checkpoint/resume/artifact state, dedicated render mode and UI, shaders/resources, registrations, export encoders and expected cook/package/validation outputs. Update callers and sole remaining real-time composition directly; no disabled enum or validation-only copy. Delete dedicated Reference code through Application/Editor/ShaderCompiler/build owners in the same complete removal chain.

**Exit evidence:** Search/type/build/cook inventories show no executable Reference route, dedicated files or output promises. Retained real-time lighting and ordinary screenshots/debug symbols still work on both APIs. Correctness controls use bounded analytic/external comparisons, not another Sparkle renderer. Clean stale cooked/package outputs so old shaders cannot mask incomplete deletion.

## Stage 2: Remove advanced HDR display and lens effects

Decisions: RE-13, RE-14.

Keep one explicit SDR presentation/output-color policy. Remove HDR display selection/metadata/branches and vignette/chromatic-aberration parameters, shader stages/settings/UI/defaults. Preserve EXR/HDR lighting inputs, internal linear/HDR math, sky environments and ordinary screenshot output. Keep resource transitions and CPU/shader ABI in sync.

**Exit evidence:** No removed display/lens setting survives a profile or persisted option. SDR output, exposure/reconstruction and raw signal checks match defined tolerances on both backends. Runtime image parity is checked after regenerated shaders, not inferred from a host build.

## Stage 3: Remove automatic mesh batching only through its cost gate

Decisions: RE-07.

Keep explicit authored instance groups and shared geometry/material resources; remove only automatic grouping/cache/planner policy if the chosen direct path stays within fixed retained-scene CPU/GPU/memory budgets. Preserve data-oriented instance packets, parallel extraction/recording and stable object identity. If a smaller parallel equivalent cannot meet the gate, retain the necessary optimized mechanism and leave this stage open.

**Exit evidence:** Compare Knight and Bistro geometry counts, draw/dispatch/build costs, CPU/GPU frame distributions and peak memory with equivalent images. Removing automatic batching cannot imply per-instance geometry duplication, serial loops or deleting explicit group support.

## Stage 4: Simplify the surviving real-time frame path

Decisions: RE-01, RE-02, RE-03, RE-05, RE-06, RE-08, RE-09, RE-10, RE-11, RE-12, RE-15.

Collapse mode-routing/state wrappers and repeated representations made obsolete by stages 1/2. Keep frame-graph ownership, resource/queue lifetime and the selected algorithms. Repair surviving producer/consumer boundaries through RHI and ShaderAssets without a renderer rewrite or new rendering SDK feature campaign.

**Exit evidence:** A specialist can trace GBuffer -> transport/reuse -> reconstruction -> exposure -> SDR and identify every resource/history reset/queue edge. Preserve failed and inconclusive comparisons. Current real-time image/Vulkan startup/material-parity issues remain open until actually observed fixed; removal builds do not close them.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
