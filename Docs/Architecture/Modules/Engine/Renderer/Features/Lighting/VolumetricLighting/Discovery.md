# Volumetric Lighting, Fog, Atmosphere, And Sky Discovery Gate

[Docs](../../../../../../../README.md) / [Up](VolumetricLightingDesign.md) / [Implementation](ImplementationPlan.md)

**Gate:** `VOL-D0`

**Status:** selected Q0/Q1/Q2 discovery open; production blocked by REL-11 and consumed decisions. Research prepares this bounded contract without implementing unselected tiers.

**Responsibility:** own roadmap/product, medium, content, reference, grid, lighting, atmosphere, reservoir, cloud, budget, evidence, and authorization decisions

**Authority boundary:** strategy/roadmap owns admission; research informs this page; this page freezes `VOL-D0`; semantics/architecture describe the target; the Plan cannot authorize itself; code and feature-local VOL-PF-01 own implementation/results

**Prepared:** 2026-09-12 at revision `8e4ffba225411965dc51c0b783e5f47a075c7e84`

## Gate At A Glance

| Question | Current answer | Required closure |
| --- | --- | --- |
| Is any volume feature implemented? | no; readiness remains 0/100 | preserve the negative claim until an admitted end-to-end slice exists |
| What ships first? | selected Q0 reference, Q1 homogeneous/height fog, then Q2 bounded local-media froxels/all analytic lights | close consumed VOL-D0 and execute VOL-S0..5 |
| Is Volumetric ReSTIR the base? | no | reference transport and froxel baseline precede its admission |
| Does existing Sky become atmosphere? | no | define an explicit image-sky versus physical-atmosphere mode and environment bridge |
| Are clouds/OpenVDB first scope? | no | later content-dependent gates |

## Iteration Control Record

| Field | Value |
| --- | --- |
| ID | `ITER-REN-VOL-00` |
| North Stars | `NS-REAL`, `NS-MATH-DATA`, `NS-EVIDENCE`, `NS-OWNERSHIP`, `NS-ADOPTION`, `NS-SIMPLIFY` |
| Graphics targets | `PGE-02`, `PGE-05` through `PGE-10`, `PGE-13`, `PGE-15` |
| Delivery target | selected post-REL-11 Q0/Q1/Q2 with exact contracts and VOL-PF-01 evidence owner |
| Principal risks | premature scope; nonphysical fog shortcut; composition error; temporal trails; duplicated sky/light/media state; ReSTIR without reference; volume asset pipeline bloat |
| Completion claim | documentation/research only; no capability/readiness increase |

## Required Decisions

### `VOL-D0-01` Roadmap, Product, And Platform Admission

M5R has selected Q0/Q1/Q2. Freeze their actual API/device, reachability and Required/Deferred/Excluded cells after release; bind VOL-PF-01 to the Renderer implementer/evidence owner. Local analytic media and temporal reconstruction are required; physical atmosphere, imported-volume assets, reservoir reuse and clouds are Deferred without experiments.

### `VOL-D0-02` World, Units, And Medium Semantics

Freeze metre-based world scale, `sigma_a`, `sigma_s`, `sigma_t`, albedo, emission units/basis, density multiplier, phase parameters, wavelength/RGB approximation, valid ranges, and scale behavior. Define friendly authoring conversions without creating a second renderer representation.

### `VOL-D0-03` Global, Height, And Local Media

Freeze global homogeneous/exponential height model, atmosphere-relative height/ground, local primitive shapes/transforms/falloff, overlap/blend/priority, capacity/overflow, and dynamic update identity. Decide whether local primitives are sphere/box only in the first unified-froxel slice.

### `VOL-D0-04` Content And Cooking

Freeze the existing scene serialization/cook/preparation path for global coefficients and bounded analytic box/sphere density fields, including transform/range/overlap/capacity, identity, reload/retirement and malformed/missing input behavior. No dense/sparse texture asset or VDB import is selected; do not create its schema, dependencies or pipeline.

### `VOL-D0-05` Physical Reference And Analytic Oracles

Freeze Beer slab, homogeneous single scatter, phase normalization, height-density quadrature, and composition hand cases. Choose a CPU/reference volume integrator and an independent external/analytic cross-check. The surface Reference Path Tracer does not automatically support media and cannot be labeled the volume oracle without an admitted extension.

### `VOL-D0-06` Froxel Layout And Injection

Freeze view-space mapping, XY/Z resolution, linear/log/cascade depth, near/far behavior, jitter, world reconstruction, coefficient/density representation, local-volume raster/compute injection, overlap resolve, precision, clear/default state, and resize/multiview ownership.

### `VOL-D0-07` Direct Light And Shadow Injection

Freeze supported lights, selection/list/distribution, phase evaluation, light and view transmittance, geometry shadow provider, volume self-shadow/transmittance, per-froxel/pixel sampling, and light/medium generations. Use deterministic small-light evaluation as the independent baseline for all analytic types. Add only the bounded list/selection work needed for selected scenes; clustered/reservoir studies need a separately measured problem.

### `VOL-D0-08` Integration And Composition

Freeze numerical integration, premultiplied in-scatter/transmittance format, opaque depth, sky/far boundary, surface equation, emission, pre-exposure, upsampling, transparency/transmission exclusions, pass order, and debug/raw capture. Prove `Lout=T*Lsurface+Lscatter` once.

### `VOL-D0-09` Temporal Reconstruction

Freeze sample jitter, reprojection domain/velocity, previous froxel or screen history, depth/medium boundary checks, confidence/responsive masks, neighborhood clamp, maximum accumulation, cuts/resize/dual-view, density/light/emission/atmosphere changes, and first-frame state.

### `VOL-D0-10` Atmosphere And Sky

**Deferred; unselected tier.** This section is future reference and is not a consumed Q0/Q1/Q2 gate.

Freeze planet/ground transform, Rayleigh/Mie/absorption profiles and phase, solar irradiance/disk, transmittance/multi-scatter/sky-view/aerial-perspective LUT parameterization, precision, generation/cache, background mode, atmosphere-light limit, altitude/space boundary, and interaction with image-based `SceneSkyDesc`.

### `VOL-D0-11` Environment Bridge

**Deferred; unselected tier.** This section is future reference and is not a consumed Q0/Q1/Q2 gate.

Freeze how image sky or physical atmosphere publishes one immutable background/environment radiance generation, directional PDF, rotation/time identity, and update transaction to Direct/Indirect Lighting. Define which owner samples surface environment and which integrates camera-ray atmosphere so neither is double counted.

### `VOL-D0-12` Heterogeneous Tracking And Acceleration

For selected varying-density Q2 fields, freeze independent quadrature/fixed-step and bounded ratio/delta-tracking oracle, valid majorant/control field, density/transform changes, numeric limits and majorant-violation rejection. Keep reference probes offline/local with retained source and commands. Dense/sparse asset lookup, streaming, LOD and acceleration structures are unselected; no general volume integrator enters runtime.

### `VOL-D0-13` Volumetric ReSTIR Domain

**Deferred; unselected tier.** This section is future reference and is not a consumed Q0/Q1/Q2 gate.

Freeze whether the first reservoir use is per-froxel many-light injection, single-scatter camera paths, or multi-scatter path-space reuse. Specify path sample, free-flight/transmittance proposal, target, generalized contribution weight, shift/inverse/Jacobian/support, approximate candidate evaluation, unbiased/controlled final evaluation, correlation/bias mode, history identity, and reference tests.

### `VOL-D0-14` Clouds

**Deferred; unselected tier.** This section is future reference and is not a consumed Q0/Q1/Q2 gate.

Freeze separate admission only after atmosphere/heterogeneous foundations: weather/density authoring, procedural detail, bounds/LOD/empty-space skipping, direct/multiple-scatter approximation, ground/cloud shadows, atmosphere composition, temporal reconstruction, and close/fly-through acceptance. Keep clouds excluded from the first implementation plan unless explicitly promoted.

### `VOL-D0-15` Quality, Memory, Time, And Failure Budgets

Measure candidate layouts before freezing. Record render/upscale resolution, froxel dimensions/formats, light/volume capacities, steps/tracking events/rays/reservoir candidates, LUT sizes/update cost, history/replacement overlap, peak memory, queue placement, GPU time distribution, and quality metrics for clear air through extreme optical depth.

### `VOL-D0-16` Ownership, Adoption, Diagnostics, And Evidence

Freeze one feature home, authored/cooked/scene/GPU/frame/RHI integration hooks, requested/active modes and reason codes, default/quality profiles, raw diagnostics through existing infrastructure, acceptance fixtures/artifact schema/thresholds, D3D12/Vulkan cells, and the candidate report ID. No feature dashboard, generic volume framework, or speculative public type.

## Decision Closure Register

| Decision | Options that must be compared | Required retained evidence | Decision owner/reviewer | Status | Consequence while open |
| --- | --- | --- | --- | --- | --- |
| `VOL-D0-01` | selected Q0/Q1/Q2 versus excluded atmosphere/imported volumes/reuse/clouds | REL-11 closeout, M5R selection and VOL-PF-01 owner | product/Renderer evidence owner | Scope selected; release gate open | no production before prerequisites |
| `VOL-D0-02` | scalar/RGB coefficient and photometric conversion policies | dimensional derivation, coefficient/scale/finite hand cases | volume semantics / math reviewer | Open | no medium record can publish |
| `VOL-D0-03` | global/height/local shapes and additive/priority/blend overlap | field/boundary/overlap/capacity/authoring study | content + volume owners / artist reviewer | Open | only smallest ratified global field may enter Stage 1 |
| `VOL-D0-04` | bounded analytic local density fields; imported dense/sparse formats deferred | units/bounds/overlap/density-frequency oracle and byte/capacity proof | Renderer/content owner | Scope selected; representation open | no volume importer/format dependency |
| `VOL-D0-05` | analytic, CPU/high precision, PBRT/tracker and external sky reference | oracle manifest, shared-code analysis, tolerances | verification / independent renderer reviewer | Open | no physical-quality verdict |
| `VOL-D0-06` | grid/depth/layout/format/representation/integration variants | forward/inverse tests and error/bandwidth/time/memory sweeps | froxel owner / backend reviewer | Open | no persistent production grid contract |
| `VOL-D0-07` | all-light/clustered/stochastic/reservoir plus visibility/transmittance | one/many-light raw source tests and scale curve | volume-light owner / Direct/RHI reviewers | Open | first tier limited to admitted light path |
| `VOL-D0-08` | segment integration, sky/surface/transparency ordering and early termination | analytic tuples, opaque/miss/depth/order captures | composition owner / lighting reviewer | Open | no scene-color composition hook |
| `VOL-D0-09` | raw/source/integrated history, reprojection/clamp/confidence | full camera/density/light/extent/dual-view matrix | temporal owner / motion reviewer | Open | no history or product stability claim |
| `VOL-D0-10` | Hillaire/Bruneton/direct-march LUT scope, atmosphere model and sky mode | raw LUT/query reference matrix and atomic generation failure | atmosphere owner / independent sky reviewer | Deferred; unselected tier | physical atmosphere remains absent |
| `VOL-D0-11` | background/sun/aerial/direct/indirect environment generation roles | mapping/radiance/PDF/generation and double-count tests | environment + lighting owners / cross-feature reviewer | Deferred; unselected tier | no physical-sky bridge |
| `VOL-D0-12` | fixed march and delta/ratio/residual tracking plus acceleration | majorant/reference statistics and density-frequency/empty-space A/B | heterogeneous owner / estimator reviewer | Open | no heterogeneous transport claim |
| `VOL-D0-13` | froxel-light reservoir versus camera/path-space ReSTIR domains | exact sample/shift/weight/final estimator and equal-time reference study | volume estimator / GRIS reviewer | Deferred; unselected tier | no reservoir code or ReSTIR label |
| `VOL-D0-14` | clouds excluded, deferred or separately admitted with exact scope | content/authoring/weather/lighting/temporal/workload budget | product/content owner / artist reviewer | Deferred | no cloud type, setting, pass or claim |
| `VOL-D0-15` | profile dimensions, samples/steps/rays, queues, time/memory and failure budgets | local target-device quality/time/memory/replacement curves | performance owner / platform reviewers | Open | no default profile or performance promise |
| `VOL-D0-16` | feature home, hooks, authoring/status/debug/automation and FCR schema | [UX](UserExperience.md) dry run, hook/removal ledger, candidate/check review | architecture/editor/verification owners | Open | no public workflow or implementation stage |

## Gate Resolution Protocol

The release owner first closes admission. Each admitted row then fixes options, fixtures, metrics and thresholds before a bounded prototype or reference run; retains exact source/content/build/backend/dirty identity and raw artifacts; records explicit accepted/rejected/deferred/excluded reasons; completes rights/provenance review where assets/code are involved; obtains the named separate review; and updates the single semantic/architecture/UX owner. Missing admission, reference, content rights, capability or target hardware yields `Blocked`. A paper or another engine cannot close a local row.

## Risk Register

| Risk | Leading indicator | Containment | Escalation trigger |
| --- | --- | --- | --- |
| cosmetic fog ships without physical contract | depth-color blend appears before coefficient/Beer tests | Stage 1 analytic medium and composition oracle | no inverse-metre representation or raw `T/Lscatter` |
| fog, atmosphere, sky and clouds become parallel systems | multiple environment generations/composition writes | one feature enclosure and environment bridge | second sky/background or medium state owner appears |
| froxel temporal stability hides integration bias | smooth result changes with grid/step in raw mean | current-only convergence plus separate history verdict | decreasing step/grid does not approach reference |
| majorant/content errors are clamped away | violations disappear but energy changes | terminal counters and invalid publication | any density exceeds claimed majorant in accepted run |
| Volumetric ReSTIR is only light selection | sample lacks volume path/free-flight/final estimator | exact domain naming and estimator ledger | product label exceeds stored/resampled object |
| external timings become Sparkle budget | plan/profile contains source hardware numbers | local workload/device measurement required | no Sparkle candidate measurement exists |
| content source lacks durable rights/schema | test asset cannot be redistributed or recooked | provenance manifest and synthetic fallback fixture | acceptance depends on inaccessible asset |

## Proposed Admission Matrix

| Capability | Selected disposition | Dependency |
| --- | --- | --- |
| coefficient/phase/analytic and independent varying-density oracle | Required Q0 | REL-11, numeric contract and frozen oracle |
| homogeneous/exponential height fog | Required Q1 | physical coefficients and authored-to-frame flow |
| one directional-light single scattering and exact composition | Required Q1 vertical slice | existing Direct light/visibility and scene-linear depth order |
| bounded analytic box/sphere local media and overlap | Required Q2 | units/transforms/density/capacity and independent oracle |
| all analytic light types with shadows/medium transmittance | Required Q2 | exhaustive small-light truth and current light identity |
| froxel integration and temporal reconstruction | Required Q2 | raw reference, motion/reset/disocclusion and cost gates |
| atmosphere/aerial perspective and physical-sky bridge | Unselected | no LUT or new sky dependency |
| dense/sparse imported volumes and OpenVDB/NanoVDB | Unselected | no importer/format/runtime streaming dependency |
| per-froxel/path-space reservoirs and multiple scattering | Unselected | no reuse/GRIS implementation in VOL-S0..5 |
| clouds/weather and transparent-surface medium nesting | Unselected | no authoring/weather/transmission suite |

## Required Experiments

| ID | Selected exercise or explicit deferral | Decision |
| --- | --- | --- |
| VOL-X-01 | Required: independent CPU/shader Beer slab, phase normalization, height integral and vacuum/composition | 02/05 |
| VOL-X-02 | Required: bounded froxel mapping, near/far/depth/thin shaft, grid/step and camera quality/cost sweep | 06/09/15 |
| VOL-X-03 | Required: exhaustive single/few-light fixtures for every analytic type and admitted scene light counts; no automatic thousands-light/reuse track | 07/15 |
| VOL-X-04 | Required: opaque surface, image sky and volume depth/order with raw transmittance/in-scatter | 08 |
| VOL-X-05 | Deferred: physical-sky/aerial-perspective models | unselected 10/11 |
| VOL-X-06 | Required: analytic varying-density field fixed-step/quadrature versus independent tracking, valid/violated majorant | 03/05/12/15 |
| VOL-X-07 | Deferred: volumetric reservoir/path-space reuse | unselected 13 |
| VOL-X-08 | Required: density/light/camera/extent/cut/two-View temporal sequence | 09/15 |
| VOL-X-09 | Required: current scene serialization/cook/load/reload, malformed/missing field and capacity workflow | 03/04/16 |

## Discovery Evidence Package

1. approved post-`REL-11` scope, owner, FCR ID, stage budget, and exact candidate boundary;
2. medium/phase/units and analytic/reference results;
3. authored/cooked/scene/GPU data schema and lifetime experiment;
4. froxel mapping/format/resolution/memory/quality study;
5. light/shadow injection and many-light comparison;
6. integration/composition proof with raw transmittance/in-scatter;
7. temporal reconstruction mutation results;
8. atmosphere/LUT/environment comparison and bridge design;
9. heterogeneous asset/tracking/majorant study;
10. Volumetric ReSTIR path/estimator experiment and explicit admission/rejection;
11. exact integration-hook and clean-break ledger, revised estimates, thresholds, backend cells, and separate review.

## Discovery Failure Modes

| ID | Failure | Gate response |
| --- | --- | --- |
| `FM-VOL-D0-01` | work begins before `REL-11` unlock/admission | stop production; retain research only |
| `FM-VOL-D0-02` | a fog screenshot substitutes for coefficient/composition proof | remain open; require raw analytic artifacts |
| `FM-VOL-D0-03` | image sky is mutated into atmosphere without mode/migration decision | reject ownership shape |
| `FM-VOL-D0-04` | Volumetric ReSTIR is selected before reference/tracking baseline | remain open; execute `VOL-X-06/07` |
| `FM-VOL-D0-05` | clouds/VDB/transparency inflate first slice | defer through admission matrix |
| `FM-VOL-D0-06` | source grid/step/timing becomes Sparkle default | measure locally and freeze budget |
| `FM-VOL-D0-07` | separate fog/atmosphere/cloud state or duplicate light store proposed | reject and redesign one medium/lighting boundary |
| `FM-VOL-D0-08` | no supported backend/content fixture/evidence owner exists | mark blocked; do not claim partial product readiness |

## Exit Criteria And Decision

`VOL-D0` passes only after REL-11 unlocks selected Q0/Q1/Q2, VOL-PF-01 is bound to its actual candidate/evidence owner, decisions and experiments consumed by the selected tier close while unselected cells remain explicitly Deferred, first-stage hooks/checks/budgets are reviewed, and negative-capability claims are updated in the same admitted change.

**Current decision: Blocked by roadmap; discovery open.** No production code is authorized by this package, and Volumetric Lighting remains 0/100.
