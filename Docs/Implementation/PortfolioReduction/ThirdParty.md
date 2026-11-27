# Third-party integration maintenance: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Retain production libraries and necessary maintained behavior while retiring only exclusive patches/dependencies after real consumers disappear.

**Retain:** Selected logging/JSON/process/tasks/window/UI/allocation/import/animation/shader libraries and required patches. Keep enkiTS parallel scheduling/settlement behavior and pinned Slang source/debug fixes until equivalent behavior is verified.

**Target flow:** Pinned dependency/patch identity -> compiled binding and narrow integration -> actual retained consumer -> runtime/build/notice closure.

Dominant execution owner for: TP-01, TP-02, TP-03, TP-04.

Delegated complete systems: TP-05 -> [RHI](RHI.md). Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **33 files / 1,078 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **0 files / 0 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| TP-03 | 3 files / 790 lines |

## Stage 0: freeze the actual responsibility and smaller target

Trace all seven maintained patches to live consumers and the actual installed/prepared compiler bytes. Do not infer a patch is redundant from absence of .slang assets. Separate scheduler extensions from native jobs/lifetime semantics and dependency ABI/global/allocator behavior across DLLs. NVAPI retirement is owned only by RHI stage 2.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [ThirdParty/enkits/RuntimeSource.cmake](../../../ThirdParty/enkits/RuntimeSource.cmake)
- [ThirdParty/enkits/Patches/enkiTS-sparkle-runtime.patch](../../../ThirdParty/enkits/Patches/enkiTS-sparkle-runtime.patch)
- [ThirdParty/slang/Patches/SlangDebugInformationFormat.patch](../../../ThirdParty/slang/Patches/SlangDebugInformationFormat.patch)
- [ThirdParty/slang/Patches/SlangLoopSourceLocations.patch](../../../ThirdParty/slang/Patches/SlangLoopSourceLocations.patch)
- [ThirdParty/README.md](../../../ThirdParty/README.md)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Retire only unused enkiTS extensions after consumers vanish

Decisions: TP-03.

After GameFramework/Launcher/frame owners narrow their state, audit each maintained extension against all remaining calls. Remove only an extension with no retained consumer or a smaller verified native equivalent, then update patch/source hashes and recipe validation. Keep ranges, dependencies, cancellation/failure propagation, task argument ownership, external-thread registration and physical completion. No scheduler replacement or thread-pool collapse is approved.

**Exit evidence:** Real Launcher, scene-load, parallel world/render and shutdown consumers pass capacity/settlement/cancel checks. Patch reapplication/source qualification works from clean prepared dependency source. No unaccounted worker, dangling native argument or added global scheduling lock; entire fork saving is not claimed.

## Stage 2: Close retained dependency/patch/ABI inventory

Decisions: TP-01, TP-02, TP-04.

Keep required patches/forks, including Slang debug/source fixes, while pruning only dependencies whose owning system closes last-consumer removal. Check recipes/bindings/notices and conditional DLL runtime/allocator/global requirements; do not impose one static/shared rule on every vendor. Do not add new SDK algorithms as part of cleanup.

**Exit evidence:** Prepared source/patch bytes and actual runtime DLL inputs are identified for the selected toolchain. Slang symbols/source and native diagnostic exports are usable; independent library tests are not a substitute for Sparkle integration. Required libraries remain attributed and removal savings are credited to the system owner once.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
