# Renderer Feature Research And Planning Coverage

[Docs](../../../../../README.md) / [Up](RendererFeatureGuide.md)

**Status:** navigation and documentation-depth decisions; not an implementation or documentation expansion backlog.

Use Roadmap to select the rendering problem, the owning dossier for semantics/proof, and its existing delivery stage for the missing delta. This page locates those owners. It does not require every feature to acquire research, discovery, semantics, UX and plan files, or require a new document before each code repair.

## Package Decision

| Actual need in selected work | Smallest maintained owner |
| --- | --- |
| Existing behavior needs verification or a local repair | Current dossier/AC and its release stage; no new package. |
| A correctness, scope, lifetime or UX choice is unresolved | Resolve it in the owning contract; split only an independently maintained decision. |
| Algorithmic equations or external precedent materially affect the choice | Reuse or extend the existing semantics/research owner with a falsifying check. |
| Capability is absent and unselected | Keep its negative boundary; no ready production stages or full-package authoring quota. |

High assurance is precision, independent oracles and honest failure/evidence, not seven files. Delete obsolete prompts and duplicate authority when changing the selected path. Unique dated proof remains with its evidence owner; historical package-writing exercises do not become recurring tasks.

## Twenty-Six-Family Coverage

The table identifies existing routes; its suggested depth changes apply only when the actual selected work creates that need. Source/readiness claims remain dated in their owning dossiers, not refreshed by this index.

| FCR | Feature owner | Current package and delivery authority | Class / next depth action |
| --- | --- | --- | --- |
| `FCR-REN-01` | whole-frame production | [Rendering A Sparkle Frame](../RenderingASparkleFrame.md), [Frame And Scene `FS-1`](../FirstRelease/FrameAndSceneImplementationPlan.md#fs-1--frame-admission-and-coordination) | `C`; add research only for a new scheduler/publication model |
| `FCR-REN-02` | scene/view/GPU-scene preparation | [Scene And View Preparation](SceneAndViewPreparation/SceneAndViewPreparationDesign.md), local [Acceptance](SceneAndViewPreparation/SceneAndViewPreparationDesign.md#scene-and-view-preparation-acceptance), `FS-2` | `C`; split semantics only if identity/deformation/publication rules materially change |
| `FCR-REN-03` | frame graph and scheduling | [Frame Graph And Scheduling](FrameExecution/FrameGraphAndScheduling.md), `FS-3` | `C`; discovery is required before queue/aliasing/scheduler redesign |
| `FCR-REN-04` | raster/GBuffer surfaces | [Geometry, Materials, And GBuffer](GeometryAndResources/GeometryMaterialsAndGBuffer.md), `GR-2` | `C`; use a semantic companion when the material/GBuffer contract changes |
| `FCR-REN-05` | ray-traced GBuffer | [Ray Tracing](RayTracing/RayTracingDesign.md), [Execution Architecture](RayTracing/ExecutionArchitecture.md), `GR-4` | `B`; reconcile the selected contract and add only independently missing decision/proof roles; no automatic three-file expansion |
| `FCR-REN-06` | direct lighting | [Direct Lighting package](Lighting/DirectLighting/DirectLightingDesign.md), `DIR-D0`, `DIR-PLAN-02` | `A`; seven-role ReSTIR DI/MegaLights-class research, semantics, UX, execution architecture, and staged clean-break plan exist; implementation/evidence remain gated |
| `FCR-REN-07` | indirect lighting | [Indirect Lighting package](Lighting/IndirectLighting/IndirectLightingDesign.md), `IND-D0`, `IND-PLAN-02` | `A`; seven-role seed-replay gap audit and target ReSTIR GI/GRIS transport, UX, architecture, and staged clean-break plan exist; implementation/evidence remain gated |
| `FCR-REN-08` | Reference Path Tracer | [full feature package](Lighting/ReferencePathTracer/ReferencePathTracerDesign.md), `PTD-00`, `LGT-3` | `A`; accepted PTD-00 and source stages are preserved; current recovery and remaining proof use the existing plan |
| `FCR-REN-09` | exposure | [Exposure](PostProcessing/DisplayPipeline/DisplayPipelineDesign.md#exposure), [Exposure Semantics](PostProcessing/DisplayPipeline/DisplayPipelineDesign.md#exposure-semantics), `DSP-2` | `C`; histogram policy, multiplier/adaptation equations, invalid-value handling, and provider/display domains have a dedicated current contract |
| `FCR-REN-10` | reconstruction/providers | [Image Reconstruction And Upscaling](PostProcessing/ReconstructionAndGeneration/ReconstructionAndGenerationDesign.md#image-reconstruction-and-upscaling), `DSP-3` | `C/B`; provider/version/product admission change requires discovery/research refresh |
| `FCR-REN-11` | debug views and capture | [Debug Views package](DebugViews/DebugViewsDesign.md), [Bounded Show evidence](../../../../../Acceptance/Renderer/DebugViews.md), `RD-3` | `B`; Show delivery is complete and its bounded candidate evidence is retained; current control contracts and remaining family proof stay in the feature package |
| `FCR-REN-12` | TLAS and ray scene | [Ray Tracing](RayTracing/RayTracingDesign.md), [Execution Architecture](RayTracing/ExecutionArchitecture.md), `GR-3` | `B`; reuse current ray-tracing decision/proof owners; no second TLAS plan or automatic package expansion |
| `FCR-REN-13` | UI/viewport composition | [UI And Viewport Composition](ViewportAndDiagnostics/ViewportAndDiagnosticsDesign.md#ui-and-viewport-composition), `RD-4` | `C`; add UX only if the product-facing interaction expands |
| `FCR-REN-14` | tone mapping | [Tone Mapping](PostProcessing/DisplayPipeline/DisplayPipelineDesign.md#tone-mapping), `DSP-4` | `C`; semantic contract required before changing operators/color domain |
| `FCR-REN-15` | presentation/output | [Presentation And Output](PostProcessing/DisplayPipeline/DisplayPipelineDesign.md#presentation-and-output), `DSP-4` | `C`; HDR has its separate `A` package below |
| `FCR-REN-16` | pipeline materialization/binding | [Pipeline Materialization And Typed Binding](ShaderRuntime/PipelineMaterializationAndTypedBinding.md), `RD-1` | `C`; discovery required for ABI/cache/generation redesign |
| `FCR-REN-17` | mesh/texture residency | [Mesh And Texture Residency](GeometryAndResources/MeshAndTextureResidency.md), `GR-1` | `C`; add plan/research for streaming/eviction architecture changes |
| `FCR-REN-18` | temporal sampling/history | [Temporal Sampling And History](FrameExecution/TemporalSamplingAndHistory.md), `DSP-1` | `C`; semantics companion required for jitter/motion/history convention changes |
| `FCR-REN-19` | settings/persistence | [Settings State And Persistence](RuntimeConfiguration/RuntimeConfigurationDesign.md#settings-state-and-persistence), `RD-2` | `C`; UX/plan split only for a product settings redesign |
| `FCR-REN-20` | latency coordination | [Latency Coordination](FrameExecution/LatencyCoordination.md), `RD-5` | `C/B`; provider timing-contract changes require current primary-source research |
| `FCR-REN-21` | visibility/draw preparation | [Visibility And Draw Preparation](GeometryAndResources/VisibilityAndDrawPreparation.md), `GR-2` | `C`; each admitted LOD/occlusion/GPU-driven/multiview capability needs its own discovery decision |
| `FCR-REN-22` | resolution/sampling/AA | [Resolution, Sampling, And Anti-Aliasing](PostProcessing/ReconstructionAndGeneration/ReconstructionAndGenerationDesign.md#resolution-sampling-and-anti-aliasing), `DSP-1` | `C`; absent AA/dynamic-resolution modes remain negative until admitted |
| `FCR-REN-23` | deferred decals | [focused package](DeferredDecals/DeferredDecalsDesign.md), `GR-5` | `B`; add Discovery before Phase 1; add Semantics only if composition rules cannot remain in architecture |
| `FCR-REN-24` | color grading | [full package](PostProcessing/DisplayPipeline/ColorGrading/ColorGradingDesign.md), `CGRD-00`, `DSP-5` | `A`; Stage 0 only until decisions pass |
| `FCR-REN-25` | chromatic aberration | [full package](PostProcessing/DisplayPipeline/ChromaticAberration/ChromaticAberrationDesign.md), `CHRD-00`, `DSP-6` | `A`; standalone UX is justified by active-state/artifact-classification workflow; Stage 0 only |
| `FCR-REN-26` | HDR Display Output | [full package](PostProcessing/DisplayPipeline/HDRDisplayOutput/HDRDisplayOutputDesign.md), `HDRD-00`, `DSP-7` | `A`; Stage 0 only until platform/color/hardware decisions pass |

## Explicit Negative Capabilities

| Capability | Current owner | Planning rule |
| --- | --- | --- |
| volumetric lighting/media | [Volumetric Lighting package](Lighting/VolumetricLighting/VolumetricLightingDesign.md), `VOL-D0`, `VOL-PLAN-02` | `A` target package; current capability remains 0/100 and no production stages are selected; a future tier needs its own admission |
| frame generation | [Frame Generation](PostProcessing/ReconstructionAndGeneration/ReconstructionAndGenerationDesign.md#frame-generation) | `D`; latency/provider adjacency is not support. Admission requires product, platform, fallback, pacing, UI, and evidence discovery. |
| non-ray lighting fallback | [Lighting](Lighting/LightingDesign.md) | `D`; admit as a separate traversal/product decision rather than hiding fallback inside direct/indirect plans. |
| occlusion, LOD, GPU-driven/indirect, stereo, multiview | [Visibility And Draw Preparation](GeometryAndResources/VisibilityAndDrawPreparation.md) | `D` per capability; do not create one generic “advanced visibility” plan. |
| Renderer MSAA, standalone TAA/FXAA/SMAA, dynamic resolution | [Resolution, Sampling, And Anti-Aliasing](PostProcessing/ReconstructionAndGeneration/ReconstructionAndGenerationDesign.md#resolution-sampling-and-anti-aliasing) | `D` per mode; current reconstruction/jitter infrastructure does not admit them. |

Volumetric and geometry-cache production sequences are deferred admission decisions, not work unlocked by release closeout. Preserve accepted PTD-00 and delivered Show controls; their existing remaining proof does not restart discovery or implementation.

## Maintenance

Update a row when its actual owner or selected contract changes. Link directly to existing design, discovery/proof and stage owners; add no parallel priority list, multi-wave documentation program or second results ledger. Validate affected local links/anchors and preserve the central readiness/evidence distinction. [Documentation Organization](../../../../../Engineering/Workflow/DocumentationOrganization.md) owns the document lifecycle.
