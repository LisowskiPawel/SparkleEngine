# Performance Diagnostics Delivery Plan

[Docs](../../../README.md) / [Up](PerformanceDiagnosticsDesign.md)

Make a selected rendering result observable through existing native tools. Add internal measurement only when those tools and current producers cannot answer a named question.

**Status:** remaining external-capture work; internal diagnostics packages are unselected options, not a scheduled product

**Scope reconciliation:** 2026-10-24. This pass changes delivery obligations, not source readiness or candidate evidence. [Roadmap](../../../Strategy/Roadmap.md#where-we-are-and-what-is-next) selects current work; [scope protection](../../../Strategy/Roadmap.md#scope-protection) governs every phase and prompt.

## Work This Plan

Every stage and copied prompt follows [stage completion and resume](../../../Engineering/Workflow/ChangeLifecycle.md#stage-completion-and-resume). The [state update](../../../Engineering/Workflow/ChangeLifecycle.md#update-stage-state-before-handoff) is part of its exit: record the result, reconcile remaining work and name the next item before handoff. Later aggregate proof remains separate.

Use [qualified GPU crash debugging](GpuCrashDebugging/GpuCrashDebuggingDesign.md#acceptance-and-controlled-failures) for current release failures. [External Capture](ExternalCapture/ImplementationPlan.md) owns the remaining PIX, Nsight and RenderDoc implementation and proof. Reuse delivered adapters and valid evidence.

The larger stat/workspace/visualizer design remains a conditional reference. Neither release acceptance nor Workload Studies requires building it in full. A missing metric justifies its smallest existing-owner producer/export path, with explicit admission and cost evidence.

## Delivery At A Glance

| Work | Admitted boundary | Do not deliver by implication |
| --- | --- | --- |
| External capture | Existing selected provider/backend cells in the child plan; preserve PIX, Nsight, RenderDoc priority. | A second capture authority, new vendor suite or internal profiler. |
| Release measurements | Exact frame, memory and native evidence required by the release contract, through existing tools/producers. | Every metric, catalog scene, overlay and investigator proposed in the target design. |
| Later workload analysis | One original-record consumer under [Workload Studies](WorkloadStudies/ImplementationPlan.md), after its admission and producer contract. | A database, dashboard, runner service or another serializer. |
| Internal diagnostics | No package currently selected. A future bounded consumer may admit one coherent slice and only its necessary dependencies. | The full Phase 2-5 program or a framework awaiting future presenters. |

The [design](PerformanceDiagnosticsDesign.md) owns metric semantics, lifetime, bounds and conditional UI. [Acceptance](Acceptance.md) owns observer/comparison and feature proof. [Visual Design](VisualDesign.md) supplies conditional presentation, not a release checklist. Existing provisional capacities and overhead limits remain intact for any package that actually selects them.

## Phase 0 — Reconcile And Choose

**Prerequisite:** one Roadmap-selected consumer or unresolved external-capture decision.

Inspect live producers, consumers, build membership and retained evidence. Close only decisions used by that slice. Use Empty/Sponza for first controls and name the admitted scene/API/profile roster before checks; the entire development catalog is not an automatic dependency. Release and aggregate-provider coverage remain mandatory within their separately declared matrices.

**Exit (`P0-GATE`):** exact scope, current owner, input/result identity, native completion/lifetime, failure behavior, tool prerequisites, check cards and cleanup are explicit. Unselected metric/session/UI questions do not block a valid external slice.

```text
Execute only unresolved P0-GATE decisions needed by the Roadmap-selected slice from Docs/Architecture/CrossModule/PerformanceDiagnostics/ImplementationPlan.md. For capture, use child Stage 0. Freeze current owners, exact matrix and falsifying checks; add no production framework or internal diagnostics package. Apply the owning execution contract and its Stage Source-Style Gate.
Before handoff, update the stage result, remaining plan and next item.

```

## Phase 1 — Prove External Capture End To End

**Delivery owner:** [External Capture staged plan](ExternalCapture/ImplementationPlan.md). It retains the exact `EXT-00` through `EXT-05` cells and provider-specific discovery, failure, native finalization, artifact, observer, Shipping-erasure and separate-review requirements.

**Exit (`P1-GATE`):** every selected external cell has its candidate-bound result; aggregate closure covers the child's frozen matrix. Missing tools, hardware or checks remain blockers for those cells. A qualified PIX cell stays useful while later cells remain open. No internal collection, history, query or UI product is needed to pass external capture.

```text
Execute only the earliest admitted remaining stage of Docs/Architecture/CrossModule/PerformanceDiagnostics/ExternalCapture/ImplementationPlan.md. Preserve PIX, Nsight, RenderDoc priority and current qualified cells. Reuse delivered engine repairs; retain exact candidate and native artifacts. Do not start internal phases from this instruction. Apply the owning execution contract and its Stage Source-Style Gate.
Before handoff, update the stage result, remaining plan and next item.

```

## Phase 2 — Build The Bounded Data Spine

**Disposition:** unselected. `P1-GATE` is a prerequisite, not permission to start.

Before selection, demonstrate the missing fact that an existing timing, memory or native-tool route cannot provide. Admit one real exporter or presenter with its producer. Resolve only its identity, delayed publication, capacity, overflow, cadence and observer decisions; do not build the full data model first.

**Exit (`P2-GATE`, if selected):** producer-to-consumer correctness, stale/out-of-order/loss controls, exact units and lifetime, measured bounds, Shipping erasure and replaced-path deletion. Use the design's existing Basic-mode budget if Basic is selected.

```text
Read Docs/Architecture/CrossModule/PerformanceDiagnostics/ImplementationPlan.md#phase-2--build-the-bounded-data-spine; apply its execution contract, named stage work/exit and Stage Source-Style Gate.
Do not execute Phase 2 without a recorded consumer and scope admission. If admitted, deliver only its selected FND producer-to-consumer slice under this plan's Verification Comes First section; remove duplicate collectors and state in the same change.
Before handoff, update the stage result, remaining plan and next item.

```

## Phase 3 — Ship The First Orientation Surface

**Disposition:** unselected. Current FPS text does not justify a twelve-group stat product.

A selected surface consumes accepted measurements; it creates no sampler or history authority. Prefer one existing UI location. A command catalog, graph, Editor workspace and DevelopmentGame presenter are independent choices.

**Exit (`P3-GATE`, if selected):** one usable action, correct units/identity/unavailable state, bounded rows and cost, no competing FPS truth or replaced presenter, and applicable Shipping erasure.

```text
Read Docs/Architecture/CrossModule/PerformanceDiagnostics/ImplementationPlan.md#phase-3--ship-the-first-orientation-surface; apply its execution contract, named stage work/exit and Stage Source-Style Gate.
Do not execute Phase 3 without an admitted orientation consumer. If selected, extend one current presentation route with accepted measurements, prove its states and cost, and delete the presentation it replaces. Do not implement other ORI groups by adjacency.
Before handoff, update the stage result, remaining plan and next item.

```

## Phase 4 — Deliver Reproducible Evidence

**Disposition:** conditional on an admitted workload and actual producer inputs. This does not select Phase 3 or Phase 5.

Reuse current capture/export products, workload identity and file publication. Add only missing readiness/raw fields needed by the declared check. A Workload Studies WS-S0 producer card can admit that bounded prerequisite before the analyzer's WS-D0 is complete: freeze its actual field/identity/unit, owner, falsifier, observer/capacity bounds and deletion first; qualify the real record, then return it to WS-S0. If a necessary producer does not exist, admit only its minimum FND-to-record slice with this consumer, not the whole Phase-2 spine. REL-11 and applicable parent gates still apply. The workload owns schema, destination, samples and claims; one serializer owns each actual artifact. The [comparison contract](Acceptance.md#comparison-and-regression-contract) retains raw/per-run results, at least three runs of 300 warm valid frames where applicable, uncertainty and `Inconclusive`.

**Exit (`P4-GATE`, if selected):** immutable comparable inputs, deterministic readiness and requested/active identity, complete publication or explicit failure, no default reports, and independently recomputable summaries. No internal schema versions, migration readers or duplicate evidence store.

```text
Read Docs/Architecture/CrossModule/PerformanceDiagnostics/ImplementationPlan.md#phase-4--deliver-reproducible-evidence; apply its execution contract, named stage work/exit and Stage Source-Style Gate.
Execute only an admitted Phase 4 evidence delta for the selected workload, including a necessary producer prerequisite frozen by WS-S0. Prefer existing artifacts; if insufficient, deliver the smallest owner-to-record change under its own frozen check/observer/capacity/deletion card, then return the qualified sample to WS-S0. Completed analyzer WS-D0 is not this producer's prerequisite. Preserve REL-11 and applicable parent gates, reject partial/mismatched records and retain raw identities/negative controls. Add no workspace, complete data spine or generalized runner.
Before handoff, update the stage result, remaining plan and next item.

```

## Phase 5 — Add Selected Internal Investigation Depth

**Disposition:** unselected. External tools are the default for causal investigation.

A workspace, CPU/GPU/memory view, pass-query hierarchy, task browser, hitch history or `ProfileGpu` needs a demonstrated recurring question that existing tooling cannot answer economically. Admit each separately; measure disturbance before retaining it. A negative value result deletes the experiment and dependencies while retaining useful evidence.

**Exit (`P5-GATE`, if selected):** one consumer answers that question within the design's bounds, preserves parallel recording and authoritative marker/resource identity, and has observer, lifetime, failure and Shipping-erasure proof. No unselected neighboring collectors or UI remain.

```text
Read Docs/Architecture/CrossModule/PerformanceDiagnostics/ImplementationPlan.md#phase-5--add-selected-internal-investigation-depth; apply its execution contract, named stage work/exit and Stage Source-Style Gate.
Do not execute Phase 5 without explicit package admission and a failed simpler alternative. If admitted, deliver one bounded investigation slice, measure its value and observer cost, and delete rejected experiments or duplicate production paths. Native-tool detail stays in native artifacts.
Before handoff, update the stage result, remaining plan and next item.

```

## Phase 6 — Harden And Close The Selected Product

Close only the selected scope. Preserve candidate-bound positive/negative, backend/profile, observer, shutdown, package and documented-reproduction checks applicable to that scope. Unselected packages remain deferred; do not implement or exercise them to complete a ledger.

**Exit:** `FIN-01` through `FIN-04` close the admitted matrix, failures/lifetimes, observer/capacity and deletion/documentation respectively. No unresolved required cell becomes a pass. A narrow result does not claim every development map's performance is understood.

```text
Read Docs/Architecture/CrossModule/PerformanceDiagnostics/ImplementationPlan.md#phase-6--harden-and-close-the-selected-product; apply its execution contract, named stage work/exit and Stage Source-Style Gate.
Execute Phase 6 only for the selected diagnostics candidate. Close its exact FIN matrix and failures, preserve original artifacts and limitations, remove stale code/selectors/build entries and completed instructions, and report the bounded verdict to the existing FCR/workload owner.
Before handoff, update the stage result, remaining plan and next item.

```

## Ship-Control Ledger

Stable IDs below identify possible delivery units; they are not runtime registrations or requirements to create one class/file per row.

| IDs | Responsibility | Current delivery disposition |
| --- | --- | --- |
| `P0-GATE`, `P1-GATE` | External selection/discovery and aggregate native acceptance. | Remaining work in the child plan; no replay of valid delivered cells. |
| `EXT-00`, `EXT-01` | Existing neutral capture route and PIX D3D12. | Selected child scope; prove remaining lifecycle/delivery cells. |
| `EXT-04`, `EXT-05` | Nsight D3D12 and Vulkan. | Selected child scope; experimental/native prerequisites retained. |
| `EXT-02`, `EXT-03` | RenderDoc D3D12 and Vulkan. | Selected child scope; qualify current adapters. |
| `FND-01` | Application demand/join/publication. | Deferred; only if an admitted consumer needs a cross-domain join. |
| `FND-02`, `FND-03` | Host interval/phases; Renderer CPU/waits/top-level GPU spans. | Deferred; reuse existing measurements first. |
| `FND-04`, `FND-05` | Process memory; GPU memory identity/segments. | Deferred; add only missing workload facts. |
| `P2-GATE` | Selected foundation semantics, bounds and Shipping erasure. | Applies only to selected FND work. |
| `ORI-01` | Shared typed Stat request. | Deferred; not required by one existing presenter. |
| `ORI-02`, `ORI-03`, `ORI-04` | Fps, Unit, UnitGraph. | Deferred independently; no automatic group catalog. |
| `ORI-05`, `ORI-06` | Editor Quick Check; DevelopmentGame presentation. | Deferred independently. |
| `P3-GATE` | Selected orientation usability and cost. | Applies only to selected ORI work. |
| `EVD-01`, `EVD-02` | Workload readiness; raw/manifest export. | Conditional; reuse existing producers and publication. |
| `EVD-03`, `EVD-04` | MAP-00 calibration; repeat-run comparison. | Conditional; workload/analysis owners retain their admission. |
| `P4-GATE` | Reproducibility, no-default-files and erasure. | Applies only to selected EVD work. |
| `INV-00` | Shared Performance workspace/selection. | Deferred; no empty workspace. |
| `INV-01`, `INV-02`, `INV-03`, `INV-04` | CPU, GPU, GPU-pass and memory views. | Deferred independently. |
| `INV-05`, `INV-06`, `INV-07`, `INV-08` | Render/RHI counts, task aggregates, scene cardinality, hitch navigation. | Deferred independently; no diagnostic scans of production state. |
| `CAP-00`, `CAP-01` | Bounded parallel-preserving query plan; frozen ProfileGpu. | Deferred; native capture is the default. |
| `P5-GATE` | Investigation value, topology, bounds and erasure. | Applies only to selected INV/CAP work. |
| `FIN-01`, `FIN-02`, `FIN-03`, `FIN-04` | Admitted matrix, failures, observer/bounds and deletion closure. | Required for the scope actually delivered. |

## Verification Comes First

Use [Change Lifecycle](../../../Engineering/Workflow/ChangeLifecycle.md), its [scope/retirement decision](../../../Engineering/Workflow/ChangeLifecycle.md#scope-and-retirement-decision) and [source-style gate](../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate), plus the [Engineering task map](../../../Engineering/README.md#choose-by-task). Keep one small iteration record containing:

- selected output, actual consumer, exact source/configuration and workload/tool matrix;
- existing owners, producer/consumer/lifetime and the smallest necessary delta;
- mapped feature AC/FM/CHK, independent oracle, capacities/deadlines and observer controls frozen before results;
- additions and same-stage deletion of duplicated measurements, histories, presenters, commands, schemas or dependencies; justify a zero-deletion result;
- exact commands, raw artifacts, failure/cleanup observations and unavailable required checks.

Use existing commands and disposable local probes. Check IDs do not authorize committed test executables, CTest registrations, fixture libraries, production fault APIs or permanent reporting machinery. Remove temporary validation additions before handoff and retain existing tests. Escalate through [claim-driven validation](../../../Engineering/Verification/ValidationAndEvidence.md#claim-driven-validation-selection); do not prescribe all-map, whole-engine, paired-build or clean-rebuild loops for every package. Aggregate acceptance still covers every cell actually promised.

Shipping erasure, zero authoring tax, exact identity/validity, bounded storage, preserved recording topology and safe native retirement remain mandatory when applicable. Measure diagnostics `Off` versus the selected observer mode; runtime Off alone does not establish erasure. Run `architecture_boundary_check` only when Renderer/RHI boundaries change, and `git diff --check` before handoff.

## Reference And Ownership

| Question | Owner |
| --- | --- |
| What should run next? | [Roadmap](../../../Strategy/Roadmap.md) |
| What does a metric/request mean and cost? | [Diagnostics design](PerformanceDiagnosticsDesign.md) |
| Which original artifacts and comparison controls are required? | [Acceptance](Acceptance.md), [Graphics Workloads](../../../Acceptance/GraphicsWorkloads.md) |
| What remains for each native provider? | [External Capture](ExternalCapture/ImplementationPlan.md) |
| Where does the offline analysis consumer belong? | [Workload Studies](WorkloadStudies/ImplementationPlan.md) |
| How do selected internal views behave? | [Visual Design](VisualDesign.md); conditional, no admission by wireframe |
| What exists in source? | [Capability snapshot](Capability.md), revalidated against live code |
