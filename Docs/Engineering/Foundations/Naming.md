# Naming and Vocabulary

[Docs](../../README.md) / [Up](README.md)

**Status:** binding naming and cross-boundary vocabulary standard

**Applies to:** owned C++, shaders, serialized fields, profiler labels, files, and public documentation

## General Naming Baseline

- Types, functions, and methods use `UpperCamelCase`.
- Local variables and parameters use `lowerCamelCase`, except boolean variables use `b` followed by `UpperCamelCase` (for example `bSourceReady`). Apply the boolean convention consistently to locals, parameters, fields, constants and atomic boolean state. Preserve an existing scope prefix before `b`, for example `m_bReady` or `s_bInitialized`; do not introduce a scope prefix where the owner does not use one. External serialized/native spellings retain their contract names; update owned producers and consumers together when a source identifier changes.
- Private data members use `m_` plus descriptive lower camel case.
- Booleans describe a state or predicate. The `b` prefix follows [Epic's boolean naming convention](https://dev.epicgames.com/documentation/en-us/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine); Sparkle keeps its existing scope-prefix conventions. The prefix is a type-specific rule; the configured camel-back local naming check also accepts it.
- Apply the boolean prefix to C# source-check helpers as well; retain their existing naming conventions for other identifiers.
- Use the surrounding function, type and capability folder as naming context. Prefer a short domain fact such as `bPackAvailable` over repeating the entire operation name. Avoid generated `Evaluation`, numbered stages, concatenated variable/member/call names such as `bSecondaryJointsReadyGuardreaduint`, and repeated context in intermediate names. A guarded operation can use a short intent such as `bReadSecondary`. Expand a name only when it resolves a real ambiguity; do not shorten it into an unexplained abbreviation.
- Enum values use `UpperCamelCase`.
- Macros are reserved for build/export/preprocessor needs and follow existing module spelling.
- Acronyms use canonical Sparkle casing such as `Rhi`, `Gpu`, and `Io` in C++ names unless an external ABI requires exact spelling.

Use one responsibility per noun and one canonical term per responsibility. The broader the scope and lifetime of a name, the more precise it must be.

## Meaning-Bearing Suffixes

- `Handle` identifies something.
- `Token` proves ordering, completion, or authority.
- `Lease` is a temporary exclusive borrow.
- `Context` is transient call/run state, not a service locator.
- `Scope` binds work lifetime and settlement.
- `Lane` is scheduling policy, not correctness.
- `Graph` is reusable topology.
- `Execution` is one run.
- `Desc` is immutable/configuration description.
- Queue names state the payload they carry.

Use distinct verbs:

- `Build` or `Compile` creates a plan or immutable product.
- `Record` writes commands.
- `Submit` transfers work to an executor or GPU queue.
- `Execute` performs a task or plan.
- `Publish` makes immutable state visible.
- `Commit` mutates authority at a boundary.
- `Apply` consumes an accepted command or delta into owned state.
- `Extract` transforms authority into a derived boundary representation.
- `Retire` delays reuse/destruction until its completion condition.
- `Find` searches for an existing match; `Resolve` determines or validates a canonical value/path. Neither implies opening a document, loading an asset or acquiring a resource.

Avoid vague owners such as `Manager`, `System`, or `Services` unless the name states the domain and the type truly coordinates it. Prefer capability names such as evaluator, sampler, builder, compiler, planner, allocator, registry, publisher, committer, coordinator, encoder, decoder, or store.

## Canonical Concurrency and Rendering Terms

Use the vocabulary defined by the multithreading architecture:

- `Task`, `TaskDesc`, `TaskNodeHandle`;
- `CompiledTaskGraph`;
- `TaskExecution`, `TaskExecutionContext`, `TaskExecutor`;
- `TaskScope`, `TaskEvent`, `ParallelFor`, `TaskLane`;
- `GameThread`, `EditorThread`, `RenderThread`;
- `RenderCoordinator`, `RenderFrameQueue`, `RenderThreadCommandQueue`;
- `RenderCommandContext`;
- backend-private `D3D12CommandRecordingContext` and `VulkanCommandRecordingContext`;
- `RhiCommandRecordingLease`, `ERhiQueueType`, `RhiSubmissionToken`;
- `FrameGraphSubmissionBatch`, `RecordingGroup`.

Shared graphics contracts use the same vocabulary at every boundary:

- Backend implementations of an `Rhi*Service` are `D3D12*Service` or `Vulkan*Service`; contained allocators remain `*Allocator`.
- `RendererHost` owns composition lifetime. `RendererBackendOwner` owns `RenderDeviceServices`. Callers use `deviceServices`; a `RenderHardwareInterface` reference is `renderHardwareInterface`.
- `TextureCache`, `MaterialCache`, and `RenderPassRuntimeCache` are persistent generation/revision-keyed caches. Do not reintroduce `*Manager` variants.
- `RenderCommandContext` is the backend-neutral command-recording wrapper. `PassCommandContext` is the narrower frame-graph pass recording surface and contains only command, declared-resource, and diagnostic infrastructure.
- General renderer graph topology uses `BuildRenderFrameGraph`, `RenderFrameGraphSettings`, and `RenderFrameGraphResources`. Do not encode a shading technique such as deferred, forward, path traced, or hybrid in a renderer-wide graph owner; technique names belong only to the feature passes and policy they actually describe.
- `RenderFrame` is one frame-slot scene/view value, while `RenderFrameGraph*` names graph topology and handles. Do not shorten either responsibility to ambiguous `Frame*` assembly/build records.
- Pass-specific parameter structs carry semantic frame, scene, view, ray-tracing, provider, history, and display inputs. Broad semantic context bags are prohibited; do not introduce a service bag under a different `*Context`, `*Services`, or `*Resources` name.
- `SourceImportOutput` contains imported content plus `SourceImportProvenance`; import failure is `Diagnostics::Error`. Prefer `output` or `importOutput`, not `result`.
- Use `GpuMesh`, `GpuMeshCache`, and `commandContext`; do not use `GPU*` casing or `cmd` in owned neutral code.
- Use `TLAS` and `PTLAS` in rendering prose, UI, diagnostics and labels, with `Tlas` and `Ptlas` in owned C++ names and filenames. Use the same short terms in configuration keys. Keep exact SDK identifiers only at external API boundaries and in source URLs.
- Neutral render-target APIs use `renderTarget`, `renderTargets`, `renderTargetCount`, and `depthStencil`; `rtv`/`dsv` remain native D3D boundary terms.
- Neutral pipelines are `RenderPipeline` described by `GraphicsPipelineDesc` or `ComputePipelineDesc`, created with `Create*Pipeline`, and bound with `SetPipeline`. `PipelineState`, `PSO`, native pipeline types, and `SetPipelineState` do not leak into neutral contracts.
- `LevelSession` owns registered/active/pending/loading level lifetime. `SceneLoadExecutor` owns work that produces `SceneLoadPackage`; shared payload is `SceneLoadWorkState`.
- `EditorTransactionHistory` owns undo/redo stacks and coalescing; panels borrow `transactionHistory`.
- `WorkspaceRoot` / `workspaceRoot` identifies the ordinary marker-based tree shared by a source checkout and portable extraction. `RepositoryRoot` names the source repository in build/provenance operations; shared consumer discovery must not imply that Git or source files are required. Launcher discovery is `WorkspaceLocator`; explicit-root validation is `TryResolveWorkspaceRoot`, and ancestor search is `TryFindWorkspaceRoot`.
- Application's `EditorShaderCompileOperation` owns background-operation lifetime, UI status and successful reload. The tooling compile action owns compiler invocation; an operation wrapper is not named as though it were the compiler.

## Quantities, Transforms, and ABI

- Preserve physical light meaning: directional `illuminance`, point/spot `luminousIntensity`, area `luminance`, angular `*Radians`, sky/environment `brightness`.
- `JointMatrix` names stored per-joint transforms and packed ranges; `Skinning` names blending; `SkinInfluence` names indices and weights.
- `MorphTarget` is a target shape; `MorphWeight` is its contribution. Use `MorphWeightRange`, `MorphWeightCopyRange`, and `MorphWeightHistory` for those exact responsibilities.
- `GpuSceneSlot` addresses a transient GPU-scene slot. Preserve it through CPU/shader mirrors and hit reconstruction; it is not identity or `DebugData`.
- Preserve `WorldMatrix`, `PreviousWorldMatrix`, and `WorldInverseTranspose` across C++ and HLSL.
- Temporal transforms use directional coordinate-space names: `PreviousWorldToViewMatrix`, `PreviousViewToClipMatrix`, and `PreviousWorldToClipMatrix`.
- Camera projection contracts use `FovY`, `NearZ`, and `FarZ`; append `Degrees` or `Radians` where types do not carry the unit.
- Scalar time crossing a boundary names its unit, such as `DeltaTimeSeconds`.
- Preserve `ViewportSizeInv` for reciprocal viewport extent; matrix inverses use directional spaces.
- Temporal offsets name their space: `CurrentJitterNdc` and `PreviousJitterNdc`.
- CPU/shader ABI mirrors use identical field names unless a toolchain restriction has an explicit documented mapping. Paired renames require C++/HLSL edits, layout checks, and all target shader compilations.

## Identity Vocabulary

Do not call every identity a GUID.

- `Guid` is a 128-bit persistent value primitive, not a global manager.
- `AssetGuid`, `AuthoredInstanceGuid`, and `AuthoredObjectGuid` are persistent authoring identities.
- Derived `Source*Id` hashes are source lookup/reimport keys, not GUIDs.
- `EntityId` is compact runtime ECS identity with stale-handle generation.
- `RenderObjectId` is separate renderer identity created by extraction.
- Runtime resource handles identify immutable assets.
- Content hashes identify bytes/options for verification and caching, not authored objects.
- `Index` addresses a contiguous element; `Id` expresses identity.

Storage, resolution, and hot-path rules for these identities belong to [Data-Oriented Design](DataAndMemory.md#identity-and-references); this section owns only their canonical vocabulary.

## Rejected Patterns

Do not introduce:

- `New*`, `*2`, or `Legacy*` used to keep both a replacement and an obsolete Sparkle-owned concept alive;
- `Async*`, `ThreadSafe*`, `LockFree*`, `MultiThreaded*`, or `MT*` as vague claims;
- `SceneAnimation*` for distinct sampling, pose, morph, skinning, or output responsibilities;
- `Data`, `Info`, `Thing`, `Object`, `Manager`, `Helper`, or `Util` when a precise responsibility exists;
- overloaded “queue” names that blur CPU tasks, render control, RHI command lists, and GPU queues;
- `Snapshot` for broad mutable-world copies; use a precise packet, read model, immutable state, or diagnostics product.

The [current clean-break policy](../Workflow/ChangeIntegration.md#current-clean-break-policy) does not allow temporary compatibility names or deferred renames. Rename all owned producers and consumers atomically and delete the obsolete spelling.
