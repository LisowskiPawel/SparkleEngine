# Owned Neural Denoising Delivery Plan

[Docs](../../../README.md) / [Up](NeuralGraphicsDesign.md)

Stages: [NG-S0](#ng-s0---freeze-discovery) / [NG-S1](#ng-s1---deterministic-dataset) / [NG-S2](#ng-s2---owned-training-and-model) / [NG-S3](#ng-s3---export-and-fp32-conformance) / [NG-S4](#ng-s4---runtime-vertical-slice) / [NG-S5](#ng-s5---measured-kernel-optimization) / [NG-S6](#ng-s6---evaluation-delivery-and-adoption).

Build one owned diffuse-indirect denoiser, from a licensed dataset through checked shader inference and independent evaluation.

Its [evidence home](../../../Acceptance/GraphicsWorkloads.md#evidence-folders-and-delivery-owners) separates component, classical baseline, data, training, conformance, runtime and evaluation artifacts under one exact candidate. Each stage links its original manifests/results there; it does not create another model registry or readiness ledger.

**Status:** remaining target work; conditional stage plan awaiting `NG-D0`, `REL-11` and `M3`

## Work This Plan

| What to do | Working route |
| --- | --- |
| First action | [NG-S0 discovery](#remaining-stages): ratify the concrete [Semantics](Semantics.md) and [Execution Architecture](ExecutionArchitecture.md) proposals through the nine NG-D0 cells, using actual environment, rights and oracle inputs. |
| Result to reach | Accepted discovery before dataset/training/export/runtime work; prove FP32 conformance before optimization. |
| Start / stop | Production waits for NG-D0, REL-11, M3 and stage-specific reference/data/model proof. Earlier documentation does not authorize training or an ML framework. |

## Stage Contract

Apply [Change Integration](../../../Engineering/Workflow/ChangeIntegration.md), [Change Lifecycle](../../../Engineering/Workflow/ChangeLifecycle.md) and the [delivery package](../../../Engineering/Workflow/Templates/FeatureDeliveryPackage.md). Before each stage freeze the candidate, prerequisite revisions, file allowlist, producer/consumer/deletion and hook/copy ledgers, AC/FM/CHK mappings and smallest checks. Use existing shader/frame-graph/Assets/runtime/package ownership. Its [stage complexity budget](../../../Engineering/Workflow/ChangeLifecycle.md#define-the-outcome-and-complexity-budget) applies to both current scripts/runtime code and proposed additions: name what to retain, narrow or delete and compare actual state/API/hooks/copies/variants/dependencies at exit. An unused abstraction or postponed replacement deletion keeps that stage open.

Each exit includes applicable formatting and logical-spacing checks from [Stage Source-Style Gate](../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate), review of SRP/orchestration/naming, narrow builds/checks, directly affected docs and `git diff --check`. Run `architecture_boundary_check` when Renderer/RHI boundaries change. Disposable tests/probes are removed; retained artifacts go to the existing completion/workload report. No source/static check earns a numerical, GPU, performance, package or adoption verdict.

## Remaining Stages

| Stage | Prerequisite and bounded work | Exit, failure and deletion obligations |
| --- | --- | --- |
| [`NG-S0`](#ng-s0---freeze-discovery) Freeze discovery | Ratify or explicitly revise [Semantics](Semantics.md), [Execution Architecture](ExecutionArchitecture.md) and the limits in [Research](Research.md) through [NG-D0](Discovery.md); no production work. | All nine cells closed against a reviewed revision: component derivation/check card, installed environment, numeric quality/motion/edge and cost limits, dataset rights and per-slice addition/retirement decisions. Remove superseded decision text and probes; no tensor API, schema or framework lands. Missing oracle, rights or lifetime remains explicit. |
| [`NG-S1`](#ng-s1---deterministic-dataset) Qualified component, baseline and dataset | `NG-D0`, `REL-11`, `M3` and accepted base Reference prerequisites. Execute the three owner slices below in order: qualified Reference component, fixed classical filter/guide proof, then one data builder over immutable raw exports. | `AC-NG-01/02/09`, `FM-NG-01`, `CHK-NG-01`: component/leakage/PDF/prefix and classical CPU/GPU controls, affected Reference requalification, repeated data generation, split/partial/non-finite/uncertainty negatives. Beauty is not a label. Reuse transport/export/schema owners; remove replaced routes per slice. No AOV or denoising framework. |
| [`NG-S2`](#ng-s2---owned-training-and-model) Owned training and model | Accepted dataset. Execute the ratified eleven-channel `11→16→16→3` residual CNN recipe, three seeds, tiny-batch/gradient diagnostics and validation-only guide/residual ablations. | `AC-NG-03`, `CHK-NG-01`: repeat tolerance, untouched scene/sequence-held-out split, classical/noisy baselines and model card. A topology/recipe revision returns affected cells to discovery; remove rejected supported variants while retaining experiment evidence. |
| [`NG-S3`](#ng-s3---export-and-fp32-conformance) Export and FP32 conformance | Accepted model/semantics. Export the one fixed OIHW/bias artifact and NHWC16 FP32 operators through existing ShaderSystem; preserve per-layer replicate padding and residual/decode rules. | `AC-NG-04`, `FM-NG-02`, `CHK-NG-02`: independent float64, PyTorch/export and D3D12/Vulkan layer/full-model comparisons against the ratified tolerance, with seeded transpose/padding/channel faults and near-zero/extreme/invalid controls. Delete replaced conversions; no graph interpreter or operator catalog. |
| [`NG-S4`](#ng-s4---runtime-vertical-slice) Runtime vertical slice | FP32 conformance and runtime ledger. Three typed compute passes, RR-independent existing guides, real surface-valid mask, two transient hidden buffers, one output, generation lifetime and classical fallback. | `AC-NG-05/07`, `FM-NG-03/05`, `CHK-NG-02/03`: actual output and bounded invalid-output publication, black-surface validity, RR-off, missing/corrupt/unsupported/allocation/compile/all-queue retirement/shutdown controls. Delete replaced inference/upload/state paths; no copied scene, second model owner or generic runtime. |
| [`NG-S5`](#ng-s5---measured-kernel-optimization) Measured kernel optimization | Correct runtime baseline and a measured limiter. Choose only the precision/layout/packing/dispatch/tiling/fusion or optional vector experiment that tests that limiter; the option list is not a delivery checklist. | `AC-NG-04/06`, `FM-NG-04`, `CHK-NG-02/03`: numeric drift and full-frame quality/latency/memory/observer results for each selected variant. Preserve the required FP32 correctness baseline, delete rejected product variants/registrations/settings/dependencies in this stage. A negative result cannot become a speed claim or leave a dormant optimization switch. |
| [`NG-S6`](#ng-s6---evaluation-delivery-and-adoption) Evaluation, delivery and adoption | Supported runtime candidate. Use sealed San Miguel for final testing; Bistro remains the declared validation scene. Evaluate the frozen quality/cost Pareto, motion/edge/bias bounds, fallback population and failures; deliver licenses/manifests and independent reproduction through existing records/tools. | `AC-NG-01..09`, `FM-NG-01..06`, `CHK-NG-01..04` complete for the declared matrix; `WL-07/08`. No test-driven model selection, hidden fallback or inherited paper result. Verify earlier cleanup/addition budgets and remove proof-only code; no benchmark/report/experiment platform. Publish only supported claims. |

No unresolved architecture, threshold or tensor choice moves from discovery into a stage prompt. The first failed prerequisite stops dependent work, while independent documentation/research can continue. A scope revision returns affected cells to discovery and invalidates affected evidence.

## Ready-To-Use Stage Prompts

Use the stage contract above with every prompt; these are bounded entries, not permission to execute multiple stages.

### NG-S0 - Freeze Discovery

<details>
<summary>Copy implementation prompt</summary>

```text
Execute NG-S0 only from Docs/Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md#ng-s0---freeze-discovery. Apply the owning execution contract and its Stage Source-Style Gate.
Ratify or explicitly revise Semantics.md and ExecutionArchitecture.md through NG-D0 against current owners and actual reference, rights, dataset, environment and lifetime inputs. Freeze the component derivation/check card, operator/layout recipe, conformance/value/cost limits and actual motion/edge thresholds; close all nine cells or name unresolved owners. Use Research.md as precedent, not results. Documentation/research only: preserve REL-11/M3 production admission; do not train, add runtime code or claim acceptance.
```

</details>

### NG-S1 - Deterministic Dataset

Each row is a separate reviewable owner slice. Execute only the first unfinished row; NG-S1 closes after all three pass, not when the builder first emits files.

| Order | Consumer and bounded delta | Exit and cleanup |
| --- | --- | --- |
| 1a Reference component | Existing Reference transport/product/raw transaction delivers the derived diffuse-indirect measurement, component statistics and exact prefix identity. | `AC-NG-09`, affected Reference checks and isolated leakage/PDF/prefix/uncertainty controls. Preserve beauty and accepted math; delete any replaced output path. No dataset publication yet. |
| 1b Classical baseline and guides | Same denoising owner delivers the [fixed classical filter](Semantics.md#bounded-classical-baseline), one shared helper/baseline dispatch and qualified RR-independent guides/mask. | Independent CPU/GPU flat/impulse/edge/black/non-finite controls on both APIs, exact source/runtime joins and measured baseline. Remove redundant helper/guide copies; no filter registry, temporal framework or neural inference. |
| 1c Dataset publication | One Tools builder consumes qualified raw/component/guide records, using the ratified real-time noise identity, licensed scene/sequence split and one immutable manifest route. | `AC-NG-02`, repeated generation, alignment/leakage/partial/non-finite/uncertainty negatives and classical/noisy comparison. Delete replaced ad hoc supported generators; no training service or duplicated scene. |

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md#ng-s1---deterministic-dataset and apply its Stage Contract, including the Stage Source-Style Gate.
Execute only the first unfinished NG-S1 owner slice in this section, after NG-D0/REL-11/M3 and accepted base Reference prerequisites: 1a qualified component, then 1b fixed classical filter/RR-independent guides, then 1c dataset publication. Use ratified Semantics.md and ExecutionArchitecture.md; current beauty is not a label and vendor RR is not the spatial baseline. Apply that row's controls, affected owner checks and same-slice deletion. Preserve math/beauty, raw identity, rights and scene/sequence split. Handoff the row's exact proof and remaining NG-S1 obligations; stop on an unproved producer or unledgered hook. No second tracer, AOV/filter registry, model training or inference integration.
```

</details>

### NG-S2 - Owned Training And Model

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md#ng-s2---owned-training-and-model and apply its Stage Contract, including the Stage Source-Style Gate.
Execute NG-S2 only after the accepted NG-S1 dataset. Follow ratified Semantics.md: eleven channels, fixed residual CNN, loss/halo, three seeds, environment and repeat tolerance. Complete tiny-batch overfit, finite-difference gradient, noisy/classical baselines and validation-only guide/residual ablations. Keep San Miguel sealed for final test. Publish model/recipe identity and failures; delete rejected supported variants. Handoff AC-NG-03/CHK-NG-01. Stop on leakage, unfrozen thresholds or non-finite training; return a necessary recipe change to discovery. No inference integration.
```

</details>

### NG-S3 - Export And FP32 Conformance

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md#ng-s3---export-and-fp32-conformance and apply its Stage Contract, including the Stage Source-Style Gate.
Execute NG-S3 only after accepted model semantics. Inspect ShaderSystem cook/reflection/ABI and publication. Implement the frozen fixed OIHW/bias export and NHWC16 FP32 operators, per-layer replicate padding and residual/decode. Compare layers and full model to independent float64 and PyTorch references on D3D12/Vulkan at ratified tolerances; seed layout/channel/padding faults and test near-zero/extreme/invalid inputs. Keep native lowering private and one weight identity; delete replaced conversions. Handoff AC-NG-04/CHK-NG-02. Stop on numerical mismatch, guessed semantics or unowned lifetime. No FP16/vendor optimization.
```

</details>

### NG-S4 - Runtime Vertical Slice

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md#ng-s4---runtime-vertical-slice and apply its Stage Contract, including the Stage Source-Style Gate.
Execute NG-S4 only after NG-S3 conformance. Follow ExecutionArchitecture.md: three typed passes, two transient hidden buffers, one output, existing RR-independent guides and genuine surface validity. Prove black-surface/RR-off behavior, bounded invalid-output publication, input/model/output generations, declared dependencies, all-queue retirement and truthful fallback under failure/shutdown. Follow frozen hook/copy/deletion limits, remove replaced paths and run narrow owner/boundary checks. Handoff AC-NG-05/07. Stop on guessed guides, unowned lifetime or hidden fallback. No copied scene, second uploader or generic tensor runtime.
```

</details>

### NG-S5 - Measured Kernel Optimization

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md#ng-s5---measured-kernel-optimization and apply its Stage Contract, including the Stage Source-Style Gate.
Execute NG-S5 only after accepted FP32 runtime. Start from Semantics.md's operation/byte model, predict the actual limiter and profile the frozen workloads. Test only justified precision/layout/dispatch/fusion or supported optional-vector candidates, one hypothesis per comparison. Freeze controls before results; retain prediction/measurement, full-frame latency, peak overlap, quality/drift and observer context. Preserve the correct baseline, delete rejected product variants and retain negative/inconclusive results. Handoff AC-NG-04/06 and exact tuples. Stop on drift, hidden settings changes or threshold edits. No mandatory new vendor SDK.
```

</details>

### NG-S6 - Evaluation, Delivery And Adoption

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md#ng-s6---evaluation-delivery-and-adoption and apply its Stage Contract, including the Stage Source-Style Gate.
Execute NG-S6 only after the supported runtime/optimization candidate. Close AC-NG-01..09, existing FM-NG/CHK-NG and WL-07/08 against the ratified Semantics.md value/cost and motion/edge/bias rules. San Miguel is sealed test; Bistro remains validation. Report reference uncertainty, per-scene/mask/fallback populations, observer controls, package/model/kernel/license joins and actual non-author reproduction. Reuse only valid candidate-bound proof; remove probes/obsolete routes and record results in existing reports. Stop on missing adopter, model mismatch, unsafe failure, failed value or incomplete package; preserve valid individual cells. No test-driven topology selection or new results authority.
```

</details>

Every production prompt inherits this non-negotiable exit: one identity and owner per data/runtime boundary; accepted reference and frozen numerical rules; no held-out leakage; correct FP32 route before optimizations; actual GPU/native results distinguished from source; complete controlled failures; no unledgered hooks or duplicate mechanisms. Quote each clause with retained proof or `BLOCKED` at handoff.

## Reference And Ownership

<details>
<summary>Exact owners, source snapshots and related contracts</summary>

**Authority:** [Discovery](Discovery.md) freezes the slice; [Training And Evaluation](NeuralGraphicsDesign.md#neural-graphics-training-and-evaluation), [Model To Kernel And Runtime Inference](NeuralGraphicsDesign.md#model-to-kernel-and-runtime-inference) and [Acceptance](NeuralGraphicsDesign.md#neural-graphics-acceptance) own contracts. [Roadmap](../../../Strategy/Roadmap.md) owns admission. No stage has candidate acceptance here.

This plan delivers one owned diffuse-indirect denoiser, with a useful classical fallback and both Windows graphics APIs. It deliberately has no calendar targets or time estimates. Shared engine repairs need an existing consumer and independently useful proof; do not hide a diagnostics/ML framework in this feature.

</details>
