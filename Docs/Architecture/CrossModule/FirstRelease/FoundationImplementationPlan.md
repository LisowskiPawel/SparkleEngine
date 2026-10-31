# First Release Foundation And Content Implementation Plan

[Docs](../../../README.md) / [Up](ImplementationPlan.md)

Deliver only the foundation, world and content repairs needed to make the approved rendering and product journeys reproducible.

**Status:** remaining release integration and proof; stage names do not imply missing implementation or passed evidence.

## Work This Plan

Every stage and copied prompt follows [stage completion and resume](../../../Engineering/Workflow/ChangeLifecycle.md#stage-completion-and-resume). The [state update](../../../Engineering/Workflow/ChangeLifecycle.md#update-stage-state-before-handoff) is part of its exit: record the result, reconcile remaining work and name the next item before handoff. Later aggregate proof remains separate.

Choose the failing edge from the current product/frame journey. Use FWC-0 only if that edge lacks a current owner or decidable check; otherwise enter its owning stage directly. Existing cold/warm cook, process and product evidence is reused by candidate identity, rather than dismissed by an old "no evidence" snapshot.

Run the stage that owns the demonstrated missing contract, failing check or named bounded simplification from [Roadmap](../../../Strategy/Roadmap.md#portfolio-focus-and-existing-code-simplification). Keep supporting world/Core/tool code to the selected rendering and reproduction role; retain real Tasks, data/lifetime, animation and deterministic-content requirements. Stage numbers identify responsibilities; they do not require rebuilding existing layers or completing unrelated stages first. Every stage and copied prompt follows the [Stage Source-Style Gate](../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate) and the [shared delivery contract](ImplementationPlan.md#phase-card-contract).

## Dependencies

Content must be acquired and cooked before world activation; world publication must be valid before Renderer consumes it. Core/process and Tasks repairs are prerequisites only for consumers that demonstrate their failure. FWC-5 delegates shader proof to the existing shader plan; FWC-6 joins the resulting reports, rather than rerunning eleven implementations.

## `FWC-0` — Reconcile Feature Contracts And Boundaries

**Start:** A selected journey exposes an ambiguous contract, unmatched selector or missing check.

**Work:** Trace that input through its producer, publication, consumer and shutdown. Reuse the eleven-family owner map; resolve only the changed scope, path, import, worker or shader cells. Keep Linux, uncooked loading and unselected import semantics excluded.

**Cleanup:** Remove superseded criteria or duplicate owner descriptions; do not create a dossier or control system for each inventory row.

**Exit:** The selected edge has one owner, scope, failure/AC/check mapping and next repair or evidence action; accepted decisions are not reopened.

```text
Execute only FWC-0 from Docs/Architecture/CrossModule/FirstRelease/FoundationImplementationPlan.md#fwc-0--reconcile-feature-contracts-and-boundaries. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Trace that input through its producer, publication, consumer and shutdown.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to the stage's existing report owner; no neighboring stage or new mechanism merely to fill a report.
Before handoff, update the stage result, remaining plan and next item.

```

## `FWC-1` — Core And Windows Platform

**Start:** An admitted Windows path, child process, configuration, window or input transition fails or lacks required final-candidate proof.

**Work:** Repair that current Core/Platform owner. Cover its affected Unicode/path bounds, immutable package/per-user paths, atomic last-good config, focus/capture/DPI, zero-size/resize and handle/error/shutdown cells. Reuse process quoting, cancellation and log-failure results while their inputs remain valid.

**Cleanup:** Delete the path reconstruction, encoding conversion, duplicated process policy or forwarding error state the repair replaces; preserve caller-specific workflow policy.

**Exit:** Affected CORE/PLAT criteria and negative controls pass for standard-user Windows; package bytes stay immutable and unavailable cells stay open in FCR-CORE-01/FCR-PLAT-01.

```text
Execute only FWC-1 from Docs/Architecture/CrossModule/FirstRelease/FoundationImplementationPlan.md#fwc-1--core-and-windows-platform. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Repair that current Core/Platform owner.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-CORE-01, FCR-PLAT-01; no neighboring stage or new mechanism merely to fill a report.
Before handoff, update the stage result, remaining plan and next item.

```

## `FWC-2` — Task Runtime And Cancellation

**Start:** A selected loading, world, rendering or tool operation needs a scheduler repair or missing task-lifetime evidence.

**Work:** Use the one task runtime. Prove the affected DAG/result, exclusive writes, serial reference, queue/worker bounds, nested waits, failure, cancellation and shutdown. Use serial/1/2/N where the task contract requires it; a throughput claim additionally needs scheduling/crossover measurements.

**Cleanup:** Remove duplicate task ownership, local executor/queue state or orphaned cancellation machinery exposed by this repair. Do not create a scheduler or generic workflow graph.

**Exit:** FCR-TASK-01 records applicable parity, pressure, failure and settlement checks. Functional closure does not claim unmeasured speedup.

```text
Execute only FWC-2 from Docs/Architecture/CrossModule/FirstRelease/FoundationImplementationPlan.md#fwc-2--task-runtime-and-cancellation. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Use the one task runtime.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-TASK-01; no neighboring stage or new mechanism merely to fill a report.
Before handoff, update the stage result, remaining plan and next item.

```

## `FWC-3` — Level, World, And Render Publication

**Start:** Selected cooked content is available and the failing activation/publication edge is identified, or Roadmap selects a bounded world-surface simplification with a concrete retained-path check.

**Work:** Repair transactional request/load/cancel/switch/reload and the immutable world-to-render publication, or remove only the selected unnecessary surface. Retain the current fixed eleven-stage pipeline and one Tasks runtime, including access/hazard checks, structure epochs and deterministic commits; do not reopen its delivered scheduling refactor or generalize it into a gameplay/plugin framework. Preserve entity/resource generations, transforms, camera/light/material and existing skin/morph semantics used by the release. Check delete/reuse, old-world retirement and in-flight readers; quantify changed copies and extraction cost.

**Cleanup:** Keep descriptor/query/resource facts consumed by the compiler/executor private; do not restore unused graph inspection surface. Delete parallel activation flags, copied world truth or replaced publication records when that is the owned repair. Renderer never queries ECS; a failed requested map cannot report successful Empty activation.

**Exit:** FCR-WORLD-01/02/03 agree on candidate and active-level/submission identity; affected lifecycle, semantic, cancellation and cost checks pass. A simplification shows the unused surface is gone and preserves camera/animation/skin/morph/transform/extraction semantics. For an accessor-only cut, check absence and unchanged compiler/executor consumers with the narrow owning-target check; expand serial/worker runtime proof only if execution/data changes. Source proof does not pass the family's unrun runtime cells or claim speedup.

```text
Execute only FWC-3 from Docs/Architecture/CrossModule/FirstRelease/FoundationImplementationPlan.md#fwc-3--level-world-and-render-publication. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Repair transactional request/load/cancel/switch/reload and the immutable world-to-render publication, or remove only the selected unnecessary surface.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-WORLD-01; no neighboring stage or new mechanism merely to fill a report.
Before handoff, update the stage result, remaining plan and next item.

```

## `FWC-4` — Import, Cook, And Engine Assets

**Start:** The selected map/dependency closure or an advertised retained input fails provenance, import, cook or loading proof.

**Work:** Use existing import/normalization/cook/publication/load for the retained glTF/GLB/FBX subset. Verify coordinate/material/alpha/animation losses, dependency paths, default/sky meanings and notices. Exercise malformed/oversized/missing input, bounded jobs/memory and interrupted publication; regenerate disposable output under the current contract.

**Cleanup:** Remove duplicate source/cooked authority, stale reachable products and superseded serializers or asset scans in the changed path. Do not implement an unsupported format to fill an inventory cell.

**Exit:** FCR-CONT-01/02/03 bind selected assets to complete package-relative products, deterministic identities, rights and failure-safe publication. Unsupported visible effects remain blockers or require a scope decision.

```text
Execute only FWC-4 from Docs/Architecture/CrossModule/FirstRelease/FoundationImplementationPlan.md#fwc-4--import-cook-and-engine-assets. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Use existing import/normalization/cook/publication/load for the retained glTF/GLB/FBX subset.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-CONT-01; no neighboring stage or new mechanism merely to fill a report.
Before handoff, update the stage result, remaining plan and next item.

```

## `FWC-5` — Shader Route And Shared Tool Output

**Start:** A selected program, typed binding, publication/reload or tool-output claim needs proof or repair.

**Work:** Use the [remaining shader validation plan](../ShaderSystem/ImplementationPlan.md) for compiler/ABI/native/lifetime checks. Repair the current owner only where it fails. For shared tool output, check the changed consumer's severity/streams/exit/progress/path-quoting/cancellation result; do not repeat unaffected journeys.

**Cleanup:** Delete replaced registrations, shader/binding/generation authority, duplicate output formatting and temporary validation additions. Add no diagnostic schema or compiler service.

**Exit:** FCR-SHDR-01/FCR-TOOL-01 link actual producer-to-consumer results, both required shader targets and affected CLI failures; compile alone is not runtime/package acceptance.

```text
Execute only FWC-5 from Docs/Architecture/CrossModule/FirstRelease/FoundationImplementationPlan.md#fwc-5--shader-route-and-shared-tool-output. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Use the [remaining shader validation plan](../ShaderSystem/ImplementationPlan.md) for compiler/ABI/native/lifetime checks.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-SHDR-01, FCR-TOOL-01; no neighboring stage or new mechanism merely to fill a report.
Before handoff, update the stage result, remaining plan and next item.

```

## `FWC-6` — Foundation Candidate Closure

**Start:** Admitted reports and the exact source/package candidate exist.

**Work:** Join the eleven family dispositions and original artifacts to the current package-relative content-to-frame journey. Run only missing cross-boundary checks; invalidate affected results after code/input changes.

**Cleanup:** Remove stale current-state instructions, duplicate reports and obsolete generated membership. Check that preceding repairs already removed their replaced paths.

**Exit:** Every admitted foundation claim has its candidate decision and required negatives; exclusions have reasons. This stage does not recreate source-present layers or turn local rehearsal into documented reproduction.

```text
Execute only FWC-6 from Docs/Architecture/CrossModule/FirstRelease/FoundationImplementationPlan.md#fwc-6--foundation-candidate-closure. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Join the eleven family dispositions and original artifacts to the current package-relative content-to-frame journey.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to the stage's existing report owner; no neighboring stage or new mechanism merely to fill a report.
Before handoff, update the stage result, remaining plan and next item.

```

## Reference And Ownership

[Module contracts](../../Modules/ModuleInventory.md), [Tasks/world topology](../MultithreadedEngine.md), [shader design](../ShaderSystem/ShaderSystemDesign.md) and [product remaining work](ProductImplementationPlan.md) own the boundaries. This plan routes FCR-CORE-01, FCR-PLAT-01, FCR-TASK-01, FCR-WORLD-01 through 03, FCR-CONT-01 through 03, FCR-SHDR-01 and FCR-TOOL-01; it stores no candidate results.
