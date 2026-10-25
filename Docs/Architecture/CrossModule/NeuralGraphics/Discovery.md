# Owned Neural Denoising Discovery

[Docs](../../../README.md) / [Up](NeuralGraphicsDesign.md) / [Implementation](ImplementationPlan.md)

**Status:** concrete design proposal awaiting qualification/ratification; `NG-D0` is open and no model, producer or runtime implementation is authorized merely by this page

**Admission:** after `REL-11` and the measured classical baseline `M3` in the [roadmap](../../../Strategy/Roadmap.md). Documentation research may proceed earlier. This is the selected `PGD-06/07` route in the [delivery catalog](../../../Strategy/FeatureDeliveryCatalog.md), not a generic ML platform.

**Product candidate:** the [eleven-channel three-layer residual CNN](Semantics.md), qualified diffuse-indirect label and [existing-owner runtime](ExecutionArchitecture.md). These pages now propose the exact signal, topology, layout, recipe and conformance/value/cost targets. This gate ratifies or explicitly revises them before candidate training; it does not infer a result from documentation. The [dossier acceptance](NeuralGraphicsDesign.md#neural-graphics-acceptance) retains binary proof requirements.

## Known Facts And Boundaries

The [current dossier](NeuralGraphicsDesign.md) distinguishes existing vendor reconstruction from absent owned data/model/lowering/runtime paths. The [Reference Path Tracer recovery plan](../../Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ImplementationPlan.md) records delivered source and unclosed oracle evidence; raw reference generation cannot be assumed reliable for training. [Graphics Workloads](../../../Acceptance/GraphicsWorkloads.md) already owns scenes, sample/benchmark policy and held-out generalization. [Shader System](../ShaderSystem/ShaderSystemDesign.md) owns cook, reflection, ABI and published shader identity. Reuse these authorities.

The initial matrix targets available Windows/NVIDIA hardware with both D3D12 and Vulkan. Record the actual installed environment during discovery; this page does not certify it. Missing tuple evidence is blocked. AMD hardware and profiler work are excluded from this selected program. Optional cooperative-vector or vendor-specific kernels need their own pinned capability cell after the generic shader baseline, not a new mandatory dependency.

## Decisions Required For `NG-D0`

| Cell | Owner and decision to close | Artifact and defect-detecting exit |
| --- | --- | --- |
| `NG-D01` User output and comparison | Ratify the Semantics diffuse-indirect product, spatial residual network, pre-composite edge and fixed 7x7 cross-bilateral/raw baseline; qualify supported content and real-time sampling/reuse policy. The inspected source has no separate spatial diffuse denoiser to assume. | Product/feature matrix, baseline equation/constants/CPU-GPU check card and one named equivalent output. NG-S1b qualifies the baseline before training; reject transport/history/resolution/component mismatch. |
| `NG-D02` Reference and radiometry | Ratify the component estimator/event partition, same-pixel/guide identity, no-demodulation log transform and qualified range in Semantics. Freeze the exact Reference producer delta/check card and uncertainty; current beauty is not the label. | Independent component derivation and isolated leakage/PDF/prefix controls; qualify the producer in NG-S1 before publishing data. No second tracer, beauty relabel or noisy subtraction. |
| `NG-D03` Rights and split identity | Qualify exact Sponza/Cornell train, Bistro validation and sealed San Miguel test manifests/rights; freeze camera/pose/seed sequences and retention. | Scene/sequence split and leakage check; no adjacent-patch/frame split or test-driven recipe selection. Missing rights/data remains blocked, not guessed. |
| `NG-D04` Model and training | Ratify the fixed equations/halo/loss/Adam recipe and repeat rule, plus the bounded batch 8/16 profile control; pin actual environment, population and per-example/update units. | Freeze overfit/gradient/alignment and separate training/inference profile cards before execution. No two full training campaigns merely for a batch comparison or held-out architecture search. |
| `NG-D05` Export and conformance | Ratify OIHW/NHWC16, replicate padding, bias/residual/decode, proposed FP32 rule and checked payload/compiler policy. Freeze the FP16 storage-round-trip and one alternate-layout probe, with exact numerical and measurement scope. | Independent float64/PyTorch/GPU layer/full-model and seeded layout/edge/channel/finite controls. Storage quantization is not native half-arithmetic speed; a file alone proves neither. |
| `NG-D06` Runtime and lifecycle | Ratify Execution Architecture's Reference/data/Assets/Renderer hooks, three neural passes, shared classical helper/dispatch, RR-independent guides and proposed bounded GPU fault counts/publication. Freeze pending/mixed/failure identity, capacity, generation and all-queue retirement. | One request-to-cook/bind/dispatch/output/retirement trace and failure card, including no-surface zero-fill, black surfaces and model-fault fallback cost. No guessed guides, copied scene, native model API or second uploader/collector. |
| `NG-D07` Quality and cost | Ratify the proposed frontier, cost/population and motion/edge/bias rules. Freeze every workload-required metric, including FLIP implementation/variant, input domain/transform/exposure and numeric decision rule. NG-S1b qualifies the new baseline before training; it need not exist before its admission. | Per-sequence raw/log/FLIP/temporal/shift, timing/peak-overlap and bounded profile cards, with inherited uncertainty/effect rules. Missing metric qualification keeps this cell open; no post-result weakening. |
| `NG-D08` Build and delivery | Build/Assets owners: dependency lock, notices, model/kernel manifests, supported API/profile matrix, optional payload erasure and clean failure. | Configuration/source/link/package membership card; missing/corrupt/incompatible artifact and capability-negative journeys. No training dependencies in game products. |
| `NG-D09` Scope and adoption | Feature owner and independent reviewer: architecture budget, one model, one consumer, personal/external ownership, baseline retained and non-author workflow. | Review signed to one dossier/source revision; `AC-NG-01..09` and `FM-NG-01..06` mapped to predeclared check cards; adoptable first-use route. |

Every cell records decision, source revision, evidence, unresolved owner and invalidation trigger. NG-D0 freezes derivations, decisions and defect-detecting cards; it does not claim the component/baseline checks already ran. NG-S1a/b implement and qualify those necessary producers before dataset publication/training. This prevents a producer-before-admission cycle. Unknown numeric rules or API/lifetime choices still prevent NG-D0 closure; the plan cannot choose them while coding. Prefer reducing topology/input scope to a general runtime. No dataset capture needs unbounded scene/material APIs.

## Primary Precedent

The [role and technical study](../../../Strategy/Research/GraphicsEngineerProfiles.md#technical-evidence-and-transfer-limits) records official PyTorch reproducibility, RTX neural shading, shader capability and Monte Carlo sources. These are precedent. When actually selecting a library, paper/model or compiler route, pin the original revision/specification, reproduce its relevant behavior and record rights here. No SDK, architecture or operator is adopted solely by mentioning that study.

### Reference Use In Remaining Discovery

The [source cards](../../../Strategy/Research/RenderingReferenceExamples.md) record immutable study locations and transfer limits. Select the relevant assumption and falsifier in each decision's artifact; these references do not select the model or close a cell.

| Discovery cells | Reference and what to inspect | Why / required local decision |
| --- | --- | --- |
| `NG-D01/02/06` | `NVR-03/04/15`: NRD guide encoding and tracer output decomposition. | Freeze the actual diffuse-indirect, raw-radiance, guide and remodulation contract. Wrong units/space/history must be detectable; neither an NRD integration nor a shared tracer qualifies as an independent neural oracle. |
| `NG-D03/04/08` | `NVR-11`: experiment inputs, snapshots and evaluation; existing reproducibility sources. | Pin the owned split/recipe/environment and exported model. Snapshot identity is not repeatable training; no NeRF product is admitted. |
| `NG-D05/07` | `NVR-10`: fused MLP activation storage/layout and hardware bounds; `NVR-18`: executed source correlation. | Derive the selected fixed operators, prove FP32 equivalence, then freeze one layout/fusion/precision hypothesis with quality and whole-frame checks. CUDA kernels are precedent, not portable shader code. |
| `NG-D06/09` | `NVR-01/13/20`: resource retirement, typed host/device examples and teaching exercises. | Reuse existing RHI/ShaderSystem identity/lifetime; one actual consumer and non-author explanation. No second RHI, OptiX adapter or training runtime. |

Use the [reference-use record](../../../Strategy/Research/RenderingReferenceExamples.md#delivery-reference-use-record) within `NG-D0`; record study-only versus actual reuse and rights. Accepted numeric/scope decisions remain authoritative until deliberately reopened.

## Ready-To-Use Discovery Prompt

```text
Execute only NG-D0 in Docs/Architecture/CrossModule/NeuralGraphics/Discovery.md. Research and planning only; do not change production code or train a final candidate. Apply AGENTS.md and the Feature Delivery Documentation Package.

Read Semantics.md, ExecutionArchitecture.md and Research.md as the concrete proposal to ratify. Inspect current NeuralGraphics owners, ReferencePathTracer recovery/oracle status, GraphicsWorkloads, Renderer diffuse-indirect products and guides, ShaderSystem cook/reflection/binding, Assets publication, runtime selection/fallback, package membership and installed GPU/toolchain. Freeze the component producer's derivation/check card now; NG-S1 qualifies the producer before publishing data. Preserve REL-11/M3 production admission.

Use the selected NVR source cards from this discovery's reference-use table. Record exact source/file/assumption, local differences, rights and a falsifying check; do not import the source framework or substitute vendor integration for owned model work.

Close NG-D01..09 with exact sources, one fixed product/topology/operator inventory, tensor/radiometric/reference domain, licensed scene-separated splits, training reproducibility policy, numeric conformance/quality/cost thresholds, existing consumers, generation/retirement/failure protocol and hook/copy/deletion ledger. Record unresolved owners; no speculative generic APIs or dependency download. Revalidate primary sources before choosing a compiler/library/model route.

NON-NEGOTIABLE: classical and neural outputs compare the same product; references have accepted domain and uncertainty; San Miguel test identity is held out; numeric thresholds and repeat policy precede results; FP32 shader conformance precedes precision/vendor optimization; runtime policy has one feature owner. Quote each clause with evidence or BLOCKED. Stop on unproved oracle, rights, lifetime or unsupported installed tuple.

Update the existing training/runtime/acceptance contracts and Plan only from frozen decisions. Validate links/anchors/UTF-8/whitespace, traceability, architecture budget and git diff --check. Remove disposable probes. Handoff NG-D0 decisions, unresolved owners, accepted revision and first permitted stage; no readiness or executable acceptance claim.
```
