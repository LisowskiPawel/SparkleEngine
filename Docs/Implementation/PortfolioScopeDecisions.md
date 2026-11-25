# SparkleEngine module scope decisions

[Goals](../Goals.md) | [References](../References.md) | [Roadmap](../Roadmap.md)

**Selected: Option C, a reduced engine suite with distinct Launcher, Editor and Game. Real-time path tracing is the only supported lighting path. The Reference path tracer and all of its dedicated infrastructure are to be removed.** You are the sole maintainer/developer. Reviewers launch the prebuilt portfolio and read its source; they are not expected to iterate code, shaders or content. This is the target scope, not a claim that implementation cleanup is complete.

**Yes** means retain the capability, understand its full data flow and lifetime, and refine or reimplement it to the required standard. **No** means retire the capability and its otherwise unused callers, state, settings, build membership, scripts, dependencies and documentation. An undecided item stays undecided. The current recommendations below reflect this audience split; your recorded answers remain authoritative and are not changed by an editorial update.

A `-` in `Your answer` means no decision has been recorded. You can answer a section at a time, using the item IDs. An asterisk on a suggestion means another decision or content requirement must be resolved first. Coupled choices are listed near the end.

There are 202 decision rows: the original 129 plus 73 more detailed questions added on 25 November. Original answers remain recorded. BU-05/BU-06, TP-01/TP-02 and DO-01/DO-02 remain unanswered, and every new answer is blank. The Stanford non-PLY condition and checks on accepted conditional cuts remain unresolved acceptance work. TX-03 was implemented and verified during this scope review; the new questions are planning only. Use stable IDs for further answers; a recommendation never silently changes an earlier answer.

Correctness, safe lifetime, useful errors and protection of accepted outputs remain requirements of every retained workflow. They are not optional features in this checklist. Existing working code also needs your review and understanding; a Yes is not approval of its current architecture.

## How these recommendations follow Goals and References

Decision authority is [Goals: Product scope](../Goals.md#product-scope) and your recorded answers. [References](../References.md) supplies engineering precedents and the archived role evidence; it is not a feature checklist. A capability appearing in a research paper, NVIDIA sample or job description does not make it an engine requirement. Later neural/model work and case narratives remain separate from this cleanup.

| Decision test | Source and application here |
| --- | --- |
| Does this demonstrate the work you want to own? | [Target persona](../Goals.md#target-persona), [rendering/GPU systems](../Goals.md#rendering-and-gpu-systems), [role sources](../References.md#role-sources): prioritize explicit APIs, real-time transport, measured integration, shader work and difficult debugging. A general installer, project framework or elaborate operation-history UI provides weaker evidence for this target. |
| Does a named user still need it? | [Product scope](../Goals.md#product-scope), [review and transfer](../Goals.md#review-and-transfer): preserve your fast build/cook/debug workflow and the reviewer's offline Launcher/Editor/Game experience. Removing the developer GUI is an explicit reopening, not an inferred consequence of reviewer simplicity. |
| Does removal reduce an obligation or just move its code? | [North Star](../Goals.md#north-star), [integration precedents](../References.md#integration-precedents), [code style](../References.md#code-style): remove a supported mode, duplicate authority, state machine or unused producer/consumer chain. Count new replacement code and remaining dependencies. A renamed coordinator is not a saving. |
| Is a library already responsible for the mechanism? | [C++ engineering](../Goals.md#software-and-analysis), [integration precedents](../References.md#integration-precedents): use CMake target dependencies, Qt controls and compiler/library facilities when they cover the real contract. You still own integration, failure behavior and lifetime; do not replace working dependencies with handwritten equivalents merely to claim authorship. |
| Would the cut damage evidence or correctness? | [Whole-system performance and difficult debugging](../Goals.md#software-and-analysis), [NVIDIA production studies](../References.md#nvidia-production-studies), [tools and APIs](../References.md#tools-and-apis): preserve symbols/source correlation, breadcrumbs, bounded publication/settlement, valid material/animation data and useful controlled comparisons. No line-count target overrides these. |
| Is an execution simplification actually cheaper on retained workloads? | [Hardware/concurrency](../Goals.md#rendering-and-gpu-systems), [whole-system performance](../Goals.md#software-and-analysis): GF-06/GF-08 and similar changes need a defined retained workload, baseline, memory/time limits and checked output. Removing parallelism without that comparison is not an established improvement. |
| Does the result remain easy to inspect and maintain? | [Make ownership clear](../Goals.md#north-star), [code style](../References.md#code-style), [documentation examples](../References.md#documentation-examples): use descriptive responsibility names, explicit entry points and one data-flow explanation. Keep module/lifetime boundaries that aid reasoning, even in small code. |

This leads to three outcomes: **retain for a named portfolio/development requirement**, **remove an unnecessary capability**, or **replace a mechanism only after proving the narrower contract**. The detailed tables name the item, removal boundary, reason and recommendation separately. Your answer stays blank for every new question. A conditional replacement is not permission to weaken correctness or delete the old path before its replacement works.

The local application of these precedents is a recommendation, not a claim that NVIDIA or AMD requires a particular Sparkle class structure. The [NVRHI guide](https://raw.githubusercontent.com/NVIDIA-RTX/NVRHI/main/doc/ProgrammingGuide.md) is a useful resource-lifetime precedent; it does not replace the selected owned RHI. [Qt item views](https://doc.qt.io/qt-6.11/qlistview.html) provide list/icon presentation; whether they cover a specific Launcher wrapper still needs a local comparison. [CMake target dependencies](https://cmake.org/cmake/help/v3.30/manual/cmake-buildsystem.7.html) motivate one native-build authority, not blind deletion of SDK/deployment invalidation. [NVIDIA's shader-debugging study](https://developer.nvidia.com/blog/harness-powerful-shader-insights-using-shader-debug-info-with-nvidia-nsight-graphics/) supports retaining usable source correlation while simplifying surrounding UI. These primary pages were checked on 25 November; this was not a new vacancy-status or qualification survey.

## Where the code footprint actually is

Snapshot: 25 November 2026, working tree after KTX removal. Counts include tracked, present first-party C/C++ headers/implementation and build/automation scripts, including blank/comment lines. They exclude downloaded vendor trees, generated output, notices, documentation and raw asset data. Shader assets and maintained patches are reported separately. These are containing scopes, not promised deletion amounts or measurements of code quality, build time or runtime cost. File-level inventory is recorded locally in `build/validation/portfolio-footprint-20261125/authored-files.json`.

| Owner | Files | Physical lines | What deserves a closer decision |
| --- | ---: | ---: | --- |
| Launcher application | 205 | 19,662 | GUI 8,101; native build orchestration 3,835; clean/maintenance 1,152; scene operations 1,184. Setup, history and alternate workflows need separate choices. |
| GameFramework | 219 | 12,223 | World implementation 4,501; asset implementation 2,516; level implementation 2,420. Inspect representation, load concurrency and runtime animation construction separately. |
| Editor | 152 | 12,005 | Asset inspection 1,314 and shader UI 779 are already retirement candidates. Scene UI 2,072, viewport 1,994 and custom widgets 872 contain retained work and optional presentation. |
| ShaderCompiler | 104 | 7,938 | Compilation/publication/debugging dominate. The optional analysis folder is only 43 lines; removing its label does not remove the expensive part of this module. |
| Core | 127 | 7,278 | Process implementation 1,189; paths 969; files 907, plus their public contracts. Much serves retained tools, publication or a relocatable reviewer bundle. |
| SourceImporters | 89 | 5,900 | glTF implementation 3,245; shared Assimp adapters 1,191; FBX adapters 740; PLY adapters only 37. PLY removal alone will not retire Assimp while FBX remains. |
| Cooking libraries/tools together | 105 | 5,810 | TextureCooker 2,019; AssetCooker 1,550; SceneCooker 1,430; MaterialCooker 434; MeshCooker 377. Small library boundaries are not the dominant cost. |
| Application | 60 | 5,373 | Reference-specific Editor code 949; GPU-crash persistence/input handling 1,482; ordinary image capture 744. These are different responsibilities with different removal rules. |
| CMake root | 50 | 3,207 | Packaging 954; dependency preparation 596; artifact policy 492; GPU diagnostics 442. The current GitHub workflow is just 41 additional lines of source/whitespace checks. |
| Platform | 25 | 1,783 | Input implementation 1,036; window implementation 494. Most implements the retained Editor/Game interaction contract. |
| ToolSupport | 20 | 1,428 | Project/level catalog implementation alone is 700. Its content-pack model matters more than its small progress helpers. |
| Showcase source/conversion | 15 | 762 | CornellBox and LPSHead converters, small product entry points and composition. Scene data and asset bytes are outside this count. |
| Renderer, for comparison | 619 | 36,806 | Existing rendering decisions remain authoritative. Do not cut retained rendering features indirectly through host/tool simplification. |
| RHI, for comparison | 372 | 37,814 | Both APIs, multiple queues, shader-target enumeration and breadcrumbs remain protected. |

Across the counted tree there are 159,223 physical lines. Outside Renderer/RHI there are 84,603, about 53%; this includes graphics tooling, not just unrelated infrastructure. Shader asset source contributes another 7,544 lines, and maintained patch files another 1,071 diff lines. The Launcher root composition outside its application adds 34 lines. Do not count cached third-party source or binary assets as code you authored.

The three largest non-Renderer/non-RHI application modules are Launcher, GameFramework and Editor. They should be reviewed before shaving small math helpers or merging the 377-line MeshCooker. The already selected Editor asset/shader panels and the two host/Editor Reference folders contain 3,275 lines together; their shared callers still require classification, so this is not a guaranteed net saving.

| Review order | Decisions to make | Why this comes first |
| --- | --- | --- |
| 1 | LA-16 through LA-25; existing LA-11/LA-13/LA-15 | Largest non-rendering module; remove extra workflow/UI obligations while preserving your Launcher iteration controls. |
| 2 | GF-08/GF-13/GF-16; existing GF-02/GF-06 | Review cross-pool loading, runtime offline animation construction and read-view copying independently of retained animation and read lifetime. |
| 3 | ED-17 through ED-24; existing ED-12/ED-13/ED-14 | Bound presentation and persistence. Keep useful live inspection without accidentally promising a full scene-authoring product. |
| 4 | BU-05/BU-07 through BU-12, TS-03/TS-04 | Product import, package contents, feature combinations and content-pack generality cross several owners. |
| 5 | CP-05 through CP-10, IM-06 through IM-09, TX-05 through TX-08 | Narrow tool interfaces and admitted inputs after proving what the retained scenes and debuggers actually use. |

`No*` means a narrower replacement or a retained-content/cost check is needed before removal. It does not mean keeping a second implementation indefinitely. A rewrite that merely moves lines is not a scope saving. The optional RV questions near the end explicitly identify larger changes to earlier answers; all other new questions aim to refine their accepted scope.

## Who the engine serves

| Person | Intended workflow | What the repository must provide |
| --- | --- | --- |
| You, sole maintainer/developer | Use Launcher to prepare/sync dependencies and content, configure the Rider workflow, build DLLs, cook shaders/assets, launch/debug, clean outputs and package a result | Efficient iteration, explicit state/lifetime, useful errors and small reusable implementations you understand. No contributor onboarding, plugin SDK or general engine-user workflow is required. |
| Reviewer exploring the portfolio | Open Launcher, choose a supplied map and start Editor or Game; move around, try supported options/effects and inspect scene/render signals | A complete prebuilt runtime, clear starting configuration and controls, usable performance and honest feature/hardware limits. No build, cook, source acquisition or shader compilation is required. |
| Reviewer inspecting engineering | Read the matching source, follow application entry points and trace a selected asset/material/frame through the modules | A concise architecture/data-flow map, navigable code and explicit ownership/tradeoffs. Reading and questioning the design does not require installing Rider or becoming another developer. |

Keep reviewer Quick Start focused on scene selection, Editor/Game launch and the supported runtime options. In Editor, retain the approved camera, light, transform, visibility, material-variant, exposure, reconstruction and debug-view controls; reviewer simplicity must not reduce it to a video player. Ordinary runtime changes use shipped cooked shaders/data. Preserve supplied demo defaults so an inspection session can return to a known starting state through the supported reload/reset workflow.

Keep your developer workflow in Launcher with its agreed preparation/build/cook/clean controls. In a prebuilt inspection bundle, source-only actions can be absent or clearly unavailable; missing developer tools must not make a runnable scene appear unready. Derive availability from actual source/product/tool capabilities, without accounts, permissions or a new user-role framework. Share the existing operation owners and runtime contracts between the two workflows.

A retained feature earns its place by serving your iteration/understanding, the reviewer's exploration/code inspection, or the correctness/delivery of those workflows. Multiple developers are not needed to justify useful module boundaries, DLL contracts, symbols, breadcrumbs, PCHs or reproducibility. General consumer extensibility, stable third-party engine APIs, broad setup matrices and unneeded workflows do not follow from being a portfolio engine.

## Launcher

Owner: [Tools/Launcher/SparkleLauncher](../../Tools/Launcher/SparkleLauncher). Observed scope: 205 source/build files, 19,662 physical lines. Launcher remains a distinct application.

Reviewer Quick Start is its public entry point. Your source-workspace Launcher also owns the retained developer iteration actions; those actions are not reviewer prerequisites.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| LA-01 | Reviewer Quick Start: select a supplied scene, see runtime readiness and launch Editor or Game with explicit settings; also usable during your iteration. | Yes | Yes |
| LA-02 | Scene thumbnails/previews and Qt scene browsing that make the prepared demonstrations easy to find. | Yes | Yes |
| LA-03 | Maintainer Launcher actions to build Editor, Game and cooking tools; reviewers run the prepared products. | Yes | Yes |
| LA-04 | Maintainer workspace/toolchain readiness and setup for Rider only. Remove the Visual Studio IDE choice/open action and a redundant IDE selector; retain required compiler/SDK configuration. | Yes, Rider only | Yes, Rider only |
| LA-05 | Maintainer dependency acquisition/preparation controls. Missing source dependencies do not block a prebuilt reviewer run. | Yes | Yes |
| LA-06 | Maintainer scene-content download/sync. Required reviewer content is already in the inspection bundle. | Yes | Yes |
| LA-07 | Maintainer Launcher actions to cook selected content and shaders; no reviewer cook step. | Yes | Yes |
| LA-08 | Retained advanced build/cook options useful to your iteration, bounded to supported operations and the separately decided cooker scope. | Yes | Yes |
| LA-09 | Maintainer Clean page, reduced to the four ordered scopes under LA-10; outside reviewer Quick Start. | Yes | Yes |
| LA-10 | Simplified clean scopes in this order: All; Content; Native code compilation artifacts; Third-party code compilation artifacts. | Yes | Yes, simplify |
| LA-11 | Rich CLI dashboard/catalog presentation beyond help, errors and retained command execution. Remove this presentation; retain required previews of destructive effects. Owner: `Private/Shell`. | No | No |
| LA-12 | Noninteractive CLI access for your automation and retained Editor compile action, sharing the Launcher operation owners. | Yes | Yes |
| LA-13 | Launcher bootstrap/live-copy and rebuild/restart behavior. A No may require closing Launcher before replacing its executable. | No | No |
| LA-14 | Optional GPU debugger/capture configuration and launch for your diagnosis or specialist inspection, including Editor crash-debugging mode. No debugger installation is required for an ordinary reviewer run. | Yes | Yes |
| LA-15 | Launcher post-crash incident monitoring, polling, presentation, automatic inspection/recovery and incident-specific actions. Remove; launch-mode support stays under LA-14/AP-05. | No | No |

Retained operations still need coherent progress, output, cancellation where supported, failure reporting and close behavior. Removing their UI does not remove the shared operation owners automatically.

Recorded clean scope order:

1. **All**
2. **Content**
3. **Native code compilation artifacts**
4. **Third-party code compilation artifacts**

All means the union of these generated-output categories. For now, Content is interpreted as generated/cooked content; original assets, downloaded sources, captures and user state are preserved. Exact output membership must be mapped before implementation. Compiled third-party outputs are distinct from downloaded dependency source. No cleanup is being executed by recording these decisions.

LA-11 is removed from the target scope. It is the text-mode Launcher dashboard listing workspace/profile information, grouped operations and readiness, recent activity and job output. CLI command execution, help and useful errors remain under LA-12.

LA-15 is removed from the target scope. Launcher may configure/start the GPU debugging tools and launch Editor in the requested crash-debugging mode, then leave crash investigation to the engine's necessary SDK integration and external Aftermath/Nsight tools. Do not retain a Launcher incident observer, crash-state polling, incident panels/catalogs, automatic post-crash tool invocation, crash-specific recovery or an incident-report workflow. Normal child-process completion/error reporting remains; this decision does not remove ordinary process supervision.

Required engine-side SDK initialization, breadcrumbs/markers, native fault collection, crash-data callbacks/output, shader debug information and bounded collection-completion waits remain under AP-05/RH-09. Their ownership is separate from Launcher reacting to a crash. Remove additional custom post-crash orchestration under AP-07; do not delete required diagnostic collection merely because it occurs after a fault. The [explicit keep/remove boundary](#gpu-crash-diagnostics-preservation-and-removal) governs all such cuts.

### Launcher: additional decisions

The 8,101-line GUI contains required scene browsing as well as developer setup, models, styling and activity presentation. The 494-line `Private/Build/State` area implements independent build-freshness tracking. These totals overlap the module total; they are not additional savings.

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| LA-16 | **Native compiler component installer** | Remove `HostToolInstaller.*`, installation actions and GUI/planner branches. Install your chosen compiler once yourself; keep detection and useful missing-tool errors. | No | [product scope](../Goals.md#product-scope): OS tool installation is outside the portfolio; detection still serves your iteration. | - |
| LA-17 | **Multiple Windows compiler families: MSVC and clang-cl** | Choose one proven MSVC or clang-cl route; retire the other compiler-selection, detection and reset branches. Rider-only does not itself select a compiler. | No* | [product scope](../Goals.md#product-scope): one maintainer needs one proven toolchain, not two permanent qualification branches. | - |
| LA-18 | **Multiple CMake generator families: Visual Studio/MSBuild and Ninja** | Keep the generator/project format proven with Rider and standalone Launcher. Retire unused Ninja/other-generator choices; retain MSBuild/solution support if that is the chosen route. | No* | [product scope](../Goals.md#product-scope): one proven Rider route reduces configuration state; project support remains when required. | - |
| LA-19 | **Launcher build-file freshness scanner and signatures** | Prefer CMake regeneration/build dependencies plus minimum cache/configuration identity. Prove added/removed sources, changed recipes and toolchain changes are detected before retiring `Build/State` logic. | No* | [integration precedents](../References.md#integration-precedents): prefer the native build authority; preserve invalidation before removing duplicate tracking. | - |
| LA-20 | **Automatic toolchain-change artifact reset** | Use an explicit incompatible-cache error and the approved native Clean scope. Retain automatic reset only for a demonstrated single-toolchain iteration need. Source caches are preserved. | No* | [ownership](../Goals.md#north-star): explicit clean ownership can replace extra reset state after toolchain narrowing. | - |
| LA-21 | **Multi-run operation-history browser** | Show current/last operation, bounded output, failure and the existing log path. Remove the multi-run browser; preserve progress and cancellation. | No | [product scope](../Goals.md#product-scope): current progress/failure is useful; a history browser adds presentation maintenance. | - |
| LA-22 | **Concurrent independent build/cook/sync/clean operations** | Serialize build/cook/sync/clean through one responsive operation owner. Clearly reject or queue another request. A launched Editor/Game and bounded parallelism within a build/cook remain independent. | No* | [ownership](../Goals.md#north-star): serialize mutations to reduce operation lifetimes; preserve parallel work inside each operation. | - |
| LA-23 | **Expanded compiler/SDK/dependency setup dashboard** | Keep concise developer readiness, explicit SDK/tool paths and retained prepare/build actions. Remove extra detail cards and installation/repair branches. Quick Start still reports runtime readiness. | No | [reviewer usability](../Goals.md#review-and-transfer): keep usable readiness and setup; remove a general environment-management interface. | - |
| LA-24 | **Per-dependency code-sync controls** | Sync the enabled pinned set as one operation. Keep selected scene-content sync, incremental cache reuse and dependency-specific errors. | No | [product scope](../Goals.md#product-scope): the enabled pinned set is your supported development unit; cache reuse remains. | - |
| LA-25 | **Custom Qt styling and general widget-wrapper layer** | Prefer standard Qt controls and one compact owned theme, retaining scene previews. Remove wrappers only when behavior has a simpler owner; preserve useful accessibility, focus and error behavior. | No* | [integration precedents](../References.md#integration-precedents): reuse Qt behavior while keeping a compact polished interface you can own. | - |
| LA-26 | **Scene-ready child-process launch handshake** | No reports process creation only, losing startup/map-failure detection. The Core readiness protocol is unrelated to post-crash monitoring. | Yes | [reviewer usability](../Goals.md#review-and-transfer): successful process creation alone does not demonstrate a usable map. | - |
| LA-27 | **Explicit workspace/tool/SDK path overrides** | Keep recovery for your own relocated checkouts and SDK installations through one supported path contract, without a general IDE/project framework. | Yes | [product scope](../Goals.md#product-scope): practical solo-developer recovery is useful without a generic setup matrix. | - |
| LA-28 | **Non-destructive workflow dry-run reports** | Remove optional non-destructive preview presentation and its CLI switch only where unused. Keep clean effects preview, internal planning required by execution/readiness, help and errors. | No* | [product scope](../Goals.md#product-scope): remove unused presentation, while retaining required clean previews and execution planning. | - |

Sources: [host-tool installation](../../Tools/Launcher/SparkleLauncher/Private/Build/Toolchains/HostToolInstaller.cpp), [freshness scanning](../../Tools/Launcher/SparkleLauncher/Private/Build/State/BuildFreshnessSignature.cpp), [activity UI](../../Tools/Launcher/SparkleLauncher/Private/Gui/Shell/LauncherActivityPanel.cpp), [operation lifetime](../../Tools/Launcher/SparkleLauncher/Private/Gui/Operations/LauncherOperationService.cpp). LA-22 changes admission, completion/cancellation and close together; deleting a worker without following its callbacks is not valid simplification.

## Editor

Owner: [Engine/Editor](../../Engine/Editor). Observed scope: 152 source/build files, 12,005 physical lines. Editor remains distinct from Game.

Reviewers explore the scene, supported edits, rendering options/effects and signal views. You also use it to debug and iterate. Keep those useful controls without building a general content-authoring product.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| ED-01 | Reviewer-usable camera, rendering settings/effect controls and render/debug views, also used during your development. | Yes | Yes |
| ED-02 | Scene outliner, selection and inspection of existing scene objects. | Yes | Yes |
| ED-03 | Live mesh transform editing. A No keeps imported transforms and camera navigation. | Yes | Yes |
| ED-04 | Live visibility editing for existing objects. | Yes | Yes |
| ED-05 | Editing camera properties and choosing an active camera. | Yes | Yes |
| ED-06 | Editing existing lights and sky/environment settings. | Yes | Yes |
| ED-07 | Switching imported material variants. This depends on GF-03. | Yes | Yes |
| ED-08 | Undo/redo and interaction coalescing for retained edits. A No means edits have no transaction history. | Yes | Yes |
| ED-09 | Save supported scene properties for your iteration or optional inspection-session changes. Preserve supplied demo defaults; no general content-authoring system is added. | Yes | Yes |
| ED-10 | Switching levels while Editor remains open. A No can use close/relaunch per scene. This follows GF-04. | Yes | Yes |
| ED-11 | Retained console/history/runtime CVars for your debugging and optional specialist inspection; ordinary reviewer controls do not require console commands. | Yes | Yes |
| ED-12 | Used-mesh diagnostics and interactive mesh preview. A No removes the panel and its unused diagnostic producers. | No | No |
| ED-13 | Used-texture diagnostics and texture inspection. A No still permits native GPU capture inspection. | No | No |
| ED-14 | Shader list/export inspection UI, beyond compile/reload and compiler error reporting. | No | No |
| ED-15 | Optional native capture launch controls for your debugging or a reviewer with the tools installed; normal Editor launch remains independent. | Yes | Yes |
| ED-16 | Persistent viewport preferences and presentation state between runs. | Yes | Yes |

Editor suggestions accepted in full: ED-01 through ED-11 and ED-15/ED-16 Yes; ED-12 through ED-14 No. GF-03, GF-04 and RH-05/RH-06 are now retained too. CO-01 is now retained, completing the linked console/CVar scope decision for Editor and Application.

The existing [edit command set](../../Engine/GameFramework/Public/World/WorldEditCommand.h) is bounded: active camera, transforms, camera/light descriptions, visibility, sky and material variants. General object creation, duplication, reparenting or asset editing is not silently added by choosing Option C.

ED-09 also needs an explicit list of saved fields: a live edit being supported does not establish that it survives Save All. Keep that distinction visible when choosing the final Editor contract.

### Editor: additional decisions

Existing ED-12/ED-13/ED-14 retire mesh, texture and shader-inspection interfaces. Shader compile/reload, useful compiler failures and the console remain. Custom widgets, style, icons and workspace presentation deserve separate choices.

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| ED-17 | **Custom Editor title bar and native hit testing** | Use a normal decorated window. Remove custom drawing and otherwise unused Platform non-client hit testing; retain windowed/borderless presentation, DPI and focus behavior. | No* | [product scope](../Goals.md#product-scope): bespoke desktop chrome is weaker portfolio evidence than rendering and inspection. | - |
| ED-18 | **Generated RGBA icon catalogs and Editor icon texture service** | Prefer already available font glyphs or a smaller fixed set. Retire generation/upload only when the replacement covers retained controls; ordinary ImGui rendering remains. | No* | [integration precedents](../References.md#integration-precedents): reuse a smaller existing presentation path if it covers the retained interface. | - |
| ED-19 | **Per-property inspector text search** | Keep the outliner and clear fixed property sections. Lose per-property search; this is a small convenience cut. | No | [product scope](../Goals.md#product-scope): a small optional convenience beyond the retained outliner and selected property controls. | - |
| ED-20 | **Console command/CVar autocompletion** | Remove `CompleteConsoleLine`/`CompleteConsoleCVarName` and completion UI only. Keep input parsing, execution, history, CVar get/set, compile/reload, help and errors under ED-11; a small cut. | No | [product scope](../Goals.md#product-scope): keep execution/history, reduce optional convenience code only if you accept the loss. | - |
| ED-21 | **Undo/redo command pairs and drag coalescing** | No weakens undo/redo selected in ED-08. Keep the bounded command set without building a generic transaction framework. | Yes | [reviewer usability](../Goals.md#review-and-transfer): reversible edits make the selected Editor inspection usable. | - |
| ED-22 | **Save lighting, sky and active-camera description** | These are the fields currently captured by `CaptureLevelWorldState`. Recommendation preserves ED-09's existing narrow responsibility. | Yes | [product scope](../Goals.md#product-scope): preserve the selected narrow authoring workflow and describe its real saved fields. | - |
| ED-23 | **Persistent per-object transform/visibility/material overrides** | Keep approved edits live for inspection but session-only, with reload/reset. Full object persistence is a scope expansion, not implemented by today's Save All. | No | [product scope](../Goals.md#product-scope): broad scene authoring is outside cleanup; this would add an unimplemented obligation. | - |
| ED-24 | **Writing scene edits into the reviewer bundle** | Keep maintainer save in a writable source workspace; reviewer edits stay in-session and reload returns to supplied defaults. Avoid a second user-content storage system. | No | [reviewer usability](../Goals.md#review-and-transfer): preserve supplied defaults and offline inspection without a second content-authoring system. | - |

The [save projection](../../Engine/GameFramework/Private/Level/LevelWorldProjection.cpp) captures light descriptions, sky and the active camera description into the existing level document. It does not serialize every object edit or establish persistence of every imported camera transform/selection. The [writer](../../Engine/GameFramework/Private/Level/Parsing/LevelDocumentFormat.cpp) supports fixed level/camera/sky/lighting/scene-assets sections. Rename or explain Save All according to the selected fields. ED-23 limits new work; it is not current removable code.

[Undo history](../../Engine/Editor/Private/Scene/Transactions/EditorTransactionHistory.h) is small and directly serves a retained interaction. [Title-bar controls](../../Engine/Editor/Private/Workspace/TitleBarWindowControls.cpp) and [icon generation](../../Engine/Editor/CMake/EditorIconAssets.cmake) are distinct presentation costs. Decorative cuts must preserve focus-loss cancellation and correct viewport input.

## Application hosts

Owner: [Engine/Application](../../Engine/Application). Observed scope: 60 source/build files, 5,373 physical lines. Keep separate Editor/Game composition and a cooked-only Game runtime.

Shared host composition makes Editor/Game run reliably for reviewers and connects your optional development operations. Runtime launch must not depend on a developer workspace.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| AP-01 | Maintainer Editor shader compile/reload with accepted-output preservation. The reviewer bundle uses cooked shaders and does not require this development action. | Yes | Yes |
| AP-02 | Viewport screenshot/image export for your evidence and optional reviewer captures, independent of the removed Reference artifact pipeline. | Yes | Yes |
| AP-03 | Reference path tracer accumulation, checkpoint/resume and dedicated raw/reference artifact export. Remove completely; no replacement offline renderer. | No | No |
| AP-04 | Persisting user rendering settings across runs, separate from authored scene settings. | Yes | Yes |
| AP-05 | Opt-in Editor GPU-crash/Aftermath mode with native breadcrumbs/markers, crash-data collection and persistence, shader/debug identity, bounded completion and required deployment. Excludes extra incident-management UI/workflows. | Yes, protected | Yes, protected |
| AP-06 | Developer command-line CVar overrides in the application hosts. A No leaves explicit launch settings and retained Editor controls. | Yes | Yes |
| AP-07 | Extra post-crash incident UI/catalog/reporting and automatic analysis/recovery. This removal excludes breadcrumb/marker sidecars, matched debug metadata, essential dump writing and bounded native collection completion. | No | No |

Startup, shutdown, process/task settlement, capture completion and lifetime ordering remain work to own. Three task resources or several coordinators should be justified by their actual lifetimes, rather than removed merely because they look similar.

### Application hosts: additional decisions

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| AP-08 | **Ordinary BMP viewport screenshots** | No reopens AP-02. Keep one ordinary capture route; Reference EXR/statistics/checkpoint writers are unnecessary for screenshots. | Yes | [reviewer usability](../Goals.md#review-and-transfer): one usable evidence/export path earns scope; Reference artifacts do not follow from it. | - |
| AP-09 | **Explicit Editor restart for startup-only settings** | No means close/relaunch manually. Trace setting/capture callers before removing `Restart` or Core process relaunch; LA-13 removes Launcher self-rebuild/restart only. | Yes | [debugging evidence](../References.md#nvidia-production-studies): required capture startup changes are distinct from Launcher self-rebuilding. | - |
| AP-10 | **Persisted adapter/startup preferences** | Keep effect persistence under AP-04; choose startup options explicitly at launch. This avoids an earlier inspection silently selecting a different startup path. | No | [reviewer usability](../Goals.md#review-and-transfer): explicit startup choices keep reviewer behavior predictable while effect settings still persist. | - |
| AP-11 | **CPU render-thread mode and frame-lead diagnostic controls** | A fixed production execution policy is possible, but first identify the retained performance/debugging consumers of `r.ThreadedRenderer` and `r.RenderPipelineDepth`. Do not confuse CPU frame lead with retained multiple GPU queues. | Yes, developer diagnostics | [performance](../Goals.md#software-and-analysis): controlled CPU/submission comparisons can justify these developer controls. | - |

The [ordinary capture writer](../../Engine/Application/Private/Editor/Capture/ViewportCaptureWriter.cpp) uses BMP. `ImageEncoding::EncodeExr` serves Reference artifact encoding; it is a last-consumer candidate under AP-03, not a reason to remove TinyEXR texture input. Trace [Editor restart](../../Engine/Application/Private/Editor/Host/EditorApplication.cpp) and [settings persistence](../../Engine/Application/Private/RenderingSettings/EngineRenderingSettingsPersistence.cpp) separately from Launcher bootstrap behavior.

The 1,482-line GPU-crash persistence/input area is mixed responsibility. Its size does not authorize removing required dumps, markers, shader identity or completion waits. The explicit diagnostic boundary below remains authoritative.

## GameFramework and scene runtime

Owner: [Engine/GameFramework](../../Engine/GameFramework). Observed scope: 219 source/build files, 12,223 physical lines.

Shared runtime scene ownership supports map loading, selected Editor edits and basic animated demonstrations. It is not a public gameplay SDK or a platform for other developers to add systems.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| GF-01 | Basic skeletal animation: imported skeletons/clips, simple playback and skinning for Knight and simple animated examples. Keep required animated geometry and ray-tracing updates; no general animation-authoring framework. | Yes, basic | Yes, basic |
| GF-02 | Morph targets, morph weights and their animation/import/cook/GPU paths. Remove without removing skeletal skinning. | No | No |
| GF-03 | Imported material variants and runtime variant selection. A No affects importer/cooked data, world views and Editor controls. | Yes | Yes |
| GF-04 | Asynchronous in-process scene replacement, cancellation and keeping the accepted world when loading fails. A No can replace the whole workflow with one scene per process. | Yes | Yes |
| GF-05 | The existing typed ECS/component-storage scene representation. A No means designing a simpler scene representation, not deleting scene identity or Editor functionality. | Yes | Yes |
| GF-06 | Parallel world-system execution for retained transforms and extraction. A No uses a simpler serial implementation where acceptable. | No* | No, subject to cost check |
| GF-07 | Explicit authored instance groups and renderer-facing instance batching inputs. This is separate from automatically batching meshes in RE-07. | Yes | Yes |

Keep scene/material identity, selected edits, stable read/publication boundaries and one-way render extraction. Basic skeletal animation is retained; morph deformation is removed. Preserve skeleton/clip loading, pose evaluation, skin weights/joint palettes, GPU skinning, previous-frame data and geometry/acceleration-structure updates required by the real-time path tracer. Do not delete shared animation/skinning code with morph-only code.

The intended scope is basic clip playback and movement demonstrations, not blend trees, animation graphs, retargeting, IK, timeline tooling or general animation authoring. Do not add these during cleanup. CesiumMan is retained under PR-11 as a second basic animation example; additional examples remain later choices.

GF-06 accepts the suggested simplification subject to its workload/cost check. It concerns CPU world-system execution, not asynchronous scene loading, Renderer tasks or GPU queues retained under RH-04.

### GameFramework: additional decisions

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| GF-08 | **Cross-pool per-asset read/decode pipeline** | Keep asynchronous replacement under GF-04, but consider one background load operation with a bounded asset loop. Remove cross-pool machinery only after Bistro/Knight load time, memory, cancellation and accepted-world preservation checks. | No* | [performance](../Goals.md#software-and-analysis): keep asynchronous loading, but require a measured benefit for the extra handoffs. | - |
| GF-09 | **Transactional replacement-world staging** | Keep staging/generation checks even if GF-08 makes execution serial. No cannot mean destroying the active scene before replacement succeeds. | Yes | [ownership](../Goals.md#north-star): failed preparation must not destroy the accepted world. | - |
| GF-10 | **Immutable Editor world read view** | Keep one publication authority and safe borrowed read lifetime. The current incremental path still clones previous storage; GF-16 separately decides that copying mechanism. | Yes | [ownership](../Goals.md#north-star): protect consumer lifetime while separately questioning the copying mechanism. | - |
| GF-11 | **Incremental world-to-renderer identity/resource publication** | No implies rebuilding/resending more scene data. Keep the narrow protocol for scene replacement, edits and animation; no public gameplay extension API is needed. | Yes | [performance](../Goals.md#software-and-analysis): large scenes need stable identity and bounded update work. | - |
| GF-12 | **Fixed text level-document parser and writer** | No requires one replacement format across all consumers and regeneration, preserving chosen save fields. Do not keep both grammars. Parsing's 1,092 lines still contain necessary field validation. | Yes, narrow | [ownership](../Goals.md#north-star): one small explicit schema can be owned; a format rewrite is not a free saving. | - |
| GF-13 | **Runtime Ozz offline animation/skeleton construction** | Move offline construction to SceneCooker; ship prepared runtime data and required metadata. Retire runtime offline-builder dependencies/duplicate conversion only after bounded loading and basic animation pass. Moving lines alone is not net simplification. | No* | [product scope](../Goals.md#product-scope): basic runtime playback should not need unnecessary preparation work at load time. | - |
| GF-14 | **Multi-asset level composition** | No breaks Knight/Candles composition or requires rebaked assets. Keep needed composition without adding general scene streaming. | Yes | [product scope](../Goals.md#product-scope): Knight/Candles require actual base/add-on composition. | - |
| GF-15 | **Shared scene resource stores** | No risks duplicated Bistro resources/memory. Keep real shared identity and lifetime; retire a store only after its retained consumers disappear. | Yes | [performance](../Goals.md#software-and-analysis): preserve real reuse instead of inflating per-instance data. | - |
| GF-16 | **Whole-storage copying during Editor read-view publication** | Retain immutable read lifetime from GF-10. Investigate smaller publication/storage sharing when only a camera or property changed; check large-scene CPU/memory cost before replacing it. No additional cache/layer is justified without a smaller measured design. | No* | [performance](../Goals.md#software-and-analysis): measure the full snapshot copy independently of dirty-field refresh and read safety. | - |

The [read-view publisher](../../Engine/GameFramework/Private/World/GameWorldPublication.cpp) constructs `WorldReadView::Storage` from the entire previous storage before refreshing changed entities. Dirty-field refresh does not establish that full-storage copying is absent; GF-16 addresses that exact mechanism. No runtime timing or memory benefit is claimed yet.

Sources: [scene loading](../../Engine/GameFramework/Private/Level/Loading/SceneLoadRun.cpp), [world publication](../../Engine/GameFramework/Private/World/GameWorldPublication.cpp), [render extraction](../../Engine/GameFramework/Private/World/Extraction/RenderFrameSubmissionExtractor.cpp), [animation construction](../../Engine/GameFramework/Private/Assets/Loaders/AnimationAssetLoader.cpp), [skeleton construction](../../Engine/GameFramework/Private/Assets/Loaders/SkeletonAssetLoader.cpp). Both loaders include Ozz offline-builder APIs today; GF-13 is an actual ownership choice.

GF-02 removes morphs; GF-06 conditionally simplifies per-frame world-system parallelism; GF-08 conditionally simplifies loading. None removes skeletal playback, GPU queues, renderer threading or all asynchronous work. `AnimationPreviewBaker`/`AnimationPreviewEvaluator` serve retained animation despite their names; they are not merely the removed mesh-preview panel.

The private ECS uses `Destroy` and slot reuse in mesh/camera/light/animation construction rollback. Do not call it unused gameplay infrastructure or remove cleanup/identity checks because the Editor lacks Delete Object.

## RHI

Owner: [Engine/RHI](../../Engine/RHI). Observed scope: 372 source/build files, 37,814 physical lines.

Your owned graphics systems implementation and an important code-review subject. Reviewers exercise it through supplied scenes and API options; they do not need a native SDK installation to run the bundle.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| RH-01 | Personally owned D3D12 backend. | Yes | Yes |
| RH-02 | Personally owned Vulkan backend with independent qualification. A No also removes otherwise unused Vulkan compiler, SDK and deployment paths. | Yes | Yes |
| RH-03 | PTLAS and its vendor-specific command/resource/capability path. Standard BLAS/TLAS remains a separate retained requirement. | No | No |
| RH-04 | Multiple native queue types and the submission, dependencies, synchronization and retirement needed to use them. Keep applicable async-compute support without inventing extra workloads. | Yes | Yes |
| RH-05 | RenderDoc capture integration in the application. A No can still allow external/manual captures. | Yes | Yes |
| RH-06 | PIX capture integration in the application. This is separate from PIX event markers. | Yes | Yes |
| RH-07 | PIX marker/runtime support for non-Shipping D3D12 profiling. | Yes | Yes |
| RH-08 | Shader-target enumeration and its supported compiler/contracts mappings. Retain rather than narrowing to only the selected runtime targets. | Yes | Yes |
| RH-09 | Native GPU-crash breadcrumbs and semantic markers, D3D12 DRED diagnostics, supported Vulkan checkpoints, Aftermath marker resolution and required fault-data collection. Preserve readable labels and safe marker lifetime. | Yes | Yes |

Required: explicit resource/descriptor ownership, upload, commands, pipeline/shader ABI, native interop for retained SDKs, validation and GPU retirement. D3D12MA/VMA are already integrated. Their use does not remove your responsibility to understand allocation and lifetime policy.

Both D3D12 and Vulkan are now explicitly retained. Multi-queue support and shader-target enumeration are retained too. Qualify each API separately, including cross-queue dependencies/resource transfers and retirement after all relevant queues finish.

## Renderer

Owner: [Engine/Renderer](../../Engine/Renderer). Observed scope: 619 source/build files, 36,806 physical lines.

The main visible portfolio result and a source-inspection subject. Approved options and debug views must work with prepared data; changing them must not require a reviewer to recompile shaders.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| RE-01 | The owned frame graph, resource transitions and transient/history lifetime model. A No requires a deliberate simpler execution design. | Yes | Yes |
| RE-02 | Real-time path tracer as the only lighting path, including its direct/indirect lighting and shadow work. | Yes | Yes |
| RE-03 | Owned production ReSTIR indirect-lighting work within the real-time path tracer. A No reduces real-time GI scope substantially. | Yes | Yes |
| RE-04 | Independent Reference path tracer, its render mode, sessions, accumulation, scheduling, resources, shaders and controls. Remove completely. | No | No |
| RE-05 | Both rasterized and ray-traced GBuffer algorithms as alternative geometry/surface-data paths feeding the same real-time path-traced lighting. | Yes | Yes |
| RE-06 | TLAS refit/update policy as well as full rebuild. A No simplifies policy, with possible performance cost for movement. | Yes | Yes |
| RE-07 | Automatic mesh batching in addition to explicitly authored instance groups. | No* | No, subject to cost check |
| RE-08 | DLSS Super Resolution through the existing Streamline integration. | Yes | Yes |
| RE-09 | DLSS Ray Reconstruction through the existing Streamline integration. | Yes | Yes |
| RE-10 | The existing subsurface material/shading approximation and its dedicated outputs/debug views. | Yes | Yes |
| RE-11 | Alpha masking, two-sided surfaces and their matching raster/ray material behavior. Removing these excludes content that needs them. | Yes | Yes |
| RE-12 | Histogram-based automatic exposure alongside manual exposure. | Yes | Yes |
| RE-13 | HDR/advanced output-color modes beyond an explicit SDR presentation path. Source support still needs native/display qualification. | No | No |
| RE-14 | Vignette and chromatic aberration. A No simplifies presentation and removes their settings and shader branches. | No | No |
| RE-15 | Broad material/GBuffer/lighting visualization modes, rather than only a few final-image views. | Yes | Yes |

All Renderer decisions are now recorded. Keep the owned frame graph, both GBuffer algorithms, production ReSTIR indirect lighting, TLAS refit, DLSS Super Resolution/Ray Reconstruction, subsurface shading, alpha/two-sided semantics, automatic/manual exposure and useful visualizations. Both GBuffer algorithms feed the same real-time lighting path; they do not restore another lighting renderer.

Remove Reference (RE-04), HDR display/advanced output modes (RE-13), vignette and chromatic aberration (RE-14). RE-07 accepts removing optional automatic mesh batching subject to the representative-workload cost check; preserve explicit instance groups under GF-07. PTLAS removal remains RH-03.

Subsurface retention covers admitted material fields, import/cooking, runtime layouts, shader evaluation and dedicated outputs/debug views. It does not automatically select any particular subsurface demonstration scene or expand the existing approximation into a new scattering model.

## Shader assets and rendering data

Owner: [Engine/Assets/Shaders](../../Engine/Assets/Shaders). Observed source scope: 145 HLSL/include files, 7,544 physical lines. This is a source/data area rather than a separately composed engine module.

You author and compile shaders; reviewers run the cooked shader set and can read the matching sources. Preserve useful signal views and environments without adding a reviewer shader-development workflow.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| SH-01 | Both inline ray-query and ray-tracing pipeline shader paths where the retained frame requires them. A No requires choosing and adapting a tracing path. | Yes | Yes |
| SH-02 | Wireframe and GPU-scene instance visualization in addition to material/lighting signal views. | Yes | Yes |
| SH-03 | Multiple built-in sky/environment images beyond one neutral diagnostic environment and one presentation environment. | Yes | Yes |

All three Shader assets/rendering data items are accepted as Yes: retain both tracing mechanisms where required, wireframe/GPU-scene visualization, and the existing collection of built-in sky/environment images. This does not reverse feature removals already decided in other modules.

Shader removals follow the recorded cuts: morph deformation GF-02, PTLAS RH-03 and display effects RE-13/RE-14. Preserve both GBuffer paths under RE-05 and subsurface shader/data/debug support under RE-10. Keep shared material semantics and host/shader layout checks. Remove the Reference-specific shader tree and registrations under RE-04. Shared transport/material/math includes remain only where retained real-time consumers need them; trace transitive use before retiring code.

## Core

Owner: [Engine/Core](../../Engine/Core). Observed scope: 127 source/build files, 7,278 physical lines.

Shared foundations support your iteration and both applications. The target is clear internal contracts and useful diagnostics, with no general-purpose library distribution or third-party compatibility promise.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| CO-01 | Runtime console command/CVar registration and execution, including the Editor console consumer. | Yes | Yes |
| CO-02 | Named/category-based logging beyond simple process output. | Yes | Yes |
| CO-03 | Reusable event/subscription infrastructure rather than direct callbacks at every boundary. A No requires replacing all actual subscriptions. | Yes | Yes |
| CO-04 | A reusable Core child-process supervision service. A No moves or replaces a capability still required by retained Launcher/tools. | Yes | Yes |

File/path/JSON/hash/math/time primitives still follow actual consumers. File-set publication and reader lifetime are not cosmetic scaffolding: simplify only with an equivalent accepted-output guarantee. Remove unused public utilities and forwarding layers as normal cleanup after the scope choices.

### Core: additional decisions

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| CO-05 | **Unused non-Windows OS fallback branches** | Remove unused OS branches such as XDG/HOME state-root discovery. Keep the supported Windows failure behavior. This is a small cleanup, not a platform rewrite or major saving. | No | [product scope](../Goals.md#product-scope): the selected engine is Windows-only; this is a small support-surface cleanup. | - |
| CO-06 | **Core-owned engine/project/cooked-layout discovery** | Consider host/product-owned roots passed into actual consumers, retaining one shared layout contract. Remove duplicate discovery/authority, not merely relocate 969 lines of path implementation. This requires a concrete smaller design first. | No* | [code ownership/style](../References.md#code-style): product policy should have one appropriate owner; prove a smaller design before moving it. | - |

Do not count Core's entire Files/Process/Paths areas as removable scaffolding. Retained consumers need UTF-8/native argument conversion, bounded reads, atomic publication, child cancellation/settlement, relocatable read-only content and writable user state. LA-26 governs the readiness handoff in [ChildProcessWindowsReadiness](../../Engine/Core/Private/Process/Windows/ChildProcessWindowsReadiness.cpp); removing post-crash observation does not retire it. [File-set publication](../../Engine/Core/Public/Files/FileSetPublication.h) already explicitly excludes journal replay, so a hypothetical recovery framework is not available deletion credit.

## Platform

Owner: [Engine/Platform](../../Engine/Platform). Observed scope: 25 source/build files, 1,783 physical lines.

Reviewer-facing window/input behavior must work without setup; you own its implementation. Retain the chosen Windows, DPI, focus and input-routing behavior rather than expanding platform coverage.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| PL-01 | The current SDL3 window/input backend with owned Sparkle adapters. A No is a platform rewrite, not automatic code reduction. | Yes | Yes |
| PL-02 | Both windowed and borderless presentation modes. | Yes | Yes |
| PL-03 | Layered/deferred input routing for Editor UI and viewport ownership. A No replaces that routing with a simpler explicit design. | Yes | Yes |

Keyboard/mouse camera control, focus-loss cancellation, pointer release and correct DPI/resize behavior remain requirements of the selected modes. No controller, VR or Linux work is being added by this checklist.

### Platform: narrower review

This is a 1,783-line adapter, not a general multi-platform engine layer. ED-17 owns the custom title-bar/hit-test choice across Editor and Platform; do not ask for a contradictory second answer here.

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| PL-04 | **Immediate and deferred delivery in fixed input layers** | No needs a proven simpler delivery contract for UI, camera and system observers. Preserve releases on focus/routing changes and the layered/deferred behavior accepted under PL-03. The current API is already fixed-purpose, not a plugin input stack. | Yes | [reviewer usability](../Goals.md#review-and-transfer): retained UI/camera interaction needs coherent routing and cancellation. | - |

Source: [InputSystem contract](../../Engine/Platform/Public/Input/InputSystem.h). No controller/VR/plugin input subsystem was found here to retire. Do not invent one as a scope-cut candidate.

## SourceImporters

Owner: [Tools/Import/SourceImporters](../../Tools/Import/SourceImporters). Observed scope: 89 source/build files, 5,900 physical lines. [Source dispatch](../../Tools/Import/SourceImporters/Private/SourceSceneImporter.cpp) currently admits glTF/GLB, FBX and PLY.

Maintainer tooling for acquiring/preparing the retained content. Reviewers consume cooked scenes; they can inspect the import code but are not expected to supply assets or run converters.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| IM-01 | glTF/GLB scene import through cgltf, with the material/geometry subset chosen for Sparkle. | Yes | Yes |
| IM-02 | FBX import required by the retained Knight scene, including its geometry, material and basic skeletal-animation conversion. Narrow supported behavior to retained content. | Yes | Yes |
| IM-03 | PLY import for Stanford scan content. | Yes | Yes |
| IM-04 | Importing source-authored cameras, rather than authoring supported cameras in level files/Editor. | Yes | Yes |
| IM-05 | Importing source-authored lights, rather than authoring all retained lights in level files/Editor. | Yes | Yes |

All five importer capabilities are retained for your preparation workflow: glTF/GLB, FBX, PLY, source-authored cameras and lights. Preserve the admitted camera/light data through cooking/loading; SC-01 is required. Reviewer inspection uses those cooked results. This does not select every scene or restore morph support.

Animation/morph/variant import follows GF-01/GF-02/GF-03. Tangent generation and coordinate/unit/material interpretation remain as required by retained content. Do not assume FBX can disappear while keeping every current scene; ModernSponzaKnight explicitly references an FBX asset. FBX is now retained for Knight; The local asset audit also identified FBX source for both Bistro scenes. No glTF-only cut can remove those retained source routes.

### SourceImporters: additional decisions

The glTF geometry area is 1,562 lines, animation 705, and Assimp/FBX adapters together 1,931. Necessary tangent frames, skeleton transforms and source interpretation remain even when their source files are individually small.

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| IM-06 | **FBX embedded-texture extraction/cache/TGA encoding** | Require external texture files for retained FBX assets. Remove extraction/cache/encoding only after an actual retained FBX import confirms embedded payloads are unnecessary. Existing DDS filename references alone do not establish this. | No* | [product scope](../Goals.md#product-scope): support the actual retained FBX inputs rather than all possible embedded payload forms. | - |
| IM-07 | **Missing-tangent generation and tangent-frame remapping** | No requires consistently preprocessed source meshes and visual checks. Keep the MikkTSpace-backed path needed for correct normal mapping; it is not just optional importer polish. | Yes | [product scope](../Goals.md#product-scope): correct normal mapping is part of content-to-pixel evidence, not cosmetic import tooling. | - |
| IM-08 | **Step/cubic source-animation curve support** | Require source clips already baked to the supported simple form. First inspect retained glTF samplers and FBX conversion; preserve Knight/Cesium timing and rotation quality. This narrows accepted inputs, with modest direct code savings. | No* | [product scope](../Goals.md#product-scope): basic clip playback can use a bounded source subset only after content equivalence is shown. | - |
| IM-09 | **Import/cook every source animation clip** | Keep only named required clips for basic demonstrations. This can reduce data/preparation and your supported examples; ordinary iteration over clips may remain and does not earn a large source-deletion estimate. | No* | [product scope](../Goals.md#product-scope): retain deliberate movement demonstrations rather than an arbitrary clip-support obligation. | - |

The current glTF camera importer accepts its checked perspective-camera subset; keeping source-authored cameras is not a promise to add every glTF projection or infinite-far-plane form. Unsupported forms need an explicit error rather than silent reinterpretation.

Sources: [FBX texture extraction](../../Tools/Import/SourceImporters/Private/Fbx/FbxEmbeddedTextureImporter.cpp), [tangent generation](../../Tools/Import/SourceImporters/Private/Gltf/Geometry/GltfMeshTangentGenerator.cpp), [animation baking](../../Tools/Cooking/SceneCooker/Private/Animations/AnimationPreviewBaker.cpp). IM-08/IM-09 do not remove basic animation, skinning or imported cameras/lights.

IM-03 remains Yes. Its own PLY adapter is only 37 lines; Assimp is still required for retained FBX. The non-PLY Stanford condition means those scenes cannot justify PLY, but this audit does not silently reverse your importer answer. Removing unsupported scene entries also does not imply every material feature used by retained shader assets can go.

## AssetCooker

Owner: [Tools/Cooking/AssetCooker](../../Tools/Cooking/AssetCooker). Observed scope: 30 source/build files, 1,550 physical lines.

Maintainer iteration and release preparation. A missing cooker/toolchain does not block reviewer scene readiness when the required cooked outputs are already shipped.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| AC-01 | Incremental cooking with source/output identity checks, rather than always recooking the full selection. | Yes | Yes |
| AC-02 | Bounded parallel asset/texture preparation. A No uses a simpler serial cook with potentially longer preparation time. | Yes | Yes |
| AC-03 | Additional fine-grained category/subset controls inside AssetCooker beyond the retained selected-content workflow. Removing these does not remove your Launcher build/cook/sync workflows. | No | No |

Retain one explicit selected-content plan, useful diagnostics and reliable publication for your iteration. LA-08 keeps supported Launcher options without restoring the extra AssetCooker control surface removed under AC-03. This executable composes SceneCooker, MeshCooker, MaterialCooker and texture requests; the supporting cookers below are mostly libraries, not independent user-facing applications.

### AssetCooker: additional decisions

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| AC-04 | **General multi-project/ALL/positional cook CLI** | Support Showcase and a small explicit argument grammar. Retain profile/root options needed by actual callers; remove duplicate multi-project policy under TS-02. | No | [product scope](../Goals.md#product-scope): Showcase is the only product; remove unused CLI generality. | - |
| AC-05 | **AssetCooker-owned shader-cook orchestration** | Make AssetCooker own scene/mesh/material/texture preparation and let the retained Launcher owner sequence ShaderCompiler. Preserve one Cook All action and noninteractive operation. Verify no retained caller loses its complete cook. | No* | [ownership](../Goals.md#north-star): one cook sequencer can replace duplicate orchestration while retaining Cook All. | - |
| AC-06 | **Supervised TextureCooker child-process stage** | No implies in-process composition while TX-04's standalone tool still exists. Keep the existing shared request format and process boundary unless a smaller measured design justifies changing it. | Yes | [ownership](../Goals.md#north-star): a real process boundary and one request contract can be clearer than a second in-process integration. | - |
| AC-07 | **Extension-probing scene-source resolver** | Use one explicit catalog source path per retained scene asset. Preserve stable scene identity, relocatable paths and rejection of ambiguous/escaping input; update all level/cook/acquisition consumers together. | No* | [ownership](../Goals.md#north-star): explicit curated source paths reduce hidden selection policy and ambiguity. | - |

AC-07 targets `ProjectCookPlanner::ResolveSceneSource`: it currently accepts an exact source path or probes four extensions. This is distinct from stable cooked scene identity.

Sources: [CLI grammar](../../Tools/Cooking/AssetCooker/Private/Cli/AssetCookerCli.cpp), [shader stage](../../Tools/Cooking/AssetCooker/Private/Cooking/CookShaders.cpp), [texture stage](../../Tools/Cooking/AssetCooker/Private/Cooking/CookTextures.cpp). AC-05 is orchestration cleanup with a small direct footprint, not permission to retire ShaderCompiler or retained Launcher cooking.

## SceneCooker

Owner: [Tools/Cooking/SceneCooker](../../Tools/Cooking/SceneCooker). Observed scope: 30 source/build files, 1,430 physical lines.

Maintainer pipeline library whose output supplies runtime cameras, lights, instances and animation. Keep its data contract understandable; reviewers do not operate it directly.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| SC-01 | Metadata for whichever imported cameras/lights you retain, rather than cooking only mesh/material instances. A No must be reconciled with IM-04/IM-05. | Yes | Yes, required by imported cameras/lights |
| SC-02 | SceneCooker as a distinct cohesive library boundary within the content pipeline. A No merges its retained responsibility into the consuming cooker. | Yes | Yes |

Keep skeleton/animation builders under GF-01; remove morph payloads under GF-02. Complete scene generations, content identity and acceptance stay protected even if scene content becomes much simpler.

### SceneCooker: additional decision

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| SC-03 | **Custom cook-time animation curve resampler** | Keep only interpolation genuinely needed after IM-08 and the chosen Ozz preparation route under GF-13. A replacement must preserve rotations, bind transforms, source timing and bounded data; do not discard it merely because its name contains Preview. | No* | [integration precedents](../References.md#integration-precedents): reuse a proven narrow preparation path where possible, preserving animation semantics. | - |

The remaining scene cook is already constrained by retained cameras/lights, instances, variants and skeletal animation. GF-13 is the single decision about moving offline Ozz construction here; SC-03 concerns the curve-conversion implementation, not a second vote on runtime construction.

## MeshCooker

Owner: [Tools/Cooking/MeshCooker](../../Tools/Cooking/MeshCooker). Observed scope: 7 source/build files, 377 physical lines.

Maintainer pipeline library and a focused source-inspection boundary. Reviewers require prepared static/skinned meshes, not a mesh-conversion UI or SDK.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| MC-01 | A separate MeshCooker library with a narrow checked mesh-writing contract. A No merges the responsibility; it does not remove mesh cooking. | Yes | Yes |

Static vertex/index/layout validity remains essential. Keep skeletal/skinning payload support under GF-01 and remove morph payload support under GF-02. This small library is not a large saving merely because it has its own folder.

## MaterialCooker

Owner: [Tools/Cooking/MaterialCooker](../../Tools/Cooking/MaterialCooker). Observed scope: 8 source/build files, 434 physical lines.

Maintainer pipeline library that preserves material meaning through to the demonstrated pixel. Reviewer controls operate on admitted runtime data rather than recooking source materials.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| MA-01 | A separate MaterialCooker library that carries admitted material semantics and texture references into runtime data. | Yes | Yes |

Variant support follows GF-03 and subsurface support follows RE-10. Do not vote separately to remove their cooked representation while retaining the same runtime feature. Base PBR, alpha semantics, texture interpretation and raster/ray agreement remain correctness work.

## TextureCooker

Owner: [Tools/Cooking/TextureCooker](../../Tools/Cooking/TextureCooker). Observed scope: 30 source/build files, 2,019 physical lines.

Maintainer pipeline and compression tooling. Reviewers need the resulting GPU textures; they do not need decoders, encoders or conversion commands installed to inspect a map.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| TX-01 | GPU block-compressed cooked textures and the associated offline processing. | Yes | Yes |
| TX-02 | HDR/EXR texture inputs for lighting/environment work. This is separate from HDR display output RE-13. | Yes | Yes |
| TX-03 | KTX/KTX2 input support in addition to the texture formats required by the selected scenes. | No | No |
| TX-04 | A standalone TextureCooker executable, in addition to AssetCooker orchestration and shared texture contracts. | Yes | Yes |

Correct sRGB/linear handling, normal-map processing, mip generation, dimensions and source/output identity remain requirements for admitted textures. Remove a decoder/dependency only after checking retained source and embedded textures. A No to HDR display, or removing the Reference renderer, does not justify dropping EXR environment/texture inputs needed by the real-time path.

TX-03 is decided No. Remove KTX/KTX2 source input and the KTX dependency entirely. GPU BC compression, DDS, PNG/JPEG and HDR/EXR support remain independent and retained.

The 25 November asset audit found no KTX/KTX2 files or image references in the retained asset set. All retained source entries were present; local glTF/GLB images used PNG/JPEG and the retained Bistro FBX metadata referenced DDS. The Knight FBX contained no Texture/Video objects. This asset inspection is separate from executed cook/build verification.

The implementation retires the KTX loader, dispatch/declaration, special mip-policy branch, dependency recipe/catalog and build link, Launcher dependency presentation and KTX-only notices. Keep unsupported BasisU admission rejection and the raw-source package rejection guard: neither is a dependency on KTX. Source removal is implemented. Root CMake configure and DevelopmentEditor builds of TextureCooker, AssetCooker and SparkleLauncher passed on 25 November. PNG, JPEG, multi-mip DDS, HDR and a retained sky EXR cooked successfully; DDS retained its mip count. Both removed extensions were rejected while preserving an accepted output. Scoped formatting and diff checks passed. These are tool checks, not GUI/GPU or package qualification. Local receipts: `build/validation/ktx-removal-20261125/`. Previously produced archives/notices are historical artifacts and are not rewritten by source removal.

### TextureCooker: additional decisions

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| TX-05 | **PSD/PIC/PNM-family texture source inputs** | Reject unused formats explicitly after checking retained external and embedded inputs. stb may remain for PNG/TGA, so this is a small support-surface cut, not full decoder-library removal. | No* | [product scope](../Goals.md#product-scope): admit only required inputs; remaining stb users prevent a whole-library removal claim. | - |
| TX-06 | **BMP/GIF texture source inputs through WIC** | Retain only formats used by retained inputs. Multi-frame sources are already rejected, and WIC may remain for JPEG; no large dependency saving is promised. BMP screenshot output is a different path. | No* | [product scope](../Goals.md#product-scope): trim unused admission paths without confusing screenshot output or JPEG decoding. | - |
| TX-07 | **Texture request-file inspection/report command** | Keep request-file cooking, validation, help and failures. Remove presentation in `InspectTextureCookRequestFileCommand` if you do not use it. | No | [product scope](../Goals.md#product-scope): optional reporting has less value than the required cooking/diagnostic contract. | - |
| TX-08 | **BC6H and floating-point HDR cooked outputs** | No needs one chosen quality/memory policy for retained environments. Keep an explicit fallback where compression quality is inadequate; retaining EXR input alone does not decide the GPU representation. | Yes | [performance](../Goals.md#software-and-analysis): quality/memory tradeoffs can justify both outputs; input format is a different decision. | - |

Sources: [format dispatch](../../Tools/Cooking/TextureCooker/Private/SourceLoading/TextureSource.cpp), [request inspection](../../Tools/Cooking/TextureCooker/Private/Cli/InspectTextureCookRequestFileCommand.cpp), [processing](../../Tools/Cooking/TextureCooker/Private/Processing/TextureProcessor.cpp). TX-05/TX-06 are lower priority than Launcher/world/Editor cuts. Do not remove normal-map, sRGB, mip or accepted-output guarantees to shrink the processor.

## ShaderCompiler

Owner: [Tools/Shaders/ShaderCompiler](../../Tools/Shaders/ShaderCompiler). Observed scope: 104 source/build files, 7,938 physical lines.

Maintainer-only compilation, publication and symbol preparation. Reviewers can explore supported GBuffer/lighting/reconstruction modes using precompiled artifacts and inspect matching source.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| CP-01 | Slang shader-source backend alongside DXC/HLSL, with its required SDK/tooling and supported binary-output contracts. | Yes | Yes |
| CP-02 | Shader symbols, source correlation and the relevant native debug exports. | Yes | Yes |
| CP-03 | Optional stand-alone shader analysis/report/export modes beyond runtime reflection, compiler diagnostics and the symbols/source data needed by your debuggers. | No | No |
| CP-04 | The first-party source preprocessing/mount machinery, rather than a narrower proven compiler-owned include path. This requires a replacement investigation before removal. | No* | No, subject to proven replacement |

DXIL follows RH-01; SPIR-V follows RH-02. Slang and DXC/HLSL are both retained. Keep shader symbols, native source correlation and required debug exports, including GPU-crash/breadcrumb investigation support. Retain typed host/shader ABI checks, reflection consumed by runtime, publication and failed-recompile preservation. ShaderMake is already integrated. Remove only optional developer analysis/export modes under CP-03, not data required by compilation, reflection, debugging or external tools. CP-04 accepts retirement of redundant first-party preprocessing only after a proven replacement; it is not an established safe deletion or a reason to repeat an already failed trial unchanged.

### ShaderCompiler: additional decisions

Of 7,938 lines, cook publication is 1,524, planning 1,016, compile batching 991, and DXIL/SPIR-V/Slang reflection occupies substantial compiler code. These areas contain retained debugging and ABI work; the optional analysis folder alone is just 43 lines.

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| CP-05 | **Single registered-shader compilation selector** | No compiles a larger selection. Keep the small useful selector; it is distinct from removing optional analysis/export UI. | Yes | [product scope](../Goals.md#product-scope): focused shader iteration directly serves your retained development workflow. | - |
| CP-06 | **Changed-source/include-closure incremental selection** | No falls back to broader compilation. Preserve correct invalidation and accepted-output publication; do not drop dependency tracking while claiming incremental correctness. | Yes | [product scope](../Goals.md#product-scope): fast iteration must still invalidate the actual dependent shaders correctly. | - |
| CP-07 | **Bounded parallel compiler sessions** | No uses serial compilation and may slow your iteration. Keep the existing small limit and a serial setting for diagnosis. | Yes | [performance](../Goals.md#software-and-analysis): useful bounded iteration throughput with a serial diagnostic setting. | - |
| CP-08 | **Two symbol-export CLI routes: debug-info and debug-artifacts** | Prefer one clear symbols operation with an explicit output contract. Keep exact code/symbol/source identity, fresh outputs and required SDK/debug associations; this is interface consolidation, not removal of debug exports. | No* | [debugging evidence](../References.md#nvidia-production-studies): preserve matched code/symbol/source exports while consolidating optional interface duplication. | - |
| CP-09 | **ShaderCompiler catalog/inspection CLI commands** | Remove optional command presentation after checking your automation. Preserve shader-target enumeration and its supported mappings under RH-08. | No* | [product scope](../Goals.md#product-scope): optional reporting can go; target enums, contract checks and runtime reflection stay. | - |
| CP-10 | **Host/shader ABI verification and runtime reflection data** | No risks accepting mismatched shader layouts. Keep the real consumed contract; remove duplicate representations only after tracing Renderer/RHI consumers. | Yes | [debugging evidence](../References.md#nvidia-production-studies): inspectable correct shader integration is stronger evidence than fewer validation lines. | - |

Sources: [cook options](../../Tools/Shaders/ShaderCompiler/Private/Cli/ShaderCookOptions.cpp), [planning](../../Tools/Shaders/ShaderCompiler/Private/Cooking/Planning/ShaderCookPlanBuilder.cpp), [debug artifact writer](../../Tools/Shaders/ShaderCompiler/Private/Cooking/Publication/ShaderDebugArtifactWriter.cpp), [ABI verification](../../Tools/Shaders/ShaderCompiler/Private/Verification/ShaderParameterStructVerifier.cpp). CP-08 must follow the protected GPU-diagnostics boundary before touching manifests or identity data.

The default cook already selects DXIL SM 6.6 and SPIR-V 1.6. Keeping the shader-target enum does not require cooking every target for every release, nor does a narrower default justify deleting the enum or Slang.

## ToolSupport

Owner: [Tools/Support/ToolSupport](../../Tools/Support/ToolSupport). Observed scope: 20 source/build files, 1,428 physical lines.

Shared maintainer/tool contracts that prevent duplicate path/startup/error policy. Keep only the single Showcase workflow and runtime path support needed by the inspection bundle.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| TS-01 | Shared tool startup/error handling and project/content path contracts instead of copying them into each tool. | Yes | Yes |
| TS-02 | General multi-project discovery/resolution beyond the single Showcase project. A No fixes the supported product to Showcase. | No | No |

Retain genuine shared lifetime/format policy. Remove forwarding helpers and copies where actual consumers can use the owner directly. Project generality also reaches CMake, Launcher and cook planning, so TS-02 is one decision across those consumers.

### ToolSupport: additional decisions

The catalog implementation is 700 of this module's 1,428 lines. Much describes packs, parent dependencies, families, variants, source/download readiness and selected levels; review that model rather than deleting a small shared progress writer.

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| TS-03 | **General scene-family/variant presentation metadata** | Use explicit retained scene entries and the actual pack dependency relationships. Keep Knight/Candles base-content dependencies; flattening presentation must not lose required downloads. | No* | [product scope](../Goals.md#product-scope): a curated scene list needs real dependencies, not a general content-browser model. | - |
| TS-04 | **Unsupported scene/add-on catalog policies** | Remove retired entries and branches with no remaining consumer. Preserve manual-download/source-availability explanations still needed for retained packs; do not delete a field just because a reviewer never downloads content. | No* | [product scope](../Goals.md#product-scope): remove retired workflow obligations while retaining required manual-source diagnostics. | - |

Source: [catalog contract](../../Tools/Support/ToolSupport/Public/ProjectLevelCatalog.h). TS-02 already removes general multi-project discovery across Core paths, CMake project discovery, Launcher project resolution and cook planning. A hardcoded project label with the discovery framework still underneath is not completion.

## Showcase scenes and conversion scripts

Owner: [Projects/Showcase](../../Projects/Showcase), especially [Levels.catalog](../../Projects/Showcase/Levels.catalog). Observed source/script scope: 15 files, 762 physical lines; asset binaries and level data are outside those counts.

Scenes are reviewer-facing demonstrations; acquisition and conversion scripts serve you. Include each retained scene and its real dependencies in the prepared experience, with an intentional starting camera/configuration.

The selected list retains eleven scenes and makes the two Stanford scenes conditional on a supported non-PLY route. Everything else in the current scene/catalog scope is selected for removal, except assets required by retained scenes. Source suitability and redistribution status still need checking for public presentation.

| ID | Keep this scene or scene family? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| PR-01 | Empty, for startup, failure handling and a minimal runtime loop. | Yes | Yes |
| PR-02 | CornellBox and the owned conversion needed to reproduce it, for bounded transport/lighting checks. | Yes | Yes |
| PR-03 | Classic Sponza as a short geometry/material regression loop. | Yes | Yes |
| PR-04 | BistroExterior as the substantial presentation/performance workload. The inspected local source is FBX. | Yes | Yes |
| PR-05 | BistroInteriorWine as a second substantial scene. | Yes | Yes |
| PR-06 | Standalone ModernSponza base showcase level. A No does not remove base assets required by Knight/Candles. | No | No |
| PR-07 | ModernSponzaCandles and its additional content. | Yes | Yes |
| PR-08 | ModernSponzaKnight and its animated FBX content, with the referenced base scene and Curtains assets required by its level. Depends on IM-02 and GF-01. | Yes | Yes |
| PR-09 | SanMiguelHigh and SanMiguelLow, which add large content and preparation/qualification work. | No | No |
| PR-10 | JungleRuins and its separate content route. | No | No |
| PR-11 | CesiumMan as a retained animation demonstration. Depends on GF-01. | Yes | Yes |
| PR-12 | LPSHead and its conversion scripts as a dedicated material/subsurface demonstration. | Yes | Yes |
| PR-13 | StanfordBunnySubsurface and StanfordDragonSubsurface only with a verified supported non-PLY source route. Otherwise remove these scenes; no PLY-dependent exception. | Conditional: non-PLY route only | Conditional: non-PLY route only |
| PR-14 | AnisotropyBarnLamp and SheenCloth as retained material examples. Keeping the scenes does not establish that their named advanced shading is implemented. | No | No |
| PR-15 | DamagedHelmet as an additional material example; DiffuseTransmissionPlant is decided separately under PR-18. | Yes | Yes |
| PR-16 | ABeautifulGame as another scene/content route. | Yes | Yes |
| PR-17 | Unsupported/unselected add-on catalog entries such as Ivy, Trees, Flood and Explosion as future work. | No | No |
| PR-18 | DiffuseTransmissionPlant as an additional material example. | No | No |

PR-07/PR-08 retain Candles and Knight. Preserve the ModernSponza base and any Curtains/Candles/Knight assets referenced by those level documents, while removing the standalone base showcase level. PR-11 retains CesiumMan as another basic animation example. Keep the conversion scripts actually needed by CornellBox and LPSHead.

PR-13 is conditional, not an instruction to build a new conversion framework. The existing Stanford route was reviewed as PLY-dependent; no working alternative has been qualified. Keep those scenes only after a usable source in an already supported non-PLY format and a reproducible acquisition/preparation route are established; otherwise remove them. Their current PLY route cannot satisfy this condition. The earlier general PLY-importer decision IM-03 remains separate; these conditional scenes must not be used as a reason to retain PLY.

A No removes the associated level/catalog/thumbnail/acquisition/conversion/package obligations when unused. It does not authorize deleting unrelated local downloads or captures. Removing a scene does not automatically retire a rendering algorithm still needed by another scene.

## CMake, packaging and repository automation

Owners: [CMake](../../CMake), root/module build files and [.github/workflows](../../.github/workflows). Measured CMake-root scope: 50 source/script files and 3,207 physical lines, plus the root composition file.

Build efficiency serves your iteration; packaging serves reviewer Quick Start. A reviewer is not asked to configure CMake, open a solution, build/cook, sync dependencies or repair deployment.

| ID | Keep this capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| BU-01 | Shared/DLL builds for Sparkle engine modules, private per-module PCHs and efficient incremental iteration. Retire the alternate static-engine mode; dependency/helper linkage follows actual needs. | Yes, DLL only | Yes, DLL only |
| BU-02 | Rider as the only supported maintainer IDE, with one proven Windows toolchain/project workflow. Remove Visual Studio IDE selection/integration and unnecessary IDE/compiler/generator matrix. | Yes, Rider only | Yes, Rider only |
| BU-03 | Standalone Launcher configure/build, independent of the full engine build. | Yes | Yes |
| BU-04 | Self-contained prebuilt Launcher/Editor/Game inspection bundle with approved cooked content and all required runtime files. Reviewers need no development setup. | Yes, inspection bundle | Yes, inspection bundle |
| BU-05 | General import/synchronization of prepared products across workspaces. Recommend removal unless your own iteration demonstrates a need; this is separate from source/content sync retained in Launcher. | No | - |
| BU-06 | Your refinable style tooling and focused source/CI checks. No broad external-contributor build matrix or reviewer test setup is required. | Yes | - |

### Maintainer development workflow

Rider is the only intended IDE and you are the only developer. Keep source acquisition/sync, native builds, shader/content cooking, debugging and packaging as your iteration tools. The inspection bundle is not a developer SDK or a general engine distribution. This does not remove those operations from your source Launcher.

Keep Sparkle engine modules as DLLs with explicit exports, consistent ABI/CRT use, owned allocation/global-state boundaries and correct runtime staging. Retain private target/module PCHs and explicit includes; narrow dependencies so private edits do not rebuild unrelated modules. No-change builds should avoid unnecessary compilation, relinking, recooking and deployment. Verify private-source edits, public-header edits, shader edits and dependency changes separately, including the invalidation each genuinely requires. Bound compilation concurrency and PCH creation rather than applying conflicting flags indiscriminately. No measured speedup or automatic DLL hot-reload feature is claimed by this decision.

BU-01 removes the alternate static Sparkle engine variant, not every STATIC/OBJECT library in CMake. Upstream libraries, cooker helpers and shader registration carriers may still need those forms. Preserve required exported contracts and registration behavior for the single shared-engine configuration.

### Rider and Visual Studio removal boundary

Remove Visual Studio as a selectable/openable IDE, its dedicated launcher actions/settings/defaults, duplicate IDE-detection/presentation branches, and IDE-only solution startup/debugger conveniences where Rider does not consume them. LA-04 is narrowed accordingly. Remove unsupported alternate generator/compiler branches only after selecting the actual working Rider route.

Do not equate Visual Studio IDE removal with deleting MSVC, Windows SDK, MSBuild, toolchain discovery or CMake-generated `.sln`/`.vcxproj` support. The [JetBrains Windows C++ support description](https://support.jetbrains.com/hc/en-us/articles/40144634985618-What-kind-of-C-development-Rider-supports-How-to-develop-non-Unreal-Engine-C-projects-in-Rider) documents that solution/project route for Rider. Sparkle currently defaults to that generator family, and some parallel-compilation policy is generator-specific. Preserve required build-tool discovery, including vswhere if the selected compiler setup uses it, while removing IDE policy. Do not promise a Ninja-only migration or delete generated-project support before proving project loading, navigation, building and native debugging in your installed Rider.

Inspect `Private/Build/Toolchains/BuildToolchainDetection.cpp`, `CMakeGeneratorModel.cpp`, `VisualStudioToolchainDiscovery.*`, tool resolution, IDE-opening actions, GUI settings, `CMake/Workspace/Projects.cmake` and `VS_*` project properties. Classify each responsibility as IDE-only or required compiler/build/project support. Rename retained owners around their actual build-tool responsibility where appropriate. Replacing labels alone is not cleanup. Keep the working current setup until the selected replacement succeeds; no installed toolchain is being uninstalled here.

### Reviewer inspection bundle

Deliver Launcher, Editor and Game together with their engine DLLs, required third-party runtimes, cooked shaders/content, configuration, notices and concise instructions. A reviewer must be able to extract and run the promised experience offline on the supported system without Rider, Visual Studio IDE, CMake, compilers, source downloads or cooking. Explicit hardware/driver/runtime requirements still apply. Missing runtime files are packaging defects, not a request that the reviewer builds them.

Reviewer Quick Start opens the map list and provides clear Editor/Game launch actions from supplied content, with the actual graphics API/hardware prerequisites reported. After launch, the approved runtime controls/effects, scene inspection, camera movement and basic animated examples work without cooking, compilation or source downloads. Missing cooked data is a package/readiness error, not an invitation to install a toolchain. Keep the supported route back to supplied defaults clear.

Build/cook/sync/clean and shader recompile stay in your source-workspace Launcher/Editor. Their absence from the inspection environment must not block ready-to-run scenes. AP-01 remains retained for your development Editor; do not ship compiler payloads merely to make an unavailable developer button appear active. Optional native debugger integrations report their own prerequisites without blocking normal inspection.

Provide matching source links or a suitable source companion for code inspection, with three application entry points and a concise scene-to-pixel/module ownership guide. No contributor setup, public engine extension API, model-training workflow or reviewer-generated shader/content artifact is part of Quick Start.

Source remains available as portfolio evidence and your own reproducible development workflow. Matching-source product import, first-time contributor onboarding and reviewer build/cook reproduction are not release acceptance requirements. Any source companions or notices required by dependency distribution terms remain required. Validate the actual extracted runtime bundle, all three products and selected scenes at the final source/content identity.

Keep one artifact layout and authoritative dependency declarations. The three products share real build policy; copying it into separate scripts is not simplification.

### Build, packaging and automation: additional decisions

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| BU-07 | **Developer cooker/compiler payloads in reviewer packages** | Ship runtime Launcher/Editor/Game and their actual dependencies. Keep developer tools in your source workspace; inspection does not need compilers or recooking. Source companions/required notices remain separate obligations. | No | [product scope](../Goals.md#product-scope): a runtime inspection bundle is not a development SDK. | - |
| BU-08 | **Mandatory bespoke SBOM/provenance package artifacts** | Prefer one minimal file/dependency inventory with revision/input identity and required notices. Preserve exact shader/debug identity and runtime closure. This is a packaging-workflow choice, not blanket removal of manifests/hashes. | No* | [pipeline precedents](../References.md#pipeline-automation): retain a useful package consumer/inventory without adopting a larger release-process inventory by imitation. | - |
| BU-09 | **Unused build-feature combination matrix** | Retire unused optional-subsystem combinations. Preserve composition switches needed by BU-03 and intentional capture/crash-debug modes, plus both retained APIs. Do not replace a matrix with duplicated build policy. | No* | [product scope](../Goals.md#product-scope): qualify real products and diagnostic modes, not speculative subsystem combinations. | - |
| BU-10 | **Batch wrapper around PowerShell code-style tooling** | Use one documented PowerShell/Rider external-tool command, updating callers. Keep the formatter, project-specific style rules and your ability to refine them under BU-06. Only a small wrapper is at issue. | No* | [code ownership/style](../References.md#code-style): keep refinable style rules through one supported entry point; tiny direct savings. | - |
| BU-11 | **Existing lightweight GitHub source checks** | No removes a 41-line automatic check, not a large CI farm. Keep it if useful; a broad compiler/GPU/reviewer-build matrix is not present in this workflow. | Yes | [pipeline precedents](../References.md#pipeline-automation): a concrete 41-line check is already small; it is not broad CI infrastructure. | - |
| BU-12 | **Optional native sanitizer build support** | No removes a small debugging facility. Keep supported sanitizer use for ownership/lifetime investigation; it is not a reviewer runtime requirement or proof that sanitizer runs have passed. | Yes | [performance](../Goals.md#software-and-analysis): lifetime debugging can justify this small developer facility. | - |

Sources: [release inventory](../../CMake/Packaging/ReleaseProducts.cmake), [package inputs](../../CMake/Packaging/Payload/PackageInputs.ps1), [workspace features](../../CMake/SparkleWorkspaceFeatures.cmake), [style wrapper](../../CMake/CodeStyle.bat), [sanitizer policy](../../CMake/Build/Sanitizers.cmake).

BU-05 remains a single unanswered choice: a No retires `SyncProducts.ps1`, `Publication/ProductImport.ps1` and their product-import-only callers. It does not retire packaging, source/content sync, shared runtime inventories or file publication. Reviewer extraction still requires an actual three-product/offline run; source checks cannot establish it.

## Third-party integration maintenance

Owner: [ThirdParty](../../ThirdParty), with runtime semantic adapters in their consuming modules.

You maintain the selected integrations and understand their Sparkle contracts. Reviewers receive required runtime payloads/notices, while optional external debugging tools keep separate prerequisites.

| ID | Keep this policy/capability? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| TP-01 | Use the already integrated libraries for logging, JSON, processes, tasks, windows/input, UI, allocation and shader compilation, while owning Sparkle's contracts and policy. A No implies selectively replacing those integrations. | Yes | - |
| TP-02 | Keep maintained upstream patches/forks whose behavior is still required by retained workflows. A No needs an identified unpatched equivalent or retirement of the dependent capability. | Yes* | - |

Keep ozz for retained basic skeletal animation and Assimp for the Knight FBX route. Other dependency retirement follows the last real consumer: KTX is retired under TX-03, while SDK payloads retire after their actual integrations disappear. TP-05 now explicitly reviews the native NVAPI provider; prove its complete last-consumer closure before retiring SDK acquisition or headers. Keep Aftermath, Streamline and native DRED separate. Bindings and patches count as first-party understanding/maintenance regardless of their folder.

### Third-party maintenance: additional decisions

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| TP-03 | **Project-maintained enkiTS runtime patch surface** | Re-audit each extension against remaining Launcher, loading, frame and shutdown consumers. Retire only extensions whose last consumer disappears; retained completion, failure and native-argument lifetime contracts must have a proven replacement. Do not promise the whole fork can go. | No* | [ownership](../Goals.md#north-star): own every retained scheduler extension and retire only proven unused obligations. | - |
| TP-04 | **Pinned Slang debug/source-correlation patches** | No needs a verified compiler with equivalent behavior. Keep required symbols/source correlation; removing analysis UI does not establish that these patches are redundant. | Yes* | [debugging evidence](../References.md#nvidia-production-studies): debug exports remain useful until a verified compiler removes the need. | - |
| TP-05 | **Native NVAPI ray-tracing provider and SDK integration** | The inspected provider exposes PTLAS capabilities/building. Retire it and the SDK only after checking all remaining native, shader and build consumers. Standard DXR, Aftermath/DRED, Streamline/DLSS and generic NVIDIA tool launch remain separate. | No* | [product scope](../Goals.md#product-scope): the selected PTLAS cut should retire its exclusive dependencies, not other NVIDIA SDKs. | - |

Sources: [enkiTS integration](../../ThirdParty/enkits/Dependency.cmake), [runtime patch](../../ThirdParty/enkits/Patches/enkiTS-sparkle-runtime.patch), [Slang source-location patch](../../ThirdParty/slang/Patches/SlangLoopSourceLocations.patch), [Slang debug-format patch](../../ThirdParty/slang/Patches/SlangDebugInformationFormat.patch). Maintained patches are your review burden even inside ThirdParty. A library's upstream source size is not your first-party line count; its supported features, binary/build cost and patch surface are different costs.

## Documentation and future scope

Owners: [Docs](..), source instructions, build/cook documentation and dependency notices.

Write a short runtime walkthrough and code-architecture entry point for reviewers, plus precise development/build/cook guidance for you. Avoid contributor onboarding and generic engine-consumer documentation.

| ID | Keep this commitment? | Current recommendation | Your answer |
| --- | --- | --- | --- |
| DO-01 | Require three complete case narratives before delivering the selected engine. Recommend finishing the reviewer walkthrough and inspectable architecture first; case narratives can draw on that work. | No | - |
| DO-02 | Treat optional future rendering/SDK/neural features as cleanup-release prerequisites. Recommend keeping them outside the current reviewer delivery unless separately selected. | No | - |

The already chosen direction remains: Sparkle is the main piece, every retained first-party responsibility must be understood, and separate experiment repositories come after cleanup. Preserve the useful professional/role references and current correctness limitations. Remove completed diaries, duplicate policy and obsolete feature instructions as their owners change.

### Documentation: additional decision

| ID | Keep this capability or mechanism? | Precise removal boundary / retained behavior | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| DO-03 | **Permanent detailed cleanup worksheet after delivery** | Once choices are implemented, keep a compact accepted-scope record and the actual architecture/data-flow entry point; retire completed action detail. Do not erase unresolved decisions or required ownership explanations. | No | [reviewer usability](../Goals.md#review-and-transfer): durable architecture guidance should replace completed planning detail. | - |

## Larger cuts that would reopen earlier decisions

These are alternatives for a second reduction pass if the normal refinements leave more scope than you want. They are not approved, and none changes an existing Yes until you explicitly choose it. Each asks whether to keep the named capability.

| ID | Keep this earlier commitment? | What No would buy and cost | Recommendation | Portfolio reason | Your answer |
| --- | --- | --- | --- | --- | --- |
| RV-01 | **Launcher developer workflow GUI beyond Quick Start** | Keep those operations in CLI and retain a small graphical scene launcher. This attacks more of the 8,101-line GUI, but changes the GUI interpretation of LA-03 through LA-10 and your stated Launcher development workflow. It does not remove the operation owners wholesale. | Yes, given your stated workflow | [product scope](../Goals.md#product-scope): your explicitly retained GUI workflow currently justifies this scope; No must reopen that choice. | - |
| RV-02 | **In-process Editor map switching** | One scene per process can simplify replacement/cancellation/state transitions. This changes ED-10/GF-04; startup still needs loading, useful errors and responsive presentation, so the whole level/asset module cannot disappear. | Yes, unless you choose this tradeoff | [product scope](../Goals.md#product-scope): useful inspection behavior, with a larger simplification available only if you accept relaunching. | - |
| RV-03 | **Current typed ECS scene implementation** | A fixed scene representation may remove generic storage/mutation machinery, but requires redesigning loading, edits, identities, animation and extraction. This reopens GF-05 and is a rewrite with uncertain net savings, not an easy folder deletion. | Yes; narrow current ownership first | [ownership](../Goals.md#north-star): narrow existing responsibilities before undertaking a representation rewrite. | - |
| RV-04 | **In-repository FBX source pipeline** | A verified externally prepared glTF/GLB route could retire FBX/shared Assimp adapters and perhaps Assimp itself after the separate PLY choice is resolved. This changes IM-02 and retained content preparation. Conversion must preserve animation, camera/light units, materials and redistribution/reproduction inputs before any source route is removed. | Yes, until an alternative is proven | [product scope](../Goals.md#product-scope): retained Bistro/Knight inputs justify it until an equivalent prepared route is proven. | - |
| RV-05 | **PLY source import without a retained PLY scene** | Reopens IM-03: remove the PLY adapter/dispatch/build option if no retained input needs it. This is only 37 adapter lines; Assimp remains while FBX does, so do not sell this as a large dependency removal. | No, if no retained input | [product scope](../Goals.md#product-scope): reconsider unused input scope explicitly; this reverses IM-03 only with your answer. | - |

The strongest first pass is the approved feature cuts plus LA-16 through LA-25, GF-08/GF-13 and the Editor/tool/package refinements. If that still exceeds your ownership budget, RV-01/RV-02 are the clearest capability reductions to discuss before RV-03's representation rewrite. There is no defensible fixed percentage reduction yet: conditional replacements are unimplemented, scopes overlap and the retained large scenes impose real obligations.

For each accepted cut, list the actual producer, consumers, state lifetime, build membership and dependency before editing; then remove its last-use closure in one coherent change. For each retained large subsystem, produce a short path from entry point to final consumer that you can explain yourself. This planning pass does not implement new decisions.

## Precise deletion names and protected neighbors

Use the decision ID and the full capability name when discussing a cut. Broad words below are search terms, not deletion boundaries. A file containing both responsibilities must be narrowed before removal.

| Name of the removable item | Explicitly retained neighbor |
| --- | --- |
| **Launcher GPU-crash incident observer, history/catalog and managed inspector workflow** | Native crash dump/debug-data persistence, DRED/Aftermath breadcrumbs/markers, Vulkan checkpoints, matched shader identity and bounded completion. |
| **Independent Reference path-tracer renderer and its checkpoint/export pipeline** | The real-time path tracer; shared BSDF/material/sampling code with real-time consumers; external/analytic validation controls and ordinary screenshots. |
| **Mesh/texture/shader inspection panels and their unused diagnostic producers** | Scene outliner/inspection, signal/debug views, shader compile/reload, ABI reflection, native captures and shader symbols. |
| **Morph target/weight deformation pipeline** | Skeletons, skeletal clips, poses, joint palettes, skinning, previous-frame motion and required BLAS/TLAS updates. |
| **Visual Studio IDE actions/settings** | The compiler, Windows SDK, required discovery, MSBuild and generated solution/project files when consumed by the proven Rider route. |
| **HDR display/output mode support** | HDR/EXR source textures, floating-point lighting data, exposure, SDR tone mapping and chosen HDR texture encoding. |
| **Optional shader catalog/analysis reports** | Compiler errors, host/shader ABI checks, consumed reflection and debugger/source-correlation artifacts. |
| **Cooked-product import into another source workspace** | Source dependency sync, selected content acquisition, cooking, runtime packaging and ordinary extraction. |
| **Generated native/third-party compilation outputs in a chosen clean scope** | Downloaded sources, original assets and unrelated captures/user state; source-cache retirement is a separate last-consumer decision. |
| **Cook-time animation curve resampling or runtime offline Ozz construction, if replaced** | Correct clip interpolation/bind data and basic skeletal playback. Neither name permits deleting all animation preview/evaluation code. |
| **PTLAS-specific NVAPI provider, if its last consumer disappears** | Standard DXR BLAS/TLAS/refit, Aftermath/DRED, Streamline/DLSS and optional Nsight/RenderDoc/PIX launch. |

## Audit coverage and next implementation boundary

The footprint inventory covers every tracked, present code/script owner under the stated counting rule. The capability review follows the largest support areas and their public contracts/callers; it is not a claim that all 159,223 lines received a correctness review or that runtime/package qualification was performed.

| Audited area | Decision coverage / remaining implementation question |
| --- | --- |
| Launcher GUI, native build/toolchains, operation service, startup/CLI, scene run/sync, cook and maintenance | LA-01 through LA-28 cover the user-facing obligations and the largest independent mechanisms. Trace unused observers, display models and operation fields to their final consumers after answers. No Launcher live-copy implementation was found in the inspected current source; LA-13 remains excluded scope, not claimed deletion savings. |
| Editor workspace/widgets/icons, scene commands, viewport/settings, console, inspection panels | ED-01 through ED-24 bound live edits, save behavior and optional presentation. Retire panel-only Renderer/host producers too; do not keep expensive producer work after removing its window. |
| World/ECS, loading/staging, asset readers, animation, resource stores, render extraction | GF-01 through GF-16 cover representation, execution, publication/copying and preparation. Asset allocation bounds, failed-load preservation and construction rollback remain correctness work. |
| Application capture, restart/settings and diagnostics | AP-01 through AP-11 plus the protected diagnostics table distinguish normal runtime, development and native fault collection. |
| Core/Platform and shared tool catalogs/paths | CO/PL/TS decisions distinguish reusable mechanisms from product policy. No independent gameplay, physics, audio, networking, scripting, controller/VR or public-plugin module appears among the current module owners; do not manufacture savings from absent systems. |
| Importers, cookers and source-content routes | IM/AC/SC/MC/MA/TX and PR decisions cover the admitted data path. MeshCooker and MaterialCooker remain intentionally small retained boundaries; no extra format/schema abstraction is justified just to simplify their names. |
| ShaderCompiler, source mounts/reflection, debug publication and maintained patches | CP/TP decisions preserve the measured-frame/debugging path. CP-04 remains conditional on a proven compiler-owned replacement, including source identity. |
| Build graph, deployment, package metadata/import, style and CI | BU/TP/DO decisions cover the actual repository automation. A 41-line CI check and small sanitizer/style helpers are not a hidden large automation suite. |
| Showcase scenes, conversion scripts, shader assets, Renderer and RHI | Existing PR/SH/RE/RH answers remain authoritative. Source formats, generated data, SDK/build consumers and runtime contracts must be pruned with their retired features; shader/RHI algorithms are not indirectly removed by a tool UI decision. |

For an accepted No, the implementation starts with a bounded list of exact files/functions/fields, their remaining consumers and the retained behavior that must still work. For a conditional No, first show the smaller replacement and acceptance limits. Validate affected success/failure paths and remove superseded code in the same change. Do not keep compatibility branches or add a new generic framework to manage this cleanup.

## GPU crash diagnostics preservation and removal

**Breadcrumb support is a retained requirement.** LA-15/AP-07 remove extra incident management, not native crash diagnostics. This boundary supersedes any broader interpretation of "remove post-crash handling" elsewhere in the plan. In particular, Launcher crash-state polling can go while the engine's bounded SDK collection-status wait must remain where required to finish writing the dump.

The following source owners were inspected. Their presence is source evidence, not proof of correctly decoded crash output on a GPU.

| Keep or protect | Current owner and reason |
| --- | --- |
| D3D12 DRED setup and fault collection | `Engine/RHI/Private/D3D12/Diagnostics/D3D12DebugLayer.cpp`, `D3D12Dred.cpp` and `GpuCrash/D3D12GpuCrash.cpp`: preserve pre-device setup, automatic breadcrumbs and native device-removal/page-fault observations. |
| D3D12 Aftermath marker emission | `D3D12GpuCrash.cpp` and `Commands/D3D12RenderCommandList.cpp`: preserve command-list contexts, begin/end/insert markers and queue/recording context needed to interpret them. |
| Vulkan diagnostic checkpoints | `Engine/RHI/Private/Vulkan/Diagnostics/GpuCrash/VulkanGpuCrash.cpp` and `Commands/VulkanRenderCommandList.cpp`: preserve capability checks, device configuration, checkpoint emission and queue checkpoint collection on supported devices. Do not promise extension support on every Vulkan GPU. |
| Marker identity, text and lifetime | `Engine/RHI/Private/Diagnostics/GpuCrash/GpuCrashSession.cpp`, its header and `GpuCrashCallbacks.cpp`: preserve semantic scope nesting, marker/token resolution, storage until native consumers finish and explicit incomplete/overflow reporting. |
| SDK callbacks and bounded completion | The same common RHI owners: retain dump/debug callbacks, fault collection, the necessary Aftermath completion wait and safe callback/writer settlement. Do not terminate or free borrowed data before diagnostics are consumed. |
| Native diagnostic persistence | `Engine/Application/Private/Diagnostics/GpuCrash/GpuCrashIncidentSink.cpp`, its header and `GpuCrashIncidentFiles.cpp`: preserve writing the dump, shader debug information, native diagnostic data and marker context required for external investigation. These are mixed-responsibility owners; do not delete them wholesale because their names contain Incident. |
| Matching symbols and source identity | `GpuCrashInputs.cpp`, `GpuCrashInputVerification.cpp`, `GpuCrashShaderInputs.cpp`, ShaderCompiler exports and `CMake/Diagnostics/GpuCrashArtifacts.*`/`GpuCrashInputs.*`: preserve the necessary build/shader/SDK associations, usable local paths, input checks and runtime staging. Simplifying presentation must not break source correlation. |
| Debugger and diagnostic-mode launch | Launcher tool configuration, `Private/GpuCrash/GpuCrashLaunch.cpp` and Editor startup: retain manual tool launch, correct diagnostic-mode arguments and actionable prerequisite errors. |

The removal list is deliberately narrower:

| Remove | Exact intended boundary |
| --- | --- |
| Launcher incident presentation | Incident state, status display, refresh/inspection actions and incident-specific dialogs in `Private/Gui/GpuCrash/LauncherGpuCrashPanel.*`. Split out and retain tool configuration/launch and diagnostic-mode controls before removing the incident UI. |
| Launcher incident observation | `ObserveGpuCrash` and associated backend/service observers, callbacks and polling. Do not remove general operation progress or child exit/error reporting. |
| Crash-announcement-to-UI plumbing | `ReadGpuCrashAnnouncement`/`ReadGpuCrashState` use in `Private/Levels/LevelRunOperationExecutor.cpp` and `OperationRecord` incident fields when no retained consumer needs them. Do not confuse them with the child's native dump-writing path. |
| Managed incident handoff | Launcher incident-specific inspector actions and the custom receipt/terminal-state/bundle workflow in `CMake/Diagnostics/OpenGpuCrash.ps1` and its callers. Retain a simple explicit debugger launch plus any data/tool inputs needed for manual diagnosis. |
| Extra incident catalogs and reporting | State polling, report/index presentation and bespoke packaging solely serving the removed managed incident workflow. Retain any minimal sidecar/metadata needed to associate breadcrumbs, native faults, shaders, dumps and symbols. |
| Automatic analysis or recovery workflow | Remove such orchestration wherever it exists; do not add it. External Aftermath/Nsight tools perform investigation. Necessary engine-side collection after device loss remains. |

Before an implementation cut, show its exact files/functions and classify each responsibility as removed, preserved or moved. Mixed files must be split or narrowed before deletion. No folder-wide GPU-crash deletion, blanket removal of manifests/JSON, or blanket ban on post-fault work is justified by LA-15/AP-07.

Acceptance requires a diagnostic-mode run on each retained supported API: ordinary frames emit meaningful scopes; a controlled GPU-fault reproduction in a dedicated diagnostic run produces a readable dump/native diagnostic result with identifiable scopes and matched shader/debug data. Check marker lifetime across command-list reuse/frames in flight and collection completion during device loss/shutdown. Report unavailable, truncated or incomplete data explicitly; do not present missing markers as success. Validate that the external tools can consume the retained outputs without Launcher incident management. Choose the actual fault reproduction and environment before executing it; no crash was induced while preparing this plan.

## Reference path tracer removal across modules

**Decided, not pending:** AP-03 No, RE-04 No, RE-02 Yes. Both Editor and Game use the real-time path tracer as their only lighting path. Debug views, a rasterized GBuffer, exposure, reconstruction and upscaling can remain as parts of that frame; they do not introduce another lighting renderer. Other undecided algorithm/backend choices remain open within this boundary.

Remove the feature rather than hiding its menu, disabling it by default, moving it into a test executable or leaving it available behind a build flag. Existing source still contains the implementation; the table defines the required future removal.

| Module or area | Required removal or retained boundary |
| --- | --- |
| Launcher | Remove any Reference-only launch/configuration/readiness choices and stale descriptions. Preserve the selected general build/cook/run and capture-tool workflows. |
| Editor | Remove `Private/Viewport/ReferencePathTracer`, the Reference view-mode entry, overlay, progress/convergence/sample controls and dedicated artifact actions. ED-01/ED-15 Yes cover retained real-time/debug views and native captures only. |
| Application | Remove `Private/Editor/ReferencePathTracer` and its host wiring, writers, encoders, manifests, checkpoints, resume and background publication jobs. AP-02 ordinary viewport capture remains independently selectable. |
| Renderer | Remove `Private/Passes/Lighting/ReferencePathTracer`, its sessions/state/work scheduling, accumulation/statistics/display resources, registrations and branch in `SceneRenderingPasses.cpp`. Narrow viewport products/progress, settings and frame setup to real-time consumers. |
| Shader assets | Remove `Passes/Lighting/ReferencePathTracer`, dedicated inline/pipeline/display kernels, work flags and generated/registered payloads. Keep shared BSDF, visibility, sampling or material code only when used by real-time rendering. |
| RHI | Remove exclusively Reference-used contracts, reflection/layout data and resources after checking consumers. Retain native ray-tracing operations, shader tables, readback, barriers and resource retirement needed by the real-time frame or ordinary capture. |
| GameFramework | Remove any Reference-only settings or submission fields. Keep shared scene/material/light/camera representation and real-time extraction; do not remove scene data solely because Reference also consumed it. |
| SourceImporters and cookers | Remove Reference-only options, artifacts and payload fields if present. Keep required scene, mesh, material and texture cooking. CornellBox and other scene decisions remain independent. |
| ShaderCompiler | Remove Reference entry points/registrations from cook inputs, expected outputs and inspection modes. Keep reflection, symbols and source correlation for retained shaders. |
| Core and ToolSupport | Remove helpers only when Reference was their last consumer. Keep general publication, process, file I/O and lifetime guarantees needed by other products. |
| Platform | No automatic window/input removal; retire only any actual Reference-only interaction wiring. |
| Projects and configuration | Remove Reference-specific presets, settings, UI text and reproduction commands. Keep selected scenes as real-time workloads; do not equate removing Reference with removing transport validation scenes. |
| CMake, scripts and packaging | Remove dedicated build/registration membership, exported checkpoint/manifest expectations and package/validation rules. Retire Reference-only dependency payloads and notices after last-consumer checks. |
| ThirdParty | Do not remove TinyEXR, compression or other dependencies by name alone: texture loading and ordinary image export may still need them. Charge maintained bindings/patches to actual remaining consumers. |
| Documentation and validation | Remove Reference convergence/resume/export goals, bugs and release gates. Validate real-time rendering with analytic/unit controls, actual signal/native captures and suitable external comparisons, stating their domains and uncertainty. No separate Sparkle renderer is required. |

Ordinary image capture and shared GPU readback do not become Reference features merely because they were reused by it. Conversely, ordinary capture must not retain dedicated Reference checkpoint/state machinery. Historical reports can remain dated evidence, but their requirements do not override this scope decision.

## Coupled decisions to resolve together

These are consequences of the IDs above, not additional votes.

| Primary choice | Coupled scope |
| --- | --- |
| RE-04 / AP-03 Reference removal | Apply the module removal table above; RE-02 is the sole lighting path. Preserve only code with an actual retained consumer. |
| GF-01 / GF-02 animation and morphs | Keep basic skeleton/clip import, cooking, pose evaluation, skinning, previous-frame data, ray-tracing geometry updates and ozz for Knight. Remove morph-only tracks, weights, storage and GPU paths without deleting shared skeletal machinery. |
| IM-02 / IM-03 source formats | Source dispatch and Assimp-backed/Ply adapters; source admission/catalog/acquisition; retained scene sources; scene conversion; cook dependencies. Knight requires FBX and its base/Curtains assets. Standalone ModernSponza scene choices do not authorize removing those dependencies. Stanford scenes are now conditional on a supported non-PLY route; they cannot justify retaining PLY. The earlier general IM-03 answer remains separate. |
| GF-04 scene replacement | Editor Open Level; loading ownership, cancellation and publication; world generations; history/selection invalidation; Renderer resource retirement. A No changes the user workflow. |
| CO-01 runtime console/CVars | Editor console ED-11, application CVar overrides AP-06 and registration/execution users. Retained rendering settings still need their own explicit configuration path. |
| RH-02 Vulkan | Native backend, shader SPIR-V path, build/SDK inputs, Streamline interop, captures, deployment and independent GPU/image checks. |
| RH-03 PTLAS | RHI descriptors/commands/providers; Renderer scene partition/update state; settings persistence; Editor panel; shaders; vendor-specific build inputs still required by other features. |
| LA-14 / AP-05 / RH-09 native crash diagnostics | Protect breadcrumbs/markers, DRED, supported Vulkan checkpoints, required fault collection/persistence and bounded completion, debug identity and launch/staging. Follow the explicit keep/remove table above. |
| LA-15 / AP-07 extra incident management | Remove only the Launcher observation/UI and extra managed reporting/handoff workflow identified above. Native post-fault collection, marker sidecars and matched debug metadata remain protected. |
| RE-08 / RE-09 DLSS | Retain shared Streamline while either feature uses it; remove only feature-specific resources/options when one is retired. |
| RE-05 / RE-10 GBuffer and subsurface retention | Keep both GBuffer algorithms and their layouts/registrations, and subsurface material import/cook/runtime fields, shader math/payload/debug views and relevant UI. Dedicated scene choices remain separate. |
| RE-13 / RE-14 presentation | Renderer/shader paths, settings, Editor controls, native presentation metadata where applicable and tests of retained output. |
| TS-02 project generality | Launcher project resolution, ToolSupport, cook planning, CMake project discovery and product packaging. |
| BU-01 shared engine | Keep DLL exports/ABI, private PCHs, necessary static/object contributors and registration behavior, incremental builds and runtime closure. Remove only the alternate static-engine variant. |
| BU-02 / LA-04 Rider-only IDE | Remove Visual Studio IDE policy and duplicate choices. Preserve whichever compiler/MSBuild/generator/project support the proven Rider workflow needs. |
| BU-04 reviewer bundle | All three applications, engine/vendor DLLs and cooked inputs must run without a development setup. Keep maintainer build/cook/sync and AP-01 recompile separate from reviewer readiness. |

## Acceptance by workflow

| Workflow | Required evidence | Outside that workflow |
| --- | --- | --- |
| Your development iteration | Rider load/navigation/build/debug, DLL/PCH incremental behavior, Launcher prepare/sync/build/cook/clean, failed-operation recovery and protected native crash diagnostics | A matrix of other IDEs, other developers onboarding, general SDK/extension workflows |
| Reviewer Quick Start | Extracted bundle launches Launcher and every promised map/product offline on the supported system; no source/build/cook prerequisites; readiness is based on runtime dependencies | Automatic downloading/building/cooking to repair an incomplete inspection bundle |
| Reviewer exploration | Editor/Game controls, approved effect switches, debug views, movement/animation and return to supplied state work using prepared data | Editing source, compiling shaders or operating content converters |
| Reviewer code inspection | Matching source identity, obvious application/module entry points, traceable data flow and concise ownership/tradeoff explanations | An independent source build, developer installation or external adoption requirement |

Single-maintainer scope does not relax correctness, reproducibility, input/publication safety or your understanding of retained code. Verify your development workflow and the actual reviewer bundle separately. Runtime tests do not establish build-iteration performance, and successful builds do not establish reviewer/GPU/image correctness. External source-build reproduction is optional evidence, not reviewer acceptance.

## Review basis

The original checklist started from source revision `6dd51591b0890e71da5b4ae795ac730a0f49b695`. This expanded footprint/capability pass uses the post-KTX source at `20587c481dc03def914d600e20bc5ac4bd5fd081`, 25 November 2026, with documentation edits in progress. The earlier inventory used a different selection and must not be treated as a measured before/after reduction. The current inventory is 2,201 code/script files and 159,223 physical lines, plus separately counted shader source and maintained patch files as explained above. File hashes and local receipts are under `build/validation/portfolio-footprint-20261125/`.

Reviewed paths include Launcher workflow/toolchain/state/GUI owners, scene loading and world publication, Ozz load-time builders, Editor saving/commands, import/texture admission, shader compilation/debugging, dependency recipes and package composition. The source evidence supports the named mechanisms and their containing footprint, not a complete correctness audit or a guaranteed deletion percentage. References' archived role/persona studies inform relevance; they do not establish current vacancies, hiring outcomes or a requirement to reproduce another project's feature count.

All 129 existing answer cells were compared with the starting document and preserved; all 73 new answer cells remain blank. Recommendation names and reasons are tied to Goals/References, with explicit conditional checks and protected neighbors. This pass changes documentation only after the separately implemented KTX retirement. Its earlier tool checks used the existing root DevelopmentEditor configuration; they do not qualify the planned DLL/Rider migration, full engine behavior, images, GPU faults or an extracted reviewer package.

At 10?15 hours per week, start with complete removal chains and small bounded refinements before representation/format rewrites. After answers, derive implementation order from the actual retained dependencies and the two user workflows. Keep reviewer delivery and your personal understanding of the surviving source as the completion criteria.
