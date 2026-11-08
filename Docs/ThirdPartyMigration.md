# Third-party migration

[Documentation](README.md) | [Roadmap](Roadmap.md#supporting-system-simplification) | [Developer guide](Engineering.md)

Replace generic infrastructure with maintained libraries so Sparkle can concentrate on rendering. The return should be fewer custom parsers, schedulers and codecs to debug and maintain, with the same supported workflows and output quality.

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
| Command lines | [CLI11](https://github.com/CLIUtils/CLI11) replaces repeated option parsing, help generation and common validation in Launcher, AssetCooker and ShaderCompiler. | First integration; preserve the current command interface. |
| JSON | [nlohmann/json](https://github.com/nlohmann/json) replaces [Core JSON](../Engine/Core/Private/Json) tokenization/writing and callers' repeated property scans. | Next bounded migration. Parse each document once; keep field/domain conversion. Other text formats need an explicit conversion decision. |
| Existing dependencies | DXC/Slang can take over include preprocessing; TinyEXR can use the already selected zlib backend. See [reuse opportunities](#reuse-before-adding-a-library). | Investigate alongside parser work; adds no new library. Removal is still conditional on equivalence. |
| Task execution | [oneTBB](https://github.com/uxlfoundation/oneTBB) could replace [Tasks](../Engine/Tasks/Private) worker queues, graph execution and parallel partitioning across frame, loading and cook consumers. | Largest horizontal candidate here, with high lifecycle risk. Trial before adoption; avoid rebuilding the same scheduler around it. |
| Cooked metadata | [FlatBuffers](https://github.com/google/flatbuffers) could replace [manifest wire records](../Engine/GameFramework/Public/Assets/Cooked/CookedSceneManifest.h), matching readers/writers and structural byte checks. | Trial a complete record family. Preserve packed GPU payloads and domain checks; schema evolution is unnecessary for disposable output. |
| Texture cooking | Broaden existing [Compressonator](https://github.com/GPUOpen-Tools/compressonator) use before adding [DirectXTex](https://github.com/microsoft/DirectXTex) or [NVTT](https://developer.nvidia.com/gpu-accelerated-texture-compression). Targets are [DDS loading](../Tools/Cooking/TextureCooker/Private/SourceLoading/DdsTextureSourceLoader.cpp), [mips](../Tools/Cooking/TextureCooker/Private/Pipeline/Stages/MipStage.cpp), conversion and block-loop preparation. | Prefer one processing owner. DDS-only is a smaller alternative; no complete cooker replacement is established. |
| Window and input | [SDL3](https://github.com/libsdl-org/SDL) could replace [native window machinery](../Engine/Platform/Private/Window), [Win32 input](../Engine/Platform/Private/Input/Backends) and ImGui's platform backend. | Conditional, after the larger trials. Retain viewport actions/routing; remove the replaced native path. |
| Profiling | [Tracy](https://github.com/wolfpld/tracy) supports CPU and D3D12/Vulkan GPU profiling. Possible overlap is [frame timing collection](../Engine/Renderer/Private/Diagnostics/FrameExecutionDiagnostics.cpp) and task instrumentation. | Defer until an analysis consumer needs it and a retirement scope is clear. Task ETW instrumentation and native captures already exist. Any trial stays in development builds with local capture, preserving required product counters and offline behavior. |

Start with CLI11, then JSON. Investigate task execution and metadata before committing them; choose the later texture/platform work from measured removal and integration cost.

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

## Defer or retain

| Area | Reason |
| --- | --- |
| Launcher build state | [CMake presets](https://cmake.org/cmake/help/v4.3/manual/cmake-presets.7.html) and [File API](https://cmake.org/cmake/help/v4.3/manual/cmake-file-api.7.html) are worth investigating only if custom source scans/cache state disappear. Preserve toolchain choices, readiness, progress, cancellation and recovery. |
| UI | Keep Qt, ImGui and the existing scene-selection, editing, inspection and recovery workflows. No complete replacement was identified. |
| ECS | Defer [EnTT](https://github.com/skypjack/entt): the current typed store is small. Any replacement must preserve ID width/validity, generation exhaustion, iteration freezes and render identity without a parallel store. |
| Animation | Defer [Ozz](https://github.com/guillaumeblanc/ozz-animation): cubic splines, quaternion sampling, morph weights, skeleton relationships and GPU skinning have no demonstrated complete conversion. Retaining a custom evaluator defeats the intended replacement. |
| Import | Keep cgltf, Assimp and MikkTSpace. Replacing FBX import with ufbx would leave PLY support and domain translation to maintain. |
| Process execution | Keep the shared process owner. libuv does not establish process-tree cancellation, readiness and handoff; Launcher-only Qt cannot cover Editor/tool consumers. |
| Style checks | Keep the custom paragraph, boolean and decision rules. A new Clang plugin is not an established replacement. `clang-tidy --verify-config` checks configuration, not translation units. |
| Shaders and rendering frameworks | Keep DXC/Slang and typed reflection/debug/publication contracts. [ShaderMake](https://github.com/NVIDIA-RTX/ShaderMake) does not replace them. [Donut](https://github.com/NVIDIA-RTX/Donut) has scene restrictions, including morph targets and KTX2; a second scene stack is not a useful replacement. Native preprocessing needs source/debug equivalence and removal of the replaced preprocessor without a regex fallback. |
| Packaging | Keep output protection, file membership, provenance, rights/notices, relocation and dependency checks. Archive tooling alone cannot replace these contracts. Binary inspection is a smaller opportunity if consumers remain tool-free. |
| RHI and frame graph | Retain the explicit API/lifetime work selected for the portfolio. [NVRHI](https://github.com/NVIDIA-RTX/NVRHI), [NRI](https://github.com/NVIDIA-RTX/NRI) and AMD [RPS](https://github.com/GPUOpen-LibrariesAndSDKs/RenderPipelineShaders) are alternatives to whole owners, not useful layers underneath equivalent local machinery. Reconsider only with a complete retirement scope and an explicit change to the owned evidence. |
| Mesh processing | [meshoptimizer](https://github.com/zeux/meshoptimizer) is a strong cooking candidate when vertex/index optimization or compression has a measured consumer. The current MeshCooker is only six C++ files/338 lines, including headers; no equivalent optimizer was found to delete. Preserve morph/skin remaps, triangle/material identity and raster/ray agreement. Meshlets/GPU-driven geometry remain outside selected scope. |
| Color and broad asset frameworks | Defer [OpenColorIO](https://opencolorio.readthedocs.io/en/latest/), USD, MaterialX and broad image-processing stacks until a required workflow displaces substantial existing machinery. The selected grading/LUT work does not require a second scene/material/color authority. |
| Measurement outside the product | Use [FLIP](https://github.com/NVlabs/flip) and numerical/statistical tools from analysis scripts with identified inputs/settings. They can enrich image evidence without becoming runtime dependencies. Keep raw bias and temporal checks; a perceptual score alone does not establish correctness. |
| More generic utility libraries | Keep existing DirectXMath, standard-library containers/strings/filesystem and Windows CNG SHA-256. No replacement hotspot was established for another math/hash/container/allocator dependency. Avoid adding an entire utility framework to replace small domain helpers. |

## Existing dependencies

Keep established library ownership: VMA/D3D12MA for allocation, spdlog for logging, cgltf/Assimp/MikkTSpace for import, stb/tinyexr/KTX-Software for images and containers, Compressonator for block codecs, Qt/ImGui for UI, and DXC/Slang/SPIRV-Reflect/SPIRV-Tools for shaders. Source also contains Streamline, NVAPI, PIX-event and GPU-crash/capture integration; their presence establishes integration code, not runtime qualification. [Build composition](../CMake/Dependencies/FetchDependencies.cmake), [RHI targets](../Engine/RHI/CMakeLists.txt) and [ShaderCompiler targets](../Tools/Shaders/ShaderCompiler/CMakeLists.txt) define actual selection. VMA/D3D12MA and Compressonator already provide AMD library integration; Radeon-specific optimization needs its own hardware results.

Sparkle retains rendering/RHI semantics, scene extraction, material interpretation, resource lifetime, domain validation and publication. These are product responsibilities that the proposed libraries do not replace.
