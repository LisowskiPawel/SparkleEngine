# SparkleEngine module scope decisions

[Portfolio direction](../PortfolioDirection.md) | [Goals](../Goals.md)

**Selected: Option C, a reduced engine suite with distinct Launcher, Editor and Game.** The decisions below choose what those products contain. This is a scope checklist; implementation stages come after your answers.

**Yes** means retain the capability, understand its full data flow and lifetime, and refine or reimplement it to the required standard. **No** means retire the capability and its otherwise unused callers, state, settings, build membership, scripts, dependencies and documentation. An undecided item stays undecided. My suggestion is not your decision.

Every `Your answer` cell starts with `-`. You can answer a section at a time, for example `LA-04 No, ED-03 Yes, GF-01 No`. An asterisk on a suggestion means another decision or content requirement must be resolved first. Coupled choices are listed near the end.

Start with [Launcher](#launcher), [Editor](#editor), [GameFramework](#gameframework-and-scene-runtime), [RHI](#rhi), [Renderer](#renderer) and [scene selection](#showcase-scenes-and-conversion-scripts). Those answers determine much of the supporting scope. You can also answer a row with a narrower subset if you want to split a grouped capability.

Correctness, safe lifetime, useful errors and protection of accepted outputs remain requirements of every retained workflow. They are not optional features in this checklist. Existing working code also needs your review and understanding; a Yes is not approval of its current architecture.

## Launcher

Owner: [Tools/Launcher/SparkleLauncher](../../Tools/Launcher/SparkleLauncher). Observed scope: 205 source/build files, 19,723 physical lines. Launcher remains a distinct application.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| LA-01 | Scene selection, readiness reporting and launching Editor or Game with explicit settings. | Yes | - |
| LA-02 | Scene previews and the Qt model/view scene browser. A No permits a much simpler selection UI. | Yes | - |
| LA-03 | Build Editor, Game and cooking tools from the Launcher, beyond simply running prepared products. | Yes | - |
| LA-04 | Workspace/IDE/compiler configuration UI, including choosing toolchains and generators. A No leaves one documented supported setup. | No | - |
| LA-05 | Per-dependency acquisition and preparation controls in the UI. A No can retain explicit source-preparation commands. | No | - |
| LA-06 | Download/sync selected scene content through Launcher. A No uses explicitly prepared local content. | Yes | - |
| LA-07 | Cook selected content and shaders from Launcher. This is separate from whether cooking tools exist. | Yes | - |
| LA-08 | Many build/cook scopes and advanced per-operation options, rather than a small set of supported workflows. | No | - |
| LA-09 | A full Clean page covering build trees, artifacts, workspace state, logs and dependency caches. | No | - |
| LA-10 | Narrow clean/rebuild actions for the selected generated products. A Yes still requires exact paths and preservation rules. | Yes | - |
| LA-11 | Rich CLI dashboard/catalog presentation beyond help, errors and retained command execution. Required previews of destructive effects remain. Owner: `Private/Shell`. | No | - |
| LA-12 | Scriptable noninteractive CLI access to the same retained operations. Editor shader compilation currently uses Launcher functionality too. | Yes | - |
| LA-13 | Launcher bootstrap/live-copy and rebuild/restart behavior. A No may require closing Launcher before replacing its executable. | No | - |
| LA-14 | GPU capture-tool setup and provider settings in Launcher. A No still permits manually installed native debugging tools. | No | - |
| LA-15 | GPU-crash incident presentation and related launch modes. This follows the crash-debugging decision AP-05. | No* | - |

Retained operations still need coherent progress, output, cancellation where supported, failure reporting and close behavior. Removing their UI does not remove the shared operation owners automatically.

## Editor

Owner: [Engine/Editor](../../Engine/Editor). Observed scope: 152 source/build files, 12,005 physical lines. Editor remains distinct from Game.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| ED-01 | Viewport camera, rendering settings and useful render/debug views. | Yes | - |
| ED-02 | Scene outliner, selection and inspection of existing scene objects. | Yes | - |
| ED-03 | Live mesh transform editing. A No keeps imported transforms and camera navigation. | Yes | - |
| ED-04 | Live visibility editing for existing objects. | Yes | - |
| ED-05 | Editing camera properties and choosing an active camera. | Yes | - |
| ED-06 | Editing existing lights and sky/environment settings. | Yes | - |
| ED-07 | Switching imported material variants. This depends on GF-03. | Yes* | - |
| ED-08 | Undo/redo and interaction coalescing for retained edits. A No means edits have no transaction history. | Yes | - |
| ED-09 | Saving the scene properties supported by the existing level serializer. This does not imply a general asset-authoring system. | Yes | - |
| ED-10 | Switching levels while Editor remains open. A No can use close/relaunch per scene. This follows GF-04. | Yes* | - |
| ED-11 | Interactive console, command history and runtime CVar editing. This depends on CO-01. | Yes* | - |
| ED-12 | Used-mesh diagnostics and interactive mesh preview. A No removes the panel and its unused diagnostic producers. | No | - |
| ED-13 | Used-texture diagnostics and texture inspection. A No still permits native GPU capture inspection. | No | - |
| ED-14 | Shader list/export inspection UI, beyond compile/reload and compiler error reporting. | No | - |
| ED-15 | Native capture launch controls in Editor for retained tools. Tool support itself is decided by RH-05/RH-06. | Yes* | - |
| ED-16 | Persistent viewport preferences and presentation state between runs. | Yes | - |

The existing [edit command set](../../Engine/GameFramework/Public/World/WorldEditCommand.h) is bounded: active camera, transforms, camera/light descriptions, visibility, sky and material variants. General object creation, duplication, reparenting or asset editing is not silently added by choosing Option C.

ED-09 also needs an explicit list of saved fields: a live edit being supported does not establish that it survives Save All. Keep that distinction visible when choosing the final Editor contract.

## Application hosts

Owner: [Engine/Application](../../Engine/Application). Observed scope: 60 source/build files, 5,373 physical lines. Keep separate Editor/Game composition and a cooked-only Game runtime.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| AP-01 | Editor-triggered shader compilation and reload, retaining accepted shaders when the attempt fails. | Yes | - |
| AP-02 | Final viewport screenshots and image export. | Yes | - |
| AP-03 | Reference accumulation plus raw/reference artifact export with the metadata needed to interpret it. | Yes | - |
| AP-04 | Persisting user rendering settings across runs, separate from authored scene settings. | Yes | - |
| AP-05 | The opt-in qualified NVIDIA GPU-crash/Aftermath product variant and incident pipeline. A No spans native hooks, identity checks, tools and packaging. | No | - |
| AP-06 | Developer command-line CVar overrides in the application hosts. A No leaves explicit launch settings and retained Editor controls. | Yes | - |

Startup, shutdown, process/task settlement, capture completion and lifetime ordering remain work to own. Three task resources or several coordinators should be justified by their actual lifetimes, rather than removed merely because they look similar.

## GameFramework and scene runtime

Owner: [Engine/GameFramework](../../Engine/GameFramework). Observed scope: 219 source/build files, 12,223 physical lines.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| GF-01 | Skeletal animation, imported clips, playback, skinning and animated geometry. A No reaches importers, cookers, world execution, GPU deformation and animation content. | No | - |
| GF-02 | Morph targets and animated morph weights. These are a separate choice from skeletal playback, though they share infrastructure. | No | - |
| GF-03 | Imported material variants and runtime variant selection. A No affects importer/cooked data, world views and Editor controls. | Yes | - |
| GF-04 | Asynchronous in-process scene replacement, cancellation and keeping the accepted world when loading fails. A No can replace the whole workflow with one scene per process. | Yes | - |
| GF-05 | The existing typed ECS/component-storage scene representation. A No means designing a simpler scene representation, not deleting scene identity or Editor functionality. | Yes | - |
| GF-06 | Parallel world-system execution for retained transforms and extraction. A No uses a simpler serial implementation where acceptable. | No* | - |
| GF-07 | Explicit authored instance groups and renderer-facing instance batching inputs. This is separate from automatically batching meshes in RE-07. | Yes | - |

Keep scene/material identity, selected edits, stable read/publication boundaries and one-way render extraction. GF-01/GF-02 No still permits camera and rigid-object movement; removing deformation does not require a static-only application. GF-06 needs a workload/cost check before sacrificing useful parallelism.

## RHI

Owner: [Engine/RHI](../../Engine/RHI). Observed scope: 372 source/build files, 37,814 physical lines.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| RH-01 | Personally owned D3D12 backend. | Yes | - |
| RH-02 | Personally owned Vulkan backend with independent qualification. A No also removes otherwise unused Vulkan compiler, SDK and deployment paths. | Yes | - |
| RH-03 | PTLAS and its vendor-specific command/resource/capability path. Standard BLAS/TLAS remains a separate retained requirement. | No | - |
| RH-04 | Multiple queue types and asynchronous compute scheduling beyond what the selected frame actually uses. A No narrows execution; required upload/submission synchronization remains. | No* | - |
| RH-05 | RenderDoc capture integration in the application. A No can still allow external/manual captures. | Yes | - |
| RH-06 | PIX capture integration in the application. This is separate from PIX event markers. | Yes | - |
| RH-07 | PIX marker/runtime support for non-Shipping D3D12 profiling. | Yes | - |
| RH-08 | Broad shader-target enumeration beyond the selected runtime targets. A No narrows compiler/contracts support; this is a small cut, not a major saving. | No | - |

Required: explicit resource/descriptor ownership, upload, commands, pipeline/shader ABI, native interop for retained SDKs, validation and GPU retirement. D3D12MA/VMA are already integrated. Their use does not remove your responsibility to understand allocation and lifetime policy.

At least one of RH-01 and RH-02 must remain Yes for a runnable engine. My suggestions keep both; choosing Option C did not itself decide the API count.

## Renderer

Owner: [Engine/Renderer](../../Engine/Renderer). Observed scope: 619 source/build files, 36,806 physical lines.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| RE-01 | The owned frame graph, resource transitions and transient/history lifetime model. A No requires a deliberate simpler execution design. | Yes | - |
| RE-02 | The main real-time Lit frame and direct-light/shadow path. | Yes | - |
| RE-03 | Owned production ReSTIR indirect-lighting work in addition to direct lighting and Reference. A No reduces real-time GI scope substantially. | Yes | - |
| RE-04 | Independent Reference path tracer as a correctness and investigation tool. | Yes | - |
| RE-05 | Both rasterized and ray-traced GBuffer algorithms. A No selects the rasterized GBuffer and retires the alternative where unused. | No | - |
| RE-06 | TLAS refit/update policy as well as full rebuild. A No simplifies policy, with possible performance cost for movement. | Yes | - |
| RE-07 | Automatic mesh batching in addition to explicitly authored instance groups. | No* | - |
| RE-08 | DLSS Super Resolution through the existing Streamline integration. | Yes | - |
| RE-09 | DLSS Ray Reconstruction through the existing Streamline integration. | Yes | - |
| RE-10 | The existing subsurface material/shading approximation and its dedicated outputs/debug views. | No | - |
| RE-11 | Alpha masking, two-sided surfaces and their matching raster/ray material behavior. Removing these excludes content that needs them. | Yes | - |
| RE-12 | Histogram-based automatic exposure alongside manual exposure. | Yes | - |
| RE-13 | HDR/advanced output-color modes beyond an explicit SDR presentation path. Source support still needs native/display qualification. | No | - |
| RE-14 | Vignette and chromatic aberration. A No simplifies presentation and removes their settings and shader branches. | No | - |
| RE-15 | Broad material/GBuffer/lighting visualization modes, rather than only a few final-image views. | Yes | - |

RE-03 Yes preserves meaningful algorithm depth and makes that part of your understanding budget. RE-07 needs a representative scene measurement before removing useful batching. Native and SDK comparison modes remain useful even with DLSS retained. PTLAS follows RH-03 and is not a second Renderer vote.

## Shader assets and rendering data

Owner: [Engine/Assets/Shaders](../../Engine/Assets/Shaders). Observed source scope: 145 HLSL/include files, 7,544 physical lines. This is a source/data area rather than a separately composed engine module.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| SH-01 | Both inline ray-query and ray-tracing pipeline shader paths where the retained frame requires them. A No requires choosing and adapting a tracing path. | Yes* | - |
| SH-02 | Wireframe and GPU-scene instance visualization in addition to material/lighting signal views. | Yes | - |
| SH-03 | Multiple built-in sky/environment images beyond one neutral diagnostic environment and one presentation environment. | No | - |

Shader removals otherwise follow their owning feature: deformation GF-01/GF-02, PTLAS RH-03, GBuffer alternative RE-05, subsurface RE-10, display effects RE-13/RE-14. Keep shared material semantics and host/shader layout checks. A shared include used by Reference is not removable just because a production pass disappears.

## Core

Owner: [Engine/Core](../../Engine/Core). Observed scope: 127 source/build files, 7,278 physical lines.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| CO-01 | Runtime console command/CVar registration and execution, including the Editor console consumer. | Yes | - |
| CO-02 | Named/category-based logging beyond simple process output. | Yes | - |
| CO-03 | Reusable event/subscription infrastructure rather than direct callbacks at every boundary. A No requires replacing all actual subscriptions. | Yes | - |
| CO-04 | A reusable Core child-process supervision service. A No moves or replaces a capability still required by retained Launcher/tools. | Yes | - |

File/path/JSON/hash/math/time primitives still follow actual consumers. File-set publication and reader lifetime are not cosmetic scaffolding: simplify only with an equivalent accepted-output guarantee. Remove unused public utilities and forwarding layers as normal cleanup after the scope choices.

## Platform

Owner: [Engine/Platform](../../Engine/Platform). Observed scope: 25 source/build files, 1,783 physical lines.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| PL-01 | The current SDL3 window/input backend with owned Sparkle adapters. A No is a platform rewrite, not automatic code reduction. | Yes | - |
| PL-02 | Both windowed and borderless presentation modes. | Yes | - |
| PL-03 | Layered/deferred input routing for Editor UI and viewport ownership. A No replaces that routing with a simpler explicit design. | Yes | - |

Keyboard/mouse camera control, focus-loss cancellation, pointer release and correct DPI/resize behavior remain requirements of the selected modes. No controller, VR or Linux work is being added by this checklist.

## SourceImporters

Owner: [Tools/Import/SourceImporters](../../Tools/Import/SourceImporters). Observed scope: 89 source/build files, 5,900 physical lines. [Source dispatch](../../Tools/Import/SourceImporters/Private/SourceSceneImporter.cpp) currently admits glTF/GLB, FBX and PLY.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| IM-01 | glTF/GLB scene import through cgltf, with the material/geometry subset chosen for Sparkle. | Yes | - |
| IM-02 | FBX import, including Assimp-backed geometry/materials and FBX-specific conversion. A No must first account for all retained FBX content. | No* | - |
| IM-03 | PLY import for Stanford scan content. | No* | - |
| IM-04 | Importing source-authored cameras, rather than authoring supported cameras in level files/Editor. | No | - |
| IM-05 | Importing source-authored lights, rather than authoring all retained lights in level files/Editor. | Yes | - |

Animation/morph/variant import follows GF-01/GF-02/GF-03. Tangent generation and coordinate/unit/material interpretation remain as required by retained content. Do not assume FBX can disappear while keeping every current scene; ModernSponzaKnight explicitly references an FBX asset. Bistro's retained source format must be established before deciding IM-02.

## AssetCooker

Owner: [Tools/Cooking/AssetCooker](../../Tools/Cooking/AssetCooker). Observed scope: 30 source/build files, 1,550 physical lines.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| AC-01 | Incremental cooking with source/output identity checks, rather than always recooking the full selection. | Yes | - |
| AC-02 | Bounded parallel asset/texture preparation. A No uses a simpler serial cook with potentially longer preparation time. | Yes | - |
| AC-03 | Fine-grained category/subset cook controls in addition to one supported selected-content cook. | No | - |

Retain one explicit selected-content plan, useful diagnostics and reliable publication. This executable composes SceneCooker, MeshCooker, MaterialCooker and texture requests; the supporting cookers below are mostly libraries, not independent user-facing applications.

## SceneCooker

Owner: [Tools/Cooking/SceneCooker](../../Tools/Cooking/SceneCooker). Observed scope: 30 source/build files, 1,430 physical lines.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| SC-01 | Metadata for whichever imported cameras/lights you retain, rather than cooking only mesh/material instances. A No must be reconciled with IM-04/IM-05. | Yes* | - |
| SC-02 | SceneCooker as a distinct cohesive library boundary within the content pipeline. A No merges its retained responsibility into the consuming cooker. | Yes | - |

Skeleton/animation builders follow GF-01 and morph payloads follow GF-02. Complete scene generations, content identity and acceptance stay protected even if scene content becomes much simpler.

## MeshCooker

Owner: [Tools/Cooking/MeshCooker](../../Tools/Cooking/MeshCooker). Observed scope: 7 source/build files, 377 physical lines.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| MC-01 | A separate MeshCooker library with a narrow checked mesh-writing contract. A No merges the responsibility; it does not remove mesh cooking. | Yes | - |

Static vertex/index/layout validity remains essential. Skeletal, skinning and morph payload support is a consequence of GF-01/GF-02. This small library is not a large saving merely because it has its own folder.

## MaterialCooker

Owner: [Tools/Cooking/MaterialCooker](../../Tools/Cooking/MaterialCooker). Observed scope: 8 source/build files, 435 physical lines.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| MA-01 | A separate MaterialCooker library that carries admitted material semantics and texture references into runtime data. | Yes | - |

Variant support follows GF-03 and subsurface support follows RE-10. Do not vote separately to remove their cooked representation while retaining the same runtime feature. Base PBR, alpha semantics, texture interpretation and raster/ray agreement remain correctness work.

## TextureCooker

Owner: [Tools/Cooking/TextureCooker](../../Tools/Cooking/TextureCooker). Observed scope: 31 source/build files, 2,192 physical lines.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| TX-01 | GPU block-compressed cooked textures and the associated offline processing. | Yes | - |
| TX-02 | HDR/EXR texture inputs for lighting/environment work. This is separate from HDR display output RE-13. | Yes | - |
| TX-03 | KTX/KTX2 input support in addition to the texture formats required by the selected scenes. | No* | - |
| TX-04 | A standalone TextureCooker executable, in addition to AssetCooker orchestration and shared texture contracts. | Yes | - |

Correct sRGB/linear handling, normal-map processing, mip generation, dimensions and source/output identity remain requirements for admitted textures. Remove a decoder/dependency only after checking retained source and embedded textures. A No to HDR display does not justify dropping EXR references or environment maps.

## ShaderCompiler

Owner: [Tools/Shaders/ShaderCompiler](../../Tools/Shaders/ShaderCompiler). Observed scope: 104 source/build files, 7,938 physical lines.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| CP-01 | The Slang shader-source backend in addition to DXC/HLSL. The measured Engine shader tree currently contains HLSL/includes and no `.slang` sources. | No* | - |
| CP-02 | Shader symbols, source correlation and the relevant native debug exports. | Yes | - |
| CP-03 | Developer-facing reflection/disassembly/export analysis modes beyond producing runtime artifacts and required debug data. | No | - |
| CP-04 | The first-party source preprocessing/mount machinery, rather than a narrower proven compiler-owned include path. This requires a replacement investigation before removal. | No* | - |

DXIL follows RH-01; SPIR-V follows RH-02. Retain typed host/shader ABI checks, reflection actually consumed by runtime, publication and failed-recompile preservation. ShaderMake is already integrated. CP-01 No does not automatically remove every Slang SDK use: identify whether any required tooling/debug export still depends on it. CP-04 is a proposed simplification, not an established safe deletion or a reason to repeat an already failed trial unchanged.

## ToolSupport

Owner: [Tools/Support/ToolSupport](../../Tools/Support/ToolSupport). Observed scope: 20 source/build files, 1,428 physical lines.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| TS-01 | Shared tool startup/error handling and project/content path contracts instead of copying them into each tool. | Yes | - |
| TS-02 | General multi-project discovery/resolution beyond the single Showcase project. A No fixes the supported product to Showcase. | No | - |

Retain genuine shared lifetime/format policy. Remove forwarding helpers and copies where actual consumers can use the owner directly. Project generality also reaches CMake, Launcher and cook planning, so TS-02 is one decision across those consumers.

## Showcase scenes and conversion scripts

Owner: [Projects/Showcase](../../Projects/Showcase), especially [Levels.catalog](../../Projects/Showcase/Levels.catalog). Observed source/script scope: 15 files, 762 physical lines; asset binaries and level data are outside those counts.

The suggestions below keep Empty, CornellBox, classic Sponza and one Bistro scene as distinct diagnostic/presentation roles. They are proposals: source suitability and redistribution status still need checking for the chosen public presentation.

| ID | Keep this scene or scene family? | My suggestion | Your answer |
| --- | --- | --- | --- |
| PR-01 | Empty, for startup, failure handling and a minimal runtime loop. | Yes | - |
| PR-02 | CornellBox and the owned conversion needed to reproduce it, for bounded transport/lighting checks. | Yes | - |
| PR-03 | Classic Sponza as a short geometry/material regression loop. | Yes | - |
| PR-04 | BistroExterior as the substantial presentation/performance workload. Source-format dependency must be established. | Yes* | - |
| PR-05 | BistroInteriorWine as a second substantial scene. | No | - |
| PR-06 | ModernSponza base scene in addition to classic Sponza/Bistro. | No | - |
| PR-07 | ModernSponzaCandles and its additional content. | No | - |
| PR-08 | ModernSponzaKnight and its animated FBX content. Depends on IM-02 and GF-01. | No | - |
| PR-09 | SanMiguelHigh and SanMiguelLow, which add large content and preparation/qualification work. | No | - |
| PR-10 | JungleRuins and its separate content route. | No | - |
| PR-11 | CesiumMan as a retained animation demonstration. Depends on GF-01. | No | - |
| PR-12 | LPSHead and its conversion scripts as a dedicated material/subsurface demonstration. | No | - |
| PR-13 | StanfordBunnySubsurface and StanfordDragonSubsurface. Retaining the source route needs PLY; the named shading demo also needs RE-10. | No | - |
| PR-14 | AnisotropyBarnLamp and SheenCloth as retained material examples. Keeping the scenes does not establish that their named advanced shading is implemented. | No | - |
| PR-15 | DamagedHelmet and DiffuseTransmissionPlant as additional material examples. | No | - |
| PR-16 | ABeautifulGame as another scene/content route. | No | - |
| PR-17 | Unsupported/unselected add-on catalog entries such as Ivy, Trees, Flood and Explosion as future work. | No | - |

A No removes the associated level/catalog/thumbnail/acquisition/conversion/package obligations when unused. It does not authorize deleting unrelated local downloads or captures. Removing a scene does not automatically retire a rendering algorithm still needed by another scene.

## CMake, packaging and repository automation

Owners: [CMake](../../CMake), root/module build files and [.github/workflows](../../.github/workflows). Measured CMake-root scope: 50 source/script files and 3,208 physical lines, plus the root composition file.

| ID | Keep this capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| BU-01 | Both static and shared/DLL engine module builds. A No standardizes on the existing static build and removes the unused shared variant obligations. | No | - |
| BU-02 | Multiple supported IDE/compiler/generator combinations. A No chooses one documented setup, with LA-04 reduced accordingly. | No | - |
| BU-03 | Standalone Launcher configure/build, independent of the full engine build. | Yes | - |
| BU-04 | Prepared prebuilt Launcher/Editor/Game bundle, plus matching-source build/cook instructions. | Yes | - |
| BU-05 | Importing/synchronizing prepared products across workspaces in addition to building and packaging the chosen suite. | No | - |
| BU-06 | Refinable project-specific style tooling and useful source/CI checks. | Yes | - |

Keep one artifact layout, authoritative dependency declarations, configuration-correct runtime staging and required notices. A No to multiple toolchains does not remove distinct Editor/Game products or necessary development/measurement configurations. Three products share real build policy; copying it into three scripts is not simplification.

## Third-party integration maintenance

Owner: [ThirdParty](../../ThirdParty), with runtime semantic adapters in their consuming modules.

| ID | Keep this policy/capability? | My suggestion | Your answer |
| --- | --- | --- | --- |
| TP-01 | Use the already integrated libraries for logging, JSON, processes, tasks, windows/input, UI, allocation and shader compilation, while owning Sparkle's contracts and policy. A No implies selectively replacing those integrations. | Yes | - |
| TP-02 | Keep maintained upstream patches/forks whose behavior is still required by retained workflows. A No needs an identified unpatched equivalent or retirement of the dependent capability. | Yes* | - |

Dependency retirement follows the last real consumer: ozz after animation cuts, Assimp after all Assimp-backed format cuts, KTX after admitted input cuts, and SDK payloads after their actual integrations disappear. NVAPI may have consumers beyond PTLAS; do not remove it from RH-03 alone. Bindings and patches count as first-party understanding/maintenance regardless of their folder.

## Documentation and future scope

Owners: [Docs](..), source instructions, build/cook documentation and dependency notices.

| ID | Keep this commitment? | My suggestion | Your answer |
| --- | --- | --- | --- |
| DO-01 | Keep three complete portfolio cases as a prerequisite for calling this cleanup finished, rather than finishing the selected engine first. | No | - |
| DO-02 | Keep optional future features as active prerequisites: additional reconstruction SDKs, SHaRC/NRC/NTC, owned neural training, broader materials or volumetrics. These are backlog commitments, not claims of current implementation. | No | - |

The already chosen direction remains: Sparkle is the main piece, every retained first-party responsibility must be understood, and separate experiment repositories come after cleanup. Preserve the useful professional/role references and current correctness limitations. Remove completed diaries, duplicate policy and obsolete feature instructions as their owners change.

## Coupled decisions to resolve together

These are consequences of the IDs above, not additional votes.

| Primary choice | Coupled scope |
| --- | --- |
| GF-01 / GF-02 animation and morphs | Importers; SceneCooker and mesh payloads; GameFramework components/resources/execution; Renderer deformation and geometry lifetime; shader registrations; relevant Editor presentation; ozz; PR-08/PR-11. |
| IM-02 / IM-03 source formats | Source dispatch and Assimp-backed/Ply adapters; source admission/catalog/acquisition; retained scene sources; scene conversion; cook dependencies. Resolve Bistro's source before committing to glTF-only. |
| GF-04 scene replacement | Editor Open Level; loading ownership, cancellation and publication; world generations; history/selection invalidation; Renderer resource retirement. A No changes the user workflow. |
| CO-01 runtime console/CVars | Editor console ED-11, application CVar overrides AP-06 and registration/execution users. Retained rendering settings still need their own explicit configuration path. |
| RH-02 Vulkan | Native backend, shader SPIR-V path, build/SDK inputs, Streamline interop, captures, deployment and independent GPU/image checks. |
| RH-03 PTLAS | RHI descriptors/commands/providers; Renderer scene partition/update state; settings persistence; Editor panel; shaders; vendor-specific build inputs still required by other features. |
| AP-05 crash debugging | Application incident ownership; RHI native hooks; ShaderCompiler symbol identity; Launcher UI; PowerShell scripts; SDK admission/staging and package variant. |
| RE-08 / RE-09 DLSS | Retain shared Streamline while either feature uses it; remove only feature-specific resources/options when one is retired. |
| RE-10 subsurface | Material import/cook/runtime fields; shader math/payload/debug views; dedicated content and any relevant UI. |
| RE-13 / RE-14 presentation | Renderer/shader paths, settings, Editor controls, native presentation metadata where applicable and tests of retained output. |
| TS-02 project generality | Launcher project resolution, ToolSupport, cook planning, CMake project discovery and product packaging. |
| BU-01 shared modules | DLL exports/linkage, static/shared registration behavior, target deployment and qualification; retain every required ABI annotation if any shared consumer remains. |

## Review basis

Reviewed source: `6dd51591b0890e71da5b4ae795ac730a0f49b695`, 25 November 2026, initially clean before these documentation changes. The selected-source inventory contained **2,313 files and 165,884 physical lines**, including comments and blanks. It covers C/C++, shaders and build/scripts under Engine, Tools, Projects and CMake plus the root CMake file. It excludes upstream vendor source, generated outputs, documentation and binary content. No removal count or runtime result is claimed.

The checklist follows current module composition, selected operation/host/editor paths, source-format dispatch, the world edit contract, rendering settings, shader backend declarations and the Showcase catalog. Per-module numbers are scale indicators, not judgments of code quality. Future implementation still needs complete consumer/lifetime checks for each chosen cut.

After your answers, the next document change is to turn the chosen rows into an ordered removal and reauthoring plan. No code or content has been removed by producing this list.
