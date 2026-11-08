# Goals

[Docs](README.md) · [Next work](Roadmap.md)

Build a compact rendering demonstration that proves deep graphics engineering: take a problem from mathematics through C++, shaders and native APIs to a measured, reproducible result. Engine breadth serves that result.

## Product boundary

- Windows showcase with Launcher, Editor and Game; D3D12 and Vulkan have separate proof obligations.
- Launcher is the polished front door for scene selection, readiness, Build/Cook/Run, progress, cancellation and recovery.
- Game presents rendering and camera input. Editor provides inspection, supported edits and developer controls.
- Target distribution: one fresh prebuilt bundle with admitted cooked content. Matching source may reuse that base and replace it through the existing build/cook path. Public archive qualification and redistribution rights remain open.
- Supporting Core, Tasks, world, content and tools work stops when its selected rendering or reproduction consumer works.

The [level catalog](../Projects/Showcase/Levels.catalog) names release-supported entries: ModernSponza, Sponza, ABeautifulGame, LPSHead, CornellBox, DiffuseTransmissionPlant and CesiumMan, plus Empty. Catalog admission does not grant asset redistribution rights. [Build and delivery](Architecture/BuildAndRun.md#release-inputs) owns that distinction.

After release, Sponza remains the fast regression scene; Bistro exterior/interior is the flagship, with San Miguel 2.0 providing independent scene breadth. Sponza-only results cannot satisfy those later targets.

## Engineering outcomes

| Outcome | What a useful change demonstrates |
| --- | --- |
| `NS-REAL` | A current consumer receives the complete promised result. |
| `NS-MATH-DATA` | Mathematics, units, data transformations and numerical assumptions are understood. |
| `NS-EVIDENCE` | Claims have reproducible checks on the declared matrix. |
| `NS-OWNERSHIP` | State, lifetime, publication, failure and retirement have explicit owners. |
| `NS-ADOPTION` | Someone can discover, use and diagnose the result without private repair. |
| `NS-SIMPLIFY` | Replaced code, duplicate truth and unnecessary maintenance disappear in the same change. |

## Capability targets

These stable `PGE-*` identifiers name professional goals, not awarded qualifications. Several targets should share one case study.

| ID | Target and required evidence | Working reference |
| --- | --- | --- |
| `PGE-01` | Integrate technology under another consumer's constraints; retain feedback, interventions and a reproduction/adoption result. | [Portfolio](Architecture/Portfolio.md) |
| `PGE-02` | Explain and prove ray tracing, GI and path transport: independent references, temporal quality, cost, memory and paired APIs on Bistro/San Miguel. | [Lighting](Architecture/Lighting.md), [Reference](Architecture/ReferencePathTracer.md) |
| `PGE-03` | Deliver one trained neural rendering feature with real inference, provenance, classical control and held-out quality/cost/failure evidence. | [Neural graphics](Architecture/NeuralGraphics.md) |
| `PGE-04` | Translate a fixed model into checked GPU kernels; derive operators/layouts and measure a precision, layout or fusion choice. | [Neural graphics](Architecture/NeuralGraphics.md#model-and-numerical-contract) |
| `PGE-05` | Measure whole-system performance: warm-up policy, p50/p95/p99, CPU/GPU timelines, memory high-water and causal before/after controls. | [Performance studies](Architecture/PerformanceStudies.md) |
| `PGE-06` | Diagnose one difficult graphics incident with native artifacts, competing hypotheses, reduced reproduction, root cause and bounded fix. | [Diagnostics](Architecture/Diagnostics.md), [studies](Architecture/PerformanceStudies.md) |
| `PGE-07` | Show cohesive modern C++, useful Python automation, narrow APIs, a clean build and proportionate correctness checks. | [Engineering](Engineering/README.md), [studies](Architecture/PerformanceStudies.md) |
| `PGE-08` | Derive the actual estimator/model and units; compare known values or an independent numerical control with predicted behavior and uncertainty. | [Reference](Architecture/ReferencePathTracer.md), [studies](Architecture/PerformanceStudies.md) |
| `PGE-09` | Trace source through compiler, reflection, cooked bytecode and native execution on both APIs; explain bindings, synchronization and failures. | [Shaders](Architecture/Shaders.md), [RHI](Architecture/RHI.md) |
| `PGE-10` | Explain CPU/GPU scheduling, cache/bandwidth, occupancy or synchronization using counters/disassembly, a serial control and measured product effects. | [Performance studies](Architecture/PerformanceStudies.md) |
| `PGE-11` | Own a reproducible ML experiment: separated data, loss/metric rationale, baseline, ablations, leakage and overfit controls. | [Neural graphics](Architecture/NeuralGraphics.md) |
| `PGE-12` | Profile training and inference separately; identify export, precision/layout, startup, workspace and latency tails. | [Neural graphics](Architecture/NeuralGraphics.md) |
| `PGE-13` | Communicate three concise cases with actual code/captures, one useful tool and a justified productization or deletion decision. | [Portfolio](Architecture/Portfolio.md) |
| `PGE-14` | Reproduce the Windows/toolchain/paired-API workflow and explain its platform/hardware limits. Native Linux is outside this delivery program. | [Build and run](Architecture/BuildAndRun.md) |
| `PGE-15` | Demonstrate repeated delivery, consequential direction, challenged decisions, second-context transfer and feedback-driven revision. | [Portfolio](Architecture/Portfolio.md#review-and-transfer) |

Evidence levels remain: `E0` absent, `E1` claimed/scaffolded, `E2` implemented, `E3` verified reproducibly, `E4` transferred through actual adoption/publication/review. Aim for E3 across technical core targets and E4 on several transfer/communication targets. Document volume, SDK integration and feature counts award no level. Employment, degrees, tenure and organizational leadership require their own truthful professional evidence.

## Scope protection

Admit work only for a named result and consumer. Prefer an existing path, a bounded repair or deletion to a new framework. A target here is not permission to implement it ahead of the [roadmap](Roadmap.md).

Selected later depth: raw direct/indirect transport; base, anisotropic and coated materials; SR/RR and one RELAX path; PTLAS; bounded volumetrics; SHaRC fully completed before NRC; NTC; one owned diffuse-indirect model. Preserve raw outputs, uncached transport, BCn and TLAS as necessary controls.

Unselected: general gameplay/physics/network/audio/scripting platforms, stable plugin SDK, installer/updater, native Linux, extra hardware campaigns, geometry-cache animation, frame generation, meshlets/DGC/SER, physical atmosphere/clouds/imported volumes, broad Editor/diagnostics dashboards and general ML runtimes. Reconsider one only for a specific consumer and measurable benefit.
