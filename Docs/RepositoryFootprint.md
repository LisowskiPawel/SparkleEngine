# Repository footprint and cut candidates

[Documentation](README.md) | [Goals](Goals.md) | [Roadmap](Roadmap.md) | [Third-party migration](ThirdPartyMigration.md)

SparkleEngine should be a compact rendering and evidence platform for the [target persona](Goals.md#target-persona): a graphics systems engineer who can explain, implement, measure and transfer a useful result. Its supporting systems earn their cost by enabling the [three cases](Goals.md#the-three-cases): content to correct pixel, one measured frame on two APIs, and an owned model to shader.

**The main reduction opportunity is supporting breadth and the coordination around it.** Launcher, Editor, cooking, compiler integration, dependency preparation and delivery create substantial work around the renderer. Some of that work is required by the selected product; the cut targets are repeated policy, custom mechanisms already available upstream, and machinery without a current consumer.

This document names places to investigate and the implementation that could disappear. [Goals](Goals.md) owns scope, [Rendering](Rendering.md) owns graphics requirements, [Roadmap](Roadmap.md) owns order, and [Third-party migration](ThirdPartyMigration.md) owns dependency decisions and retirement gates. These candidates are proposals, not authorization to remove supported workflows or new release prerequisites.

## Where complexity concentrates

The [historical footprint baseline](#footprint-baseline) provides scale, rather than a live count of the current checkout:

| Responsibility | Owned code lines | Share | What deserves scrutiny |
| --- | ---: | ---: | --- |
| Graphics systems and shaders: RHI, Renderer, shaders | 70,276 | 46.8% | Repeated pass policy, conversions and planning work. Keep native API, synchronization and resource-lifetime reasoning. |
| Hosts and interaction: Launcher, Editor, Application, entry points | 33,084 | 22.0% | Bespoke presentation, writable state mirrors and coordination across UI, operations and runtime owners. |
| Content and world: GameFramework, importers, cookers | 20,952 | 13.9% | Repeated semantic translation and adapters around existing codecs. Keep one admitted content/material meaning. |
| Shared foundations: Core, former Tasks, Platform, ToolSupport, native build glue | 11,928 | 7.9% | Remaining support mechanisms and caller glue after delegation. The Tasks subtotal here predates its retirement. |
| Shader tooling and automation: ShaderCompiler, build, scripts, CI | 13,971 | 9.3% | Custom compiler behavior and duplicated build, dependency or deployment facts. Count automation growth alongside native cuts. |

The supporting surface is spread across workflows. A setting can cross Renderer, Application persistence and Editor presentation; a shader change crosses registration, compilation, publication and runtime replacement. These boundaries become bloat when several places decide the same fact, or when a small change requires editing mechanical copies. Different ownership, failure or lifetime requirements can justify separate representations.

The baseline comparison reduced native code by 967 lines but added 1,061 automation lines and five shader lines: **the complete owned footprint grew by 99 code lines**. Native deletion alone did not establish repository simplification. Later delivered cuts are outside that count and must not be silently added to it.

The pinned peer study found a larger supporting share here than in Falcor, RTXPT with Donut/NVRHI, Cauldron and VEngine2. Their scope differs, including cooking and package reproduction. This supports reviewing host/tooling cost; it supplies no ideal LOC, file-count or shader-percentage target. See the [peer comparison](Images/RepositoryFootprint/peer-proportions.svg) and [source data](Images/RepositoryFootprint/peer-proportions.csv).

Assets dominate tracked checkout storage, while fetched dependencies, builds and generated products add a separate local footprint. Content selection, redistribution and prepared-output reuse belong to the existing product and delivery contracts. Source volume, disk use and build/runtime cost need separate accounting.

## Cut candidates

The strongest bounded removal candidates come first. Later entries require a demonstrated duplicate, a selected replacement or profiling before implementation. Large module totals are exposure, not estimated deletions.

### Launcher presentation machinery

**Cut candidate: `ResponsiveCardGridWidget` and unnecessary presentation-owned state.** The [custom layout widgets](../Tools/Launcher/SparkleLauncher/Private/Gui/Widgets/LauncherLayoutWidgets.cpp) maintain card sizing, column selection and reflow. Trial the existing Qt item-view facilities against that complete responsibility through the [Launcher item-view plan](Implementation/LauncherItemViews.md). Remove the custom reflow implementation only if the model, delegate and caller changes are smaller and clearer.

Keep scene selection, readiness, keyboard actions, useful progress/errors, cancellation and supported DPI behavior. Follow one operation through request construction and presentation before removing a state mirror. [Shared plan/run dispatch](../Tools/Launcher/SparkleLauncher/Private/Operations/LauncherOperationExecution.cpp) and the [Activity panel](../Tools/Launcher/SparkleLauncher/Private/Gui/Shell/LauncherActivityPanel.cpp) have already been simplified; they are not future savings. Launcher remains a selected product workflow.

### Shader include expansion

**Cut candidate: the custom recursive include expander in [ShaderSourcePreprocessor](../Tools/Shaders/ShaderCompiler/Private/Compiler/ShaderSourcePreprocessor.cpp).** It scans include directives with regular expressions before compiler conditional evaluation. DXC and Slang already own preprocessing semantics. Investigate using their include/preprocess facilities while retaining only Sparkle's necessary source mapping and generated parameter declarations.

The existing two-file review scope is 221 code lines before replacement costs. Preserve exact dependency/debug source identity, virtual roots, typed reflection/ABI, accepted-output publication and reload. ShaderMake already owns batch execution; its adoption did not retire this expander. A new parser or compiler-adapter framework of comparable complexity would fail the purpose of this cut.

### Glue around delivered dependencies

**Cut candidate: redundant adapter state and translations left around adopted mechanisms.** Native enkiTS, SDL3, Boost.Process, ozz, JSON and ShaderMake already have selected owners. Review actual callers for overlapping stop/completion flags, repeated callback/result conversion or a wrapper that merely recreates the former local API. The [delivery and cost ledger](ThirdPartyMigration.md#delivered-integrations-versus-savings) identifies which transfers grew native code and where further review is justified.

Remove only demonstrated overlap. Domain admission, partitioning, process-tree policy, animation mapping, publication and physical settlement still need owners. Prefer a focused change at an existing consumer over another general integration layer. Count all adapters, callers, maintained bindings and patches; upstream adoption alone earns no simplification claim.

### Dependency and build policy

**Cut candidate: a second handwritten authority for dependency identity, required files, runtime payloads or artifact paths.** Trace a dependency from its [recipe](../ThirdParty/README.md) through Launcher readiness, [runtime deployment](../CMake/Artifacts/RuntimeDeployment.cmake) and [package requirements](../CMake/Packaging/Payload/PackageDependencies.ps1). Where two consumers repeat the same fact, make one consume the existing owner's declaration and retire the other copy.

Source/target separation and recipe-generated Launcher facts are already implemented. This is a review of remaining duplication, not a claim that those earlier problems remain. Keep source admission, offline use, legitimate configuration differences, runtime repair and package identity. Continue the [remaining integration qualification](Implementation/ThirdPartyIntegration.md); avoid introducing a package manager or cross-tree binary cache without a separate demonstrated need.

### Repeated material interpretation

**Cut candidate: divergent material conversion and sampling policy across import, cook, runtime packing and shaders.** [MaterialData](../Engine/Renderer/Private/Scene/Materials/MaterialData.cpp) carries texture mappings into ray data, while [raster sampling](../Engine/Assets/Shaders/Passes/GBuffer/Raster/RasterMaterial.hlsli) uses incoming UVs and a fixed wrap sampler. [Ray sampling](../Engine/Assets/Shaders/RayTracing/RayTracingMaterialSampling.hlsli) applies mappings and authored occlusion strength. This is a concrete source-path difference requiring a checked material contract.

Make the admitted meaning consistent, share small semantic helpers where useful, and remove superseded interpretation. Preserve path-specific filtering, derivatives and independent Reference transport. This is primarily correctness work and may add necessary shader code; no net deletion or cause of the current Sponza failure is established. [Material acceptance](Rendering.md#frame-and-materials) remains with Rendering.

### Overlapping production rendering paths

**Cut candidate: an old production estimator or repeated pass policy after its selected replacement qualifies.** The [planned RTXDI investigation](Roadmap.md#after-release) should replace one production sampling owner rather than maintain two complete production implementations indefinitely. Application bindings, resources and dispatch remain necessary, so replacing sampling math supplies no automatic native-code saving.

Keep the independent Reference and explicitly bounded research comparisons. Preserve [per-shader declarations, registrations and recognizable pass construction](CodeStyle.md#runtime-cvars-and-shader-rules); small files are not a removal target. Trace a real resource/history change to find duplicated policy or forwarding with no independent purpose. SDK additions remain subject to the existing roadmap and complete integration costs.

### Repeated frame planning

**Conditional cut candidate: stable planning work repeated for an unchanged graph.** [Frame execution](../Engine/Renderer/Private/Frame/Graph/ExecuteRenderFrameGraph.cpp) performs setup, compile, prepare and execute each executable frame. Investigate which work is stable, what it costs, and whether a simpler execution path can stop doing it.

The earlier allocation/vector cleanup is delivered. A compilation cache is not selected by this review: imported state, histories, provider changes, resize and reload all affect validity. Reuse a plan only if measurements justify its invalidation and lifetime cost, and retire superseded recomputation. This is an unproved performance opportunity, with no promised source or whole-frame saving.

### Documentation and workflow scaffolding

**Cut candidate: repeated decisions, completed implementation diaries and automation without a recurring consumer.** Keep this document at the level of repository shape and cut choices. Keep migration detail with its owner, unfinished acceptance in the existing implementation plan, and individual measurements/source receipts in ignored evidence output.

Review tracked scripts by the build, cook, package or graphics question they answer. Retire obsolete wrappers and one-off machinery when their consumer disappears; retain useful verification, custom style rules and reproduction commands. An image comparison, ABI check or failure control serving a selected case earns its cost even if it removes no source lines. Do not create a second report or catalogue to hold this document's removed detail.

## What the reference projects teach

These primary sources were followed from [References](References.md) and checked for this rewrite. Upstream behavior is the observation; its proposed application to Sparkle is an inference. The examples are design precedents, not whole-repository endorsements or dependencies selected for adoption.

| Primary source | Observed practice | Application to these cuts |
| --- | --- | --- |
| [Qt 6.11 QListView](https://doc.qt.io/qt-6.11/qlistview.html) | Model-backed item layout supports wrapping, grids and resize-driven layout. | Trial existing layout machinery before maintaining custom reflow. Charge the full delegate/model bridge. |
| [DXC include handling](https://github.com/microsoft/DirectXShaderCompiler/wiki/Using-dxc.exe-and-dxcompiler.dll), [Slang compilation API](https://docs.shader-slang.org/en/latest/external/slang/docs/user-guide/08-compiling.html) | Compiler interfaces accept include handling, search paths, macros and filesystem input. | Delegate language preprocessing; preserve Sparkle's exact source/debug and ABI contracts. |
| [Donut dependency composition](https://github.com/NVIDIA-RTX/Donut/blob/e50042f4bbd762971cc90431261e967404855b81/thirdparty/CMakeLists.txt), [RTXPT build composition](https://github.com/NVIDIA-RTX/RTXPT/blob/303b408ab81790a5051abd7573830b7a4112eb8e/CMakeLists.txt) | External targets and application composition are identifiable in the build graph. | Keep recipes and native consumers distinct; remove repeated facts rather than invent another orchestration system. Their acquisition/global-option choices are not blanket recommendations. |
| [Falcor MinimalPathTracer](https://github.com/NVIDIAGameWorks/Falcor/blob/eb540f6748774680ce0039aaf3ac9279266ec521/Source/RenderPasses/MinimalPathTracer/MinimalPathTracer.cpp) | Pass input/output reflection and execution are visible together. | Keep a feature's resource contract and execution easy to follow. Do not import its estimator/domain shortcuts into Reference. |
| [Falcor graph compilation](https://github.com/NVIDIAGameWorks/Falcor/blob/759aad033ff610fb0d82c74f7e0a508d0096d5f2/Source/Falcor/RenderGraph/RenderGraph.cpp) | Compilation returns early when its recompile flag is clear. | Investigate repeated planning, with Sparkle's dynamic inputs and measured cost deciding whether reuse is worthwhile. |
| [RTXDI integration](https://github.com/NVIDIA-RTX/RTXDI/blob/a6efab966b7c3b272da0461578eb56ac61c7cbff/Doc/Integration.md), [NVRHI resource guide](https://github.com/NVIDIA-RTX/NVRHI/blob/6b96fb03e07539f08327aea76c56d55f1de9d906/doc/ProgrammingGuide.md) | RTXDI leaves resource allocation, application bridges and pipeline assembly to the application. NVRHI bindless tables leave resource lifetime/state and write synchronization to the application. | Retire replaced algorithms while preserving explicit graphics ownership. An SDK does not eliminate the engine's lifetime work. |
| [RTXPT image tests](https://github.com/NVIDIA-RTX/RTXPT/blob/303b408ab81790a5051abd7573830b7a4112eb8e/Support/tests/run_tests.ps1) | Scene, sample count and image-error threshold drive an executable comparison. | Prefer verification with a concrete rendering consumer. Regression images complement independent correctness controls. |

## Ownership to retain

The [product scope](Goals.md#product-scope) remains the boundary: Windows, separate D3D12/Vulkan validation, Launcher build/cook/run, focused Editor inspection/edits and Game rendering. Broad gameplay, authoring, extra platforms and general ML infrastructure do not help the selected cases.

| Keep owned | Why it remains necessary |
| --- | --- |
| RHI, FrameGraph, graphics resources and synchronization | These are central to the target persona. Preserve recording and submitted GPU lifetime, cross-queue waits, histories and safe replacement/retirement. |
| Material/scene meaning and independent Reference | Correct pixels require a traceable content contract and an independent check. Reusing codecs or production algorithms does not replace those responsibilities. |
| Domain cancellation, publication and recovery | Logical completion differs from settled readers and workers. [Scene-load admission](../Engine/GameFramework/Private/Level/Loading/SceneLoadExecutor.cpp) is separate from the [native run and its settlement](../Engine/GameFramework/Private/Level/Loading/SceneLoadRun.cpp); preserve that ownership. Failure rollback does not itself prove interruption-safe publication. |
| Distinct defaults, persisted intent, device selection and presentation state | These represent different decisions. Existing settings capture/apply mappings are already shared; another registry or service needs a real missing consumer. |
| Useful diagnostics, raw output and identified offline products | Debugging, reproduction and transfer are explicit goals. Keep shader ABI/source identity, accepted outputs, package membership and failure evidence. Source/build checks do not establish product acceptance. |

No blanket replacement of Core, GameFramework, RHI or the host products follows from this review. Small codec/cooker entry points and the remaining tiny library proposals need a complete maintenance win before investment. The migration guide's pending native review ceiling is about 1,000 code lines before integration; it cannot explain or eliminate the much larger supporting surface by itself.

## Choosing the next cut

Continue the [current Sponza and Reference repairs](Roadmap.md#current-priority). For discretionary simplification, start with one bounded compiler or Launcher responsibility, then review the complete bridge before another SDK integration. Material consistency belongs to the existing correctness work; frame-plan reuse waits for profiling.

A useful cut should answer four questions:

1. **What disappears?** Name the mechanism, duplicate authority, writable state or recurring work and its current consumer.
2. **Who owns the result?** Trace producer to final consumer, including cancellation, failure, publication and destruction.
3. **Is the complete result simpler?** Charge callers, adapters, shaders, scripts, generated declarations and maintained patches. Moving source or changing language earns no removal credit.
4. **What demonstrates preservation?** Use the real workflow and the checks required by its owner. Keep source/build, interactive, GPU, package and release claims distinct.

Prefer a smaller completed responsibility to a new framework. Numerical release requirements and migration budgets stay in their existing owners; this review adds no LOC quota, effort allocation or speculative saving.

## Footprint baseline

The figures above retain the frozen **22 November 2026, 19:05 Warsaw / 18:05 UTC** inventory: revision `d5bf01d53791407de8bf691a8bf54cb404c69226` plus its recorded local changes. It contains **150,211 owned code lines in 2,429 files**, including **136,684 native code lines**. It predates task-runtime publication and later source cuts; it is not a recount of today's HEAD. The [snapshot, hashes and detailed inventory](../artifacts/analysis/footprint-refresh-20261122) retain provenance.

The [21 November baseline](../artifacts/analysis/repository-footprint-2026-11-21) contains 150,112 owned code lines. Both use cloc 2.06 with the same source/ownership rules: code excludes blanks and comment-only lines; first-party bindings remain counted wherever placed; upstream/generated implementations and assets are separate. The [peer study](../artifacts/analysis/peers-20261121) pins its own revisions, classifications and exclusions. These counts measure maintained source, not defects, development time, performance or capability parity.

This rewrite checked selected current source paths and the cited upstream practices. It does not re-audit every subsystem, recount the repository or requalify prior results. Completed dispatch, settings, lifecycle and planning cleanups are no longer future cut estimates. Detailed delivery receipts and remaining acceptance stay with [Third-party migration](ThirdPartyMigration.md), [active plans](Implementation) and [Roadmap](Roadmap.md). No engine code was changed, built or run for this documentation update.
