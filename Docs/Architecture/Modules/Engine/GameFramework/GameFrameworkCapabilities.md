# GameFramework Capability Inventory

[Docs](../../../../README.md) / [Up](../README.md)

**Status:** capability snapshot; current, but not release approval or runtime evidence

**Snapshot:** 2026-09-06 at committed `master` revision `8414b5dc`; `Engine/GameFramework` public/private source, CMake membership, Application/Renderer boundaries, and cooked loaders inspected; evidence `S` only

**Scope:** level lifecycle, cooked asset loading, world/ECS storage, camera/light/material/mesh/animation behavior, editing, publication, and render extraction

**Owner:** `Engine/GameFramework` / `SparkleGameFramework`

**Evidence and disposition:** [Capability Evidence Plan](../../CapabilityEvidencePlan.md) and [First Release Acceptance Contract](../../../../Acceptance/FirstRelease.md)

**Current readiness:** **48/100** — level/world/publication paths are broadly integrated; content-class, cancellation, reload, determinism, and runtime proof remain open. See [Current Feature Readiness](../../../../Acceptance/CurrentReadiness.md#foundation-world-content-shaders-and-tools).

## At A Glance

| Layer | Current capability | Main boundary |
| --- | --- | --- |
| level/session | registered cooked level activation, cancellation/stale-generation rejection, and save of authored level state | source import/cook and package acquisition remain tool/product owners |
| world/ECS | generation-aware entities, fixed component types, typed queries, world-owner edits, and compiled system graph | purpose-built internal ECS, not a public arbitrary component framework |
| simulation | transforms, camera, lights, material variants, skeletal animation, morph animation, and extraction | deterministic parallel equivalence and broad gameplay systems unproved |
| render handoff | immutable `RenderFrameSubmission` carries current world result to Renderer | Renderer never reads live ECS; world never owns GPU resources or render technique |

```mermaid
flowchart LR
    Level[Cooked level generation] --> Activate[Validate and activate into world]
    Activate --> Systems[Run compiled simulation systems]
    Systems --> Commit[Commit structural and derived state]
    Commit --> Extract[Extract immutable render submission]
    Extract --> Renderer[Renderer scene/view preparation]
```

This boundary trades direct Renderer access for deterministic publication and clearer threading. A failed load/system/extraction generation cannot expose a partly updated world or frame submission.

## Module Boundary

GameFramework depends on Core and Tasks but not Platform, Renderer, or RHI. It owns simulation/world truth and publishes Renderer-neutral `RenderFrameSubmission` data. This separation is a current implemented boundary, not merely an intended layering rule.

### World System Scheduling

The fixed world pipeline uses task-local typed outputs and world-owner commits. There is no generic structural command buffer, temporary-entity remapping, command conflict tracker, or hashed component-schema catalog. The unused private scaffolding and registry/storage bulk-add, component-removal and reset APIs were retired; scene activation, real world edits, frozen queries and render extraction retain their existing paths.

Source ownership reconciled on 2026-10-27. `GameWorldSystems.cpp` composes the
eleven-stage graph in execution order and owns the frozen-epoch execution flow.
`GameWorldSystemDeclarations.cpp` constructs each stage through a focused private
function, keeping query/resource access, phase, prerequisites, range policy, item
count and body together. Prerequisites use indices returned by the same graph
builder; there is no separate hashed system identity or per-frame binding
registry. `GameWorldSystemPreparation.cpp` implements resource admission and
scratch sizing on the existing `GameWorldState`; execution orchestration retains
freeze, run and success-only dirty-state clearing. Helpers retain no new state,
descriptor copies or epoch references. This is source/build preservation evidence,
not runtime, determinism or performance acceptance.

Compilation retains phase/resource validation, write-hazard rejection, and cycle rejection, then records dependency waves. The host submits each wave through `SparkleTasks`, resolving item counts after prerequisite commits. Empty stages add no tasks; serial workloads add one task; larger stages use the shared `ParallelFor` implementation. The frozen structure epoch spans all waves and deterministic output commits. Failure stops subsequent waves and prevents successful world publication. Each wave retains one compiled Tasks graph and its item-count key; count changes replace it after the previous submission settles. Cached callbacks obtain the current frame execution owner from the typed Tasks context, never from a retained frame reference. Graph construction occurs on count changes, while host settlement remains a scheduling cost; fewer scheduled nodes alone do not establish a frame-time improvement.

System declarations and their phase, component/resource-access and prerequisite metadata live only in `GameSystemGraphCompiler`. After validation and wave construction, the compiler moves stage names and publishes range policies and execution callbacks as private `CompiledGameSystem` records. Declaration vectors expire with the compiler; runtime execution retains only these records, wave indices and the existing cached Tasks graphs. Cached callbacks borrow stable runtime records and the current execution context.

`CompiledGameSystemGraph` exposes validity, errors and execution; it has no `GetSystems` inspection accessor. Its executable records remain in the private compiled-data header, consumed by the implementation, compiler and executor. [FCR-WORLD-01's FWC-3 result](../../../../Acceptance/FeatureCompletionReports.md#foundation-contract-progress) records the earlier accessor deletion and its source-only limits. Narrowing retained system records does not alter validation policy, wave topology, task ranges, world publication or extraction semantics; frame-time and GPU claims require their own evidence.

### Scene Activation And Read Publication

Source ownership reconciled on 2026-11-03. `GameWorld::CommitSceneLoadPackage`
orders isolated-world construction, level initialization, staged population and
validation, and the accepted state/resource swap.
Private `World/SceneLoadWorldStaging` owns entity-count validation, authored
camera/sky/light initialization, payload traversal, camera selection and final
resource/entity checks. `GameWorldSceneAssetCommitter` is a file owner for the
direct `CommitSceneAssetPayload` function; it retains no committer object or
borrowed-reference members. Focused private functions transfer animation and
material resources, construct mesh groups and material bindings, and add cameras
and lights. Existing mesh and material-variant translators remain authoritative.
`LevelSession` owns the active level document and name, request/generation
admission, events and activation completion; it never observes a partially staged
world. `GameWorld` retains no duplicate level description or name. Its public
editing path is generation-checked `SubmitEdit`; material variants remain
available through that path and `CaptureMaterialVariants`, without separate
direct mutation/query methods. The unused entity-destruction facade and its
unproduced journal kinds are retired; private registry/resource removal remains
for failed scene construction, and render extraction retains structural removals.

`GameWorldState::PublishReadView` owns snapshot allocation, generation/sequence,
baseline-versus-incremental selection, sky capture and release publication. Its
private baseline, change-application and entity-refresh functions own row
construction, changed-ID deduplication and sorted erase/upsert mechanics. The
existing shared immutable snapshot remains the only read-view publication
boundary; helpers borrow it for one call and retain no state. These are
source/build preservation boundaries, not runtime, task-determinism, content,
performance or release acceptance evidence.

### Animation Sampling Ownership

Source ownership reconciled on 2026-10-06: [AnimationSampler](../../../../../Engine/GameFramework/Private/Animation/AnimationSampler.cpp) keeps step-key selection, segment selection, alpha calculation, and cubic interpolation as file-private operations. Vector and rotation sampling retain distinct interpolation and default contracts; step selection includes keys at the requested time, while linear/cubic selection preserves the existing segment boundary rule. Empty channels return zero vectors or identity rotations, and sampled rotations remain normalized. Keyframe indices stay within the sampling call; selection and interpolation allocate no storage and retain no references. Stateless sampling and pose-composition helpers have no class shells or value-loading forwarding layer; pose, morph, skinning, and output-storage owners remain separate. Focused CPU comparisons do not establish task determinism, rendered animation, or content/runtime acceptance.

## Level And Cooked-Asset Capabilities

| ID | Capability | State | Exact current coverage and limit | Evidence |
| --- | --- | --- | --- | --- |
| `GF-001` | Level documents | Implemented path | Text `.level` parser/writer covers name, authored camera, optional sky, authored lights, and one or more scene-asset IDs. It is not a general scene graph serialization format. | `S` |
| `GF-002` | Level registry/session | Implemented path | Discovers registered levels, initializes a startup level, queues one requested change, reports Reading/Decoding/Validating/Ready/Failed/Cancelled progress, emits change events, exposes diagnostics, and saves the active level. | `S` |
| `GF-003` | Asynchronous scene load | Implemented path | A Tasks graph reads cooked files, decodes asset payloads, validates the manifest/package, and commits only the current request into the world. Cancellation and request IDs prevent obsolete publication. | `S` |
| `GF-004` | Cooked-only runtime | Implemented path | Runtime loaders consume scene manifests, meshes, materials, skeletons, animations, textures by reference, and the scene registry. Source import libraries are not linked to GameFramework. | `S` |
| `GF-005` | Cooked format identification | Partial | Mesh, material, scene, skeleton, and animation files have distinct 32-bit magic values and bounded readers validate layout. The shared header contains only magic—no explicit schema version, endian marker, payload size/hash, or producer identity. | `S` |
| `GF-006` | Scene registry | Implemented path | Maps stable scene IDs to relative cooked-manifest paths with load/save/upsert/release operations. It is a flat ordered map, not a dependency database. | `S` |
| `GF-007` | File lifetime during load | Implemented path | Each load work state owns one Core read scope before fresh catalog resolution through mapping/decoding and worker settlement; file sets borrow it and own their mappings. Decoded package data moves on successful settlement. Repeat-load residency remains unmeasured. | `S` |
| `GF-008` | Manifest validation | Implemented path | Loader/validator checks file presence, indices, references, payload consistency, and reports explicit load failure rather than silently constructing partial scene data. Exact malformed corpus coverage is unproven. | `S` |

### Explicit Level Selection And Publication

**Current runtime boundary, 2026-11-01:** `LevelRegistry` receives a project directory from `LevelSession` composition, discovers `Levels/<id>.level` documents and validates filename/document identity. It never reads `Levels.catalog`, selected flags, acquisition manifests or raw-source readiness. Launcher/Cooker share the existing catalog in ToolSupport; its selection affects preparation only. Existing cooked loaders validate the actual scene products on activation, identically after sync or local generation. [Native regression controls](../../../../../artifacts/validation/runtime-inputs-slim-20261101/completion.json) cover the previous mismatch between Launcher preflight and runtime admission, with no GPU/frame or new-archive claim.

**Source reconciliation:** 2026-10-31 at `e44692cce7eb33052b0a1ac763e66137a861639b`; `GF-002`/`GF-003` and their publication/shutdown edges only. This source trace adds no runtime or release verdict. [FWC-0 results](../../../../Acceptance/FeatureCompletionReports.md#foundation-contract-progress) preserve the original candidate evidence.

`LevelSession` owns requested-to-active level selection. Sponza is the selected reconciliation baseline: catalog ID `Sponza`, document `Projects/Showcase/Levels/Sponza.level`, authored name `Sponza`, scene asset ID `Sponza/Publisher/glTF/Sponza`. It does not change the default level, source content, settings or rendering quality. The tool catalog has Sponza unselected by default; this only affects an explicit source cook. Runtime can load a supplied Sponza document and its cooked dependencies without raw assets or that selection flag. A positive presented-frame claim still needs its actual rendering proof.

| Boundary | Current producer, publication and consumer |
| --- | --- |
| Request | Launcher's `LevelRunOperations` validates product/profile, API, catalog identity and required output presence, then sends `SPARKLE_STARTUP_LEVEL=Sponza` and the same `ChildProcessRequest::ReadinessValue` to the selected Game/Editor child. `LevelRegistry` registers valid documents present in `Levels` and rejects a name that differs from its filename identity. Source selection/readiness cannot hide supplied products. |
| Resolve and load | `LevelSession::InitializeStartupLevel` resolves the explicit name once. An unavailable Sponza leaves no active startup level and records a named diagnostic; an unavailable later request preserves the active level. The built-in `Empty` entry serves an absent/empty startup selector or an explicit Empty request; it never satisfies an explicit Sponza request. |
| Publish | `SceneLoadExecutor` refreshes its immutable catalog for each request under the same Core read scope used by every mapped asset, reads/decodes into an owned package and exposes only settled completion. An incomplete publication refuses loading and directs clean generated output followed by recooking; scope release follows worker settlement. `LevelSession` rejects failed/cancelled or stale world/document/catalog generations. On success `GameWorld` stages and swaps world/resource state; finalization precedes setting the active level and broadcasting `OnLevelChanged`. No partly loaded world is published. |
| Consume | Application forwards the changed active name to Core's parent-readiness signal, which requires equality with the requested value. This proves activation identity, not presentation. GameWorld extraction publishes scene/view data in `RenderFrameSubmission`; Application moves it to Renderer without Renderer reading ECS. LevelSession records an input diagnostic for Launcher/Editor on failure; a responsive window or `Empty` image cannot pass a Sponza check. |
| Shut down | Application releases Renderer before LevelSession, world and the Tasks runtime. Renderer closes/joins its threaded queues or destroys its serial context and settles device work. SceneLoadExecutor cancels and joins its scope before package/catalog storage is released; only then can world and the application executor be destroyed. Cancellation settlement is a required executable check, not a bound proved by these destructor calls. |

`LevelRegistry` owns document availability; `LevelSession` alone commits requested-to-active selection. Core readiness transports that result, and product acceptance requires the actual presented frame. Source import/cook and foundation dependencies retain their existing owners in the [eleven-family map](../../../../Acceptance/FeatureCompletionReports.md#foundation-world-and-content). Package admission and GPU retirement retain their Product/RHI owners.

| Selected failure/acceptance edge | Existing check owner and required observation |
| --- | --- |
| Unavailable Sponza document or missing/corrupt cooked dependency; `AC-PROD01-02` / `FM-PROD01-02` | [Showcase `CHK-PROD01-03`](../../Projects/Showcase/ShowcaseCapabilities.md#fcr-prod-01-game-consumer-contract): on an isolated candidate, positive Sponza has the expected scene/camera, bindings and presented output; a controlled missing/corrupt product names the failure and never activates false Empty. Intentional Empty remains valid. |
| Wrong/failed child activation; `AC-PROD03-03` / `FM-PROD03-02` | [Launcher `CHK-PROD03-02`](../../Tools/Launcher/LauncherCapabilities.md#fcr-prod-03-launcher-contract): follow the final child, matching readiness identity and actionable failure; process creation or Launcher exit alone cannot pass. |
| Quit with a load in flight; `AC-PROD01-05` / `FM-PROD01-05` | [Showcase `CHK-PROD01-02`](../../Projects/Showcase/ShowcaseCapabilities.md#fcr-prod-01-game-consumer-contract), with [Editor `CHK-PROD04-01`](../Editor/EditorCapabilities.md#fcr-prod-04-editor-contract) for the shared Editor route: establish the pending load, quit, observe native exit and no remaining owned work within the existing product budgets. |

These checks retain their Windows x64 Game/Editor and D3D12/Vulkan product matrices and exact executable, content, shader and settings identities. Source-workspace results remain attached to their original candidates; installed equivalents belong to PD-3. Linux, uncooked runtime loading, source import selection, worker-count equivalence and shader-contract changes are outside this reconciliation. Broader switching/generation proof remains `GF-E01`/`GF-E02` in the [evidence plan](../../CapabilityEvidencePlan.md#world-content-and-tool-evidence).

## World, ECS, And Simulation

| ID | Capability | State | Exact current coverage and limit | Evidence |
| --- | --- | --- | --- | --- |
| `GF-009` | Entity identity/lifetime | Implemented path | Generation-aware entity IDs and registry-backed component storage; private alive checks validate typed edits, and private destruction rolls back failed scene construction. World replacement retires the previous scene. No public arbitrary entity/component creation or destruction API is exposed. | `S` |
| `GF-010` | Fixed component types | Implemented path | 13 private runtime component types: LocalTransform, WorldTransform, CameraDerivedState, MeshInstance, Visibility, Camera, Light, AnimationState, MorphState, SkinningState, Name, AuthoredIdentity, and EditorMetadata. | `S` |
| `GF-011` | Typed queries and frozen structure | Implemented path | Read/write query descriptors declare system access; world structure is frozen for a system epoch and structural changes commit outside it. This is a purpose-built ECS, not a generic public ECS framework. | `S` |
| `GF-012` | Compiled system graph | Implemented path | Eleven stages execute camera movement; playback; pose; morph; skinning; morph commit; system-output commit; transforms; camera derived state; mesh extraction; extraction commit. Access and dependency topology are compiled; populated ranges are materialized at host wave boundaries. | `S` |
| `GF-013` | Parallel world evaluation | Implemented path | Camera, animation, pose, transform, and extraction systems use explicit grain/serial/partition limits through Tasks. Deterministic numerical output under different worker counts is not yet evidenced. | `S` |
| `GF-014` | Transform hierarchy evaluation | Implemented path | Dirty local/world transforms propagate through hierarchy; full reevaluation is available after scene commit; inverse-transpose data is produced for rendering. | `S` |
| `GF-015` | Camera simulation | Implemented path | Camera input intent drives navigation; derived world matrix, direction, aspect, visibility, and active camera are published. Perspective and orthographic vocabulary exists, but importer/runtime coverage differs. | `S` |
| `GF-016` | Lighting model | Implemented path | Directional, point, spot, and rectangular lights with physical/intensity fields plus optional sky environment flow from level/import through read view and render submission. Capacity/rendering limits belong to Renderer. | `S` |
| `GF-017` | Material model and variants | Implemented path | Material descriptions and handles, scene material tables, named imported variant sets, active variant selection, and per-instance material rebinding are represented and published. Blend rendering remains unsupported downstream. | `S` |
| `GF-018` | Static/instanced meshes | Implemented path | Immutable mesh resources, static mesh instances, authored/shared instance-group metadata, visibility, transforms, material bindings, and structural create/update/destroy extraction. | `S` |
| `GF-019` | Skeletal animation | Implemented path | Skeleton hierarchy, animation clips, linear/step/cubic sampling, playback advance, pose evaluation, up to eight imported influences, and joint-matrix ranges reach Renderer. | `S` |
| `GF-020` | Morph animation | Implemented path | Per-target position/normal/tangent deltas, default/per-instance weights, animation weight channels, evaluated morph outputs, and render weight ranges reach Renderer. | `S` |
| `GF-021` | World edits | Implemented path | Generation-checked commands can set active camera, local transform, camera description, visibility, light description, sky, and active material variant; result is Accepted/Stale/Rejected with message. | `S` |
| `GF-022` | Read snapshots/change journal | Implemented path | Immutable shared `WorldReadView` snapshots expose cameras/lights/meshes/sky; sequence cursors read and acknowledge change batches. Retention/bounded-history behavior needs long-run evidence. | `S` |

## Render Publication Boundary

| ID | Capability | State | Exact current coverage and limit | Evidence |
| --- | --- | --- | --- | --- |
| `GF-023` | Structural publication | Implemented path | Scene generation/sequence, reset, object creates/updates/destroys, immutable material/texture tables, instance groups, and sky are published only when structurally required. | `S` |
| `GF-024` | Dynamic publication | Implemented path | Every frame can publish object transforms/visibility, all light descriptions, joint-matrix ranges/data, and morph-weight ranges/data. | `S` |
| `GF-025` | View publication | Implemented path | Camera data plus camera-cut/teleport flags are separate from scene data, allowing Renderer to invalidate view history without conflating scene ownership. | `S` |
| `GF-026` | Stable render identity | Implemented path | World entities map to generation-aware `RenderObjectId` values and immutable resource handles. Identity continuity across repeated load/reload remains an evidence item. | `S` |

## Vertical Frame Trace

Application publishes input -> `GameWorld::Update` freezes ECS structure and runs the 11-stage system graph -> system output commits changes -> `ExtractRenderFrameSubmission(frameId)` emits structural delta, dynamic arrays, and view input -> Application optionally replaces only view camera data for the editor -> Renderer consumes the moved submission and owns GPU publication.

## Explicit Non-Capabilities And Risks

- No physics, collision, audio, networking, scripting, gameplay class hierarchy, prefab system, generic public ECS authoring API, particle system, navigation mesh, or save-game system was found.
- Cooked assets have magic but no explicit version field; stale/incompatible rejection therefore depends on exact current layout validation and operational recooking.
- Runtime is cooked-only, but package independence, missing-registry behavior, repeated cancellation, and total decoded/GPU residency remain unproved.
- Editor-visible world edits cover a fixed property subset; entity creation, arbitrary component editing, material authoring, and mesh replacement are not current capabilities.
