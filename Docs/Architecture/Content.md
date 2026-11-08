# Content from source to pixel

[Docs](../README.md) ? [Subjects](README.md)

Importers interpret source formats; cookers create validated runtime products; GameFramework loads a complete accepted world; Renderer uploads and renders derived resources. Each boundary has its own identity and lifetime.

```mermaid
flowchart LR
    Source[Source and provenance] --> Import[Import and translate]
    Import --> Cook[Cook complete typed products]
    Cook --> Publish[Publish accepted file set]
    Publish --> Load[Load candidate world and settle workers]
    Load --> World[Publish world and render submission]
    World --> GPU[Renderer residency and pixels]
```

A successful cook is an intermediate result. It does not prove material fidelity, a correct frame or permission to redistribute the source.

## Owners

| Owner | What belongs there |
| --- | --- |
| [Engine assets](../../Engine/Assets) | Shader/image/fixture source bytes and licenses; not a runtime asset manager. |
| [SourceImporters](../../Tools/Import/SourceImporters) | Format parsing and translation, including glTF geometry/material/animation semantics. Import support is bounded by the actual parser and output schemas. |
| [Cooking](../../Tools/Cooking) | Texture/asset orchestration, deterministic conversion, complete staging and product validation. Shader cooking has its [own compiler contract](Shaders.md). |
| [ToolSupport](../../Tools/Support) | Preparation catalogs, source lookup, tool paths and project acquisition metadata shared by Launcher/cookers. |
| [GameFramework assets/levels](../../Engine/GameFramework/Private) | Cooked decode, reference resolution, candidate world lifetime and accepted publication. |
| [Renderer residency](../../Engine/Renderer/Private) | Upload, active generations, CPU/GPU lifetime and material/mesh/texture use. [Materials](Materials.md) covers the frame contract. |

Persistent authored asset/object GUIDs, reimport/source keys, runtime entity/render IDs, GPU indices and content hashes serve different purposes. Preserve those distinctions through import, cook, load and capture. Runtime reads cooked resources, never silently searches raw assets.

## Publication and recovery

Cookers stage the complete output set and validate schemas/references before replacement. Core supplies containment, a shared publication lock, incomplete markers and checked in-process rollback. Runtime holds one read scope from fresh lookup through decode and worker settlement; cooperative writers cannot replace that domain while it is borrowed.

A killed publisher or failed rollback leaves generated output marked incomplete. Readers refuse it; explicit cleanup and recook regenerate the disposable domain from source. Do not reconstruct a success through stale files. There is no persisted transaction journal, checksum-replay recovery, schema migration or power-loss guarantee. User source assets/settings stay outside the disposable boundary.

Keep output locks and borrowed data alive until every worker settles. Failure/cancellation preserves the accepted world and existing valid publication where the operation's transaction permits it; it never publishes a partial replacement.

## Content-to-pixel

For FR-04 and later CPIX-0..3, freeze source provenance/hash, importer/options, cooked dependencies, level/camera/pose, material interpretation, shader/settings identities and the actual raw/display artifact. Trace one known pixel or feature back through those inputs. Record unsupported/lost source semantics explicitly.

First prove topology, transforms, normals/tangents, winding, alpha/sidedness, texture roles/color spaces, UVs, animation/skinning/morph, light/environment units and raster/ray correspondence. Then compare raw lighting against a qualified [reference](ReferencePathTracer.md); display/denoising cannot hide wrong inputs.

Post-release Bistro/San Miguel proof requires separate scene/camera populations and cross-scene material/quality/generalization checks. Do not promote Sponza or importer-fixture results to those scenes. Source previews are provenance aids, not renderer oracles.

## Remaining checks

Use the smallest affected fixture and selected-map path: deterministic serial/worker output, malformed/overflow/truncated references, unsupported format rejection, cancellation, stale-generation rejection, accepted-world preservation, reader/writer exclusion, interrupted publication refusal and successful recook. Recheck only claimed importer/texture formats; a blanket unused-format matrix is not a packaging prerequisite.

Geometry-cache animation remains unselected: existing skin/morph/animation support does not imply sampled geometry playback. KTX build/dependency presence does not establish an active KTX source/product path. Broader streaming, remote caches and a general incremental content graph are outside the selected slice.
