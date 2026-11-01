# Performance Diagnostics Delivery Plan

[Docs](../../../README.md) / [Up](PerformanceDiagnosticsDesign.md)

Make a selected rendering result observable through existing native tools. Add internal measurement only when those tools and current producers cannot answer a named question.

**Status:** remaining external-capture work; internal diagnostics packages are unselected options, not a scheduled product

**Scope reconciliation:** 2026-11-01. This pass changes delivery obligations, not source readiness or candidate evidence. [Roadmap](../../../Strategy/Roadmap.md#where-we-are-and-what-is-next) selects current work; [scope protection](../../../Strategy/Roadmap.md#scope-protection) governs every phase and prompt.

## Work This Plan

Every stage and copied prompt follows [stage completion and resume](../../../Engineering/Workflow/ChangeLifecycle.md#stage-completion-and-resume). The [state update](../../../Engineering/Workflow/ChangeLifecycle.md#update-stage-state-before-handoff) is part of its exit: record the result, reconcile remaining work and name the next item before handoff. Later aggregate proof remains separate.

Use [qualified GPU crash debugging](GpuCrashDebugging/GpuCrashDebuggingDesign.md#acceptance-and-controlled-failures) for current release failures. [External Capture](ExternalCapture/ImplementationPlan.md) owns qualification of the selected existing PIX, Nsight and RenderDoc routes; new integrations are deferred. Reuse delivered adapters and valid evidence.

The larger stat/workspace/visualizer design remains a conditional reference. Neither release acceptance nor Workload Studies requires building it in full. A missing metric justifies its smallest existing-owner producer/export path, with explicit admission and cost evidence.

## Delivery At A Glance

| Work | Admitted boundary | Do not deliver by implication |
| --- | --- | --- |
| External capture | Existing Editor adapters needed for the selected frame evidence; PIX first for D3D12, an existing compatible Vulkan route for Vulkan. | New Game controls, Nsight Vulkan integration, a provider-completeness campaign or an internal profiler. |
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

**Delivery owner:** [External Capture staged plan](ExternalCapture/ImplementationPlan.md) and its [selected scope](ExternalCapture/ExternalCaptureDesign.md#product-and-support-disposition). Qualify only the existing routes needed for an admitted rendering measurement. Native finalization, artifact identity, failure/lifetime, observer and Shipping-erasure checks remain required for each claimed route. Reuse shared delivery proof; provider-complete acceptance is not a public-release prerequisite.

**Exit (`P1-GATE`):** every selected external cell has its candidate-bound result; aggregate closure covers the child's frozen matrix. Missing tools, hardware or checks remain blockers for those cells. A qualified PIX cell stays useful while later cells remain open. No internal collection, history, query or UI product is needed to pass external capture.

```text
Execute only the admitted remaining provider slice needed by the selected rendering question in Docs/Architecture/CrossModule/PerformanceDiagnostics/ExternalCapture/ImplementationPlan.md. Use PIX first for D3D12 or an existing compatible Vulkan route for Vulkan; unrelated Nsight/RenderDoc gates are not a serial waitlist. Preserve current qualified cells, reuse delivered engine repairs and retain exact candidate/native artifacts. Do not start internal phases from this instruction. Apply the owning execution contract and its Stage Source-Style Gate.
Before handoff, update the stage result, remaining plan and next item.

```

## Phase 2 — Build The Bounded Data Spine

Unselected. No executable stage or production prompt remains. A future rendering measurement must first demonstrate why existing native/runtime records are insufficient, then admit only its smallest producer-to-consumer change under Phase 4; the full spine is not a dependency.

## Phase 3 — Ship The First Orientation Surface

Unselected. Keep current FPS and inspection controls. A stat catalog, graph, workspace or second presenter needs a new consumer/value decision; there is no UI delivery sequence to run from this plan.

## Phase 4 — Deliver Reproducible Evidence

**Disposition:** conditional on an admitted workload and actual producer inputs. This does not select Phase 3 or Phase 5.

Reuse current capture/export products, workload identity and file publication. Add only missing readiness/raw fields needed by the declared check. A Workload Studies WS-S0 producer card can admit that bounded prerequisite before the analyzer's WS-D0 is complete: freeze its actual field/identity/unit, owner, falsifier, observer/capacity bounds and deletion first; qualify the real record, then return it to WS-S0. If a necessary producer does not exist, admit only its minimum FND-to-record slice with this consumer, not the whole Phase-2 spine. REL-11 and the selected producer's actual tool/native prerequisites still apply. Neither unrelated provider acceptance nor the full P1 aggregate or unselected internal phases is an entry prerequisite. The workload owns schema, destination, samples and claims; one serializer owns each actual artifact. The [comparison contract](Acceptance.md#comparison-and-regression-contract) retains raw/per-run results, at least three runs of 300 warm valid frames where applicable, uncertainty and `Inconclusive`.

**Exit (`P4-GATE`, if selected):** immutable comparable inputs, deterministic readiness and requested/active identity, complete publication or explicit failure, no default reports, and independently recomputable summaries. No internal schema versions, migration readers or duplicate evidence store.

```text
Read Docs/Architecture/CrossModule/PerformanceDiagnostics/ImplementationPlan.md#phase-4--deliver-reproducible-evidence; apply its execution contract, named stage work/exit and Stage Source-Style Gate.
Execute only an admitted Phase 4 evidence delta for the selected workload, including a necessary producer prerequisite frozen by WS-S0. Prefer existing artifacts; if insufficient, deliver the smallest owner-to-record change under its own frozen check/observer/capacity/deletion card, then return the qualified sample to WS-S0. Completed analyzer WS-D0 is not this producer's prerequisite. Preserve REL-11 and this producer's actual tool/native prerequisites; do not wait for unrelated provider/full-P1 or internal-phase closure. Reject partial/mismatched records and retain raw identities/negative controls. Add no workspace, complete data spine or generalized runner.
Before handoff, update the stage result, remaining plan and next item.

```

## Phase 5 — Add Selected Internal Investigation Depth

Unselected. Use external tools for CPU/GPU/memory/lifetime investigation. The conditional design retains possible views and visualizers, but their implementation prompts are retired. Any future admission must name a question existing tools cannot answer and the smallest necessary change.

## Phase 6 — Harden And Close The Selected Product

Reconcile the selected child/workload evidence. For external-only scope, Stage 8 supplies the capture, observer, erasure and shared-delivery results; map them to FIN-01..04 here without another capture/package/reproduction campaign. Run only a genuinely missing shared check, and return a discovered defect to its existing owner. Required candidate-bound failure/lifetime/capacity proof remains binding; unselected internal packages create no hardening tasks.

**Exit:** `FIN-01` through `FIN-04` close the admitted matrix, failures/lifetimes, observer/capacity and deletion/documentation respectively. No unresolved required cell becomes a pass. A narrow result does not claim every development map's performance is understood.

```text
Read Docs/Architecture/CrossModule/PerformanceDiagnostics/ImplementationPlan.md#phase-6--harden-and-close-the-selected-product; apply its execution contract, named stage work/exit and Stage Source-Style Gate.
Execute Phase 6 only as the selected candidate's evidence reconciliation. Reuse child Stage-8/workload results for FIN-01..04 and run only missing shared checks; add no second capture, package or reproduction campaign. Route defects/residue to the existing owner with affected proof invalidated. Preserve original artifacts and limitations, remove obsolete completed instructions, and report the bounded verdict to the existing FCR/workload owner.
Before handoff, update the stage result, remaining plan and next item.

```

## Ship-Control Ledger

Stable IDs remain proof references, not runtime registrations or a class/file checklist.

| IDs | Current delivery disposition |
| --- | --- |
| `P0-GATE`, `P1-GATE`, `EXT-00`..`EXT-05` | Qualify the selected existing capture routes under the child scope; deferred providers do not become blockers for the selected matrix. |
| `EVD-01`..`EVD-04`, `P4-GATE` | One workload's missing record or comparison, only when existing exports cannot supply it. |
| `FIN-01`..`FIN-04` | Join the delivered scope's existing failure, lifetime, observer and deletion proof once. |
| `FND-*`, `ORI-*`, `INV-*`, `CAP-*`, `P2-GATE`, `P3-GATE`, `P5-GATE` | Unselected design options; no executable delivery backlog. |

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
