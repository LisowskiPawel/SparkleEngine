# Roadmap

[Scope decisions](Implementation/PortfolioScopeDecisions.md) | [Goals](Goals.md)

## Current priority

The delivery journey is prebuilt bundle -> Launcher Quick Start -> supplied map -> Editor or Game -> supported controls/effects and source inspection. You are the only developer and use the source-workspace Launcher for iteration. Preserve those developer tools without making them prerequisites for the reviewer. The [workflow acceptance table](Implementation/PortfolioScopeDecisions.md#acceptance-by-workflow) separates the two responsibilities.

The selected scope uses the real-time path tracer as the only lighting path. Remove the Reference renderer and its dedicated infrastructure through the [cross-module removal decision](Implementation/PortfolioScopeDecisions.md#reference-path-tracer-removal-across-modules). This is a planned removal, not a claim that source retirement is complete.

Recover the real-time Sponza image using one fixed camera and identified source, content and settings. Inspect raw signals before and after display processing; use bounded analytic checks and suitable external comparisons to find the first incorrect stage. Retire Reference-only defects and convergence/export work; preserve repairs required by the retained real-time frame.

Reuse earlier design decisions and shader-identity results when their inputs still match. Current real-time radiance and the replacement release package still need validation.

For GPU-crash diagnostics, preserve breadcrumbs/semantic markers, DRED, supported Vulkan checkpoints, Aftermath callbacks, matched shader/symbol data and bounded native collection/dump completion. Retain debugger/tool launch and Editor crash-mode startup. Remove only the extra Launcher incident workflow identified in the [explicit preservation/removal table](Implementation/PortfolioScopeDecisions.md#gpu-crash-diagnostics-preservation-and-removal). Show exact files/functions before implementation cuts and verify usable native crash output afterward. Existing evidence applies only at its tested identities; source inspection and a successful launch do not qualify breadcrumb or crash-dump correctness.

## Before the first release

Build and delivery target shared/DLL engine modules with private PCHs and a proven Rider-only maintainer workflow. Remove Visual Studio IDE selection/integration, preserving required compiler/build/project machinery. Ship a complete offline inspection bundle for Launcher, Editor and Game; reviewers do not acquire sources or build/cook. Keep those tools for your iteration, not as reviewer prerequisites. See the [build/package boundary](Implementation/PortfolioScopeDecisions.md#cmake-packaging-and-repository-automation).

The selected cleanup retains both native backends, multiple GPU queues, shader-target enumeration and basic skeletal animation for Knight; morph targets/weights and PTLAS are retired from the target scope. Keep required Knight FBX/base-scene dependencies and validate moving/skinned geometry in the real-time path tracer. The [module decisions](Implementation/PortfolioScopeDecisions.md) own the exact retained boundaries.

1. **Finish rendering.** Validate the sole real-time lighting path, frame/scene lifetime, geometry and materials, raw lighting, temporal behavior, exposure/reconstruction, display effects and the complete frame.
2. **Check the scenes.** Fix starting cameras/settings, inspect content/materials, compare with suitable controls and resolve image defects, including motion. Exercise reviewer map selection, Editor/Game launch, approved options/effects/debug views and return to supplied state without a build/cook step.
3. **Measure the frame.** Meet the 30 FPS requirement, memory limits and stability checks on the supported system. Test D3D12 and Vulkan separately.
4. **Test the package.** Freeze the repaired package and reproduce the reviewer prebuilt workflow: standard-user clean-machine/offline launch, all three products, selected scenes, symbols where promised, failures and recovery. Check your maintainer build/cook workflow separately; reviewer source builds are not a release gate. ShaderMake code/build/CLI checks do not qualify development Editor reload/cancel/close, D3D12/Vulkan pipelines or native shader-source correlation; verify retained developer functionality, compiler/SDK delivery for your workflow and the frozen compiler cost cells against the [bound qualification record](../build/validation/shadermake/stage-2/20261122-next/qualification.json) and [remaining constraints](../build/validation/shadermake/closeout-20261122/qualification-handoff.md). Do not require compiler/SDK setup merely to run the inspection bundle.
5. **Publish.** Complete rights/notices/provenance, Windows signing/timestamping, review of signed files and verification after download. Check the final package's custom SPDX license text, [matching Qt source companions and library replacement](../CMake/Notices/Qt/README.md), and each SDK's redistribution terms.
6. **Close the release.** Resolve blockers, finish your maintainer checks and rehearse the reviewer walkthrough. Include matching source links, application entry points and a concise architecture/data-flow guide; no other developer's installation or source build is required before starting the next program.

Scene exports and unsigned archives may support feature testing before publication approval; qualify the final package's actual files separately. The earlier package has a scene-publication interruption defect; package removal was outside its tested scope. Repairs need a newly tested archive; image, performance, native-API, reproduction and publication gates remain open. Later RELAX and renderer extensions are not release prerequisites.

## Release requirements

### Hardware

The first benchmark system is the Ryzen 9 8940HX / GeForce RTX 5070 Ti Laptop machine with about 64 GiB RAM, 12 GiB GPU memory and NVMe storage. Record its actual OS, driver and build with every result.

General prerequisites are Windows 11 x64 24H2 or newer, four physical cores/eight threads with SSE4.1, 16 GiB RAM and 8 GiB GPU memory. These floors do not establish support for a lower-end machine.

D3D12 requires feature level 12_1, Shader Model 6.6 and DXR 1.1. Vulkan requires 1.3 and the full rendering/RT, layout, descriptor, address and synchronization feature set used by the frame. The [device selector](../Engine/RHI/Private/Vulkan/Device/VulkanDeviceBootstrap.cpp) owns detailed admission checks; version support alone does not qualify the full frame. Report missing requirements before attempting the unsupported path.

### Time, memory and storage

| Operation | Limit on the benchmark system |
| --- | --- |
| First responsive UI or intentional Game output, cold file cache | 15 seconds |
| Default example selected to first correct frame | 30 seconds; other selected examples 60 seconds |
| Quit or cancellation | Game 5 seconds; Launcher/Editor finish owned work in 10 seconds |
| Runtime memory high-water | Process commit 12 GiB; tracked local GPU memory 7 GiB; no growth across repeated loops |
| Game size | Archive 4 GiB; extracted tree 8 GiB; 16 GiB free before extraction |
| Source preparation | Cold sync/configure/build/cook 120 minutes; warm no-change replay 15 minutes; peak system memory 32 GiB and working storage 100 GiB |
| Local Launcher readiness refresh | 5 seconds, with download/process time reported separately |

The total bundle and selected scene content size still need an agreed accounting method, using the current scene decisions rather than the former seven-scene assumption. The Game limits alone do not establish a limit for that combined package. Reference accumulation/convergence requirements are retired; the retained lighting renderer is subject to the real-time frame requirement.

### Frame-rate test

Use ShippingGame at 1920x1080 with the declared native real-time Lit or DLSS Quality/Ray Reconstruction configuration, SDR output and the retired lens effects absent. Disable VSync and validation for measurement. After settling, warm up for at least 300 frames, then measure at least 300 valid frames. Repeat three times per scene and backend.

Presented, CPU and GPU 95th-percentile frame times must each be at most 33.33 ms. Report tails, exclusions, load time, memory and thermal behavior. Generated frames, dynamic quality, silent fallback or changing quality after measurement cannot satisfy the target.

### Starting settings

Target D3D12, real-time Lit, VSync On, automatic histogram exposure, ACES approximation, SDR and neutral grading. HDR display, vignette and chromatic aberration are selected for removal, not optional starting modes; existing source/settings still need implementation cleanup. VSync off is a benchmark setting, not a change to the default. Retain both GBuffer algorithms and existing subsurface shading within that frame.

## After release

Use the retained Bistro/Sponza and selected diagnostic/animation scenes with reproducible content, cameras and suitable comparison results. San Miguel is selected for removal. Continue with two related bodies of work:

- **Classical graphics:** CPU preparation, GPU shader and resource-lifetime studies, useful offline analysis and one difficult incident.
- **Owned neural graphics:** independently checked external or analytic diffuse-indirect labels, separate datasets, one fixed model, checked GPU kernels and held-out quality, cost, failure and package results.

Model production starts after the classical-study technical results are ready. Final documented reproduction happens later, when both bodies of work can be included in the portfolio.

The renderer work follows this order:

1. Select one production sampling owner per stage within the real-time path tracer, including the bounded RTXDI replacement investigation; finish selected direct/indirect light/path improvements and base, anisotropic and coated materials. No independent Reference renderer or second lighting path is retained. Focused research must fit the selected real-time path or wait until after cleanup.
2. Qualify the existing DLSS path, integrate one NRD/RELAX path and the selected AMD FSR upscaler, then check affected guides, temporal behavior, native APIs and delivery. XeSS remains a deferred comparison; the owned model is independent of this step.
3. Bounded volumetrics, SHaRC, NRC and neural texture compression as their inputs become ready. **Finish SHaRC, including package and reproduction results, before NRC production.** Neural texture compression requires the classical-study results and does not wait for NRC.

PTLAS is removed from the selected scope; do not preserve it as an unresolved hardware gate. Test retained features individually before the selected combined configuration.

Use the [case framework](Goals.md#the-three-cases) to explain completed technical results and guide later work, subject to the open DO-01/DO-02 scope decisions. You reproduce development experiments; reviewers can explore the prepared runtime and inspect the matching code/evidence. Independent source builds, external adoption and second-context use are optional evidence, not prerequisites for the inspection workflow.

## Supporting-system simplification

Launcher scene-view qualification retains physical fractional/mixed-DPI, assistive-technology and broader GUI/GPU/profile/capture/package checks as **UNVERIFIED** in the [bound delivery record](../build/validation/launcher-item-views/candidate-20261122/REPORT.md). Its required temporary cleanup, recorded with that evidence, remains open after automatic approval review rejected deletion; this adds no release prerequisite.

Reduce maintained native support code before adding optional rendering SDK capabilities. Task-runtime replacement and SDL3 window/input code delivery are complete; this does not close broader product qualification, and no SDL3 net line saving is claimed. Compiler preprocessing reuse remains conditional under CP-04. Other replacements require a demonstrated net win. The [module scope decisions](Implementation/PortfolioScopeDecisions.md#where-the-code-footprint-actually-is) now own named cuts, retained boundaries and conditional replacements; [Engineering](Engineering.md#module-boundaries) owns integration/lifetime rules. These simplifications add no new first-release prerequisites.

Task-runtime code delivery closes with actual owner/capacity/lifetime checks and retirement of the local runtime. The [final record](../build/validation/task-runtime/close-20261122/REPORT.md) and [frozen comparison limits](../build/validation/task-runtime/close-20261122/QUALIFICATION-LIMITS.md) retain unrun timing, frame distributions, memory, cancellation latency, full build/package growth and relocated/clean-machine delivery. New ShaderMake/Slang debug exports need GPU-diagnostics qualification at their actual source/tool/SDK identity; rejecting old receipts remains required. Static/shared and focused D3D12/Vulkan owner checks do not certify broader release behavior. Cost targets missed for added code, API, state and cleanup; the net retirement result is separate. These are existing product/integration qualification responsibilities, without a replacement implementation plan or a new first-release prerequisite.

Window/input product qualification retains the [frozen comparison limits](../build/validation/window-input/stage-1-20261122/PROTOCOL.md) and [bound observations](../build/validation/window-input/delivery-20261122/REPORT.md). Focused 100% single-monitor, input, native chrome, both API capture/GPU replay and delivery results do not certify 150/200% or mixed-DPI displays, physical slow/fast/edge-crossing response, layout/IME/clipboard, full profile/image/startup/frame/memory distributions, or relocated package/clean-machine delivery; these remain UNVERIFIED. Preserve the failed Vulkan device-loss run with a preceding RenderDoc replay active and the replay-GUI loading/cleanup issue separately from passing isolated captures and CLI replays. Broader product/tool qualification stays here and in the report, without a replacement implementation plan.

Keep product blockers open through final qualification: raster/ray material parity for UV transforms, address modes and occlusion strength; the unresolved Vulkan swapchain allocation failure; and full image/package/reproduction checks on both APIs. Isolated cooker or sampling results do not close those gates. See the retained [texture-cooking boundary](Implementation/PortfolioScopeDecisions.md#texturecooker).

## Choosing the next task

Prefer an unmet dependency, a useful rendering improvement or a focused simplification. Reuse earlier results when their inputs still match.

Use the [performance method](Engineering.md#performance-and-images) for measurements and the [capability goals](Goals.md#technical-development) to choose evidence worth keeping. Analyze existing exports before adding diagnostics infrastructure.
