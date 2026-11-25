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

Real-time path tracing is the only lighting path supported by Sparkle. Remove the independent Reference path tracer, its render mode, accumulation/convergence state, checkpoints/resume, dedicated exports and UI, shaders, resources and release requirements. Validate the retained renderer through bounded analytic checks, native signals/captures and suitable external comparisons; do not maintain a second Sparkle renderer for validation.

Launcher is the reviewer's Quick Start entry point: choose a supplied map and open Editor or Game. Editor exposes the selected scene inspection/editing, rendering options/effects and debug views; Game presents the scene with camera input. Those runtime controls use prepared shaders/content. All three target Windows, with separate D3D12 and Vulkan validation.

Retain basic skeletal clip playback/skinning for the Knight scene and simple movement examples; remove morph targets and morph weights. General animation authoring is outside this cleanup. Keep both native backends, multiple GPU queues and shader-target enumeration, with explicit cross-queue synchronization and lifetime ownership.

Keep the owned frame graph, rasterized and ray-traced GBuffer alternatives, and existing subsurface shading within the single real-time lighting path. Retain ReSTIR indirect lighting, TLAS refit, DLSS Super Resolution/Ray Reconstruction, exposure controls and useful signal views. Remove HDR display modes, vignette and chromatic aberration; HDR/EXR content inputs remain a separate capability. Optional automatic mesh batching is selected for removal subject to its recorded workload/cost check; authored instance groups remain.

Launcher can start configured GPU debugging tools and launch Editor in GPU-crash debugging mode. Breadcrumbs and semantic GPU markers are required: preserve D3D12 DRED, supported Vulkan checkpoints, Aftermath integration, matched debug data, native fault collection and safe dump completion. Remove extra Launcher incident monitoring/presentation and managed analysis/recovery workflows. Native diagnostic collection after a fault remains essential; external tools perform investigation.

The release target is a complete prebuilt Launcher/Editor/Game inspection bundle with approved cooked content, engine DLLs and required runtime dependencies. Reviewers extract and run it without an IDE, compiler, source acquisition or cooking. It is a portfolio demonstration, not a developer SDK. Public package validation and redistribution rights remain open.

You are the sole maintainer/developer and Rider is the only supported IDE. Keep shared/DLL engine modules, private per-module PCHs and efficient incremental builds. Your source-workspace Launcher retains preparation, dependency/content sync, build, cook, clean and debugging controls. Reviewers are not expected to iterate code, shaders or content. Remove Visual Studio IDE integration while preserving compiler/build/project support required by the proven Rider workflow. The [module decisions](Implementation/PortfolioScopeDecisions.md#who-the-engine-serves) own the audience and capability boundaries.

The public walkthrough covers launch, map selection, Editor/Game controls, supported rendering effects, known limits and return to supplied defaults. Matching source provides a concise architecture/data-flow map and clear entry points for inspecting your design. No developer installation, contributor onboarding or public engine SDK is required to review it. Missing development tools must not block a prepared scene's runtime readiness.

The first release uses English keyboard/mouse input on one windowed or borderless display. Check UI at 100%, 150% and 200% DPI. The user's display resolution is independent of the benchmark resolution. Controllers, exclusive fullscreen and VR are outside this release.

Packaged use must work offline after extraction, without accounts, telemetry, unattended downloads or crash uploads. Source and content acquisition are explicit. No ongoing support service or response deadline is promised.

The [level catalog](../Projects/Showcase/Levels.catalog) identifies intended release scenes and previews. Catalog flags establish neither qualification nor asset rights; see [distribution requirements](Engineering.md#build-and-delivery).

The selected scene set is Empty, CornellBox, classic Sponza, BistroExterior, BistroInteriorWine, ModernSponzaKnight, ModernSponzaCandles, CesiumMan, LPSHead, DamagedHelmet and ABeautifulGame. Stanford Bunny/Dragon remain conditional on a supported non-PLY route. Retire the other scene entries while preserving assets referenced by retained scenes; the [scene decisions](Implementation/PortfolioScopeDecisions.md#showcase-scenes-and-conversion-scripts) own that boundary.

Keep ownership of the RHI, frame graph, graphics API/resource lifetime and real-time path tracer. Simplify support code and delegate selected production algorithms through the [migration decisions](ThirdPartyMigration.md), focusing personal research on the three cases and crediting upstream work. [Roadmap](Roadmap.md#supporting-system-simplification) sets priorities.

Keep supporting systems as small as the rendering and reproduction workflows allow. General gameplay, physics, networking, audio, scripting, plugin SDKs, installers/updaters, Linux, extra hardware campaigns, geometry caches, frame generation, GPU-driven geometry, atmosphere/clouds/imported volumes, broad authoring or diagnostics tools and general ML runtimes are outside the selected work.

## Technical development

Develop these longer-term capabilities through the same three cases where useful. They guide your study and evidence, not features to add simply because they appear in a role description. The selected module decisions define the current portfolio engine. Each claimed result needs an explained decision, checked output and honest limitations. [Engineering](Engineering.md#verification) owns the validation method; the [role archive](References.md#role-sources) preserves the original qualifications.

### Rendering and GPU systems

- **Light transport.** Derive and implement lighting, sampling, temporal reuse and path tracing within the single real-time lighting path. Explain estimators, distributions, reuse assumptions and normalization, with independent controls and counterexamples that test correctness, temporal behavior and cost.

- **Applied mathematics.** Use linear algebra, calculus, probability/statistics, Monte Carlo estimation, optimization, signal processing and numerical analysis to explain rendering and ML decisions. State units, assumptions, reference uncertainty and a prediction that measurements can confirm or refute.

- **Explicit APIs and shaders.** Understand D3D12/Vulkan resources, commands, synchronization, memory, presentation and capabilities. Work fluently in HLSL/SM6 and reason about GLSL/Slang, DXIL/SPIR-V, reflection and host/shader layout. Trace a feature from source through native execution on both APIs, including lifetime, diagnostics and failure behavior.

- **Hardware and concurrency.** Explain how CPU caches, SIMD, tasks and submission interact with GPU waves, bandwidth, registers, occupancy, divergence, atomics and memory ordering. Test a specific mechanism with traces/counters or useful disassembly, a serial/reference control and a rejected alternative. Scope conclusions to the tested system and workload.

### Software and analysis

- **C++ and Python engineering.** Write cohesive modern C++ and useful Python automation with narrow interfaces, explicit ownership and durable failure behavior. Debug lifetime, race, numerical and API errors. Demonstrate reviewed code, a useful analysis consumer and a reproducible build under [Code style](CodeStyle.md).

- **Whole-system performance.** Connect CPU preparation, GPU execution, queues, compilation, residency/loading, memory, startup, frame pacing and input-to-present latency. Develop complementary CPU, shader and resource-lifetime studies with controlled, quality-equivalent comparisons and regression thresholds. Keep a rejected or inconclusive optimization.

- **Difficult debugging.** Use captures, native validation, compiler diagnostics or crash data to distinguish application, API, compiler, driver and hardware causes. Preserve one difficult incident from the earliest incorrect result through a reduced reproduction, competing hypotheses, root cause, focused fix and regression check.

- **Reproducible development.** Make the Windows, D3D12 and Vulkan workflow repeatable with identified tools, source, commands, expected output and debugging/profiling steps. Demonstrate capability limits. Linux, another GPU architecture and driver implementation require separate evidence; the engine scope remains Windows.

### Neural graphics

- **Owned neural rendering.** Deliver one trained model/operator path with owned data/model/kernel work, licensed labels, actual inference, a classical comparison/fallback and held-out evaluation. Report quality, temporal behavior, latency, memory, generalization and failures. Attribute vendor integrations separately.

- **Model-to-kernel work.** Derive operators, tensor shapes, layouts and cost from a model graph, then translate a bounded model into HLSL/Slang kernels. Explain precision, fusion, traffic and dispatch. Check each operator and the full model on actual GPUs against a reference, and profile optimizations that preserve numerical and product quality.

- **Machine-learning fundamentals.** Understand differentiation, graphs, broadcasting, losses/metrics, optimization, quantization and deployment. Use separate scene-based train/validation/test data, leakage and overfitting checks, a classical baseline and ablations. Independently check AI-assisted code, mathematics and claims.

- **Training and deployment.** Profile data preparation/training separately from inference, including startup, latency tails and peak workspace. Reproduce export and loading with identified artifacts; explain the quality/performance tradeoff. Offline models and online adaptive caches need different reset and lifetime checks.

### Collaboration and direction

Use permitted professional work, technical discussion and optional feedback to demonstrate collaboration. Sparkle remains a single-maintainer project; these career goals do not require contributor infrastructure or a general-purpose engine SDK.

- **Partner integration.** Understand another team's constraints and coordinate research, hardware, drivers, compilers, content and production concerns. Deliver usable configuration, diagnostics and maintenance guidance with measured results, review feedback and reproduction/adoption records. Credit personal and team contributions separately.

- **Research productization and communication.** Turn a prototype into maintained code, a useful tool or a justified negative result. Prioritize, package, document and review it; explain the decision to ship, simplify or remove it, including failed experiments. Present the cases and a concise paper/talk/demo in clear English with working reproduction links.

- **Technical direction.** Choose high-value problems and finish repeated substantial work under real constraints. Lead uncertain investigations, challenge designs, review constructively, teach reasoning and remove complexity. Demonstrate decisions improved through feedback and second-context use, alongside credible professional evidence of shipped impact, mentoring, stewardship or partner trust.

The [rendering guide](Rendering.md#later-rendering-work) selects feature scope; [Roadmap](Roadmap.md#after-release) sets delivery order.

### Professional qualifications

The supplied principal role asks for a relevant master's degree or equivalent experience and **15 or more years of relevant work or research**. The research-productization role asks for **at least five full-time years of efficient HLSL SM6/GLSL/Slang and modern C++17 or newer**, with its own academic preferences. Other descriptions distinguish essentials from preferred experience; retain that distinction when assessing a role.

Track degree subject/level, employment duration, shipped-product contribution, partner collaboration, review/mentoring impact, spoken English and willingness to travel through the CV, talks, recommendations and permitted professional examples. Some roles also ask for Linux, Windows/Linux driver development, HIP/CUDA/OpenCL or console/CPU/GPU ISA experience. Record those as role-specific requirements and evidence gaps without expanding the engine automatically. The repository complements this professional record.

## The three cases

Use these as a framework for explaining your work and longer-term growth. Reviewer Quick Start is the prepared engine and its code guide, not a request to run research, build shaders or prepare datasets. The separate narrative/future-scope decisions remain in the module checklist.

1. **Content to correct pixel.** Follow an asset and material through cooking, shading and checked raw output. Include selected material and bounded volumetric work, with a defined reference domain and uncertainty.
2. **One measured frame, two APIs.** Combine CPU preparation, shader and resource-lifetime studies with a difficult incident and whole-frame results. Include relevant lighting, reconstruction, cache and acceleration-structure comparisons, plus a rejected or inconclusive alternative.
3. **Owned model to shader.** Connect independently checked external or analytic labels, data, training, checked GPU operators, runtime behavior and held-out evaluation. This later work does not require a Sparkle Reference renderer. Separate personally owned model work from vendor cache/texture integration.

Open each case with the result, problem, constraints, personal contribution, baseline and measured configuration. Link owned code, native captures and a reproduction command. Explain mathematics/API choices, alternatives, failures, limitations and simplification; credit papers, libraries and collaborators. Integration cases also explain SDK selection, signal/lifetime decisions and maintenance removed.

| Reader | Time | What they should find |
| --- | --- | --- |
| Recruiter | 60–90 seconds | Identity, actual role level, three relevant outcomes, demo, location/travel status and working links |
| Hiring manager | 10 minutes | Launcher/map walkthrough, selected results and constraints, personal contribution, code/capture links and limitations |
| Graphics specialist | 45–60 minutes | Mathematics, code, native captures, experiments and failure cases |
| Reviewer running the portfolio | 30 minutes | Quick Start, map selection, Editor/Game controls, supported effects/options, expected results and troubleshooting; no development setup |
| Reviewer inspecting the code | As needed | Matching source, application entry points, module ownership/data flow and explanations of important decisions; no requirement to build |

Prepare a landing/Quick Start page, complete inspection bundle, concise architecture guide, matching source links and selected code/capture/result explanations. Your own build/cook commands remain documented separately. Additional case-narrative or future research prerequisites follow the still-open DO-01/DO-02 scope decisions; the runtime walkthrough itself does not require a reviewer to develop anything.

## Review and transfer

A reviewer should be able to run the selected demonstrations, explore the approved runtime controls and trace a claim through matching source, cooked/native output and your measurements. Provide the important assumptions, uncertainty and failure cases. You reproduce the build/cook and controlled experiments; a reviewer may inspect their evidence without repeating the development workflow.

Reproduce your maintainer build/cook workflow and the reviewer's extracted-package result without undocumented repairs. Reviewer acceptance does not require building or cooking from source. Record who performed the run, prior involvement, environment, failed steps, assistance and output. Check hashes, prerequisites, settings and sharing rights before review. Distinguish assisted reproduction from an independent run.

For further technical development, challenge an assumption in review and try a result in a second context when useful. Record actual feedback and revisions. External adoption, another developer's source build and a second project are optional evidence, not prerequisites for reviewer Quick Start; separate experiment repositories follow Sparkle cleanup.

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
