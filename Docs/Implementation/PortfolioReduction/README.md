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

The approved cuts identify **138 distinct whole-file candidates / 9,509 existing lines**, plus **446 distinct lines in shared files**, before replacement/caller cleanup. Whole-file candidates split into **8,909 code/script lines**, **579 shader lines**, and **21 other text lines**. Do not subtract shader/data lines from the code-only 159,223 baseline. Candidates include gated cuts: they are a gross retirement pool, not a promised final reduction or permission to delete the file without checking remaining consumers.

The plan refreshes the Candles catalog spans by record identity; older catalog line numbers are stale. Completed KTX/earlier scene cuts are excluded. Reference/Application exports, PLY/RV-05 and other aliases are counted once. Containing rewrite scopes in module docs overlap and must never be added as guaranteed savings.

**Planning objective:** first close complete removals and use the resulting architecture to pursue substantial net reduction in the largest support owners. A useful stretch review target is **20% of the 84,603 non-Renderer/RHI lines (about 16,900)**; this is a design target, not an evidence-backed forecast. A 50% repository reduction is not supported by the present choices: much Renderer/RHI and several support capabilities are explicitly retained. Do not promise that number or remove multithreading/features to manufacture it.

For sizing designs, use these provisional review ceilings: Launcher about 14,000 lines (from 19,662), Editor about 9,500 (12,005), GameFramework about 10,000 (12,223), ShaderCompiler about 6,500 (7,938), Core about 6,300 (7,278). They are deliberate pressure on replacement complexity, not blind acceptance thresholds. Source moving into cookers, DLL exports/ABI work and new ED-23 persistence must be added back across the repository. If a ceiling is not attainable under retained contracts, show the measured lower bound and largest surviving responsibilities; revise the design target transparently rather than hiding moved code or reopening kept capabilities.

Every stage records: whole files/lines removed; shared lines removed; new/replacement lines; moved lines; net code/shader/script change by owner; removed public APIs, owned mutable stores, mode/CLI switches and dependency edges; asset/package bytes separately. Zero net savings in a tiny input-admission change is valid. A replacement that grows overall policy/state without a required accepted behavior is reworked, not declared a simplification. ED-23 is an explicitly accepted addition and its cost remains visible.

## Module ownership and uniquely assigned retirement pool

The IDs in this table cover all 194 decision rows once. A module plan includes retained contracts and delegated participation even when it has no independent retirement. Whole-file columns are **files / physical lines**, shared-span columns are lines; shared file counts are not additive. All figures are prospective gross baselines with final-consumer and conditional gates.

| Dominant owner / plan | Decision IDs owned | Whole-file pool | Shared spans |
| --- | --- | ---: | ---: |
| [Launcher](Launcher.md) | LA-01, LA-02, LA-03, LA-05, LA-06, LA-07, LA-08, LA-09, LA-10, LA-11, LA-12, LA-13, LA-14, LA-15, LA-16, LA-19, LA-20, LA-21, LA-22, LA-23, LA-24, LA-25, LA-26, LA-27, LA-28, AP-07, RV-01 | 8 / 639 | 239 |
| [Editor](Editor.md) | ED-01, ED-02, ED-03, ED-04, ED-05, ED-06, ED-07, ED-08, ED-09, ED-10, ED-11, ED-12, ED-13, ED-14, ED-15, ED-16, ED-17, ED-18, ED-19, ED-20, ED-21, ED-22, ED-23, ED-24 | 24 / 2,394 | 35 |
| [Application hosts](Application.md) | AP-01, AP-02, AP-04, AP-05, AP-06, AP-08, AP-09, AP-10, AP-11 | 0 / 0 | 0 |
| [GameFramework and scene runtime](GameFramework.md) | GF-01, GF-02, GF-03, GF-04, GF-05, GF-06, GF-07, GF-08, GF-09, GF-10, GF-11, GF-12, GF-15, GF-16, RV-02, RV-03 | 8 / 355 | 1 |
| [RHI](RHI.md) | RH-01, RH-02, RH-03, RH-04, RH-05, RH-06, RH-07, RH-08, RH-09, TP-05 | 26 / 2,332 | 0 |
| [Renderer](Renderer.md) | AP-03, RE-01, RE-02, RE-03, RE-04, RE-05, RE-06, RE-07, RE-08, RE-09, RE-10, RE-11, RE-12, RE-13, RE-14, RE-15 | 53 / 3,095 | 57 |
| [Shader assets and rendering data](ShaderAssets.md) | SH-01, SH-02, SH-03 | 0 / 0 | 0 |
| [Core](Core.md) | CO-01, CO-02, CO-03, CO-04, CO-05, CO-06 | 0 / 0 | 49 |
| [Platform](Platform.md) | PL-01, PL-02, PL-03, PL-04 | 0 / 0 | 0 |
| [SourceImporters](SourceImporters.md) | IM-01, IM-02, IM-03, IM-04, IM-05, IM-06, IM-07, IM-08, IM-09, RV-04, RV-05 | 4 / 269 | 0 |
| [AssetCooker](AssetCooker.md) | AC-01, AC-02, AC-03, AC-04, AC-05, AC-06, AC-07 | 1 / 24 | 35 |
| [SceneCooker](SceneCooker.md) | GF-13, GF-14, SC-01, SC-02, SC-03 | 0 / 0 | 0 |
| [MeshCooker](MeshCooker.md) | MC-01 | 0 / 0 | 0 |
| [MaterialCooker](MaterialCooker.md) | MA-01 | 0 / 0 | 0 |
| [TextureCooker](TextureCooker.md) | TX-01, TX-02, TX-04, TX-05, TX-06, TX-07, TX-08 | 2 / 49 | 0 |
| [ShaderCompiler](ShaderCompiler.md) | CP-01, CP-02, CP-03, CP-04, CP-05, CP-06, CP-07, CP-08, CP-09, CP-10 | 8 / 213 | 0 |
| [ToolSupport](ToolSupport.md) | TS-01, TS-02, TS-03, TS-04 | 0 / 0 | 0 |
| [Showcase content and conversion scripts](Showcase.md) | PR-01, PR-02, PR-03, PR-04, PR-05, PR-07, PR-08, PR-11, PR-12, PR-15, PR-16 | 1 / 21 | 30 |
| [Build, packaging and repository automation](BuildPackaging.md) | LA-04, LA-17, LA-18, BU-01, BU-02, BU-03, BU-04, BU-05, BU-06, BU-07, BU-08, BU-09, BU-10, BU-11, BU-12 | 3 / 118 | 0 |
| [Third-party integration maintenance](ThirdParty.md) | TP-01, TP-02, TP-03, TP-04 | 0 / 0 | 0 |
| [Documentation and ownership handover](Documentation.md) | DO-01, DO-02, DO-03 | 0 / 0 | 0 |

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

Use complete vertical changes as reviewable units. Where a vertical owner also closes a participating stage in another module, record both distinct exits on the same source identity and prune both passed stages; do not reimplement the same migration later. Do not implement a row in another plan twice. This is a dependency order, not a promise about week count; at 10–15 hours/week schedule one bounded stage at a time after its Stage 0 estimate.

| Wave | Work ready to take | Dependency / reason |
| --- | --- | --- |
| 0 | Shared baseline + relevant module Stage 0 | Freeze source/working tree, source content, screenshots, native diagnostics and thread/data ownership. Establish actual input validity and cost limits before candidates. |
| 1 | Renderer 1; Editor 1–2; Launcher 1; Showcase 1 | Largest complete optional systems first; mostly retire code without replacement architecture. Preserve existing build route until its replacement qualifies. |
| 2 | GameFramework 1; RHI 1–2; SourceImporters 1; TextureCooker 1 | Close morph/PTLAS/input chains and remove exclusive dependencies. Coordinate shared shader/cooked ABI regeneration. |
| 3 | BuildPackaging 1; Launcher 2 | Qualify the final Clang/Ninja/DLL route and native diagnostic preservation. Launcher 2 can be developed earlier but its final dump/source/symbol evidence must match the selected build identity. |
| 4 | ToolSupport 1; Core 1–2; AssetCooker 1; ShaderCompiler 1 | One Showcase/root/argument authority; preserve real callers, incremental identity and accepted outputs. |
| 5 | SceneCooker 1–2 with Showcase 2/AssetCooker 2 | Animation preservation then one prepared Knight scene; prerequisite to deleting runtime composition and some resource/loader machinery. |
| 6 | GameFramework 2–3; Editor 3; Launcher 3–4 | Biggest supporting-system rewrites and UI/status reduction, with safe parallel ownership and the newly narrowed products. Count cross-module replacements, including added persistence. |
| 7 | Renderer 2–4; ShaderCompiler 2–3; remaining module stages | Gated preprocessing/automatic batching, retained contracts and dependency/state cleanup. Can advance independent bounded stages earlier after their prerequisites and input/cost controls exist. |
| 8 | BuildPackaging 2–4; ThirdParty final stages; Documentation handover | Remove residual build/process extras, qualify complete runtime bundle and owned source, prune all passed plans. Re-run affected identity-bound gates after any build/schema/debug change. |

ToolSupport 2 follows recipe/consumer retirement. MeshCooker, MaterialCooker, ShaderAssets and Platform participate when their owning system stage changes their contracts, then close their own retained-boundary stage. No new large abstraction is justified merely to make wave ordering uniform.

## Stage protocol

**Stage 0, before the first edit in an owner:** read the named guides and current actual code/callers. Inspect `git status`, HEAD and relevant working-tree file hashes; preserve concurrent/unrelated work and do not cancel someone else's build. Refresh the inventory rather than trust old lines. List producer -> owner/worker -> representation -> consumer -> publication/destruction, source/SDK/cooked identities and build/dependency membership. Name what disappears and what remains, and sketch the smallest concrete target flow. For mixed files, classify functions/fields, not filenames.

Use migration outputs permitted by the existing artifact/layout contract, or an isolated checkout with its own canonical build root when changing generators. Do not invent an output layout or compete with another build.

Freeze a useful failing/working baseline and numeric comparison bounds before implementing a candidate. Existing known failures remain recorded: do not call an old corrupt render a successful baseline or quietly lower the retained behavior. Use Empty/CornellBox for bounded checks, Knight/Cesium for animation and Bistro for scale; add source-specific cases only for a real changed invariant. Check both native APIs independently. Baseline procedures use existing validation/capture/build owners; local probes are temporary and permanent test-only additions need explicit project approval.

**Each numbered stage:**

1. Implement the full owned change through all modules, registrations, CLI/settings/UI, cook formats, dependencies, build membership, runtime/package outputs and documentation. Keep one direct owner and bounded parallel work; remove the old path in the same delivered stage. No aliases, disabled scaffolding, copied stores or universal replacement framework.
2. Read the changed neighborhood with actual callers. Follow [CodeStyle](../../CodeStyle.md), public/private dependency rules, stable data layout and source-oriented names. Use retained libraries/native APIs where they own the mechanism. Moving a function or wrapping a subsystem does not earn retirement credit.
3. Run the smallest meaningful source/style/build checks, then behavior/failure/lifetime/native/image/performance checks required by the change. A shared-library change needs a real DLL consumer; process success is not scene readiness; source metadata is not cooked/playback equivalence; successful compilation is not GPU correctness.
4. Record identity-bound commands, inputs, environment, expected tolerance, observation and unrun limits in ignored `build/validation/portfolio-reduction/<owner>-<stage>/`. Retain failure evidence. Remove temporary test/probe code from the submitted change; preserve existing tests unless an approved capability retires their contract.
5. Record the net footprint/state/API/dependency result and owner explanation. An absent invariant, serial replacement, new global lock, duplicate authority or unexplained footprint growth prevents stage completion. A conditional stage that cannot safely reduce the mechanism stays open with its retained minimum; it is not a reason to drop Knight, breadcrumbs or the frame graph.
6. After exits really pass, remove that stage's heading/checklist/prompt from its plan in the same change, keep stable numbers and repair links. Partial work keeps only unmet actions/gates. Do not mark a plan delivered based on source inspection, a test stub, or budget/time pressure.

The module continuation prompt refers to this protocol. It authorizes necessary cross-module refactors, not scope expansion or weakening checks. No user approval ceremony is introduced by this documentation workflow.

## Conditional cuts and recording gaps

IM-08/IM-09/SC-03 and SceneCooker/Ozz/flattening changes are conditional on native Knight/Cesium preparation and correct threaded runtime animation. Preserve required support if equivalence fails. CP-04 requires a smaller correct native include/debug/invalidation path. RE-07 needs equivalent-workload performance/memory bounds. Native GPU diagnostics require actual usable data after the final build/shader identity. These remain live gates even though decision selection is finished.

Five cells were still `-` in the frozen worksheet: **BU-05, BU-06, DO-01, DO-02, DO-03**. No new deletion or release prerequisite is inferred from them. Preserve current prepared-product import until BU-05 is explicitly decided; preserve local style tools as required by your earlier instructions/Engineering while BU-06's worksheet cell is blank. Do not make three case studies or future neural features a new prerequisite. Existing Engineering rules already require pruning completed implementation stages; DO-03's final worksheet-retention choice is not needed to begin approved removals. These recording gaps do not block independent authorized work.

Already completed KTX/earlier scene removals, task-runtime replacement and SDL3 replacement are not reopened as implementation stages. Their current product qualification limitations remain in Roadmap. Candles, PLY and the new source/build/runtime cuts still exist in source and are planned, not delivered.

## Final reduced-engine gate

The selected ten maps and both Editor/Game products run from Launcher using prepared data on D3D12/Vulkan; Knight/Cesium animate; approved controls/edits, undo, maintainer save and reviewer reset work. Both API resource/queue/thread contracts and native captures/breadcrumbs/crash exports remain usable. The source-workspace Launcher builds/cooks/syncs/cleans/debugs with the selected toolchain and actual artifact readiness; one-file/no-op iteration and parallel overlap are measured. The extracted runtime bundle works offline without an IDE/compiler/cooker; rights and existing rendering/Vulkan/material-parity limits are honestly resolved or stated before external presentation.

The owner can read the remaining first-party implementation, draw its data/lifetime/thread flow, explain key math/units/assumptions and make a small manual change through a real debugger/build. The architecture/source guide points directly to the actual owners. Publish the measured final footprint and largest remaining obligations alongside results, not a subjective masterpiece label. Principal-review readiness is demonstrated through these inspectable facts, attribution and rejected alternatives, not asserted from a line count.

## Local evidence for this planning pass

`build/validation/portfolio-module-plans-20261125/` stores scope/source hashes, unique retirement allocation, full decision-to-stage ownership and review results. All 194 worksheet IDs and all 66 approved removal IDs are mapped exactly once. Older file-level selections were rechecked against current source; retired level records are excluded and Candles catalog spans were remeasured. These are source-planning receipts only; no implementation/build/cook/GPU/package acceptance was run by generating these plans.
