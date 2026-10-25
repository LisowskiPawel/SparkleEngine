# Volumetric Lighting Delivery Plan

[Docs](../../../../../../../README.md) / [Up](VolumetricLightingDesign.md)

**Status:** deferred; no fog, atmosphere, heterogeneous-medium, volume-reuse or cloud implementation is selected.

The release and selected advanced program use the existing surface-transport and image-environment paths. The former nine-stage production ladder is retired. Fog, physical atmosphere, heterogeneous transport, reservoir reuse and clouds are distinct product decisions, not an automatic chain after REL-11.

## Admission Decision

Keep volume content/types, import dependencies, froxel/history/LUT resources, shaders and selectors out of production until a named workload justifies one tier. Reuse the current sky/environment and direct/indirect owners. A source-scene add-on, completed release gate or detailed mathematical study supplies no admission by itself.

For a proposed tier, the scope owner must first identify the consumer, expected image/evidence benefit, cheaper alternative, exact material/path domain, qualified content/rights and CPU/GPU/memory cost. Select or reject that tier explicitly; no allocation of effort to the entire media/cloud family is implied.

## Discovery If Admitted

Use [VOL-D0](Discovery.md) for the decisions used by that tier, retaining the [transport/composition math](TransportAndComposition.md), [ownership/lifetime architecture](ExecutionArchitecture.md) and [feature proof](VolumetricLightingDesign.md#acceptance-criteria). Defer unrelated grid, atmosphere, heterogeneous, reuse and cloud decisions instead of conducting all experiments to fill a template.

An admitted first fog slice still needs correct coefficients/phase/transmittance, independent analytic/raw oracle, surface/sky composition, existing authored-to-frame owners, bounds and failure/Off identity. Advanced stochastic transport needs its own reference/support/Jacobian/majorant and value evidence; basic fog cannot inherit it or require it by adjacency.

After discovery, write the actual selected stage sequence and its cleanup before source work, including the [Stage Source-Style Gate](../../../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate) for each admitted slice; no-source work records justified N/A. Each replacing slice removes duplicate medium/environment/light state, temporary integrators and rejected experiments; current sky and surface consumers must survive. This page contains no executable production prompt.

The [Roadmap](../../../../../../../Strategy/Roadmap.md#conditional-and-excluded-work) owns admission and order. VOL-PLAN-02 identifies the historical conditional sequence; it is not a current production contract or an acceptance result.
