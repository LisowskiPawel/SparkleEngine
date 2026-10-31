# Renderer Runtime And Diagnostics Closure Plan

[Docs](../../../../../README.md) / [Up](README.md) / [Design](../RendererDesign.md)

Make existing bindings, settings, debug/capture and viewport/latency observations describe the same active frame without building a diagnostics product.

**Status:** remaining release integration and proof; stage names do not imply missing implementation or passed evidence.

## Work This Plan

Every stage and copied prompt follows [stage completion and resume](../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-completion-and-resume). Close its finite local exit once; keep later aggregate proof separate.

Use RD-0 only for a missing identity/authority decision. A binding defect enters RD-1 immediately as a prerequisite to its rendering consumer; settings/UI repairs do not wait for a pipeline rewrite. Preserve delivered Show controls, process/capture fixes and qualified native tooling.

Run the stage that owns the demonstrated missing contract or failing check. Stage numbers identify responsibilities; they do not require rebuilding existing layers or completing unrelated stages first. Every stage and copied prompt follows the [Stage Source-Style Gate](../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate) and the [shared delivery contract](../../../../CrossModule/FirstRelease/ImplementationPlan.md#phase-card-contract).

## Dependencies

RD-1 supplies valid bound programs to frame features. RD-2 applies requested state; RD-3/RD-4 observe and present the frame. RD-5 closes only admitted latency/provider behavior. These are dependency edges, not a serial rewrite from bindings to UI; RD-6 joins compatible results.

## `RD-0` — Freeze Runtime State And Observation Contracts

**Start:** A selected journey cannot correlate request, active state or visible/captured frame.

**Work:** Trace only that request through current settings/CVar precedence, pipeline/feature generation, view product, capture or latency token. Resolve missing domain/encoding and requested/active/restart/unavailable meanings in their owner.

**Cleanup:** Delete obsolete control/status descriptions and duplicate authority in the changed contract; no global registry, read-model framework or report browser.

**Exit:** One state/identity trace and smallest missing check identify the repair; other families are not reopened by default.

```text
Execute only RD-0 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md#rd-0--freeze-runtime-state-and-observation-contracts. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Trace only that request through current settings/CVar precedence, pipeline/feature generation, view product, capture or latency token.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to the stage's existing report owner; no neighboring stage or new mechanism merely to fill a report.
```

## `RD-1` — Pipeline Materialization And Typed Binding

**Start:** A selected program/parameter/domain, cache-key, reload or pipeline-lifetime claim fails or needs proof.

**Work:** Use current registration/cook/reflection/materialization/binding and the [shader validation plan](../../../../CrossModule/ShaderSystem/ImplementationPlan.md). Exercise affected key/ABI/capability/corruption/reload and all-queue retirement cells, retaining required API coverage.

**Cleanup:** Remove obsolete shader lookups, duplicate binding/cache/generation state and rejected layouts with their consumers; no replayed migration or second cache.

**Exit:** FCR-REN-16 links actual program/binding/native and failure/cost evidence; registration or compilation is not runtime proof.

```text
Execute only RD-1 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md#rd-1--pipeline-materialization-and-typed-binding. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Use current registration/cook/reflection/materialization/binding and the [shader validation plan](../../../../CrossModule/ShaderSystem/ImplementationPlan.md).
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-16; no neighboring stage or new mechanism merely to fill a report.
```

## `RD-2` — Settings State And Persistence

**Start:** An approved control needs request-to-active or persistence closure.

**Work:** Repair current defaults/ranges/precedence, atomic per-user save/load/reset, render-thread delivery and resolved/active/restart truth. Exercise malformed values, save failure, affected edit/queue/shutdown and package paths; retain approved shared Game/Editor settings.

**Cleanup:** Delete stale aliases, duplicate UI/CVar/file truth and replaced transfer state. Do not add another Game settings frontend or compatibility schema.

**Exit:** FCR-REN-19 affected round-trip/precedence/atomic-failure/active-state cells pass without modifying immutable package defaults.

```text
Execute only RD-2 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md#rd-2--settings-state-and-persistence. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Repair current defaults/ranges/precedence, atomic per-user save/load/reset, render-thread delivery and resolved/active/restart truth.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-19; no neighboring stage or new mechanism merely to fill a report.
```

## `RD-3` — Debug Views, Diagnostics Products, And Capture

**Start:** An existing required mode, Show control or capture output needs proof or repair.

**Work:** Use [Debug Views acceptance](../Features/DebugViews/Acceptance.md) and retained [Show results](../../../../../Acceptance/Renderer/DebugViews.md). Close remaining availability, per-view mode, signal/display, readback/sidecar/identity, privacy or observer cells. Use qualified crash/external tools through current owners.

**Cleanup:** Remove stale modes without producers through their scope owner, duplicate capture/status state and temporary probes. Do not recreate delivered controls or implement Stat groups, histories, workspace or ProfileGpu.

**Exit:** FCR-REN-11 records admitted mode/frame/view/domain/encoding and failure/cost proof; missing required output cannot display stale data as valid.

```text
Execute only RD-3 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md#rd-3--debug-views-diagnostics-products-and-capture. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Use [Debug Views acceptance](../Features/DebugViews/Acceptance.md) and retained [Show results](../../../../../Acceptance/Renderer/DebugViews.md).
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-11; no neighboring stage or new mechanism merely to fill a report.
```

## `RD-4` — UI And Viewport Composition

**Start:** A current viewport/packet, resize/DPI transition or texture handle is the failing edge.

**Work:** Check immutable packet ownership/copies, product/texture generations and retirement, blend/color/alpha, extent/DPI, level switch, missing/stale input and shutdown. Preserve the shipped product split: Game excludes Editor authoring; admitted Editor retains its UI route.

**Cleanup:** Remove redundant packet/product publication and mutable cross-thread UI state exposed by the repair; UI policy stays with its host.

**Exit:** FCR-REN-13 affected lifetime/bounds/color/resize/switch/failure/package cells pass without a viewport or UI framework.

```text
Execute only RD-4 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md#rd-4--ui-and-viewport-composition. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Check immutable packet ownership/copies, product/texture generations and retirement, blend/color/alpha, extent/DPI, level switch, missing/stale input and shutdown.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-13; no neighboring stage or new mechanism merely to fill a report.
```

## `RD-5` — Latency Coordination

**Start:** An admitted PCL/Reflex marker/provider cell needs functional closure or a latency claim is actually made.

**Work:** Check existing six-marker order, simulation/submit/present identity, 64-to-32-bit limits, capability/availability, failure/shutdown and exact DLL/package boundary. Keep unsupported Vulkan Reflex explicit. Measure independently for a claimed benefit; markers alone prove no latency reduction.

**Cleanup:** Remove duplicate frame/provider state and obsolete marker calls; add no input-to-display platform or mandatory optional latency SDK.

**Exit:** FCR-REN-20 closes declared functional/safe-unavailability behavior and any claimed controlled measurement. Missing measurement stays unproved; it does not authorize a diagnostics subsystem.

```text
Execute only RD-5 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md#rd-5--latency-coordination. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Check existing six-marker order, simulation/submit/present identity, 64-to-32-bit limits, capability/availability, failure/shutdown and exact DLL/package boundary.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-20; no neighboring stage or new mechanism merely to fill a report.
```

## `RD-6` — Runtime And Diagnostics Candidate Closure

**Start:** Five admitted family results and final package configuration exist.

**Work:** Join active bindings/settings, visible/captured product and applicable latency identity on one candidate. Fill missing integrated/package checks; distinguish noninstrumented/excluded cells from failed Included claims.

**Cleanup:** Verify prior deletions; remove stale control/pipeline/provider instructions and duplicate result summaries.

**Exit:** FCR-REN-11/13/16/19/20 carry compatible candidate/results, required negatives, honest provider limits and observer/cost evidence. A broader diagnostics product is not required.

```text
Execute only RD-6 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md#rd-6--runtime-and-diagnostics-candidate-closure. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Join active bindings/settings, visible/captured product and applicable latency identity on one candidate.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-11; no neighboring stage or new mechanism merely to fill a report.
```

## Reference And Ownership

[Shader runtime](../Features/ShaderRuntime/ShaderRuntimeDesign.md), [settings](../Features/RuntimeConfiguration/RuntimeConfigurationDesign.md), [Debug Views](../Features/DebugViews/DebugViewsDesign.md), [viewport](../Features/ViewportAndDiagnostics/ViewportAndDiagnosticsDesign.md) and [latency](../Features/FrameExecution/LatencyCoordination.md) own behavior. PD-1 consumes these repairs without duplicating their implementation sequence.
