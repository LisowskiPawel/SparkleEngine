# Renderer Geometry And Ray-Tracing Closure Plan

[Docs](../../../../../README.md) / [Up](README.md) / [Design](../RendererDesign.md)

Make admitted raster and ray paths consume the same resident geometry, material and instance identities, then integrate the bounded required decal feature.

**Status:** remaining release integration and proof; stage names do not imply missing implementation or passed evidence.

## Work This Plan

Enter the stage owning the earliest wrong geometry/material product. GR-0 resolves a missing shared decision; valid residency, raster, TLAS and ray implementations are reused. The required decal feature delegates implementation to its leaf plan rather than prescribing another composition algorithm here.

Run the stage that owns the demonstrated missing contract or failing check. Stage numbers identify responsibilities; they do not require rebuilding existing layers or completing unrelated stages first. Every stage and copied prompt follows the [Stage Source-Style Gate](../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate) and the [shared delivery contract](../../../../CrossModule/FirstRelease/ImplementationPlan.md#phase-card-contract).

## Dependencies

Residency supplies valid resources to raster and ray paths. TLAS/SBT publication precedes ray consumers; raster and ray controls qualify their own results. GR-5 integrates those current consumers after local discovery, without a new geometry database, GPU-driven renderer, LOD/occlusion system or geometry-cache pipeline.

## `GR-0` — Freeze Geometry, Material, And Traversal Semantics

**Start:** The selected geometry/material/traversal failure lacks a shared meaning or oracle.

**Work:** Resolve affected identity/generation, coordinate/tangent/UV/color/alpha/two-sided, skin/morph/motion or ray-hit/SBT cells in current owners. Choose deterministic inputs and independent/raw checks for admitted raster, inline and native-pipeline routes.

**Cleanup:** Remove duplicate semantic definitions and obsolete traversal/material claims rather than create a general scene framework.

**Exit:** The cell has one definition, lifetime, capability disposition and falsifying check; raster appearance alone is not its ray oracle.

```text
Execute only GR-0 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-0--freeze-geometry-material-and-traversal-semantics. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Resolve affected identity/generation, coordinate/tangent/UV/color/alpha/two-sided, skin/morph/motion or ray-hit/SBT cells in current owners.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to the stage's existing report owner; no neighboring stage or new mechanism merely to fill a report.
```

## `GR-1` — Mesh And Texture Residency

**Start:** Required content exists; activation/upload/replacement/retirement or residency proof is the missing edge.

**Work:** Exercise immutable generations, decode/staging/upload tokens, transactional activation, cancellation, reload/eviction and all-queue retirement. Preserve owned exact/over-budget, 16-job and 256-backlog boundary checks and changed CPU/GPU memory high-water.

**Cleanup:** Remove redundant residency-ready state, copied asset truth and replaced staging/activation records. Required missing content fails visibly; a placeholder cannot claim that content loaded.

**Exit:** FCR-REN-17 applicable limit, stale-work/failure/cancellation and memory/retirement checks pass; no speculative streaming or virtual-texturing tier is added.

```text
Execute only GR-1 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-1--mesh-and-texture-residency. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Exercise immutable generations, decode/staging/upload tokens, transactional activation, cancellation, reload/eviction and all-queue retirement.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-17; no neighboring stage or new mechanism merely to fill a report.
```

## `GR-2` — Visibility, Draw Preparation, And Raster GBuffer

**Start:** Resident admitted inputs are valid and a visibility/draw/GBuffer cell needs proof or repair.

**Work:** Check frustum/bounds/identity, deterministic opaque grouping/sorting/batching, retained transparent ordering, task failure/cancellation, depth/material/normal/motion channels, alpha/two-sided and deformation. Compare serial/parallel prepared work and native output; source transparency metadata does not itself prove a supported transparency renderer.

**Cleanup:** Delete duplicate prepared-draw/batching carriers, repeated material decode and invalid draw-producing fallbacks replaced by the repair.

**Exit:** FCR-REN-21/FCR-REN-04 pass admitted visibility, draw and raw GBuffer cases on both APIs; no absent LOD/occlusion/mesh-shader capability is inferred.

```text
Execute only GR-2 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-2--visibility-draw-preparation-and-raster-gbuffer. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Check frustum/bounds/identity, deterministic opaque grouping/sorting/batching, retained transparent ordering, task failure/cancellation, depth/material/normal/motion channels, alpha/two-sided and deformation.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-21, FCR-REN-04; no neighboring stage or new mechanism merely to fill a report.
```

## `GR-3` — Renderer TLAS Policy And Publication

**Start:** Scene/resource generations are valid; AS update, mapping, selection or lifetime is the failing edge.

**Work:** Repair current classic build/refit and instance/contribution identity, topology-change detection, scratch/result/operation bounds, queue completion, publication and retirement. Check move/delete/reload and required allocation failure. PTLAS keeps its separately experimental disposition; it cannot become the release baseline.

**Cleanup:** Remove duplicate AS plans, contribution mappings and stale update/publication records; scene policy remains Renderer-owned and native execution RHI-owned.

**Exit:** FCR-REN-12 admitted classic/refit cells pass native lifetime and cost checks; experimental PTLAS has a separate result, not an all-hardware promise.

```text
Execute only GR-3 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-3--renderer-tlas-policy-and-publication. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Repair current classic build/refit and instance/contribution identity, topology-change detection, scratch/result/operation bounds, queue completion, publication and retirement.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-12; no neighboring stage or new mechanism merely to fill a report.
```

## `GR-4` — Ray-Traced GBuffer

**Start:** Valid TLAS/SBT/material inputs and current shader/native contracts are available.

**Work:** Prove hit/miss/instance/primitive/material and GBuffer correspondence for admitted automatic/inline/native routes, with alpha/two-sided, skin/morph and motion cases. Use the shader acceptance owner's frozen [GBuffer comparison](../../../../CrossModule/ShaderSystem/Acceptance.md#same-frame-gbuffer-comparison) and native lifetime checks.

**Cleanup:** Delete route-specific material/hit decoding, duplicate effect implementations and obsolete bindings replaced by the fix; keep one selected producer per frame.

**Exit:** FCR-REN-05 meets owned raw tolerances, selection/rejection and native/cost matrices. No dual-output production mode or second material authority.

```text
Execute only GR-4 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-4--ray-traced-gbuffer. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Prove hit/miss/instance/primitive/material and GBuffer correspondence for admitted automatic/inline/native routes, with alpha/two-sided, skin/morph and motion cases.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-05; no neighboring stage or new mechanism merely to fill a report.
```

## `GR-5` — Deferred GBuffer Decals

**Start:** Local decal semantics/discovery and current raster/ray material consumers are qualified.

**Work:** Execute the next [decal stage](../Features/DeferredDecals/ImplementationPlan.md) and [AC-DECAL-01 through 08](../Features/DeferredDecals/Acceptance.md). The leaf owns projection, tile/lookup strategy and composition; this parent adds no inside/outside volume rasterization or second blend path. Preserve authored/cooked identity, zero-decal behavior, order/channels, raw/temporal and secondary-ray proof.

**Cleanup:** Reuse the existing material texture-table capability owner and preserve the real fixed-capacity table and descriptor checks. Do not reintroduce a binding-mode selector or report-only mode/mask state; keep decal-specific policy inside its feature home.

**Exit:** FCR-REN-23 passes admitted authoring/material/overlap/overflow/lifecycle/backend/package/cost checks; replaced PBR/composition data and temporary producers are gone. Static projected decals do not admit dynamic/mesh/transparent decal systems.

```text
Execute only GR-5 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-5--deferred-gbuffer-decals. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Execute the next [decal stage](../Features/DeferredDecals/ImplementationPlan.md) and [AC-DECAL-01 through 08](../Features/DeferredDecals/Acceptance.md).
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-23; no neighboring stage or new mechanism merely to fill a report.
```

## `GR-6` — Geometry, Ray, And Decal Candidate Closure

**Start:** Admitted geometry, residency, traversal and decal results use compatible candidate inputs.

**Work:** Join the six reports and fill only missing raster/ray/material, reload/soak, lifetime or package checks on the release maps. Keep raw identity/channel comparisons distinct from post-processed images.

**Cleanup:** Verify previous deletions and stale source/shader/registration/build references; retire completed instructions after preserving proof.

**Exit:** FCR-REN-04/05/12/17/21/23 have exact candidate decisions and required negatives. Unavailable Included cells stay blocked; unselected capability creates no implementation task.

```text
Execute only GR-6 from Docs/Architecture/Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-6--geometry-ray-and-decal-candidate-closure. Apply the linked Phase Card Contract, including the Stage Source-Style Gate.
Selected work: Join the six reports and fill only missing raster/ray/material, reload/soak, lifetime or package checks on the release maps.
Verify the stage's Start condition against current owners and exact prerequisite artifacts. Execute its full Work, Cleanup and Exit under the accepted scope; preserve the selected frame/product and reuse valid code/proof. Stop on a missing contract, unsafe lifetime, silent fallback, changed quality/identity or a required unavailable falsifier.
NON-NEGOTIABLE: one producer/consumer authority; actual artifact/API/profile/settings identity; no unledgered state/API/hooks/copies/dependencies; same-stage replacement deletion and positive retained-path checks. A no-source stage records justified source-style N/A. Handoff exact checks/artifacts/limits to FCR-REN-04; no neighboring stage or new mechanism merely to fill a report.
```

## Reference And Ownership

[Geometry/resources](../Features/GeometryAndResources/GeometryAndResourcesDesign.md), [ray tracing](../Features/RayTracing/RayTracingDesign.md), [native RT](../../RHI/Features/PipelineAndExecution/PipelineAndExecutionDesign.md#ray-tracing) and [decals](../Features/DeferredDecals/DeferredDecalsDesign.md) own contracts. This plan owns the six release-family assignments only.
