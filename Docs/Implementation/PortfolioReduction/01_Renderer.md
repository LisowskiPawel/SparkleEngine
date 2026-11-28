# Renderer: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

**Execution order: 01 of 21 (first focused visit).** Start: Stages 0-1 in Wave 1. Resume / dependency: Stages 2-4 in Wave 7, after runtime/content contracts are narrowed. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Keep one focused real-time path-traced renderer and its owned frame graph; remove the independent offline/reference renderer and unused presentation modes across the repository.

**Retain:** Frame graph, raster and ray-traced GBuffer, inline/ray-pipeline shader paths, ReSTIR indirect, TLAS refit, DLSS SR/RR, subsurface, alpha/two-sided parity, automatic/manual exposure and signal views. Preserve separate render-thread ownership, parallel preparation/recording and queue overlap.

**Target flow:** Game-owned immutable scene deltas -> parallel render preparation -> owned frame graph -> native RHI contexts/queues -> checked real-time output and fence retirement.

Dominant execution owner for: AP-03, RE-01, RE-02, RE-03, RE-04, RE-05, RE-06, RE-07, RE-08, RE-09, RE-10, RE-11, RE-12, RE-13, RE-14, RE-15. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **One lighting owner:** after Reference retirement, narrow host-facing settings, scene/frame inputs, render-state ownership, pass scheduling and output contracts to the retained real-time path. Remove Reference-only mode dispatch, accumulation/export state and shared fields with no real-time consumer. Both GBuffer algorithms, subsurface and other selected controls remain real choices.
- **Explicit retained passes:** compose the frame from recognizable per-pass operations and typed resource/parameter contracts. After display/lens and conditional automatic-batching cuts, remove unused graph edges, resources, shader permutations, grouping keys and policy branches through their consumers. Keep authored instances, temporal history needed by real-time rendering, parallel preparation/recording and queue lifetime.

**Structural acceptance:** map the complete retained frame on both APIs, exercise both GBuffer routes and animated geometry, and compare resources/state/branches plus the required image/cost bounds. Reject a one-option lighting selector, combined mega-pass file or blanket deletion of temporal/capture resources still used by the frame.

**Reference application:** [P2 and P5](README.md#reference-choices-and-limits) guide pass dependencies and justified invalidation; [SDK integration precedents](../../References.md#integration-precedents) inform application-owned resources. No new graph-caching or SDK-integration project is added.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **619 files / 36,806 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **53 files / 3,095 lines**; assigned shared spans: **57 lines in 1 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| AP-03 | 2 files / 310 lines |
| RE-07 | 2 files / 521 lines |
| RE-13 | 8 files / 132 lines |

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| RE-04, AP-03; stage 1 | Independent Reference PT and all artifact/session machinery. Close render-mode/CVar/UI/defaults, accumulation/convergence/checkpoint/resume, work/session graph/resources, dedicated shader registrations/includes, Application artifact encoders/writers/publication, readback queues, CLI/export/cook/package expectations and obsolete validation/docs. Repeat for helpers made unused by those deletions. | Real-time tracing/RNG/material/light math, ordinary screenshots/readback, TinyEXR input, required symbols/source and frame graph; academic external references are not executable Reference code. |
| RE-13; stage 2 | HDR display/output-color modes beyond SDR. Retire output policy selectors, settings persistence/defaults/UI, display-specific metadata/color-space negotiation, calibration/export branches and exclusive shader parameters; trace neutral RHI enums/helpers for remaining consumers. | Linear/HDR lighting math, EXR/HDR texture input, BC6H/RGBA32F textures, correct SDR presentation and retained neutral format support. |
| RE-14; stage 2 | Vignette/chromatic-aberration effects. Retire shader/pass helpers, registrations, constant-buffer fields, settings/CVars/UI/persistence/defaults and otherwise unused resource/graph bindings. Rebuild both native shader targets/layouts. | Exposure, tonemapping, reconstruction, other kept signal/debug outputs and ABI alignment. |
| RE-07; stage 3 | Automatic mesh grouping/batch policy. Retire grouping/planning/cache/dirty invalidation state, batching-only render extraction fields and corresponding settings/UI/shader flags if the qualified direct path replaces them. | Explicit authored instance groups, resource reuse, data-oriented instance packets, parallel extraction/recording and frozen quality/performance/memory bounds. |

## Retained frame and recording vocabulary

Keep `RenderFrame` for frame-slot scene/view data and `RenderFrameGraph*` for topology/resources. `PassCommandContext` is narrower than `RenderCommandContext`; native recording contexts stay backend-private. Name queues for their payload. Avoid general context/service/resource bags.

## Outstanding retained-image qualification

Recover the real-time Sponza image using one fixed camera and identified source, content and settings. Inspect raw signals before and after display processing; use bounded analytic checks and suitable external comparisons to find the first incorrect stage. Retire Reference-only defects and convergence/export work; preserve repairs required by the retained real-time frame.

## Stage 0: freeze the actual responsibility and smaller target

Map every ReferencePathTracer type, render mode, session/accumulation/checkpoint/export setting, shader registration and Application/Editor caller. Classify shared EXR/readback/capture/lighting code by its remaining real-time consumer; do not remove TinyEXR or general readback by folder label. Freeze representative real-time raw signals/camera/settings before touching mode selection.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [Engine/Renderer/Private/Passes/Lighting/ReferencePathTracer](../../../Engine/Renderer/Private/Passes/Lighting/ReferencePathTracer)
- [Engine/Application/Private/Editor/ReferencePathTracer](../../../Engine/Application/Private/Editor/ReferencePathTracer)
- [Engine/Editor/Private/Viewport/ReferencePathTracer](../../../Engine/Editor/Private/Viewport/ReferencePathTracer)
- [Engine/Assets/Shaders/Passes/Lighting/ReferencePathTracer](../../../Engine/Assets/Shaders/Passes/Lighting/ReferencePathTracer)
- [Engine/Application/Private/Editor/Capture/ImageEncoding.cpp](../../../Engine/Application/Private/Editor/Capture/ImageEncoding.cpp)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Implementation prompt:**

```text
Prepare Stage 0 of Docs/Implementation/PortfolioReduction/01_Renderer.md: freeze the actual responsibility and smaller target.
Read this module's outcome, retained contracts, source entry points and Stage 0 questions; read Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md, Docs/Roadmap.md and Docs/Implementation/PortfolioReduction/README.md. Refresh current HEAD/working-tree identities and respect concurrent work. Audit the named retirement closure/participation in this plan, including narrowed retained capabilities; prepare a transitive retire/retain-with-consumer/unresolved manifest and rerun it after each layer disappears. Do not implement the production refactor in this stage.

Acceptance criteria:
- Specify this plan's Adapt the surviving design contract: vanished assumption, concrete smaller API/data/control flow, caller changes, selected reference lesson and local limits. Budget all replacement/caller code and name the retained behavior/failure check.
- an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.
- Map each proposed changed function to one cohesive responsibility/owner and an intent-based name; distinguish workflow orchestration from implementation mechanisms. Specify callers/contracts/files/build outputs to retire and the sole target path; no legacy/backward-compatibility reader, alias, migration, disabled stub or old/new implementation may be planned.
- Define stage-specific success, invalid-input/failure/cancellation/lifetime checks and measured bounds before coding; retain all approved parallel/data-oriented/game/render/RHI contracts. No unsupported performance or qualification claim.

Failure modes to reject:
- The feature disappears but surviving contracts retain obsolete generality, or an upstream framework is copied without a local invariant.
- Historical Renderer paths, footprint counts or old receipts are treated as current without tracing actual callers/build inputs.
- Mixed retained/retired responsibilities or CPU/GPU reader lifetimes are omitted, leaving no safe complete-system cut or measurable replacement budget.
- A generic wrapper/framework, serial execution, retained compatibility path or source relocation is substituted for a smaller complete design. A first-order-only dependency scan is insufficient.

Deliver the concrete design/ownership map, bounded change and net footprint budget, planned checks and open conditions in ignored stage evidence. Keep CodeStyle's function/namespace/layout rules explicit in the design; permanent test-only additions are not authorized. Do not close an unproven implementation stage or change a recorded decision.
```

## Stage 1: Delete the independent Reference renderer as one system

Decisions: RE-04, AP-03.

**Implementation prompt:**

```text
Implement Stage 1 of Docs/Implementation/PortfolioReduction/01_Renderer.md: Delete the independent Reference renderer as one system.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Own cross-module deletion of sessions, scheduling/accumulation/convergence/checkpoint/resume/artifact state, dedicated render mode and UI, shaders/resources, registrations, export encoders and expected cook/package/validation outputs. Update callers and sole remaining real-time composition directly; no disabled enum or validation-only copy. Delete dedicated Reference code through Application/Editor/ShaderCompiler/build owners in the same complete removal chain.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Search/type/build/cook inventories show no executable Reference route, dedicated files or output promises. Retained real-time lighting and ordinary screenshots/debug symbols still work on both APIs. Correctness controls use bounded analytic/external comparisons, not another Sparkle renderer. Clean stale cooked/package outputs so old shaders cannot mask incomplete deletion.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- A Reference mode/session/export/shader registration survives through a hidden or validation-only route.
- Deleting shared capture/EXR/readback/lighting support breaks ordinary captures, environments or real-time rendering.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 2: Remove advanced HDR display and lens effects

Decisions: RE-13, RE-14.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/01_Renderer.md: Remove advanced HDR display and lens effects.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Keep one explicit SDR presentation/output-color policy. Remove HDR display selection/metadata/branches and vignette/chromatic-aberration parameters, shader stages/settings/UI/defaults. Preserve EXR/HDR lighting inputs, internal linear/HDR math, sky environments and ordinary screenshot output. Keep resource transitions and CPU/shader ABI in sync.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- No removed display/lens setting survives a profile or persisted option. SDR output, exposure/reconstruction and raw signal checks match defined tolerances on both backends. Runtime image parity is checked after regenerated shaders, not inferred from a host build.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- HDR display retirement deletes linear/HDR lighting inputs or a stale settings/shader field keeps a removed effect alive.
- Regenerated CPU/shader layouts disagree or SDR/exposure/reconstruction output regresses.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 3: Remove automatic mesh batching only through its cost gate

Decisions: RE-07.

**Implementation prompt:**

```text
Implement Stage 3 of Docs/Implementation/PortfolioReduction/01_Renderer.md: Remove automatic mesh batching only through its cost gate.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Keep explicit authored instance groups and shared geometry/material resources; remove only automatic grouping/cache/planner policy if the chosen direct path stays within fixed retained-scene CPU/GPU/memory budgets. Preserve data-oriented instance packets, parallel extraction/recording and stable object identity. If a smaller parallel equivalent cannot meet the gate, retain the necessary optimized mechanism and leave this stage open.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Compare Knight and Bistro geometry counts, draw/dispatch/build costs, CPU/GPU frame distributions and peak memory with equivalent images. Removing automatic batching cannot imply per-instance geometry duplication, serial loops or deleting explicit group support.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Automatic batching removal also deletes explicit instances, duplicates geometry or serializes extraction/recording.
- The direct path violates frozen frame/memory/image bounds; the cut is declared passed despite an inconclusive comparison.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 4: Simplify the surviving real-time frame path

Decisions: RE-01, RE-02, RE-03, RE-05, RE-06, RE-08, RE-09, RE-10, RE-11, RE-12, RE-15.

**Implementation prompt:**

```text
Implement Stage 4 of Docs/Implementation/PortfolioReduction/01_Renderer.md: Simplify the surviving real-time frame path.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Collapse mode-routing/state wrappers and repeated representations made obsolete by stages 1/2. Keep frame-graph ownership, resource/queue lifetime and the selected algorithms. Repair surviving producer/consumer boundaries through RHI and ShaderAssets without a renderer rewrite or new rendering SDK feature campaign.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- A specialist can trace GBuffer -> transport/reuse -> reconstruction -> exposure -> SDR and identify every resource/history reset/queue edge. Preserve failed and inconclusive comparisons. Current real-time image/Vulkan startup/material-parity issues remain open until actually observed fixed; removal builds do not close them.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Mode consolidation hides a rendering/queue dependency defect or deletes a retained GBuffer/lighting/debug route.
- Old and new state/parameter representations remain, or new global locks/drains remove frame parallelism.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
