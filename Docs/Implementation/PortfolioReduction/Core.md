# Core: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Keep neutral mechanisms and make product layout authority small and explicit; remove Windows-irrelevant policy without turning Core into product orchestration.

**Retain:** CVar execution, category logging, events/subscriptions, child-process supervision, path containment/publication, UTF-8/JSON/hash mechanisms and their concurrency/lifetime contracts.

**Target flow:** Host/product composition selects roots -> neutral Core values/mechanisms validate and operate -> domain owners interpret products/results. Core has no Editor/Renderer dependency.

Dominant execution owner for: CO-01, CO-02, CO-03, CO-04, CO-05, CO-06. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **127 files / 7,278 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **0 files / 0 lines**; assigned shared spans: **49 lines in 9 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| CO-05 | 3 files / 202 lines |
| CO-06 | 23 files / 1,128 lines |

## Stage 0: freeze the actual responsibility and smaller target

Trace engine/project/cooked-root discovery to host startup, Launcher, tools, resource resolution and relocated package consumers. Separate portable byte/path mechanisms from Showcase-specific selection. Count the proposed smaller API, retained storage and host composition before approving a move; relocating the same 969 lines is not a win.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Engine/Core/Public/Paths/FileSystemPaths.h](../../../Engine/Core/Public/Paths/FileSystemPaths.h)
- [Engine/Core/Private/Paths](../../../Engine/Core/Private/Paths)
- [Engine/Core/Private/Process](../../../Engine/Core/Private/Process)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Remove unused non-Windows state-root branches

Decisions: CO-05.

Delete XDG/HOME/non-Windows fallback branches only where the selected Windows workflow has no consumer. Preserve Windows discovery, Unicode/relocated paths and useful failure behavior; no platform rewrite or thread-safety relaxation.

**Exit evidence:** Windows state/source/cooked/log paths work with spaces, missing roots and relocation. Retained error paths remain explicit. Only measured removable spans are savings; containing shared files stay.

## Stage 2: Replace product-root discovery with one bounded authority

Decisions: CO-06.

With ToolSupport/host composition, pass the minimum explicit engine/Showcase/cooked roots to actual consumers or retain a smaller existing root owner. Remove duplicate discovery/project scans and forwarding state. Keep one layout contract for source and extracted products, no universal path service or per-module mirrored roots. Do not delete Core primitives merely because policy moves.

**Exit evidence:** Before retiring the old path, demonstrate fewer APIs/state/lines across Core plus new composition/callers. Relocated Launcher/Editor/Game, tools, content reads and path overrides resolve correctly without source prerequisites for reviewer runs. Concurrent readers see initialized immutable roots; initialization is not a global hot-path lock.

## Stage 3: Close neutral contract and DLL caller checks

Decisions: CO-01, CO-02, CO-03, CO-04.

Apply Editor console-completion and Launcher history/incident removals only through their owning stages. Remove genuinely unconsumed helpers and keep readable neutral contracts; preserve process cancellation/settlement, file publication guards and category logger behavior across DLL boundaries.

**Exit evidence:** Exercise a real DLL consumer, UTF-8/error paths and child failure/cancellation/output draining. Failed cooker publication preserves accepted files. No duplicate roots/event/log/process infrastructure survives under new names.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
