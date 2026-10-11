# Debug Views

[Docs](../../../../../../README.md) / [Up](../RendererFeatureGuide.md)

Sections: [Debug View Presentation Architecture](#debug-view-presentation-architecture).

**Status:** feature dossier; Show-flags implementation complete, family acceptance remains bounded

**Current readiness:** **40/100** for the family; no family/release closure is claimed. Bounded delivery, admission and direct/indirect-family execution results are recorded by [Bounded Show evidence](../../../../../../Acceptance/Renderer/DebugViews.md#earlier-execution-evidence), not inferred for the remaining mode/control matrix.

**Responsibility:** feature-local navigation and current evidence posture for Renderer debug-view controls, presentation, delivery, and acceptance

**Verified:** Show delivery at `82528cbe5edbcf729464fbefe6998435730920eb` on 2026-10-06, accepted by the user after bounded serial/threaded D3D12 controls checks and DXIL/SPIR-V shader cooking. Historical mode and lighting baselines retain their original evidence boundaries.

**Architecture:** [Viewport Rendering Controls](Controls/README.md) and [Debug View Presentation Architecture](#debug-view-presentation-architecture)

**Current proof contract:** [Acceptance](Acceptance.md). [Bounded Show evidence](../../../../../../Acceptance/Renderer/DebugViews.md) retains the candidate and artifact limits; completed delivery instructions have been retired.

**Show delivery:** complete for the implemented feature set. The compact grouped menu, globally editable controls, initialized disabled contributions, scene/light classifications, and shader-parameter emissive control are implemented. Future controls extend their feature owner and the current [Show contract](Controls/ShowFlags.md). Remaining family proof is selected from the acceptance contract. This does not advance whole-family or release readiness.

## Current Source Shape

Sparkle has one host-independent `RenderViewMode` contract:

- Lit is value `0`;
- Reference Path Tracer is value `1`;
- Wireframe is value `2`;
- GBuffer, lighting, and GPU-scene inspection modes occupy contiguous values `3` through `17`;
- `Count` is `18`.

The ordinary `ViewportRenderRequest` carries one selected mode. `RenderViewBuilder` freezes it into `RenderView`; frame composition, raster GBuffer, and debug resolve consume that same value at their owning decisions. The HLSL constant contract mirrors the C++ values for shader consumers.

Editor uses the same enum directly while retaining presentation ownership in `ViewportToolbar`. Game viewport owners may submit the same rendering semantic without importing Editor. RHI remains unaware.

The former process-global visualization selection, Editor mirror enum/preset resolver, proposed visualization-target/show-flag split, and Reference selector CVar are clean-break deletions. They are not compatibility routes.

The debug resolve is partitioned into GBuffer, lighting, and GPU-scene families. Each family privately owns its activation predicate, early return, pass parameters, and shader; scene-level composition simply invokes the family entry points in order. Adding or reordering an enum value cannot activate a family by ordinal accident.

## Current Modes

| Capability | Modes | Current producer |
| --- | --- | --- |
| `REN-DBG-01` final/material | Lit, Wireframe | Ordinary frame; Wireframe changes raster fill |
| `REN-DBG-02` GBuffer | Diffuse, World Normal, World Tangent, Roughness, Metallic, Emissive, Ambient Occlusion, Subsurface Color, Subsurface Strength | `GBufferVisualizationCS` reads only GBuffer products; World Normal and World Tangent map signed world-space XYZ to display-linear RGB in `[0, 1]` without changing axes or orientation |
| `REN-DBG-03` lighting | Direct Diffuse, Direct Specular, Direct Subsurface, Indirect Diffuse, Indirect Specular | one `LightingVisualizationCS` reads five initialized lighting products plus GBuffer alpha; the View uniform selects the lobe; disabled direct and indirect raw modes report unavailable |
| `REN-DBG-04` scene | GPU Scene Instances | instance identity generation plus `GpuSceneVisualizationCS` |
| `REN-LGT-04` reference | Reference Path Tracer | private Reference middle/session with Editor menu and operational overlay; source-present, not executable-proved |

## Presentation Route

Sparkle now classifies each mode once in the private presentation owner. Scene-referred HDR views retain exposure, the selected tone curve, and the configured super-resolution provider. Display-linear exact views bypass exposure/tone mapping, use point reconstruction when render and output extents differ, and still receive output transfer encoding. Visualization shaders no longer contain a local HDR preview curve. Reference Path Tracer bypasses the independent Lit Ray Reconstruction denoiser without overriding the selected presentation upscaler or quality.

This is source presence, not pixel proof. The acceptance route must still exercise isolation, mode selection, both display domains, extent changes, backends, and Reference topology.

## Ownership

- Renderer Public owns only the generic enum and ordinary request field.
- Renderer Private owns View freezing and focused consumers.
- The private Reference Path Tracer folder owns its estimator, session, identity, resources, and passes.
- Editor owns menu presentation and selection interaction.
- RHI owns no mode or feature identity.

Any new mode must have a real production consumer. Any future independently selectable show control must be orthogonal to the selected mode and land with that consumer; it cannot recreate the removed parallel taxonomy.

The [Show menu](Controls/ShowFlags.md#editor-interaction) presents fifteen feature-owned CVars: Sky, Static Meshes, Skinned Meshes, Emissive, four analytic light types, five lighting lobes, and two shadow controls. Open Show beside Viewmode to edit leaves, bulk groups, or defaults; console edits refresh its checks. Sky controls background and environment illumination independently of analytic lights. Emissive uses ordinary shader parameters while retaining authored material data. There is no Renderer Show set or request/View transport. Decals and Indirect Subsurface are not advertised because they have no implemented producer. No measured GPU savings or release result is claimed.

## Debug View Presentation Architecture

**Status:** source-present architecture; executable validation deferred

**Responsibility:** debug-view signal domains, display mapping, output encoding, and producer requirements

Post-processing runs after domain mapping and before output encoding. Effects default On in Lit and Reference display derivatives. Diagnostic views default to bypass via the [Post Processing policy](Controls/ShowFlags.md#post-processing); explicit debug opt-in changes the display derivative and is not an exact-value oracle. Raw Reference products are unaffected.

### Decision

Every `RenderViewMode` has one presentation domain: scene-referred HDR or display-linear exact. Both domains use one display-mapping owner and one output-encoding owner.

The selected mode is already the complete stock policy. Presentation does not need a second target enum, show-flag set, or Editor preset translation. The presentation owner resolves the selected mode to its signal domain and applies the corresponding mapping.

### Domains

#### Scene-Referred HDR

The producer publishes linear scene radiance or a lighting contribution. The common display mapper applies the current scene exposure and selected tone curve exactly once. Lit, Reference Path Tracer, emissive, and direct/indirect lighting views belong here.

#### Display-Linear Exact

The producer publishes a bounded diagnostic value in linear display space. Exposure and the tone curve are bypassed; output transfer encoding still runs exactly once. Scalar material values, encoded normals, material colors, and stable instance palettes belong here.

“Exact” does not mean writing linear numbers into an encoded target. It means no content-dependent exposure or filmic curve changes the producer-authored display-linear value before the required output transfer.

### Route

```text
RenderView::viewMode
        |
        v
selected producer-domain color
        |
        v
PresentationPasses(mode domain, exposure, tone mapper)
        |
        v
DisplayLinearColor
        |
        v
OutputEncodingPass
        |
        v
viewport / back buffer
```

The presentation resolver is Renderer-private and exhaustive over `RenderViewMode`. It owns only signal-domain classification. Editor does not duplicate this table, and shaders do not infer it independently.

### Mode Classification

| Mode | Domain | Producer requirement |
| --- | --- | --- |
| Lit | Scene-referred HDR | composed linear scene color |
| Reference Path Tracer | Scene-referred HDR | Reference display derivative from raw accumulation |
| Wireframe | Scene-referred HDR | current Lit shading with raster wireframe fill |
| GBufferDiffuse | Display-linear exact | saturated linear base color |
| GBufferWorldNormal | Display-linear exact | normalized normal mapped from `[-1, 1]` to `[0, 1]` |
| GBufferWorldTangent | Display-linear exact | world-space tangent mapped from `[-1, 1]` to `[0, 1]` |
| GBufferRoughness | Display-linear exact | bounded scalar replicated to RGB |
| GBufferMetallic | Display-linear exact | bounded scalar replicated to RGB |
| GBufferEmissive | Scene-referred HDR | raw non-negative emissive value; no local preview curve |
| GBufferAmbientOcclusion | Display-linear exact | bounded scalar replicated to RGB |
| GBufferSubsurfaceColor | Display-linear exact | saturated linear material color |
| GBufferSubsurfaceStrength | Display-linear exact | bounded scalar replicated to RGB |
| DirectDiffuse | Scene-referred HDR | raw non-negative contribution |
| DirectSpecular | Scene-referred HDR | raw non-negative contribution |
| DirectSubsurface | Scene-referred HDR | raw non-negative contribution |
| IndirectDiffuse | Scene-referred HDR | raw non-negative contribution |
| IndirectSpecular | Scene-referred HDR | raw non-negative contribution |
| GpuSceneInstances | Display-linear exact | stable hashed instance palette |

### Producer And Extent Rules

The GBuffer, lighting, and GPU-scene visualization families independently produce their selected diagnostic color. Each family privately resolves its mode and binds only its own inputs; the scene-level composition has no activation logic and does not infer family membership from enum ordering. The family shaders output raw HDR for scene-referred modes and one intentional bounded mapping for exact modes. They contain no exposure or tone-mapper policy.

When render and output extents differ, exact views use an explicit point selection so reconstruction does not invent category IDs, material values, or false colors. The producer must not assume source and destination extents match.

GBuffer production declares one transient `R32_UInt` identity product in every mode. Raster binds an eighth attachment; inline and pipeline ray tracing bind a UAV. Each frontend uses one shader and a write uniform: GPU-instance visualization stores GPU-scene slot plus one, while other modes store zero; zero also denotes background. Ordinary material base color remains unchanged. The visualization shader owns the hash palette and maps zero to black. The identity product costs four bytes per pixel at render extent before allocation aliasing; its binding remains active when visualization is disabled. It is regenerated per frame and has no temporal history.

The existing exposure owner continues to meter the ordinary Lit scene even while an exact diagnostic is visible. Exact presentation ignores the resulting exposure value, but keeping history warm prevents an unrelated adaptation reset when returning to Lit.

### Optional Independent Controls

Stock presentation is derived from `RenderViewMode`; no presentation flag is required. A future expert override for exposure or tone mapping is a separate product decision and may be added only with a real user workflow, explicit Custom state, reset behavior, capture provenance, and a dedicated consumer. It must not change the meaning or identity of the selected mode.

### Capture

Renderer/RHI capture transport remains neutral. A higher-level evidence record may join completed product identity with the selected mode, resolved signal domain, exposure/tone-mapper application, exposure value, output encoding, extent, and any explicit expert override. This metadata does not belong in the RHI readback contract.

### Rejected Alternatives

- Keep a producer-local HDR preview curve: rejected because it hides magnitude and creates double mapping.
- Bypass all presentation for exact views: rejected because output transfer encoding is still required.
- Add mode-shaped show flags or a visualization target: rejected because the selected mode already owns this stock policy.
- Add a process-global “debug views bypass tone mapping” CVar: rejected because it cannot describe independent viewports and makes evidence ambiguous.

### Evidence Boundary

The source route implements this architecture. Numeric fixed-value checks, extent cases, dual-viewport isolation, output encoding, shader cook, D3D12/Vulkan execution, and captured pixels remain required evidence under [Acceptance](Acceptance.md).
