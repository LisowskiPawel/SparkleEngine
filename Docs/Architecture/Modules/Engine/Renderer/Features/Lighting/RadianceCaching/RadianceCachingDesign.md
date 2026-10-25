# Radiance Caching

[Docs](../../../../../../../README.md) / [Up](../LightingDesign.md)

**Status:** selected post-release target; no SHaRC or NRC integration is claimed.

Deliver NVIDIA SHaRC first, then NVIDIA NRC. This order is an explicit portfolio requirement from 2026-10-25. Existing indirect lighting and reference source are inputs to qualify, not evidence that either cache works.

| Delivery | Result and owner | Start / closure |
| --- | --- | --- |
| [SHaRC](Sharc/SharcDesign.md) / [plan](Sharc/ImplementationPlan.md) | Shader-source spatial radiance caching inside the existing indirect continuation path | Correct uncached transport and raw reference; all SHaRC acceptance cells before NRC implementation |
| [NRC](Nrc/NrcDesign.md) / [plan](Nrc/ImplementationPlan.md) | Vendor neural radiance-cache integration, online update/query/resolve and measured comparison with SHaRC | SHaRC fully done, including package and documented reproduction; NRC feasibility research may precede this gate |

Both accelerate a bounded indirect continuation and return through existing indirect diffuse/specular products. Neither replaces direct lighting or becomes the raw reference oracle. [Reference Path Tracer](../ReferencePathTracer/ReferencePathTracerDesign.md) remains cache-free for accepted references and training labels. No second tracer, scene representation, cache service, provider registry or general AI runtime is selected.

The first SHaRC profile queries diffuse continuation only; misses continue normal tracing. NRC subsequently evaluates the same diffuse domain and one explicitly qualified rough-specular domain. Do not reuse a direction-independent SHaRC value for arbitrary sharp reflections. Cache algorithms have different state, synchronization and failure contracts; only the existing scene/transport inputs and output meaning are shared.

Use one selected cache per frame. Cache Off, SHaRC and later NRC are mutually exclusive intent in the existing indirect settings owner, with requested/active state visible. An unsupported or broken explicitly selected cache must be reported before successful publication; never label an uncached frame as cache output. Cache misses are normal transport events, distinct from provider failure.

Evidence lives under the existing graphics-workload artifact root in `radiance-cache/sharc/` and `radiance-cache/nrc/`, with original candidate/configuration/resource/shader/SDK joins. Local reports own the new `AC-SHARC-*` and `AC-NRC-*` verdicts; these are later portfolio targets, not new first-release FCR families or readiness points. Existing [workload protocols](../../../../../../../Acceptance/GraphicsWorkloads.md) govern scene rights, raw comparisons, repeats, observer effects and uncertainty.
