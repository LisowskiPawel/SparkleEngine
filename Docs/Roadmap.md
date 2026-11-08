# Roadmap

[Documentation](README.md) | [Goals](Goals.md)

## Current priority

Recover the Sponza image. Use one fixed camera and record the source, content and settings. Compare Lit and Reference before and after display processing, then fix the first incorrect input or rendering stage. The analytic-rectangle emitter defect is a separate repair. See the [Reference requirements](Rendering.md#reference-renderer).

Reuse earlier design decisions and shader-identity results when their inputs still match. Current radiance, reference accuracy and the replacement release package still need validation.

## Before the first release

1. **Finish rendering.** Validate Reference, frame/scene lifetime, geometry and materials, raw lighting, temporal behavior, exposure/reconstruction, display effects and the complete frame.
2. **Check the scenes.** Fix cameras, inspect content/materials, compare with valid references and resolve image defects, including motion.
3. **Measure the frame.** Meet the 30 FPS requirement, memory limits and stability checks on the supported system. Test D3D12 and Vulkan separately.
4. **Test the package.** Freeze the repaired package and reproduce prebuilt and matching-source workflows, scenes, symbols, failures and recovery.
5. **Publish.** Complete rights/notices/provenance, Windows signing/timestamping, review of signed files and verification after download.
6. **Close the release.** Resolve blockers and finish the maintainer review before starting the next program.

Create a needed scene export or unsigned test archive when its inputs are available. Feature testing can use it before publication approval; final package review must identify the actual tested files. Later RELAX and renderer extensions are not release prerequisites.

The earlier unsigned package has a known scene-publication interruption defect. Source changes need focused checks and a newly tested archive. Package removal was excluded from the earlier result. Image, performance, native-API, reproduction and publication work remains open; [earlier records](References.md#earlier-local-results) describe their own builds.

## Release requirements

### Hardware

The first benchmark system is the Ryzen 9 8940HX / GeForce RTX 5070 Ti Laptop machine with about 64 GiB RAM, 12 GiB GPU memory and NVMe storage. Record its actual OS, driver and build with every result.

General prerequisites are Windows 11 x64 24H2 or newer, four physical cores/eight threads with SSE4.1, 16 GiB RAM and 8 GiB GPU memory. These floors do not establish support for a lower-end machine.

D3D12 requires feature level 12_1, Shader Model 6.6 and DXR 1.1. Vulkan requires 1.3 and the rendering/RT, descriptor-indexing, buffer-address and synchronization features used by the frame. Report missing requirements before attempting the unsupported path.

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

The total bundle and seven-scene content size still needs an agreed accounting method. The Game limits alone do not establish a limit for that combined package. Reference accumulation has convergence, progress, cancellation and resource checks instead of a realtime FPS requirement.

### Frame-rate test

Use ShippingGame at 1920x1080 with the declared native Lit or DLSS Quality/Ray Reconstruction configuration and approved lens effects. Disable VSync and validation for measurement. After settling, warm up for at least 300 frames, then measure at least 300 valid frames. Repeat three times per scene and backend.

Presented, CPU and GPU 95th-percentile frame times must each be at most 33.33 ms. Report tails, exclusions, load time, memory and thermal behavior. Generated frames, dynamic quality, silent fallback or changing quality after measurement cannot satisfy the target.

### Starting settings

Use D3D12, Lit, VSync On, automatic histogram exposure, ACES approximation, SDR, neutral grading and both lens effects On. HDR is opt-in. VSync off is a benchmark setting, not a change to the default. Choose unresolved effect strengths before implementation.

## After release

Begin with reproducible Bistro/San Miguel content, cameras and references. Continue with two related bodies of work:

- **Classical graphics:** CPU preparation, GPU shader and resource-lifetime studies, useful offline analysis and one difficult incident.
- **Owned neural graphics:** correct diffuse-indirect reference labels, separate datasets, one fixed model, checked GPU kernels and held-out quality, cost, failure and package results.

Model production starts after the classical-study technical results are ready. Final documented reproduction happens later, when both bodies of work can be included in the portfolio.

The renderer work follows this order:

1. Direct/indirect estimators and selected light/path improvements; base, anisotropic and coated materials.
2. Super Resolution/Ray Reconstruction and one RELAX path, then the affected lighting/material guides, temporal behavior, native APIs and delivery checks. The owned model is independent of this step.
3. PTLAS, bounded volumetrics, SHaRC, NRC and neural texture compression as their inputs become ready. **Finish SHaRC, including package and reproduction results, before NRC production.** Neural texture compression requires the classical-study results and does not wait for NRC.

A PTLAS hardware blocker does not prevent independent work. Test features individually before the selected combined configuration.

Build the [three portfolio cases](Goals.md#the-three-cases) from the completed technical results. Reproduce them from the instructions, then complete the design review, second-context use, explanation exercise and feedback-driven revision.

## Supporting-system simplification

Planned, not started. [Third-party migration](ThirdPartyMigration.md) owns candidate research, sources, capability requirements and adoption decisions. Preserve existing workflows and retain the current implementation when no candidate transfers substantial ownership without a second complex local system. These replacements support the current rendering priority and add no first-release prerequisites.

Implement qualifying JSON and CLI parsing slices first. Investigate task execution and cooked metadata for larger gains, followed by the conditional texture and native window/input opportunities. Adopt only after the migration guide's capability, ownership, cost and delivery requirements are met; the remaining candidates stay deferred.

## Choosing the next task

Prefer an unmet dependency, a useful rendering improvement or a focused simplification. Reuse earlier results when their inputs still match.

Use the [performance method](Engineering.md#performance-and-images) for measurements and the [capability goals](Goals.md#technical-development) to choose evidence worth keeping. Analyze existing exports before adding diagnostics infrastructure.
