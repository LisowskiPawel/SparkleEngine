# Third-party migration

[Documentation](README.md) | [Roadmap](Roadmap.md#supporting-system-simplification) | [Developer guide](Engineering.md)

Replace generic infrastructure with maintained libraries so Sparkle can concentrate on rendering. The return should be fewer custom parsers, schedulers and codecs to debug and maintain, with the same supported workflows and output quality.

This inventory covers rendering/RHI, Core/Tasks, Platform/GameFramework, Editor/Launcher, import/cook/shader tools, build/packaging and offline numerical/model evidence within the [selected product scope](Goals.md#product-scope). It includes libraries, SDKs and external tools: a better content check, reference image or build loop can improve rendering work without adding engine code. New feature requirements can justify revisiting the inventory.

- [Prioritized replacements](#candidates-and-expected-benefit)
- [Selected rendering SDKs](#rendering-sdks-worth-considering)
- [Optional libraries and tools by responsibility](#optional-libraries-and-tools)
- [Existing dependencies](#existing-dependencies)

## Fit to the engineering goals

Choose dependencies against the [target persona and three portfolio cases](Goals.md), keeping delivery order in [Roadmap](Roadmap.md). The [principal DevTech role](https://nvidia.wd5.myworkdayjobs.com/NVIDIAExternalCareerSite/job/Germany-Remote/Principal-Developer-Technology-Engineer_JR2013547) emphasizes path tracing, neural graphics, hardware reasoning, measured optimization and developer adoption. AMD's [Vulkan production integration study](https://gpuopen.com/learn/porting-detroit-1/) illustrates partner constraints and engine integration. Our implication is to invest in complete rendering studies and remove supporting machinery that competes for that time.

| Ownership | What belongs in Sparkle |
| --- | --- |
| Rendering expertise | Transport and sampling decisions, material/scene interpretation, an independently checked Reference path, frame-graph scheduling/barriers/aliasing, explicit API synchronization and resource lifetime, and the selected owned neural model/kernels. Keep an implementation when a portfolio case actually examines it. |
| General machinery | Use established parsers, codecs, allocation and platform libraries where they remove local implementations. Retain domain validation, publication, cancellation and product behavior that the library does not provide. |
| AMD/NVIDIA integration | Own signal conversion, capabilities, resource states, reset/lifetime, failure behavior, packaging and measured comparisons. Fetch the selected upstream algorithm without copying its sample engine or building a parallel local version. Vendor integration and personally owned algorithm/model work provide different evidence. |

Cleanup requires a reduction in maintained mechanisms. A rendering SDK can instead earn its place through a selected capability and reproducible result; record that as added scope, with its cost. Neither dependency count nor SDK names establish principal-level expertise or partner adoption.

## Candidates and expected benefit

| Area | Candidate and local removal opportunity | Decision |
| --- | --- | --- |
| Command lines | [CLI11](https://github.com/CLIUtils/CLI11) replaces repeated option parsing, help generation and common validation in Launcher, AssetCooker and ShaderCompiler. | First integration; preserve the current command interface. Use the active [implementation plan](Implementation/CLI11.md). |
| JSON | [nlohmann/json](https://github.com/nlohmann/json) replaces [Core JSON](../Engine/Core/Private/Json) tokenization/writing and callers' repeated property scans. | Next bounded migration. Parse each document once; keep field/domain conversion. Other text formats need an explicit conversion decision. |
| Existing dependencies | DXC/Slang can take over include preprocessing; TinyEXR can use the already selected zlib backend. See [reuse opportunities](#reuse-before-adding-a-library). | Investigate alongside parser work; adds no new library. Removal is still conditional on equivalence. |
| Task execution | [oneTBB](https://github.com/uxlfoundation/oneTBB) could replace [Tasks](../Engine/Tasks/Private) worker queues, graph execution and parallel partitioning across frame, loading and cook consumers. | Largest horizontal candidate here, with high lifecycle risk. Trial before adoption; avoid rebuilding the same scheduler around it. |
| Cooked metadata | [FlatBuffers](https://github.com/google/flatbuffers) could replace [manifest wire records](../Engine/GameFramework/Public/Assets/Cooked/CookedSceneManifest.h), matching readers/writers and structural byte checks. | Trial a complete record family. Preserve packed GPU payloads and domain checks; schema evolution is unnecessary for disposable output. |
| Texture cooking | Broaden existing [Compressonator](https://github.com/GPUOpen-Tools/compressonator) use before adding [DirectXTex](https://github.com/microsoft/DirectXTex) or [NVTT](https://developer.nvidia.com/gpu-accelerated-texture-compression). Targets are [DDS loading](../Tools/Cooking/TextureCooker/Private/SourceLoading/DdsTextureSourceLoader.cpp), [mips](../Tools/Cooking/TextureCooker/Private/Pipeline/Stages/MipStage.cpp), conversion and block-loop preparation. | Prefer one processing owner. DDS-only is a smaller alternative; no complete cooker replacement is established. |
| Window and input | [SDL3](https://github.com/libsdl-org/SDL) could replace [native window machinery](../Engine/Platform/Private/Window), [Win32 input](../Engine/Platform/Private/Input/Backends) and ImGui's platform backend. | Conditional, after the larger trials. Retain viewport actions/routing; remove the replaced native path. |
| Profiling | [Tracy](https://github.com/wolfpld/tracy) supports CPU and D3D12/Vulkan GPU profiling. Possible overlap is [frame timing collection](../Engine/Renderer/Private/Diagnostics/FrameExecutionDiagnostics.cpp) and task instrumentation. | Defer until an analysis consumer needs it and a retirement scope is clear. Task ETW instrumentation and native captures already exist. Any trial stays in development builds with local capture, preserving required product counters and offline behavior. |

Start with CLI11, then JSON. Investigate task execution and metadata before committing them; choose the later texture/platform work from measured removal and integration cost.

The broader inventory also identifies focused Vulkan entry-point loading (`volk`) and packaging PE parsing (`LIEF`) replacement trials. Offline image/numerical tools and local compiler caching can strengthen results and iteration without adding Game dependencies. Their use conditions are in the [optional inventory](#optional-libraries-and-tools).

## Rendering SDKs worth considering

New SDK additions follow the existing [rendering work](Rendering.md#later-rendering-work) after release. Finish and qualify the current integration before adding another provider.

| Technology | Current engine and opportunity | Scope and decision |
| --- | --- | --- |
| NVIDIA Streamline / DLSS SR and RR | Already selected in CMake, with [Renderer integration](../Engine/Renderer/Private/Streamline). Runtime initialization currently rejects APIs other than D3D12. | Highest immediate integration value: qualify guides, exposure, jitter/motion, resets, native state and package behavior. Upstream [Vulkan integration](https://github.com/NVIDIA-RTX/Streamline/blob/main/docs/ProgrammingGuide.md) exists; extending Sparkle is separate work. Avoid a second direct NGX implementation. This adds no new algorithm dependency. |
| AMD FSR upscaling | No FSR provider exists. It would extend the existing [upscaler selection](../Engine/Renderer/Private/Upscaling/UpscalerProviderFactory.cpp) with a useful vendor integration comparison. | First AMD reconstruction candidate after release. The [current FSR SDK](https://gpuopen.com/amd-fsr-sdk/) separates analytical fallback from hardware-restricted ML features and requires signed loader/effect binaries. Select upscaling only; test D3D12/Vulkan separately. It adds capability, with no substantial current temporal upscaler to delete. Keep the small linear control. |
| NVIDIA NRD | No standalone denoiser implementation was found; current ray-reconstruction selection is Off or DLSS RR. [NRD](https://github.com/NVIDIA-RTX/NRD) offers API-agnostic classical diffuse/specular denoising. | Fits the planned RELAX path and provides a classical comparison for the owned neural study. Integrate through Sparkle's RHI or bounded native interop; NRD's NRI-based helper is optional. Avoid importing NRI as another general RHI merely to use the denoiser. Addition, not present code removal. |
| AMD FidelityFX SPD / Lens | Local [exposure reduction](../Engine/Assets/Shaders/Passes/PostProcessing/Exposure) and small lens passes provide possible overlap with [SPD](https://gpuopen.com/manuals/fidelityfx_sdk/techniques/single-pass-downsampler/) and [Lens](https://gpuopen.com/manuals/fidelityfx_sdk/techniques/lens/). | Narrow shader-level trial when touching these passes. Preserve luminance/reduction meaning, edge/odd-size behavior, image domain and pass ordering. Adopt only if replaced shaders/dispatch machinery disappear and total cost improves. A full SDK/sample framework for a few equations has poor return. |
| RTXDI | Custom direct/indirect reservoir sampling exists in [lighting shaders](../Engine/Assets/Shaders/Passes/Lighting). [RTXDI](https://github.com/NVIDIA-RTX/RTXDI/blob/main/Doc/Integration.md) supplies sampling math through renderer callbacks. | Keep it as a study/control under the current rendering decision. Replacing an estimator deliberately examined by the portfolio would change the owned evidence. If integration becomes the chosen case, replace a bounded production estimator completely and retain only the necessary analytic/reference control. No second full lighting stack. |
| SHaRC, NRC and NTC | Planned, with no corresponding local cache or neural texture codec to retire. [SHaRC](https://github.com/NVIDIA-RTX/SHARC) is shader-only; [NRC](https://github.com/NVIDIA-RTX/NRC) and [NTC](https://github.com/NVIDIA-RTX/RTXNTC) have separate runtime/toolchain obligations. | Follow the existing sequence: SHaRC before NRC; NTC after the classical study. Fetch selected components, not their renderers. NTC on-load can reduce storage/transfer while retaining BCn VRAM; on-sample has different filtering/compute costs. Audit the selected revision's preview-feature and redistribution limits. These demonstrate integration, not ownership of the learned model. |
| Native debugging and performance tools | PIX, RenderDoc, Nsight/Aftermath, native validation and marker integration already have source owners. AMD's [RGP](https://gpuopen.com/rgp/), RGA, RMV and RRA complement them. | Prefer external tools over an in-engine profiler/capture analyzer. Use shader/source identity and bounded capture hooks. The RTX benchmark machine can exercise portable AMD algorithms, but cannot establish Radeon hardware performance; additional hardware evidence remains separately scoped. |

AMD's current SDK also exposes Ray Regeneration and radiance caching. They have different hardware/API requirements from upscaling; the reviewed [requirements](https://gpuopen.com/amd-fsr-sdk/#requirements) list both as D3D12-only, Ray Regeneration as requiring Radeon RX 9000 or later, and radiance caching as a technical preview. Defer them while the selected NRD/DLSS and SHaRC/NRC work is unfinished. Frame generation remains outside [product scope](Goals.md#product-scope).

## Follow the complete path

Horizontal replacement must cover all consumers; vertical integration must end in usable output. Use these existing paths to define each migration boundary.

| Path | Keep locally | Replacement and evidence |
| --- | --- | --- |
| Source asset → import → cook → publication → load → pixel | Axes/units, material channels, skin/morph identity, packed GPU representation, accepted-output protection | Reuse import libraries; choose one texture processor and, conditionally, one metadata schema. Delete matching old readers/writers and recook. Check decoded values, animated/material images, malformed inputs and failed publication. |
| Shader source → compiler → reflection → cooked library → native execution | Typed host/shader contracts, binding policy, identity, debug correlation, publication and reload lifetime | Delegate preprocessing/compilation to DXC/Slang and reflection/disassembly to existing upstream tools. Check conditional includes, diagnostics, source hashes, layouts and both DXIL/SPIR-V outputs through actual native use. |
| Scene/frame work → tasks → submission → GPU completion | Immutable extraction, frame-graph decisions, queue ownership and deferred retirement | Trial one task runtime across frame work, loading and cooking. Check serial agreement, cancellation/settlement, blocking-I/O isolation and shutdown; CPU completion never establishes GPU completion. |
| Noisy lighting → guides → reconstruction → presentation → comparison | Signal meaning, motion/jitter/exposure units, disocclusion/reset rules and independent raw reference | Integrate only the selected provider and its package files. Compare raw bias separately from reconstruction quality, temporal artifacts, whole-frame cost and memory. Exercise resize, camera cut, unsupported hardware and provider failure on each supported API. |

## Requirements for adoption

- Preserve supported inputs, GUI/CLI workflows, output quality, cancellation, recovery and offline packaged use. Keep both graphics APIs and existing release limits.
- A cleanup dependency must replace substantial generic machinery. Keep domain rules, value conversion and lifetime orchestration in Sparkle; remove the old implementation in the same change. Avoid a second parser, scheduler, codec or native backend hidden behind an adapter. Capability additions need the selected rendering result and explicit maintenance cost described above.
- Select a maintained release with suitable Windows/toolchain support, license and distribution terms. Include dependency, generator and runtime files in build and package checks.
- Compare a fixed baseline before adoption: behavior and failure cases, numerical agreement, startup/frame/cook time, memory, shutdown, build cost and binary size. Demonstrate offline prebuilt and matching-source workflows, including affected static/shared configurations.

Before adopting a cleanup candidate, name the files/mechanisms to delete, remaining adapters, consumers and new build/runtime obligations. Report net first-party code, public surface, mutable state and failure paths as well as fetched bytes and package size. Generated codecs count toward build/review cost. Keep one library per responsibility; remove duplicate dependency trees and unused feature variants. Skip a migration when its integration is larger or harder to reason about than the implementation it retires.

If preserving capability requires a fork, another substantial local implementation or unacceptable costs, keep the working implementation and omit the dependency. Upstream tests and API research do not establish Sparkle runtime or package correctness.

## Ownership and dependency workflow

Keep Sparkle's distinctive work in rendering, GPU/API reasoning, scene/material semantics and useful developer workflows. Delegate general-purpose machinery to established projects. Judge a migration by the implementation and failure states it removes, including the integration code it adds.

| Responsibility | Existing location |
| --- | --- |
| Source selection, pinned revision and upstream target composition | [FetchDependencies.cmake](../CMake/Dependencies/FetchDependencies.cmake) |
| Downloads, revision checks and cache protection | [DependencyCache.cmake](../CMake/Dependencies/DependencyCache.cmake) |
| Upstream source and generated build files | Ignored `build/_deps`, shared by workspace and standalone Launcher builds |
| Launcher identity, display version, purpose, required files and availability | [SourceDependencyState.cpp](../Tools/Launcher/SparkleLauncher/Private/Dependencies/SourceDependencyState.cpp); GUI rows derive from this inventory |
| Distribution identity and obligations | Existing [notices](../CMake/Notices/ThirdPartyNotices.txt), package inputs, SBOM and provenance under [Packaging](../CMake/Packaging) |

New libraries use this route. Keep fetched upstream code unmodified and outside `Engine`/`Tools`; keep only focused domain integration in Sparkle. Existing checked-in D3DX12 headers remain identifiable under `Engine/RHI/Private/D3D12/ThirdParty`. Apply Sparkle's formatting and design rules to owned integration, not upstream source. Use upstream CMake targets with private usage where possible, and group actual dependency targets under `ThirdParty`.

CMake owns the fetch pin. Launcher metadata must agree on identity, version, cache path and required files without adding another registry. Launcher currently checks required-file presence; CMake verifies Git revision and cleanliness. Test both. Sync Code and individual dependency Sync/Clean must use the same cache and existing operations; cache failures preserve contents for inspection.

For additions and upgrades, check maintenance, Windows support, license, affected consumers, reproducible source acquisition and package membership. SDK source, model/data files, signed runtime binaries and sample media can have different terms; review the actual selected files. Keep optional GPU SDKs out of unrelated builds and prebuilt use independent of source downloads. Disable unused samples, tests, tools, backends and media acquisition. Measure remaining first-party mechanisms and build/runtime cost. Remove unused dependencies, options, adapters and notices when their consumers are retired.

## What decides the fit

### Reuse before adding a library

[ShaderSourcePreprocessor.cpp](../Tools/Shaders/ShaderCompiler/Private/Compiler/ShaderSourcePreprocessor.cpp) expands includes and pragma-once through regular expressions before the backend compiles the result. [DXC include handlers](https://github.com/microsoft/DirectXShaderCompiler/wiki/Using-dxc.exe-and-dxcompiler.dll) and [Slang's compiler filesystem](https://shader-slang.org/slang/user-guide/compiling.html) offer an existing-dependency replacement. Preserve virtual mounts, generated typed parameters, include-closure identity, changed-source selection and capture/debug source correlation. Test disabled/macro includes, cycles and repeated includes; remove the regex expander after both backends agree. This is a focused compiler integration, not a reason to add ShaderMake or another compiler framework.

TextureCooker currently builds `TextureCookerMiniz` from TinyEXR's bundled source, while source dependencies also select zlib for Assimp. The pinned [TinyEXR header](https://github.com/syoyo/tinyexr/blob/v1.0.7/tinyexr.h) supports a zlib-compatible backend with `TINYEXR_USE_MINIZ=0`. Trial using the existing zlib target, removing the extra miniz target/include paths if EXR decode/error behavior and build configurations agree. Correct the dependency description to match the delivered backend. This reduces dependency duplication rather than promising a large first-party code deletion.

### JSON

JSON must preserve required-field duplicate rejection, exact unsigned integers and hash strings, malformed/trailing-input handling, nesting limits and document lifetime. [Parser callbacks](https://json.nlohmann.me/features/parsing/parser_callbacks/) provide policy hooks; verify them against the chosen release. Parse once and consume domain values without rebuilding the old helper facade.

Levels, catalogs, cook requests and settings are not all JSON. Broader consolidation must update owned files, readers, writers, scripts and package inputs together, preserving normal editing and any required comments. Keep the existing format when that cannot be done without a second complex parser.

### Task execution

The replacement must cover dependency ordering and failure propagation, cleanup after cancellation, parent/child scope cancellation, timed settlement and prepared notify/coalesce/close dispatch. Preserve the deterministic caller-thread reference path when all worker counts are zero. Blocking I/O must not consume all capacity needed by frame work.

A one-worker arena does not prove caller-thread behavior, and graph cancellation does not prove cleanup callbacks run. Check [oneTBB cancellation](https://uxlfoundation.github.io/oneTBB/main/tbb_userguide/Flow-Graph-exception-tips.html) against actual consumers before choosing the design.

[Taskflow](https://taskflow.github.io/taskflow/RequestCancellation.html) and [enkiTS](https://github.com/dougbinks/enkiTS) remain alternatives. Taskflow cancellation can skip successors, which matters when they perform cleanup. Adopt one runtime only if it replaces the scheduling and dependency machinery.

### Cooked metadata

Trial the complete scene-manifest schema, writer, reader and [structural verifier](https://flatbuffers.dev/languages/cpp/#access-of-untrusted-buffers). Pin a released compiler/runtime pair and check reproducible generated C++ on Windows; the research snapshot left generator/version selection unresolved.

Sparkle still checks finite values, asset relationships, skin/morph ranges, material meaning, allocation limits and accepted-output publication. Keep bulk GPU payloads suitably packed.

Test malformed/truncated input, interruption, scene equivalence and load/memory cost. Before landing the dependency, define a scope that retires substantial manual codecs. One converted manifest alongside the old general serialization system is insufficient.

### Window and input

Preserve custom title bars, borderless/windowed operation, resize/minimize/restore, startup/close, 100/150/200% DPI, mouse capture/focus loss, cursor restoration, shortcuts, deferred dispatch and viewport navigation.

SDL3 has [hit-test callbacks](https://wiki.libsdl.org/SDL3/SDL_SetWindowHitTest) for custom window regions; its native-handle and relative-mouse APIs and the [ImGui SDL3 backend](https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_sdl3.h) are relevant integration points. Qualify D3D12 and Vulkan surfaces. Sparkle keeps camera actions and viewport routing; RHI keeps swapchain/rendering ownership.

Remove the replaced Win32 event/input and ImGui platform backends. [GLFW](https://github.com/glfw/glfw) is an alternative only if it meets the same requirements without substantial extra native machinery.

### Texture cooking

Preserve Regular, Kaiser and NormalAware filtering; Generate, PreserveExisting and NoMips policies; sRGB/linear/HDR meaning, channels and alpha; BC1/BC4/BC5/BC6H behavior; supported DDS/KTX/raster/EXR sources, arrays and cubemaps. Keep batch cancellation, memory limits and accepted-output protection.

DirectXTex [filter options](https://github.com/microsoft/DirectXTex/wiki/Filter-Flags) do not establish equivalence for the current Kaiser kernel or decoded-normal averaging/renormalization. Compare numerical and visual results before replacing them.

For broader processing, investigate [AMD texture-level APIs](https://github.com/GPUOpen-Tools/compressonator/blob/master/docs/source/developer_sdk/cmp_framework/index.rst) or [NVTT 3](https://developer.nvidia.com/gpu-accelerated-texture-compression). Required filters/codecs, edge handling, HDR range/sign, quality, CPU fallback, build inputs and redistribution terms still need checking. A DDS-only replacement should be described and measured as that smaller result.

The current target delegates block encoding to sparse `CMP_Core` and resizing to stb. A Compressonator Framework trial must show what additional mip/container/processing code it actually retires and account for its plugins/dependencies. Compare it with tool-only DirectXTex and NVTT without landing all three. NVTT advertises GPU processing with CPU fallback, but source acquisition and binary redistribution need separate review. Keep texture semantic policy and the bounded publication workflow in Sparkle whichever processor wins.

## Optional libraries and tools

The prioritized replacements and selected rendering SDKs above are part of this inventory. The expandable tables add alternatives and supporting tools, with a concrete benefit and a condition for use. **Replacement** means a possible retirement of local machinery; **capability** adds functionality; **external** keeps the dependency in a developer/analysis environment; **defer** means the current implementation or scope remains preferable. Alternatives compete for one responsibility.

### Reliability and selection

Use official upstream projects with identifiable maintainers, clear source/binary licenses, documented APIs, upstream tests and a release or established maintenance history. The tables identify vendor, foundation or established community provenance. Community libraries can be good choices without vendor support; their Windows/toolchain coverage and maintainer continuity still need review. Stars and the presence of a package-manager recipe are insufficient evidence.

Before selecting a pin, inspect its supported branch, releases, unresolved defects/security advisories, tests and transitive dependencies. Review actual license files; repository metadata can omit mixed licenses or binary/model terms. Use official source/releases and verify identity through the existing cache. A reputable project qualifies for evaluation; its chosen release and Sparkle integration still need qualification. Vendor research samples and preview-only features below are study material until those conditions are met.

<details>
<summary>Data, execution, storage and process infrastructure</summary>

| Project and provenance | Benefit to Sparkle | Use and adoption condition |
| --- | --- | --- |
| [simdjson](https://github.com/simdjson/simdjson), established SIMD parsing project | Faster large JSON ingestion and fewer repeated scans during discovery/import. | **Replacement alternative** to nlohmann/json if profiling justifies its parser/lifetime model. It does not provide a matching general JSON writer; account for that before selecting the pair. Keep duplicate-field, integer and malformed-input policy. |
| [toml++](https://github.com/marzer/tomlplusplus), maintained C++ TOML project | Standard commented configuration could retire lexical machinery in owned settings/catalog/level text formats. | **Replacement, defer** until one format conversion covers readers, writers, scripts and editing. Prefer the selected JSON route where it fits; avoid maintaining custom syntax plus JSON plus TOML. |
| [SQLite](https://www.sqlite.org/about.html), SQLite project | An indexed cook/dependency catalog could replace increasingly complex scans and ad hoc lookup/index storage, improving iteration on large scenes. | **Replacement/capability, defer** until catalog queries and scale justify a database. Keep cooked-only offline loading simple. A database transaction does not make external cooked-file publication atomic. |
| [Taskflow](https://github.com/taskflow/taskflow) or [enkiTS](https://github.com/dougbinks/enkiTS), established task projects | Alternatives for dependency execution or lightweight task partitioning if oneTBB is a poor fit. | **Replacement alternatives** to the existing Tasks runtime, evaluated against the same cancellation, cleanup, dispatch and serial-path requirements. Select one runtime. |
| [Boost.Process](https://www.boost.org/libs/process), Boost | Could replace portions of [Core process](../Engine/Core/Private/Process) launch/pipe/wait machinery shared by tools, Editor and Launcher. | **Replacement, defer.** Current v2 brings Asio dependencies; prove Windows job/process-tree cancellation, readiness, quoting, output drainage and shadow handoff before adoption. A smaller launch wrapper alone has weak return. |
| [Zstandard](https://github.com/meta/zstd), Meta | Compressed cooked payloads or transfer artifacts could reduce disk/I/O pressure and scene startup cost. | **Capability**, with replacement only for the chosen container/compression path. Use existing KTX support where applicable. Measure decode CPU/workspace and total load time; retain bounded decompression and content identity. |
| [LZ4](https://github.com/lz4/lz4), established compression project | Alternative for a measured latency-sensitive cooked-data decompression workload. | **Capability alternative** to Zstandard for the same payload. Choose from real size/load/memory results; do not add both by default. No substantial local compression algorithm currently needs retirement. |
| [libarchive](https://github.com/libarchive/libarchive), established archive project | Could consolidate archive handling if source/content acquisition needs more than existing ZIP/.NET/CMake operations. | **Replacement, defer** until archive machinery actually disappears. Preserve member/path/size limits, interrupted extraction, package membership and accepted-output protection. |
| [xxHash](https://github.com/Cyan4973/xxHash), established hashing project | Could replace [FNV hashing](../Engine/Core/Private/Hash/FnvHash.cpp) for measured cook/cache-key throughput. | **Replacement, conditional.** Change owned keys in place and regenerate disposable output. Keep collision policy explicit and SHA-256 for distribution/provenance integrity. The existing hash code is small. |
| [mimalloc](https://github.com/microsoft/mimalloc), Microsoft | A bounded CPU-allocation experiment could reduce contention or fragmentation during scene loading/cooking. | **Capability, defer** until profiles identify allocator cost after unnecessary allocations/copies are removed. Check DLL allocation/free ownership, peak memory and shutdown. VMA/D3D12MA remain GPU allocation owners. |

</details>

<details>
<summary>Asset cooking, images, materials, animation and editor widgets</summary>

| Project and provenance | Benefit to Sparkle | Use and adoption condition |
| --- | --- | --- |
| [meshoptimizer](https://github.com/zeux/meshoptimizer), established mesh-processing project | Vertex-cache/fetch optimization, simplification and compression can improve content size and rendering efficiency without writing another optimizer. | **Capability**, with possible future replacement of equivalent cook code. No local mesh optimizer was found to retire. Check skin/morph remaps, triangle/material identity and raster/ray agreement. Meshlets remain outside selected scope. |
| [ufbx](https://github.com/ufbx/ufbx), focused FBX project with extensive validation | A smaller FBX import dependency with scene/animation evaluation could simplify the Assimp-based path. | **Replacement, defer.** Assimp also supplies PLY; complete retirement needs a supported disposition for it and preservation of animation, materials, axes/units and error handling. Avoid adding a second FBX importer. |
| [ozz-animation](https://github.com/guillaumeblanc/ozz-animation), established animation project | Could replace CPU animation sampling/pose evaluation and add compressed animation data for animated rendering scenes. | **Replacement, defer** while the local evaluator remains small. Trial cubic splines, quaternion sampling, morph weights, skeleton relationships and GPU-skinning agreement. Retaining an equivalent custom evaluator defeats the cleanup. |
| [EnTT](https://github.com/skypjack/entt), established C++ ECS project | Could replace entity/component storage and iteration if scene scale makes the current typed store costly to maintain. | **Replacement, defer.** Preserve ID width/validity, generation exhaustion, iteration freezes and separate render identity. Keep a single world store. No general gameplay framework is needed. |
| [OpenImageIO](https://github.com/AcademySoftwareFoundation/OpenImageIO), Academy Software Foundation | Its tools and image API can replace custom image conversion, channel operations and comparison scripts; a larger cook migration could consolidate source decoders. | **External first; replacement conditional.** Build only required formats/plugins and compare total dependency cost with stb/TinyEXR/DirectXTex. Avoid shipping a VFX tool stack to decode a few formats. |
| [OpenEXR](https://openexr.com/en/latest/), Academy Software Foundation | Reference AOVs, HDR datasets and richer EXR metadata/channels could benefit from the format's reference implementation. | **External or replacement** for TinyEXR when actual file requirements justify it. Preserve numeric values, channel conventions, bounded decode and supported formats. Select a maintained patched release. |
| [Basis Universal](https://github.com/BinomialLLC/basis_universal), Binomial; [KTX integration](https://github.com/KhronosGroup/KTX-Software) from Khronos | A texture storage/transcode comparison could reduce download/install size while targeting existing GPU formats. | **Capability, conditional.** Reuse the existing KTX dependency's facilities before another Basis copy. Compare HDR/normal/alpha fidelity and transcode memory/time with BCn. This does not imply a GPU-memory reduction. |
| [OpenColorIO](https://opencolorio.readthedocs.io/en/latest/), Academy Software Foundation | Offline reference transforms and LUT baking can remove bespoke color-conversion tools and improve image-domain agreement. | **External first.** Runtime adoption requires a real color-management requirement beyond the selected grading/LUT contract. Keep exposure/display semantics explicit and avoid a second color authority. |
| [glTF Validator](https://github.com/KhronosGroup/glTF-Validator), Khronos | Source asset reports help distinguish malformed inputs from importer/material/rendering defects before cooking. | **External.** Retain Sparkle's domain checks; standard glTF validity does not establish supported extension semantics or correct pixels. Use official binaries/packages. |
| [glTF Sample Viewer](https://github.com/KhronosGroup/glTF-Sample-Viewer), Khronos | An independent material/animation presentation check can shorten importer debugging. | **External control**, matching extensions, camera, lighting, exposure and display. It is not the raw transport oracle. |
| [ImPlot](https://github.com/epezent/implot), established Dear ImGui companion | Could replace custom live graph drawing if an existing inspection workflow needs interactive plots. | **Capability/replacement, defer** until a concrete consumer exists. Prefer offline plots for published measurements; retain Qt and ImGui as the UI owners. |
| [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo), established Dear ImGui companion | Could supply narrow transform/view widgets for repeatable camera/light placement instead of new bespoke gizmo code. | **Capability, defer.** Use selected widgets only; preserve typed edits, undo, coordinate/matrix conventions and viewport camera ownership. Broad authoring remains outside scope. |

The texture-processor alternatives remain Compressonator, DirectXTex and NVTT from the prioritized table. Adding image tools does not automatically justify another decoder or encoder in the shipped path.

</details>

<details>
<summary>Graphics API plumbing and additional rendering integrations</summary>

| Project and provenance | Benefit to Sparkle | Use and adoption condition |
| --- | --- | --- |
| [volk](https://github.com/zeux/volk), established Vulkan loader project | Could retire repeated function-pointer loading in [Vulkan ray tracing](../Engine/RHI/Private/Vulkan/Device/VulkanRhiRayTracing.cpp) and diagnostics. | **Focused replacement trial.** Preserve feature enabling, per-device dispatch, promoted/extension entry points, external captures and Streamline interposition. Remove the replaced loader tables/assignments; keep capability policy in Sparkle. |
| [vk-bootstrap](https://github.com/charles-lunarg/vk-bootstrap), maintained Vulkan utility project | Could reduce generic instance/device/queue/swapchain initialization code. | **Replacement, defer.** Preserve requested features/extension chains, device choice, HDR/presentation, validation and provider/capture hooks without rebuilding equivalent initialization around the library. |
| [DirectX Agility SDK](https://devblogs.microsoft.com/directx/directx12agility/), Microsoft | Enables deliberate modern D3D12 feature/compiler experiments needed by selected neural/API studies. | **Capability/toolchain component.** Distinguish released deployment support from previews, driver requirements and packaged DLL selection. Keep Vulkan evidence separate and preserve the baseline path. |
| [AMD GPU Services](https://github.com/GPUOpen-LibrariesAndSDKs/AGS_SDK), AMD | AMD-specific D3D extensions and driver/device queries can support a concrete shader or hardware experiment, complementing existing NVAPI. | **Capability, conditional** on a feature unavailable through the chosen standard API. Keep vendor types backend-private and report unsupported requests accurately. No speculative extension framework. |
| [XeSS Super Resolution](https://github.com/intel/xess), Intel | An optional additional reconstruction comparison can test the quality of motion/depth/exposure/provider contracts. | **Capability, lower priority** than existing DLSS, selected FSR and RELAX work. Select SR only; check the selected release's API/device/runtime support. Frame generation remains outside scope. |
| [Reflex](https://developer.nvidia.com/performance-rendering-tools/reflex), NVIDIA; [Anti-Lag 2](https://gpuopen.com/anti-lag-2/), AMD | CPU/GPU pacing and latency-marker studies can improve responsiveness and whole-system performance evidence. Streamline already contains Reflex/PCL integration points. | **Capability/qualification, conditional.** Reuse the current NVIDIA path, and add AMD integration only for a selected latency study. Limit the trial to pacing/markers, verify actual hardware/API support and separate throughput, PC latency and input-to-photon measurements. |
| [FidelityFX components](https://gpuopen.com/manuals/fidelityfx_sdk/), AMD | CAS, Blur, DoF and LPM offer bounded sharpening/filtering/lens/display alternatives; Parallel Sort can avoid a custom GPU sorting implementation when a selected pass needs one. | **Capability or replacement**, one required component at a time. Match signal semantics and both shader targets; keep upstream callbacks narrow. SPD/Lens have the more concrete current overlap. |
| [FidelityFX reconstruction alternatives](https://gpuopen.com/manuals/fidelityfx_sdk/), AMD | Classical Denoiser, SSSR/Hybrid Reflections and CACAO can serve as bounded fallback/comparison techniques. | **Defer** until a selected comparison needs them. They add passes rather than remove current transport machinery; avoid duplicating indirect lighting or applying AO that changes the declared reference domain. |
| [RTX Texture Filtering](https://github.com/NVIDIA-RTX/RTXTF), NVIDIA | Stochastic filtering provides the upstream integration/control relevant to on-sample neural texture decoding. | **Capability/study** within the NTC work. Check texture derivatives/LOD, temporal noise, material fidelity and filtering/denoising cost. It does not establish an unbiased lighting estimator. |
| [Spatiotemporal Blue Noise](https://github.com/NVIDIA-RTX/STBN), NVIDIA | Published assets/generator provide a sampling-pattern comparison without developing a noise optimizer. | **External study; selected assets only** if experiments justify runtime use. Review asset rights and sample mapping/correlation; compare equal-cost convergence and temporal behavior. |
| [RTX Neural Shading](https://github.com/NVIDIA-RTX/RTXNS), NVIDIA | Train-to-shader examples inform the owned model/kernel path and optional cooperative-vector experiments. | **External study**, with preview/toolchain requirements checked. Bring back a bounded technique or upstream primitive, preserving the independently checked portable FP32 path. Do not import its sample renderer or claim its trained model as owned work. |
| [NVRHI](https://github.com/NVIDIA-RTX/NVRHI), [NRI](https://github.com/NVIDIA-RTX/NRI), NVIDIA; [RPS](https://github.com/GPUOpen-LibrariesAndSDKs/RenderPipelineShaders), AMD | Whole-owner alternatives for RHI/resource-lifetime or frame-graph machinery; useful design/performance controls. | **Defer adoption.** Current goals deliberately examine these owners. Replacement needs a complete retirement boundary and an explicit change to the portfolio evidence; stacking them beneath equivalent Sparkle code increases maintenance. |
| [DirectStorage](https://github.com/microsoft/DirectStorage), Microsoft | A measured Windows load/I/O bottleneck could justify batched I/O or GPU decompression. | **Capability, defer** until ordinary loading/decompression is measured. Its D3D12 integration does not establish a Vulkan route, shader/asset publication safety or a need for a new streaming subsystem. |

Streamline, FSR, NRD, RTXDI, SHaRC, NRC and NTC remain in the selected SDK table above. NVIDIA RTXMU is useful AS-compaction design material, but a new compaction system is outside the immediate cleanup; use existing allocation owners and native API studies first.

</details>

<details>
<summary>Native debugging, tracing and performance evidence</summary>

| Project and provenance | Benefit to Sparkle | Use and adoption condition |
| --- | --- | --- |
| [PIX](https://devblogs.microsoft.com/pix/), Microsoft; [RenderDoc](https://github.com/baldurk/renderdoc), established capture project | Native captures reveal actual resources, bindings, barriers, shader identity and incorrect output. Existing adapters already provide integration points. | **External, reuse.** Extend bounded capture hooks only for a real investigation; do not build an equivalent capture viewer in Editor. |
| [Nsight Graphics](https://developer.nvidia.com/nsight-graphics) and [Nsight Systems](https://developer.nvidia.com/nsight-systems), NVIDIA | Shader/queue and whole-system studies complement Sparkle timings and expose CPU/GPU bottlenecks. | **External.** Match compiler/debug artifacts and hardware; measure optimized runs separately from captures. Nsight Compute is for a selected CUDA workload, rather than a substitute for HLSL shader profiling. |
| [Radeon Developer Tool Suite](https://gpuopen.com/rgp/), AMD | RGP, RGA, RMV, RRA and GPU Detective cover Radeon profiling, shader analysis, memory, AS traversal and crash investigation. | **External.** Query each tool's hardware/API support. Add bounded marker/source-correlation integration where necessary; source or offline ISA analysis alone does not establish Radeon runtime results. |
| [Nsight Perf SDK](https://developer.nvidia.com/nsight-perf-sdk), NVIDIA; [GPUPerfAPI](https://github.com/GPUOpen-Tools/gpu_performance_api), AMD | Programmatic counters can support a reproducible experiment that external interactive captures cannot adequately automate. | **Capability, development only**, after an identified consumer. Check counter availability, permissions, replay/collection overhead and hardware; avoid a permanent in-engine counter UI or always-on collection. |
| [PresentMon](https://github.com/GameTechDev/PresentMon), Intel | External presentation measurements can check frame pacing and distinguish GPU intervals from presented behavior. | **External.** Keep run configuration, capture overhead and dropped/invalid samples explicit; presentation events alone do not measure complete input-to-photon latency. |
| [Windows Performance Toolkit](https://learn.microsoft.com/en-us/windows-hardware/test/wpt/), Microsoft | WPR/WPA can consume ETW and explain CPU scheduling, blocking I/O and process behavior around existing task instrumentation. | **External, reuse** before introducing another tracing runtime. Preserve a reproducible profile and the raw trace. |
| [Perfetto](https://perfetto.dev/), Google | Local trace viewing and SQL analysis can replace custom timeline viewers and analysis plumbing. | **External first.** Its SDK/UI/processor run on Windows; system-wide collection support differs by OS. Define how Sparkle/ETW data reaches the analysis and avoid duplicate event collection. |
| [Google Benchmark](https://github.com/google/benchmark), Google | Focused CPU kernel/cook/parser experiments gain timing repetitions and machine-readable reporting without another local benchmark harness. | **External/local probe.** Prevent dead-code/setup effects and verify the full-frame/load implication separately. Follow the [temporary-validation rules](Engineering.md#verification). |

Tracy remains a conditional alternative to local timing collection, not an additional mandatory profiler. Existing Aftermath/DRED/native-validation owners remain responsible for incident capture and resource/shader correlation.

</details>

<details>
<summary>Independent references, image analysis and neural development</summary>

| Project and provenance | Benefit to Sparkle | Use and adoption condition |
| --- | --- | --- |
| [Embree](https://www.embree.org/), Intel | CPU ray queries provide a different intersection/geometry control for bounded scenes, reducing the need for a second local traversal implementation. | **External or local probe.** Match transforms, culling, precision and primitive identity. Keep it out of the shipped RHI unless a separate justified CPU consumer appears. |
| [PBRT](https://pbrt.org/) or [Mitsuba](https://www.mitsuba-renderer.org/), established rendering/reference projects | Independent transport/material comparisons strengthen the Reference renderer and can generate selected numerical/image controls. | **External.** Match material, light, camera, units and image domain; account for conversion/shared-input error. Preserve Sparkle's own checked reference and its stated transport limits. |
| [Open Image Denoise](https://www.openimagedenoise.org/), Intel | A separate learned denoising baseline or preview can help evaluate the owned model with existing AOVs. | **External first.** Denoised output cannot become the unbiased transport oracle or training ground truth by assertion. It adds a comparison, not personally owned training/kernel evidence. |
| [FLIP](https://github.com/NVlabs/flip), NVIDIA Research | Established image-error maps/metrics can replace bespoke perceptual comparison code. | **External.** Fix HDR/LDR, exposure and viewing assumptions; retain raw numerical bias, uncertainty and temporal checks. |
| [NumPy](https://numpy.org/doc/stable/) and [SciPy](https://docs.scipy.org/doc/scipy/), scientific Python projects | Array, statistics, integration, optimization and signal-processing routines support PDF/BSDF, filter, estimator and timing studies. | **External, high utility.** Replace ad hoc numerical/analysis machinery as needed. Pin the environment and choose methods that respect scene/run independence and correlated frames. |
| [Matplotlib](https://matplotlib.org/stable/), scientific Python project | Reproducible plots of convergence, quality/cost, memory and timing can replace custom chart generation and strengthen communication. | **External.** Export from identified raw data with units, uncertainty and comparable axes. No runtime plotting subsystem is needed. |
| [scikit-image](https://scikit-image.org/docs/stable/api/skimage.metrics.html), scientific Python project | Standard image metrics such as SSIM/PSNR provide checked alternatives to local implementations. | **External, conditional** on the study. Specify range, color domain, channel handling and crop; do not use a single score to establish correctness. |
| [Eigen](https://libeigen.gitlab.io/eigen/docs-nightly/), established C++ linear-algebra project | CPU-side numerical prototypes or reference operators can use tested dense/sparse routines rather than a new local solver. | **External/tool-only, defer** until a C++ numerical consumer needs it. Retain DirectXMath for the existing runtime conventions; NumPy/SciPy may already cover the study. Select a released version when adopting. |
| [PyTorch](https://pytorch.org/), PyTorch Foundation | Training, FP32 operator/model controls and data experiments support the owned neural rendering case. | **External training/reference environment.** Record data splits, seeds, versions, artifacts and numerical uncertainty. Keep the fixed-model shader execution path independently checked. |
| [ONNX](https://onnx.ai/onnx/), ONNX project; [Safetensors](https://huggingface.co/docs/safetensors/index), official project documentation | Graph/shape checks and tensor-only artifacts can replace ad hoc offline interchange between training, checking and cooking. | **External, optional.** Select the needed format; neither implies a runtime inference dependency. Cook the selected model's bounded weights/layout into the existing asset path. |
| [SlangPy](https://github.com/shader-slang/slangpy), Slang project | Python-to-Slang GPU calls and optional PyTorch interop can accelerate kernel/reference experiments and remove throwaway C++ harness code. | **External study.** Account for its own graphics/compiler dependencies and pin them separately. A successful SlangPy run does not qualify Sparkle's cooked shader/native execution path. |
| [ONNX Runtime](https://onnxruntime.ai/docs/), Microsoft-led project; [TensorRT](https://developer.nvidia.com/tensorrt), NVIDIA | Optional inference baselines help isolate export/operator/layout or deployment-cost differences. | **External comparison only** under current scope. General inference runtimes in Game/Editor require a scope decision; they do not replace the selected owned model-to-shader work. |
| [RTXPT](https://github.com/NVIDIA-RTX/RTXPT), NVIDIA; [Capsaicin](https://github.com/GPUOpen-LibrariesAndSDKs/Capsaicin), AMD | Vendor rendering samples provide integration and optimization controls for research productization. | **External study.** Match feature/reference assumptions, credit upstream and isolate useful findings. Avoid importing another scene, task, shader or application framework. |

</details>

<details>
<summary>Build iteration, checking and distribution tooling</summary>

| Project and provenance | Benefit to Sparkle | Use and adoption condition |
| --- | --- | --- |
| [CMake Presets](https://cmake.org/cmake/help/v4.3/manual/cmake-presets.7.html) and [File API](https://cmake.org/cmake/help/v4.3/manual/cmake-file-api.7.html), Kitware | Existing build-system facilities could replace duplicated Launcher configuration/target/source/cache discovery. | **Replacement, conditional.** Preserve toolchain choices, readiness, progress, cancellation and recovery. Consume the actual generated build model; avoid another registry describing it. |
| [Ninja](https://ninja-build.org/), established build project | An optional CMake generator can improve measured configure/build iteration and source reproduction. | **External developer tool.** Qualify the actual MSVC environment/configurations and Launcher routing. Keep a single target graph; no rewritten build system. |
| [sccache](https://github.com/mozilla/sccache), Mozilla | Local compiler caching can shorten repeated dependency/owner builds and leave more time for rendering experiments. | **External developer tool.** Check MSVC generator, PCH/debug flags, cache hits, invalidation and output identity. Keep source sync, shader-cook caching and offline prebuilt use independent. Cloud/distributed services are unnecessary. |
| [LLVM/Clang tooling](https://clang.llvm.org/docs/), LLVM; [MSVC AddressSanitizer](https://learn.microsoft.com/en-us/cpp/sanitizers/asan?view=msvc-170), Microsoft | Format/tidy/static analysis, ASan and supported fuzzing can find lifetime, bounds and parser defects before they corrupt rendering. | **External validation.** Reuse current checks and select tools supported by the actual Windows compiler. Preserve custom paragraph/boolean/decision rules; `clang-tidy --verify-config` does not analyze translation units. |
| [Catch2](https://github.com/catchorg/Catch2), established C++ project; [GoogleTest](https://google.github.io/googletest/), Google; [pytest](https://docs.pytest.org/en/stable/), established Python project | Structured assertions/parameterization can replace repeated scaffolding in an approved validation effort. | **Local probes or approved validation only.** Prefer existing checks; introducing permanent test-only additions follows [Engineering](Engineering.md#verification). Select one C++ framework when a real need exists. |
| [LIEF](https://github.com/lief-project/LIEF), established executable-format project | PE import/delay-import inspection could replace the manual parser in [PackageDependencies.ps1](../CMake/Packaging/PackageDependencies.ps1) and improve runtime-dependency diagnosis. | **Replacement, packaging tool only.** Verify malformed images, delay imports and result equivalence. Account for the native/Python tool distribution; extracted products remain tool-free. |
| [vcpkg](https://vcpkg.io/en/), Microsoft; [Conan](https://conan.io/), JFrog/Conan project | Dependency graph/binary-cache management could reduce acquisition/build maintenance if it replaces the existing owner. | **Whole-workflow alternatives, defer.** Do not add either alongside the CMake/Launcher cache. A change must retire equivalent machinery and preserve pins, explicit sync, notices, offline use and source reproduction. |

</details>

### Boundaries that keep the inventory useful

Keep Qt, ImGui, cgltf, Assimp and MikkTSpace while their supported workflows remain required. A new dependency must have a named current consumer or a selected experiment with a clear stopping point. Standard-library containers/strings/filesystem, DirectXMath and Windows CNG already cover their current responsibilities; specialized hash/allocator/numerical trials above need measured justification.

Keep DXC/Slang, typed reflection/debug identity, accepted-output publication and reload lifetime together under their existing owners. [ShaderMake](https://github.com/NVIDIA-RTX/ShaderMake) is an established batch compiler frontend, but no complete retirement of these Sparkle contracts has been demonstrated. [Donut](https://github.com/NVIDIA-RTX/Donut) would add another scene/runtime stack and has morph-target/KTX2 limits. Use them as references where helpful.

Broad USD/MaterialX authoring, OpenVDB/NanoVDB volume import, mobile-only texture codecs, gameplay/physics/audio/networking, general scripting and frame generation do not serve the selected product. They remain outside this adoption inventory. Brixelizer GI, hair/geometry frameworks and further vendor caches likewise need a deliberate rendering-scope change before they justify new systems. This keeps a broad evaluation list from turning into a collection of unused SDKs.

Package membership, rights/provenance, relocation, output protection and recovery remain Sparkle responsibilities. An archive library, database or binary inspector replaces only its named mechanism. Prefer an external tool or analysis environment whenever it delivers the evidence without growing Game/Editor dependencies.

## Existing dependencies

Keep established library ownership: VMA/D3D12MA for allocation, spdlog for logging, cgltf/Assimp/MikkTSpace for import, stb/tinyexr/KTX-Software for images and containers, Compressonator for block codecs, Qt/ImGui for UI, and DXC/Slang/SPIRV-Reflect/SPIRV-Tools for shaders. Source also contains Streamline, NVAPI, PIX-event and GPU-crash/capture integration; their presence establishes integration code, not runtime qualification. [Build composition](../CMake/Dependencies/FetchDependencies.cmake), [RHI targets](../Engine/RHI/CMakeLists.txt) and [ShaderCompiler targets](../Tools/Shaders/ShaderCompiler/CMakeLists.txt) define actual selection. VMA/D3D12MA and Compressonator already provide AMD library integration; Radeon-specific optimization needs its own hardware results.

Sparkle retains rendering/RHI semantics, scene extraction, material interpretation, resource lifetime, domain validation and publication. These are product responsibilities that the proposed libraries do not replace.
