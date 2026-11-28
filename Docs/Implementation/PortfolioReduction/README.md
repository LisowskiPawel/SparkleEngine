# Portfolio reduction: implementation order and module plans

[Final scope decisions](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md) | [Goals](../../Goals.md) | [References](../../References.md) | [Roadmap](../../Roadmap.md) | [Code style](../../CodeStyle.md)

**Purpose:** deliver a functioning, substantially smaller engine whose retained source you can read, modify and explain. This is a staged refactor and complete-system retirement, not a filename purge. Sparkle stays the main portfolio piece: one maintainer at 10–15 hours/week; reviewer opens the prepared Launcher/Editor/Game bundle, explores ten scenes and inspects matching source. No new renderer, neural integration or developer SDK is added by this plan.

Decision selection is finished. Implementation and qualification are open. The worksheet's 194 stable IDs have exactly one dominant delivery owner below; recommendations are not silently promoted to user answers. Cross-module changes belong to the complete system owner. A touched module does not start a duplicate removal stage for that system.

## Authority and non-negotiable retained behavior

- [Goals / North Star](../../Goals.md#north-star): real results, understood math/data, proved claims, explicit ownership, usable evidence and less complexity. Keep personally reviewed source, not a generic rewrite generated to meet a size number.
- [Engineering / module boundaries](../../Engineering.md#module-boundaries), [data/lifetime](../../Engineering.md#data-and-lifetime) and [compatibility](../../Engineering.md#compatibility): narrow real owners, no dependency inversion, no internal compatibility versions/aliases/parallel legacy path. Regenerate disposable representations and remove the old producers/consumers together.
- [References / integration precedents](../../References.md#integration-precedents), [production studies](../../References.md#nvidia-production-studies) and [code style](../../References.md#code-style): use retained library/native mechanisms; own integration, invariants and maintenance. They motivate comparison/reuse, not imitation of every NVIDIA subsystem or a universal company tool standard.
- Separate game and render threads; bounded parallel world/load/cook/compile/recording work; data-oriented ranges/storage; Editor/UI affinity and game-owned mutation; immutable handoffs; both APIs, multiple GPU queues and fence-governed lifetime. One coordinator never means one worker. No serial replacement or coarse global lock as simplification.
- Real-time PT only; owned RHI/frame graph, both GBuffer routes, ReSTIR indirect, TLAS refit, DLSS SR/RR, subsurface, exposure/signals, basic Knight/Cesium animation, source cameras/lights/tangents, typed ECS, material variants and authored instance capability stay. Shared native capture/readback/EXR input is not Reference-only.
- Launcher iteration GUI/CLI, visual character, concurrent independent actions, actual-artifact readiness, path overrides and scene-ready handshake stay. Local style tooling stays; hosted source checks and sanitizer mode are selected for removal. Source/dependency/content sync is distinct from the undecided prepared-product import.
- Preserve native breadcrumbs/semantic markers, DRED, supported Vulkan checkpoints, Aftermath callbacks/sidecars/dumps and bounded completion, matched shader/native identity and tool launch. Remove only the extra managed incident workflow. A source check or successful launch never qualifies crash output.

This produces reviewer-ready engineering evidence, not a guarantee of a principal-level hiring outcome. The [role and professional studies](../../References.md#role-sources) stay dated; current job-market requirements were not resurveyed here. More readable ownership and measured correct behavior matter more than resemblance to sample code.

## Footprint and size targets

Live source basis: `039d7b802fbf2f3f46a076a4157c8e43ee27f662`, 25 November 2026, with the finalized documentation working tree. All **2,201 counted code/script files / 159,223 physical lines** still match their earlier file hashes. Renderer/RHI contain 74,620 lines together; the other counted code contains 84,603. Shader source (7,544 lines) and maintained patch text (1,071 lines) are separate. Fetched vendors, generated products, raw assets and these docs are not authored engine-code savings.

The remaining approved cuts identify **137 distinct whole-file candidates / 9,488 existing lines**, plus **416 distinct lines in shared files**, before replacement/caller cleanup. Whole-file candidates split into **8,909 code/script lines** and **579 shader lines**. Do not subtract shader/data lines from the code-only 159,223 baseline. Candidates include gated cuts: they are a gross retirement pool, not a promised final reduction or permission to delete the file without checking remaining consumers.

Completed scene/KTX cuts are excluded. Reference/Application exports, PLY/RV-05 and other aliases are counted once. Containing rewrite scopes in module docs overlap and must never be added as guaranteed savings.

**Planning objective:** first close complete removals and use the resulting architecture to pursue substantial net reduction in the largest support owners. A useful stretch review target is **20% of the 84,603 non-Renderer/RHI lines (about 16,900)**; this is a design target, not an evidence-backed forecast. A 50% repository reduction is not supported by the present choices: much Renderer/RHI and several support capabilities are explicitly retained. Do not promise that number or remove multithreading/features to manufacture it.

For sizing designs, use these provisional review ceilings: Launcher about 14,000 lines (from 19,662), Editor about 9,500 (12,005), GameFramework about 10,000 (12,223), ShaderCompiler about 6,500 (7,938), Core about 6,300 (7,278). They are deliberate pressure on replacement complexity, not blind acceptance thresholds. Source moving into cookers, DLL exports/ABI work and new ED-23 persistence must be added back across the repository. If a ceiling is not attainable under retained contracts, show the measured lower bound and largest surviving responsibilities; revise the design target transparently rather than hiding moved code or reopening kept capabilities.

Every stage records: whole files/lines removed; shared lines removed; new/replacement lines; moved lines; net code/shader/script change by owner; removed public APIs, owned mutable stores, mode/CLI switches and dependency edges; asset/package bytes separately. Zero net savings in a tiny input-admission change is valid. A replacement that grows overall policy/state without a required accepted behavior is reworked, not declared a simplification. ED-23 is an explicitly accepted addition and its cost remains visible.

## Module ownership and uniquely assigned retirement pool

The IDs in this table cover all 194 decision rows once. A module plan includes retained contracts and delegated participation even when it has no independent retirement. Whole-file columns are **files / physical lines**, shared-span columns are lines; shared file counts are not additive. All figures are prospective gross baselines with final-consumer and conditional gates.

| Dominant owner / plan | Decision IDs owned | Whole-file pool | Shared spans |
| --- | --- | ---: | ---: |
| [01. Renderer](01_Renderer.md) | AP-03, RE-01, RE-02, RE-03, RE-04, RE-05, RE-06, RE-07, RE-08, RE-09, RE-10, RE-11, RE-12, RE-13, RE-14, RE-15 | 53 / 3,095 | 57 |
| [02. Editor](02_Editor.md) | ED-01, ED-02, ED-03, ED-04, ED-05, ED-06, ED-07, ED-08, ED-09, ED-10, ED-11, ED-12, ED-13, ED-14, ED-15, ED-16, ED-17, ED-18, ED-19, ED-20, ED-21, ED-22, ED-23, ED-24 | 24 / 2,394 | 35 |
| [03. Launcher](03_Launcher.md) | LA-01, LA-02, LA-03, LA-05, LA-06, LA-07, LA-08, LA-09, LA-10, LA-11, LA-12, LA-13, LA-14, LA-15, LA-16, LA-19, LA-20, LA-21, LA-22, LA-23, LA-24, LA-25, LA-26, LA-27, LA-28, AP-07, RV-01 | 8 / 639 | 239 |
| [04. Showcase content and conversion scripts](04_Showcase.md) | PR-01, PR-02, PR-03, PR-04, PR-05, PR-07, PR-08, PR-11, PR-12, PR-15, PR-16 | 0 / 0 | 0 |
| [05. GameFramework and scene runtime](05_GameFramework.md) | GF-01, GF-02, GF-03, GF-04, GF-05, GF-06, GF-07, GF-08, GF-09, GF-10, GF-11, GF-12, GF-15, GF-16, RV-02, RV-03 | 8 / 355 | 1 |
| [06. RHI](06_RHI.md) | RH-01, RH-02, RH-03, RH-04, RH-05, RH-06, RH-07, RH-08, RH-09, TP-05 | 26 / 2,332 | 0 |
| [07. SourceImporters](07_SourceImporters.md) | IM-01, IM-02, IM-03, IM-04, IM-05, IM-06, IM-07, IM-08, IM-09, RV-04, RV-05 | 4 / 269 | 0 |
| [08. TextureCooker](08_TextureCooker.md) | TX-01, TX-02, TX-04, TX-05, TX-06, TX-07, TX-08 | 2 / 49 | 0 |
| [09. Build, packaging and repository automation](09_BuildPackaging.md) | LA-04, LA-17, LA-18, BU-01, BU-02, BU-03, BU-04, BU-05, BU-06, BU-07, BU-08, BU-09, BU-10, BU-11, BU-12 | 3 / 118 | 0 |
| [10. ToolSupport](10_ToolSupport.md) | TS-01, TS-02, TS-03, TS-04 | 0 / 0 | 0 |
| [11. Core](11_Core.md) | CO-01, CO-02, CO-03, CO-04, CO-05, CO-06 | 0 / 0 | 49 |
| [12. AssetCooker](12_AssetCooker.md) | AC-01, AC-02, AC-03, AC-04, AC-05, AC-06, AC-07 | 1 / 24 | 35 |
| [13. ShaderCompiler](13_ShaderCompiler.md) | CP-01, CP-02, CP-03, CP-04, CP-05, CP-06, CP-07, CP-08, CP-09, CP-10 | 8 / 213 | 0 |
| [14. SceneCooker](14_SceneCooker.md) | GF-13, GF-14, SC-01, SC-02, SC-03 | 0 / 0 | 0 |
| [15. MeshCooker](15_MeshCooker.md) | MC-01 | 0 / 0 | 0 |
| [16. MaterialCooker](16_MaterialCooker.md) | MA-01 | 0 / 0 | 0 |
| [17. Application hosts](17_Application.md) | AP-01, AP-02, AP-04, AP-05, AP-06, AP-08, AP-09, AP-10, AP-11 | 0 / 0 | 0 |
| [18. Shader assets and rendering data](18_ShaderAssets.md) | SH-01, SH-02, SH-03 | 0 / 0 | 0 |
| [19. Platform](19_Platform.md) | PL-01, PL-02, PL-03, PL-04 | 0 / 0 | 0 |
| [20. Third-party integration maintenance](20_ThirdParty.md) | TP-01, TP-02, TP-03, TP-04 | 0 / 0 | 0 |
| [21. Documentation and ownership handover](21_Documentation.md) | DO-01, DO-02, DO-03 | 0 / 0 | 0 |

## Cross-module systems: one owner each

| Complete system / responsibility | Dominant stage | Other modules changed in that stage |
| --- | --- | --- |
| Independent Reference PT, sessions/checkpoints/exports/UI/shaders | Renderer 1 | Application, Editor, ShaderAssets, ShaderCompiler, Core-only helpers, CMake/package/docs |
| Extra post-crash monitoring/catalog/inspection | Launcher 2 | Application and CMake diagnostics; protected native RHI/Renderer collectors remain |
| Morph targets/weight animation/storage/shaders | GameFramework 1 | Importers, cookers, Editor, Renderer, ShaderAssets and regenerated products |
| PTLAS plus exclusive native NVAPI dependency | RHI 1–2 | Renderer strategy, Editor settings, shaders, recipes/build/package/notices |
| Runtime Ozz construction -> cooked ready runtime records | SceneCooker 1 | SourceImporters, GameFramework, dependency linkage and generated assets |
| One prepared Knight scene / remove runtime multi-asset composition | SceneCooker 2 | Showcase fixed recipe, AssetCooker, GameFramework level/loader, Editor saved identity |
| General user-project discovery | ToolSupport 1 | Core/product roots, Launcher, cookers, CMake and packaging; Showcase only |
| Product-root/layout authority | Core 2 | Application/Launcher/tool composition and package-relative root resolution |
| Sole native toolchain/DLL/PCH route | BuildPackaging 1 | Launcher selection/configure/open behavior, engine/dependency exports and deployment |
| Editor asset/shader diagnostic frontend and exclusive producers | Editor 1 | Application/Renderer diagnostic producers, Core/API/build dependencies |
| Narrow maintainer object save; no package authoring | Editor 3 | GameFramework level writer/projection, cooked/source object identity and runtime capabilities |

## Suggested execution order

**Start with `01_Renderer.md`, Stage 0 then Stage 1.** Module filenames are numbered `01_` through `21_` in order of their first focused implementation visit; zero padding keeps filesystem sorting correct. `README.md` stays the entry point. The ownership table above uses the same order.

**Use the stage queue below for execution, not a complete-file pass.** Several owners span dependencies: GameFramework's main rewrite needs prepared scenes; Launcher crash qualification needs the selected build; the final package needs all product/dependency cuts. Each file states its first stages and return point. Stage numbers inside files and decision IDs stay stable. Complete one bounded stage, then take the next queued stage; never carry a temporary legacy route to force filename order.

Run Stage 0 before the first edit in each affected owner. Cross-module participants may be changed before their numbered standalone visit, under the dominant system's stage; verify and prune their distinct exits together when actually satisfied. A Stage 0-only checkpoint does not advance implementation or qualify a stage.

| Wave | Execute these stages from left to right | Dependency / reason |
| --- | --- | --- |
| 0 | Shared baseline + Stage 0 for the next owner and affected participants | Read this index first. Freeze source/working tree, content, diagnostics, image/cost controls and thread/data ownership before editing. Repeat Stage 0 when the source contract has changed; never infer qualification. |
| 1 | [Renderer](01_Renderer.md) 1; [Editor](02_Editor.md) 1-2; [Launcher](03_Launcher.md) 1 (temporary cleanup only) | First visits 01-03: remove complete optional systems before larger replacement work. Preserve the current build route until its replacement qualifies. |
| 2 | [GameFramework](05_GameFramework.md) 1; [RHI](06_RHI.md) 1-2; [SourceImporters](07_SourceImporters.md) 1; [TextureCooker](08_TextureCooker.md) 1 | First visits 05-08: close morph/PTLAS and unused input chains. Update shared shader/cooked ABI consumers in each dominant cut. |
| 3 | [BuildPackaging](09_BuildPackaging.md) 1; [Launcher](03_Launcher.md) 2 (native qualification and cleanup only) | First visit 09, then return to 03: qualify Clang/Ninja/DLLs before final native dump/source/symbol evidence. Earlier diagnostic investigation is allowed, but final acceptance matches the selected build identity. |
| 4 | [ToolSupport](10_ToolSupport.md) 1; [Core](11_Core.md) 1-2; [AssetCooker](12_AssetCooker.md) 1; [ShaderCompiler](13_ShaderCompiler.md) 1 | First visits 10-13: establish one Showcase/root/argument authority. Coordinate Core stage 2 with ToolSupport stage 1 as one root/product handoff, preserving real callers and accepted outputs. |
| 5 | [SourceImporters](07_SourceImporters.md) 2; [SceneCooker](14_SceneCooker.md) 1-2 (stage 2 includes Showcase 2 and AssetCooker 2); [SourceImporters](07_SourceImporters.md) 3; [MeshCooker](15_MeshCooker.md) 1; [MaterialCooker](16_MaterialCooker.md) 1; [SceneCooker](14_SceneCooker.md) 3; [TextureCooker](08_TextureCooker.md) 2; [AssetCooker](12_AssetCooker.md) 3; [Showcase](04_Showcase.md) 3; [ToolSupport](10_ToolSupport.md) 2 | First visits 14-16, with the named returns: preserve Knight/Cesium, prepare one Knight scene, then close the narrower importer/cooker/catalog chain. Conditional animation cuts stay open if equivalence fails; do not block a safe independent retained path or fake completion. |
| 6 | [GameFramework](05_GameFramework.md) 2-3; [Editor](02_Editor.md) 3-4; [Launcher](03_Launcher.md) 3-4 | Return to 05, 02 and 03: prepared scene/animation and root contracts now support the largest loading/resource/read-view/save/status/UI reductions. Coordinate Editor stage 3 with GameFramework stage 3 in the same writer/identity change. |
| 7 | [Application](17_Application.md) 1-2; [GameFramework](05_GameFramework.md) 4; [Renderer](01_Renderer.md) 2-4; [ShaderCompiler](13_ShaderCompiler.md) 2-3; [RHI](06_RHI.md) 3; [ShaderAssets](18_ShaderAssets.md) 1; [Core](11_Core.md) 3; [Platform](19_Platform.md) 1 | First visits 17-19, with the named returns: close production host/thread ownership, final frame/shader/RHI contracts and neutral/native consumers. Apply batching/preprocessing cost gates. ShaderAssets and Platform participate earlier where their dominant system changes them; this wave closes their retained-boundary checks. |
| 8 | [BuildPackaging](09_BuildPackaging.md) 2-3; [ThirdParty](20_ThirdParty.md) 1-2; [BuildPackaging](09_BuildPackaging.md) 4; [Documentation](21_Documentation.md) 1-2 | First visits 20-21, with the named returns: finish dependency/patch and payload cuts before qualifying the final offline bundle and source handover. Documentation maintenance/pruning runs during every stage; final package/native evidence must match the final changed identity. |

Each remaining delivery stage is assigned once in this queue, including Showcase stage 2 and AssetCooker stage 2 within the SceneCooker stage 2 vertical change. Within a named range, take stages in ascending order. Documentation cleanup, native diagnostics and retained thread/lifetime obligations apply throughout; the final visits do not postpone necessary integration work. If a prerequisite is unmet, leave that dependent stage open and take only independent queued work. A later source/build/shader change can invalidate earlier observations and requires the affected identity-bound checks again.

## Stage protocol

**Stage 0, before the first edit in an owner:** read the named guides and current actual code/callers. Inspect `git status`, HEAD and relevant working-tree file hashes; preserve concurrent/unrelated work and do not cancel someone else's build. Refresh the inventory rather than trust old lines. List producer -> owner/worker -> representation -> consumer -> publication/destruction, source/SDK/cooked identities and build/dependency membership. Name what disappears and what remains, and sketch the smallest concrete target flow. For mixed files, classify functions/fields, not filenames.

Use migration outputs permitted by the existing artifact/layout contract, or an isolated checkout with its own canonical build root when changing generators. Do not invent an output layout or compete with another build.

Freeze a useful failing/working baseline and numeric comparison bounds before implementing a candidate. Existing known failures remain recorded: do not call an old corrupt render a successful baseline or quietly lower the retained behavior. Use Empty/CornellBox for bounded checks, Knight/Cesium for animation and Bistro for scale; add source-specific cases only for a real changed invariant. Check both native APIs independently. Baseline procedures use existing validation/capture/build owners; local probes are temporary and permanent test-only additions need explicit project approval.

**Each numbered stage:**

1. Implement the full owned change through all modules, registrations, CLI/settings/UI, cook formats, dependencies, build membership, runtime/package outputs and documentation. Keep one direct owner and bounded parallel work; remove the old path in the same delivered stage. No aliases, disabled scaffolding, copied stores or universal replacement framework.
2. Read the changed neighborhood with actual callers and apply the module's [surviving-design contract](#reshape-the-surviving-design). Follow [CodeStyle](../../CodeStyle.md), public/private dependency rules, stable data layout and source-oriented names. Use retained libraries/native APIs where they own the mechanism. Moving a function or wrapping a subsystem does not earn retirement credit.
3. Run the smallest meaningful source/style/build checks, then behavior/failure/lifetime/native/image/performance checks required by the change. A shared-library change needs a real DLL consumer; process success is not scene readiness; source metadata is not cooked/playback equivalence; successful compilation is not GPU correctness.
4. Record identity-bound commands, inputs, environment, expected tolerance, observation and unrun limits in ignored `build/validation/portfolio-reduction/<owner>-<stage>/`. Retain failure evidence. Remove temporary test/probe code from the submitted change; preserve existing tests unless an approved capability retires their contract.
5. Record the net footprint/state/API/dependency result and owner explanation. An absent invariant, serial replacement, new global lock, duplicate authority or unexplained footprint growth prevents stage completion. A conditional stage that cannot safely reduce the mechanism stays open with its retained minimum; it is not a reason to drop Knight, breadcrumbs or the frame graph.
6. After exits really pass, remove that stage's heading/checklist/prompt from its plan in the same change, keep stable numbers and repair links. Partial work keeps only unmet actions/gates. Do not mark a plan delivered based on source inspection, a test stub, or budget/time pressure.

Every module stage, including Stage 0, now contains a copyable implementation prompt with its own acceptance criteria and specific failure modes. Use that stage prompt rather than a generic continuation instruction. Each prompt incorporates this protocol and the CodeStyle responsibility review. It authorizes necessary cross-module refactors, not scope expansion or weakening checks. No user approval ceremony is introduced by this documentation workflow.

## Complete delivery, clean breaks and function responsibility

Every implementation prompt is a delivery contract: change all producers/consumers and remove retired files, settings, routes and output expectations in the owning stage. No backward-compatibility layer, legacy reader, alias, internal schema/ABI version, migration, disabled stub or permanent old/new path is allowed for replaced owned representations. Regenerate disposable output; preserve the deliberately supported external SDK/tool/runtime ABI. If a conditional replacement fails its gate, discard the candidate and retain one authorized working implementation with the cut still open. That is an unmet change, not permission to ship a legacy fallback or weaken Knight/Cesium/debug behavior.

Apply [CodeStyle's function responsibility rule](../../CodeStyle.md#function-responsibility-and-orchestration) to every changed function and its neighboring callers: one cohesive purpose/invariant, meaningful intent names, explicit data flow and ownership, workflow orchestration at one useful level of abstraction, and substantive mechanisms in cohesive operations. No line-count-driven fragmentation, forwarding-only helpers, numbered fragments, one-method classes or generic state bags. Keep useful small expressions and related work together. A formatter pass is required where applicable but cannot establish single responsibility.

Acceptance includes the stage-specific positive checks, named failure cases, complete caller/build/product cleanup, retained concurrency/lifetime behavior and a net reduction account. No stage is complete because only its main file changed, it compiled, or its old feature was merely hidden. Remove completed stage text only after all required evidence passes; no new permanent validation framework is introduced here.

## Reshape the surviving design

Removing a capability changes the assumptions of its callers and its former shared infrastructure. A stage is complete only when the surviving system uses that smaller scope directly. Deleting implementations while retaining an obsolete general protocol, mode selector or intermediate model is incomplete. Each module's **Adapt the surviving design** section defines its concrete target and checks; apply only the parts affected by that stage, without reopening accepted capabilities.

For each affected responsibility, record a short before/after design in the existing stage evidence:

| Design question | Required result |
| --- | --- |
| Which assumption disappeared? | Name the approved decision and the obsolete requirement: another user project, renderer, composition mode, compiler, input format, inspector or publication strategy. Distinguish this from deliberately retained alternatives. |
| What becomes simpler for the remaining consumer? | Show actual target signatures, fields/schema, state transitions, call sequence and dependency edges. Remove policy arguments, tags, registries, caches and synchronization that served only the retired branch; update callers to the smaller contract. Preserve checks for real external inputs and lifetimes. |
| Where does responsibility now belong? | Identify the policy owner, cohesive implementation operations and composition boundary. Merge forwarding-only layers sharing an invariant; keep independent invariants/lifetimes separate. Use intent-based functions at one useful abstraction level, not a compressed god function. |
| How does the change preserve parallel work? | Show game/render/UI/worker and queue handoffs, borrowed-data lifetime, admission bound, settlement and retirement. Remove obsolete coordination while retaining dependency-correct parallel capacity. Fewer states do not justify a global lock or synchronous replacement. |
| Which precedent helps? | Select a source below or another source in References.md. Record its relevant lesson, the local adaptation and excluded upstream machinery. Verify the pinned source/API actually being used; a reference is design evidence, never a local speed or correctness result. |
| What proves the design improved? | Exercise a real retained caller end to end and its changed failure/lifetime path. Show removed API/state/branch/dependency obligations and net footprint including replacements and callers. Explain surviving general boundaries by retained invariants or accepted capabilities. Mark unrelated rows not applicable with a reason; do not force a rewrite of an already small cohesive owner. |

Stage 0 specifies this design; delivery stages implement and verify it. Keep the comparison in the existing ignored receipt, not another architecture database or permanent audit tool. For example, the selected build route should remove redundant compiler/generator policy throughout Launcher and CMake, while still admitting both graphics APIs and six profiles. Moving multi-source preparation to cooking should leave one prepared-scene runtime contract, not runtime composition hidden behind a one-item list. Native GPU fault completion remains required even when its Launcher incident UI disappears.

### Reference choices and limits

These bounded precedents come from [References](../../References.md#integration-precedents) and its [code-style sources](../../References.md#code-style), checked against primary sources on 28 November 2026. The module adaptations are local design requirements, not claims that those projects implement Sparkle's exact solution. Modules select relevant rows; the implementer confirms exact APIs at Stage 0.

| Key / primary source | Lesson used here | Limit |
| --- | --- | --- |
| P1: [Unreal modules](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-modules) | Public contracts, private implementation and explicit dependency ownership. | Keep Sparkle's CMake/DLL composition; no UBT, UObject, plugin loader or general gameplay framework. |
| P2: [Unreal RDG](https://dev.epicgames.com/documentation/en-us/unreal-engine/render-dependency-graph-in-unreal-engine) | Typed pass parameters expose dependencies; graph execution supports parallel recording and resource lifetime. | Keep the owned frame graph/RHI and recognizable per-pass code. No Unreal renderer import, aggregate pass framework or serialized shortcut. |
| P3: [NVIDIA Donut composition](https://github.com/NVIDIA-RTX/Donut/blob/e50042f4bbd762971cc90431261e967404855b81/thirdparty/CMakeLists.txt) and [NVRHI targets](https://github.com/NVIDIA-RTX/NVRHI/blob/6b96fb03e07539f08327aea76c56d55f1de9d906/CMakeLists.txt) | Explicit library targets and backend/SDK composition. | Narrow Sparkle's selected target closure; do not copy optional-feature matrices or replace the owned RHI. No claimed internal NVIDIA compiler standard or measured build speedup. |
| P4: [NVIDIA ShaderMake frontend](https://github.com/NVIDIA-RTX/ShaderMake/blob/4f1ffd56dac27fc3d620ad7efeee42b4dd71fa72/ShaderMake/ShaderMake.cpp) | Compiler selection, work dispatch and output mechanics have an identifiable owner. | Delegate supported mechanics; keep Sparkle's typed ABI, reflection, include identity, debug correlation and accepted publication. No new shader framework. |
| P5: [NVIDIA Falcor graph](https://github.com/NVIDIAGameWorks/Falcor/blob/759aad033ff610fb0d82c74f7e0a508d0096d5f2/Source/Falcor/RenderGraph/RenderGraph.cpp#L424) | Changed graph structure explicitly invalidates compiled execution. | Use only where the retained graph needs it. This authorizes no new caching project, plugin graph or performance claim. |
| P6: [Qt item views](https://doc.qt.io/qt-6.11/qlistview.html) | Standard views consume models and support supplied presentation facilities. | Use retained Qt facilities directly while preserving Launcher appearance and UI affinity; no replacement widget framework. |

Other applicable sources in References include native SDL window/surface integration, Assimp build admission, DXC/Slang include/reflection APIs, SDK integration boundaries, CMake link/deployment rules and concise documentation examples. Choose them for an actual changed mechanism. Do not add TBB, a new scene/asset framework, a different serializer or a package manager merely because a referenced repository uses one. If a dependency owns a required mechanism, reuse its supported API and remove the displaced first-party implementation only after the complete retained path is qualified.

## Transitive retirement closure: repeat to a fixed point

Audit basis: all six authority documents were scanned on 28 November 2026; all 66 approved removal IDs still map to one dominant stage. The per-module **Named retirement closure** tables enumerate their direct and indirect retirement surfaces, with explicit retained consumers. Narrowed green capabilities also have closure entries: notably four-scope Clean, DLL/Rider policy, level grammar and bounded persistence. Decision coverage alone does not establish dependency closure or runtime proof.

For every removal, walk **incoming callers/producers and outgoing dependencies**, then repeat after each retirement. Classify each discovered node as **retired**, **retained with a named live consumer**, or **unresolved**. An unresolved exclusive dependency keeps the owning stage open. A helper used only by another removed helper is still part of the same cut. Do not stop when the visible feature or its top-level source directory disappears.

The closure manifest must cover these surfaces where applicable:

- Public/private types, functions, exports, forwarding adapters, enums/flags, callbacks/registries, fields, duplicated derived state, task/queue messages, caches, locks and lifetime/error paths.
- Host/UI/CLI commands, option aliases/defaults/persistence, menus/icons/status/preview/history models and generated metadata/facts.
- Imported/cooked/runtime/shader schemas, layout/reflection, indices/resources, registrations/includes/source maps, dependency/hash identities and validators. Regenerate disposable formats in place; no old reader/version migration or disabled fallback.
- CMake source/target/link edges (including private transitive and LINK_ONLY), includes/definitions/options, discovery/acquisition, patch/binding/source-admission identities, SDK runtime/plugin files, deployment/package inventories and only their now-unneeded notice/attribution writers.
- Selected content/recipes and every constituent source dependency, active cooked/cache/package products, scripts/automation/test/validation callers and live documentation commands/promises.

For a library/SDK, track source use, generated registrations, target closure and dynamically staged/plugin/runtime consumers. Zero text matches alone do not prove absence. Delete the recipe/target/acquisition/deployment only after the last real retained consumer vanishes; otherwise narrow it to required components (for example, disable upstream Assimp PLY while keeping FBX). Native sources, compiled third-party objects, runtime package bytes and raw content bytes are separate footprint measures. Retain required licenses; archived role/research references are not executable retired systems.

Remove stale products only from the controlled active output/layout selected by that owner, then recook/rebuild/repackage and verify the narrowed inventory. Do not purge historical validation snapshots, rehearsal archives, arbitrary downloads/captures or admitted source caches to make the footprint look smaller. A historical name is not a live consumer or an authorization for disk deletion. Acceptance requires removal from active inputs, outputs and supported workflows. Deleting ignored local copies, caches or snapshots and reclaiming their disk space are optional unless explicitly requested as deliverables.

Completion evidence: a revision-bound closure manifest, final consumer/target/runtime inventory, negative checks for retired entry routes and old output contracts, positive checks for retained consumers, and net source/API/state/dependency/compiled-payload counts including replacements. Update after deletion until no newly orphaned owned node remains. This is a scoped implementation receipt using actual owners/tools, not a new permanent source-text policy engine or generic dependency framework. Keep CodeStyle SRP/orchestration and parallel lifetime checks in the same closure review.

Concrete audit additions: CleanScope planners/executors/UI mappings under LA-09/10; Assimp's enabled PLY build component under IM-03/RV-05; GameFramework's Ozz offline link under GF-13; NVAPI generated dependency facts/acquisition/target options under TP-05; incident inspector script chains under LA-15/AP-07; validation/report consumers and retained SPIRV-Tools debug runtime; batch-wrapper documentation callers. The original gross line pool is not increased by these candidates without a refreshed unique measurement. Each is a required trace/closure, not permission to remove a shared retained dependency.

## Conditional cuts and recording gaps

IM-08/IM-09/SC-03 and SceneCooker/Ozz/flattening changes are conditional on native Knight/Cesium preparation and correct threaded runtime animation. Preserve required support if equivalence fails. CP-04 requires a smaller correct native include/debug/invalidation path. RE-07 needs equivalent-workload performance/memory bounds. Native GPU diagnostics require actual usable data after the final build/shader identity. These remain live gates even though decision selection is finished.

Five cells were still `-` in the frozen worksheet: **BU-05, BU-06, DO-01, DO-02, DO-03**. No new deletion or release prerequisite is inferred from them. Preserve current prepared-product import until BU-05 is explicitly decided; preserve local style tools as required by your earlier instructions/Engineering while BU-06's worksheet cell is blank. Do not make three case studies or future neural features a new prerequisite. Existing Engineering rules already require pruning completed implementation stages; DO-03's final worksheet-retention choice is not needed to begin approved removals. These recording gaps do not block independent authorized work.

Already completed scene/KTX removals, task-runtime replacement and SDL3 replacement are not reopened as implementation stages. Their product qualification limitations remain with their owning plans. PLY and the remaining source/build/runtime cuts still exist in source and are planned, not delivered. An owning stage retains only unmet cleanup or qualification work after a partial delivery.

## Final reduced-engine gate

The selected ten maps and both Editor/Game products run from Launcher using prepared data on D3D12/Vulkan; Knight/Cesium animate; approved controls/edits, undo, maintainer save and reviewer reset work. Both API resource/queue/thread contracts and native captures/breadcrumbs/crash exports remain usable. The source-workspace Launcher builds/cooks/syncs/cleans/debugs with the selected toolchain and actual artifact readiness; one-file/no-op iteration and parallel overlap are measured. The extracted runtime bundle works offline without an IDE/compiler/cooker; rights and existing rendering/Vulkan/material-parity limits are honestly resolved or stated before external presentation.

The owner can read the remaining first-party implementation, draw its data/lifetime/thread flow, explain key math/units/assumptions and make a small manual change through a real debugger/build. The architecture/source guide points directly to the actual owners. Publish the measured final footprint and largest remaining obligations alongside results, not a subjective masterpiece label. Principal-review readiness is demonstrated through these inspectable facts, attribution and rejected alternatives, not asserted from a line count.

## Release measurement and open qualification

These existing product requirements and observations are relocated from the high-level guides, not new release prerequisites or proof of completion. Preserve the limits in Roadmap and qualify the final affected identities. Earlier delivery receipts retain their original limits; module plans own the corresponding outstanding checks.

### Benchmark and frame protocol

The first benchmark system is the Ryzen 9 8940HX / GeForce RTX 5070 Ti Laptop machine with about 64 GiB RAM, 12 GiB GPU memory and NVMe storage. Record its actual OS, driver and build with every result.

Use ShippingGame at 1920x1080 with the declared native real-time Lit or DLSS Quality/Ray Reconstruction configuration, SDR output and the retired lens effects absent. Disable VSync and validation for measurement. After settling, warm up for at least 300 frames, then measure at least 300 valid frames. Repeat three times per scene and backend.

Presented, CPU and GPU 95th-percentile frame times must each be at most 33.33 ms. Report tails, exclusions, load time, memory and thermal behavior. Generated frames, dynamic quality, silent fallback or changing quality after measurement cannot satisfy the target.

### Supplied starting configuration

Target D3D12, real-time Lit, VSync On, automatic histogram exposure, ACES approximation, SDR and neutral grading. HDR display, vignette and chromatic aberration are selected for removal, not optional starting modes; existing source/settings still need implementation cleanup. VSync off is a benchmark setting, not a change to the default. Retain both GBuffer algorithms and existing subsurface shading within that frame.

### Existing product blockers

Scene exports and unsigned archives may support feature testing before publication approval; qualify the final package's actual files separately. The earlier package has a scene-publication interruption defect; package removal was outside its tested scope. Repairs need a newly tested archive; image, performance, native-API, reproduction and publication gates remain open. Later RELAX and renderer extensions are not release prerequisites.

Keep product blockers open through final qualification: raster/ray material parity for UV transforms, address modes and occlusion strength; the unresolved Vulkan swapchain allocation failure; and full image/package/reproduction checks on both APIs. Isolated cooker or sampling results do not close those gates. See the retained [texture-cooking boundary](../PortfolioScopeDecisions.md#texturecooker).

## Local evidence for this planning pass

`build/validation/portfolio-module-plans-20261125/` stores scope/source hashes, unique retirement allocation, full decision-to-stage ownership and review results. All 194 worksheet IDs and all 66 approved removal IDs are mapped exactly once. Older file-level selections were rechecked against current source; retired level records are excluded and Candles catalog spans were remeasured. These are source-planning receipts only; no implementation/build/cook/GPU/package acceptance was run by generating these plans.

Prompt revision: 28 November 2026. The remaining stages have individual prompts, acceptance criteria and specific failure modes. Historical source/footprint observations above remain dated; this revision does not qualify implementation behavior.

Survivor-design revision: 28 November 2026. All 21 module plans specify structural adaptations and checks; every stage prompt enforces the applicable design. Primary-source reference lessons and limits are linked above. `build/validation/portfolio-survivor-design-20261128/` records the documentation comparison, decision/stage/closure preservation and link/source checks; no production change or runtime acceptance is implied.
