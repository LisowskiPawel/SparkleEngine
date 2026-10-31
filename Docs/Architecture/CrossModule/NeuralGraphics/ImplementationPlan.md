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

Every stage and copied prompt follows [stage completion and resume](../../../Engineering/Workflow/ChangeLifecycle.md#stage-completion-and-resume). Close its finite local exit once; keep later aggregate proof separate.

Apply [Change Integration](../../../Engineering/Workflow/ChangeIntegration.md), [Change Lifecycle](../../../Engineering/Workflow/ChangeLifecycle.md) and the [delivery package](../../../Engineering/Workflow/Templates/FeatureDeliveryPackage.md). Before each stage freeze the candidate, prerequisite revisions, file allowlist, producer/consumer/deletion and hook/copy ledgers, AC/FM/CHK mappings and smallest checks. Use existing shader/frame-graph/Assets/runtime/package ownership. Its [stage complexity budget](../../../Engineering/Workflow/ChangeLifecycle.md#define-the-outcome-and-complexity-budget) applies to both current scripts/runtime code and proposed additions: name what to retain, narrow or delete and compare actual state/API/hooks/copies/variants/dependencies at exit. An unused abstraction or postponed replacement deletion keeps that stage open.

Each exit includes applicable formatting and logical-spacing checks from [Stage Source-Style Gate](../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate), review of SRP/orchestration/naming, narrow builds/checks, directly affected docs and `git diff --check`. Run `architecture_boundary_check` when Renderer/RHI boundaries change. Disposable tests/probes are removed; retained artifacts go to the existing completion/workload report. No source/static check earns a numerical, GPU, performance, package or adoption verdict.

## Remaining Stages

| Stage | Prerequisite and bounded work | Exit, failure and deletion obligations |
| --- | --- | --- |
| [`NG-S0`](#ng-s0---freeze-discovery) Freeze discovery | Ratify or explicitly revise [Semantics](Semantics.md), [Execution Architecture](ExecutionArchitecture.md) and the limits in [Research](Research.md) through [NG-D0](Discovery.md); no production work. | All nine cells closed against a reviewed revision: component derivation/check card, installed environment, numeric quality/motion/edge and cost limits, dataset rights and per-slice addition/retirement decisions. Remove superseded decision text and probes; no tensor API, schema or framework lands. Missing oracle, rights or lifetime remains explicit. |
| [`NG-S1`](#ng-s1---deterministic-dataset) Qualified component, baseline and dataset | `NG-D0`, `REL-11`, `M3` and accepted base Reference prerequisites. Execute the three owner slices below in order: qualified Reference component, fixed classical filter/guide proof, then one data builder over immutable raw exports. | `AC-NG-01/02/09`, `FM-NG-01`, `CHK-NG-01`: component/leakage/PDF/prefix and classical CPU/GPU controls, affected Reference requalification, repeated data generation, split/partial/non-finite/uncertainty negatives. Beauty is not a label. Reuse transport/export/schema owners; remove replaced routes per slice. No AOV or denoising framework. |
| [`NG-S2`](#ng-s2---owned-training-and-model) Owned training and model | Accepted dataset and fixed CNN recipe. Execute three seeds, tiny-batch/gradient/alignment, validation-only guide/residual ablations and the bounded batch 8/16 throughput/memory comparison. | `AC-NG-03`, `CHK-NG-01`: repeat rule, sealed scene/sequence test, separate training profile and model card. The batch probe is not two full training runs; delete rejected supported variants and retain original evidence. |
| [`NG-S3`](#ng-s3---export-and-fp32-conformance) Export and FP32 conformance | Accepted model/semantics. Deliver the one OIHW/bias artifact and NHWC16 FP32 operators through ShaderSystem; perform the bounded FP16 storage-round-trip control through the same operator/evaluator. | `AC-NG-04`, `FM-NG-02`, `CHK-NG-02`: float64/PyTorch/paired-GPU conformance, seeded layout/edge/channel/finite faults and scoped quantization drift/bytes. No native half-speed claim without native work; remove prototype formats/conversions, not retain dual readers. |
| [`NG-S4`](#ng-s4---runtime-vertical-slice) Runtime vertical slice | FP32 conformance and runtime ledger. Three typed compute passes, RR-independent existing guides, real surface-valid mask, two transient hidden buffers, one output, generation lifetime and classical fallback. | `AC-NG-05/07`, `FM-NG-03/05`, `CHK-NG-02/03`: actual output and bounded invalid-output publication, black-surface validity, RR-off, missing/corrupt/unsupported/allocation/compile/all-queue retirement/shutdown controls. Delete replaced inference/upload/state paths; no copied scene, second model owner or generic runtime. |
| [`NG-S5`](#ng-s5---measured-kernel-optimization) Bounded layout profile and measured optimization | Correct runtime. Complete the required one-alternate-layout native probe and join S2 batch/S3 precision observations under [profile controls](Semantics.md#required-bounded-profile-controls). A further production precision/packing/dispatch/fusion/vector change needs a measured limiter. | `AC-NG-04/06`, `CHK-NG-02/03`: all three axes have actual scoped observations; adopted changes also meet whole-model/frame, numerical/quality/memory and both-API gates. Keep the correct FP32 route; delete rejected probes/variants/dependencies now. Neither three product variants nor a speed win is mandatory. |
| [`NG-S6`](#ng-s6---evaluation-delivery-and-adoption) Evaluation and adoption | Execute the first unfinished owner slice below: NG-S6a local held-out/metric/failure/package qualification, then assemble the three cases and run the actual portfolio exercise, then NG-S6b adoption closure. | S6a supplies the technical input to assembly with the AC-NG-08 rehearsal visibly open. S6b closes it from relevant actual observations; only then is AC-NG-01..09 complete. All quality, uncertainty, failure and cleanup obligations remain required. |

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
Execute NG-S2 only after accepted data. Follow ratified Semantics.md for fixed CNN/loss/halo, seeds/environment/repeat, tiny-batch/gradient, noisy/classical and validation-only ablations. Complete the short batch 8/16 profile on matched examples, reporting examples/s, ms/update and actual memory separately from inference; do not require two full training runs. Keep San Miguel sealed. Handoff AC-NG-03/CHK-NG-01, recipe identity, failures and cleanup. Stop on leakage, unfrozen rules or non-finite state; no inference integration.
```

</details>

### NG-S3 - Export And FP32 Conformance

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md#ng-s3---export-and-fp32-conformance and apply its Stage Contract, including the Stage Source-Style Gate.
Execute NG-S3 only after accepted semantics. Deliver the fixed OIHW/bias export and NHWC16 FP32 operators through existing ShaderSystem with exact padding/residual/decode. Prove float64/PyTorch/paired-GPU conformance and seeded layout/edge/channel/finite faults. Add only the frozen FP16 storage-round-trip diagnostic through the same operator; report bytes/drift, not native half-speed. Delete prototype formats and replaced conversions; keep one current artifact. Handoff AC-NG-04/CHK-NG-02. Stop on mismatch, guessed semantics or unowned lifetime; no vendor optimization.
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
Execute NG-S5 only after correct runtime. Complete Semantics.md's one-alternate-layout native control and join the S2 batch/S3 precision results; unrun axes remain unproved. Predict operations/traffic and retain actual operator/API scope, numerics, time and workspace. A production optimization additionally needs a measured limiter and whole-model/frame/quality/both-API proof. Keep the correct FP32 route and delete rejected prototypes/variants now. Handoff AC-NG-04/06 and original controls; no autotuner, mandatory SDK, claimed half-speed from storage-only evidence or post-result threshold change.
```

</details>

### NG-S6 - Evaluation, Delivery And Adoption

| Owner slice | Start and bounded work | Exit / pending proof |
| --- | --- | --- |
| `NG-S6a` Local evaluation and package | Supported NG-S5 candidate and frozen metric matrix. Use sealed San Miguel only for final testing; Bistro remains validation. Run raw/log/FLIP/temporal/value/cost/failure checks and the local package/hash/license/source joins. | All local AC-NG-01..09 obligations pass, including local 08 package checks. Its actual rehearsal result remains open. Hand the qualified candidate to portfolio stage 2 without waiting for its own later adoption. Remove probes and verify prior cleanup. |
| `NG-S6b` Documented reproduction closure | S6a and cleared assembly exist; use the actual portfolio stage 3 exercise for the supported neural result. Bind model/kernel/package/input identity and record environment, interventions, failure and output. | Close AC-NG-08/CHK-NG-04's remaining rehearsal cell only for relevant actual results. All criteria must pass for final neural acceptance; missing adoption remains blocked while S6a technical proof stays valid. No extra participant or result store. |

The [portfolio plan](../GraphicsPortfolio/ImplementationPlan.md#3-reproduce-review-and-revise) owns the real exercise and also covers the required study/analysis result. Reuse the same cleared package/participant where applicable; a successful unrelated launch cannot qualify neural reproduction. Neither final study adoption nor final neural adoption is an assembly prerequisite.

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md#ng-s6---evaluation-delivery-and-adoption and apply its Stage Contract, including the Stage Source-Style Gate.
Execute only the first unfinished NG-S6 slice. For S6a, qualify the supported candidate against every frozen raw/log/FLIP/temporal/value/cost and failure rule, San Miguel sealed-test identity, package/license/hash/native joins and local AC-NG-08 checks. Hand this technical packet to portfolio stage 2 with its rehearsal cell open. For S6b, consume the relevant actual portfolio stage 3 neural observations and close the remaining AC-NG-08/CHK-NG-04 cell; final adoption is not an entry prerequisite. Record exact proof, interventions, missing cells and cleanup in existing reports. Stop on failed value, uncertainty, model mismatch, unsafe failure or absent reproduction; no test-driven selection, invented result or extra platform.
```

</details>

Every production prompt inherits this non-negotiable exit: one identity and owner per data/runtime boundary; accepted reference and frozen numerical rules; no held-out leakage; correct FP32 route before optimizations; actual GPU/native results distinguished from source; complete controlled failures; no unledgered hooks or duplicate mechanisms. Quote each clause with retained proof or `BLOCKED` at handoff.

## Reference And Ownership

<details>
<summary>Exact owners, source snapshots and related contracts</summary>

**Authority:** [Discovery](Discovery.md) freezes the slice; [Training And Evaluation](NeuralGraphicsDesign.md#neural-graphics-training-and-evaluation), [Model To Kernel And Runtime Inference](NeuralGraphicsDesign.md#model-to-kernel-and-runtime-inference) and [Acceptance](NeuralGraphicsDesign.md#neural-graphics-acceptance) own contracts. [Roadmap](../../../Strategy/Roadmap.md) owns admission. No stage has candidate acceptance here.

This plan delivers one owned diffuse-indirect denoiser, with a useful classical fallback and both Windows graphics APIs. It deliberately has no calendar targets or time estimates. Shared engine repairs need an existing consumer and independently useful proof; do not hide a diagnostics/ML framework in this feature.

</details>
