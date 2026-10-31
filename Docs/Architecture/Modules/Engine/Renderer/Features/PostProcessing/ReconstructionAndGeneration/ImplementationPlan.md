# Denoising And Upscaling Delivery Plan

[Docs](../../../../../../../README.md) / [Up](DenoisingDesign.md)

**Status:** selected target sequence; discovery and prerequisite gates remain open. No stage is executed by this plan.
**Contract:** [design, semantics, measurement and local acceptance](DenoisingDesign.md); [Roadmap](../../../../../../../Strategy/Roadmap.md) owns global order.

## Work This Plan

REC-S0/1 refine existing SR/RR obligations through DSP-1 and DSP-3. RELAX production follows REL-11 and qualified raw direct/indirect products; it is not a newly imposed first-release dependency. Owned CNN work keeps its NG discovery and delivery chain. Execute the first unfinished stage only. The discovery card freezes numerical tolerances, practical-effect bands, exact eligibility, rights, hook/copy/resource limits and failure checks before production; a source-present mechanism is not a satisfied gate.

## Stage Contract

Every stage and copied prompt follows [stage completion and resume](../../../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-completion-and-resume). Close its finite local exit once; keep later aggregate proof separate.

Every stage inspects HEAD/status, applicable AGENTS.md, live producers/consumers/lifetime/build membership and concurrent edits. Apply [Change Integration](../../../../../../../Engineering/Workflow/ChangeIntegration.md), [Change Lifecycle](../../../../../../../Engineering/Workflow/ChangeLifecycle.md), relevant Renderer/RHI/Tools standards and the [Stage Source-Style Gate](../../../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate), including authored readability/spacing review after formatting. Discovery/no-source work records justified source-style N/A.

Ledger every external feature hook and material copy; default to existing semantic inputs, one feature-local owner, zero new public types and no framework. Each stage removes replaced state/paths/selectors/dependencies in the same slice and checks retained consumers. A zero-deletion stage names why no replaced path exists. Temporary probes stay local and are removed before handoff; keep valuable source/commands/results as ignored evidence.

Each exit records exact candidate/dirty state, API/device/tool/content/shader/SDK/artifact identities, command, oracle, observation and remaining cells in `reconstruction/denoising-upscaling/` under the existing workload artifact root. Use the cheapest defect-detecting check; production/native/quality/cost/package/adoption verdicts remain distinct. Stop for missing semantics, required unavailable proof, unsafe lifetime, quality change, silent fallback or a hook outside the frozen budget.

## Correctness Repair Follow-up

The 2026-10-27 source corrections do not qualify DLSS RR/SR quality or the selected SDK/device/API tuple. [Renderer's index](../../../RendererDesign.md#pbr-correctness-follow-up) routes the review; execute this work through existing REC-S0/1/3/4 and DSP-1/3 gates:

1. **REC-S0/1: prove external motion translation (N01).** The Streamline boundary now negates the engine's current-minus-previous pixel motion to match NVIDIA's current-to-previous displacement. Internal reservoir reprojection still subtracts engine motion. Verify a 20-pixel hand case, static and moving cameras/objects, deformation, sky rotation and jitter with native readback/visualization. Deliberately restore the old sign and require that control to fail. Include exact mvecScale, jitter, matrices, reset and input/output extents in the candidate report; do not infer correctness from valid resource tags.
2. **REC-S0/1/4: qualify the radiance range and formats (N05).** GBuffer emission, shared scene/direct/indirect and reconstruction-output color products now use RGBA32F instead of silently losing finite HDR values in FP16 before exposure. The existing supported format is reused; no raw radiance clamp or pre-exposure representation was added. Prove finite high-highlight and emissive-factor cases through raw lighting, composite, RR/SR and export with the actual installed SDK and both APIs. Verify supported input/output formats, native state/lifetime and refusal behavior. Measure memory/bandwidth/frame-cost changes at supported extents; the format correction doubles these color allocations' bytes per pixel and is not a performance acceptance result.
3. **REC-S0/1: settle the current RR/upscaler topology (open).** Current source still runs RR NativeAA at render extent before a separate presentation upscaler. Freeze and implement the selected release-specific combined reconstruction/output contract under REC-D0/DSP ownership, with one declared output producer and no silent provider substitution. Source corrections do not endorse the current sequence or close SDK eligibility, reset or lifetime gates.
4. **REC-S3/4: qualify guides and temporal quality after raw corrections (open).** Consume [material/geometric lineage](../../GeometryAndResources/ImplementationPlan.md#correctness-repair-follow-up), [direct reuse](../../Lighting/DirectLighting/ImplementationPlan.md#correctness-repair-follow-up) and [indirect support](../../Lighting/IndirectLighting/ImplementationPlan.md#correctness-repair-follow-up). Validate hit-distance validity separately from noisy radiance, black reflections, misses, disocclusion, light/emissive changes, thin alpha geometry, cuts, resize and provider switches at fixed exposure. Compare matched raw inputs and independent references. NRD RELAX remains later REC-S2 work; do not apply its packing/range/miss convention to DLSS by association.

## REC-S0 Freeze REC-D0 signals and topology

**Start:** Live producer/provider and installed release SDK inspection.

**Work:** Freeze actual radiance/hit-distance/normal/roughness/depth/motion/exposure/jitter semantics, RR/SR output topology and supported SDK/device/API matrix. Derive guides from transport, including ReSTIR hit-distance validity. Freeze numeric/temporal/value and hook/copy/resource gates.

**Cleanup:** Reject extra SDK filter families/upscalers and inventory redundant guides/history.

**Exit:** REC-D0 closes a supported topology and defect-detecting wrong-guide controls.

```text
Execute only REC-S0 from Docs/Architecture/Modules/Engine/Renderer/Features/PostProcessing/ReconstructionAndGeneration/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## REC-S1 Qualify SR and RR

**Start:** REC-D0 and DSP-1 and DSP-3 prerequisites.

**Work:** Execute original release input/extent/native/lifetime/failure/package proof. Replace RR-then-SR with the selected single RR output owner through the current dossier/AC-IRU/settings/caller update in the same slice. Compare NativeAA and scaled cells independently.

**Cleanup:** Delete duplicate output producers, unsupported double reconstruction and unnecessary guides while preserving retained consumers.

**Exit:** Original AC-IRU and AC-REC-01/02 pass for actual SDK tuples; owning FCRs retain verdicts.

```text
Execute only REC-S1 from Docs/Architecture/Modules/Engine/Renderer/Features/PostProcessing/ReconstructionAndGeneration/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## REC-S2 Deliver one RELAX path

**Start:** REL-11 and qualified diffuse/specular raw products/guides.

**Work:** Integrate pinned NRD RELAX shaders/dispatches through existing ShaderSystem, graph and reconstruction owners. Prove a real static minimal case and preserve raw inputs; no second NRI/RHI layer.

**Cleanup:** Replace the previous classical filter when its supported consumers are covered; delete unused NRD families/wrappers.

**Exit:** AC-REC-03 static cells pass on both APIs. DIR-6/IND-6 reuse this path/proof rather than build another filter.

```text
Execute only REC-S2 from Docs/Architecture/Modules/Engine/Renderer/Features/PostProcessing/ReconstructionAndGeneration/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## REC-S3 Close temporal and native safety

**Start:** Static RELAX output and guide semantics qualified.

**Work:** Exercise wrong-guide mutations, sky/thin-alpha/specular motion, cuts/resize, provider/light changes, two Views and delayed-completion replacement. Inject SDK/allocation/evaluation/device failures.

**Cleanup:** Remove duplicated reset authorities, guide snapshots, temporary visualizers and redundant histories.

**Exit:** AC-REC-01/03/04 and FM-REC controls pass with independently inspectable raw estimator output.

```text
Execute only REC-S3 from Docs/Architecture/Modules/Engine/Renderer/Features/PostProcessing/ReconstructionAndGeneration/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## REC-S4 Measure compatible choices

**Start:** All semantic/native cells pass; references and rules frozen.

**Work:** Compare meaningful raw/classical/SR/RR cells with identical output and sampler identity. Include owned CNN only after its own conformance and for its actual component. Measure spatial/temporal/response quality, frame cost and complete guide/history/workspace memory.

**Cleanup:** Delete losing filters/settings and redundant classical paths; no benchmark platform or full cartesian test matrix.

**Exit:** AC-REC-05 local value gate passes with separate attribution and honest domains.

```text
Execute only REC-S4 from Docs/Architecture/Modules/Engine/Renderer/Features/PostProcessing/ReconstructionAndGeneration/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```

## REC-S5 Package and reproduce

**Start:** Technical criteria and distributable dependencies available.

**Work:** Manifest exact shader/profile/SDK/native bytes and requested/active/refusal UX. The author or agent reproduces quality/cost plus a discontinuity/failure sequence; join only applicable original observations.

**Cleanup:** Remove probes, unused SDK families and completed instructions; preserve raw/native evidence.

**Exit:** All AC-REC-01..05 pass; neither NG acceptance nor release FCRs inherit unrelated results.

```text
Execute only REC-S5 from Docs/Architecture/Modules/Engine/Renderer/Features/PostProcessing/ReconstructionAndGeneration/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
```
