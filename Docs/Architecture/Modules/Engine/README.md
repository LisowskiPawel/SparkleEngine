# Engine Module Architecture

[Docs](../../../README.md) / [Up](../ModuleInventory.md)

**Status:** Engine module index

Candidate readiness belongs in [Current Feature Readiness](../../../Acceptance/CurrentReadiness.md); this index routes implementation owners.

This index mirrors the durable modules under `Engine`. Open the owning module first; use hyperlinks to follow producer/consumer relationships without relocating that knowledge into a mixed folder.

## Module Routes

| Module | Owns | Module documentation |
| --- | --- | --- |
| Application | game/editor host composition, configuration, loop, startup, and shutdown | [Application](Application/ApplicationCapabilities.md) |
| Assets | engine-authored shaders, defaults, environments, and fixtures | [Assets](Assets/AssetsCapabilities.md) |
| Core | common infrastructure, diagnostics, files, processes, serialization, math, input, and time | [Core](Core/CoreCapabilities.md) |
| Editor | workspace, viewport sessions, editing UX, settings, and editor tool entry points | [Editor](Editor/EditorCapabilities.md) |
| GameFramework | levels, worlds, ECS, editing, publication, and render extraction | [GameFramework](GameFramework/GameFrameworkCapabilities.md) |
| Platform | Windows application, window, DPI, message, input, cursor, and capture integration | [Platform](Platform/PlatformCapabilities.md) |
| Renderer | whole-frame production plus explicit scene/view, geometry/GBuffer, ray tracing, Direct/Indirect/Volumetric Lighting, Post Processing subfeatures, presentation/UI, scheduling, shader-program, and diagnostic boundaries | [Renderer](Renderer/RendererDesign.md) |
| RHI | backend-neutral GPU contracts and D3D12/Vulkan implementations | [RHI](RHI/RHIDesign.md) |
| Tasks | task graphs, lanes, parallel ranges, cancellation, events, failure, and shutdown | [Tasks](Tasks/TasksCapabilities.md) |

Rules that apply while changing these modules live in [Engineering Modules](../../../Engineering/Modules/README.md). Cross-owner system designs live in [CrossModule](../../CrossModule/README.md).
