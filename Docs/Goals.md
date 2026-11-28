# Goals

[Scope decisions](Implementation/PortfolioScopeDecisions.md) | [Roadmap](Roadmap.md) | [Module implementation plans](Implementation/PortfolioReduction/README.md)

## Target persona

Become a graphics systems engineer who can take a rendering or GPU-compute problem from first principles to a measured implementation that another engineer can use.

The focus is real-time rendering and developer technology: explicit graphics APIs, shaders, CPU/GPU architecture, concurrency, debugging and neural graphics. SparkleEngine provides a place to develop that depth and demonstrate it through a small number of complete examples.

The long-term target is principal-level responsibility: choose consequential problems, set technical direction, carry uncertain work into production and help other engineers succeed. Build on the existing rendering and technical-art background. Use the [dated professional baseline](Studies/CareerAndPortfolio.md#professional-baseline) when reviewing progress.

The [role sources](References.md#role-sources) explain this choice. Compiler engineering, driver development, research science and graphics tools have distinct requirements; develop relevant depth without adding an engine subsystem for every role term.

## North Star

- **Deliver a real result.** Build the complete path needed by its user.
- **Understand the mathematics and data.** Explain units, numerical assumptions, transformations and uncertainty.
- **Prove the claim.** Make correctness, image quality, performance and failure behavior reproducible.
- **Make ownership clear.** Identify who controls mutable state, publication, synchronization, recovery and destruction.
- **Make the work usable.** Provide enough information to run, inspect, reproduce and maintain it.
- **Leave less complexity.** Remove replaced code, duplicate state and unnecessary maintenance.

## Product scope

Sparkle is a personally owned rendering portfolio, maintained by one developer. Keep a reduced suite with distinct Launcher, Editor and Game, one Showcase project and a deliberately selected scene set. Exact capability and content decisions live in the [scope record](Implementation/PortfolioScopeDecisions.md).

Reviewers choose a supplied map in Launcher, explore Editor/Game and rendering controls, then inspect matching source. They receive a complete offline bundle and a concise architecture walkthrough; they do not build, cook or install a development environment. The maintainer retains efficient preparation, iteration and debugging workflows.

Real-time path tracing is the only supported lighting path. Retain ownership of the renderer, frame graph, graphics API integration and resource lifetime on D3D12 and Vulkan. Preserve basic skeletal motion, selected material/rendering capabilities and useful native GPU diagnostics, including breadcrumbs and usable crash data.

Preserve separate game and render ownership, data-oriented storage, useful parallel work, multiple GPU queues and safe immutable handoffs. Smaller support systems must strengthen this architecture and remain understandable enough to modify manually.

The first release targets Windows with English keyboard/mouse input on one windowed or borderless display. Support 100%, 150% and 200% DPI; display resolution is independent of benchmark resolution. Controllers, exclusive fullscreen and VR are outside the release. Packaged use is offline, without accounts, telemetry, automatic downloads or crash uploads; acquisition is explicit.

No general gameplay platform, contributor SDK, installer/updater or support service is required. Optional research and additional experiment repositories follow a focused, usable engine. [Roadmap](Roadmap.md) owns outcome order and product limits; [Engineering](Engineering.md) owns design and delivery principles.

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

The [product scope](#product-scope) selects retained engine features; [Roadmap](Roadmap.md#after-release) sets delivery order.

### Professional qualifications

Assess role-specific essentials, preferred experience and equivalent qualifications against current source material and your professional record. Track shipped contributions, technical direction, collaboration, communication and mentoring alongside repository evidence. A job-description term does not automatically justify another engine subsystem; dated examples remain in the [career study](Studies/CareerAndPortfolio.md).

## The three cases

Use these as a framework for explaining your work and longer-term growth. Reviewer Quick Start is the prepared engine and its code guide, not a request to run research, build shaders or prepare datasets. These are explanatory and longer-term development themes, not additional engine-release prerequisites.

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

Prepare a landing/Quick Start page, complete inspection bundle, concise architecture guide, matching source links and selected code/capture/result explanations. Your own build/cook commands remain documented separately. A reviewer can assess the prepared walkthrough and source without taking part in development or future research.

## Review and transfer

A reviewer should be able to run the selected demonstrations, explore the approved runtime controls and trace a claim through matching source, cooked/native output and your measurements. Provide the important assumptions, uncertainty and failure cases. You reproduce the build/cook and controlled experiments; a reviewer may inspect their evidence without repeating the development workflow.

Reproduce your maintainer build/cook workflow and the reviewer's extracted-package result without undocumented repairs. Reviewer acceptance does not require building or cooking from source. Record who performed the run, prior involvement, environment, failed steps, assistance and output. Check hashes, prerequisites, settings and sharing rights before review. Distinguish assisted reproduction from an independent run.

For further technical development, challenge an assumption in review and try a result in a second context when useful. Record actual feedback and revisions. External adoption, another developer's source build and a second project are optional evidence, not prerequisites for reviewer Quick Start; separate experiment repositories follow Sparkle cleanup.

Label self-review, assisted reproduction and external adoption accurately. External participation is optional. Upstream contributions need a real finding and must follow the recipient's submission and disclosure requirements.

## Reviewing progress

Compare past work, present evidence and the next investment against the capabilities above. Start from the [July 2026 CV snapshot](Studies/CareerAndPortfolio.md#professional-baseline), then update the assessment from actual work and permitted professional records.

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
