# Third-party migration

[Documentation](README.md) | [Roadmap](Roadmap.md#supporting-system-simplification) | [Engineering](Engineering.md#module-boundaries)

Move generic implementation and maintenance into established third-party projects so Sparkle can concentrate on rendering. Preserve existing Launcher, Editor, Game, build, cook, inspection, editing, cancellation and package workflows. When no replacement meets those requirements, retain the working implementation.

This guide owns migration research, candidate decisions and the evidence needed to adopt a dependency. [Roadmap](Roadmap.md) owns execution order; [Engineering](Engineering.md) owns module, lifetime, compatibility and verification rules. No migration described here has been implemented or qualified. Research was performed on 2026-11-08 against `dcd5b4d027543aeb11a133ac40da64cc74ce5a77` and the inspected working tree; unrelated concurrent changes are outside this assessment.

## Conditions for adoption

1. **Preserve capability.** Keep supported inputs, operations, outputs, failure behavior, recovery and automation. Preserve both graphics APIs, offline packaged use and the release limits. A dependency cannot justify dropping a workflow, a filter, a format, a check or an editing operation.
2. **Transfer implementation ownership.** The library must own the substantial generic algorithms, storage or execution machinery within the selected boundary. Sparkle should declare work, map domain values, enforce its contracts and consume results. Recreating the old subsystem around a library fails this condition.
3. **Require a credible upstream.** Check actual adoption, maintenance, released versions, Windows/toolchain support, tests, license files and distribution obligations. Prefer established broadly adopted projects or specialist libraries maintained by an established vendor. Stars and vendor names are evidence inputs, not a quality certificate.
4. **Finish the replacement.** Remove the superseded implementation as the migration lands. Do not ship a second scheduler, parser, codec implementation, native input backend, compatibility reader or permanent old/new switch to cover missing functionality. Temporary comparison controls stay local under the existing verification rules.
5. **Keep integration small.** A field decoder, a format-policy decision or a title-bar region callback is legitimate application code. A private dependency engine, interpolation engine, binary parser or event pump retained to compensate for the library is substantial ownership that has not transferred. Assess mechanisms and failure states as well as line counts.
6. **Reject an incomplete fit.** If capability preservation requires a fork, a second substantial local implementation or unacceptable build/runtime/package costs, retain the current implementation and record the specific gap. Do not add the dependency first and leave the difficult migration indefinitely open.

Several libraries can compose cleanly when their responsibilities do not overlap. A container reader and an upstream encoder can each fully own their boundary. Adding an image library while retaining a substantial custom image-processing engine does not establish a whole-pipeline replacement.

Owned representations may change under the [compatibility policy](Engineering.md#compatibility), with every producer, consumer, recipe and normal user workflow updated together. This does not permit silently removing accepted input or source/settings behavior. If preserving an existing text format is required and the candidate cannot read it, keep that format's current owner; do not preserve a second complex parser behind the new library.

## Decisions and opportunity size

These are assessment surfaces, including headers and comments, not measured savings. They overlap and must not be added together. A full module count includes domain code that a library will not replace.

| Area | Assessed surface | Decision | Dependency must take over |
| --- | --- | --- | --- |
| Task runtime | About 3,820 lines, 43 files | Largest execution candidate; investigate oneTBB first. Capability and ownership fit remain unproved. | Scheduling, dependency execution, worker management and parallel partitioning, with all actual lifecycle consumers preserved. |
| Cooked metadata | About 2,900 lines across records, loaders, writers and codecs | Large serialization candidate; trial FlatBuffers on one complete record family. | Wire layout, encoding, structural verification and field access. |
| Text formats | 578 lines of JSON infrastructure; 1,172 lines of level parsing, plus other readers | Recommend nlohmann/json for existing JSON. Broader consolidation requires a complete format/workflow migration. | JSON syntax and serialization; eventually the selected standard document grammar. |
| Texture cooker | About 3,485 lines; roughly 1,000 around DDS, compression and mip operations | No verified whole-pipeline replacement yet. DirectXTex has a credible bounded DDS replacement. | Every claimed image/container/codec mechanism, including required filter behavior. |
| Native window/input | 2,154-line Platform module; roughly 828 lines of window and native input-backend code | SDL3 is the preferred capability investigation; not a promise to delete the Platform module. | Native window/event/cursor/input mechanics and the platform ImGui backend. |
| Command-line parsing | About 780 lines in three principal parsers | Recommend CLI11; smaller but clear ownership transfer. | Argument syntax, option handling, generated help and common validation. |
| Build configuration/state | About 4,172 lines in Launcher Build | Secondary opportunity using existing CMake facilities. No full Launcher replacement identified. | Configuration declaration, regeneration and generated build information. |

Start implementation with the well-bounded JSON and CLI changes when selected by the roadmap. Investigate task execution and metadata serialization for larger gains. Full texture and platform migrations remain conditional on the specific gaps below. A conditional candidate is not permission to add a dependency while retaining its replacement implementation.

## Upstream reliability evidence

The 2026-11-08 GitHub API snapshot checked repository activity, archive status, licenses and releases. Approximate stars show relative adoption, not an adoption threshold. All projects in this table were non-archived. Versions are observed releases, not selected Sparkle pins; dependency commits, supported toolchains and notices must be frozen for the actual migration.

| Project | Stars | Observed release | License | Upstream evidence and role |
| --- | --- | --- | --- | --- |
| [nlohmann/json](https://github.com/nlohmann/json) | 50,800 | v3.12.0 | MIT | Windows tests and [conformance/fuzz testing](https://json.nlohmann.me/community/quality_assurance/); preferred JSON library. |
| [CLI11](https://github.com/CLIUtils/CLI11) | 4,500 | v2.7.2 | BSD-3-Clause | Platform, coverage, installation and fuzz workflows; [license inspected directly](https://github.com/CLIUtils/CLI11/blob/main/LICENSE). |
| [oneTBB](https://github.com/uxlfoundation/oneTBB) | 6,800 | v2023.2.0 | Apache-2.0 | Established parallel runtime; used by [Intel Open Image Denoise](https://www.openimagedenoise.org/downloads.html). Preferred large-runtime investigation. |
| [Taskflow](https://github.com/taskflow/taskflow) | 12,200 | v4.1.0 | MIT | Windows/Linux/macOS CI; task-graph alternative with a cancellation mismatch to resolve. |
| [DirectXTex](https://github.com/microsoft/DirectXTex) | 2,200 | may2026 | MIT | Microsoft texture library with Windows CTest and multiple compiler/build workflows. Specialist vendor ownership is stronger evidence here than raw star count. |
| [SDL3](https://github.com/libsdl-org/SDL) | 16,900 | release-3.4.20 | zlib | Active releases and platform build/test coverage; native APIs address existing borderless-window needs. |
| [FlatBuffers](https://github.com/google/flatbuffers) | 26,600 | Selection pending | Apache-2.0 | Established schema compiler/runtime with C++ tests; inspected main CI was mixed, as detailed below. |
| [EnTT](https://github.com/skypjack/entt) | 13,300 | v4.0.0 | MIT | Windows, coverage and sanitizer workflows; credible storage library, smaller immediate opportunity. |
| [Ozz](https://github.com/guillaumeblanc/ozz-animation) | 3,100 | 0.17.0 | MIT | Windows tests include reference-SIMD and shared-library configurations; animation compatibility remains unproved. |

Successful upstream runs were observed for [JSON on Windows](https://github.com/nlohmann/json/actions/runs/42377075043), [CLI11 platform tests](https://github.com/CLIUtils/CLI11/actions/runs/39874528209), [oneTBB](https://github.com/uxlfoundation/oneTBB/actions/runs/42006582309), [Taskflow on Windows](https://github.com/taskflow/taskflow/actions/runs/38673209843), [DirectXTex Windows CTest](https://github.com/microsoft/DirectXTex/actions/runs/41701945404), [SDL](https://github.com/libsdl-org/SDL/actions/runs/42408499416), [EnTT](https://github.com/skypjack/entt/actions/runs/37895872658) and [Ozz on Windows](https://github.com/guillaumeblanc/ozz-animation/actions/runs/30719436686). Each result belongs to its linked upstream commit and environment. The research did not run these libraries' tests locally or qualify them inside Sparkle.

## Large ownership transfers to investigate

### Task runtime: oneTBB, with a strict capability gate

Current owners include [worker scheduling](../Engine/Tasks/Private/Scheduling/TaskWorkerScheduling.cpp), [graph execution](../Engine/Tasks/Private/Scheduling/ScheduledTaskExecution.cpp), [scope lifetime](../Engine/Tasks/Private/Lifetime/TaskScope.cpp) and [prepared dispatch](../Engine/Tasks/Private/Scheduling/TaskDispatch.cpp). A successful replacement deletes substantial implementation in these areas. Keeping the private graph compiler, dependency counters and worker queues while forwarding bodies into a library is rejected.

oneTBB offers native parallel algorithms, task groups, flow graphs and execution arenas. Its [arena contract](https://uxlfoundation.github.io/oneTBB/main/reference/source/task_scheduler/task_arena/task_arena_cls.html), [cancellation guidance](https://uxlfoundation.github.io/oneTBB/main/tbb_userguide/Flow-Graph-exception-tips.html) and [graph lifetime guidance](https://uxlfoundation.github.io/oneTBB/main/tbb_userguide/Flow-Graph-waiting-tips.html) make it a credible investigation, not a drop-in match.

| Existing requirement | Actual consumer or contract | What the trial must establish |
| --- | --- | --- |
| Dependency ordering and failure propagation | Renderer scene preparation, GameWorld systems, scene loading and cook batches | Native library graph/group primitives own execution and dependency release; no second private graph engine. |
| Cleanup after cancellation or failure | [Launcher operation completion](../Tools/Launcher/SparkleLauncher/Private/Gui/Operations/LauncherOperationService.cpp) uses `TaskCompletionPolicy::Cleanup` | Completion callbacks, result publication and UI state always settle. Native graph cancellation alone is insufficient evidence. |
| Frame/background/blocking-I/O separation | Scene asset reads, texture decode, Launcher operations and frame preparation | Blocking work cannot occupy all capacity needed by frame work. Arena priority is not proof of dedicated I/O isolation. |
| Deterministic caller-thread reference execution | [TaskExecutorConfig](../Engine/Tasks/Public/TaskExecutor.h) supports all lane counts at zero | Preserve the reference execution behavior. A one-worker arena does not by itself prove caller-thread or deterministic execution. |
| Parent/child scope cancellation and timed settlement | Scene replacement, Editor operations, Launcher close and cache destruction | Inputs survive until owned work settles; timed polling and shutdown behavior remain usable without recreating a complex scope runtime. |
| Prepared, coalescing dispatch | [GPU-crash incident sink](../Engine/Application/Private/Diagnostics/GpuCrash/GpuCrashIncidentSink.cpp) | Preserve notify/coalesce/close behavior and callback constraints without retaining a second dispatch queue/state machine of comparable complexity. |

Keep only domain work declarations, small result/cancellation bindings and lifetime orchestration. Record which stable APIs cover every row and the removed mechanisms before proceeding. Include the selected library's linking/deployment requirements in Game, Editor, tools and package checks; do not assume a static-library configuration or a new runtime DLL is free.

[Taskflow cancellation](https://taskflow.github.io/taskflow/RequestCancellation.html) stops scheduling successors while running tasks finish. Therefore its `cancel()` cannot simply replace Sparkle cancellation where a successor performs cleanup. It remains an alternative only if the complete design is smaller and preserves every row. [enkiTS](https://github.com/dougbinks/enkiTS) is a maintained specialist scheduler, but its approximately 2,050-star adoption and useful scheduling APIs do not establish full graph/scope/dispatch coverage. Choose one implementation after a qualifying trial; otherwise retain SparkleTasks without adding another runtime.

### Cooked metadata: FlatBuffers

Current [manifest loading](../Engine/GameFramework/Private/Assets/Loaders/SceneManifestLoader.cpp), [record validation](../Engine/GameFramework/Private/Assets/Loaders/SceneManifestValidator.cpp) and [cooked records](../Engine/GameFramework/Public/Assets/Cooked) coordinate manual layouts, counts, strides and read/write order.

The candidate replaces one complete record family: schema, writer, reader and structural verification together. Generated accessors and verification become the single wire-format authority. Sparkle maps imported/domain values, checks semantic references and consumes the result. Keep bulk GPU payloads in an appropriate packed representation; a metadata trial does not justify rewriting them.

The [C++ verifier](https://flatbuffers.dev/languages/cpp/#access-of-untrusted-buffers) checks structural access bounds. Sparkle still owns finite values, valid asset relationships, skin/morph ranges, material meaning, allocation budgets and accepted-output publication. Those are application contracts, not an alternative serialization engine.

Start with a complete scene-manifest family and its actual cook/load path. Prove malformed/truncated input rejection, resource limits, interruption handling, scene equivalence, startup/memory cost and generator reproducibility. Remove handwritten wire records/codecs for that family. Keeping old headers and decoders as parallel authorities, or adding a generic wrapper that duplicates the schema, fails the ownership gate.

The one-family exercise is local feasibility work. Define a production scope that retires the substantial metadata codec machinery before landing the dependency. If the result is one converted manifest alongside a still-complex general local serialization system, keep the trial local and retain the current production implementation.

The [inspected main CI run](https://github.com/google/flatbuffers/actions/runs/42448259809) passed Windows C++ builds but failed Swift Linux and Windows generated-code checks. This is a specific release/generator selection concern, not evidence that every FlatBuffers version is broken. Pin a released compiler/runtime pair and verify generated C++ on the supported Windows toolchain before adopting it. No selected version or Sparkle trial currently closes this gate.

### Window and input: SDL3

The selected boundary is native mechanics in [Window](../Engine/Platform/Private/Window) and [input backends](../Engine/Platform/Private/Input/Backends), not all Platform code. SDL3 can own window/event processing, native keyboard/mouse translation, cursor behavior and the ImGui platform backend. Sparkle retains camera actions, Editor/viewport routing and a small mapping to its existing events.

There is concrete support for existing functionality: [hit-test callbacks](https://wiki.libsdl.org/SDL3/SDL_SetWindowHitTest) cover custom draggable/resizable regions, [window properties](https://wiki.libsdl.org/SDL3/SDL_GetWindowProperties) expose the Win32 handle, and [relative mouse mode](https://wiki.libsdl.org/SDL3/SDL_SetWindowRelativeMouseMode) supplies continuous captured motion. Dear ImGui provides an [SDL3 platform backend](https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_sdl3.h) with D3D and Vulkan initialization entry points.

The trial must preserve the current title bar, borderless/windowed behavior, resize/minimize/restore, startup/close behavior, 100/150/200% DPI, focus/capture loss, cursor restoration, keyboard shortcuts, deferred dispatch and viewport navigation. Qualify both RHI surface integrations. SDL must not silently become the swapchain/rendering owner or introduce unselected platform features into Sparkle's API.

Delete the replaced Win32 event pump, input backends and ImGui platform backend in the final change. A region callback and native-handle handoff are small orchestration; keeping a substantial custom Win32 implementation as a fallback fails the gate. [GLFW](https://github.com/glfw/glfw), about 15,500 stars, is an established alternative, but use it only if the same window/interaction requirements are met without additional native machinery. Do not adopt both.

### Texture processing: no verified complete replacement yet

[TexturePipeline](../Tools/Cooking/TextureCooker/Private/Pipeline/TexturePipeline.cpp) composes loading, channel/shape policies, mip processing and encoding. The current AMD integration supplies block codecs, while Sparkle still owns [block preparation](../Tools/Cooking/TextureCooker/Private/Pipeline/Compression/BCCompressor.cpp), [mip generation](../Tools/Cooking/TextureCooker/Private/Pipeline/Stages/MipStage.cpp) and [image operations](../Tools/Cooking/TextureCooker/Private/Pipeline/ImageOps.cpp).

| Current capability | Preservation requirement |
| --- | --- |
| Regular, Kaiser and NormalAware mip filters | All remain supported. The current Kaiser kernel and decoded-normal averaging/renormalization need a defined numerical/quality comparison; a different generic filter cannot silently replace them. |
| Generate, PreserveExisting and NoMips | Preserve selected policy, existing mip content and relevant compressed-source behavior. |
| sRGB, linear data, HDR, channel extraction and alpha | Preserve transfer functions, channel meaning, range and material behavior. |
| BC1/BC4/BC5/BC6H and supported codec paths | Preserve format selection, edge handling, HDR sign/range and quality. A provider change needs measured output and cost checks. |
| DDS/container shape, arrays/cubemaps and other supported source loaders | Match the existing supported cases and refusals; a DDS replacement must not remove KTX, raster or EXR workflows. |
| Batch cancellation, memory limits and publication | Keep bounded work, progress/failure reporting and preservation of accepted output. |

DirectXTex supplies [DDS/image processing](https://github.com/microsoft/DirectXTex), [mip generation](https://github.com/microsoft/DirectXTex/wiki/GenerateMipMaps), [filter choices](https://github.com/microsoft/DirectXTex/wiki/Filter-Flags) and [whole-image BC compression](https://github.com/microsoft/DirectXTex/wiki/Compress). These APIs do not establish equivalence for Sparkle's complete Kaiser/NormalAware path. A bounded replacement of the 369-line DDS reader is credible; claiming the whole 3,485-line cooker is replaced is not.

For a larger transfer, investigate the existing [AMD texture-level APIs](https://github.com/GPUOpen-Tools/compressonator/blob/master/docs/source/developer_sdk/cmp_framework/index.rst) or [NVIDIA NVTT 3](https://developer.nvidia.com/gpu-accelerated-texture-compression). NVTT offers compression and image processing with CPU fallback, but the exact filter behavior, selected SDK license/distribution terms, build inputs and packaged workflow still need proof. AMD's full framework also adds build surface beyond the currently selected `cmp_core`.

Adopt a whole-pipeline provider only when library calls own the substantial filtering, conversion, mip and compression mechanisms and the table is satisfied. Otherwise retain the current processing pipeline. A DDS-only change is a smaller complete container-reader replacement; it must not be presented as progress toward a full replacement if that full design leaves most processing ownership local.

## Clear smaller replacements

### JSON, followed by a gated format consolidation

Replace the tokenizer/reader/writer in [Core JSON](../Engine/Core/Private/Json) with nlohmann/json. The library owns grammar, Unicode handling, escaping and object/array serialization; domain readers declare required fields and convert values. Parse a document once and use focused domain results instead of recreating the old JSON helper facade over the new parser.

Preserve required-field duplicate rejection, exact unsigned integers and hash strings, malformed/trailing-input behavior, nesting limits, document lifetime and externally consumed receipts/manifests. The library's [parser callbacks](https://json.nlohmann.me/features/parsing/parser_callbacks/) support duplicate/depth policy without a second tokenizer. Recheck those policies against the selected released version rather than relying on defaults or unreleased convenience APIs.

The larger opportunity spans [level parsing](../Engine/GameFramework/Private/Level/Parsing), the [catalog reader](../Tools/Support/ToolSupport/Private/Projects/ProjectLevelCatalogReader.cpp), [texture cook requests](../Tools/Cooking/TextureCooker/Private/Requests/TextureCookRequestCodec.cpp) and [settings persistence](../Engine/Application/Private/RenderingSettings/EngineRenderingSettingsPersistence.cpp). These formats are not all JSON and the library cannot consume them unchanged. Consolidation requires explicit representation decisions and conversion of all owned files, readers, writers, scripts and package inputs while keeping the normal editing/build/cook/run workflows intact. Preserve human-editing requirements, including any required comment behavior. No parallel legacy parser or silent loss of accepted input is an acceptable result.

### Command-line parsing: CLI11

Replace the parsing in [Launcher](../Tools/Launcher/SparkleLauncher/Private/Shell/LauncherShellArguments.cpp), [AssetCooker](../Tools/Cooking/AssetCooker/Private/Cli/AssetCookerCli.cpp) and [ShaderCompiler](../Tools/Shaders/ShaderCompiler/Private/Cli/CookShadersArgumentParser.cpp) through CLI11 declarations, validators and help generation. Keep only mappings to existing typed requests and operation-specific rules.

Preserve accepted switches/aliases, defaults, repeated-value behavior, `--run`/`--dry-run` operation IDs, shader options, Unicode paths, exit codes, help/error use and automation. Do not redesign the command interface into different subcommands for the library's convenience. Outgoing Windows process argument quoting remains a separate process-launch responsibility. The [upstream API](https://github.com/CLIUtils/CLI11) supports option relationships and custom validators; the migration must demonstrate current invocations and failures through all three executables.

## Other areas and retained implementations

| Area | Decision under capability and ownership requirements |
| --- | --- |
| Launcher build state | Investigate [CMake presets](https://cmake.org/cmake/help/v4.3/manual/cmake-presets.7.html) and [File API](https://cmake.org/cmake/help/v4.3/manual/cmake-file-api.7.html) for configuration/freshness authority. Keep every supported IDE/compiler/profile, explicit prerequisite acquisition, readiness, progress, cancellation and recovery workflow. The source-list scans/cache model must actually disappear; presets added alongside the same complex model are not a completed replacement. |
| Launcher/Editor UI | Retain Qt and Dear ImGui and the existing workflows. No full replacement was identified for Sparkle-specific scene selection, editing, inspection and recovery. Their combined roughly 33,400 lines are not a library-deletion estimate. |
| ECS | EnTT is credible for [component storage](../Engine/GameFramework/Private/World/ECS/ComponentStorage.h) and the registry, but the direct surface is only about 585 lines. Defer behind larger gains. Match ID width, invalid IDs, generation exhaustion, iteration freezes and render identity through [documented registry facilities](https://github.com/skypjack/entt/wiki/Entity-Component-System); retain no parallel sparse store or allocator. Do not count world transforms, extraction or resource semantics as removed ECS code. |
| Animation | Defer Ozz until it replaces the sampling/storage/cooked representation path as a whole. Runtime animation is about 687 lines; the larger cross-system count includes required import and scene integration. Ozz's [generic tracks](https://github.com/guillaumeblanc/ozz-animation/blob/master/include/ozz/animation/offline/raw_track.h) expose step/linear interpolation. A conversion preserving Sparkle's cubic splines, quaternion sampling, morph weights, skeleton relationships and GPU skinning has not been demonstrated. Keeping the custom evaluator for unsupported tracks alongside Ozz is rejected. |
| Import | Retain cgltf, Assimp and MikkTSpace. Existing FBX and PLY routes use Assimp; changing FBX to [ufbx](https://github.com/ufbx/ufbx) does not remove PLY ownership or material/unit/scene translation. Its good testing evidence alone is not a large demonstrated transfer here. |
| Process execution | Retain the current owner until a complete replacement is proved. [libuv](https://docs.libuv.org/en/v1.x/process.html), despite about 27,300 stars, does not by itself establish Sparkle's job-tree ownership, cancellation, readiness validation and handoff. Adding its event loop while keeping the substantial native process subsystem is rejected. Moving shared process ownership into Launcher-only Qt would also fail existing Editor/tool use. |
| Source style checks | Retain current behavior. Clang already supplies standard formatting/naming analysis, but exact custom paragraph/boolean/decision rules have no verified complete replacement. Do not retire rules to manufacture savings or build a similarly complex custom Clang plugin. The current script's `clang-tidy --verify-config` checks configuration, not translation-unit analysis. |
| Shader support | Retain DXC/Slang and typed reflection/debug/publication contracts. [ShaderMake](https://github.com/NVIDIA-RTX/ShaderMake), about 118 stars, adds compilation orchestration but does not replace those owners. Native compiler preprocessing is a bounded investigation only after source identity/debug output equivalence is shown; no second regex preprocessor fallback. |
| Whole NVIDIA framework | [Donut](https://github.com/NVIDIA-RTX/Donut), about 479 stars, is not selected as a replacement engine. Its documented scene restrictions include morph targets and KTX2. Replacing the current scene/application stack without full capability parity, or maintaining both scene stacks, is rejected. |
| Packaging and publication | Retain accepted-output protection, file membership, provenance, rights/notices, relocation and dependency checks. CPack or a generic storage library does not replace those product contracts. Publisher-side binary inspection through LLVM is a small optional transfer only if tool-free consumer workflows remain intact. |

## Existing third-party ownership to preserve

GPU allocation already uses AMD VMA/D3D12MA. Logging uses spdlog; source import uses cgltf/Assimp; tangents use MikkTSpace; image loading uses stb/tinyexr; block compression uses AMD Compressonator; containers include KTX-Software; UI uses Qt/ImGui; shader compilation/reflection uses DXC, Slang and SPIRV-Reflect. [Dependency composition](../CMake/Dependencies/FetchDependencies.cmake), [RHI build](../Engine/RHI/CMakeLists.txt) and module build files own the actual selection.

Keep these established owners unless a complete, better replacement is justified. Sparkle retains renderer/RHI semantics, scene-to-render extraction, material interpretation, resource lifetime, domain validation and publication. Those responsibilities are required to preserve the rendering and product workflows; relabeling them as integration does not make them removable.

## Evidence required before a migration is complete

Before implementation, freeze a representative input/settings baseline, supported operations and failure cases, the chosen upstream release/commit, actual Windows compiler/build inputs, and numerical/performance tolerances. Record the selected capability's first-party algorithms, state machines and source/build membership so the final ownership reduction can be reviewed.

| Evidence | Completion requirement |
| --- | --- |
| Behavior | Existing GUI/CLI workflows and their required failures/recovery still work. No selector, supported input or operation is silently removed. |
| Ownership | The upstream owns every generic mechanism claimed as replaced. Remaining code is domain policy, mapping and orchestration. Report deleted/retained mechanisms and source, not gross module size. |
| Single implementation | Replaced production code, duplicate registries/metadata, fallback implementations and old-format readers are removed. Regenerate disposable outputs through the normal workflow. |
| Numerical/content agreement | Freeze tolerances first; compare images, poses, filters, decoded records and relevant identities. New output cannot inherit an old image/package pass. |
| Cost | Measure affected startup, frame/cook time, memory, cancellation and shutdown against the existing roadmap limits. Include build time and binary/dependency growth. |
| Delivery | Both graphics APIs where affected, applicable static/shared configurations, offline prebuilt and matching-source workflows, relocatable outputs, notices and exact dependency membership pass their affected checks. |
| Evidence limits | Source/API research, upstream CI and a successful build are separate from Sparkle runtime, GPU, package and performance qualification. Keep existing permanent tests and follow the local-only probe rules. |

Settle the trial before changing a candidate's decision: either preserve capability and remove the old generic implementation, or keep the current implementation and omit the added dependency. The current research status for every proposed migration remains **not implemented; Sparkle qualification pending**.
