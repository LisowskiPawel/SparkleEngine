# Third-party migration

[Documentation](README.md) | [Roadmap](Roadmap.md#supporting-system-simplification) | [Developer guide](Engineering.md)

Replace generic infrastructure with maintained libraries so Sparkle can concentrate on rendering. The return should be fewer custom parsers, schedulers and codecs to debug and maintain, with the same supported workflows and output quality.

**Status:** research reviewed on 2026-11-08. None of these migrations has been implemented or tested inside Sparkle.

## Candidates and expected benefit

| Area | Candidate and benefit | Decision |
| --- | --- | --- |
| JSON | [nlohmann/json](https://github.com/nlohmann/json) replaces handwritten tokenization, escaping and serialization. Domain code only reads fields and converts values. | Recommended for existing JSON. Other text formats need an explicit conversion decision. |
| Command lines | [CLI11](https://github.com/CLIUtils/CLI11) replaces repeated option parsing, help generation and common validation in Launcher, AssetCooker and ShaderCompiler. | Recommended; preserve the current command interface. |
| Task execution | [oneTBB](https://github.com/uxlfoundation/oneTBB) could replace worker management, dependency execution and parallel partitioning. This is a larger maintenance opportunity than parser cleanup. | Investigate first; lifecycle requirements remain unproved. |
| Cooked metadata | [FlatBuffers](https://github.com/google/flatbuffers) could replace manual wire layouts, codecs and structural checks with schemas, generated accessors and verification. | Trial a complete record family before committing to a production scope. |
| Window and input | [SDL3](https://github.com/libsdl-org/SDL) could replace the Win32 event/input machinery and reuse an upstream ImGui platform backend. | Conditional on preserving existing window and interaction behavior. |
| Texture cooking | [DirectXTex](https://github.com/microsoft/DirectXTex) offers a focused replacement for DDS parsing. A broader provider could remove custom filtering, mip generation and compression preparation. | DDS is a smaller opportunity. No complete cooker replacement is verified. |

## Requirements for adoption

- Preserve supported inputs, GUI/CLI workflows, output quality, cancellation, recovery and offline packaged use. Keep both graphics APIs and existing release limits.
- The library must replace substantial generic machinery. Keep domain rules, value conversion and lifetime orchestration in Sparkle; remove the old implementation in the same change. Avoid a second parser, scheduler, codec or native backend hidden behind an adapter.
- Select a maintained release with suitable Windows/toolchain support, license and distribution terms. Include dependency, generator and runtime files in build and package checks.
- Compare a fixed baseline before adoption: behavior and failure cases, numerical agreement, startup/frame/cook time, memory, shutdown, build cost and binary size. Demonstrate offline prebuilt and matching-source workflows, including affected static/shared configurations.

If preserving capability requires a fork, another substantial local implementation or unacceptable costs, keep the working implementation and omit the dependency. Upstream tests and API research do not establish Sparkle runtime or package correctness.

## What decides the fit

### JSON and command lines

JSON must preserve required-field duplicate rejection, exact unsigned integers and hash strings, malformed/trailing-input handling, nesting limits and document lifetime. [Parser callbacks](https://json.nlohmann.me/features/parsing/parser_callbacks/) provide policy hooks; verify them against the chosen release. Parse once and consume domain values without rebuilding the old helper facade.

Levels, catalogs, cook requests and settings are not all JSON. Broader consolidation must update owned files, readers, writers, scripts and package inputs together, preserving normal editing and any required comments. Keep the existing format when that cannot be done without a second complex parser.

CLI11 must preserve switches/aliases, defaults, repeated values, `--run`/`--dry-run` operations, shader options, Unicode paths, help/errors, exit codes and automation across all three tools. Windows child-process argument quoting remains with process-launch code.

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

## Existing dependencies

Keep established library ownership: VMA/D3D12MA for allocation, spdlog for logging, cgltf/Assimp/MikkTSpace for import, stb/tinyexr/KTX-Software for images and containers, Compressonator for block codecs, Qt/ImGui for UI, and DXC/Slang/SPIRV-Reflect for shaders. [Build composition](../CMake/Dependencies/FetchDependencies.cmake) defines actual selection.

Sparkle retains rendering/RHI semantics, scene extraction, material interpretation, resource lifetime, domain validation and publication. These are product responsibilities that the proposed libraries do not replace.
