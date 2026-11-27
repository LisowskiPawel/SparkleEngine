# TextureCooker: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Trim unused input/report support while retaining a correct standalone parallel texture pipeline and its explicit quality policies.

**Retain:** Standalone request-file cooking, validation/failure behavior, GPU BC processing, DDS mip preservation, PNG/JPEG and HDR/EXR inputs, RGBA32F default and BC6H opt-in, native texture dimensions/channel/color contracts.

**Target flow:** Asset/MaterialCooker checked request -> source decode -> parallel processing/mips/compression -> validated complete output -> accepted publication.

Dominant execution owner for: TX-01, TX-02, TX-04, TX-05, TX-06, TX-07, TX-08. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **30 files / 2,019 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **2 files / 49 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| TX-05 | 2 files / 200 lines |
| TX-06 | 1 files / 149 lines |

## Stage 0: freeze the actual responsibility and smaller target

Recheck retained external and embedded image references after importer decisions. Identify shared stb/WIC dispatch: PNG/TGA still use stb, JPEG still uses WIC. Do not count those libraries as removable. KTX retirement is already complete and excluded. Inspect manual-request BC6H versus automatic default RGBA32F, not just EXR suffixes.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Cooking/TextureCooker/Private/SourceLoading/TextureSource.cpp](../../../Tools/Cooking/TextureCooker/Private/SourceLoading/TextureSource.cpp)
- [Tools/Cooking/TextureCooker/Private/Processing/TextureProcessor.cpp](../../../Tools/Cooking/TextureCooker/Private/Processing/TextureProcessor.cpp)
- [Tools/Cooking/TextureCooker/Public/TextureCookRequestList.h](../../../Tools/Cooking/TextureCooker/Public/TextureCookRequestList.h)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Remove unused input admissions and request reporting

Decisions: TX-05, TX-06, TX-07.

Remove PSD/PIC/PNM-family and BMP/GIF source admission plus exclusive handling and the inspect-request-file command/presentation. Keep the actual cook-request-file codec/validation/help/errors. Reject unsupported formats explicitly before publication; keep BMP screenshots and shared JPEG/stb users separate.

**Exit evidence:** Retained PNG/JPEG/DDS/HDR/EXR requests still cook with correct mip/color/channel output. Removed inputs/command are rejected without corrupting accepted output. Dispatch-only edits may save zero physical lines; report that honestly. Existing KTX absence is a guard, not new savings.

## Stage 2: Simplify retained parallel processing/request ownership

Decisions: TX-01, TX-02, TX-04, TX-08.

Remove duplicate format/property/policy state uncovered by stage 1. Keep RGBA32F default and explicit BC6H quality fallback/opt-in, bounded parallel preparation and one request format shared with AssetCooker. No new texture-container/format framework or blanket decoder replacement.

**Exit evidence:** Check dimensions, HDR range/non-finite rejection, BC quality, normal orientation and preserved DDS mips with identified controls. Cancellation/child failure preserves accepted products; no-op cook and actual prepared requests agree. Benchmark throughput/peak memory only if this stage changes processing/concurrency.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
