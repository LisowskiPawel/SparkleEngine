# Principal Graphics Feature Delivery Catalog

[Docs](../README.md) / [Up](README.md)

**Status:** strategic deliverable targets and owner routing; no implementation authorization or acceptance results

**Scope:** turn the [persona](EngineerPersona.md), [requirements](Requirements.md) and [role study](Research/GraphicsEngineerProfiles.md#principal-graphics-roles-and-portfolio-implications) into measurable, inspectable outputs. [Roadmap](Roadmap.md) owns order; feature dossiers own design and thresholds; Acceptance owns results.

This is a gate-based plan. There are no delivery dates, hour estimates or calendar commitments. `PGD-*` are local strategic output IDs, not new runtime features or replacements for `PGE`, `REL`, `MAP`, `WL`, `CASE` or `FCR` identities.

**Choose the next piece at [Roadmap current/next](Roadmap.md#where-we-are-and-what-is-next).** This catalog defines what the outputs demonstrate; admission/order comes from that one driver. Detailed plans and fillable contribution/transfer cards are routed there. This page does not maintain another active queue or results tracker.

**Owner-directed release refinement:** [Required rendering closure](../Acceptance/FirstRelease.md#required-rendering-closure) makes reference reliability, realtime >=30 FPS, complete SR/RR, paired-backend/frame-graph closure and principal-level whole-frame review mandatory release outcomes. Existing PGD/FCR owners remain authoritative; no new implementation or acceptance is inferred.

## Read This Page

- **Choose an output:** [Deliverables](#deliverables-and-owning-plans); use Roadmap for their order.
- **Prepare a case:** [Three headline cases](#three-headline-cases) and the [measurement card](#measurement-card-required-before-results).
- **Study an example:** [Research-informed exercises](#research-informed-delivery-exercises).

## Deliverables And Owning Plans

### `PGD-01` Trustworthy product and reproduction

Exact approved source/product bytes, cleared rights, package manifest, clean consumer/source-adopter transcripts, negative failures and independent approval. Example: package inventory joining file hash, origin, license and import dependency.

**Evidence boundary:** Build/source paths exist; no conjunctive release acceptance is inferred.

**Delivery route:** [First Release stage briefs](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md), `REL-00..11`; [Build/Packaging](../Architecture/Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md).

**Requirements:** `PGE-01`, `07`, `13..15`

### `PGD-02` Content-to-correct-pixel and bounded reference

One content lineage case with exact import losses, material/transport domain, analytic/minimal/independent checks, raw reference uncertainty, frozen cameras and failure gallery. Example: a pixel traced from source material through cooked inputs and shader ABI to a finite linear-radiance output.

**Evidence boundary:** Tracer source stages delivered; recovery, estimator/oracle and final adoption evidence remain open.

**Delivery route:** Existing authorized recovery: [Reference Path Tracer plan](../Architecture/Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ImplementationPlan.md); release adoption `PTD-02/03`, `REL-04/05`; subsequent `M1/M2`, `WL-01..03` through the [Content To Pixel plan](../Architecture/Modules/Projects/Showcase/ContentToPixel/ImplementationPlan.md).

**Requirements:** `PGE-02`, `06..09`, `13`

### `PGD-03` Measured paired-API classical rendering

Matched Windows D3D12/Vulkan quality/cost cases on named hardware; one actual study in each [CPU preparation, GPU shader and resource-lifetime protocol](../Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/StudyProtocols.md), including a rejected/inconclusive alternative. Join analytical work/traffic/critical-path/lifetime predictions to native observations, per-run distributions and whole-frame effects. Native instruction/counter access limits stay explicit.

**Evidence boundary:** D3D12/Vulkan and rendering paths exist; complete workload evidence is unclaimed.

**Delivery route:** After release and correct references: technical `M3`, `WL-04`; final reproduction closure at `M6/WS-S4`; [Workload Studies](../Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md).

**Requirements:** `PGE-02`, `05`, `08..10`

### `PGD-04` Difficult graphics incident and useful capture workflow

One reproducible incident: symptom, earliest bad product, competing hypotheses, counterexample, minimal fix/deletion, regression/failure evidence and recovery. Capture artifacts identify the actual request, scene, host interval and tool tuple.

**Evidence boundary:** Provider paths and known failures are documented; source presence is not aggregate provider acceptance.

**Delivery route:** [External Capture plan](../Architecture/CrossModule/PerformanceDiagnostics/ExternalCapture/ImplementationPlan.md) for its remaining checks; [Workload Studies](../Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md) for the [incident case](../Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/IncidentAnalysis/ImplementationPlan.md).

**Requirements:** `PGE-01`, `05..07`, `09..10`, `13`

### `PGD-05` Useful Python analysis consumer

One CLI consumes immutable workload records, rejects mismatched/partial data, emits inspectable per-run comparisons and plots. Two frozen-input runs reproduce the output identity within the declared numeric policy; no duplicate sampler or exporter.

**Evidence boundary:** Existing narrow scripts do not establish a supported workload analyzer.

**Delivery route:** After `REL-11`, frozen producer input and a real comparison consumer: [Workload Studies plan](../Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md); [Python dossier](../Architecture/Modules/Tools/PythonAutomationAndAnalysis.md), `PY-03`.

**Requirements:** `PGE-05`, `07..08`, `13`

### `PGD-06` Owned data, neural model and training

The personally owned model slice remains one diffuse-indirect denoiser, alongside separately attributed NRC and NTC integrations in M5R. First qualify its component target inside the existing Reference owner, then licensed scene/sequence splits, sealed San Miguel test, fixed eleven-channel residual CNN, reproducible recipe, overfit/gradient diagnostics and guide/residual ablations. [Semantics](../Architecture/CrossModule/NeuralGraphics/Semantics.md) proposes exact math and gates for NG-D0 ratification. Beauty is not a label; a seed is not a cross-platform reproducibility guarantee.

**Evidence boundary:** Owned dataset/training/model family remains a target, distinct from vendor inference.

**Delivery route:** `REL-11` and `M3`; [Neural discovery](../Architecture/CrossModule/NeuralGraphics/Discovery.md), then [plan](../Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md) Stages 1–2; `M4`, `WL-05`.

**Requirements:** `PGE-03`, `07..08`, `11..12`

### `PGD-07` Model-to-shader product path

The [fixed operators/layout and existing-owner runtime](../Architecture/CrossModule/NeuralGraphics/ExecutionArchitecture.md), independently checked against float64, PyTorch and FP32 shaders on both APIs; RR-independent guides, typed scheduling, all-queue retirement, capability/fallback and package proof. Test only measured precision/layout/fusion hypotheses after correctness, against the ratified whole-product quality/cost/workspace gates. Preserve the required FP32 baseline.

**Evidence boundary:** Owned lowering/kernels/runtime absent in the existing target dossier; no general ML runtime is requested.

**Delivery route:** Accepted neural discovery and `PGD-06`; [Neural plan](../Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md) Stages 3–5; [Shader System](../Architecture/CrossModule/ShaderSystem/ShaderSystemDesign.md); `M5`, `WL-06`.

**Requirements:** `PGE-03..04`, `09..10`, `12`

### `PGD-08` Held-out evaluation and transferable explanation

The [three selected narratives and small package](../Architecture/CrossModule/GraphicsPortfolio/ReviewAndTransfer.md#the-three-headline-cases), quality/latency/memory frontier, failures, exact manifests, paper and talk/demo. The author or agent reproduces a headline result without private repair; a specialist challenges a meaningful assumption and the disposition records feedback and resulting changes.

**Evidence boundary:** No owned neural evaluation/adoption result is claimed.

**Delivery route:** `M6`, `WL-07/08`; [portfolio plan](../Architecture/CrossModule/GraphicsPortfolio/ImplementationPlan.md#graphics-portfolio-delivery-plan) assembles accepted [Neural](../Architecture/CrossModule/NeuralGraphics/NeuralGraphicsDesign.md#neural-graphics-acceptance), [Workload Studies](../Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/WorkloadStudiesDesign.md) and [Graphics Workloads](../Acceptance/GraphicsWorkloads.md) evidence.

**Requirements:** `PGE-01`, `03..05`, `08`, `11..13`, `15`

### `PGD-09` Observed direction, transfer and honest breadth

M7 requires a real second consumer context, technical direction/review record, teaching with an observable before/after task, and one feedback-driven post-adoption revision/retest. Reuse existing cases rather than build another engine or SDK. Upstream contribution remains conditional on a real issue and recipient. Linux implementation, AMD hardware acquisition and AMD profiler work are excluded.

**Evidence boundary:** Windows work does not prove another architecture, employment history or sustained external influence. Linux delivery is excluded.

**Delivery route:** Required [M7 / Technology Transfer KT-0..3](../Architecture/CrossModule/GraphicsPortfolio/TechnologyTransfer/ImplementationPlan.md), `AC-PORT-04..08` in [Review And Transfer](../Architecture/CrossModule/GraphicsPortfolio/ReviewAndTransfer.md); actual participant inputs in the [transfer card](../Architecture/CrossModule/GraphicsPortfolio/GraphicsPortfolioDesign.md#graphics-knowledge-transfer). Conditional [contribution card](../Architecture/CrossModule/GraphicsPortfolio/GraphicsPortfolioDesign.md#upstream-graphics-contribution); truthful [non-repository facts](Requirements.md#non-repository-requirements).

**Requirements:** `PGE-01`, `07`, `13..15`


Counts here define portfolio coverage, not statistical validity. The three studies and one incident may reuse the same candidate/workload when each answers a distinct question. A deleted negative experiment can be valuable evidence; it cannot stand in for an implemented neural shader path or a passed acceptance criterion.

## Research-Informed Delivery Exercises

These are concrete examples within the nine outputs above, not additional features or mandatory vendor integrations. [Public engineer profiles](Research/GraphicsEngineerProfiles.md) establish attribution; [reference cards](Research/RenderingReferenceExamples.md) record exact code/publication locations. Select the local problem and falsifier before implementation. Numeric tolerances remain in the owning discovery/workload.

| Output / example question | Deliverable a reviewer can inspect | Source precedent and actual owner |
| --- | --- | --- |
| `PGD-02`: Why is this reference pixel trustworthy? | One raw-linear pixel/product lineage, bounded transport/material table, estimator derivation, independent fixture and unconverged/invalid-sample rejection. Preserve the accepted tracer contract and finish its remaining proof. | `NVR-04/05/15/17`; [Reference Path Tracer plan](../Architecture/Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ImplementationPlan.md). |
| `PGD-02/03`: When is reservoir reuse valid? | Proposal/target/normalization and history/shift note; matched direct/indirect baseline; disocclusion/light-edit counterexample and quality/memory/cost comparison. | `NVR-02/03/17`; [Direct discovery](../Architecture/Modules/Engine/Renderer/Features/Lighting/DirectLighting/Discovery.md), [Indirect discovery](../Architecture/Modules/Engine/Renderer/Features/Lighting/IndirectLighting/Discovery.md). |
| `PGD-03`: Are CPU recording and binding work limiting the frame? | One declared study with thread/queue timeline, command-order/resource-retirement explanation, matched scene/settings and before/after per-run evidence. Retain the current production parallel topology. | `NVR-01/07/16`; [Workload Studies](../Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md). The old CAD sample is deprecated; use its successor for current comparison. |
| `PGD-03/04`: Is shader live state, traffic or code size the actual limiter? | One exact source/bytecode/native join; hypothesis and alternative; counter/ISA evidence where available; quality-equivalent whole-frame result. A failure or inconclusive outcome is useful. | `NVR-18/19`; [Shader System plan](../Architecture/CrossModule/ShaderSystem/ImplementationPlan.md), [Workload Studies](../Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md). SER is optional. |
| `PGD-05`: Can another engineer compare the evidence correctly? | One bounded offline CLI, stable original-input hashes, run-level tables/plots, raw/display-domain distinction and mismatch/partial-input controls. | `NVR-12/14`; [Workload Studies](../Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md), existing Python owner. No embedded viewer or general analyzer platform. |
| `PGD-06/07`: Does a small owned model become correct, useful shader work? | Dataset/model/operator cards, held-out split, FP32 shader conformance, one layout/fusion/precision ablation, resource-retirement/fallback and quality/latency/memory frontier. | `NVR-03/10/11/13`; [Neural discovery](../Architecture/CrossModule/NeuralGraphics/Discovery.md) and [plan](../Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md). Do not add NeRF or OptiX. |
| `PGD-08/09`: Can knowledge and maintenance transfer? | M6: documented author/agent reproduction and separate technical challenge. M7: second usage context, before/after explanation task and justified revision/retest, with a finite follow-up/retirement boundary. | `NVR-17/20`; [Review And Transfer](../Architecture/CrossModule/GraphicsPortfolio/ReviewAndTransfer.md), existing CASE/WL owners. No support service or invented external result. |
| Conditional `PGD-03/09`: Would a GPU-driven geometry or memory change solve a measured problem? | First produce a visibility/draw-granularity or residency-pressure diagnosis. Admit a bounded feature only if the current path cannot meet a frozen consumer need. | `NVR-08/09/16`; [Visibility](../Architecture/Modules/Engine/Renderer/Features/GeometryAndResources/VisibilityAndDrawPreparation.md), [Residency](../Architecture/Modules/Engine/Renderer/Features/GeometryAndResources/MeshAndTextureResidency.md). Meshlets/DGC/new vendor SDKs remain study-only until separately admitted. |

Every selected exercise records the source card, exact assumption, local differences, rights route, falsifier and result owner using the [reference-use record](Research/RenderingReferenceExamples.md#delivery-reference-use-record). An algorithm replacement requires its existing feature gate; a documentation precedent cannot reopen or silently change an accepted scope.

## Selected Rendering Depth

These are required later portfolio cuts within the existing PGD outputs, selected by the owner on 2026-10-25. NVIDIA's [principal role](https://nvidia.wd5.myworkdayjobs.com/nvidiaexternalcareersite/job/Principal-Developer-Technology-Engineer_JR2013547) calls for advanced path-traced/neural rendering, mathematical and CPU/GPU depth, optimization, demos and communication; it does not prescribe these SDKs or certify a hiring level. The named algorithms below are local delivery choices. [M5R](Roadmap.md#required-renderer-depth-wave) owns their order.

| Area | Required technical depth and evidence | Exact implementation/proof owner |
| --- | --- | --- |
| Path tracing | Rendering-equation/measure ledger, NEE/BSDF/MIS/PDF and roulette accounting; analytic/independent minimal fixtures, raw-linear uncertainty/convergence, invalid sample and domain rejection. Cached/denoised images never become the raw oracle. | [Reference Stages 3..10](../Architecture/Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ImplementationPlan.md), accepted TransportAndEstimator and original AC-RPT checks; PGD-02 |
| Direct lighting | Exhaustive small-light truth, proposal/target/normalization derivation, temporal/spatial reservoir reuse, visibility/bias/support controls; selected mixed analytic/emissive/environment many-light case with equal-time and matched-quality results. | [DIR-1..8](../Architecture/Modules/Engine/Renderer/Features/Lighting/DirectLighting/ImplementationPlan.md), AC-DIR and source/native/raw artifacts; PGD-02/03 |
| Indirect lighting | Explicit path state, technique/PDF/MIS, shift/reconnection Jacobians and support, GRIS normalization/correlation assumptions; multi-bounce diffuse and rough specular, dynamic occlusion and emitter/environment terminals. | [IND-1..6/8](../Architecture/Modules/Engine/Renderer/Features/Lighting/IndirectLighting/ImplementationPlan.md), AC-IND; PGD-02/03 |
| Materials/shading | Authored-to-pixel lineage, base scattering and subsurface limits, anisotropic GGX plus one coat; energy/PDF/sample/reciprocity controls and tangent/alpha/raster/ray/API evidence. | [MAT-S0..5](../Architecture/Modules/Engine/Renderer/Features/GeometryAndResources/ImplementationPlan.md), AC-MAT-01..06; PGD-02/03 |
| Upscaling/denoising | Complete SR/RR SDK/input/output proof; one RELAX diffuse/specular temporal path, exact radiance/hit-distance/guide semantics, motion/disocclusion/reset/failure, held-out spatial/temporal quality and total cost. | [REC-S0..5](../Architecture/Modules/Engine/Renderer/Features/PostProcessing/ReconstructionAndGeneration/ImplementationPlan.md), AC-IRU/REC; separate owned NG chain |
| Volumetric lighting | Coefficients/units/phase and radiative transfer, analytic slab/height and independent varying-density oracle; bounded local froxels/all analytic lights, shadows, temporal/composition and quality/time/memory proof. | [VOL-S0..5](../Architecture/Modules/Engine/Renderer/Features/Lighting/VolumetricLighting/ImplementationPlan.md), applicable AC-VOL Q0/Q1/Q2; no clouds/atmosphere/VDB/reuse expansion |
| Radiance caching | SHaRC first: grid/update/resolve/query, contribution accounting, cold start/aging/capacity/leakage/response and useful total cost. Only after full SHaRC delivery: NRC online update/training/query/resolve, bias/adaptation and common-domain comparison. | [SHaRC](../Architecture/Modules/Engine/Renderer/Features/Lighting/RadianceCaching/Sharc/ImplementationPlan.md) then [NRC](../Architecture/Modules/Engine/Renderer/Features/Lighting/RadianceCaching/Nrc/ImplementationPlan.md), AC-SHARC/NRC-01..05 |
| Neural textures | Real offline compression and immutable correlated material sets, on-sample decode, channel/color/normal semantics, filtering/mips/LOD and complete resident/package bytes versus shader/frame cost. | [NTC-S0..5](../Architecture/CrossModule/NeuralGraphics/TextureCompression/ImplementationPlan.md), AC-NTC-01..05; PGD-03/07 |
| Owned neural work | Qualified component labels, scene-separated data, fixed CNN/training, float64/PyTorch/FP32 shader conformance, measured optimization, held-out/generalization and delivery. NRC/NTC/DLSS integration cannot substitute for personal model/kernel ownership. | [NG-S0..6](../Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md), AC-NG; PGD-06..08 |
| PTLAS | Sparse operations, stable identity, migration/global policy, legal BLAS/SBT update and partition translation; classic parity, native/capacity/lifetime/refusal controls and build-plus-trace/full-frame benefit. | [PTLAS-S0..5](../Architecture/Modules/Engine/Renderer/Features/RayTracing/PartitionedTlas/ImplementationPlan.md), AC-PTLAS-01..05; PGD-03 |

Each row needs a derivation/contract, independent falsifier, actual native/product result, dynamics/failure controls, quality/time/memory frontier and reviewable original artifacts. A pre-existing implementation, SDK import, provider enum or attractive screenshot alone closes none of these obligations. Reuse measurements only for their actual domain and candidate; new selected features receive their subject folders under the existing workload artifact root, not new first-release families or readiness credit.

## Three Headline Cases

| Case | What the reviewer should learn | Concrete delivery shape |
| --- | --- | --- |
| Content to correct pixel | How assets, scattering/material profiles, transport, output domain and reference uncertainty join. | Original lineage/oracle case plus material and bounded volume derivations, raw failure controls and reproduction links. |
| Classical graphics and causal optimization | Why the frame is limited and when reuse, denoising, SHaRC or PTLAS helps on the advertised APIs. | Original three studies/incident plus selected rendering-depth comparisons, native artifacts, full cost and rejected/inconclusive evidence. |
| Owned neural model to shader | How personally owned model/kernel work differs from NRC, NTC and reconstruction SDK integration, and whether each is useful. | Original data/conformance/generalization case plus SHaRC-to-NRC and BCn-to-NTC frontiers; separate attribution, domains, artifacts and failure controls. |

These are narrative groupings over [canonical `CASE-*` and `WL-*` workloads](../Acceptance/GraphicsWorkloads.md), not replacement acceptance identities. A report states personal ownership explicitly and credits external algorithms, assets, integrations and reviewers.

## Measurement Card Required Before Results

| Field | Required value or example |
| --- | --- |
| Question and hypothesis | “Does reducing descriptor preparation work improve the selected workload without changing the rendered output?” Name a proposed mechanism and an observation that would refute it. |
| Candidate and baseline | Commit/dirty state, cooked input and shader/model hashes, API/profile, driver/GPU/CPU, scene/camera and output settings. |
| Metric definition | Quantity, unit and denominator: per-run p95 GPU frame milliseconds; bytes of peak workspace; reference-domain image error; not unnamed “performance.” |
| Decision rule | Absolute and relative practical-effect bands, uncertainty method, quality tolerance and failure policy frozen by the owning workload/discovery before candidate results. An overlapping interval is inconclusive. |
| Samples and observation | Reuse [Graphics Workloads](../Acceptance/GraphicsWorkloads.md#performance-contract) and [Diagnostics comparison](../Architecture/CrossModule/PerformanceDiagnostics/Acceptance.md#comparison-and-regression-contract), including equal-N or justified tail treatment. Instrumented captures and optimized timing are separate runs. |
| Output join | Each row joins run/frame/scene/setting identity to original artifact and explicitly reports missing metrics; a merged chart must not conceal run-level tails or exclusions. |
| Stop and failure | Stale input, mismatched symbols, unconverged reference, changed settings, unsupported counter/tool or failed negative control prevents the claimed verdict. |

For example, an optimization table contains `run ID`, `API`, `baseline p95`, `candidate p95`, `absolute delta`, `relative delta`, `uncertainty`, `quality check` and `decision`. This describes required fields; no numbers or wins are fabricated here. Frame-time deltas are candidate minus baseline; percentage change divides that delta by the baseline, with units and sign stated.

## Admission And Remaining-Work Rules

Deliver these outputs through shared production paths and evidence. `PGD-*` does not prescribe a subsystem per output, and `PGE-*` does not prescribe a feature per skill. Reuse one candidate, artifact, incident or case where it answers each distinct question; do not duplicate measurements or build a tool solely to fill a report field. Narrowing or deleting unneeded engine scope is a valid `NS-SIMPLIFY` result when the selected outputs and their proof survive.

The [Roadmap WIP/admission rule](Roadmap.md#dependency-and-work-in-progress-rule) selects work. An output here supplies the consumer need; its owning dossier freezes the defect-detecting check. A fillable card names known facts, unresolved decisions, owner, exit evidence and first permitted action. Reuse existing owners instead of creating framework placeholders for role keywords.

Completed implementation instructions are removed from their plans; enduring semantics, source evidence, valuable captures and decisions remain in their owners. Partial stages retain only their unfinished obligations and links to valid proof. [Feature Delivery Package](../Engineering/Workflow/Templates/FeatureDeliveryPackage.md#measurable-delivery-card) provides the reusable planning card and stage structure.

Unrun or missing hardware/software evidence stays unrun; excluded scope is explicitly excluded. Neither is accepted or “unsupported” without proof. Public claims include only closed evidence gates and can be narrower than this professional target.
