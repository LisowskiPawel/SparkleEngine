# Third-party integration maintenance: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

**Execution order: 20 of 21 (first focused visit).** Start: Stages 0-2 in Wave 8 after BuildPackaging stages 2-3. Resume / dependency: Do not retire a patch/library before its real callers disappear; finish before the final package gate. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Retain production libraries and necessary maintained behavior while retiring only exclusive patches/dependencies after real consumers disappear.

**Retain:** Selected logging/JSON/process/tasks/window/UI/allocation/import/animation/shader libraries and required patches. Keep enkiTS parallel scheduling/settlement behavior and pinned Slang source/debug fixes until equivalent behavior is verified.

**Target flow:** Pinned dependency/patch identity -> compiled binding and narrow integration -> actual retained consumer -> runtime/build/notice closure.

Dominant execution owner for: TP-01, TP-02, TP-03, TP-04.

Delegated complete systems: TP-05 -> [RHI](06_RHI.md). Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **Binding follows real use:** after native enkiTS users are migrated and PTLAS/PLY/runtime-Ozz edges close with their dominant owners, narrow dependency recipes, patches, compile bindings and SDK staging to the actual retained consumers. Keep libraries and required patches selected for retention; upstream API reuse replaces duplicate first-party mechanics only when the full path is smaller and qualified.
- **Keep per-library semantics:** maintain explicit include/link/export, allocator/context and source/debug identity boundaries across DLLs. Retire only unused enkiTS extension hunks after consumer and native-equivalence proof; do not replace the scheduler or drop required concurrency/settlement behavior. Ozz offline remains a cook dependency; Assimp remains required for FBX.

**Structural acceptance:** map every changed dependency/patch hunk to a native consumer and build/runtime/notice contribution, then verify the changed ABI and threaded lifecycle. A recipe retained solely for a retired system, blanket static/linkage rule or locally duplicated supported library mechanism without a required contract fails.

**Reference application:** [P3](README.md#reference-choices-and-limits) plus [allocator, Assimp and scheduler precedents](../../References.md#integration-precedents) guide precise reuse boundaries. Their broader frameworks and dependency stacks are excluded.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **33 files / 1,078 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **0 files / 0 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| TP-03 | 3 files / 790 lines |

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| TP-03; stage 1 | Unused maintained enkiTS extensions. After all domain cuts, trace patch hunks/native ABI declarations, exported bindings, RuntimeSource required-source/hash admission and fork recipes. Prune only hunks without a live owner or a proven smaller equivalent; update qualified patch/source bytes. | Native task graph/ranges, failure/cancel/settlement, external registration and parallel frame/load/Launcher/texture work; entire scheduler/fork is not an assumed saving. |

## Stage 0: freeze the actual responsibility and smaller target

Trace all seven maintained patches to live consumers and the actual installed/prepared compiler bytes. Do not infer a patch is redundant from absence of .slang assets. Separate scheduler extensions from native jobs/lifetime semantics and dependency ABI/global/allocator behavior across DLLs. NVAPI retirement is owned only by RHI stage 2.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [ThirdParty/enkits/RuntimeSource.cmake](../../../ThirdParty/enkits/RuntimeSource.cmake)
- [ThirdParty/enkits/Patches/enkiTS-sparkle-runtime.patch](../../../ThirdParty/enkits/Patches/enkiTS-sparkle-runtime.patch)
- [ThirdParty/slang/Patches/SlangDebugInformationFormat.patch](../../../ThirdParty/slang/Patches/SlangDebugInformationFormat.patch)
- [ThirdParty/slang/Patches/SlangLoopSourceLocations.patch](../../../ThirdParty/slang/Patches/SlangLoopSourceLocations.patch)
- [ThirdParty/README.md](../../../ThirdParty/README.md)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Implementation prompt:**

```text
Prepare Stage 0 of Docs/Implementation/PortfolioReduction/20_ThirdParty.md: freeze the actual responsibility and smaller target.
Read this module's outcome, retained contracts, source entry points and Stage 0 questions; read Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md, Docs/Roadmap.md and Docs/Implementation/PortfolioReduction/README.md. Refresh current HEAD/working-tree identities and respect concurrent work. Audit the named retirement closure/participation in this plan, including narrowed retained capabilities; prepare a transitive retire/retain-with-consumer/unresolved manifest and rerun it after each layer disappears. Do not implement the production refactor in this stage.

Acceptance criteria:
- Specify this plan's Adapt the surviving design contract: vanished assumption, concrete smaller API/data/control flow, caller changes, selected reference lesson and local limits. Budget all replacement/caller code and name the retained behavior/failure check.
- an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.
- Map each proposed changed function to one cohesive responsibility/owner and an intent-based name; distinguish workflow orchestration from implementation mechanisms. Specify callers/contracts/files/build outputs to retire and the sole target path; no legacy/backward-compatibility reader, alias, migration, disabled stub or old/new implementation may be planned.
- Define stage-specific success, invalid-input/failure/cancellation/lifetime checks and measured bounds before coding; retain all approved parallel/data-oriented/game/render/RHI contracts. No unsupported performance or qualification claim.

Failure modes to reject:
- The feature disappears but surviving contracts retain obsolete generality, or an upstream framework is copied without a local invariant.
- Historical ThirdParty paths, footprint counts or old receipts are treated as current without tracing actual callers/build inputs.
- Mixed retained/retired responsibilities or CPU/GPU reader lifetimes are omitted, leaving no safe complete-system cut or measurable replacement budget.
- A generic wrapper/framework, serial execution, retained compatibility path or source relocation is substituted for a smaller complete design. A first-order-only dependency scan is insufficient.

Deliver the concrete design/ownership map, bounded change and net footprint budget, planned checks and open conditions in ignored stage evidence. Keep CodeStyle's function/namespace/layout rules explicit in the design; permanent test-only additions are not authorized. Do not close an unproven implementation stage or change a recorded decision.
```

## Stage 1: Retire only unused enkiTS extensions after consumers vanish

Decisions: TP-03.

**Implementation prompt:**

```text
Implement Stage 1 of Docs/Implementation/PortfolioReduction/20_ThirdParty.md: Retire only unused enkiTS extensions after consumers vanish.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
After GameFramework/Launcher/frame owners narrow their state, audit each maintained extension against all remaining calls. Remove only an extension with no retained consumer or a smaller verified native equivalent, then update patch/source hashes and recipe validation. Keep ranges, dependencies, cancellation/failure propagation, task argument ownership, external-thread registration and physical completion. No scheduler replacement or thread-pool collapse is approved.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Real Launcher, scene-load, parallel world/render and shutdown consumers pass capacity/settlement/cancel checks. Patch reapplication/source qualification works from clean prepared dependency source. No unaccounted worker, dangling native argument or added global scheduling lock; entire fork saving is not claimed.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- An extension is removed by patch size rather than live consumers, or source/patch hashes are no longer qualified.
- Task argument lifetime, external-thread registration, cancellation/settlement or parallel scheduling contracts break.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 2: Close retained dependency/patch/ABI inventory

Decisions: TP-01, TP-02, TP-04.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/20_ThirdParty.md: Close retained dependency/patch/ABI inventory.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Keep required patches/forks, including Slang debug/source fixes, while pruning only dependencies whose owning system closes last-consumer removal. Check recipes/bindings/notices and conditional DLL runtime/allocator/global requirements; do not impose one static/shared rule on every vendor. Do not add new SDK algorithms as part of cleanup.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Prepared source/patch bytes and actual runtime DLL inputs are identified for the selected toolchain. Slang symbols/source and native diagnostic exports are usable; independent library tests are not a substitute for Sparkle integration. Required libraries remain attributed and removal savings are credited to the system owner once.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- A required Slang/allocator/global/ABI patch is dropped without equivalent integration evidence.
- A blanket static/shared policy or dependency purge breaks real DLL/runtime/debug consumers or attribution.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
