# Goals

[Documentation](README.md) | [Roadmap](Roadmap.md)

## Target persona

Become a graphics systems engineer who can take a rendering or GPU-compute problem from first principles to a measured implementation that another engineer can use.

The focus is real-time rendering and developer technology: explicit graphics APIs, shaders, CPU/GPU architecture, concurrency, debugging and neural graphics. SparkleEngine provides a place to develop that depth and demonstrate it through a small number of complete examples.

The long-term target is principal-level responsibility: choose consequential problems, set technical direction, carry uncertain work into production and help other engineers succeed. Build on the existing rendering and technical-art background. Use the [dated professional baseline](References.md#professional-baseline) when reviewing progress.

The [role sources](References.md#role-sources) explain this choice. Compiler engineering, driver development, research science and graphics tools have distinct requirements; develop relevant depth without adding an engine subsystem for every role term.

## North Star

- **Deliver a real result.** Build the complete path needed by its user.
- **Understand the mathematics and data.** Explain units, numerical assumptions, transformations and uncertainty.
- **Prove the claim.** Make correctness, image quality, performance and failure behavior reproducible.
- **Make ownership clear.** Identify who controls mutable state, publication, synchronization, recovery and destruction.
- **Make the work usable.** Provide enough information to run, inspect, reproduce and maintain it.
- **Leave less complexity.** Remove replaced code, duplicate state and unnecessary maintenance.

## Product scope

Launcher selects scenes, reports readiness and runs build, cook and launch operations. Editor provides inspection, supported edits and developer controls. Game presents the rendered scene with camera input. All three target Windows, with separate D3D12 and Vulkan validation.

The release is a complete prebuilt bundle with approved cooked content. Matching source can reuse that bundle's outputs and replace them through the normal build/cook path. Public package validation and redistribution rights remain open.

The first release uses English keyboard/mouse input on one windowed or borderless display. Check UI at 100%, 150% and 200% DPI. The user's display resolution is independent of the benchmark resolution. Controllers, exclusive fullscreen and VR are outside this release.

Packaged use works offline after extraction, without accounts, telemetry, unattended downloads or crash uploads. Source and content acquisition are explicit. No ongoing support service or response deadline is promised.

The [level catalog](../Projects/Showcase/Levels.catalog) lists ModernSponza, Sponza, ABeautifulGame, LPSHead, CornellBox, DiffuseTransmissionPlant and CesiumMan, plus Empty. Release selection does not grant asset rights; see [distribution requirements](Engineering.md#build-and-delivery).

After release, Bistro exterior/interior becomes the flagship, with San Miguel 2.0 providing a second scene and Sponza remaining the short regression loop. Their [test roles](Rendering.md#test-scenes) are different.

Keep supporting systems as small as the rendering and reproduction workflows allow. General gameplay, physics, networking, audio, scripting, plugin SDKs, installers/updaters, Linux, extra hardware campaigns, geometry caches, frame generation, GPU-driven geometry, atmosphere/clouds/imported volumes, broad authoring or diagnostics tools and general ML runtimes are outside the selected work.

## Technical development

Demonstrate these capabilities through the same three cases where possible. The [role archive](References.md#role-sources) preserves the original responsibilities, required qualifications and differentiators.

### Rendering and GPU systems

- **Light transport.** Derive and implement rasterization, ray queries and ray-tracing pipelines, global illumination, sampling, accumulation and path tracing within a real-time budget. Explain the actual estimator, proposal and target distributions, reuse assumptions and normalization. Demonstrate independent numerical/image controls, an invalid-history or sample counterexample, temporal behavior, quality, frame time and memory. Use Bistro for the flagship and San Miguel to test whether the result transfers to different content.

- **Applied mathematics.** Use linear algebra, calculus, probability, statistics, Monte Carlo estimation, stochastic optimization, signal processing and numerical analysis to explain rendering and ML decisions. Retain definitions, units and assumptions, known values or a CPU reference, error/stability checks and a prediction that measurements can confirm or refute. Explain reference equivalence and uncertainty before comparing images or performance.

- **Explicit APIs and shaders.** Understand D3D12/Vulkan devices, queues, commands, resources, descriptors, pipelines, barriers, memory, presentation and capabilities. Work fluently in HLSL/SM6 and reason about GLSL/Slang, DXIL/SPIR-V, reflection and host/shader layout. Follow one feature from source and compiler options through cooked bytecode, bindings and executed native commands. Demonstrate both backends, completion and deferred retirement, diagnostics and a bounded failure/fallback case.

- **Hardware and concurrency.** Connect CPU caches, topology, SIMD, tasks and submission cost to GPU waves, caches, bandwidth, latency hiding, registers, occupancy, divergence, atomics and memory ordering. Use counters, traces or useful disassembly to test a specific mechanism. Retain a serial/reference control, scheduling or memory model, concurrency checks and a rejected alternative. Scope conclusions to the tested hardware, driver, compiler, API and workload.

### Software and analysis

- **C++ and Python engineering.** Write cohesive modern C++ and useful Python automation with narrow public interfaces, explicit ownership and durable failure behavior. Debug lifetime errors, races, numerical errors and API misuse. Show a clean-checkout build, representative reviewed C++, one useful Python analysis/automation consumer, bounded input/cleanup and appropriate correctness or native-validation checks. Follow [Code style](CodeStyle.md); keep instructions consistent with the working product.

- **Whole-system performance.** Study CPU preparation, GPU execution, queues, shader compilation, residency, streaming, memory, startup, frame pacing and input-to-present latency together. Retain complementary CPU, shader and resource-lifetime studies with fixed inputs, warm-up, repeats, p50/p95/p99, timelines and memory high-water. Join each predicted mechanism to a controlled experiment, quality-equivalent before/after result and regression threshold. Keep at least one rejected or inconclusive optimization.

- **Difficult debugging.** Analyze how real D3D12/Vulkan workloads use queues, barriers, descriptors, memory, pipelines, shaders and presentation. Use PIX, RenderDoc, Nsight, native validation, compiler diagnostics or crash data to distinguish application, API, compiler, driver and hardware causes. Preserve one difficult incident: exact configuration, earliest incorrect result, capture/dump and markers, reduced reproduction, competing hypotheses, experiments, root cause, focused fix and regression check.

- **Reproducible development.** Make the Windows, toolchain, D3D12 and Vulkan workflow repeatable through source control, build instructions, expected output and usable debugging/profiling steps. Demonstrate supported settings and capability limits. Windows Vulkan work establishes API experience; Linux development, another GPU architecture or driver implementation requires separate evidence. The selected engine scope remains Windows.

### Neural graphics

- **Owned neural rendering.** Deliver one real trained model or neural operator path that improves a rendering result. Connect licensed data and labels, an identified model artifact, actual runtime inference, classical comparison/fallback and held-out evaluation. Report image and temporal quality, latency, memory, generalization, failures and supported capabilities. Keep personally owned data/model/kernel work distinct from vendor reconstruction, radiance-cache or texture-compression integration.

- **Model-to-kernel work.** Read PyTorch/ONNX-style graphs and derive the operators, tensor shapes, layouts and computational cost. Translate a bounded model into HLSL/Slang or compute kernels; reason about precision, fusion, traffic and dispatch. Check every fixed operator and the full model against an FP32 reference on actual GPUs within a stated tolerance. Retain per-stage profiles and an optimization comparison that preserves numerical and product quality.

- **Machine-learning fundamentals.** Understand automatic differentiation, computational graphs, tensor broadcasting, objectives, losses, metrics, optimization, quantization and deployment constraints. Keep training, validation and test data separate by scene; test leakage, overfitting and failure cases. Retain a reproducible experiment, model/data description, classical baseline and ablations. Independently check AI-assisted code, mathematics and claims.

- **Training and deployment.** Treat data preparation/training and low-latency inference as different workloads. Profile batching, precision, layout, concurrency, memory, export and scheduling. Retain separate training and inference profiles, reproducible export, identified artifacts and the loading path. Attribute cold startup, latency tails and peak workspace; explain the deployment choice and its quality/performance tradeoff. Offline models and online adaptive caches need different reset and state-lifetime checks.

### Collaboration and direction

- **Partner integration.** Discover another team's requirements and constraints; integrate into an existing system while coordinating research, hardware, drivers, compilers, content and production concerns. Retain the alternatives, integration and failure behavior, measured result, review feedback and reproduction/adoption record. Credit personal and team contributions separately. A partner-facing capability includes configuration, diagnostics, maintenance and enough explanation for another engineer to own it.

- **Research productization and communication.** Turn a prototype into mature code, a useful tool/plugin or a justified negative result. Prioritize, package, document and review it. Present the three cases, one useful analysis consumer and a concise paper/talk/demo outline with working code, captures and reproduction links. Explain a decision to ship, simplify or remove something, including failed experiments. Write and present clearly in English.

- **Technical direction.** Choose high-value problems from first principles and finish repeated substantial work under real constraints. Lead uncertain investigations, challenge designs, give constructive reviews, teach the reasoning and remove unnecessary complexity. Retain a consequential review/decision, second usage context, observed explanation exercise and feedback-driven revision/retest. Sustained influence also needs credible professional examples of shipped impact, mentoring, stewardship or partner trust.

The [selected rendering work](Rendering.md#later-rendering-work) develops these capabilities through transport, materials, reconstruction, caches, texture compression, PTLAS and volumetrics. [Roadmap](Roadmap.md#after-release) sets the delivery order.

### Professional qualifications

The supplied principal role asks for a relevant master's degree or equivalent experience and **15 or more years of relevant work or research**. The research-productization role asks for **at least five full-time years of efficient HLSL SM6/GLSL/Slang and modern C++17 or newer**, with its own academic preferences. Other descriptions distinguish essentials from preferred experience; retain that distinction when assessing a role.

Track degree subject/level, employment duration, shipped-product contribution, partner collaboration, review/mentoring impact, spoken English and willingness to travel through the CV, talks, recommendations and permitted professional examples. Some roles also ask for Linux, Windows/Linux driver development, HIP/CUDA/OpenCL or console/CPU/GPU ISA experience. Record those as role-specific requirements and evidence gaps without expanding the engine automatically. The repository complements this professional record.

## The three cases

1. **Content to correct pixel.** Follow an asset and material through cooking, shading and checked raw output. Include selected material and bounded volumetric work, with a defined reference domain and uncertainty.
2. **One measured frame, two APIs.** Combine CPU preparation, shader and resource-lifetime studies with a difficult incident and whole-frame results. Include relevant lighting, reconstruction, cache and acceleration-structure comparisons, plus a rejected or inconclusive alternative.
3. **Owned model to shader.** Connect reference labels, data, training, checked GPU operators, runtime behavior and held-out evaluation. Separate personally owned model work from vendor cache/texture integration.

Start each case with the result, problem, constraints, personal contribution, baseline and measured configuration. Follow with mathematics/API choices, alternatives, failures, limitations, reproduction and simplification. Credit papers, libraries and collaborators.

| Reader | Time | What they should find |
| --- | --- | --- |
| Recruiter | 60–90 seconds | Identity, actual role level, three relevant outcomes, demo, location/travel status and working links |
| Hiring manager | 10 minutes | Three cases with constraints, personal contribution, measured results, code/capture links and limitations |
| Graphics specialist | 45–60 minutes | Mathematics, code, native captures, experiments and failure cases |
| Developer reproducing the work | 30 minutes plus build time | Supported setup, commands, expected output and troubleshooting |

The package contains one landing page, three case narratives, links to shareable original files, commands and expected results, and a short paper/talk/demo outline.

## Review and transfer

A reviewer should be able to trace a claim from mathematics through source, cooked code and native output, reproduce its comparison and inspect uncertainty and failure cases. Each capability needs evidence or a stated limitation.

Reproduce the documented build and result without undocumented repairs. Record who performed the run, prior involvement, environment, failed steps, assistance and output. Check hashes, prerequisites, settings and sharing rights before review. Distinguish assisted reproduction from an independent run.

Challenge a meaningful assumption in a separate technical review. Use the result in a second workflow or project and observe an explanation exercise. Record the task, feedback and revision/retest, or the evidence for leaving the design unchanged.

Label self-review, assisted reproduction and external adoption accurately. External participation is optional. Upstream contributions need a real finding and must follow the recipient's submission and disclosure requirements.

## Reviewing progress

Compare past work, present evidence and the next investment against the capabilities above. Start from the [July 2026 CV snapshot](References.md#professional-baseline), then update the assessment from actual work and permitted professional records.

| Review | Record |
| --- | --- |
| Past | What was delivered, when, under which constraints and with what personal contribution? Link the original result or professional account. |
| Now | What can be demonstrated and reproduced today? Identify the strongest artifact, its limits and any missing proof. |
| Next | Which useful result closes the most important gap? State the baseline, expected improvement, evidence to retain and how the result will be reviewed. |

Describe the strength of evidence in ordinary words:

| Evidence | Meaning |
| --- | --- |
| Absent | No supported claim or artifact yet. |
| Claimed | A CV statement, interface or unverified implementation. |
| Implemented | Reviewable code or a completed professional example with incomplete reproduction evidence. |
| Verified | A reproducible result with reference/correctness checks, configuration, measurements and limitations. |
| Transferred | Actual use, reproduction, shipment, publication or teaching with credible feedback beyond the original implementation. |

Target verified evidence for every technical capability and transfer evidence for several productization, collaboration and communication requirements.

Progress from contributing a pass, to owning a feature, to explaining a complete rendering system. Then develop technical direction, transfer capabilities and help other engineers do the same. These describe growing responsibility rather than employer job grades.

Choose work that closes an evidence gap, strengthens a headline case or simplifies a useful capability. If reproduction fails, observe the failed step, improve the product or instructions and retest.

Update the target when role evidence or professional feedback changes it; preserve the source and date in [References](References.md#role-sources). Keep tasks and delivery order in [Roadmap](Roadmap.md).
