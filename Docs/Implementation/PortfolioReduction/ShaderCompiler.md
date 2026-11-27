# ShaderCompiler: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Keep one correct incremental parallel shader tool with DXC/Slang and matched native debug data; remove optional reports and redundant source/export mechanisms only when equivalence is proven.

**Retain:** DXC and Slang, target enumeration, shader-id/changed/include closure selection, bounded parallel sessions, runtime reflection/host ABI checks, accepted-output publication, symbols/source identity and required native exports.

**Target flow:** Typed registered shaders + source/include identity -> independent compiler sessions -> checked code/reflection/debug products -> one accepted publication -> runtime reload and native debugging.

Dominant execution owner for: CP-01, CP-02, CP-03, CP-04, CP-05, CP-06, CP-07, CP-08, CP-09, CP-10. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **104 files / 7,938 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **8 files / 213 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| CP-04 | 6 files / 560 lines |
| CP-08 | 2 files / 260 lines |
| CP-09 | 2 files / 49 lines |

## Stage 0: freeze the actual responsibility and smaller target

Trace CLI selectors, ValidateShaderCompilerCli.cmake, mount/preprocessing identity, ShaderMake invocation and ShaderArtifactPublication. The validation target calls reports/analysis/shader-id/changed; optional UI removal does not mean unreferenced code. Preserve source paths/private source/export bytes required by Aftermath and Slang patches.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Shaders/ShaderCompiler/Private/Cli/ShaderCookOptions.cpp](../../../Tools/Shaders/ShaderCompiler/Private/Cli/ShaderCookOptions.cpp)
- [Tools/Shaders/ShaderCompiler/ValidateShaderCompilerCli.cmake](../../../Tools/Shaders/ShaderCompiler/ValidateShaderCompilerCli.cmake)
- [Tools/Shaders/ShaderCompiler/Private/Compiler/ShaderSourcePreprocessor.cpp](../../../Tools/Shaders/ShaderCompiler/Private/Compiler/ShaderSourcePreprocessor.cpp)
- [Tools/Shaders/ShaderCompiler/Private/Cooking/Publication/ShaderArtifactPublication.cpp](../../../Tools/Shaders/ShaderCompiler/Private/Cooking/Publication/ShaderArtifactPublication.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Remove optional analysis/catalog presentation and consolidate export CLI

Decisions: CP-03, CP-08, CP-09.

Delete stand-alone analysis/stats/catalog/inspection command presentation and exclusive writers; keep ABI/registration validation as a real compiler invariant and update the existing validation target. Use --debug-info as the public symbols switch; retire --debug-artifacts only after its exact code/source/fresh-output guarantees live in the shared publisher. Update all Launcher/diagnostic/script callers; do not remove the shader-target enum.

**Exit evidence:** Full, shader-id and changed-source cooks still pass deterministic/invalid-selector/accepted-output checks. Both target contracts and symbols/source exports remain matched; stale debug identity is rejected. Updated validation checks retained behavior rather than expecting removed report text.

## Stage 2: Replace first-party preprocessing only through proven compiler reuse

Decisions: CP-04.

Investigate a smaller ShaderMake/DXC/Slang-owned include path with a minimal virtual-path adapter only if required. Preserve source mount containment/canonicalization, include closure/invalidation, defines/entry identity, diagnostics and debug-source mapping. Keep parallel isolated compiler sessions. Remove the old machinery/callers together after a candidate is both smaller end to end and semantically equivalent; no permanent dual path or speculative include service.

**Exit evidence:** Compile representative shared/nested includes and retained stages for DXIL/SPIR-V through both kept backends where supported. Changed includes select exactly affected shaders; escaping/missing includes fail without replacing accepted outputs. Source correlation/native debugging works at this exact toolchain identity. If the contract is not met, keep the minimum current path and leave conditional retirement open.

## Stage 3: Close retained compiler/publication boundaries

Decisions: CP-01, CP-02, CP-05, CP-06, CP-07, CP-10.

Consolidate duplicate code/reflection/debug identity representations left by stages 1/2. Keep typed ABI data with its real consumers, bounded parallel compile jobs and correct accepted-output locks/journals. Remove Reference/morph/PTLAS shader registrations only through the corresponding system owner.

**Exit evidence:** Parallel repeated/incremental cooks and Editor reload remain deterministic; consumers cannot read incomplete products. Exact code/PDB/source/debug identifiers survive both APIs and retained GPU diagnostic workflows. Net reduction includes any replacement adapter, altered validation and publication code.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
