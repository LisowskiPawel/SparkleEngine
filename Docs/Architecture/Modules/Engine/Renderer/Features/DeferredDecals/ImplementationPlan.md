# Deferred GBuffer Decals Delivery Plan

[Docs](../../../../../../README.md) / [Up](DeferredDecalsDesign.md)

Stages: [Phase 0](#phase-0---freeze-contracts-and-evidence) / [Phase 1](#phase-1---authoring-and-scene-ownership) / [Phase 2](#phase-2---rasterized-deferred-decals) / [Phase 3](#phase-3---ray-traced-primary-gbuffer-parity) / [Phase 4](#phase-4---reflections-and-gi-material-hits) / [Phase 5](#phase-5---evidence-preparation-and-cleanup).

Add authored decals that modify the right material surface consistently for raster and ray rendering.

**Status:** implementation plan; not proof of implementation or acceptance

**Related current readiness:** **0/100 — first-release target, Blocked.** No decal data, publication, deferred composition, or ray-hit evaluation implementation was found; release admission and this plan add no implementation credit. See [Current Feature Readiness](../../../../../../Acceptance/CurrentReadiness.md#renderer).

## Work This Plan

Every stage and copied prompt follows [stage completion and resume](../../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-completion-and-resume). Close its finite local exit once; keep later aggregate proof separate.

| What to do | Working route |
| --- | --- |
| First action | [Phase 0 contracts](#phase-0---freeze-contracts-and-evidence): freeze projection/composition meanings, fixture, budgets and actual material consumers. |
| Result to reach | Reviewed semantics and defect-detecting checks before authoring/scene data or a blend pass; then primary and secondary-ray proof. |
| Start / stop | GR-5 and local predecessor gates remain required. A convincing screenshot does not establish ordering, residency or ray-hit correctness. |

## Delivery At A Glance

| Phase | New trustworthy boundary | Dependency that blocks the next phase |
| --- | --- | --- |
| 0. contracts | projection/composition oracle, fixture, budgets, and consumer inventory | no runtime work before semantics and defect-detecting checks are frozen |
| 1. data | authored/cooked/scene identity, lifecycle and zero-content behavior | GPU buffers/bindings wait for their Phase-2 rendering consumer |
| 2. raster | consumed GPU publication and one pre-lighting GBuffer composition path | ray reuse waits for shared functions and primary material correctness |
| 3. ray primary | ray-produced primary GBuffer consumes the same deferred composition | primary parity does not yet claim reflection or GI hits |
| 4. secondary ray hits | reflections and GI reuse projection/material semantics with an admitted arbitrary-hit lookup | unsupported producer rows remain excluded, never silently omitted |
| 5. closeout | backend, overlap, failure, performance, fixture, and deletion evidence | only the feature report can record completion |

This ordering prevents a visually convincing blend pass from hardening the wrong material, ordering, residency, or ray-consumer contract.

## Staged Delivery

Each phase is independently reviewable. A later phase starts only after the preceding exit gate is recorded. Apply the [stage complexity budget](../../../../../../Engineering/Workflow/ChangeLifecycle.md#define-the-outcome-and-complexity-budget): compare actual state/API/hooks/copies/dependencies with the accepted slice at every exit. A phase that replaces an existing or temporary path deletes it in the same change; final evidence verifies those deletions. Every copied prompt inherits its phase's work, exclusions and cleanup exit. Apply the [Stage Source-Style Gate](../../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate) after the final edit, with exact formatter coverage and authored spacing/responsibility review; no-source stages record justified N/A.

### Phase 0 - Freeze contracts and evidence

Freeze documentation and independent oracle cases before runtime decal output:

- freeze projector axes, unit volume, UV orientation, coverage, channel groups, overlap order, receiver bit, and static-only scope;
- design deterministic CPU reference cases for box/facing/edge projection and every composition group; execute disposable local probes only for an unresolved decision, and retain their inputs/results without submitting a new test executable or fixture framework;
- define D3D12/Vulkan image-comparison cameras and timing capture fields;
- record the implementation budget at the accepted resolution instead of inventing a post-hoc threshold;
- audit every GBuffer and ray-hit surface consumer and record migration/deletion obligations;
- define the Sparkle-owned Modern Sponza fixture and provenance.

Exit: contracts are reviewed, oracle inputs are deterministic, workload/cameras and each phase's additions/deletions are named, and local probes are removed. Existing material-table owners and the ineffective binding selector have a retain/narrow/remove disposition; no runtime types, inactive seams or feature claim are delivered.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only Phase 0 from Docs/Architecture/Modules/Engine/Renderer/Features/DeferredDecals/ImplementationPlan.md#phase-0---freeze-contracts-and-evidence. Apply the owning execution contract and its Stage Source-Style Gate.
Task: Freeze contracts and evidence. Complete only its remaining work using the stage requirements and owning checks.
Check prerequisites; reuse completed work and follow repository conventions. Record exact candidate/configuration, commands and exit results. Stop on unresolved contracts or unavailable required checks.
```

</details>

### Phase 1 - Authoring And Scene Ownership

- add the decal ECS/cooked schema using an ordinary material reference;
- extend the existing render-object lifecycle with a mesh-or-decal payload;
- publish only the scene decal records consumed by extraction/lifecycle checks; defer receiver shader fields, GPU allocation/upload, bindings and registrations until Phase 2 consumes them;
- remove the dead binding-mode concept if the Phase 0 audit confirms it has no consumer;
- fail invalid projector transforms, material references, channel masks, and table capacity at the owning publication boundary with source identity;
- add the repository-owned fixture assets and placements, but keep rendering disabled until Phase 2.

Exit: cook/load/extract/create/update/destroy round trips pass through existing owners; no decal GPU allocation or dormant shader state is introduced. Any replaced authoring/extraction carrier and the confirmed ineffective selector are deleted with their consumers/build/docs; ordinary mesh/material lifecycle remains intact. No rendering or ray claim is made.

If a scene contains decals but the required scene material table is unsupported, readiness fails once with the missing capability and source scene identified. There is no silent omission and no forward fallback.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only Phase 1 from Docs/Architecture/Modules/Engine/Renderer/Features/DeferredDecals/ImplementationPlan.md#phase-1---authoring-and-scene-ownership. Apply the owning execution contract and its Stage Source-Style Gate.
Task: Authoring and scene ownership. Complete the consumed source/cook/extraction route and same-slice deletions; defer GPU buffers/bindings to Phase 2.
Check prerequisites; reuse completed work and follow repository conventions. Record exact candidate/configuration, commands and exit results. Stop on unresolved contracts or unavailable required checks.
```

</details>

### Phase 2 - Rasterized deferred decals

- publish the receiver bit and one dirty-range decal GPU table with this first rendering consumer; bind the existing scene material texture table at the consumed shader boundary instead of creating another material resource owner;
- introduce `SurfaceMaterial` and shared projection/composition HLSL;
- preserve current material sampling bindings while deleting duplicated PBR field definitions that the new value actually replaces;
- write/read the receiver bit through the unused normal alpha channel;
- build the per-view active-tile plan with deterministic ordering;
- add one `DeferredDecalResolve` compute pass after the common GBuffer producer join;
- enable and accept the feature first with `GBufferAlgorithm::Rasterized` on D3D12 and Vulkan;
- use existing GBuffer visualization and a single pass marker for inspection.

Exit: raster image tests pass for every channel group, overlap, near-plane/camera-inside volumes, projection edges, jitter, receiver opt-out, and zero-decal identity. The chosen tile plan meets the recorded CPU/GPU/memory budget on the fixture and an overlap stress case. Zero decals allocate/schedule no decal GPU work; replaced PBR field definitions and rejected planners are gone. One consumed table/pass/shared composition owner remains, without a decal manager, parallel material table or permanent debug toggle.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only Phase 2 from Docs/Architecture/Modules/Engine/Renderer/Features/DeferredDecals/ImplementationPlan.md#phase-2---rasterized-deferred-decals. Apply the owning execution contract and its Stage Source-Style Gate.
Task: Rasterized deferred decals. Complete only its remaining work using the stage requirements and owning checks.
Check prerequisites; reuse completed work and follow repository conventions. Record exact candidate/configuration, commands and exit results. Stop on unresolved contracts or unavailable required checks.
```

</details>

### Phase 3 - Ray-traced primary GBuffer parity

- exercise the same post-producer pass with `GBufferAlgorithm::RayTracing` under every automatically selected frontend, using a temporary focused capability harness where required to cover the non-selected adapter;
- reconcile any depth reconstruction or receiver-bit production difference at the producer adapters, not in a ray-specific decal shader;
- compare the raster and ray-traced primary results from the same camera, scene, material table, ordering, and output formats on both backends;
- keep `RayTracingGBuffer` base-hit reconstruction decal-free so primary decals are not applied twice.

Exit: paired primary images agree within the recorded format/tolerance budget, pass ordering is identical, and there is still exactly one primary decal pass. Producer-adapter fixes delete their superseded reconstruction/receiver logic; no ray-specific composition, extra pass/selector or persistent comparison resource remains. Remove the focused harness before handoff.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only Phase 3 from Docs/Architecture/Modules/Engine/Renderer/Features/DeferredDecals/ImplementationPlan.md#phase-3---ray-traced-primary-gbuffer-parity. Apply the owning execution contract and its Stage Source-Style Gate.
Task: Ray-traced primary GBuffer parity. Complete only its remaining work using the stage requirements and owning checks.
Check prerequisites; reuse completed work and follow repository conventions. Record exact candidate/configuration, commands and exit results. Stop on unresolved contracts or unavailable required checks.
```

</details>

### Phase 4 - Reflections and GI material hits

- build stable per-receiver candidate spans from existing receiver world bounds and decal boxes;
- append span fields to the existing ray hit instance payload and publish one candidate-index buffer;
- call the shared projection/sampling/composition functions at the central arbitrary-hit material seam;
- provide one ordinary-ray-material texture footprint/LOD contract and use it for decal samples too;
- classify and migrate all hit-surface consumers; shadow/visibility and primary-GBuffer exclusions stay explicit;
- validate reflected base color/normal/roughness, indirect bounce response, and emissive-at-hit behavior in reference/path and real-time ray modes that consume those material hits;
- record candidate links, candidates tested per shaded hit, rebuild time, buffer high-water, and shader time in the existing evidence route.

Exit: the same authored decal appears on the visible receiver and in an off-screen reflection, changes indirect material response where expected, and matches the CPU projection/composition reference. Candidate spans meet the recorded budget on Modern Sponza and the overlap stress case. Keep one selected candidate lookup and shared material composition; remove replaced hit decoders/lookup experiments and stale payload/buffer bindings in this phase. No per-effect decal tables, general spatial-index framework or unconsumed payload fields survive.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only Phase 4 from Docs/Architecture/Modules/Engine/Renderer/Features/DeferredDecals/ImplementationPlan.md#phase-4---reflections-and-gi-material-hits. Apply the owning execution contract and its Stage Source-Style Gate.
Task: Reflections and GI material hits. Complete only its remaining work using the stage requirements and owning checks.
Check prerequisites; reuse completed work and follow repository conventions. Record exact candidate/configuration, commands and exit results. Stop on unresolved contracts or unavailable required checks.
```

</details>

### Phase 5 - Evidence preparation and cleanup

- run the full raster/ray-primary/ray-secondary matrix on D3D12 and Vulkan;
- capture the fixed Modern Sponza cameras and one adversarial overlap/near-plane scene;
- run existing architecture, shader ABI, formatting, build, and workload checks selected by the touched standards;
- verify the preceding phases already removed replaced structs, planners, flags, payload fields and bindings; remove remaining proof-only guards/probes, and return any production residue to its owner as an explicit repair before freezing a new candidate;
- reconcile the capability snapshot and architecture with proven behavior, record the acceptance result in its owning contract/report, and retire this plan when no active delivery consumer remains.

Exit: the candidate is ready for judgment under [Deferred GBuffer Decals — Acceptance](Acceptance.md), with one authored contract, one runtime decal table, one primary resolve, one shared shader composition path, and one selected secondary lookup. Earlier deletion and actual addition budgets are verified; no test executable, runner, duplicate report or fault API was added for this proof. Unavailable checks and unsupported surfaces remain stated rather than hidden by fallbacks.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only Phase 5 from Docs/Architecture/Modules/Engine/Renderer/Features/DeferredDecals/ImplementationPlan.md#phase-5---evidence-preparation-and-cleanup. Apply the owning execution contract and its Stage Source-Style Gate.
Task: Evidence preparation and cleanup. Complete only its remaining work using the stage requirements and owning checks.
Check prerequisites; reuse completed work and follow repository conventions. Record exact candidate/configuration, commands and exit results. Stop on unresolved contracts or unavailable required checks.
```

</details>

## Reference And Ownership

<details>
<summary>Exact owners, source snapshots and related contracts</summary>

**Scope:** staged delivery of decal authoring, scene/GPU data, deferred composition, ray-hit reuse, and validation

**Architecture authority:** [Deferred GBuffer Decal Composition Architecture](CompositionArchitecture.md)

**Cross-system shader sequence:** [Shader System Delivery Plan](../../../../../CrossModule/ShaderSystem/ImplementationPlan.md)

**Feature acceptance:** [Deferred GBuffer Decals — Acceptance](Acceptance.md)

**First-release owner:** mandatory `FCR-REN-23`; selected by [`GR-5`](../../FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-5--deferred-gbuffer-decals) under `REL-04`

This plan owns feature-local delivery order, dependencies, and phase exit sequence. It does not redefine decal semantics, own the final acceptance criteria, or prove that any phase is complete.

</details>
