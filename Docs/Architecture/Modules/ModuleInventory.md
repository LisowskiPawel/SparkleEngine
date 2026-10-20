# Module Architecture And Capability Inventory

[Docs](../../README.md) / [Up](../README.md)

**Status:** module architecture index and dated capability snapshot; not release approval or executable evidence

**Snapshot:** 2026-09-06 at committed `master` revision `8414b5dc`; the detailed inventories listed below inspected their named source/build surfaces in the live working tree; no build, shader cook, launch, GPU capture, performance run, package run, or clean-machine run was performed for this inventory

**Scope:** navigation by repository module plus current engine, tool, project, and build capabilities, their exact coverage and limits, and the evidence still required before a release claim

**Release classification authority:** [First Release Acceptance Contract](../../Acceptance/FirstRelease.md)

**Release scope:** [Approved scope](../../Acceptance/FirstRelease.md#approved-release-scope) and [capability dispositions](../../Acceptance/ReleaseScope/CapabilityDispositions.md). These decisions do not promote the source evidence below.

**Per-feature candidate report authority:** [First Release Feature Completion Reports](../../Acceptance/FeatureCompletionReports.md)

**Current readiness authority:** [Current Feature Readiness](../../Acceptance/CurrentReadiness.md)

**Sequencing authority:** [Release-First Principal Graphics Roadmap](../../Strategy/Roadmap.md)

## Browse By Repository Boundary

| Boundary | Modules |
| --- | --- |
| [Engine](Engine/README.md) | Application, Assets, Core, Editor, GameFramework, Platform, Renderer, RHI, Tasks |
| [Tools](Tools/README.md) | Cooking, Launcher, ShaderCompiler, SourceImporters, ToolSupport |
| [Projects](Projects/README.md) | Showcase product composition and workloads |
| [Build And Packaging](BuildAndPackaging/BuildAndPackagingCapabilities.md) | build/dependency mechanics plus distinct package/install, CI/regression, and adoption/support dossiers |
| [CrossModule](../CrossModule/README.md) | systems that genuinely span multiple durable owners without one primary module |

Physical placement follows ownership: Renderer details stay under `Engine/Renderer`, RHI details under `Engine/RHI`, and relationships are expressed through links. A feature's plan and research stay beside its dossier; broader module plans stay at the module root, and ownerless joins use `CrossModule`. The inventory below provides the repository-wide evidence ledger without replacing those routes.

## Use The Inventory By Question

| Reader question | Shortest route |
| --- | --- |
| What does one module support and explicitly not support? | Open its detailed inventory below; capability rows and non-capabilities are the source snapshot. |
| Where is the implementation owner or build boundary? | Start with [Engine](Engine/README.md), [Tools](Tools/README.md), [Projects](Projects/README.md), or [Build And Packaging](BuildAndPackaging/BuildAndPackagingCapabilities.md), then verify source/CMake. |
| How does a graphics feature vary across backend or execution mode? | Use the [Graphics Feature Coverage Matrix](../CrossModule/GraphicsCoverageMatrix.md). |
| How does a graphics feature execute end to end? | Use the [Graphics Feature Execution Traces](../CrossModule/FeatureExecutionTraces.md). |
| Can a developer or user complete a build/content/editor/game/delivery journey? | Use [Product Workflow Coverage](../CrossModule/ProductWorkflowCoverage.md). |
| Where does that product journey cross owners and fail or settle? | Use [Product Execution Traces](../CrossModule/ProductExecutionTraces.md). |
| What question is still unanswered and what is the smallest next check? | Use the [Capability Evidence Plan](CapabilityEvidencePlan.md). |
| What must this capability prove, and can it ship? | Use its Architecture feature dossier for the local proof contract, then its `FCR-*` report and the [First Release Acceptance Contract](../../Acceptance/FirstRelease.md) for actual approval. |
| How do I harden or review this dossier? | Follow [Capability Documentation Review](../../Engineering/Workflow/CapabilityReview.md). |

## How To Read A Row

### Capability identity

Every independently referenceable inventory row has a durable capability ID. Module inventories use `<module>-<family>-NN` so the identifier states both the owner and the local subject, for example `RHI-BIND-05`, `REN-LGT-03`, or `SHD-PUB-05`. Evidence work uses the separate `<module>-ENN` namespace, such as `RHI-E06`; an evidence item may cover several capability rows and a capability may require several evidence items.

Keep an ID with its semantic capability when rows move or tables are reordered. Append new IDs within the owning family. Do not reuse an identifier after a capability is removed or split; record its replacement or terminal disposition in the applicable candidate report before removing the current-snapshot row. Cross-module workflows (`WF-*`) and candidate reports (`FCR-*`) remain separate identities and must map back to capability IDs rather than replace them.

### Implementation state

| State | Meaning |
| --- | --- |
| **Implemented path** | Current build membership and source contain the named producer-to-consumer path. This is not proof that it builds or works on a machine. |
| **Capability-gated** | The path exists, but build options, SDKs, device features, vendor support, or optional binaries decide whether it can run. |
| **Partial** | A useful path exists, but the row names material coverage gaps, unsupported variants, or an unfinished product boundary. |
| **Vocabulary only** | An enum, schema, flag, or low-level primitive exists without a current product producer/consumer path. It must not be advertised as supported. |
| **Not yet audited** | This inventory has not reconciled the module deeply enough to make a feature claim. It does not mean absent. |
| **Not found** | A targeted source/build search did not find the named capability in this snapshot. This remains a dated observation, not a permanent prohibition. |

### Evidence state

| Mark | Evidence |
| --- | --- |
| `S` | Source and executable build membership reconciled for the stated scope. |
| `B` | Relevant targets built from the recorded revision/configuration. |
| `R` | The capability was exercised through its real runtime or tool consumer. |
| `N` | Native validation or an external GPU capture checked the backend claim. |
| `P` | Performance and resource behavior were measured on a named machine/workload. |
| `A` | The release owner approved the row as Included, Experimental, Excluded, or Removed. |

The initial detailed inventories carry `S` only. No `B`, `R`, `N`, `P`, or `A` evidence was produced by this documentation pass. A source-present row therefore remains **Pending** for release disposition. Candidate-bound results belong in the feature completion reports and their retained artifacts, not in this dated source snapshot.

## Detailed Inventories

| Module | Depth in this snapshot | Document |
| --- | --- | --- |
| Application | Detailed | [Application Capability Inventory](Engine/Application/ApplicationCapabilities.md) |
| Build and packaging | Detailed | [Build And Packaging Capability Inventory](BuildAndPackaging/BuildAndPackagingCapabilities.md) |
| Core | Detailed | [Core Capability Inventory](Engine/Core/CoreCapabilities.md) |
| Editor | Detailed | [Editor Capability Inventory](Engine/Editor/EditorCapabilities.md) |
| Engine asset corpus | Detailed | [Engine Assets Capability Inventory](Engine/Assets/AssetsCapabilities.md) |
| GameFramework | Detailed | [GameFramework Capability Inventory](Engine/GameFramework/GameFrameworkCapabilities.md) |
| Launcher | Detailed | [Launcher Capability Inventory](Tools/Launcher/LauncherCapabilities.md) |
| Asset cooking | Detailed | [Asset Cooking Capability Inventory](Tools/Cooking/CookingCapabilities.md) |
| Platform | Detailed | [Platform Capability Inventory](Engine/Platform/PlatformCapabilities.md) |
| RHI | Detailed | [RHI Capability Inventory](Engine/RHI/CapabilityInventory.md), reached through the [RHI module route](Engine/RHI/RHIDesign.md) |
| Renderer | Detailed | [Renderer Capability Inventory](Engine/Renderer/CapabilityInventory.md), reached through the [Renderer module route](Engine/Renderer/RendererDesign.md) |
| Shader compilation and delivery | Detailed | [Shader Compilation Capability Inventory](Tools/ShaderCompiler/ShaderCompilerCapabilities.md) |
| Showcase products | Detailed | [Showcase Product Capability Inventory](Projects/Showcase/ShowcaseCapabilities.md) |
| Source importers | Detailed | [Source Importers Capability Inventory](Tools/SourceImporters/SourceImportersCapabilities.md) |
| Tasks | Detailed | [Tasks Capability Inventory](Engine/Tasks/TasksCapabilities.md) |
| Shared tool support | Detailed | [Shared Tool Support Capability Inventory](Tools/ToolSupport/ToolSupportCapabilities.md) |
| Python automation and analysis | Negative/target dossier | [Python Automation And Analysis](Tools/PythonAutomationAndAnalysis.md) |
| Persona/roadmap/gap coverage | Strategy crosswalk | [Capability Coverage Against Persona, Roadmap, And Gap Assessment](../CrossModule/StrategyCoverage.md) |
| Neural graphics | Negative/target dossier | [Neural Graphics](../CrossModule/NeuralGraphics/NeuralGraphicsDesign.md) |
| Performance diagnostics | Focused dated snapshot | [Performance Diagnostics Capability Inventory](../CrossModule/PerformanceDiagnostics/Capability.md) |
| Geometry-cache animation | Focused dated snapshot | [Geometry Cache Animation Capability Snapshot](../CrossModule/GeometryCacheAnimation/GeometryCacheAnimationDesign.md#geometry-cache-animation-capability-snapshot) |
| Deferred GBuffer decals | Current feature-gap dossier | [Deferred Decals](Engine/Renderer/Features/DeferredDecals/DeferredDecalsDesign.md) |
| Debug-view presentation | Current feature dossier | [Debug Views](Engine/Renderer/Features/DebugViews/DebugViewsDesign.md) |
| Cross-system graphics coverage | Deep horizontal | [Graphics Feature Coverage Matrix](../CrossModule/GraphicsCoverageMatrix.md) |
| Feature execution | Deep vertical | [Graphics Feature Execution Traces](../CrossModule/FeatureExecutionTraces.md) |
| Product/developer workflow coverage | Deep horizontal | [Product And Developer Workflow Coverage](../CrossModule/ProductWorkflowCoverage.md) |
| Product/developer execution | Deep vertical | [Product And Developer Execution Traces](../CrossModule/ProductExecutionTraces.md) |
| Registered Renderer programs | Exact catalog | [Renderer Shader Program Catalog](Engine/Renderer/Features/ShaderRuntime/ShaderProgramCatalog.md) |
| Evidence closure | Plan | [Capability Evidence Plan](CapabilityEvidencePlan.md) |


## Current Coverage Boundary

Every top-level implementation owner is routed above and has a source-depth capability inventory. That closes the first navigation skeleton, not the release or the dossier audit. Horizontal graphics and product-workflow matrices, vertical execution traces, and the evidence plan own the remaining cross-module questions. [Inventory hardening items `INV-009` through `INV-012`](CapabilityEvidencePlan.md#inventory-expansion) deliberately remain Open until every capability has all dossier dimensions, journeys, proof destinations, and reverse mappings reconciled. Any new public selector, executable, catalog entry, backend/mode, generated product, or module must enter those routes before it can be treated as inventoried.

## Claims That Require The Deeper Route

| Claim or reviewer concern | Owning detail |
| --- | --- |
| D3D12/Vulkan coverage or parity | [RHI capability inventory](Engine/RHI/CapabilityInventory.md), [RHI feature dossiers](Engine/RHI/Features/RHIFeatureGuide.md), [graphics matrix](../CrossModule/GraphicsCoverageMatrix.md), and candidate evidence |
| Ray tracing, inline/native pipelines, shader tables, classic/PTLAS | [Renderer ray-tracing dossier](Engine/Renderer/Features/RayTracing/RayTracingDesign.md), [RHI ray-tracing dossier](Engine/RHI/Features/PipelineAndExecution/PipelineAndExecutionDesign.md#ray-tracing), and [graphics traces](../CrossModule/FeatureExecutionTraces.md) |
| Bindless/material binding coverage | [graphics matrix](../CrossModule/GraphicsCoverageMatrix.md#binding-and-material-coverage) |
| PBR/material/direct-light coverage | [Geometry, Materials, and GBuffer](Engine/Renderer/Features/GeometryAndResources/GeometryMaterialsAndGBuffer.md), [Direct Lighting](Engine/Renderer/Features/Lighting/DirectLighting/DirectLightingDesign.md), and the [exact inventory](Engine/Renderer/CapabilityInventory.md) |
| Indirect-light/history/environment coverage | [Indirect Lighting and ReSTIR GI](Engine/Renderer/Features/Lighting/IndirectLighting/IndirectLightingDesign.md) and the [whole-frame route](Engine/Renderer/RenderingASparkleFrame.md) |
| Volumetric/fog/atmosphere/sky coverage | [Volumetric Lighting](Engine/Renderer/Features/Lighting/VolumetricLighting/VolumetricLightingDesign.md), which owns the current absence and blocked post-release target |
| Exposure, tone mapping, color grading, chromatic aberration, reconstruction/upscaling, frame generation, and output | [Post Processing](Engine/Renderer/Features/PostProcessing/PostProcessingDesign.md), including explicit negative dossiers for unsupported stages |
| Shader language/stage/compiler/target/runtime coverage | [ShaderCompiler](Tools/ShaderCompiler/ShaderCompilerCapabilities.md) and [program catalog](Engine/Renderer/Features/ShaderRuntime/ShaderProgramCatalog.md) |
| Build/cook/editor/game/package user journey | [product workflow matrix](../CrossModule/ProductWorkflowCoverage.md) and [product traces](../CrossModule/ProductExecutionTraces.md) |
| Persona, roadmap, and gap completeness | [strategy coverage](../CrossModule/StrategyCoverage.md) |
| Release inclusion or exclusion | [feature reports](../../Acceptance/FeatureCompletionReports.md) and [release acceptance](../../Acceptance/FirstRelease.md) |

## Maintenance Route

Use [Capability Documentation Review](../../Engineering/Workflow/CapabilityReview.md) to audit a changed dossier, [Documentation Organization](../../Engineering/Workflow/DocumentationOrganization.md) for placement and ownership, and the [Capability Evidence Plan](CapabilityEvidencePlan.md) for unanswered checks. Do not mark a row `B`, `R`, `N`, `P`, or `A` from documentation review, a responsive process, an uninspected screenshot, or an unrecorded local result.
