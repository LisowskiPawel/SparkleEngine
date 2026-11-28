# References

[Goals](Goals.md) | [Scope decisions](Implementation/PortfolioScopeDecisions.md)

Sources behind the project guides, including dated role and professional studies. Pinned links identify the studied revision; check current prerequisites, licenses and fixes before reuse. Project rules and decisions remain in their owning guides.

## Contents

- [Rendering research](#rendering-research), [scene sources](#scene-sources) and [production studies](#nvidia-production-studies)
- [Integration precedents](#integration-precedents), [tools and APIs](#tools-and-apis), [pipeline automation](#pipeline-automation) and [code style](#code-style)
- [Role and professional sources](#role-sources)
- [Documentation examples](#documentation-examples)

## Rendering research

Methods and comparison implementations for the [rendering scope](Implementation/PortfolioScopeDecisions.md#renderer).

### Path transport and materials

Published path tracers and offline-rendering literature below are study sources or possible external comparison controls. They do not require a Reference renderer inside Sparkle; the selected product scope includes only real-time path-traced lighting.

- [PBRT Monte Carlo integration](https://pbr-book.org/4ed/Monte_Carlo_Integration), [microfacets](https://www.pbr-book.org/4ed/Reflection_Models/Roughness_Using_Microfacet_Theory), [rounding error](https://pbr-book.org/4ed/Shapes/Managing_Rounding_Error) and [volume scattering](https://www.pbr-book.org/4ed/Volume_Scattering): independent mathematical controls.
- [glTF specification](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html): source material, coordinate and unit semantics.
- [Veach's light-transport thesis](https://graphics.stanford.edu/papers/veach_thesis/) and [PBRT's improved path tracer](https://pbr-book.org/4ed/Light_Transport_I_Surface_Reflection/A_Better_Path_Tracer): estimator, MIS and independent reference reasoning.
- [Falcor MinimalPathTracer](https://github.com/NVIDIAGameWorks/Falcor/blob/eb540f6748774680ce0039aaf3ac9279266ec521/Source/RenderPasses/MinimalPathTracer/MinimalPathTracer.rt.slang#L30-L52), [Mitsuba's path integrator](https://github.com/mitsuba-renderer/mitsuba3/blob/e262b493806f518e3151905a271955d1ff3203a9/src/integrators/path.cpp) and [Capsaicin's path kernel](https://github.com/GPUOpen-LibrariesAndSDKs/Capsaicin/blob/914b91596cd119eda85fbc1d3c7ee6ac391b1452/src/core/src/ray_tracing/path_tracing.hlsl): small and cross-renderer comparison controls.
- [GGX visible-normal sampling](https://jcgt.org/published/0007/04/01/), [OpenPBR Surface](https://academysoftwarefoundation.github.io/OpenPBR/) and [RTXPT BxDF](https://github.com/NVIDIA-RTX/RTXPT/blob/303b408ab81790a5051abd7573830b7a4112eb8e/Rtxpt/Shaders/PathTracer/BSDFs/BxDF.hlsli): sampling/material-domain choices and the cost of extending lobes.
- [NVIDIA's self-intersection analysis](https://developer.nvidia.com/blog/solving-self-intersection-artifacts-in-directx-raytracing/) and [FLIP](https://github.com/NVlabs/flip): robust geometry controls and image-error evaluation.

### Reuse, reconstruction and caches

- [ReSTIR DI](https://research.nvidia.com/sites/default/files/pubs/2020-07_Spatiotemporal-reservoir-resampling/ReSTIR.pdf), [ReSTIR GI](https://research.nvidia.com/publication/2021-06_restir-gi-path-resampling-real-time-path-tracing), [GRIS](https://research.nvidia.com/labs/rtr/publication/lin2022generalized/): target/proposal/reuse assumptions.
- [RTXDI integration](https://github.com/NVIDIA-RTX/RTXDI/blob/a6efab966b7c3b272da0461578eb56ac61c7cbff/Doc/Integration.md) and [noise/bias](https://github.com/NVIDIA-RTX/RTXDI/blob/a6efab966b7c3b272da0461578eb56ac61c7cbff/Doc/NoiseAndBias.md): production resampling tradeoffs.
- [NRD](https://github.com/NVIDIA-RTX/NRD/tree/e06852bd545973cb224eb28820b891205cfc1624), [Streamline SR](https://github.com/NVIDIA-RTX/Streamline/blob/625db07d4bca5abc67d781c42af7e7fb0761f1b6/docs/ProgrammingGuideDLSS.md) and [RR](https://github.com/NVIDIA-RTX/Streamline/blob/625db07d4bca5abc67d781c42af7e7fb0761f1b6/docs/ProgrammingGuideDLSS_RR.md): signal/guide/provider contracts.
- [SHaRC](https://github.com/NVIDIA-RTX/SHARC/tree/4e21b585c33c83d723ca9a1e11bbb1090d145793), [NRC](https://github.com/NVIDIA-RTX/NRC/tree/7de30e4eb2a1a27118eb9a6acf5f715d757240c0) and [NTC on-sample decode](https://github.com/NVIDIA-RTX/RTXNTC/blob/0ffc32685134c50d6bd09689112fa756052c1b8c/docs/integration/InferenceOnSample.md): separate cache/model/texture integrations.
- [RTXDI ReSTIR GI guide](https://github.com/NVIDIA-RTX/RTXDI/blob/a6efab966b7c3b272da0461578eb56ac61c7cbff/Doc/RestirGI.md) and [ReSTIR PT guide](https://github.com/NVIDIA-RTX/RTXDI/blob/a6efab966b7c3b272da0461578eb56ac61c7cbff/Doc/RestirPT.md): path representation and reuse.
- [Conditional resampled importance sampling](https://research.nvidia.com/labs/rtr/publication/kettunen2023conditional/) and [SVGF](https://research.nvidia.com/labs/rtr/publication/schied2017spatiotemporal/): estimator assumptions versus reconstruction/history assumptions.
- [SHaRC integration at the studied revision](https://github.com/NVIDIA-RTX/SHARC/blob/4e21b585c33c83d723ca9a1e11bbb1090d145793/docs/Integration.md), [neural radiance caching paper](https://research.nvidia.com/labs/rtr/publication/muller2021nrc/) and [NTC quality settings](https://github.com/NVIDIA-RTX/RTXNTC/blob/0ffc32685134c50d6bd09689112fa756052c1b8c/docs/SettingsAndQuality.md): adaptation/bias and quality-memory-compute tradeoffs.

### Decals, grading and HDR

- [FidelityFX Lens](https://gpuopen.com/manuals/fidelityfx_sdk/techniques/lens/): the bounded lens-effect precedent and its sampling assumptions.
- [Epic decal materials](https://dev.epicgames.com/documentation/en-us/unreal-engine/decal-materials-in-unreal-engine) and [ray-tracing decal research](https://i3dsymposium.org/2021/posters/hansen2021_rendering_decals_and_many_lights_paper.pdf): pre-lighting material composition, overlap and arbitrary-hit coverage.
- [Filament's color-grading contract](https://github.com/google/filament/blob/5a90ed9838a8a7115120fe6887d21b138d21c39d/filament/include/filament/ColorGrading.h) and [OpenColorIO's cube reader](https://github.com/AcademySoftwareFoundation/OpenColorIO/blob/5a808fb57a94c7229640a97835c420c9a1fbd1fe/src/OpenColorIO/fileformats/FileFormatIridasCube.cpp): typed color domains/LUTs and source validation. Check parser safety fixes before reusing code.
- [ITU HDR signal definition](https://www.itu.int/dms_pubrec/itu-r/rec/bt/R-REC-BT.2100-3-202502-I%21%21PDF-E.pdf), [Windows Advanced Color](https://learn.microsoft.com/en-us/windows/win32/direct3darticles/high-dynamic-range) and [Vulkan HDR metadata](https://docs.vulkan.org/refpages/latest/refpages/source/VK_EXT_hdr_metadata.html): distinguish signal semantics, compositor eligibility and metadata.
- [Unity's fixed chromatic effect](https://github.com/Unity-Technologies/PostProcessing/blob/32c3155207d3fac4138b24af5c7e0e43805c6f2b/PostProcessing/Runtime/Effects/ChromaticAberration.cs) and FidelityFX provide examples of channel displacement and filtering.

### Volumetrics

- [Frostbite's unified volumetric lighting](https://www.advances.realtimerendering.com/s2015/index.html) and [residual ratio tracking](https://www.jannovak.info/publications/RRTracking/index.html): production froxel reasoning and independent transmittance controls.

### Neural rendering

- [PyTorch Conv2d](https://docs.pytorch.org/docs/main/generated/torch.nn.Conv2d.html): cross-correlation operator semantics.
- [Learned Monte Carlo reconstruction](https://research.nvidia.com/publication/2017-07_interactive-reconstruction-monte-carlo-image-sequences-using-recurrent), [PyTorch reproducibility](https://docs.pytorch.org/docs/2.14/notes/randomness.html) and [numerical accuracy](https://docs.pytorch.org/docs/2.14/notes/numerical_accuracy.html): separate signal/model choice, training repeatability and GPU equivalence. Pin the environment used for an experiment.
- [RTX Neural Shading at the studied revision](https://raw.githubusercontent.com/NVIDIA-RTX/RTXNS/f309ae68677a67060cc6ded2d0af925a3931435f/README.md): training-to-shader examples with specific compiler/driver/API prerequisites. A source study does not establish installed support.
- [Shader Model 6.9 specification](https://microsoft.github.io/DirectX-Specs/d3d/HLSL_ShaderModel6_9.html) and [release announcement](https://devblogs.microsoft.com/directx/shader-model-6-9-retail-and-more/): cooperative vectors as an optional measured optimization after portable FP32 conformance. Verify actual toolchain and device support before selecting it.

### Background for deferred choices

Further reading outside the current plan: [MegaLights](https://advances.realtimerendering.com/s2025/content/MegaLights_Stochastic_Direct_Lighting_2025.pdf), [enhanced ReSTIR PT](https://research.nvidia.com/labs/rtr/publication/lin2026restirptenhanced/), [ReSTIR path guiding](https://research.nvidia.com/labs/rtr/publication/zeng2025restirpg/), [MCMC decorrelation](https://research.nvidia.com/labs/prl/sawhney2024decorrelating/restirmcmc2024.pdf), [volumetric ReSTIR](https://graphics.cs.utah.edu/research/projects/volumetric-restir/volumetric_restir.pdf), [atmosphere models](https://ebruneton.github.io/precomputed_atmospheric_scattering/) and [Nubis clouds](https://www.guerrilla-games.com/read/nubis-evolved).

## Scene sources

- [ORCA Bistro](https://developer.nvidia.com/orca/amazon-lumberyard-bistro): the exterior and Interior Wine source family; authored light and material information.
- [McGuire Computer Graphics Archive](https://casual-effects.com/g3d/data10/index.html): CornellBox and LPSHead publisher inputs and source notes.
- [Intel GPU Research Samples](https://www.intel.com/content/www/us/en/developer/topic-technology/graphics-research/samples.html): Modern Sponza and separately scoped add-ons.

Record the source, notices and content identity. Check redistribution rights separately from local acquisition.

## NVIDIA production studies

Studies that connect the professional goals to concrete engineering work. Published results describe their tested systems.

- [ReSTIR production experience](https://research.nvidia.com/publication/2021-07_rearchitecting-spatiotemporal-resampling-production), [course notes](https://intro-to-restir.cwyman.org/presentations/2023ReSTIR_Course_Notes.pdf) and [Cyberpunk integration](https://intro-to-restir.cwyman.org/presentations/2023ReSTIR_Course_Cyberpunk_2077_Integration.pdf): connect estimator assumptions, content constraints, bias and sampling budgets.
- [Shader source correlation](https://developer.nvidia.com/blog/harness-powerful-shader-insights-using-shader-debug-info-with-nvidia-nsight-graphics/) and [Indiana Jones live-state analysis](https://developer.nvidia.com/blog/path-tracing-optimization-in-indiana-jones-shader-execution-reordering-and-live-state-reductions/): explain the executed shader before attributing stalls, then measure liveness/code-size/memory tradeoffs with whole-frame controls.
- [NVRHI state tracking](https://github.com/NVIDIA-RTX/NVRHI/blob/6b96fb03e07539f08327aea76c56d55f1de9d906/src/common/state-tracking.cpp) and [Vulkan memory-budget sample](https://github.com/nvpro-samples/vk_mini_samples/blob/1ca74a9c2b459848fded141dc14392dcc9e88029/samples/memory_budget/memory_budget.cpp): resource state/lifetime and the distinction between allocation, device budget and physical memory.
- [Tiny CUDA NN's fused network](https://github.com/NVlabs/tiny-cuda-nn/blob/3daa6e5b8b8e0fa17971a1a20acfbb3754494b38/src/fully_fused_mlp.cu) and [Instant NGP experiment script](https://github.com/NVlabs/instant-ngp/blob/abe236ee00cf90cfca6e36e65c00435d5b21f50a/scripts/run.py): learn from layout/traffic and training-to-artifact experiments. CUDA, NeRF and a general tensor runtime remain outside our selected work.
- [Slang lab exercises](https://developer.download.nvidia.com/ProGraphics/nvpro-samples/SlangLab/Slides.pdf): a model for short explanations with an observable task and real feedback, rather than a slide deck alone.

## Integration precedents

Study ownership, composition and reuse through these primary examples. Apply the lessons to local requirements; a reference is neither a feature mandate nor proof of a local result.

| Example | Relevant lesson |
| --- | --- |
| NVIDIA [Donut dependency composition](https://github.com/NVIDIA-RTX/Donut/blob/e50042f4bbd762971cc90431261e967404855b81/thirdparty/CMakeLists.txt), [ImGui target](https://github.com/NVIDIA-RTX/Donut/blob/e50042f4bbd762971cc90431261e967404855b81/thirdparty/imgui.cmake) and [NVRHI build](https://github.com/NVIDIA-RTX/NVRHI/blob/6b96fb03e07539f08327aea76c56d55f1de9d906/CMakeLists.txt) | Explicit external composition and per-library targets; keep dependency acquisition, compilation and native integration distinguishable. |
| NVIDIA [Falcor external composition](https://github.com/NVIDIAGameWorks/Falcor/blob/759aad033ff610fb0d82c74f7e0a508d0096d5f2/external/CMakeLists.txt) and [RTXPT build](https://github.com/NVIDIA-RTX/RTXPT/blob/303b408ab81790a5051abd7573830b7a4112eb8e/CMakeLists.txt) | Source, prebuilt package and SDK boundaries; architecture-specific inputs and runtime payloads. |
| AMD [Capsaicin dependency setup](https://github.com/GPUOpen-LibrariesAndSDKs/Capsaicin/blob/914b91596cd119eda85fbc1d3c7ee6ac391b1452/CMakeLists.txt) and [Cauldron library composition](https://github.com/GPUOpen-LibrariesAndSDKs/Cauldron/blob/b92d559bd083f44df9f8f42a6ad149c1584ae94c/CMakeLists.txt) | Distinguishable native and external targets; evaluate global options and acquisition policy separately. |
| [Epic third-party integration](https://dev.epicgames.com/documentation/en-us/unreal-engine/integrating-third-party-libraries-into-unreal-engine), CMake 3.30 [dependencies](https://cmake.org/cmake/help/v3.30/guide/using-dependencies/index.html), [FetchContent](https://cmake.org/cmake/help/v3.30/module/FetchContent.html) and [subdirectory policy](https://cmake.org/cmake/help/v3.30/command/add_subdirectory.html) | External-library boundaries, imported/source-built dependencies and runtime staging. |
| CMake 3.30 [conditional/transitive link semantics](https://cmake.org/cmake/help/v3.30/manual/cmake-generator-expressions.7.html), [custom targets](https://cmake.org/cmake/help/v3.30/command/add_custom_target.html) and [output/build-event rules](https://cmake.org/cmake/help/v3.30/command/add_custom_command.html) | Transitive link semantics and the difference between build outputs, build events and deployment repair. |
| Pinned [ImGui context/allocator guidance](https://github.com/ocornut/imgui/blob/6d910d5487d11ca567b61c7824b0c78c569d62f0/imgui.cpp#L1360) and [Assimp build options](https://github.com/assimp/assimp/blob/c35200e38ea8f058812b83de2ef32c6093b0ece2/CMakeLists.txt) | Linkage, allocator/context state and real per-module consumers determine integration boundaries. |
| [Cycles replaces its scheduler with TBB](https://github.com/blender/blender/commit/e50f1ddc6540680d2aafc1c76f8339d69350f84a); [OpenUSD Work](https://openusd.org/release/api/work_page_front.html) | Scheduler replacement and process concurrency require explicit lifecycle and capacity reasoning. |
| [SDL3/ImGui Vulkan example](https://github.com/ocornut/imgui/blob/master/examples/example_sdl3_vulkan/main.cpp), [native window properties](https://wiki.libsdl.org/SDL3/SDL_GetWindowProperties) and [Vulkan surface API](https://wiki.libsdl.org/SDL3/SDL_Vulkan_CreateSurface) | Native window and surface boundaries can preserve an independently owned renderer. |
| [Falcor graph compilation](https://github.com/NVIDIAGameWorks/Falcor/blob/759aad033ff610fb0d82c74f7e0a508d0096d5f2/Source/Falcor/RenderGraph/RenderGraph.cpp#L424) | Graph changes and compiled execution have an explicit invalidation relationship; caching needs local evidence. |
| [Falcor SDK composition](https://github.com/NVIDIAGameWorks/Falcor#nvidia-rtx-sdks); [NRD native dispatch API](https://github.com/NVIDIA-RTX/NRD#api) | Production algorithms can integrate through application-owned graphics APIs and resource lifetime. |
| [RTXDI integration](https://github.com/NVIDIA-RTX/RTXDI/blob/main/Doc/Integration.md) and [integrable runtime](https://github.com/NVIDIA-RTX/RTXDI-Library) | Resampling libraries depend on application scene, material and tracing contracts. |
| [Snowdrop FSR production study](https://gpuopen.com/download/GDC2024_High_Performance_Rendering_in_Snowdrop_Using_AMD_FidelityFX_Super_Resolution_3.pdf); [AMD FSR SDK requirements](https://gpuopen.com/amd-fsr-sdk/) | Temporal inputs, pass/queue placement and whole-frame integration obligations. |
| [Intel XeSS-SR guide](https://github.com/intel/xess/blob/main/doc/xess_sr_developer_guide_english.md) and [official Unreal plugin](https://github.com/GameTechDev/XeSSUnrealPlugin) | Resource/signal contracts and portable execution versus vendor acceleration. |
| [ShaderMake frontend](https://github.com/NVIDIA-RTX/ShaderMake/blob/4f1ffd56dac27fc3d620ad7efeee42b4dd71fa72/ShaderMake/ShaderMake.cpp), [Slang JSON reflection emitter](https://github.com/shader-slang/slang/blob/d88cc8fff35baf96cf30d242b2572564cf8043b6/source/slang/slang-reflection-json.cpp) | Delegate compiler mechanics while preserving declared outputs, reflection, ABI and debug identity. |
| [DXC include-handler example](https://github.com/microsoft/DirectXShaderCompiler/wiki/Using-dxc.exe-and-dxcompiler.dll), [Slang compilation/filesystem API](https://docs.shader-slang.org/en/latest/external/slang/docs/user-guide/08-compiling.html), [volk device tables](https://github.com/zeux/volk#optimizing-device-calls) | Reuse compiler/include and device-dispatch facilities without losing source identity or native ownership. |
| [Qt 6.11 item views](https://doc.qt.io/qt-6.11/qlistview.html), [vswhere](https://github.com/microsoft/vswhere), [D3D12MA virtual allocation](https://gpuopen-librariesandsdks.github.io/D3D12MemoryAllocator/html/virtual_allocator.html) | Existing library mechanisms for item views, tool discovery and bounded allocation. |

Check the actual licenses: [oneTBB](https://github.com/uxlfoundation/oneTBB/blob/master/LICENSE.txt), [SDL](https://github.com/libsdl-org/SDL/blob/main/LICENSE.txt), [FlatBuffers](https://github.com/google/flatbuffers/blob/master/LICENSE), [volk](https://github.com/zeux/volk/blob/master/LICENSE.md), [RTXDI runtime](https://github.com/NVIDIA-RTX/RTXDI-Library/blob/main/LICENSE.txt), [NRD](https://github.com/NVIDIA-RTX/NRD/blob/master/LICENSE.txt) and each selected AMD/Intel SDK artifact. Public source, sample licensing and runtime redistribution are separate facts.

### Windows build and Rider selection

Use the [CMake generator documentation](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html), [Ninja scheduling manual](https://ninja-build.org/manual.html), [Clang Windows compatibility guide](https://clang.llvm.org/docs/MSVCCompatibility.html) and [LLD Windows support](https://lld.llvm.org/windows_support.html) to understand build and ABI constraints. Tool selection still requires local compilation, incremental-build, debugging and deployment evidence; another project's preset does not establish an employer-wide standard.

## Tools and APIs

- [Nsight shader profiling](https://docs.nvidia.com/nsight-graphics/UserGuide/shader-profiler.html): connect the executed kernel and source identity to measured hypotheses; native-ISA access is not assumed.
- [PIX capture](https://devblogs.microsoft.com/pix/taking-a-capture/) and [programmatic capture](https://devblogs.microsoft.com/pix/programmatic-capture/).
- [RenderDoc application API at v1.46](https://github.com/baldurk/renderdoc/blob/v1.46/docs/in_application_api.rst).
- [Nsight Graphics](https://developer.nvidia.com/nsight-graphics-features) and [Aftermath](https://developer.nvidia.com/nsight-aftermath).
- [Radeon Developer Tool Suite](https://gpuopen.com/rgp/), [Windows Performance Toolkit](https://learn.microsoft.com/en-us/windows-hardware/test/wpt/) and [PresentMon](https://github.com/GameTechDev/PresentMon): hardware-specific GPU analysis, ETW scheduling/I/O traces and presentation measurements. Check each tool's supported hardware/API and collection overhead; presentation events alone do not measure input-to-photon latency.
- [Peak-performance analysis](https://developer.nvidia.com/blog/the-peak-performance-analysis-method-for-optimizing-any-gpu-workload/): isolate a limiting mechanism before optimization. Its historical tool table and queue assumptions are not current compatibility evidence or local acceptance thresholds.
- [DXR specification](https://microsoft.github.io/DirectX-Specs/d3d/Raytracing.html), [Vulkan synchronization](https://docs.vulkan.org/samples/latest/samples/extensions/synchronization_2/README.html) and [PTLAS proposal](https://docs.vulkan.org/features/latest/features/proposals/VK_NV_partitioned_acceleration_structure.html).

## Pipeline automation

For CMake composition, compare [Donut's modules and shaders](https://github.com/NVIDIA-RTX/Donut/blob/52efbe37d7f550ea1c5c7ee02b75404ec7c72c09/CMakeLists.txt), [NVRHI's backends and SDK targets](https://github.com/NVIDIA-RTX/NVRHI/blob/bf0bbd9b666a7297492beb1999a4ddc0dfded143/CMakeLists.txt), and [RTXPT's shader/runtime setup and CPU test registration](https://github.com/NVIDIA-RTX/RTXPT/blob/303b408ab81790a5051abd7573830b7a4112eb8e/CMakeLists.txt).

[NRD's CI](https://github.com/NVIDIA-RTX/NRD/blob/90dea8bd91ec6b58d0f90cb6adf53c7da613b82c/.github/workflows/build.yml) runs deploy, build and SDK-preparation scripts. [RTXPT's scene runner](https://github.com/NVIDIA-RTX/RTXPT/blob/303b408ab81790a5051abd7573830b7a4112eb8e/Testing/RegressionTesting-RUN.bat) and [image tests](https://github.com/NVIDIA-RTX/RTXPT/blob/303b408ab81790a5051abd7573830b7a4112eb8e/Support/tests/run_tests.ps1) render identified scenes and compare results. [Falcor's CI](https://github.com/NVIDIAGameWorks/Falcor/blob/759aad033ff610fb0d82c74f7e0a508d0096d5f2/.github/workflows/build.yml) invokes setup and native build commands directly.

These are examples of concrete pipeline consumers, not a required script inventory. Sparkle's [script ownership rule](Engineering.md#script-ownership) governs local use.

## Code style

Examples behind the project [coding conventions](CodeStyle.md):

- [NVIDIA NRD formatter](https://github.com/NVIDIA-RTX/NRD/blob/fd33827618aea7cb8b17b74a4923b0c1afc2c319/.clang-format), [NRI](https://github.com/NVIDIA-RTX/NRI/blob/56e42c2f50351538609fef0d1a642e679a8c3458/.clang-format), [CCCL](https://github.com/NVIDIA/cccl/blob/4bd9aaa474d608d2b42548ac7dfadd0c34a9d3a1/.clang-format).
- [C++ Core Guidelines whitespace](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rl-space), [Epic C++ standard](https://dev.epicgames.com/documentation/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine), [LLVM formatter options](https://clang.llvm.org/docs/ClangFormatStyleOptions.html).
- [NVRHI build separation](https://github.com/NVIDIA-RTX/NVRHI/blob/bf0bbd9b666a7297492beb1999a4ddc0dfded143/CMakeLists.txt) and [NVIDIA engineering standards](https://docs.omniverse.nvidia.com/kit/docs/rtx_remix/latest/docs_dev/code-quality/engineering-standards.html): ownership and root-cause repair.
- [Epic module organization](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-modules): public contracts, private implementation and explicit module dependencies.
- [Epic Render Dependency Graph](https://dev.epicgames.com/documentation/en-us/unreal-engine/render-dependency-graph-in-unreal-engine): typed pass parameters, declared resource dependencies and parallel graph execution.

## Role sources

The [career and portfolio study](Studies/CareerAndPortfolio.md) preserves dated role descriptions, employer research, the professional baseline and public engineer examples. Use it to assess capability and communication, keeping required/preferred qualifications and personal/team contributions distinct. Recheck employment and vacancy claims before relying on them.

## Documentation examples

- [RTXPT README](https://github.com/NVIDIA-RTX/RTXPT/blob/main/README.md) and [Design](https://github.com/NVIDIA-RTX/RTXPT/blob/main/Docs/Design.md): overview/requirements/build/known issues, followed by one pipeline guide.
- [NVRHI README](https://github.com/NVIDIA-RTX/NVRHI/blob/main/README.md) and [Programming Guide](https://github.com/NVIDIA-RTX/NVRHI/blob/main/doc/ProgrammingGuide.md): a small entry point and coherent resource/command/lifetime reference.
- [SHaRC overview](https://github.com/NVIDIA-RTX/SHARC/blob/main/README.md) and [Integration](https://github.com/NVIDIA-RTX/SHARC/blob/main/docs/Integration.md): minimal introduction, actionable integration, focused technical detail.
- [RTXDI README](https://github.com/NVIDIA-RTX/RTXDI/blob/main/README.md): direct build/run instructions, explicit known issues and a linked integration guide.
- [NRD README](https://github.com/NVIDIA-RTX/NRD/blob/master/README.md): substantial technical knowledge can stay in one navigable guide.
