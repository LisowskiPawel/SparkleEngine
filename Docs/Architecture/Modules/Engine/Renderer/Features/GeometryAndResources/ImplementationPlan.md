# Materials And Shading Models Delivery Plan

[Docs](../../../../../../README.md) / [Up](ShadingModelsDesign.md)

**Status:** selected target sequence; discovery and prerequisite gates remain open. No stage is executed by this plan.
**Contract:** [design, semantics, measurement and local acceptance](ShadingModelsDesign.md); [Roadmap](../../../../../../Strategy/Roadmap.md) owns global order.

## Work This Plan

MAT-S0/1 qualify the existing release material promise through GR-0/2/4. Anisotropy/clearcoat production follows REL-11 and qualified base/reference inputs. New lobes need an explicit reviewed reference-domain revision before raw-reference claims. Execute the first unfinished stage only. The discovery card freezes numerical tolerances, practical-effect bands, exact eligibility, rights, hook/copy/resource limits and failure checks before production; a source-present mechanism is not a satisfied gate.

## Stage Contract

Every stage inspects HEAD/status, applicable AGENTS.md, live producers/consumers/lifetime/build membership and concurrent edits. Apply [Change Integration](../../../../../../Engineering/Workflow/ChangeIntegration.md), [Change Lifecycle](../../../../../../Engineering/Workflow/ChangeLifecycle.md), relevant Renderer/RHI/Tools standards and the [Stage Source-Style Gate](../../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate), including authored readability/spacing review after formatting. Discovery/no-source work records justified source-style N/A.

Ledger every external feature hook and material copy; default to existing semantic inputs, one feature-local owner, zero new public types and no framework. Each stage removes replaced state/paths/selectors/dependencies in the same slice and checks retained consumers. A zero-deletion stage names why no replaced path exists. Temporary probes stay local and are removed before handoff; keep valuable source/commands/results as ignored evidence.

Each exit records exact candidate/dirty state, API/device/tool/content/shader/SDK/artifact identities, command, oracle, observation and remaining cells in `materials/shading-models/` under the existing workload artifact root. Use the cheapest defect-detecting check; production/native/quality/cost/package/adoption verdicts remain distinct. Stop for missing semantics, required unavailable proof, unsafe lifetime, quality change, silent fallback or a hook outside the frozen budget.

## Correctness Repair Follow-up

The 2026-10-27 fixes are bounded source corrections, not closure of `MAT-S0/1`. [Renderer's finding index](../../RendererDesign.md#pbr-correctness-follow-up) routes the full review. Execute the following base-material work before anisotropy or coat delivery:

1. **MAT-S0: freeze and falsify the complete base model (N04 and model-agreement gap).** Real-time direct diffuse now uses NVIDIA RTXPT's normalized Frostbite formula; Reference retains frozen Lambert/uncorrelated GGX. The roughness-one grazing furnace counterexample is corrected, but that does not prove conservation at every roughness/view/F0 or agreement across direct/indirect/reference routes. Freeze the compared domain and lobe equations, integrate directional-hemispherical reflectance, test reciprocity where required, mixture PDFs and sample means with independent quadrature/replicates. Any remaining energy gain or mismatched target remains a blocker; do not lower albedo or tune exposure to pass. Inventory Beckmann, Oren-Nayar, Chan, Kelemen and Disney-subsurface branches: retire unconsumed branches or qualify their pole/zero-denominator/strength behavior explicitly.
2. **MAT-S1: restore geometric support at the real-time primary surface (N03; open).** Preserve oriented geometric normal separately from the normal-mapped shading normal and derive a defensible primary-position uncertainty. Trace both raster and ray producers through GBuffer, path sampling and ray origins; keep the existing tangent product and the eight-color-attachment budget. Freeze packing/layout/precision and update every producer/consumer together. Falsifiers: the 60-degree normal-map case must reject directions below the actual triangle; test grazing, thin gaps, nonuniform/mirrored transforms, deformation and both frontends/backends. Replacing the denoiser shading normal with a geometric normal is not this fix.
3. **MAT-S1: restore material/coverage lineage (F06; open).** Carry vertex RGBA through raster input/output and reconcile texture UV transforms, per-axis addressing and normal/AO strengths with the RT decoder. Freeze the allowed raster filtering versus Reference base-mip policy, then compare identical-domain known texels and alpha boundaries. Required negatives include vertex-alpha `.25` with cutoff `.5`, transformed/clamped UVs, zero occlusion strength, mirrored UV/tangent handedness and current/previous deformation. Regenerate affected cooked/schema and shader products directly; no legacy decoder or second material path.
4. **MAT-S1: qualify the corrected mechanics (F02, F03, N06).** GGX now has one shared cross-product-based evaluation without denominator peak clipping; delta sampling checks geometric support; raster tangent space is rebuilt before normal mapping and derives its bitangent instead of interpolating one. Test smooth peaks/PDF agreement, delta hemispheres, the two-frame interpolation counterexample and degenerate tangents. Extreme/unrepresentable numeric states still depend on the Reference validity/failure bridge. Exact DXIL/SPIR-V, D3D12/Vulkan and retained content evidence is required before closing these cells.

The module's source corrections leave no claim that the current displayed material is a qualified physical oracle. `AC-MAT-01/02` close only after the above base cells and the linked Reference/lighting/reconstruction obligations pass in their owning reports.

## MAT-S0 Freeze MAT-D0

**Start:** Planning/source inspection.

**Work:** Pin model formulas, import/cook losses, ranges/frame conventions, mixture sampling/PDF, independent numeric/energy/statistical gates and exact layout/hook/copy/cost budget. Inventory actual consumers of dormant BRDF branches.

**Cleanup:** Reject material graph/lobe breadth and classify unused switches for removal.

**Exit:** MAT-D0 accepted with separate base-release and later-extension boundaries.

```text
Execute only MAT-S0 from Docs/Architecture/Modules/Engine/Renderer/Features/GeometryAndResources/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## MAT-S1 Qualify lineage and base scattering

**Start:** MAT-D0 and GR-0/2/4 inputs.

**Work:** Trace authored/cooked/CPU-GPU values into raster/ray and direct/indirect/reference evaluation. Check color/F0/metallic/roughness, normal/tangent/alpha/transforms/deformation. Derive energy/PDF/sample behavior and honest existing subsurface limits.

**Cleanup:** Delete unconsumed BRDF branches, duplicate decoders and obsolete fields only with producer/consumer/ABI/cook reconciliation; preserve accepted reference math.

**Exit:** AC-MAT-01/02 base cells pass in original report owners; compile-time names are not proof.

```text
Execute only MAT-S1 from Docs/Architecture/Modules/Engine/Renderer/Features/GeometryAndResources/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## MAT-S2 Deliver anisotropic GGX

**Start:** REL-11, base and independent reference proof.

**Work:** Add the frozen profile through current material/cook/layout/shared BRDF/path-sampling owners using the existing tangent product. Prove distribution/masking/PDF/Jacobian, sample means/histograms, orientation and isotropic limit.

**Cleanup:** Remove duplicate isotropic/aniso math where semantics agree and rejected parameter/frame records.

**Exit:** AC-MAT-03 and affected lineage/path cells pass on both APIs.

```text
Execute only MAT-S2 from Docs/Architecture/Modules/Engine/Renderer/Features/GeometryAndResources/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## MAT-S3 Deliver one dielectric coat

**Start:** Anisotropy qualified; coat derivation and reference-domain revision approved.

**Work:** Implement attenuated base, coat energy treatment, lobe mixture/PDF and coat-zero identity. Extend only necessary current fields and shared kernels. Independently qualify the new raw Reference material profile.

**Cleanup:** Delete additive coat prototypes, per-effect BRDF forks and unused channels; no arbitrary lobe stack.

**Exit:** AC-MAT-04 and base positive controls pass; prior reference/dataset identities keep their original domain.

```text
Execute only MAT-S3 from Docs/Architecture/Modules/Engine/Renderer/Features/GeometryAndResources/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## MAT-S4 Close cross-path consumers

**Start:** Selected model checks pass.

**Work:** Run raster/ray, static/deformed, mirrored/degenerate tangent, alpha/minification and native API cells. Qualify material guides for each selected denoiser/upscaler. Inject malformed content, stale ABI/descriptors and invalid sample/PDF faults.

**Cleanup:** Remove duplicate guide conversions and unnecessary GBuffer/material products; ledger retained bytes per pixel.

**Exit:** AC-MAT-05 and FM-MAT counterexamples pass; reconstruction cannot hide raw material errors.

```text
Execute only MAT-S4 from Docs/Architecture/Modules/Engine/Renderer/Features/GeometryAndResources/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## MAT-S5 Measure package and reproduce

**Start:** Technical cells and held-out rules frozen.

**Work:** Measure raw/shaded/temporal quality and whole-frame/peak-memory cost. Publish derivation/sampling/content-loss evidence and real cook/shader/package identities; documented author/agent rehearsal reproduces the comparison.

**Cleanup:** Remove probes, unused permutations/dependencies and completed instructions after preserving original evidence.

**Exit:** All AC-MAT-01..06 pass for base, anisotropy and coat; no later result silently changes first-release scores.

```text
Execute only MAT-S5 from Docs/Architecture/Modules/Engine/Renderer/Features/GeometryAndResources/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```
