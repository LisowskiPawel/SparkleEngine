# Platform: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Keep the narrow Windows SDL3 input/window adapter and its selected delivery modes; avoid rebuilding a platform layer during engine reduction.

**Retain:** SDL3 backend, windowed/borderless modes, fixed input layers and immediate/deferred delivery, focus-release/cancellation semantics, native window/surface integration and custom Editor hit testing.

**Target flow:** Native event/window affinity -> fixed input routing -> UI/camera/game owner; native surface handles feed backend-specific RHI composition.

Dominant execution owner for: PL-01, PL-02, PL-03, PL-04. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **25 files / 1,783 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **0 files / 0 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

## Stage 0: freeze the actual responsibility and smaller target

Trace InputSystem/window ownership and actual Editor hit-test/surface callers. Distinguish selected input delivery from the removed operation-history or CPU execution mode selectors. Freeze focus/drag/resize and input-routing behavior.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Engine/Platform](../../../Engine/Platform)
- [Engine/Editor/Private/Viewport](../../../Engine/Editor/Private/Viewport)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Reconcile platform consumers and retain the fixed adapter

Decisions: PL-01, PL-02, PL-03, PL-04.

Update consumers only where App/Editor/RHI system cuts changed contracts. Remove last-consumer forwarding after proof, retaining both fixed input delivery paths and custom title-bar hit testing. No SDL replacement, generic input plugin system, new OS backend or serialized input bottleneck is authorized.

**Exit evidence:** Check focus loss releases, UI capture, viewport camera routing, deferred delivery lifetime, resize, windowed/borderless behavior and native surfaces on both APIs at supported DPI. Prior isolated platform results do not qualify all DPI/hardware cases. There is no independent approved system deletion or promised net saving in this all-retained module.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
