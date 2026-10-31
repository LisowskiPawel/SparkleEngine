# Renderer Frame And Scene Closure Plan

[Docs](../../../../../README.md) / [Up](README.md) / [Design](../RendererDesign.md)

Keep the selected frame tied to one immutable world submission, view/settings identity and completion-safe graph execution.

**Status:** remaining release integration and proof; stage names do not imply missing implementation or passed evidence.

## Work This Plan

Every stage and copied prompt follows [stage completion and resume](../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-completion-and-resume). Close its finite local exit once; keep later aggregate proof separate.

Use FS-0 for a missing identity/oracle decision. Otherwise repair or prove FS-1, FS-2 or FS-3 at the earliest incorrect product. Existing coordinator, Scene/View and frame-graph implementations are the starting path.

Run the stage that owns the demonstrated missing contract or failing check. Stage numbers identify responsibilities; they do not require rebuilding existing layers or completing unrelated stages first. Every stage and copied prompt follows the [Stage Source-Style Gate](../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate) and the [shared delivery contract](../../../../CrossModule/FirstRelease/ImplementationPlan.md#phase-card-contract).

## Dependencies

Admission supplies one immutable input. Scene/View preparation consumes it; the graph consumes those prepared products. A valid predecessor needs no reimplementation. Serial/threaded controls compare the same input; both native APIs remain required for final closure.

## `FS-0` — Contract, Identity, And Serial Oracle

**Start:** The selected failure cannot yet be assigned to admission, preparation or graph execution.

**Work:** Trace actual world/control/settings epochs, persistent Scene, per-View state, GPU mirror and output consumer. Identify the existing serial comparison and only missing AC/FM/check cells.

**Cleanup:** Retire obsolete packet/scene assumptions and duplicated contract descriptions; do not restore historical topology or create a scheduler.

**Exit:** One candidate/input/oracle and affected ownership/lifetime/copy boundary identify the next check or repair for FCR-REN-01/02/03.

```text
Execute only FS-0 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/FrameAndSceneImplementationPlan.md#fs-0--contract-identity-and-serial-oracle. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Trace actual world/control/settings epochs, persistent Scene, per-View state, GPU mirror and output consumer.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-01; no neighboring stage or new mechanism merely to fill a report.
```

## `FS-1` — Frame Admission And Coordination

**Start:** Admission, backpressure, control order or shutdown is the affected edge.

**Work:** Exercise monotonic submissions/settings epochs, bounded frame/control queues, wakes/waits, failure propagation, cancellation and shutdown in existing serial/threaded modes. Measure changed coordinator work, latency and queue high-water.

**Cleanup:** Remove redundant pending-frame/control state and forwarding coordination the repair replaces. Keep one immutable world input; retries or another queue cannot hide a broken producer edge.

**Exit:** Affected FCR-REN-01 pressure/order/failure/shutdown checks pass with equivalent serial/threaded results and owned time/queue limits.

```text
Execute only FS-1 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/FrameAndSceneImplementationPlan.md#fs-1--frame-admission-and-coordination. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Exercise monotonic submissions/settings epochs, bounded frame/control queues, wakes/waits, failure propagation, cancellation and shutdown in existing serial/threaded modes.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-01; no neighboring stage or new mechanism merely to fill a report.
```

## `FS-2` — Scene, View, And GPU-Scene Preparation

**Start:** Published input is valid, but scene/view identity, upload or lifetime proof is missing or wrong.

**Work:** Repair structural/dynamic publication, resource tables, allocation/reuse, bindings and completion-driven retirement. Check create/update/delete/reuse/reload, skin/morph updates and per-view/history isolation on admitted view routes. Resolve required snapshots once per epoch; account for changed copies/uploads/high-water.

**Cleanup:** Delete duplicated scene truth, replaced GPU mirror/publication records and redundant view products; preserve snapshots required by a real lifetime or ABI boundary.

**Exit:** FCR-REN-02 affected semantic/generation/isolation and all-queue retirement checks pass; no stale binding or unbounded mirror growth remains.

```text
Execute only FS-2 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/FrameAndSceneImplementationPlan.md#fs-2--scene-view-and-gpu-scene-preparation. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Repair structural/dynamic publication, resource tables, allocation/reuse, bindings and completion-driven retirement.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-02; no neighboring stage or new mechanism merely to fill a report.
```

## `FS-3` — Frame Graph Compile And Execution

**Start:** Required frame inputs exist; a graph dependency, resource/state, recording or retirement claim fails or remains unproved.

**Work:** Validate selected production passes' reads/writes/imports/export roots, queues, culling, transient alias/initialization, histories and typed bindings. Exercise missing/conflicting production, invalidation/rebuild, recording/submission failure and serial/parallel equivalence; inspect native barriers/waits and changed compile/record/copy cost.

**Cleanup:** Remove superseded version/frontier records, dead unexported work, duplicate command state and replaced helpers in the owning repair. Missing required production fails; a clear/copy placeholder cannot satisfy it.

**Exit:** Applicable AC-FGS-01 through 08 pass on admitted frame topologies/APIs with truthful failures and completion-safe reuse. No new async queue or graph facility is selected without a measured frame need.

```text
Execute only FS-3 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/FrameAndSceneImplementationPlan.md#fs-3--frame-graph-compile-and-execution. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Validate selected production passes' reads/writes/imports/export roots, queues, culling, transient alias/initialization, histories and typed bindings.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to the stage's existing report owner; no neighboring stage or new mechanism merely to fill a report.
```

## `FS-4` — Frame And Scene Candidate Closure

**Start:** Admitted admission/preparation/graph results and exact candidate are available.

**Work:** Join FCR-REN-01/02/03 to the [principal frame review](../../../../../Acceptance/FirstRelease.md#principal-frame-review): source-to-present lineage, both native captures, hazards/aliasing/queues, serial/parallel equivalence, provider transitions and bounded rebuild/retirement. Fill only missing integration cells.

**Cleanup:** Verify stage-local deletions and stale includes/registrations/current docs against the final frame; remove completed instructions after retaining original proof.

**Exit:** The three reports agree on source/content/settings identity and required failure/native/cost/lifetime results. An empty graph or compiler-plan comparison is not production-frame proof.

```text
Execute only FS-4 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/FrameAndSceneImplementationPlan.md#fs-4--frame-and-scene-candidate-closure. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Join FCR-REN-01/02/03 to the [principal frame review](../../../../../Acceptance/FirstRelease.md#principal-frame-review): source-to-present lineage, both native captures, hazards/aliasing/queues, serial/parallel equivalence, provider transitions and bounded rebuild/retirement.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-01; no neighboring stage or new mechanism merely to fill a report.
```

## Reference And Ownership

[Frame execution](../Features/FrameExecution/FrameExecutionDesign.md), [Scene/View preparation](../Features/SceneAndViewPreparation/SceneAndViewPreparationDesign.md) and [frame graph acceptance](../Features/FrameExecution/FrameGraphAndScheduling.md) own behavior and proof. This plan routes FCR-REN-01 through 03; [Roadmap](../../../../../Strategy/Roadmap.md) owns global order.
