# Basic Vignette

[Docs](../../../../../../../../README.md) / [Up](../DisplayPipelineDesign.md) / [Implementation](ImplementationPlan.md)

Implementation stages: [ImplementationPlan](ImplementationPlan.md).

Sections: [Basic Vignette Remaining Delivery Plan](ImplementationPlan.md#basic-vignette-remaining-delivery-plan).

**Status:** target feature dossier with partial connected source; numerical/native acceptance remains open

**Scope:** the required basic, bounded scene-image darkening toward the viewport corners, separate from chromatic aberration

**Source reconciled:** 2026-10-31 at `14b580f3`: dedicated pass/shader, registration, CVar and shared settings bridge exist; the older absence finding is superseded. No native/visual/performance acceptance is claimed.

**Dated readiness:** **0/100** in the lens scope of [FCR-REN-25](../../../../../../../../Acceptance/FeatureCompletionReports.md#initial-completion-report-registry); not rescored by source presence

**Parent:** [Display Pipeline](../DisplayPipelineDesign.md). [Release requirements](../../../../../../../../Acceptance/FirstRelease.md#required-rendering-closure) require this small effect on both backends. This page owns its proposed semantics and local delivery checks; release results belong in the existing FCR report.

## Result And Bounds

The existing [`Vignette.cpp`](../../../../../../../../../Engine/Renderer/Private/Passes/Presentation/PostProcessing/Vignette.cpp) and [shader](../../../../../../../../../Engine/Assets/Shaders/Passes/Presentation/PostProcessing/Vignette.hlsl) apply a fixed mask with at most 20% attenuation, selected through `r.Vignette` and shared boolean settings. The code currently runs after chromatic aberration. The target below places vignette first and requires frozen numeric/parameter/neutral behavior; the plan must align and qualify the existing route against that contract rather than add a second effect.

The approved presented output includes a basic vignette On with values frozen before implementation, consistently on D3D12/Vulkan. Neutral is a formula/no-work control, not the shipped default. Existing Editor/shared settings remain authoritative; no new public lens toggle or Game settings frontend is required. It is an artistic post-process example, not a physical lens model. No texture asset, lens profile, animated authoring system, barrel distortion, film grain or new general effect registry is required.

| Choice | Target contract |
| --- | --- |
| Placement | Target-linear scene image after the selected SDR/HDR tone/gamut mapping, before chromatic aberration, output encoding and UI. Raw reference radiance and exact diagnostic products bypass both lens effects. |
| Parameter/default policy | Intensity in `[0,1]`, inner radius in `[0,0.99]`; freeze actual nonzero On defaults before code without inventing values here. Intensity zero is the independent neutral control. Reject invalid/nonfinite input at the existing owner; only admitted persisted fields enter current per-user settings. New public toggles are deferred. |
| Coordinates | For local viewport UV and nonzero output extent, `r = length((uv - 0.5) * extent) / (0.5 * length(extent))`. Radius is zero at the center and one at the corners, with aspect-correct pixel distances. Use the viewport's own extent/subrect, not the containing host window. |
| Mask | `w = smoothstep(innerRadius, 1, r)`; `RGB_out = RGB_in * (1 - intensity * w)`. Copy alpha unchanged. No sampling beyond the input pixel and no new history. |
| Neutral state | Intensity zero schedules no effect pass/resource/copy; its output is the same input product. The image must be bit-identical to the disabled route. |
| Composition | Multiplication applies once in the declared linear domain. No metering, DLSS guide, raw reference, HUD or UI darkening. Chromatic aberration retains its own coordinates and alpha policy. |
| Ownership | Renderer-private display feature owns pixel policy/parameters and typed frame-graph pass. Existing settings authority owns values; Application owns persistence; existing Editor settings own admitted controls; Game only consumes shared persisted values. No native API or feature policy enters generic graph/host code. |

The formula and placement are proposed design choices, not SDK facts. Before code, freeze formats, dispatch coverage, floating-point tolerances, supported extent/aspect range, HDR treatment and pass-cost budget in this owner using the existing display-domain contract. Do not choose thresholds after seeing candidate results.

## Acceptance And Failure Checks

| Criterion | Required check and retained evidence |
| --- | --- |
| `AC-VIG-01` | Intensity zero yields exact input bytes and no vignette pass/allocation in the native capture on both APIs. |
| `AC-VIG-02` | Independent CPU formula and GPU output agree within the predeclared format tolerance for center, edges/corners, gradients and uniform inputs; both APIs agree under that same tolerance. |
| `AC-VIG-03` | Resize, non-square extent, viewport subrect and two distinct views use their own extents/settings; no stale shared state or host-window mask. |
| `AC-VIG-04` | Alpha, raw reference, exact debug products and UI remain unchanged; scene vignette occurs once before output encoding on SDR and qualified HDR tuples. |
| `AC-VIG-05` | Invalid values are visibly rejected; product Reset returns approved On defaults; explicit neutral validation omits work; state persists only in the declared per-user root. Included Shipping behavior has no editor/tool dependency. |
| `AC-VIG-06` | Record pass CPU/GPU cost, resource/copy delta and complete-frame effect against the fixed budget; no permanent history or hidden work while disabled. |

`CHK-VIG-01` exercises the neutral/corner/ramp formula and alpha controls (`AC-VIG-01/02/04`); `CHK-VIG-02` exercises extent/view/settings transitions (`AC-VIG-03/05`); `CHK-VIG-03` captures order, package membership and bounded cost (`AC-VIG-04/05/06`). All run on the same candidate and both APIs after the discovery card freezes numeric tolerances and budgets. A documentation check cannot pass them.

| Failure | Detection and safe result |
| --- | --- |
| `FM-VIG-01`: NaN/Inf/out-of-range controls or zero extent | Settings/viewport boundary rejects before recording; preserve valid settings, report the failing value/extent; no NaN output or partial pass. `CHK-VIG-02` controlled input. |
| `FM-VIG-02`: wrong domain/order or UI/raw product affected | Native markers and raw/formula comparison expose the defect; fail `AC-VIG-04` rather than tune intensity to conceal it. `CHK-VIG-01/03` controlled product/order cases. |
| `FM-VIG-03`: stale extent/view value after resize | Generation-qualified viewport transition rejects stale work; no cross-view state; fail `AC-VIG-03` if the old mask appears. `CHK-VIG-02` transition cases. |

## Remaining Delivery

The [fillable plan](ImplementationPlan.md#basic-vignette-remaining-delivery-plan) owns remaining discovery, the one effect slice and delivery proof. This dossier owns behavior and acceptance; the [master Roadmap](../../../../../../../../Strategy/Roadmap.md#required-feature-closure-priority) selects when to execute it. Preserve independent chromatic discovery and separate vignette evidence inside FCR-REN-25.
