# Stable Identifier Architecture Coverage

[Docs](../../../README.md) / [Up](README.md)

**Status:** exact identifier-to-owner routing ledger; no implementation or acceptance result is inferred

**Scope:** exact source-authority identifiers and their Architecture owner; feature-local capability and acceptance IDs remain owned by their dossiers

**Source authorities:** [Engineer Persona](../../../Strategy/EngineerPersona.md), [Requirements](../../../Strategy/Requirements.md), [Roadmap](../../../Strategy/Roadmap.md), [Feature Completion Reports](../../../Acceptance/FeatureCompletionReports.md), [First Release](../../../Acceptance/FirstRelease.md), and [Graphics Workloads](../../../Acceptance/GraphicsWorkloads.md)

> [!TIP]
> This file is a reverse-lookup ledger. Search for an `NS-*`, `PGE-*`, `REL-*`, `RISK-*`, `FCR-*`, `MAP-*`, or `CASE-*` identifier to find its Architecture owner. The link proves routing coverage, not implementation or acceptance.

## Strategic Deliverable Outputs

`PGD-*` are local outputs owned by the [Strategy catalog](../../../Strategy/FeatureDeliveryCatalog.md), not new product capability IDs. This table owns routing only; prerequisites, numeric thresholds and results remain in their respective authorities.

| ID | Architecture owner routes |
| --- | --- |
| `PGD-01` | [First Release](../FirstRelease/ImplementationPlan.md), [Build/Packaging](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md) |
| `PGD-02` | [Content To Pixel design/plan](../../Modules/Projects/Showcase/ContentToPixel/ContentToPixelDesign.md), [Reference plan](../../Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ImplementationPlan.md), original Graphics Workloads proof |
| `PGD-03` | [CPU, GPU and lifetime subject plans](../PerformanceDiagnostics/WorkloadStudies/StudyProtocols.md#owned-evidence-folders), existing Renderer/RHI repair owners |
| `PGD-04` | [Incident design/plan](../PerformanceDiagnostics/WorkloadStudies/IncidentAnalysis/IncidentDesign.md), [External Capture](../PerformanceDiagnostics/ExternalCapture/ImplementationPlan.md) |
| `PGD-05` | [Python Automation And Analysis](../../Modules/Tools/PythonAutomationAndAnalysis.md), [Workload Studies](../PerformanceDiagnostics/WorkloadStudies/WorkloadStudiesDesign.md) |
| `PGD-06` | [Neural Training/Evaluation](../NeuralGraphics/NeuralGraphicsDesign.md#neural-graphics-training-and-evaluation), [Discovery](../NeuralGraphics/Discovery.md) |
| `PGD-07` | [Model To Kernel/Runtime](../NeuralGraphics/NeuralGraphicsDesign.md#model-to-kernel-and-runtime-inference), [Shader System](../ShaderSystem/ShaderSystemDesign.md), [Neural plan](../NeuralGraphics/ImplementationPlan.md) |
| `PGD-08` | [Neural acceptance](../NeuralGraphics/NeuralGraphicsDesign.md#neural-graphics-acceptance), [Workload Studies](../PerformanceDiagnostics/WorkloadStudies/WorkloadStudiesDesign.md), [portfolio assembly plan](../GraphicsPortfolio/ImplementationPlan.md#graphics-portfolio-delivery-plan); workload owners retain original case results |
| `PGD-09` | Required M7 [review/transfer criteria](../GraphicsPortfolio/ReviewAndTransfer.md#binary-review-criteria) and [Technology Transfer plan](../GraphicsPortfolio/TechnologyTransfer/ImplementationPlan.md); actual people/outcomes remain external evidence. [Upstream contribution](../GraphicsPortfolio/GraphicsPortfolioDesign.md#upstream-graphics-contribution) remains conditional on a real issue. |

## Research Reference Routing

Research-only `NVR-01..20` references are owned by [Rendering Reference Examples](../../../Strategy/Research/RenderingReferenceExamples.md); they are not product/gate identities. `01/06/13/18/19` route to ShaderSystem/RHI and workload/native evidence; `02/03/04/05/15/17` to Direct/Indirect/Reference lighting; `03/10/11/13` to owned NeuralGraphics; `07/09/12/14/16/20` to Workload Studies/tooling/adoption; `08` to conditional GeometryAndResources discovery. The [catalog exercises](../../../Strategy/FeatureDeliveryCatalog.md#research-informed-delivery-exercises) provide explicit links. Routing does not authorize optional feature adoption or add acceptance credit.

## Persona Outcomes

| ID | Architecture coverage |
| --- | --- |
| `NS-REAL` | [RHI](../../Modules/Engine/RHI/RHIDesign.md), [Renderer](../../Modules/Engine/Renderer/RendererDesign.md), [Product Execution](../ProductExecutionTraces.md), [Build/Packaging](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md) |
| `NS-MATH-DATA` | [Core](../../Modules/Engine/Core/CoreCapabilities.md), [Renderer Lighting](../../Modules/Engine/Renderer/Features/Lighting/LightingDesign.md), [Temporal Sampling](../../Modules/Engine/Renderer/Features/FrameExecution/TemporalSamplingAndHistory.md), [Neural Training/Evaluation](../NeuralGraphics/NeuralGraphicsDesign.md#neural-graphics-training-and-evaluation) |
| `NS-EVIDENCE` | [Performance Diagnostics](../PerformanceDiagnostics/PerformanceDiagnosticsDesign.md), [RHI Diagnostics](../../Modules/Engine/RHI/Features/DiagnosticsAndCapture/DiagnosticsAndCaptureDesign.md), [CI And Regression](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md#automation) |
| `NS-OWNERSHIP` | [Whole Repository Map](../../WholeRepositoryMap.md), module inventories, [Feature Execution](../FeatureExecutionTraces.md), [Product Execution](../ProductExecutionTraces.md) |
| `NS-ADOPTION` | [Adoption And Support](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#local-diagnostics-and-release-review), [Packaging](../../Modules/BuildAndPackaging/PackagingAndInstallation.md), [Launcher](../../Modules/Tools/Launcher/LauncherCapabilities.md) |
| `NS-SIMPLIFY` | [Whole Repository Map](../../WholeRepositoryMap.md), [Strategy Coverage](../StrategyCoverage.md), and the explicit non-capability sections in each module/feature dossier |

## Principal Graphics Engineer Requirements

This is the implementation/evidence lookup for all fifteen canonical targets, not a second definition or score. Open the exact plan stage, then its linked design/check owner. [Evidence folders](../../../Acceptance/GraphicsWorkloads.md#evidence-folders-and-delivery-owners) supplies original artifact homes. Several targets deliberately share one completed slice; create no subsystem, folder or reproduction per skill keyword.

| Target | Exact delivery route | Contract, result or explicit boundary |
| --- | --- | --- |
| `PGE-01` | [PD-2/3 source/product](../FirstRelease/ProductImplementationPlan.md), [M6 reproduction](../GraphicsPortfolio/ImplementationPlan.md#3-reproduce-review-and-revise), [KT-0..3 transfer](../GraphicsPortfolio/TechnologyTransfer/ImplementationPlan.md) | Product FCR/CASE-05 originals; [AC-PORT-03..06](../GraphicsPortfolio/ReviewAndTransfer.md#binary-review-criteria). Real people, constraints and interventions; no invented cross-company partnership. |
| `PGE-02` | [Reference recovery/proof](../../Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ImplementationPlan.md), [LGT-0..4](../../Modules/Engine/Renderer/FirstRelease/LightingImplementationPlan.md), [CPIX-2/3](../../Modules/Projects/Showcase/ContentToPixel/ImplementationPlan.md#cpix-2-prove-materials-lighting-and-raw-pixels) | Existing transport/oracle and WL-03/CASE-01/03 proof; raw domain and uncertainty remain mandatory. |
| `PGE-03` | [NG-S0..6](../NeuralGraphics/ImplementationPlan.md#remaining-stages) | [AC-NG-01..09](../NeuralGraphics/NeuralGraphicsDesign.md#neural-graphics-acceptance), original component/data/model/runtime/value proof; current vendor reconstruction is a separate feature. |
| `PGE-04` | [NG-S3 FP32](../NeuralGraphics/ImplementationPlan.md#ng-s3---export-and-fp32-conformance), [NG-S5 optimization](../NeuralGraphics/ImplementationPlan.md#ng-s5---measured-kernel-optimization), [shader Phase 12](../ShaderSystem/ImplementationPlan.md) | Fixed operator/export/math/ABI and float64/PyTorch/GPU proof, separate numerical drift and product effects; no arbitrary ML compiler is required. |
| `PGE-05` | [CPU](../PerformanceDiagnostics/WorkloadStudies/CpuPreparation/ImplementationPlan.md), [GPU](../PerformanceDiagnostics/WorkloadStudies/GpuShader/ImplementationPlan.md), [lifetime](../PerformanceDiagnostics/WorkloadStudies/ResourceLifetime/ImplementationPlan.md), existing release frame review | [WS acceptance](../PerformanceDiagnostics/WorkloadStudies/WorkloadStudiesDesign.md#local-acceptance-and-failures), per-run native/whole-frame/pacing/memory and quality evidence; one shader profile cannot stand for all domains. |
| `PGE-06` | [INC-0..3](../PerformanceDiagnostics/WorkloadStudies/IncidentAnalysis/ImplementationPlan.md), [external capture](../PerformanceDiagnostics/ExternalCapture/ImplementationPlan.md) | AC-WS-03/04, original bad/good/source/native incident record and controlled causal replay; qualified capture tools alone do not prove diagnosis. |
| `PGE-07` | [FWC-1/2/4/5](../FirstRelease/FoundationImplementationPlan.md), [WS-S1 Python consumer](../PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md#ws-s1---one-analysis-consumer), [PD-2 adoption](../FirstRelease/ProductImplementationPlan.md#pd-2--source-build-cook-and-launcher) | Existing C++ ownership/review/failure/source-adoption proof and [bounded Python contract](../PerformanceDiagnostics/WorkloadStudies/WorkloadStudiesDesign.md#bounded-offline-analysis-consumer). Each changed-source stage includes the source-style gate. |
| `PGE-08` | [CPIX-2](../../Modules/Projects/Showcase/ContentToPixel/ImplementationPlan.md#cpix-2-prove-materials-lighting-and-raw-pixels), domain study freeze/model stages, [NG-S0..3](../NeuralGraphics/ImplementationPlan.md) | Accepted estimator rules and independent fixtures, predicted-versus-measured cost, [neural semantics](../NeuralGraphics/Semantics.md), statistics/reference uncertainty; equations without falsifiers do not pass. |
| `PGE-09` | [RHI-2/3](../../Modules/Engine/RHI/FirstReleaseImplementationPlan.md), [shader Phase 12](../ShaderSystem/ImplementationPlan.md), [GPU-1](../PerformanceDiagnostics/WorkloadStudies/GpuShader/ImplementationPlan.md#gpu-1-qualify-executed-baseline-and-oracle) | RHI/ShaderSystem AC and FCR originals; paired API, compiler/options/cooked ABI/native joins, capability/failure and queue/resource truth. |
| `PGE-10` | [CPU-1/2](../PerformanceDiagnostics/WorkloadStudies/CpuPreparation/ImplementationPlan.md#cpu-1-qualify-the-baseline), [GPU-1/2](../PerformanceDiagnostics/WorkloadStudies/GpuShader/ImplementationPlan.md#gpu-1-qualify-executed-baseline-and-oracle), [LIFE-1/2](../PerformanceDiagnostics/WorkloadStudies/ResourceLifetime/ImplementationPlan.md#life-1-qualify-the-lifetime-baseline) | Actual topology/cache/work/traffic/register/occupancy/token models and native controls. State IR/instruction-range/full-ISA access honestly; no unsupported architecture generalization. |
| `PGE-11` | [NG-S1/2](../NeuralGraphics/ImplementationPlan.md#ng-s2---owned-training-and-model) | AC-NG-02/03; licensed scene/sequence splits, tiny-batch/gradient, fixed operators/loss, repeat policy, ablations/generalization and failure evidence. Owned training remains distinct from vendor inference. |
| `PGE-12` | [NG-S2](../NeuralGraphics/ImplementationPlan.md#ng-s2---owned-training-and-model), [NG-S4..6](../NeuralGraphics/ImplementationPlan.md#ng-s4---runtime-vertical-slice) | Separate training/inference profiles and the [bounded batch/storage-precision/native-layout controls](../NeuralGraphics/Semantics.md#required-bounded-profile-controls), plus startup, immutable export, workspace/tails and deployment value. Storage-only quantization cannot claim native half-speed. |
| `PGE-13` | [Portfolio stages 1..3](../GraphicsPortfolio/ImplementationPlan.md), [WS-S1](../PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md#ws-s1---one-analysis-consumer), [KT-1..3](../GraphicsPortfolio/TechnologyTransfer/ImplementationPlan.md#kt-1-record-the-meaningful-technical-challenge) | Three original cases, useful tool, paper/talk/demo, review and productization/deletion records; [Review And Transfer](../GraphicsPortfolio/ReviewAndTransfer.md) owns binary review checks. |
| `PGE-14` | [FWC-1 Windows](../FirstRelease/FoundationImplementationPlan.md#fwc-1--core-and-windows-platform), [RHI-3 paired API](../../Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-3--paired-backend-lowering), [M6/KT explanation](../GraphicsPortfolio/ImplementationPlan.md) | Windows D3D12/Vulkan, source/tool and clear English explanation evidence. Linux and AMD acquisition remain excluded; travel/communication/professional facts need truthful external records. |
| `PGE-15` | [KT-0..3](../GraphicsPortfolio/TechnologyTransfer/ImplementationPlan.md), [INC-0..3](../PerformanceDiagnostics/WorkloadStudies/IncidentAnalysis/ImplementationPlan.md), repeated admitted release/feature slices | Actual direction/review/learning/revision and simplification with original results. [Non-repository facts](../../../Strategy/Requirements.md#non-repository-requirements) remain CV/reference/interview evidence; plans cannot certify tenure or hiring level. |

The persona's six North Star outcomes apply inside each selected stage through Change Lifecycle: real consumer/result, math/data, reproducible proof, one owner, adoption and simplification. Unselected scope retains its negative owner. Plans/criteria/folders advance routing and preparation only; implementation, numerical/native and external evidence remain independently required.

## Feature Completion Report Families

### Product, Build, And Delivery

| ID | Architecture contract owner | Required contributors |
| --- | --- | --- |
| `FCR-PROD-01` | [Showcase game consumer](../../Modules/Projects/Showcase/ShowcaseCapabilities.md#fcr-prod-01-game-consumer-contract) | [Application host](../../Modules/Engine/Application/ApplicationCapabilities.md#product-contract-boundary), GameFramework level/session, Renderer/RHI, Packaging |
| `FCR-PROD-02` | [Build/Packaging source adopter](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md#fcr-prod-02-source-adopter-contract) | Adoption, CMake/dependencies, Cooking/ShaderCompiler, Showcase |
| `FCR-PROD-03` | [Launcher](../../Modules/Tools/Launcher/LauncherCapabilities.md#fcr-prod-03-launcher-contract) | Core process/files, build/cook/content owners, Showcase child products |
| `FCR-PROD-04` | [Editor](../../Modules/Engine/Editor/EditorCapabilities.md#fcr-prod-04-editor-contract) | [Application host](../../Modules/Engine/Application/ApplicationCapabilities.md#product-contract-boundary), GameFramework world/level, Renderer |
| `FCR-PROD-05` | [Packaging And Installation](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#fcr-prod-05-package-contract) | CMake/artifact owners, Showcase/Application, all runtime-byte contributors |
| `FCR-PROD-06` | [Adoption, Support, And Incident Response](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#fcr-prod-06-support-and-incident-contract) | Core/Application logs and failures, [RHI Diagnostics](../../Modules/Engine/RHI/Features/DiagnosticsAndCapture/DiagnosticsAndCaptureDesign.md), package/symbol/publication owners |

### Foundation, World, And Content

| ID | Architecture owner |
| --- | --- |
| `FCR-CORE-01` | [Core](../../Modules/Engine/Core/CoreCapabilities.md) |
| `FCR-PLAT-01` | [Platform](../../Modules/Engine/Platform/PlatformCapabilities.md); non-Windows hosts excluded |
| `FCR-TASK-01` | [Tasks](../../Modules/Engine/Tasks/TasksCapabilities.md) and [Multithreaded Engine](../MultithreadedEngine.md) |
| `FCR-WORLD-01` | [GameFramework](../../Modules/Engine/GameFramework/GameFrameworkCapabilities.md) |
| `FCR-WORLD-02` | [GameFramework](../../Modules/Engine/GameFramework/GameFrameworkCapabilities.md) and [Scene/View Preparation](../../Modules/Engine/Renderer/Features/SceneAndViewPreparation/SceneAndViewPreparationDesign.md) |
| `FCR-WORLD-03` | [GameFramework](../../Modules/Engine/GameFramework/GameFrameworkCapabilities.md), [Geometry/Resources](../../Modules/Engine/Renderer/Features/GeometryAndResources/GeometryAndResourcesDesign.md), and [Lighting](../../Modules/Engine/Renderer/Features/Lighting/LightingDesign.md) |
| `FCR-CONT-01` | [Source Importers](../../Modules/Tools/SourceImporters/SourceImportersCapabilities.md) |
| `FCR-CONT-02` | [Cooking](../../Modules/Tools/Cooking/CookingCapabilities.md) |
| `FCR-CONT-03` | [Engine Assets](../../Modules/Engine/Assets/AssetsCapabilities.md) |
| `FCR-SHDR-01` | [Shader System](../ShaderSystem/ShaderSystemDesign.md), [ShaderCompiler](../../Modules/Tools/ShaderCompiler/ShaderCompilerCapabilities.md), and [Shader Runtime](../../Modules/Engine/Renderer/Features/ShaderRuntime/ShaderRuntimeDesign.md) |
| `FCR-TOOL-01` | [Tool Support](../../Modules/Tools/ToolSupport/ToolSupportCapabilities.md) |

### RHI And GPU Execution

| ID | Architecture owner |
| --- | --- |
| `FCR-RHI-01` | [RHI Capability Inventory](../../Modules/Engine/RHI/CapabilityInventory.md) and [RHI Features](../../Modules/Engine/RHI/Features/RHIFeatureGuide.md) |
| `FCR-RHI-02` | [Command Submission And Synchronization](../../Modules/Engine/RHI/Features/PipelineAndExecution/PipelineAndExecutionDesign.md#command-submission-and-synchronization) and [Presentation](../../Modules/Engine/RHI/Features/PresentationAndInterop/PresentationAndInteropDesign.md#presentation) |
| `FCR-RHI-03` | [Backend Selection And Device Capabilities](../../Modules/Engine/RHI/Features/DeviceAndResources/DeviceAndResourcesDesign.md#backend-selection-and-device-capabilities) plus the RHI feature families |
| `FCR-RHI-04` | [RHI Ray Tracing](../../Modules/Engine/RHI/Features/PipelineAndExecution/PipelineAndExecutionDesign.md#ray-tracing) |
| `FCR-RHI-05` | [RHI Diagnostics And Capture](../../Modules/Engine/RHI/Features/DiagnosticsAndCapture/DiagnosticsAndCaptureDesign.md) |
| `FCR-RHI-06` | [Device Lifecycle And Failure Recovery](../../Modules/Engine/RHI/Features/DeviceAndResources/DeviceAndResourcesDesign.md#device-lifecycle-and-failure-recovery) |

### Renderer

| ID | Architecture owner |
| --- | --- |
| `FCR-REN-01` | [Renderer](../../Modules/Engine/Renderer/RendererDesign.md) and [Frame Execution](../../Modules/Engine/Renderer/Features/FrameExecution/FrameExecutionDesign.md) |
| `FCR-REN-02` | [Scene And View Preparation](../../Modules/Engine/Renderer/Features/SceneAndViewPreparation/SceneAndViewPreparationDesign.md) |
| `FCR-REN-03` | [Frame Graph And Scheduling](../../Modules/Engine/Renderer/Features/FrameExecution/FrameGraphAndScheduling.md) |
| `FCR-REN-04` | [Geometry, Materials, And GBuffer](../../Modules/Engine/Renderer/Features/GeometryAndResources/GeometryMaterialsAndGBuffer.md) |
| `FCR-REN-05` | [Renderer Ray Tracing](../../Modules/Engine/Renderer/Features/RayTracing/RayTracingDesign.md) and [Geometry/GBuffer](../../Modules/Engine/Renderer/Features/GeometryAndResources/GeometryMaterialsAndGBuffer.md) |
| `FCR-REN-06` | [Direct Lighting](../../Modules/Engine/Renderer/Features/Lighting/DirectLighting/DirectLightingDesign.md) |
| `FCR-REN-07` | [Indirect Lighting](../../Modules/Engine/Renderer/Features/Lighting/IndirectLighting/IndirectLightingDesign.md) |
| `FCR-REN-08` | [Reference Path Tracer](../../Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ReferencePathTracerDesign.md) |
| `FCR-REN-09` | [Exposure](../../Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/DisplayPipelineDesign.md#exposure) |
| `FCR-REN-10` | [Image Reconstruction And Upscaling](../../Modules/Engine/Renderer/Features/PostProcessing/ReconstructionAndGeneration/ReconstructionAndGenerationDesign.md#image-reconstruction-and-upscaling) |
| `FCR-REN-11` | [Debug Views](../../Modules/Engine/Renderer/Features/DebugViews/DebugViewsDesign.md) and [Diagnostics/Capture](../../Modules/Engine/Renderer/Features/ViewportAndDiagnostics/ViewportAndDiagnosticsDesign.md#diagnostics-products-and-capture) |
| `FCR-REN-12` | [Renderer Ray Tracing](../../Modules/Engine/Renderer/Features/RayTracing/RayTracingDesign.md) and [RHI Ray Tracing](../../Modules/Engine/RHI/Features/PipelineAndExecution/PipelineAndExecutionDesign.md#ray-tracing) |
| `FCR-REN-13` | [UI And Viewport Composition](../../Modules/Engine/Renderer/Features/ViewportAndDiagnostics/ViewportAndDiagnosticsDesign.md#ui-and-viewport-composition) |
| `FCR-REN-14` | [Tone Mapping](../../Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/DisplayPipelineDesign.md#tone-mapping) |
| `FCR-REN-15` | [Presentation And Output](../../Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/DisplayPipelineDesign.md#presentation-and-output) |
| `FCR-REN-16` | [Pipeline Materialization And Typed Binding](../../Modules/Engine/Renderer/Features/ShaderRuntime/PipelineMaterializationAndTypedBinding.md) |
| `FCR-REN-17` | [Mesh And Texture Residency](../../Modules/Engine/Renderer/Features/GeometryAndResources/MeshAndTextureResidency.md) |
| `FCR-REN-18` | [Temporal Sampling And History](../../Modules/Engine/Renderer/Features/FrameExecution/TemporalSamplingAndHistory.md) |
| `FCR-REN-19` | [Settings State And Persistence](../../Modules/Engine/Renderer/Features/RuntimeConfiguration/RuntimeConfigurationDesign.md#settings-state-and-persistence) |
| `FCR-REN-20` | [Latency Coordination](../../Modules/Engine/Renderer/Features/FrameExecution/LatencyCoordination.md) |
| `FCR-REN-21` | [Visibility And Draw Preparation](../../Modules/Engine/Renderer/Features/GeometryAndResources/VisibilityAndDrawPreparation.md) |
| `FCR-REN-22` | [Resolution, Sampling, And Anti-Aliasing](../../Modules/Engine/Renderer/Features/PostProcessing/ReconstructionAndGeneration/ReconstructionAndGenerationDesign.md#resolution-sampling-and-anti-aliasing) |
| `FCR-REN-23` | [Deferred Decals](../../Modules/Engine/Renderer/Features/DeferredDecals/DeferredDecalsDesign.md) |
| `FCR-REN-24` | [Color Grading](../../Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/ColorGrading/ColorGradingDesign.md) |
| `FCR-REN-25` | [Chromatic Aberration](../../Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/ChromaticAberration/ChromaticAberrationDesign.md) |
| `FCR-REN-26` | [HDR Display Output](../../Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/HDRDisplayOutput/HDRDisplayOutputDesign.md) |

## Release Gates And Risks

| ID | Architecture coverage |
| --- | --- |
| `REL-00` | [Runtime Configuration](../../Modules/Engine/Renderer/Features/RuntimeConfiguration/RuntimeConfigurationDesign.md), module inventories, [Adoption](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#local-diagnostics-and-release-review) |
| `REL-01` | [Packaging](../../Modules/BuildAndPackaging/PackagingAndInstallation.md), [Assets](../../Modules/Engine/Assets/AssetsCapabilities.md), [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md) |
| `REL-02` | [Build/Packaging](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md), [CI/Regression](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md#automation) |
| `REL-03` | [Packaging And Installation](../../Modules/BuildAndPackaging/PackagingAndInstallation.md) |
| `REL-04` | All module/feature dossiers; exact current families are mapped above |
| `REL-05` | [Renderer Features](../../Modules/Engine/Renderer/Features/RendererFeatureGuide.md), [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Reference Path Tracer](../../Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ReferencePathTracerDesign.md) |
| `REL-06` | [Performance Diagnostics](../PerformanceDiagnostics/PerformanceDiagnosticsDesign.md), [Frame Execution](../../Modules/Engine/Renderer/Features/FrameExecution/FrameExecutionDesign.md), residency owners |
| `REL-07` | [RHI Diagnostics](../../Modules/Engine/RHI/Features/DiagnosticsAndCapture/DiagnosticsAndCaptureDesign.md), [Device Lifecycle](../../Modules/Engine/RHI/Features/DeviceAndResources/DeviceAndResourcesDesign.md#device-lifecycle-and-failure-recovery) |
| `REL-08` | [Packaging](../../Modules/BuildAndPackaging/PackagingAndInstallation.md), [Adoption](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#local-diagnostics-and-release-review) |
| `REL-09` | [Adoption And Support](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#local-diagnostics-and-release-review), [Product Workflows](../ProductWorkflowCoverage.md) |
| `REL-10` | [Packaging](../../Modules/BuildAndPackaging/PackagingAndInstallation.md), [Adoption/Support](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#local-diagnostics-and-release-review) |
| `REL-11` | [Adoption, Support, And Incident Response](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#local-diagnostics-and-release-review) |
| `RISK-REL-01` | `REL-00` owners |
| `RISK-REL-02` | `REL-01` owners |
| `RISK-REL-03` | `REL-02` owners |
| `RISK-REL-04` | `REL-03` and `REL-08` owners |
| `RISK-REL-05` | `REL-04` feature owners |
| `RISK-REL-06` | `REL-05` owners |
| `RISK-REL-07` | `REL-06` owners |
| `RISK-REL-08` | `REL-07` owners |
| `RISK-REL-09` | `REL-08` and `REL-09` owners |
| `RISK-REL-10` | `REL-10` and `REL-11` owners |
| `RISK-REL-11` | [CI/Regression](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md#automation) and every evidence-producing feature owner |
| `RISK-REL-12` | [Strategy Coverage](../StrategyCoverage.md) and release sequencing authority |
| `RISK-REL-13` | [Reference Path Tracer](../../Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ReferencePathTracerDesign.md) |

### Common Release Failure Modes

| ID | Architecture owner |
| --- | --- |
| `FM-REL-01` | [Platform](../../Modules/Engine/Platform/PlatformCapabilities.md), [RHI device capability](../../Modules/Engine/RHI/Features/DeviceAndResources/DeviceAndResourcesDesign.md#backend-selection-and-device-capabilities) |
| `FM-REL-02` | [Packaging And Installation](../../Modules/BuildAndPackaging/PackagingAndInstallation.md) |
| `FM-REL-03` | [Packaging And Installation](../../Modules/BuildAndPackaging/PackagingAndInstallation.md) |
| `FM-REL-04` | [Core](../../Modules/Engine/Core/CoreCapabilities.md), [Packaging](../../Modules/BuildAndPackaging/PackagingAndInstallation.md) |
| `FM-REL-05` | [Assets](../../Modules/Engine/Assets/AssetsCapabilities.md), [Shader System](../ShaderSystem/ShaderSystemDesign.md), [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md) |
| `FM-REL-06` | [Packaging And Installation](../../Modules/BuildAndPackaging/PackagingAndInstallation.md), [Core](../../Modules/Engine/Core/CoreCapabilities.md) |
| `FM-REL-07` | [Core](../../Modules/Engine/Core/CoreCapabilities.md), [Renderer Settings](../../Modules/Engine/Renderer/Features/RuntimeConfiguration/RuntimeConfigurationDesign.md#settings-state-and-persistence) |
| `FM-REL-08` | [Feature Selectors](../../Modules/Engine/Renderer/Features/RuntimeConfiguration/RuntimeConfigurationDesign.md#feature-selector-catalog), [Image Reconstruction](../../Modules/Engine/Renderer/Features/PostProcessing/ReconstructionAndGeneration/ReconstructionAndGenerationDesign.md#image-reconstruction-and-upscaling) |
| `FM-REL-09` | [Device Lifecycle](../../Modules/Engine/RHI/Features/DeviceAndResources/DeviceAndResourcesDesign.md#device-lifecycle-and-failure-recovery), [RHI Diagnostics](../../Modules/Engine/RHI/Features/DiagnosticsAndCapture/DiagnosticsAndCaptureDesign.md) |
| `FM-REL-10` | [RHI Resource Lifetime](../../Modules/Engine/RHI/Features/DeviceAndResources/DeviceAndResourcesDesign.md#resource-lifetime-and-memory), [Renderer Residency](../../Modules/Engine/Renderer/Features/GeometryAndResources/MeshAndTextureResidency.md) |
| `FM-REL-11` | [Platform](../../Modules/Engine/Platform/PlatformCapabilities.md), [RHI Presentation](../../Modules/Engine/RHI/Features/PresentationAndInterop/PresentationAndInteropDesign.md#presentation), [Frame Execution](../../Modules/Engine/Renderer/Features/FrameExecution/FrameExecutionDesign.md) |
| `FM-REL-12` | [Adoption And Support](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#local-diagnostics-and-release-review), [Packaging](../../Modules/BuildAndPackaging/PackagingAndInstallation.md) |
| `FM-REL-13` | [Adoption And Support](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#local-diagnostics-and-release-review), [RHI Diagnostics](../../Modules/Engine/RHI/Features/DiagnosticsAndCapture/DiagnosticsAndCaptureDesign.md) |
| `FM-REL-14` | [Build/Packaging](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md), [CI/Regression](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md#automation) |
| `FM-REL-15` | [Packaging](../../Modules/BuildAndPackaging/PackagingAndInstallation.md), [Adoption/Support](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#local-diagnostics-and-release-review) |
| `FM-REL-16` | [Packaging](../../Modules/BuildAndPackaging/PackagingAndInstallation.md), [Adoption/Support](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#local-diagnostics-and-release-review) |

## Graphics Map Gates

| ID | Architecture coverage |
| --- | --- |
| `MAP-00` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Performance Diagnostics](../PerformanceDiagnostics/PerformanceDiagnosticsDesign.md), Renderer/RHI diagnostics |
| `MAP-01` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Geometry/GBuffer](../../Modules/Engine/Renderer/Features/GeometryAndResources/GeometryMaterialsAndGBuffer.md), [Lighting](../../Modules/Engine/Renderer/Features/Lighting/LightingDesign.md) |
| `MAP-02` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Presentation/Output](../../Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/DisplayPipelineDesign.md#presentation-and-output) |
| `MAP-03` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Geometry/GBuffer](../../Modules/Engine/Renderer/Features/GeometryAndResources/GeometryMaterialsAndGBuffer.md) |
| `MAP-04` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [GameFramework](../../Modules/Engine/GameFramework/GameFrameworkCapabilities.md), [Temporal Sampling](../../Modules/Engine/Renderer/Features/FrameExecution/TemporalSamplingAndHistory.md) |
| `MAP-05` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Geometry/GBuffer](../../Modules/Engine/Renderer/Features/GeometryAndResources/GeometryMaterialsAndGBuffer.md), [Ray Tracing](../../Modules/Engine/Renderer/Features/RayTracing/RayTracingDesign.md) |
| `MAP-06` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Residency](../../Modules/Engine/Renderer/Features/GeometryAndResources/MeshAndTextureResidency.md), [Visibility](../../Modules/Engine/Renderer/Features/GeometryAndResources/VisibilityAndDrawPreparation.md) |
| `MAP-07` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Source Importers](../../Modules/Tools/SourceImporters/SourceImportersCapabilities.md), Renderer feature owners |
| `MAP-08` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Source Importers](../../Modules/Tools/SourceImporters/SourceImportersCapabilities.md), [Lighting](../../Modules/Engine/Renderer/Features/Lighting/LightingDesign.md) |
| `MAP-09` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Geometry/GBuffer](../../Modules/Engine/Renderer/Features/GeometryAndResources/GeometryMaterialsAndGBuffer.md), [Residency](../../Modules/Engine/Renderer/Features/GeometryAndResources/MeshAndTextureResidency.md) |
| `MAP-10` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Lighting](../../Modules/Engine/Renderer/Features/Lighting/LightingDesign.md) |
| `MAP-11` | [Geometry Cache Animation](../GeometryCacheAnimation/GeometryCacheAnimationDesign.md), [Temporal Sampling](../../Modules/Engine/Renderer/Features/FrameExecution/TemporalSamplingAndHistory.md), [Ray Tracing](../../Modules/Engine/Renderer/Features/RayTracing/RayTracingDesign.md) |
| `MAP-12` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Source Importers](../../Modules/Tools/SourceImporters/SourceImportersCapabilities.md), [Residency](../../Modules/Engine/Renderer/Features/GeometryAndResources/MeshAndTextureResidency.md) |
| `MAP-13` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Reference Path Tracer](../../Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ReferencePathTracerDesign.md), [Lighting](../../Modules/Engine/Renderer/Features/Lighting/LightingDesign.md) |

## Specialist Cases And Workload Ladder

| ID | Architecture coverage |
| --- | --- |
| `CASE-01` | SourceImporters, Cooking, Showcase, Geometry/GBuffer, Debug Views |
| `CASE-02` | [RHI Features](../../Modules/Engine/RHI/Features/RHIFeatureGuide.md), [Graphics Coverage](../GraphicsCoverageMatrix.md), [Feature Execution](../FeatureExecutionTraces.md) |
| `CASE-03` | [Reference Path Tracer](../../Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ReferencePathTracerDesign.md), [Performance Diagnostics](../PerformanceDiagnostics/PerformanceDiagnosticsDesign.md) |
| `CASE-04` | [Neural Graphics](../NeuralGraphics/NeuralGraphicsDesign.md), [Training/Evaluation](../NeuralGraphics/NeuralGraphicsDesign.md#neural-graphics-training-and-evaluation), [Model To Kernel](../NeuralGraphics/NeuralGraphicsDesign.md#model-to-kernel-and-runtime-inference) |
| `CASE-05` | [Packaging](../../Modules/BuildAndPackaging/PackagingAndInstallation.md), [Adoption/Support](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#local-diagnostics-and-release-review), [Product Workflows](../ProductWorkflowCoverage.md) |
| `WL-01` | [Assets](../../Modules/Engine/Assets/AssetsCapabilities.md), [Source Importers](../../Modules/Tools/SourceImporters/SourceImportersCapabilities.md), [Cooking](../../Modules/Tools/Cooking/CookingCapabilities.md) |
| `WL-02` | [Showcase](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), source/cook owners |
| `WL-03` | Showcase and Renderer/RHI feature owners, including [Reference Path Tracer](../../Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ReferencePathTracerDesign.md) |
| `WL-04` | [Performance Diagnostics](../PerformanceDiagnostics/PerformanceDiagnosticsDesign.md), [RHI](../../Modules/Engine/RHI/RHIDesign.md), [Renderer](../../Modules/Engine/Renderer/RendererDesign.md) |
| `WL-05` | [Neural Training And Evaluation](../NeuralGraphics/NeuralGraphicsDesign.md#neural-graphics-training-and-evaluation) |
| `WL-06` | [Model To Kernel And Runtime Inference](../NeuralGraphics/NeuralGraphicsDesign.md#model-to-kernel-and-runtime-inference) |
| `WL-07` | [Performance Diagnostics](../PerformanceDiagnostics/PerformanceDiagnosticsDesign.md), [CI/Regression](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md#automation), [Adoption](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#local-diagnostics-and-release-review) |
| `WL-08` | Feature dossiers plus [Adoption/Support](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#local-diagnostics-and-release-review); publication content remains a strategy/acceptance artifact, not an engine module |

All current identifiers in the named persona, requirement, FCR registry, release-gate/risk register, map queue, specialist-case table, and workload ladder occur explicitly above. New identifiers must be added here in the same change that introduces them.
