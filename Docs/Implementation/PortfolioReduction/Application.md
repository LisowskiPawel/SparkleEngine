# Application hosts: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Keep thin Editor/Game composition, fixed threaded production rendering and native diagnostics; remove selectable execution modes without deleting execution capacity.

**Retain:** Editor/Game hosts, command-line CVars, settings/adapter preferences with explicit launch precedence, Editor restart, viewport BMP screenshots, accepted-output shader reload, native crash mode and required task/process lifetimes.

**Target flow:** Host composition supplies roots/execution resources -> game-thread world work -> bounded immutable frame handoff -> separate render-thread execution/recording -> queue-fence retirement.

Dominant execution owner for: AP-01, AP-02, AP-04, AP-05, AP-06, AP-08, AP-09, AP-10, AP-11.

Delegated complete systems: AP-03 -> [Renderer](Renderer.md); AP-07 -> [Launcher](Launcher.md). Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **60 files / 5,373 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **0 files / 0 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| AP-11 | 19 files / 2,041 lines |

## Stage 0: freeze the actual responsibility and smaller target

Trace GameApplicationExecution and RendererExecutionConfig, frame queue and startup preferences. Record fixed production threading/frame-lead policy before removing r.ThreadedRenderer/r.RenderPipelineDepth. Identify ordinary screenshot encoding versus Renderer-owned Reference encoding. Launcher owns extra incident-management retirement, not native collectors.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Engine/Application/Private/Game/GameApplicationExecution.cpp](../../../Engine/Application/Private/Game/GameApplicationExecution.cpp)
- [Engine/Renderer/Private/Concurrency/RendererExecutionConfig.cpp](../../../Engine/Renderer/Private/Concurrency/RendererExecutionConfig.cpp)
- [Engine/Application/Private/Editor/Capture/ImageEncoding.cpp](../../../Engine/Application/Private/Editor/Capture/ImageEncoding.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Fix production execution policy and remove diagnostic selectors

Decisions: AP-11.

Remove runtime/UI/CLI/CVar selection of CPU render-thread mode and frame lead, then delete unused alternate-mode branches. Preserve separate game/render owners, bounded frame handoff, parallel frame preparation and recording, actual worker execution and orderly shutdown. Pick the already intended threaded production policy; do not force single-thread or one-frame global drains to reduce branches.

**Exit evidence:** Trace game/render overlap and independent worker jobs on retained scenes. Repeated startup, switch, resize and shutdown do not leak/stall or race. Frame time, latency and peak memory are recorded against the frozen threaded baseline. Both APIs retain multiple queue/fence behavior.

## Stage 2: Narrow composition after cross-module removals

Decisions: AP-01, AP-02, AP-04, AP-05, AP-08, AP-09, AP-10.

After Renderer Reference and Launcher incident cuts, remove unused host forwarding/settings/encoding fields and redundant bootstrap state. Feed Core/ToolSupport roots from explicit product composition. Preserve normal BMP capture, native restart, shader accepted-output reload and adapter preference validation. Native fault collection/marker sidecars stay alive until completion even if UI is gone.

**Exit evidence:** Editor compile failure leaves accepted shaders usable; ordinary capture/restart/preferences remain correct. Source-only actions are unavailable cleanly in reviewer products. Triggered native fault data remains correlated with this exact native/shader build; no source/build success is substituted for that qualification.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
