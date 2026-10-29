# Materials And Shading Model Depth

[Docs](../../../../../../README.md) / [Up](GeometryAndResourcesDesign.md) / [Implementation](ImplementationPlan.md)

**Status:** selected target contract; `MAT-D0` open. The [current material/GBuffer dossier](GeometryMaterialsAndGBuffer.md) owns existing behavior.
**Inspected:** 2026-10-25, source baseline `154c6e883e51689f12feab95646795cc7cbfcdd1`: shared material decoding and BRDF diffuse/specular/subsurface helpers exist. Helpers, switches and screenshots do not qualify their physics, sampling or cross-path agreement.

## Required Portfolio Result

An engineer can follow authored material values through cook, GPU layout, raster/ray surface evaluation, direct/indirect lighting and a bounded independent raw reference. Demonstrate metal/rough dielectric/conductor behavior, diffuse-model limits, normal/tangent correctness, the existing direct subsurface approximation, and two later extensions: anisotropic GGX and one dielectric clearcoat layer. This serves `PGE-02/06..10/13`; material depth is an end-to-end scattering and evidence obligation.

Release retains its existing opaque/alpha-tested base material promise. Anisotropy/clearcoat are selected later portfolio work, with explicit reference-domain revisions and new proof before any claims. No transmission/glass/caustics, hair/fur, cloth suite, multilayer material graph, spectral renderer, arbitrary lobe stack or new authoring product is implied. The existing wrap/subsurface approximation is not volumetric transport or a BSSRDF.

## Model And Domain Contract

The 2026-10-27 correctness repair selects NVIDIA RTXPT's energy-normalized Frostbite diffuse formula for the existing real-time direct branch, replacing unnormalized Burley. This is a bounded correction of the measured roughness-one energy gain, not acceptance of the complete material model: `MAT-S0/1` must still close the roughness/view/F0 furnace matrix, direct-versus-reference model differences, shading-normal support and every retained optional branch. The frozen Reference Lambert/GGX target is unchanged. The implementation precedent is [RTXPT BxDF](https://github.com/NVIDIA-RTX/RTXPT/blob/303b408ab81790a5051abd7573830b7a4112eb8e/Rtxpt/Shaders/PathTracer/BSDFs/BxDF.hlsli), with that revision's Frostbite default; no NVIDIA implementation is treated as an independent oracle by association.

| Profile | Required reasoning and checks |
| --- | --- |
| `MAT-BASE` | Pin actual Lambert/Frostbite/Oren-Nayar and GGX/geometry/Fresnel branches used by supported shaders; derive parameter/range/roughness mapping, lobe weights and energy behavior rather than certify every dormant compile-time option |
| `MAT-ANISO` | Two principal roughness values and tangent orientation; normalized anisotropic GGX, correlated masking, visible-normal sampling, directional PDF/Jacobian and isotropic-limit agreement |
| `MAT-COAT` | One dielectric coat over the base, explicit coat weight/roughness/IOR range and attenuation of underlying scattering; matched evaluation/sampling/PDF and no additive energy creation |
| Existing subsurface | State its direct-light approximation, controls and absent transport; check zero-strength identity, finite/grazing response and bounded energy, without claiming skin/path-volume accuracy |

[PBRT's microfacet treatment](https://www.pbr-book.org/4ed/Reflection_Models/Roughness_Using_Microfacet_Theory) is primary precedent for anisotropic distributions, masking and visible-normal sampling. [Khronos glTF material extensions](https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos) own imported extension semantics. Pin any reused derivation/spec/source revision and rights at MAT-D0; these sources do not prove the local model.

For every selected profile freeze `f(wi,wo)`, sampling transform, PDF measure, mixture probabilities, delta/near-zero handling and conventions. Apply `beta *= f * abs(n dot wi) / p(wi)` only with the matching measure and actual lobe mixture. Independent quadrature/white-furnace, reciprocity where applicable, PDF integral and sampled histogram/mean checks must distinguish energy loss, forbidden gain and Monte Carlo uncertainty. The reference and realtime approximations may differ only through a stated domain/error contract.

## Ownership And Representation

Extend existing authored/cooked material, MaterialCache, shared HLSL material/BRDF helpers and bounded path-tracer sampling. Reuse the existing tangent product and canonical normal-frame conventions; do not add another tangent target, surface record or per-effect BRDF fork. The existing reference transport semantics remain unchanged until an explicit profile revision is reviewed and qualified in its Stage 4 proof; existing raw datasets keep their original domain identity.

Budget: one material-profile discriminator only if real dispatch needs it; minimal selected parameter fields through current cook/CPU-GPU ABI; one common evaluation/sample/PDF kernel; existing surface-guide and debug hooks. Ledger any GBuffer format/target change with bytes/pixel, bandwidth, layout and native checks before accepting it. Zero material graph/compiler framework or renderer-wide lobe interface.

Alpha cutout, normal-map handedness/strength, non-uniform and negative transforms, mirrored UVs, tangent degeneracy, deformation and raster/ray LOD are mandatory input controls. Missing tangents/unsupported imported extensions have explicit rejection/loss reports; do not silently render a different material as correct.

## MAT-D0 Measurement And Failure Card

The material/transport implementer and content/native reviewers freeze exact selected model formulas, ranges/fixtures, analytic/numerical/statistical tolerances, field/layout/copy budget, source rights and profile qualification order. Numeric tolerances use independent quadrature/reference uncertainty, not beauty inspection. Existing workload scene/repeat/CI rules apply to whole-frame quality/time/memory; freeze practical cost and quality bands before results.

`FM-MAT-01` malformed/non-finite parameters -> reject at cook/publication. `FM-MAT-02` unsupported profile or missing required frame -> explicit content loss/refusal. `FM-MAT-03` ABI/descriptor generation mismatch -> block scheduling. `FM-MAT-04` non-finite or out-of-domain sample/PDF -> diagnose the exact event and prevent reference/evidence acceptance. Exercise each with a positive retained-material control.

## Binary Acceptance

| ID | Required artifact/check |
| --- | --- |
| `AC-MAT-01` | Authored -> cooked -> CPU/GPU -> pixel lineage; color spaces, parameter ranges and content losses verified |
| `AC-MAT-02` | Base model energy/reciprocity/PDF/sample checks, direct/indirect parameter consistency and honest subsurface approximation bounds |
| `AC-MAT-03` | Anisotropy normalized distribution/sampling, orientation rotation and isotropic limit, grazing/degenerate controls |
| `AC-MAT-04` | Layered coat energy and matched evaluation/sample/PDF, coat-zero identity and independently qualified raw-reference profile |
| `AC-MAT-05` | Raster/ray/static/deformed/alpha/normal-frame and both API agreement, guide semantics and raw/reconstructed separation |
| `AC-MAT-06` | Held-out shaded quality, full-frame/peak-memory cost, failures, package identity and documented reproduction of the material comparison |

Use `materials/shading-models/` under the existing workload artifact root. Completion requires all selected profiles, not only base helpers; references from an older material domain cannot qualify a new lobe. Reports retain results; no first-release score or accepted transport formula is changed by this target.

## Check And Risk Ownership

| Check | Defect-detecting obligation |
| --- | --- |
| CHK-MAT-01 | energy/reciprocity/PDF/sample and independent material-domain checks; retain the associated AC and FM row identities in the candidate report. |
| CHK-MAT-02 | tangent/alpha/deformation/ABI/content and raster/ray counterexamples; retain the associated AC and FM row identities in the candidate report. |
| CHK-MAT-03 | held-out native/raw/temporal cost and actual package reproduction; retain the associated AC and FM row identities in the candidate report. |

RISK-MAT-01: new lobes invalidate oracle or guide assumptions. The feature implementer and subject reviewer own mitigation: qualify each reference profile and each affected guide before claims. A failed required falsifier/value band stops the stage; do not hide it by expanding scope or lowering a post-hoc threshold. The local acceptance table remains conjunctive, and the report records every supported API/domain cell separately.
