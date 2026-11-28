# ShaderCompiler: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

**Execution order: 13 of 21 (first focused visit).** Start: Stages 0-1 in Wave 4. Resume / dependency: Stages 2-3 in Wave 7 after the retained shader/pass set settles. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Keep one correct incremental parallel shader tool with DXC/Slang and matched native debug data; remove optional reports and redundant source/export mechanisms only when equivalence is proven.

**Retain:** DXC and Slang, target enumeration, shader-id/changed/include closure selection, bounded parallel sessions, runtime reflection/host ABI checks, accepted-output publication, symbols/source identity and required native exports.

**Target flow:** Typed registered shaders + source/include identity -> independent compiler sessions -> checked code/reflection/debug products -> one accepted publication -> runtime reload and native debugging.

Dominant execution owner for: CP-01, CP-02, CP-03, CP-04, CP-05, CP-06, CP-07, CP-08, CP-09, CP-10. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **Runtime-oriented products:** after optional analysis/catalog reporting and export-selector retirement, narrow CLI/request/result records and callers to selected code, reflection, debug and publication products. Remove report-centric stores/aggregation and obsolete selectors throughout validation/build users. Keep symbols and the live SPIRV-Tools disassembly/debug consumers.
- **Delegate supported mechanics:** keep compiler-specific include/preprocess/compile operations at the actual DXC/Slang boundary. Replace first-party preprocessing only through its qualified smaller path, then remove old intermediates and include-policy mirrors. Registered shader selection, independent sessions, source/include identity, typed ABI checks and accepted-output reload stay explicit and parallel.

**Structural acceptance:** run changed/include invalidation, symbols/source correlation, both target layouts and reload/cancel/failure publication checks on real callers. Show net savings including compiler adapters and debug products. A report-shaped runtime model, lost source identity or a shipped old/new preprocessing path fails; conditional failure leaves the sole authorized path and the stage open.

**Reference application:** [P4](README.md#reference-choices-and-limits) and the [DXC/Slang native API precedents](../../References.md#integration-precedents) guide mechanism reuse. Their outputs do not replace Sparkle's ABI or lifetime contracts.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **104 files / 7,938 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **8 files / 213 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| CP-04 | 6 files / 560 lines |
| CP-08 | 2 files / 260 lines |
| CP-09 | 2 files / 49 lines |

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| CP-03; stage 1 | Optional shader analysis/stats/report pipeline. Retire Analysis/CookedShaderStatsPass, --analysis parsing/registries and report-only counters/formatters; rewrite ValidateShaderCompilerCli.cmake count/report expectations around retained invariant checks. | Typed shader contracts/ABI/reflection, exact debug export; SPIRV-Tools disassembly has ShaderMakeOutputs/SpirVDisassembler debug consumers. |
| CP-08; stage 1 | Second debug export CLI route. Retire --debug-artifacts option/state and exclusive output-directory branches after --debug-info shared publication owns exact fresh code/source/symbol exports. Update Launcher/diagnostic/validation consumers and output help. | Native export manifests/identifiers, full debug source/PDBs and matched code; no deletion of writer mechanics still used by --debug-info. |
| CP-09; stage 1 | Optional backend/target/shader listing and inspection commands. Retire CLI registrations/presentation and exclusive inspection readers/output types; update list-backends/list-targets/list-shaders/inspect-shader validation callers and shared formatting after final use. | Backend/target enums/mappings, shader registration validation and runtime reflection/ABI; listing API internals may still resolve real compile jobs. |
| CP-04; stage 2 | Redundant owned include/preprocessor machinery. After proven native replacement, retire include expander/mount/resolver mirrors, source/path caches and duplicated closure identity only where the single compiler path owns the same contract. Recheck callers, validation and SDK staging. | Canonical source containment, include invalidation/diagnostics, private source/debug correlation, concurrent DXC/Slang sessions; no dual filesystem fallback. |

## Existing workspace validation entry points

In the configured root workspace, run `code_style_check` or `ShaderCompilerCliValidation` with `cmake --build build --config <Profile> --target <Target>`. The standalone Launcher lacks these targets. Shader validation requires `SPARKLE_ENABLE_SHADER_COMPILER=ON` and cooks DXIL/SPIR-V into Shared's cooked shader directory.

## Outstanding shader-tool product qualification

ShaderMake code/build/CLI checks do not qualify development Editor reload/cancel/close, D3D12/Vulkan pipelines or native shader-source correlation; verify retained developer functionality, compiler/SDK delivery for your workflow and the frozen compiler cost cells against the [bound qualification record](../../../build/validation/shadermake/stage-2/20261122-next/qualification.json) and [remaining constraints](../../../build/validation/shadermake/closeout-20261122/qualification-handoff.md). Do not require compiler/SDK setup merely to run the inspection bundle.

## Stage 0: freeze the actual responsibility and smaller target

Trace CLI selectors, ValidateShaderCompilerCli.cmake, mount/preprocessing identity, ShaderMake invocation and ShaderArtifactPublication. The validation target calls reports/analysis/shader-id/changed; optional UI removal does not mean unreferenced code. Preserve source paths/private source/export bytes required by Aftermath and Slang patches.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Shaders/ShaderCompiler/Private/Cli/ShaderCookOptions.cpp](../../../Tools/Shaders/ShaderCompiler/Private/Cli/ShaderCookOptions.cpp)
- [Tools/Shaders/ShaderCompiler/ValidateShaderCompilerCli.cmake](../../../Tools/Shaders/ShaderCompiler/ValidateShaderCompilerCli.cmake)
- [Tools/Shaders/ShaderCompiler/Private/Compiler/ShaderSourcePreprocessor.cpp](../../../Tools/Shaders/ShaderCompiler/Private/Compiler/ShaderSourcePreprocessor.cpp)
- [Tools/Shaders/ShaderCompiler/Private/Cooking/Publication/ShaderArtifactPublication.cpp](../../../Tools/Shaders/ShaderCompiler/Private/Cooking/Publication/ShaderArtifactPublication.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Implementation prompt:**

```text
Prepare Stage 0 of Docs/Implementation/PortfolioReduction/13_ShaderCompiler.md: freeze the actual responsibility and smaller target.
Read this module's outcome, retained contracts, source entry points and Stage 0 questions; read Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md, Docs/Roadmap.md and Docs/Implementation/PortfolioReduction/README.md. Refresh current HEAD/working-tree identities and respect concurrent work. Audit the named retirement closure/participation in this plan, including narrowed retained capabilities; prepare a transitive retire/retain-with-consumer/unresolved manifest and rerun it after each layer disappears. Do not implement the production refactor in this stage.

Acceptance criteria:
- Specify this plan's Adapt the surviving design contract: vanished assumption, concrete smaller API/data/control flow, caller changes, selected reference lesson and local limits. Budget all replacement/caller code and name the retained behavior/failure check.
- an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.
- Map each proposed changed function to one cohesive responsibility/owner and an intent-based name; distinguish workflow orchestration from implementation mechanisms. Specify callers/contracts/files/build outputs to retire and the sole target path; no legacy/backward-compatibility reader, alias, migration, disabled stub or old/new implementation may be planned.
- Define stage-specific success, invalid-input/failure/cancellation/lifetime checks and measured bounds before coding; retain all approved parallel/data-oriented/game/render/RHI contracts. No unsupported performance or qualification claim.

Failure modes to reject:
- The feature disappears but surviving contracts retain obsolete generality, or an upstream framework is copied without a local invariant.
- Historical ShaderCompiler paths, footprint counts or old receipts are treated as current without tracing actual callers/build inputs.
- Mixed retained/retired responsibilities or CPU/GPU reader lifetimes are omitted, leaving no safe complete-system cut or measurable replacement budget.
- A generic wrapper/framework, serial execution, retained compatibility path or source relocation is substituted for a smaller complete design. A first-order-only dependency scan is insufficient.

Deliver the concrete design/ownership map, bounded change and net footprint budget, planned checks and open conditions in ignored stage evidence. Keep CodeStyle's function/namespace/layout rules explicit in the design; permanent test-only additions are not authorized. Do not close an unproven implementation stage or change a recorded decision.
```

## Stage 1: Remove optional analysis/catalog presentation and consolidate export CLI

Decisions: CP-03, CP-08, CP-09.

**Implementation prompt:**

```text
Implement Stage 1 of Docs/Implementation/PortfolioReduction/13_ShaderCompiler.md: Remove optional analysis/catalog presentation and consolidate export CLI.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Delete stand-alone analysis/stats/catalog/inspection command presentation and exclusive writers; keep ABI/registration validation as a real compiler invariant and update the existing validation target. Use --debug-info as the public symbols switch; retire --debug-artifacts only after its exact code/source/fresh-output guarantees live in the shared publisher. Update all Launcher/diagnostic/script callers; do not remove the shader-target enum. SPIRV-Tools has retained ShaderMakeOutputs/SpirVDisassembler debug consumers; report removal alone does not retire its SDK/runtime.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Full, shader-id and changed-source cooks still pass deterministic/invalid-selector/accepted-output checks. Both target contracts and symbols/source exports remain matched; stale debug identity is rejected. Updated validation checks retained behavior rather than expecting removed report text.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Removed catalog/stats/export commands survive as aliases or existing validation still calls them.
- Consolidated --debug-info drops fresh exact code/source/debug artifacts, ABI checks or target enumeration.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 2: Replace first-party preprocessing only through proven compiler reuse

Decisions: CP-04.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/13_ShaderCompiler.md: Replace first-party preprocessing only through proven compiler reuse.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Investigate a smaller ShaderMake/DXC/Slang-owned include path with a minimal virtual-path adapter only if required. Preserve source mount containment/canonicalization, include closure/invalidation, defines/entry identity, diagnostics and debug-source mapping. Keep parallel isolated compiler sessions. Remove the old machinery/callers together after a candidate is both smaller end to end and semantically equivalent; no permanent dual path or speculative include service.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Compile representative shared/nested includes and retained stages for DXIL/SPIR-V through both kept backends where supported. Changed includes select exactly affected shaders; escaping/missing includes fail without replacing accepted outputs. Source correlation/native debugging works at this exact toolchain identity. If the contract is not met, keep the minimum current path and leave conditional retirement open.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Native includes change containment, include closure/invalidation, diagnostics or source correlation.
- A failed replacement leaves old/new preprocessors or a compatibility filesystem active; a smaller but incorrect candidate is accepted.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 3: Close retained compiler/publication boundaries

Decisions: CP-01, CP-02, CP-05, CP-06, CP-07, CP-10.

**Implementation prompt:**

```text
Implement Stage 3 of Docs/Implementation/PortfolioReduction/13_ShaderCompiler.md: Close retained compiler/publication boundaries.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Consolidate duplicate code/reflection/debug identity representations left by stages 1/2. Keep typed ABI data with its real consumers, bounded parallel compile jobs and correct accepted-output locks/journals. Remove Reference/morph/PTLAS shader registrations only through the corresponding system owner.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Parallel repeated/incremental cooks and Editor reload remain deterministic; consumers cannot read incomplete products. Exact code/PDB/source/debug identifiers survive both APIs and retained GPU diagnostic workflows. Net reduction includes any replacement adapter, altered validation and publication code.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Deduplicated code/reflection/debug state loses a consumed ABI field or identity association.
- Parallel cooks/reloads expose incomplete output, publish nondeterministically or regress compiler-session concurrency.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
