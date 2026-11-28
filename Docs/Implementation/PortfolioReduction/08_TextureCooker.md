# TextureCooker: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

**Execution order: 08 of 21 (first focused visit).** Start: Stages 0-1 in Wave 2. Resume / dependency: Stage 2 in Wave 5 after MeshCooker/MaterialCooker and SceneCooker stage 3. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Trim unused input/report support while retaining a correct standalone parallel texture pipeline and its explicit quality policies.

**Retain:** Standalone request-file cooking, validation/failure behavior, GPU BC processing, DDS mip preservation, PNG/JPEG and HDR/EXR inputs, RGBA32F default and BC6H opt-in, native texture dimensions/channel/color contracts.

**Target flow:** Asset/MaterialCooker checked request -> source decode -> parallel processing/mips/compression -> validated complete output -> accepted publication.

Dominant execution owner for: TX-01, TX-02, TX-04, TX-05, TX-06, TX-07, TX-08. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **Narrow admissions and results:** after unused source formats and request inspection/reporting are removed, simplify decoder routing, admitted-extension policy, command results and callers. Remove exclusive format/report flags and data, not just help text. Keep stb/WIC or other shared decode mechanisms where retained formats still need them.
- **One processing contract:** checked texture requests flow through owned decode, mip/channel/color processing, BC compression and validated output publication. Reduce duplicate request/source/output state and unnecessary dispatch layers; retain bounded parallel work and independent valid choices such as RGBA32F default versus BC6H opt-in and DDS mip preservation.

**Structural acceptance:** cook the retained PNG/JPEG/DDS/HDR/EXR cases through real asset/material requests, including invalid data, cancellation and HDR/BC outputs. Compare request/state/API surface and retained source uses. A one-branch plugin decoder registry or removal of WIC merely because BMP input is gone fails; screenshot BMP output is a separate consumer.

**Reference application:** [P1 and P3](README.md#reference-choices-and-limits) inform cohesive mechanism ownership and selected dependency closure, not a new image-processing abstraction.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **30 files / 2,019 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **2 files / 49 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| TX-05 | 2 files / 200 lines |
| TX-06 | 1 files / 149 lines |

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| TX-05; stage 1 | Unused PSD/PIC/PNM-family admission. Remove dispatch/extensions/help/validation and format-only helpers that have no selected input; inspect embedded/default/manual requests as well as top-level asset files. | stb remains for PNG/TGA; shared decoder/cooked texture contracts and parallel processing stay. |
| TX-06; stage 1 | BMP/GIF texture admission through WIC. Remove format admission and exclusive multi-frame/probe handling only after verifying remaining JPEG callers; update help/rejection/cook source identity. | WIC/JPEG and ordinary BMP screenshot output; output encoder and input decoder are separate. |
| TX-07; stage 1 | inspect-request-file command/report. Retire InspectTextureCookRequestFileCommand, CLI help/registration and exclusive formatting/report state; update any manual command references. | cook-request-file request codec, validation, execution, failures and supervised parallel texture cooking. |

## Stage 0: freeze the actual responsibility and smaller target

Recheck retained external and embedded image references after importer decisions. Identify shared stb/WIC dispatch: PNG/TGA still use stb, JPEG still uses WIC. Do not count those libraries as removable. KTX retirement is already complete and excluded. Inspect manual-request BC6H versus automatic default RGBA32F, not just EXR suffixes.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Tools/Cooking/TextureCooker/Private/SourceLoading/TextureSource.cpp](../../../Tools/Cooking/TextureCooker/Private/SourceLoading/TextureSource.cpp)
- [Tools/Cooking/TextureCooker/Private/Processing/TextureProcessor.cpp](../../../Tools/Cooking/TextureCooker/Private/Processing/TextureProcessor.cpp)
- [Tools/Cooking/TextureCooker/Public/TextureCookRequestList.h](../../../Tools/Cooking/TextureCooker/Public/TextureCookRequestList.h)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Implementation prompt:**

```text
Prepare Stage 0 of Docs/Implementation/PortfolioReduction/08_TextureCooker.md: freeze the actual responsibility and smaller target.
Read this module's outcome, retained contracts, source entry points and Stage 0 questions; read Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md, Docs/Roadmap.md and Docs/Implementation/PortfolioReduction/README.md. Refresh current HEAD/working-tree identities and respect concurrent work. Audit the named retirement closure/participation in this plan, including narrowed retained capabilities; prepare a transitive retire/retain-with-consumer/unresolved manifest and rerun it after each layer disappears. Do not implement the production refactor in this stage.

Acceptance criteria:
- Specify this plan's Adapt the surviving design contract: vanished assumption, concrete smaller API/data/control flow, caller changes, selected reference lesson and local limits. Budget all replacement/caller code and name the retained behavior/failure check.
- an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.
- Map each proposed changed function to one cohesive responsibility/owner and an intent-based name; distinguish workflow orchestration from implementation mechanisms. Specify callers/contracts/files/build outputs to retire and the sole target path; no legacy/backward-compatibility reader, alias, migration, disabled stub or old/new implementation may be planned.
- Define stage-specific success, invalid-input/failure/cancellation/lifetime checks and measured bounds before coding; retain all approved parallel/data-oriented/game/render/RHI contracts. No unsupported performance or qualification claim.

Failure modes to reject:
- The feature disappears but surviving contracts retain obsolete generality, or an upstream framework is copied without a local invariant.
- Historical TextureCooker paths, footprint counts or old receipts are treated as current without tracing actual callers/build inputs.
- Mixed retained/retired responsibilities or CPU/GPU reader lifetimes are omitted, leaving no safe complete-system cut or measurable replacement budget.
- A generic wrapper/framework, serial execution, retained compatibility path or source relocation is substituted for a smaller complete design. A first-order-only dependency scan is insufficient.

Deliver the concrete design/ownership map, bounded change and net footprint budget, planned checks and open conditions in ignored stage evidence. Keep CodeStyle's function/namespace/layout rules explicit in the design; permanent test-only additions are not authorized. Do not close an unproven implementation stage or change a recorded decision.
```

## Stage 1: Remove unused input admissions and request reporting

Decisions: TX-05, TX-06, TX-07.

**Implementation prompt:**

```text
Implement Stage 1 of Docs/Implementation/PortfolioReduction/08_TextureCooker.md: Remove unused input admissions and request reporting.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Remove PSD/PIC/PNM-family and BMP/GIF source admission plus exclusive handling and the inspect-request-file command/presentation. Keep the actual cook-request-file codec/validation/help/errors. Reject unsupported formats explicitly before publication; keep BMP screenshots and shared JPEG/stb users separate.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Retained PNG/JPEG/DDS/HDR/EXR requests still cook with correct mip/color/channel output. Removed inputs/command are rejected without corrupting accepted output. Dispatch-only edits may save zero physical lines; report that honestly. Existing KTX absence is a guard, not new savings.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Shared JPEG/PNG/TGA loading or BMP screenshot output is deleted with unsupported texture admissions.
- Removed formats/report command still have a fallback, or invalid input replaces an accepted cooked texture.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 2: Simplify retained parallel processing/request ownership

Decisions: TX-01, TX-02, TX-04, TX-08.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/08_TextureCooker.md: Simplify retained parallel processing/request ownership.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Remove duplicate format/property/policy state uncovered by stage 1. Keep RGBA32F default and explicit BC6H quality fallback/opt-in, bounded parallel preparation and one request format shared with AssetCooker. No new texture-container/format framework or blanket decoder replacement.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Check dimensions, HDR range/non-finite rejection, BC quality, normal orientation and preserved DDS mips with identified controls. Cancellation/child failure preserves accepted products; no-op cook and actual prepared requests agree. Benchmark throughput/peak memory only if this stage changes processing/concurrency.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- RGBA32F/BC6H, HDR range, normal orientation, channel/color policy or DDS mip preservation changes incorrectly.
- A request/policy rewrite leaves duplicate schemas/state, corrupts publication or serializes independent texture work.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
