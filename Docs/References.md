# References

[Documentation](README.md) | [Rendering](Rendering.md)

Papers, SDK guides and examples used in the project. Pinned links identify the studied revision; check current prerequisites, licenses and fixes before using the code.

## Contents

- [Rendering research](#rendering-research), [scene sources](#scene-sources) and [production studies](#nvidia-production-studies)
- [Tools and APIs](#tools-and-apis) and [code style](#code-style)
- [Role sources](#role-sources), [employer study](#employer-role-study), [professional baseline](#professional-baseline) and [public work](#persona-and-portfolio)
- [Documentation examples](#documentation-examples)

## Rendering research

Methods and comparison implementations for the [rendering features](Rendering.md).

### Path transport and materials

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
- [McGuire Computer Graphics Archive](https://casual-effects.com/g3d/data10/index.html): San Miguel's corrected high/low pair and its source notes.
- [Intel GPU Research Samples](https://www.intel.com/content/www/us/en/developer/topic-technology/graphics-research/samples.html): Modern Sponza and separately scoped add-ons.

Record the source, notices and content identity. Check redistribution rights separately from local acquisition.

## NVIDIA production studies

Studies that connect the professional goals to concrete engineering work. Published results describe their tested systems.

- [ReSTIR production experience](https://research.nvidia.com/publication/2021-07_rearchitecting-spatiotemporal-resampling-production), [course notes](https://intro-to-restir.cwyman.org/presentations/2023ReSTIR_Course_Notes.pdf) and [Cyberpunk integration](https://intro-to-restir.cwyman.org/presentations/2023ReSTIR_Course_Cyberpunk_2077_Integration.pdf): connect estimator assumptions, content constraints, bias and sampling budgets.
- [Shader source correlation](https://developer.nvidia.com/blog/harness-powerful-shader-insights-using-shader-debug-info-with-nvidia-nsight-graphics/) and [Indiana Jones live-state analysis](https://developer.nvidia.com/blog/path-tracing-optimization-in-indiana-jones-shader-execution-reordering-and-live-state-reductions/): explain the executed shader before attributing stalls, then measure liveness/code-size/memory tradeoffs with whole-frame controls.
- [NVRHI state tracking](https://github.com/NVIDIA-RTX/NVRHI/blob/6b96fb03e07539f08327aea76c56d55f1de9d906/src/common/state-tracking.cpp) and [Vulkan memory-budget sample](https://github.com/nvpro-samples/vk_mini_samples/blob/1ca74a9c2b459848fded141dc14392dcc9e88029/samples/memory_budget/memory_budget.cpp): resource state/lifetime and the distinction between allocation, device budget and physical memory.
- [Tiny CUDA NN's fused network](https://github.com/NVlabs/tiny-cuda-nn/blob/3daa6e5b8b8e0fa17971a1a20acfbb3754494b38/src/fully_fused_mlp.cu) and [Instant NGP experiment script](https://github.com/NVlabs/instant-ngp/blob/abe236ee00cf90cfca6e36e65c00435d5b21f50a/scripts/run.py): learn from layout/traffic and training-to-artifact experiments. CUDA, NeRF and a general tensor runtime remain outside our selected work.
- [Slang lab exercises](https://developer.download.nvidia.com/ProGraphics/nvpro-samples/SlangLab/Slides.pdf): a model for short explanations with an observable task and real feedback, rather than a slide deck alone.

## Tools and APIs

- [Nsight shader profiling](https://docs.nvidia.com/nsight-graphics/UserGuide/shader-profiler.html): connect the executed kernel and source identity to measured hypotheses; native-ISA access is not assumed.
- [PIX capture](https://devblogs.microsoft.com/pix/taking-a-capture/) and [programmatic capture](https://devblogs.microsoft.com/pix/programmatic-capture/).
- [RenderDoc application API at v1.46](https://github.com/baldurk/renderdoc/blob/v1.46/docs/in_application_api.rst).
- [Nsight Graphics](https://developer.nvidia.com/nsight-graphics-features) and [Aftermath](https://developer.nvidia.com/nsight-aftermath).
- [Peak-performance analysis](https://developer.nvidia.com/blog/the-peak-performance-analysis-method-for-optimizing-any-gpu-workload/): isolate a limiting mechanism before optimization. Its historical tool table and queue assumptions are not current compatibility evidence or local acceptance thresholds.
- [DXR specification](https://microsoft.github.io/DirectX-Specs/d3d/Raytracing.html), [Vulkan synchronization](https://docs.vulkan.org/samples/latest/samples/extensions/synchronization_2/README.html) and [PTLAS proposal](https://docs.vulkan.org/features/latest/features/proposals/VK_NV_partitioned_acceleration_structure.html).

## Code style

Examples behind the project [coding conventions](CodeStyle.md):

- [NVIDIA NRD formatter](https://github.com/NVIDIA-RTX/NRD/blob/fd33827618aea7cb8b17b74a4923b0c1afc2c319/.clang-format), [NRI](https://github.com/NVIDIA-RTX/NRI/blob/56e42c2f50351538609fef0d1a642e679a8c3458/.clang-format), [CCCL](https://github.com/NVIDIA/cccl/blob/4bd9aaa474d608d2b42548ac7dfadd0c34a9d3a1/.clang-format).
- [C++ Core Guidelines whitespace](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rl-space), [Epic C++ standard](https://dev.epicgames.com/documentation/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine), [LLVM formatter options](https://clang.llvm.org/docs/ClangFormatStyleOptions.html).
- [NVRHI build separation](https://github.com/NVIDIA-RTX/NVRHI/blob/bf0bbd9b666a7297492beb1999a4ddc0dfded143/CMakeLists.txt) and [NVIDIA engineering standards](https://docs.omniverse.nvidia.com/kit/docs/rtx_remix/latest/docs_dev/code-quality/engineering-standards.html): ownership and root-cause repair.

## Role sources

The target in [Goals](Goals.md#technical-development) comes from these supplied role descriptions and the later employer/public-work study.

These are the **normalized transcriptions archived on 2026-07-26**, with OCR corrections, merged duplicates and employer branding, locations and personal details removed. They preserve the archived wording without the old requirement codes; they are not exact copies of the advertisements. Required, preferred and differentiating qualifications remain separate.

| Supplied description | Main emphasis |
| --- | --- |
| [Principal advanced rendering and AI adoption](#principal-advanced-rendering-and-ai-adoption) | Path-traced/neural rendering, system performance and technology transfer |
| [Graphics workload analysis tools](#graphics-workload-analysis-tools) | D3D12/Vulkan analysis, debugging and useful developer tools |
| [Graphics software development](#graphics-software-development) | Modern C++, shaders, ray tracing and integration |
| [Neural rendering research inference](#neural-rendering-research-inference) | Models, operators, GPU kernels and low-level performance |
| [Advanced rendering research productization](#advanced-rendering-research-productization) | Research prototypes into maintained game technology |
| [Short public hiring summary](#short-public-hiring-summary) | The four skills highlighted in the public message |

### Principal advanced rendering and AI adoption

Inline screenshot supplied with the original review.

<details>
<summary>Focus, responsibilities, required background and differentiators</summary>

#### Role focus

- Focus on advanced rendering, including path tracing and neural graphics.
- Drive broader adoption of AI technology in games.

#### Responsibilities

- Collaborate with highly innovative AAA developers around the world to optimize GPU and system performance and deliver fast, fluid gameplay with path-traced neural rendering and complex AI behaviors.
- Develop, profile, optimize, and tune neural-rendering algorithms and AI models to make effective use of available compute resources.
- Collaborate with architecture and driver teams to ensure the best experience on current hardware and help determine trends and features for future architectures.
- Explore current GPU technology, develop new GPU techniques, build technical demonstrations, write whitepapers, and present work at conferences.

#### Required background

- Master’s degree in computer science, computer engineering, or a related computationally focused science, or equivalent experience.
- Fifteen or more years of relevant work experience or research.
- Proficiency in C++ with strong software development, optimization, debugging, and software-engineering foundations.
- Very strong mathematics, including linear algebra and calculus, for problem solving and performance modeling.
- Excellent real-time graphics and GPU knowledge, including real-time rendering algorithms, ray/path tracing, shaders, shading languages, and APIs such as DirectX or Vulkan.
- Hands-on low-level performance optimization experience.
- In-depth CPU and GPU architecture fundamentals.
- Solid AI fundamentals, including the ability to use AI tools for programming productivity and to design new solutions.
- Good communication, organization, and prioritization.
- Willingness to travel for on-site developer visits and conferences.

#### Differentiators

- Professional experience with machine-learning algorithms and applications.
- Experience optimizing inference and training workloads.
- Solid understanding of neural-graphics techniques.
- Windows or Linux driver-development experience.

</details>

### Graphics workload analysis tools

Supplied image (1).webp.

<details>
<summary>Responsibilities and preferred experience</summary>

#### Responsibilities

- Design and develop graphics tools that analyze and process D3D12 and Vulkan workloads.
- Develop techniques for analyzing how different game engines use graphics APIs.
- Investigate and resolve complex graphics issues using deep expertise in both APIs.
- Collaborate with other engineers to integrate new features and enhancements into existing systems.
- Maintain high-quality documentation and coding standards.
- Collaborate closely with multiple teams to deliver solutions that work for them.
- Contribute to future architecture for a highly scalable, durable, and innovative system.

#### Preferred experience

- Proficiency in C++ and Python.
- In-depth D3D12 and/or Vulkan knowledge and experience.
- Experience with graphics debuggers such as PIX and RenderDoc.
- Solid understanding of rasterization, ray-tracing pipelines, and GPU architecture.
- Game-development or real-time-rendering experience.
- Familiarity with CPU and GPU performance-profiling tools and techniques.
- Excellent communication and collaborative teamwork.
- Passion for graphics technology and AI with continuous learning.
- Experience with machine-learning frameworks such as TensorFlow, PyTorch, or similar.

</details>

### Graphics software development

Supplied image (2).webp and image (5).webp, merged once.

<details>
<summary>Role, responsibilities, essentials and academic background</summary>

#### Persona

- A strategic graphical engineer passionate about creating new GPU-rendered experiences.
- Work with current hardware and software technology to push rendering boundaries.
- Combine graphics and software engineering with innovation and research.
- Communicate effectively and work well with different teams.

#### Responsibilities

- Develop and maintain applications using DirectX 12, Vulkan, and other modern graphics APIs.
- Optimize complex algorithms and solve difficult problems to improve software efficiency and performance.
- Design and implement advanced shaders for high-quality visual effects.
- Collaborate across functions to deliver integrated solutions and execute projects smoothly.
- Innovate and implement ray-tracing technologies, especially global illumination or path-tracing applications.
- Develop tools and plugins that support developers and streamline graphics programming.
- Explore and apply machine-learning techniques to optimize graphics processing and performance.

#### Essentials

- Extensive C++ and graphics-programming experience with high software-development proficiency.
- Expertise writing and optimizing complex shaders for graphics applications.
- Successful cross-functional collaboration with strong interpersonal and communication skills.

#### Useful differentiators

- Hands-on ray tracing, especially global illumination or path tracing.
- Experience creating developer tools or plugins that improve functionality and usability.
- Understanding of machine-learning applications in graphics optimization and use of AI techniques to improve graphics processing.

#### Academic foundation

- Bachelor’s or master’s degree in computer science or a related field, with grounding in graphics, mathematics, machine learning, or computer engineering. Equivalent relevant experience is also represented elsewhere in the source set.

</details>

### Neural rendering research inference

Supplied image (3).webp and image (4).webp.

<details>
<summary>Role, responsibilities and preferred experience</summary>

#### Persona and role

- Translate neural-network models and algorithms into efficient inference solutions.
- Understand graphics and machine learning, neural-network operators, their mathematical foundations, and computational requirements.
- Use C++, HIP, CUDA, HLSL, and related GPU technologies.
- Communicate and work across teams to create optimized inference implementations for neural rendering and generative-AI applications.
- Implement and optimize high-performance GPU kernels for machine-learning operators.
- Work with researchers developing real-time neural graphics, inverse rendering, and other ML-accelerated opportunities.

#### Responsibilities

- Work with ML researchers and engineers to translate models and algorithms written in PyTorch/ONNX into efficient GPU shaders using HIP, CUDA, HLSL, or equivalent languages.
- Design, implement, and optimize high-performance GPU kernels for ML operators.
- Work across research, hardware, driver, and compiler teams to analyze performance issues and improve rendering speed and ML-workload efficiency.
- Stay current with GPU hardware, rendering techniques, graphics APIs, and GPU-accelerated ML.
- Contribute tools and methodologies for integrating optimized shaders into game engines.
- Document and share best practices for graphics and compute/ML GPU programming.
- Participate in code reviews and provide constructive feedback.

#### Preferred experience

- Strong object-oriented programming; C or C++ preferred.
- Proven ML GPU-kernel development and optimization using HIP, OpenCL, CUDA, HLSL, or equivalent.
- Ability to program or reason in low-level languages such as x86 assembly, SIMD intrinsics, GPU ISA, or comparable assembly.
- Strong GPU-architecture knowledge: compute cores, cache hierarchy, memory model, graphics APIs, and shader programming.
- Solid understanding of common neural-network operators, mathematics, and computational requirements.
- Modern concurrent-programming and threading APIs.
- Windows and Linux operating-system development.
- Software-development processes and tools such as debuggers, Git-based source control, and profilers.
- Machine-learning techniques and graphics applications.
- Effective communication, problem solving, and interpersonal skills.

</details>

### Advanced rendering research productization

Supplied image (6).webp and image (8).webp; image (7).webp duplicated image (6).webp.

<details>
<summary>Responsibilities, required experience and differentiators</summary>

#### Persona and operating context

- Work at the intersection of graphics research and productization.
- Create cutting-edge software that equips game developers and publishers to build immersive, high-performance games.
- Bring a strong technical background in GPU/APU programming, C++, and machine learning.
- Collaborate within and across teams and synthesize information from specialist domains.
- Influence technical direction and start new projects from first principles.

#### Responsibilities

- Work closely with research engineers to transform proof-of-concept prototypes into mature, high-quality products.
- Collaborate with external game-development partners to integrate technology into their titles.
- Optimize, extend, package, and document high-level compute-shader and C++ code.

#### Required experience

- Computer-science background and at least five years of full-time experience writing efficient high-level shader code such as HLSL SM6, GLSL, or Slang and modern C++17 or newer.
- Strong low-level machine-learning concepts and design patterns including automatic differentiation, computational graphs, tensor broadcasting, and related foundations.
- Real-time-rendering and graphics-algorithm knowledge.
- Excellent written and verbal English.
- Willingness to travel domestically and internationally when required.

#### Desirable but not essential

- Knowledge of modern GPU and game-console instruction-set architectures.
- Experience with modern ML libraries such as PyTorch or TensorFlow.
- Physically based rendering algorithms: sampling, shading, and light transport.
- Modern graphics APIs such as D3D12 or Vulkan.
- Applied mathematics in a relevant field: linear algebra, differential calculus, stochastic optimization, and statistical analysis.

#### Academic foundation

- PhD, bachelor’s, or master’s degree emphasizing computer science, computer engineering, or applied mathematics with relevant experience preferred.

</details>

### Short public hiring summary

Supplied image.webp.

<details>
<summary>Four highlighted skills</summary>

- D3D12 and Vulkan APIs.
- C++ and Python.
- Real-time rendering, rasterization, and ray tracing.
- Graphics debuggers including PIX and RenderDoc.

</details>

## Employer role study

These paraphrases were recorded on **2026-10-10**, with a principal-role refresh on **2026-10-25**. Sources were official descriptions or indexed official text where full pages were unavailable. Recheck vacancy status and qualifications before an application.

<details>
<summary>Eight NVIDIA role descriptions and the cross-employer findings</summary>

| Role and source | Preserved expectations |
| --- | --- |
| [Principal Developer Technology Engineer](https://nvidia.wd5.myworkdayjobs.com/nvidiaexternalcareersite/job/Principal-Developer-Technology-Engineer_JR2013547) | Turn advanced path-traced and neural rendering into useful game-engine results. Optimize CPU/GPU systems and models; collaborate with game developers, architecture and driver teams; publish demonstrations and technical explanations. Strong mathematics, C++, explicit APIs and low-level performance support the work. The indexed refresh recorded a relevant master's degree or equivalent experience and 15+ years of relevant work/research. |
| [Principal Graphics Developer Tools Engineer](https://nvidia.wd5.myworkdayjobs.com/en-US/NVIDIAExternalCareerSite/job/Principal-Graphics-Developer-Tools-Engineer_JR2019836) | Set direction for ambiguous initiatives, understand developer problems, prototype hands-on and carry useful tools into production. Coordinate across organizations, influence decisions and mentor engineers, backed by graphics API/C++ depth. The original study retrieved an official job-data description; the later indexed refresh recorded 15+ years and a relevant degree or equivalent experience. |
| [Senior Graphics Driver Engineer](https://nvidia.wd5.myworkdayjobs.com/en-US/NVIDIAExternalCareerSite/job/US-CA-Santa-Clara/Senior-Graphics-Driver-Engineer_JR2025486) | Analyze representative graphics applications and isolate bottlenecks through experiments, traces and hardware observations. Investigate visual/performance regressions and coordinate driver/architecture fixes. |
| [Senior Graphics System Software Engineer](https://nvidia.wd5.myworkdayjobs.com/NVIDIAExternalCareerSite/job/US-CA-Santa-Clara/Senior-Graphics-System-Software-Engineer_JR2019241) | Deliver graphics/AI functionality across explicit APIs, operating systems and GPU generations, working with studios and internal teams. Include robust platform behavior, integration, capability handling and failure behavior. |
| [Senior Graphics Shader Compiler Engineer](https://nvidia.wd5.myworkdayjobs.com/en-US/NVIDIAExternalCareerSite/job/Senior-Graphics-Shader-Compiler-Engineer_JR2024686) | Implement compiler components and generated-code optimizations using strong C/C++ and parallel-programming skills. Work across compiler, driver, architecture and application teams; contribute to LLVM/DXC, GPU code generation and SPIR-V extensions/standards. |
| [Neural Graphics Engineer](https://nvidia.wd5.myworkdayjobs.com/en-US/NVIDIAExternalCareerSite/job/Neural-Graphics-Engineer_JR2013428) | Connect trained models with production rendering, shaders/runtime and practical C++/Python/ML tooling. |
| [Senior Developer Technology Engineer, Simulation Performance](https://nvidia.wd5.myworkdayjobs.com/en-US/NVIDIAExternalCareerSite/job/Senior-Developer-Technology-Engineer---Simulation-Performance_JR2025157) | Deliver durable performance improvements through numerical and parallel programming, production integration and stewardship of consequential public software. |
| [Senior Software Engineer, Graphics Performance](https://nvidia.wd5.myworkdayjobs.com/en-US/NVIDIAExternalCareerSite/job/Senior-Software-Engineer--Graphics-Performance_JR2012523) | Maintain and optimize Linux graphics-driver behavior, including Vulkan/OpenGL features, shader/compiler interactions, platform correctness and long-term maintenance. |

The [Apple Graphics GPU Performance Analysis Architect](https://jobs.apple.com/en-us/details/200657300-3760/graphics-gpu-performance-analysis-architect) study recovered only an official indexed excerpt: performance planning, hardware/software bottleneck isolation, quantified architectural benefit and useful analysis tools. A full qualification list was not recovered.

The [Ubisoft 3D Graphics Engineer](https://jobs.smartrecruiters.com/Ubisoft2/744000113760857-3d-graphics-engineer-rendering-programmer-graphics-programmer) page reported the position expired; no requirements were recovered from it.

</details>

The shared direction is research-to-product ownership, causal measurement, usable tools and transfer. Senior ownership means delivering a substantial feature or fix; expert depth means testing its difficult assumptions. Principal scope adds problem selection, direction, repeated adoption and development of other engineers. Titles vary by employer. [Goals](Goals.md#professional-qualifications) distinguishes the selected scope from specialist requirements: using an API, shader cooker or vendor model does not establish driver, compiler or model authorship.

## Professional baseline

The supplied **PawelStoleckiCV.pdf**, summarized on **2026-07-26**, is the historical starting point for [progress reviews](Goals.md#reviewing-progress). It claimed experience in physically based light transport, denoising, advanced materials, GPU optimization, memory and parallelism across proprietary/commercial engines and AA/AAA open worlds. This snapshot has not been reverified.

<details>
<summary>Experience, supporting evidence and gaps visible in the archived CV</summary>

| Period in the archived CV | Claimed work |
| --- | --- |
| 2025 to the archive date: rendering engineer | A 60 FPS real-time GI target for a current-generation console demonstration; internal GI-system leadership; lighting integration with external engine/hardware partners; low-level profiling and cross-platform optimization. |
| 2024–2025: senior technical-art engineer; 2023–2024: technical-art engineer | Roles listed with little outcome detail. |
| 2021–2023: rendering programmer / technical artist | Direct, indirect and volumetric lighting; CPU/GPU/memory optimization; scalability from an older console generation to high-end PC. |
| 2019–2021: junior technical artist | GBuffer/post-processing shaders, cross-platform profiling and handheld-console rendering adaptation. |
| 2019: 3D pipeline instructor | Team training in environment workflows, shaders, optimization and rendering-pipeline cost. |
| 2017–2019: technical artist | Landscape pipelines, procedural fire/ocean shaders, foliage wind, general shaders and pipeline tools. |

The CV named HLSL, C++, RHI, GPU architecture, shader development, pipeline profiling, cross-platform scalability and hardware-aware tuning. Credits included a current open-world project/demonstration, a shipped open-world game/expansion, a handheld port and a multiplayer title. Listed talks/articles from 2018–2025 covered engine programming, PBR, shipped-game lighting, artist technical skills, natural structures, landscapes and the technical-artist role.

Education listed a leadership program and a bachelor's degree. The extracted PDF did not establish the bachelor's subject clearly enough to claim a computational degree.

The strongest measurable bullets were concentrated in the latest and 2019–2023 roles. D3D12, Vulkan, Python, PIX, RenderDoc, ML/neural graphics, Linux and named shader/compiler targets were not explicit. **An omission from this CV is a question to investigate, not evidence that the skill is absent.** Use current work, permitted artifacts and professional examples to update the assessment.

</details>

## Persona and portfolio

This study was recorded on **2026-10-10**. Affiliations are dated publisher descriptions or self-reports; they are not current employment verification. Use the examples to study engineering and communication. [Goals](Goals.md#the-three-cases) applies the lessons to three local cases.

<details>
<summary>Public engineer examples, attribution and what to study</summary>

| Engineer and source evidence | Work to study and lesson |
| --- | --- |
| **Alexey Panteleev.** [NVIDIA biography](https://developer.nvidia.com/blog/author/alexeypanteleev/) described distinguished-level Developer and Performance Technology work. | [NVRHI](https://developer.nvidia.com/blog/writing-portable-rendering-code-with-nvrhi/): portable API design, resource state, lifetime and integration. Quake II RTX is team work. The [apanteleev account](https://github.com/apanteleev) remains an unverified name match. |
| **Christoph Kubisch.** [NVIDIA biography](https://developer.nvidia.com/blog/author/ckubisch/); [pixeljetstream](https://github.com/pixeljetstream) is corroborated by an [attributed contribution](https://github.com/nvpro-samples/vk_device_generated_cmds/commit/857cc4429b05a42b4087b7662ec575b2bbb8fa7f). | Controlled comparisons of recording, binding, culling and geometry granularity. Contribution does not establish sole authorship or leadership. |
| **Nia Bickford.** [NVIDIA biography](https://developer.nvidia.com/blog/author/niabickford/) and co-credited [Slang lab](https://developer.download.nvidia.com/ProGraphics/nvpro-samples/SlangLab/Slides.pdf). | Teach Vulkan, ray tracing, compression and low-level shaders through small exercises with observable results. No personal GitHub identity was verified. |
| **Louis Bavoil.** [NVIDIA biography](https://developer.nvidia.com/blog/author/louisbavoil/) and authored performance articles. | [Shader debug information](https://developer.nvidia.com/blog/harness-powerful-shader-insights-using-shader-debug-info-with-nvidia-nsight-graphics/) and [Indiana Jones optimization](https://developer.nvidia.com/blog/path-tracing-optimization-in-indiana-jones-shader-execution-reordering-and-live-state-reductions/): executed source, limiting mechanisms and matched whole-frame results. No principal title was established. |
| **Chris Wyman.** [NVIDIA Research profile](https://research.nvidia.com/person/chris-wyman), coauthored production paper and [ReSTIR course](https://intro-to-restir.cwyman.org/). | Derive estimators, expose bias/correlation and adapt to ray, memory and quality budgets. Distinguish research coauthorship from SDK ownership. |
| **Bart Wronski.** [NVIDIA Research profile](https://research.nvidia.com/person/bart-wronski) and [professional page](https://bartwronski.com/about/) linking the explicitly outdated [bartwronski account](https://github.com/bartwronski). | Mathematics, small experiments, ML and artist/content problems. Study sampling notebooks and prototypes as explanations. |
| **Pawel Kozlowski.** [NVIDIA author page](https://developer.nvidia.com/blog/author/pkozlowski/); [2023 course notes, revised March 2024](https://intro-to-restir.cwyman.org/presentations/2023ReSTIR_Course_Notes.pdf) identified a principal Developer Technology role at publication. | Coauthored [Cyberpunk integration](https://intro-to-restir.cwyman.org/presentations/2023ReSTIR_Course_Cyberpunk_2077_Integration.pdf): partner materials, lighting, sampling and denoising constraints. |
| **Dmitry Zhdan.** Historical affiliation in a [2020 NVIDIA presentation](https://developer.download.nvidia.com/video/gputechconf/gtc/2020/presentations/s22699-fast-denoising-with-self-stabilizing-recurrent-blurs.pdf); [dzhdanNV](https://github.com/dzhdanNV) is corroborated by an [NRD maintenance commit](https://github.com/NVIDIA-RTX/NRD/commit/e06852bd545973cb224eb28820b891205cfc1624). | Classical denoising: guide encodings, disocclusion, history and integration obligations. |
| **Ingo Wald.** Historical context from a [page last modified in 2019](https://www.sci.utah.edu/~wald/) and a [2021 HPG paper](https://diglib.eg.org/bitstreams/911b634f-4d0b-4dd2-9f27-168c587033fc/download); [ingowald](https://github.com/ingowald) links the same site. | [OWL](https://github.com/ingowald/owl): typed host/device examples, launch/data ABI, acceleration structures and adoption. |
| **Thomas Müller.** [Tom94](https://github.com/Tom94) described an independent researcher and former NVIDIA principal. | tiny-cuda-nn, instant-ngp and tev connect mathematics, high-performance kernels and durable tools. Distinguish individual tools from collaborative research. |
| **Jakub Boksansky.** Historical NVIDIA context from a [2023 session](https://www.nvidia.com/en-us/on-demand/playlist/playList-c48d18a7-fd57-443a-8550-43c8cdc046ae/); a [2025 JCGT paper](https://www.jcgt.org/published/0014/02/01/paper-lowres.pdf) lists AMD. His [site](https://boksajak.github.io/) links [boksajak](https://github.com/boksajak). | [Ray Tracing Gems examples](https://boksajak.github.io/blog/RayTracingGemsChapters): transport, ray cones and many-light derivations with executable explanations. |
| **Christoph Schied.** [2019 NVIDIA biography](https://developer.nvidia.com/blog/author/cschied/) described an internship and later Facebook Reality Labs work. | [Quake II RTX](https://developer.nvidia.com/blog/real-time-path-tracing-and-denoising-in-quake-ii-rtx/): research-to-product changes and content adaptation, distinguishing Q2VKPT from later team work. |

The study did not establish NVIDIA affiliation for [Will Usher / Twinklebear](https://github.com/Twinklebear), whose profile described Luminary Cloud.

</details>

Related application-integration writing: [Alan Gray](https://developer.nvidia.com/blog/author/alan-gray/) on application optimization; [Ingo Esser](https://developer.nvidia.com/blog/author/iesser/) and [Evan Hart](https://developer.nvidia.com/blog/author/ehart/) on partner/engine work. The [neural shading course](https://research.nvidia.com/labs/rtr/publication/duca2025neural/) connects model, operator and renderer integration.

The recurring lessons are research carried into a product, API/hardware reasoning that solves a user's problem, small useful tools and explanations tested through feedback. Public projects do not establish why someone was hired.

## Documentation examples

- [RTXPT README](https://github.com/NVIDIA-RTX/RTXPT/blob/main/README.md) and [Design](https://github.com/NVIDIA-RTX/RTXPT/blob/main/Docs/Design.md): overview/requirements/build/known issues, followed by one pipeline guide.
- [NVRHI README](https://github.com/NVIDIA-RTX/NVRHI/blob/main/README.md) and [Programming Guide](https://github.com/NVIDIA-RTX/NVRHI/blob/main/doc/ProgrammingGuide.md): a small entry point and coherent resource/command/lifetime reference.
- [SHaRC overview](https://github.com/NVIDIA-RTX/SHARC/blob/main/README.md) and [Integration](https://github.com/NVIDIA-RTX/SHARC/blob/main/docs/Integration.md): minimal introduction, actionable integration, focused technical detail.
- [RTXDI README](https://github.com/NVIDIA-RTX/RTXDI/blob/main/README.md): direct build/run instructions, explicit known issues and a linked integration guide.
- [NRD README](https://github.com/NVIDIA-RTX/NRD/blob/master/README.md): substantial technical knowledge can stay in one navigable guide.
