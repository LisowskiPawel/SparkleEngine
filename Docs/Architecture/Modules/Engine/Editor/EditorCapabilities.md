# Editor Capability Inventory

[Docs](../../../../README.md) / [Up](../README.md)

**Status:** capability snapshot; current, but not usability, correctness, or packaged-editor evidence

**Snapshot:** 2026-09-08 at committed `master` revision `ffe60e3a`; `Engine/Editor`, `SparkleApplicationEditor`, world-edit, Renderer diagnostic, save, shutdown, and CMake surfaces inspected; evidence `S` only

**Scope:** workspace UI, viewport, level actions, scene inspection/editing, undo/redo, rendering settings, shader/mesh/texture tools, console, capture, restart, and runtime separation

**Owner:** `Engine/Editor` / `SparkleEditor`; editor hosting and long-running operations belong to `SparkleApplicationEditor`

**Evidence and disposition:** [Capability Evidence Plan](../../CapabilityEvidencePlan.md) and [First Release Acceptance Contract](../../../../Acceptance/FirstRelease.md)

**Current readiness:** **45/100** — the development Editor is source-integrated; usability, correctness, performance, distribution classification, and rehearsal evidence remain open. See [Current Feature Readiness](../../../../Acceptance/CurrentReadiness.md#product-build-and-delivery).

## At A Glance

| Workflow | Current result | Main limitation |
| --- | --- | --- |
| workspace and level | fixed ImGui workspace, registered-level open, progress gating, and Save All | no general docking, new/Save As, or dirty-document workflow |
| scene inspection/editing | outliner plus typed camera/light/mesh/sky/variant commands with generation checks | not a full content/model/material authoring environment |
| history | command-based undo/redo for owned world edits | multi-document persistence and conflict behavior unproved |
| rendering controls | viewport camera, display/settings controls, diagnostics, console, and capture | requested/active feature truth and per-viewport isolation still need evidence |
| developer operations | shader recook and mesh/texture diagnostic tools through editor host services | workspace/tool availability and cancellation remain product constraints |

```mermaid
flowchart LR
    Intent[Editor UI intent] --> Command[Typed generation-checked command]
    Command --> World[World or settings owner commits]
    World --> ReadModel[Rebuild immutable editor read model]
    ReadModel --> Viewport[Renderer product and diagnostics]
    Viewport --> UI[Present result and operation state]
```

The Editor should expose owner commands and read models, not mutate Renderer/RHI/native state directly. That makes undo, concurrency, and failure tractable at the cost of explicit command/result plumbing.

### UI Implementation Boundary

- Private `UIImplementation` owns panels, controllers, scene/history state and their composition. Private `ImGui/EditorImGuiContext` owns the ImGui context, stable ini filename, platform backend, DPI and window-message subscriptions. UI binds its logical shortcut handler through that owner; native message/repeat handling stays below composition. Normal teardown removes subscriptions, destroys viewport overlays/widgets and icons, then releases the platform backend and context. The context member is declared before its clients so constructor unwinding preserves their context lifetime; partial subscription setup rolls back. The public `UI` facade retains host services and viewport operations.
- [ViewportToolbar](../../../../../Engine/Editor/Private/Viewport/ViewportToolbar.h) owns camera, view-mode and Show controls. `ViewportPanel` composes layout, fresh input bounds, aspect-fitted image and progress; its request is the mode/extent publication boundary.
- [ViewportOverlay](../../../../../Engine/Editor/Public/Viewport/ViewportOverlay.h) is the optional Application-installed surface overlay. ViewportPanel destroys it before icon/ImGui teardown. See [capture composition](../../../CrossModule/PerformanceDiagnostics/ExternalCapture/ExecutionArchitecture.md#implemented-editor-viewport-composition) for clients and evidence.
- [Menu widgets](../../../../../Engine/Editor/Private/Util/UiMenuWidgets.cpp) own shared palette, font-relative metrics and semantic drawing. ImGui owns input, navigation and popup lifetime; panels retain labels, control state and commands. `MenuStyleScope` restores style on every exit.
- The [icon service](Icons.md) owns shared image resources and per-profile admission; clients borrow drawing and own layout. `UiIconWidgets.cpp` maps `EditorIcon` glyphs and owns badge/tooltip drawing with one `EditorIconSize`.

The Console starts collapsed. Its footer button, `~` outside text input, or `Window > Console` opens and focuses it; focused `Esc` closes it without discarding scrollback/history. These are source contracts, not interactive, DPI or shutdown acceptance.

Shader inspection and mesh preview source ownership was reconciled on 2026-10-25. Private `Shaders/ShaderArtifactInspection` owns selected-artifact loading, text cache invalidation, and missing-file messages; `UsedShadersPanel` retains shader selection, actions, and tabs. Private `Panels/Diagnostics/MeshPreview` owns bounds, projection, triangle traversal, and preview drawing; `UsedMeshesPanel` retains selection, preview controls, and the diagnostic-provider binding. These are source and build boundaries, not interactive or visual acceptance.

### Rendering Settings Ownership

The settings UI separates window orchestration, category presentation, and edit state:

- `SettingsPanel` owns the settings window, search, and restart action. It constrains the window to the current viewport so saved placement and resize cannot hide its controls. `RenderingSettingsPanel` orders rendering categories and presents the rendering-wide interaction/restart state; it does not draw setting rows.
- `Rendering*SettingsSection` files implement their named categories. Geometry owns the GBuffer and mesh-batching controls. Ray Tracing Scene owns general TLAS controls and composes the independently implemented PTLAS subgroup; PTLAS owns its enable-dependent controls.
- `RenderingSettingsUi` owns shared category/table/row widgets. `ExposureSettingsEditor` shares exposure controls between engine defaults and viewport overrides; category implementations do not duplicate those rows.
- `EngineRenderingSettingsController` owns the mutable interaction state, semantic setters, commit/refresh callbacks, and restart comparison. Application persistence and Renderer-owned value/active-state policy remain outside these UI implementations.

This source separation was reconciled on 2026-10-04; it does not establish runtime or usability acceptance.

## Workspace And Level Surface

| ID | Capability | State | Exact current coverage and limit | Evidence |
| --- | --- | --- | --- | --- |
| `ED-001` | Fixed workspace shell | Implemented path | ImGui main menu/title bar, scene outliner left, viewport/top controls center, scene inspector right, console dock, and floating utility panels. It is a purpose-built layout, not a general dockable workspace. | `S` |
| `ED-002` | Native window controls | Implemented path | Custom minimize, maximize/restore, drag, and close controls delegate to Platform; fullscreen hides these controls. | `S` |
| `ED-003` | Level open/save | Implemented path | File menu lists registered selected/ready levels, requests asynchronous activation, disables interaction during change, and Save All serializes the active level. No Save As, new level, or dirty-document prompt was found. | `S` |
| `ED-004` | Level progress/failure gating | Implemented path | UI interaction is disabled while LevelSession is changing; last load diagnostics and events are available to host/panels. Cancellation/recovery UX still requires execution evidence. | `S` |

## Scene Inspection And Editing

| ID | Capability | State | Exact current coverage and limit | Evidence |
| --- | --- | --- | --- | --- |
| `ED-005` | Scene outliner | Implemented path | Builds a read model of cameras, lights, meshes, sky, and material variants; supports object selection, type/visibility presentation, and active-camera choice. | `S` |
| `ED-006` | Camera inspector | Implemented path | Edits local transform, camera description/projection fields, visibility, and active camera through generation-checked world commands. | `S` |
| `ED-007` | Light inspector | Implemented path | Edits the current directional/point/spot/rect light description through one typed world command pair. Renderer limits remain elsewhere. | `S` |
| `ED-008` | Mesh inspector | Implemented path | Edits local transform and visibility; shows read-only mesh type/cooked ID and material slot/generation from the immutable scene model. Invalid material displays None. Identity text wraps in narrow panels. No mesh replacement, asset selector or material-property authoring. | `S` |
| `ED-009` | Sky inspector | Implemented path | Enables/disables and edits the current sky environment through world commands. It does not import/cook a new texture. | `S` |
| `ED-010` | Material variant selector | Implemented path | Lists cooked/imported named variants and changes the active variant. There is no variant authoring interface. | `S` |
| `ED-011` | Undo/redo | Implemented path | Ctrl+Z/Ctrl+Y execute inverse/forward world commands; history supports coalescing and is invalidated on world-generation change. Coverage is limited to the current seven world-edit payloads. | `S` |
| `ED-012` | Stale edit protection | Implemented path | Every edit submits with expected world generation and reports Accepted/Stale/Rejected. Inspector displays failures until dismissal or a successful edit; generation changes clear history without hiding a failure. Undo/redo shortcuts are disabled during level load. Packaged rejection journeys remain unqualified. | `S` |

## Viewport And Rendering Controls

| ID | Capability | State | Exact current coverage and limit | Evidence |
| --- | --- | --- | --- | --- |
| `ED-013` | Editor viewport | Implemented path | Publishes requested extent, renders Renderer scene-color products into ImGui, registers exact input bounds, and owns a separate navigable viewport camera. | `S` |
| `ED-014` | View modes | Implemented path | Top panel exposes the Renderer view-mode catalog, including lit and diagnostic modes. Exact 16-mode semantics/availability live in [Debug Views](../Renderer/Features/DebugViews/DebugViewsDesign.md). | `S` |
| `ED-015` | Exposure overrides | Implemented path | Per-viewport manual/automatic exposure controls override view presentation without changing scene state. | `S` |
| `ED-016` | Rendering settings | Implemented path | Separate searchable Display, Exposure, and Tone Mapping categories expose presentation/device controls, exposure/metering/adaptation, and the tone mapper respectively. Upscaling and Ray Reconstruction remain separate. Ray Tracing Scene keeps TLAS refit outside a PTLAS subgroup; the subgroup retains its enable toggle and hides partition tuning when disabled. PTLAS defaults off; saved choices are preserved. Vignette and chromatic aberration use dedicated passes with approved defaults; their widgets are deferred. Color grading and frame generation remain absent. Apply/restart behavior follows each setting owner. | `S` |
| `ED-017` | Restart service | Implemented path | Settings can request an editor restart when a process-level setting requires it. Recovery/session restoration is not a current documented capability. | `S` |
| `ED-018` | Viewport capture | Implemented path | File menu queues a viewport capture through Application/Renderer coordination. The console shows admission/readback/write failure, cancellation or the saved path. Format/path/content correctness remains candidate evidence. | `S` |

## Developer Tools

| ID | Capability | State | Exact current coverage and limit | Evidence |
| --- | --- | --- | --- | --- |
| `ED-019` | Editor console | Implemented path | Tilde shortcut opens a scoped console with parsing, history, autocomplete, output severity, built-in commands, and host-registered render/shader commands. | `S` |
| `ED-020` | Shader tools | Capability-gated | Lists registered/cooked generation rows; filters; shows source, reflection, disassembly, parameter match, and compile request artifacts; reloads generation; recooks all or selected shader. Requires workspace source/tools. | `S` |
| `ED-021` | Mesh diagnostics | Implemented path | Filterable used-mesh table with source/identity, CPU/GPU load and byte estimates, instancing summary, and local preview controls/geometry. Values are diagnostics, not profiler-certified truth. | `S` |
| `ED-022` | Texture diagnostics | Implemented path | Filterable used-texture table with source/path, format/dimensions/mips, residency and byte estimates, selection inspector, and preview. | `S` |
| `ED-023` | Memory/renderer diagnostics | Implemented path | Host wires Renderer memory/mesh/texture/shader providers to UI. External capture and performance authority remain outside the editor. | `S` |
| `ED-024` | Icon/theme assets | Capability-gated | Custom palette/theme, Font Awesome Solid glyphs and shared image-icon service/catalog; CMake validates required font and declared artwork. See [Icons](Icons.md). | `S` |

## Vertical Edit Trace

World publishes immutable read view/change sequence -> `EditorSceneModelBuilder` creates presentation rows -> user selects an object -> inspector creates forward and inverse typed commands -> transaction history submits with current world generation -> GameWorld queues and commits accepted edits -> change journal advances -> editor rebuilds affected presentation -> Renderer receives the resulting structural/dynamic publication. Undo runs the stored inverse through the same path.

## Product Journey Changes

The PD-1 route includes Help > Controls, File > Quit, level-operation status and Reset rendering defaults. Transient status is viewport-bounded and drawn after the workspace; a new failure receives focus without taking it every frame. Dedicated vignette/chromatic aberration passes retain approved defaults; effect widgets are deferred. Failed settings writes retain accepted state and show the error; reopening Settings preserves restart-required state. The existing shared settings capture/apply owner remains authoritative. [Candidate checks](../../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) record which journeys passed.

## `FCR-PROD-04` Editor Contract

Sparkle Editor is a **required shipped product** for `v0.1.0`, currently implemented by `ShowcaseEditor`. Qualify its package/workspace/dependency closure and the seven release-map workflows on a clean standard-user machine; intentional Empty startup needs no example download. Game remains structurally isolated from Editor code and tools. [Application](../Application/ApplicationCapabilities.md#product-contract-boundary) owns hosting; this section owns the feature-local verdict contract.

**Proof allocation:** source editing/failures belong to PD-1, independent source reproduction to PD-2, and installed journeys/component closure to PD-3. PD-4 reconciles their exact candidate. [Product delivery progress](../../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) records existing results; no stage must wait for an archive to test source-only behavior.

| ID | Binary acceptance criterion | Proof phase |
| --- | --- | --- |
| `AC-PROD04-01` | A standard user can launch packaged Sparkle Editor without a source checkout or undeclared SDK, start Empty, and open each selected/synced release map with its expected identity, scene/camera, geometry/material bindings and presented viewport. Crashes, blank frames and wrong/missing content fail; lighting quality remains in renderer/release gates. A source adopter can build `ShowcaseEditor` through the documented toolchain. Workspace/helpers are explicit and owned operations settle within the frozen Editor budget. | PD-1 source journey; PD-2 adopter; PD-3 installed journey |
| `AC-PROD04-02` | For one admitted authored level, open -> inspect -> supported typed edit -> undo -> redo -> explicit Save All -> reload produces the expected world/source result; only explicit Save All may mutate the selected `.level` source. | PD-1 disposable authored level; PD-3 selected-content store |
| `AC-PROD04-03` | Missing source/tool/shader/cooked content, stale world generation, rejected edit, failed save, cancelled recook/capture, and restart-required setting remain distinguishable and preserve the previous accepted world/source/generation. | PD-1 source faults; PD-3 packaged equivalents |
| `AC-PROD04-04` | Game component/import inspection contains no Editor linkage, UI, shader-recook/capture operation or Editor-only dependency. The separate Editor component is present in the three-product bundle and its declared helpers are verified against the manifest. | PD-1 link boundary; PD-3 component/helper inventory |

| ID | Cause/injection, safe result, and affected criterion |
| --- | --- |
| `FM-PROD04-01` | Omit a required workspace/tool input. Configure/start/operation fails with the missing owner and recovery; it never degrades to a consumer promise or reports success (`AC-PROD04-01`, `AC-PROD04-03`). |
| `FM-PROD04-02` | Change world generation or force an unsupported/stale edit. The command is rejected visibly, history stays coherent, and no optimistic UI state becomes source truth (`AC-PROD04-02`, `AC-PROD04-03`). |
| `FM-PROD04-03` | Deny or fault the selected level save. The existing source bytes retain their hash and the failure remains actionable; no temp/partial file is accepted (`AC-PROD04-02`, `AC-PROD04-03`). |
| `FM-PROD04-04` | Inspect the Game component for Editor dependencies/UI/imports and the Editor component for missing declared inputs. Editor leakage into Game or omission from the bundle fails `AC-PROD04-04`. |

| Check | Claims falsified | Smallest route and oracle |
| --- | --- | --- |
| `CHK-PROD04-01` | `AC-PROD04-01`, `AC-PROD04-02`, `AC-PROD04-03`; `FM-PROD04-01`–`03` | Focused supported-toolchain Editor build, then clean-machine packaged Empty/all-seven-map activation and documented open/edit/undo/redo/save/reload/exit on a disposable source copy with each named fault. Expected scene/camera and bindings, presented output, world/source hashes and operation results are the oracle. Activation alone cannot pass a crash, blank viewport or wrong/missing content; lighting quality stays in renderer/release gates. |
| `CHK-PROD04-02` | `AC-PROD04-04`; `FM-PROD04-04` | Independently inventory the verified three-product stage: require the declared Editor component/helpers and inspect Game (`ShowcaseGame`) imports against the Editor dependency denylist. Component presence and zero Editor linkage in Game are required. |

Candidate results belong in `FCR-PROD-04`; no usability, build, runtime, save, or package result was produced by this contract pass.

## Explicit Non-Capabilities And Risks

- No general content browser, drag-and-drop import, material graph/editor, mesh authoring, entity creation/deletion UI, arbitrary component inspector, prefab editor, animation timeline, sequencer, plugin UI, multi-document tabs, or collaborative editing was found.
- Save does not imply dirty tracking, autosave, crash recovery, source control integration, or Save As.
- Renderer selectors visible in UI still need a finite release disposition; a control existing does not make its backend/feature combination shippable.
- `SparkleEditor` exists in all configured workspace builds, while Game products avoid linking it. Only final package inspection proves erasure.
