# Owned Neural Denoiser Execution Architecture

[Docs](../../../README.md) / [Up](NeuralGraphicsDesign.md) / [Semantics](Semantics.md) / [Delivery](ImplementationPlan.md)

**Status:** target proposal for NG-D0; no dataset, model, shader dispatch or performance result is implemented by this document

**Responsibility:** join the qualified component producer, training/export, immutable model and existing Renderer/Assets/RHI owners without creating an ML platform.

## Current Source And Necessary Delta

Targeted 2026-10-25 read at `9bf955414bfe98fbb9062f3563a9654f42e64e1e`: `RestirIndirectResolve` binds indirect diffuse/specular textures and GBuffer/depth, and its shader stores weighted diffuse contributions. Reference publication exposes beauty mean/M2, not a qualified diffuse-indirect label. No separate spatial diffuse-denoising pass was found in the inspected lighting/pass routes; vendor Ray Reconstruction is a different whole-image path. Existing source does not prove these signals correct or numerically comparable.

The mandatory first delta is the [component measurement](Semantics.md#selected-product-and-reference-dependency), under the existing Reference owner. NG-S1 delivers/qualifies it before publishing a dataset; NG-S0 freezes its derivation and exact hooks first. Do not require a complete dataset before permitting its necessary producer. Reuse the same transport/sample identity and raw transaction, with a named component identity and independent mean/uncertainty. Adding an output must not change beauty sampling or silently relabel a prefix.

| Boundary | Owner and smallest contract |
| --- | --- |
| Reference component and guides | Existing Reference/scene/view owners; one measured component and typed raw-product hook, not an AOV registry or offline renderer. Requalify affected math/output checks. |
| Dataset/training | One Tools-owned builder/recipe consumes original records; PyTorch remains offline. Dataset/split/model cards and checkpoints are evidence, not runtime services. |
| Export/cook | Fixed tensor/bias order and checked byte/count manifest; existing Assets/ShaderSystem publication and dependency closure. No ONNX interpreter or arbitrary operator IR is needed for this topology. |
| Runtime model | Immutable current generation with logical model/semantic/weight/shader identity; existing upload, completion and retirement. No second asset cache or uploader. |
| Feature execution | One Renderer-private denoising owner, consuming existing raw diffuse/guide inputs and publishing one reconstructed diffuse product. Existing composition selects it once. |
| Classical comparison/fallback | The [fixed cross-bilateral equation](Semantics.md#bounded-classical-baseline), one shared GPU helper and ordinary baseline dispatch inside that same feature owner. NG-S1 qualifies it; final neural output uses the helper only on explicit model-result faults, not as an always-executed second image pipeline. |
| Native work | Existing RHI resources, bindings, dispatch and submission tokens. No neural enum, tensor object or model policy in RHI. |
| Selection and delivery | Extend the existing per-feature request/status and package manifest only where actually consumed; no global frame mode, Game settings screen, model browser or experiment dashboard. |

## Kernel And Memory Baseline

Start with three ordinary typed FP32 compute passes matching [Semantics](Semantics.md#fixed-first-network). The first reads existing textures directly; do not materialize another eleven-channel scene snapshot. Two transient NHWC16 buffers carry the hidden layers, then a distinct output resource carries reconstructed RGB. Each dispatch declares its real reads/writes and uses the active render extent, checked index/count arithmetic and partial-group bounds.

The 253.125 MiB hidden-buffer arithmetic at 1080p excludes output, weights, allocator alignment, upload overlap and the normal frame. Report all of them, including simultaneous old/new model generations. No persistent image/history state is needed for the spatial model. FrameGraph owns barriers/aliasing; model bytes retire after every queue using them completes. Never recycle on upload completion alone or add device-idle waits as lifetime policy.

The ordinary FP32 path is the correctness baseline. Measure one actual limiter before changing FP16 storage, vector packing, tiling, fusion or dispatch. Preserve halo semantics and operation identity. Cooperative-vector specialization is a separately qualified optional cell: pin actual compiler/SDK/extension/driver support and both API behavior before adoption. A faster kernel that changes the model or hides unsupported data is a different candidate, not an optimization pass.

Guides must be available independently of vendor Ray Reconstruction selection. Derive reflectance, normal, depth and hit validity through existing shared surface owners; do not require RR merely to obtain a neural input. In particular, current `IndirectDiffuse.a` indicates positive contribution, not surface validity: a valid black surface remains a valid sample. Missing/mismatched guides fail admission rather than introducing guessed data or another scene snapshot.

## Publication And Failures

Admission checks exact component/domain, extent, guide/model generations, fixed tensor counts/layout, finite supported inputs, cooked bytes/hash and device capability before scheduling. No partial model activates. Failed replacement preserves the last accepted generation under the existing owner; absence or unsupported input yields truthful classical fallback/unavailable state with requested/active identity.

Proposed invalid-output protocol: the final pass checks transformed output/range before decode, uses the one classical helper for a fault on otherwise valid inputs and writes bounded per-frame input/model-fault counts. Reset counters through existing FrameGraph mechanisms; bind them to exact frame/model/input generations and publish them through the existing bounded readback/status owner, without synchronous per-frame waits or a new collector. No-surface input has the explicit zero-fill/bypass rule in Semantics; it is not a model fault. A malformed raw input is unavailable, not repaired by the filter. The mixed/fallback result is not labeled as successful all-neural output, and supported value runs require zero in-domain model faults. NG-D06 must ratify the exact counter/publication hooks, capacity and observable pending/failure state on current owners; if safe attribution is unavailable, runtime stays blocked. All guard/fallback work counts in cost.

Exercise missing/corrupt/wrong-shape weights, wrong semantic/component, stale upload/frame, two Views, rapid replacement, allocation/compile/dispatch/device failure and shutdown with in-flight work. Preserve a real classical product; a clear/copy pass or requested Neural label is not fallback proof. Training libraries, source scenes and debug fixtures remain out of runtime packages; model/kernel licenses and exact bytes are verified.

## Implementation And Exit Route

NG-S0 ratifies semantics, component attribution, reference qualification, numerical/value/temporal budgets and exact source/build/hooks. NG-S1 qualifies the necessary raw component and deterministic data. NG-S2 proves training; NG-S3 proves exported operators/FP32; NG-S4 joins one real runtime consumer; NG-S5 tests a measured kernel hypothesis; NG-S6 closes quality/package/adoption. No earlier proof is manufactured by a later integration.

Every stage follows the existing complexity/hook/copy budget and removes its replaced component/export/upload/filter route in that slice. A negative experiment leaves original evidence and no dormant product variant. `AC-NG-01..09`, existing `FM-NG-*` and `CHK-NG-*` remain the acceptance route; this architecture creates no result authority.
