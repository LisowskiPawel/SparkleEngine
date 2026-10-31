# Volumetric Lighting Delivery Plan

[Docs](../../../../../../../README.md) / [Up](VolumetricLightingDesign.md)

**Status:** selected target sequence; discovery and prerequisite gates remain open. No stage is executed by this plan.
**Contract:** [design, semantics, measurement and local acceptance](VolumetricLightingDesign.md); [Roadmap](../../../../../../../Strategy/Roadmap.md) owns global order.

## Work This Plan

Selected 2026-10-25 after REL-11: VOL-Q0 semantic/reference, VOL-Q1 homogeneous/height fog and VOL-Q2 bounded local-media froxel lighting. Atmosphere/physical sky, imported dense/sparse volumes, multiple scattering, volumetric ReSTIR and clouds remain unselected. Existing sky or a fog screenshot does not satisfy the target. Execute the first unfinished stage only. The discovery card freezes numerical tolerances, practical-effect bands, exact eligibility, rights, hook/copy/resource limits and failure checks before production; a source-present mechanism is not a satisfied gate.

## Stage Contract

Every stage and copied prompt follows [stage completion and resume](../../../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-completion-and-resume). The [state update](../../../../../../../Engineering/Workflow/ChangeLifecycle.md#update-stage-state-before-handoff) is part of its exit: record the result, reconcile remaining work and name the next item before handoff. Later aggregate proof remains separate.

Every stage inspects HEAD/status, applicable AGENTS.md, live producers/consumers/lifetime/build membership and concurrent edits. Apply [Change Integration](../../../../../../../Engineering/Workflow/ChangeIntegration.md), [Change Lifecycle](../../../../../../../Engineering/Workflow/ChangeLifecycle.md), relevant Renderer/RHI/Tools standards and the [Stage Source-Style Gate](../../../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate), including authored readability/spacing review after formatting. Discovery/no-source work records justified source-style N/A.

Ledger every external feature hook and material copy; default to existing semantic inputs, one feature-local owner, zero new public types and no framework. Each stage removes replaced state/paths/selectors/dependencies in the same slice and checks retained consumers. A zero-deletion stage names why no replaced path exists. Temporary probes stay local and are removed before handoff; keep valuable source/commands/results as ignored evidence.

Each exit records exact candidate/dirty state, API/device/tool/content/shader/SDK/artifact identities, command, oracle, observation and remaining cells in `volumetrics/lighting/` under the existing workload artifact root. Use the cheapest defect-detecting check; production/native/quality/cost/package/adoption verdicts remain distinct. Stop for missing semantics, required unavailable proof, unsafe lifetime, quality change, silent fallback or a hook outside the frozen budget.

## VOL-S0 Close selected VOL-D0

**Start:** Planning/source inspection; production waits for REL-11.

**Work:** Freeze coefficient/phase/units/composition, independent oracle, Q0/Q1/Q2 representation, analytic local density fields, light/shadow/transmittance, froxel/reprojection and authored/status contracts. Freeze numeric/temporal/quality/time/peak-memory gates and assign the local candidate report.

**Cleanup:** Mark atmosphere/cloud/VDB/reservoir decisions Deferred; remove blanket no-volume admission wording without claiming implementation.

**Exit:** Consumed VOL-D0 decisions, failures/checks/hooks/copies and AC-VOL-15 evidence owner accepted.

```text
Execute only VOL-S0 from Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/VolumetricLighting/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
Before handoff, update the stage result, remaining plan and next item.

```

## VOL-S1 Prove medium transport

**Start:** REL-11, selected VOL-D0 decisions and oracle inputs.

**Work:** Qualify sigma_t=sigma_a+sigma_s, inverse-metre conversion, normalized phase, analytic slab/height transmittance/single scattering, vacuum, opaque-depth stop and Lout=T*Lsurface+Lscatter once. Use independent quadrature and bounded ratio/delta-tracking reference evidence for varying density.

**Cleanup:** Remove temporary runtime integrators and duplicate sky/fog constants; retain independent oracle source/commands as ignored evidence, not a second engine tracer.

**Exit:** AC-VOL-01..03/06 Q0 cells pass with numeric bounds and unchanged surface/image-sky controls.

```text
Execute only VOL-S1 from Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/VolumetricLighting/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
Before handoff, update the stage result, remaining plan and next item.

```

## VOL-S2 Deliver height fog and one light

**Start:** VOL-S1 and current scene/light owners qualified.

**Work:** Implement Q1 through authored/prepared scene, one feature-local Renderer owner, graph and composite. Produce physical global homogeneous/height fog, one direct light, geometry visibility and medium attenuation; expose raw transmittance/in-scatter and Off identity.

**Cleanup:** Delete additive screen-overlay/fake-fog prototypes and duplicate medium state; no atmosphere LUTs/weather system.

**Exit:** Q1 AC-VOL-02/05/06/12 and applicable failure cells pass on both APIs.

```text
Execute only VOL-S2 from Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/VolumetricLighting/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
Before handoff, update the stage result, remaining plan and next item.

```

## VOL-S3 Deliver local-media froxels

**Start:** Q1 and frozen Q2 density/overlap/light/capacity rules.

**Work:** Add bounded analytic varying-density fields, deterministic overlap, directional/point/spot/rect scattering, geometry shadows and light-path medium transmittance. Compare varying-density integration with independent reference and small-light exhaustive fixtures.

**Cleanup:** Replace Q1 duplicate evaluation/resources with the selected common owner; no imported volume format, duplicate light store or surface-reservoir shortcut.

**Exit:** AC-VOL-04/05/06/12 Q2 cells pass including bounds/overlap/capacity and invalid-input refusal.

```text
Execute only VOL-S3 from Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/VolumetricLighting/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
Before handoff, update the stage result, remaining plan and next item.

```

## VOL-S4 Close temporal native and value proof

**Start:** Q2 raw transport qualified; sequences and rules frozen.

**Work:** Add needed jitter/reprojection/upsampling and declared volume history. Test camera/medium/light changes, disocclusion, two Views, extent/reset/replace/shutdown and allocation/dispatch/device faults. Compare grid/step/history profiles, native hazards, full-frame and complete froxel/history/replacement memory.

**Cleanup:** Delete rejected grid/history variants, redundant snapshots and debug-only resources; preserve sky/surface controls.

**Exit:** AC-VOL-07/13/14 and applicable FM/CHK cells pass; smoothing cannot hide raw integration error.

```text
Execute only VOL-S4 from Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/VolumetricLighting/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
Before handoff, update the stage result, remaining plan and next item.

```

## VOL-S5 Package and reproduce selected tiers

**Start:** Q0/Q1/Q2 technical cells qualified.

**Work:** Manifest scene/shader/setting/rights identities; documented author/agent rehearsal reproduces raw transport, dynamic quality/cost and one failure. Publish coefficient/phase derivation and step/grid convergence study. Keep AC-VOL-08..11 exclusions scoped to unselected atmosphere/imported-volume/reservoir promises, preserving required Q2 varying-density proof.

**Cleanup:** Remove probes, unused selectors/dependencies and completed instructions; retain one medium capsule.

**Exit:** All applicable AC-VOL-01..15 pass for selected tiers, exclusions are explicit, and current negative capability claims change only to observed behavior.

```text
Execute only VOL-S5 from Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/VolumetricLighting/ImplementationPlan.md. Apply the Stage Contract, including the Stage Source-Style Gate. Verify Start against current source and exact prerequisite artifacts; perform the full Work and Cleanup, then prove Exit. Retain positive controls for supported behavior and each required failure counterexample. Stop on a missing semantic, unsafe lifetime, unmet prerequisite or exceeded complexity budget. Report exact checks/artifacts and unrun evidence; do not advance another stage.
Before handoff, update the stage result, remaining plan and next item.

```
