# Roadmap

[Goals](Goals.md) | [Engineering](Engineering.md) | [Implementation plans](Implementation/PortfolioReduction/README.md)

This roadmap sets outcome priorities and product limits. The implementation plans own module order, active blockers, qualification protocols and results; planned scope is not evidence of completion.

## Current priority

Finish a smaller, understandable engine whose retained behavior works end to end. Reduce support-system complexity while preserving owned rendering, concurrency, native diagnostics and the selected portfolio capabilities.

The delivery path is prepared bundle -> Launcher -> supplied scene -> Editor or Game -> rendering controls and matching source. Keep the maintainer's preparation workflow separate from reviewer prerequisites.

## Before the first release

1. **Focus the engine.** Complete approved removals and reshape surviving systems around the reduced scope, preserving thread, data and resource-lifetime boundaries.
2. **Establish correct rendering.** Verify the real-time lighting path, scene/material meaning, animation, temporal behavior and image quality on both APIs.
3. **Make inspection useful.** Provide intentional scene defaults, runtime controls, native debugging and clear source/architecture entry points.
4. **Meet product limits.** Measure frame time, startup, cancellation, memory and stability under declared conditions.
5. **Qualify delivery.** Verify the actual complete bundle offline on a clean system, separately from maintainer build/cook reproduction. Resolve rights, notices, signing and download identity before public release.

Keep unmet image, performance, native-API, diagnostics and package gates open until observed on the affected final identities. Earlier source/build or focused feature results do not establish release readiness. Later research and optional renderer extensions are not first-release prerequisites.

## Release requirements

### Hardware

General prerequisites are Windows 11 x64 24H2 or newer, four physical cores/eight threads with SSE4.1, 16 GiB RAM and 8 GiB GPU memory. These floors do not establish support for a lower-end machine.

D3D12 requires feature level 12_1, Shader Model 6.6 and DXR 1.1. Vulkan requires 1.3 and the full rendering/RT, layout, descriptor, address and synchronization feature set used by the frame. Version support alone does not qualify the full rendering feature set. Report missing requirements before attempting the unsupported path.

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

The Game size limits do not establish a combined-bundle limit. Measure and report the complete bundle separately.

### Frame-rate test

Target at least 30 FPS at 1920x1080 under declared quality settings: presented, CPU and GPU 95th-percentile frame times must each be at most 33.33 ms. Qualify both APIs without reducing promised quality or counting generated frames. The execution plan owns the fixed measurement protocol and environment.

### Starting settings

Ship intentional, documented defaults and a reliable return to the supplied state. Keep normal presentation settings distinct from controlled benchmark settings.

## After release

Develop two connected bodies of evidence: classical CPU/GPU/resource-lifetime studies with a difficult debugging case, then owned neural graphics with checked data, models, operators and held-out evaluation. Model production follows the classical-study results; final reproduction should explain both bodies of work.

Strengthen lighting/sampling and materials before extending reconstruction and caches. Qualify the retained DLSS path, then selected NRD/RELAX and AMD FSR work; XeSS remains a deferred comparison. Broader volumetrics, SHaRC, NRC and neural texture compression follow their required inputs. Finish SHaRC, including delivery and reproduction, before NRC production; neural texture compression requires the classical results and need not wait for NRC.

Use the [case framework](Goals.md#the-three-cases) to explain useful results and guide later development. Additional research, external adoption and another developer's source build are optional evidence, not prerequisites for inspecting the portfolio.

## Supporting-system simplification

Reduce maintained support code before adding optional capabilities. Retain a supporting system only for a real rendering, iteration, diagnostic or reproduction need. Prefer complete removals and direct library mechanisms over replacement frameworks; account for replacement and moved code in net savings.

The [accepted scope](Implementation/PortfolioScopeDecisions.md) owns exact selections and conditional cuts. Module plans own execution and outstanding qualification; this roadmap does not duplicate their stage lists or delivery histories.

## Choosing the next task

Prefer an unmet dependency, a useful rendering improvement or a simplification that closes an evidence gap. Choose a bounded outcome and meaningful acceptance before implementation; reuse earlier observations only while their relevant inputs remain valid.

Use the [performance method](Engineering.md#performance-and-images) and [capability goals](Goals.md#technical-development) to choose evidence worth keeping. Analyze available results before adding more diagnostics infrastructure.
