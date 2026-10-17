# GPU Crash Debugging Research

[Docs](../../../../README.md) / [Design decisions](GpuCrashDebuggingDesign.md) / [Implementation stages](ImplementationPlan.md)

**Status:** primary-source research and comparison, inspected 2026-10-17. Not an implementation or native acceptance result.

**Scope:** NVIDIA Aftermath samples/NVRHI, AMD RGD/AGS/FidelityFX Breadcrumbs, Epic's published RHI and shader-debugging contracts, supporting specifications and engineering presentations. AMD is a design reference; an AMD collector, SDK dependency or hardware purchase is not admitted by this study.

## Findings That Change The Design

1. Reuse the existing GPU scope and RHI boundaries. A vendor library belongs below them; a crash collector does not belong in frame orchestration.
2. Preserve native evidence before analysis. Do not copy a sample's synchronous decoding, global shader directory scan or callback file I/O into the failing application.
3. Marker storage needs a lifetime proof. A four-frame history, a small recently-destroyed list, or a completed CPU recording does not prove that a later crash callback cannot reference a token. The refined design retains immutable semantic tokens for the whole session instead of reclaiming them per frame.
4. Preserve full shader debug information and exact active bytes. NVIDIA and AMD do not accept identical symbol formats; a CPU PDB does not supply GPU shader mapping.
5. Separate GPU-observed progress, CPU recording history and shader fault attribution. Missing page-fault data does not prove an infinite loop; the last breadcrumb does not prove the faulty command.

The [design](GpuCrashDebuggingDesign.md#architecture-and-lifetimes) owns Sparkle's selected behavior. This page records the evidence, alternatives and limitations behind it. The [decision crosswalk](#decision-crosswalk) links findings to delivery and checks.

Read by subject: [NVIDIA](#nvidia-sdk-sample-versus-production-rhi), [AMD](#amd-analysis-breadcrumbs-and-resource-history), [Epic](#epic-engine-level-responsibilities), [presentations](#technical-presentations-and-their-limits), [comparison](#mechanism-comparison), [fault matrix](#fault-and-evidence-matrix).

## Evidence And Access

Repository observations use immutable commits, not their current default branches. Selected source files and web/PDF receipts are retained under ignored `artifacts/validation/gpu-crash-debugging/20261017-research`; vendor source is not added to Sparkle's production tree.

| Reference implementation | Revision inspected | Access / provenance boundary |
| --- | --- | --- |
| NVIDIA Aftermath D3D12 and Vulkan samples | [d3585be2c58b1c23ff29a77713b0d04c2de8b70a](https://github.com/NVIDIA/nsight-aftermath-samples/tree/d3585be2c58b1c23ff29a77713b0d04c2de8b70a) | Read tracker, shader database and application integration. Sample licenses do not replace the separate SDK runtime license. |
| NVIDIA NVRHI | [6b96fb03e07539f08327aea76c56d55f1de9d906](https://github.com/NVIDIA-RTX/NVRHI/tree/6b96fb03e07539f08327aea76c56d55f1de9d906) | Read common marker helpers, native device/command-list integration and SDK acquisition. MIT source; SDK terms remain separate. |
| NVIDIA Nvpro Vulkan crash sample | [1ca74a9c2b459848fded141dc14392dcc9e88029](https://github.com/nvpro-samples/vk_mini_samples/tree/1ca74a9c2b459848fded141dc14392dcc9e88029/samples/crash_aftermath) | Read sample, Slang fault shader and scenario documentation. Use only as a controlled-fault reference; it was not built or run. |
| AMD Radeon GPU Detective | [2314d0945329576522b71dbb9331fa018ba5b806](https://github.com/GPUOpen-Tools/radeon_gpu_detective/tree/2314d0945329576522b71dbb9331fa018ba5b806) | Read marker/resource/code-object analysis and the manual. Open analysis code does not establish access to every driver-side collector implementation. |
| AMD AGS | [086c47ed8e427199510887230718cd7c086f9702](https://github.com/GPUOpen-LibrariesAndSDKs/AGS_SDK/tree/086c47ed8e427199510887230718cd7c086f9702) | Read public marker declarations and DX12 sample. API capability must be checked; its existence is not NVIDIA support. |
| AMD FidelityFX SDK 1.1.4, Breadcrumbs 1.0.1 | [c6efa6bf7f2027b3ec94f28578bb5965eabb9e55](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/tree/c6efa6bf7f2027b3ec94f28578bb5965eabb9e55) | Read component, public/private headers and DX12/Vulkan lowering. This version contains the studied library; current HEAD is not substituted for it. |
| Epic Unreal Engine | Public UE 5.5 RHI API; UE 5.6 crash guide; current shader/reporting documentation | Documentation/API inspection only. No authenticated Epic source checkout was available; private backend implementation and teardown details were not inspected. Do not describe them as audited source. |

Local Sparkle source was rechecked at `343d9e4fe112f10e5b05a46c45532b8a8a061d62` plus the worktree. Existing owners are listed in the [design's source audit](GpuCrashDebuggingDesign.md#current-route-and-gaps). This iteration changes documentation only and leaves GPU hang reproduction paused.

Local integration constraints: `FrameExecutionDiagnostics` checks RHI event capability; pass/chunk admission also checks existing marker verbosity. D3D12 scope calls currently compile under the PIX-event guard; Vulkan uses debug-utils scopes. Therefore native Aftermath code alone would not establish baseline breadcrumbs. The design adds one neutral scope-admission requirement at those existing owners, independent of timing/capture/validation, and keeps vendor conditions out of frame orchestration.

## NVIDIA: SDK Sample Versus Production RHI

### SDK Contract

The [SDK guide](https://docs.nvidia.com/nsight-aftermath/SDK/index.html) specifies collection setup before native devices, separate per-device configuration, asynchronous callbacks and a wait for dump completion after device loss. Application-managed markers need resolvable tokens; copied payloads have a size limit. Sparkle therefore needs one early process session, native backend lowering and callback lifetime that survives terminal device loss. These are SDK obligations, not evidence that Sparkle already satisfies them.

The pinned header does not specify that `GFSDK_Aftermath_DisableGpuCrashDumps` joins callbacks or makes immediate DLL unload safe. The design retains a bounded closed callback anchor, marker tokens and DLL until process exit; it does not infer a shutdown guarantee from the sample destructor. The Application sink still requires its own admission/borrow settlement protocol.

The [2026.3 release notes](https://developer.nvidia.com/nsight-aftermath/getting-started/release-note-v2026.3) and installed headers govern exact APIs. In particular, `GFSDK_Aftermath_DX12_UpdateResourceInfo` is the current resource-metadata path; older resource-registration code must not be copied without checking its SDK revision. NVRHI's [pinned acquisition file](https://github.com/NVIDIA-RTX/NVRHI/blob/6b96fb03e07539f08327aea76c56d55f1de9d906/cmake/FetchAftermath.cmake) names the matching API generation and versioned packages. Its MD5-based acquisition is precedent for explicit identity, not Sparkle's checksum policy: retain SHA-256 receipts.

That SDK release also documents richer resource timelines on R615 and known attribution limits: CUDA-related workloads can yield a dump in a graphics application, and a legacy page-fault query can select a different overlapping resource. Keep fault origin and resource candidates distinct; do not infer a local rendering defect from the watched API or a single VA match.

### Sample Crash Tracker

The [D3D12 tracker](https://github.com/NVIDIA/nsight-aftermath-samples/blob/d3585be2c58b1c23ff29a77713b0d04c2de8b70a/D3D12HelloNsightAftermath/NsightAftermathGpuCrashTracker.cpp) demonstrates callback registration, synchronized shader-debug storage, marker resolution, raw dump writing and JSON decoding. It opts into deferred debug callbacks and writes/decodes during callbacks. It also includes command-line context. These are useful demonstrations, but increase failure-path work and privacy exposure if copied wholesale.

**Transfer:** register the necessary callbacks; retain NVIDIA shader-debug identifiers; preserve the raw dump and its candidate identity. **Reject:** callback-time JSON generation, arbitrary command-line capture, unbounded shader-debug maps and current-directory publication. Sparkle's task executor owns asynchronous file work, but inspection found that current TaskScope registration requires its owner thread, workers cannot submit to their executor, and scheduler allocation failure can call Fatal. Direct callback-side Launch is therefore unsuitable. A parked writer or private subsystem worker also violates its [Tasks standard](../../../../Engineering/Modules/Tasks.md). The design therefore selects the Tasks-owned [prepared dispatch contract](../../MultithreadedEngine.md#prepared-task-dispatch): pre-admit on the scope owner, then notify without allocation or a parked worker. GCD-2 implements it; this decision is not execution proof. SDK completion, sink-borrow settlement and durable writes remain separate obligations.

The [sample application](https://github.com/NVIDIA/nsight-aftermath-samples/blob/d3585be2c58b1c23ff29a77713b0d04c2de8b70a/D3D12HelloNsightAftermath/D3D12HelloNsightAftermath.cpp) polls dump status for a three-second window after loss and keeps four frames of application-managed marker history. This illustrates asynchronous finalization and bounded history; neither number proves safety for Sparkle's parallel queues. Use its controlled-fault idea only in disposable validation fixtures. Sparkle's collection budget bounds cooperative waits, not an unreturning SDK/driver/OS call. External test termination is containment, not proof of successful finalization.

The [Vulkan tracker](https://github.com/NVIDIA/nsight-aftermath-samples/blob/d3585be2c58b1c23ff29a77713b0d04c2de8b70a/VkHelloNsightAftermath/NsightAftermathGpuCrashTracker.cpp) repeats the same collector responsibilities for Vulkan. The [Vulkan application](https://github.com/NVIDIA/nsight-aftermath-samples/blob/d3585be2c58b1c23ff29a77713b0d04c2de8b70a/VkHelloNsightAftermath/VkHelloNsightAftermath.cpp) supplies checkpoints and device diagnostics configuration. Transfer the native extension/configuration order; do not copy automatic checkpoint/call-stack settings, sample startup plumbing or feature-reducing crash controls into normal application code.

The official [SDK sample index](https://docs.nvidia.com/nsight-aftermath/SDK/samples.html) also points to Nvpro's `crash_aftermath`. Its [pinned scenarios](https://github.com/nvpro-samples/vk_mini_samples/blob/1ca74a9c2b459848fded141dc14392dcc9e88029/samples/crash_aftermath/README.md), [integration](https://github.com/nvpro-samples/vk_mini_samples/blob/1ca74a9c2b459848fded141dc14392dcc9e88029/samples/crash_aftermath/crash_aftermath.cpp) and [Slang shader](https://github.com/nvpro-samples/vk_mini_samples/blob/1ca74a9c2b459848fded141dc14392dcc9e88029/samples/crash_aftermath/shaders/crash_aftermath.slang) distinguish timeout loops from buffer-device-address faults and direct/indirect stale pointers. Side effects keep a test loop from disappearing under optimization. Use that distinction when selecting disposable GCD-4 fixtures; do not copy the permanent crash UI, randomized address writes or registry instructions. Sample reproducibility claims still need qualification on Sparkle's exact compiler/driver.

### NVRHI Ownership And Marker Retention

NVRHI's [common helper contract](https://github.com/NVIDIA-RTX/NVRHI/blob/6b96fb03e07539f08327aea76c56d55f1de9d906/include/nvrhi/common/aftermath.h) separates command-list marker tracking from device-level lookup assistance. Its [implementation](https://github.com/NVIDIA-RTX/NVRHI/blob/6b96fb03e07539f08327aea76c56d55f1de9d906/src/common/aftermath.cpp) stores full nested event paths, caps event strings at 128 and retains two destroyed trackers. It hashes names and searches tracker collections for resolution.

**Benefit:** vendor logic stays close to native command lists; nested semantic labels help locate rendering work. **Limit:** fixed-size eviction is not a GPU/callback retirement proof; name hashes alone are not collision-free identities. The inspected common helper exposes no internal synchronization around those collections, so its caller assumptions must not be generalized to Sparkle's concurrent recording/callbacks.

One source-level lifetime hazard also deserves review: `ResolveMarker` iterates destroyed trackers by value, then returns a reference wrapper into the copied tracker. I infer that this can outlive its owner; no executable upstream test was run. Sparkle must not return borrowed storage from such a temporary.

Sparkle keeps the ownership pattern, not the helper registry. Use existing scope entry/exit, copied D3D12 payloads and a bounded immutable Vulkan semantic-token dictionary. Repeated frames reuse the same semantic token rather than allocate new strings indefinitely. A Vulkan checkpoint cannot truthfully supply a unique frame from a repeated token alone.

Native examples are in [D3D12 device setup](https://github.com/NVIDIA-RTX/NVRHI/blob/6b96fb03e07539f08327aea76c56d55f1de9d906/src/d3d12/d3d12-device.cpp), [D3D12 command lists](https://github.com/NVIDIA-RTX/NVRHI/blob/6b96fb03e07539f08327aea76c56d55f1de9d906/src/d3d12/d3d12-commandlist.cpp), [Vulkan device state](https://github.com/NVIDIA-RTX/NVRHI/blob/6b96fb03e07539f08327aea76c56d55f1de9d906/src/vulkan/vulkan-device.cpp) and [Vulkan command lists](https://github.com/NVIDIA-RTX/NVRHI/blob/6b96fb03e07539f08327aea76c56d55f1de9d906/src/vulkan/vulkan-commandlist.cpp). They justify backend-private hooks; they do not justify native API conditions in Sparkle's `FramePipeline`.

### Symbols, Cost And Tool Conflicts

The [sample shader database](https://github.com/NVIDIA/nsight-aftermath-samples/blob/d3585be2c58b1c23ff29a77713b0d04c2de8b70a/D3D12HelloNsightAftermath/NsightAftermathShaderDatabase.cpp) joins shader binaries to native hashes and debug data to names, but discovers files by scanning sample shader directories. Transfer the lookup identities; replace scanning with Sparkle's existing compiler/export manifest. No second runtime shader database is needed.

The [inspector search-path documentation](https://docs.nvidia.com/nsight-aftermath/UserGuide/ui-global-options-dialog.html) distinguishes shader code, source/debug files, NVIDIA `.nvdbg` and CPU symbols. Embedded NVIDIA debug data is a newer-driver behavior; driver 610.47 still needs retained callback data. Missing one join must remain visible rather than trigger a search of unrelated builds.

The [Aftermath limitations](https://docs.nvidia.com/nsight-aftermath/ReleaseNotes/index.html) exclude simultaneous active PIX graphics analysis and do not support source-line mapping from DXC slim `-Zs` PDBs. D3D12's debug layer is supported; that does not qualify every GPU-validation/interposer combination. PIX event labels and an active PIX capturer are distinct. Keep compatible event labels, reject active capture with Aftermath, and qualify validation separately.

NVIDIA's [debugging guidance](https://developer.nvidia.com/blog/advanced-api-performance-debugging/) recommends semantic checkpoints and shader debug information while acknowledging instrumentation cost. Sparkle measures shader preparation, recording, memory, disk and finalization costs separately. No published low-overhead claim becomes a Sparkle performance result.

The [inspector/setup guide](https://docs.nvidia.com/nsight-aftermath/UserGuide/gpu-crash-dump-setting-up.html) explains shader-error reporting for otherwise hidden invalid accesses and the overhead/privacy cost of automatic CPU stacks. Select error reporting in the explicit diagnostic mode; leave automatic stacks off. More detected faults are useful evidence, but change the detection policy. Do not confuse this hardware reporting flag with full GPU-based shader validation or assume register inspection is available in every Nsight edition.

## AMD: Analysis, Breadcrumbs And Resource History

### Radeon GPU Detective

The [current RGD quickstart](https://gpuopen.com/manuals/rgd_manual/) identifies **v1.6.3**, Windows 11, supported RDNA hardware, D3D12/Vulkan and a TDR requirement. This supersedes the D3D12-only scope of the [historical RGD 1.0 tutorial](https://gpuopen.com/learn/rgd_1_0_tutorial/). The latter also warns that crash-analysis mode changes page-fault behavior. Tool mode is therefore part of the candidate record; a diagnostic run is not automatically equivalent to ordinary execution.

The [pinned execution-tree serializer](https://github.com/GPUOpen-Tools/radeon_gpu_detective/blob/2314d0945329576522b71dbb9331fa018ba5b806/source/radeon_gpu_detective_backend/rgd_exec_marker_tree_serializer.cpp) reconstructs command-buffer hierarchy, nested submissions and marker progress. The [data types](https://github.com/GPUOpen-Tools/radeon_gpu_detective/blob/2314d0945329576522b71dbb9331fa018ba5b806/source/radeon_gpu_detective_backend/rgd_data_types.h) preserve event occurrence timing and use indices to avoid pointer invalidation when storage moves.

**Transfer:** distinguish recorded, not-started, in-progress, finished and unknown evidence; keep queue/submission identity and stable references. **Do not transfer:** the entire offline analysis object model into the rendering process, or a global order inferred by merging parallel queue markers.

The [resource serializer](https://github.com/GPUOpen-Tools/radeon_gpu_detective/blob/2314d0945329576522b71dbb9331fa018ba5b806/source/radeon_gpu_detective_backend/rgd_resource_info_serializer.cpp) retains create/bind/destroy and virtual-address history. The [code-object database](https://github.com/GPUOpen-Tools/radeon_gpu_detective/blob/2314d0945329576522b71dbb9331fa018ba5b806/source/radeon_gpu_detective_backend/rgd_code_object_database.cpp) and [enhanced crash serializer](https://github.com/GPUOpen-Tools/radeon_gpu_detective/blob/2314d0945329576522b71dbb9331fa018ba5b806/source/radeon_gpu_detective_backend/rgd_enhanced_crash_info_serializer.cpp) join shader analysis to crash facts.

**Benefit:** an address can be related to historical allocations and shader work. **Limit:** reused addresses, aliasing and missing events can leave several candidates. Sparkle should preserve available driver/DRED resource facts and existing object names, not build another per-resource recorder or label the first overlapping allocation as the culprit.

The [RGD manual](https://gpuopen.com/manuals/rgd_manual/help_manual/) documents hardware-crash analysis and DXC embedded, separate and slim debug formats. A running wave can be correlated with a shader, but not every crash has such a shader. AMD's slim-PDB support must not be generalized to Aftermath. Rich register capture also increases artifact size. Sparkle's first delivery uses full debug data and an explicit unavailable mapping state; it does not promise register-level debugging for every GPU fault.

### AGS Markers

The [AGS header](https://github.com/GPUOpen-LibrariesAndSDKs/AGS_SDK/blob/086c47ed8e427199510887230718cd7c086f9702/ags_lib/inc/amd_ags.h) defines D3D12 Push/Pop/Set marker operations and requires the reported user-marker capability. This is another example of lowering semantic events at the native command-list boundary. It is not a reason to put AGS calls beside every render pass. Existing `BeginDiagnosticScope/EndDiagnosticScope` is the reusable integration seam; AGS itself remains outside this implementation scope.

### FidelityFX Breadcrumbs

The [Breadcrumbs documentation](https://gpuopen.com/manuals/fidelityfx_sdk/techniques/breadcrumbs/) and [pinned component](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/c6efa6bf7f2027b3ec94f28578bb5965eabb9e55/sdk/src/components/breadcrumbs/ffx_breadcrumbs.cpp) demonstrate nested markers, command-list/submission identity, configurable frame history and optional pipeline associations. Some operations require single-threaded ownership even when recording synchronization is enabled. This is a useful lifecycle reference, not a thread-safe service to copy blindly.

The [D3D12 backend](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/c6efa6bf7f2027b3ec94f28578bb5965eabb9e55/sdk/src/backends/dx12/ffx_dx12.cpp) uses crash-surviving host memory where available and `WriteBufferImmediate` marker-in/marker-out operations. The [Vulkan backend](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/c6efa6bf7f2027b3ec94f28578bb5965eabb9e55/sdk/src/backends/vk/ffx_vk.cpp) uses supported AMD marker commands or a buffer-fill path. Stage selection, queue capability, visibility and post-loss memory accessibility matter; these implementations cannot be treated as equivalent generic writes.

**Benefit:** readable progress trees without requiring a rich vendor dump. **Cost:** another GPU buffer protocol, synchronization/retirement state, backend lowering and failure-surviving allocation. **Decision:** study the begin/end and lifetime semantics; do not import FidelityFX or add a parallel custom breadcrumb buffer in this Aftermath slice. Existing DRED plus Aftermath/checkpoints are the selected mechanisms. Missing native evidence stays unavailable, rather than invoking an unqualified custom fallback.

## Epic: Engine-Level Responsibilities

The [UE 5.6 crash guide](https://dev.epicgames.com/documentation/unreal-engine/dealing-with-a-gpu-crash-when-using-unreal-engine?application_version=5.6) separates CPU-side detection from GPU cause, documents queue-oriented RHI breadcrumbs using `WriteBufferImmediate`, and distinguishes them from DRED and Aftermath. It also uses intentional faults to verify diagnostics. Transfer independent evidence sources and controlled qualification; do not equate an active pass with proof of fault or change Sparkle's rendering features to declare success.

The public [UE 5.5 `FRHIBreadcrumbNode`](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/RHI/FRHIBreadcrumbNode?application_version=5.5) describes hierarchy, per-pipeline begin/end tracking and crash-context writing. [`FRHIBreadcrumbAllocator`](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/RHI/FRHIBreadcrumbAllocator?application_version=5.5) separates node allocation/lifetime from clients. These support keeping instrumentation identity below RHI with explicit lifetime. They do not expose enough implementation to prove Epic's retirement algorithm, callback synchronization or GPU-memory guarantees; Sparkle needs its own checked contract.

Epic's [shader debugging workflow](https://dev.epicgames.com/documentation/unreal-engine/shader-debugging-workflows-unreal-engine) separates symbol generation, extra shader metadata and writing/exporting symbols. It supports build-machine export and per-platform choices. **Transfer:** generate once through the existing compiler and export exact matched artifacts separately from ordinary packages. **Limit:** unchanged executable shader data is possible for some exported-debug workflows, not a universal DXIL/SPIR-V guarantee. Sparkle compares actual hashes; debug-enabled shaders form a different candidate when bytes differ. No UE-style DDC, shader symbol server or family of new CVars is required.

Epic's [crash-reporting architecture](https://dev.epicgames.com/documentation/en-us/unreal-engine/crash-reporting-in-unreal-engine) separates report production, optional report client and server. It also prepares custom context before the fault. Transfer the responsibility split and prebuilt non-sensitive candidate context. Do not import upload defaults, unattended reporting, server infrastructure, player state collection or a support-service promise. Sparkle's writer is local; export remains an explicit user action.

The official [Unreal Fest 2023 session](https://www.youtube.com/watch?v=CyrGLMmVUAI) is an additional architecture reference for engine breadcrumbs and automated crash classification. Only its published session description was available in this study; no transcript or full-video implementation review is claimed. Automatic crash grouping is outside this delivery.

## Shader Correlation Comparison

| Route | Required join | Design consequence |
| --- | --- | --- |
| Aftermath DXIL | Native shader/debug identity → exact DXIL → full compiler debug data/source; `.nvdbg` supplies driver mapping where needed | Preserve [full DXC output](https://github.com/microsoft/DirectXShaderCompiler/blob/main/docs/SourceLevelDebuggingHLSL.rst); reject slim `-Zs` as the source-mapping route. |
| Aftermath Slang SPIR-V | Exact SPIR-V and native identifiers → qualified embedded NonSemantic debug/source data → NVIDIA mapping | The [Slang compilation guide](https://docs.shader-slang.org/en/stable/external/slang/docs/user-guide/08-compiling.html) specifies direct SPIR-V with `-g2`. Validate the pinned compiler and inspector together, including compute/ray tracing; do not substitute another compiler. |
| DXC SPIR-V export | Final normalized SPIR-V → embedded debug/source data | DXC documents [`-fspv-debug=vulkan-with-source`](https://github.com/microsoft/DirectXShaderCompiler/blob/main/docs/SPIR-V.rst). Keep this existing target route; emission and inspector behavior remain unqualified. |
| RGD | In-flight native code object/API shader identity → supported debug data and optional high-level source | Rich AMD format support is not a portable promise for Aftermath. Keep vendor-specific lookup identifiers distinct from archive/code SHA-256. |
| Epic export pattern | Existing compile job generates debug data; a separate operation exports it | Reuse the existing cook/export authority. Retention/export does not require a new runtime shader registry or an always-on viewer. |
| CPU symbols | Exact executable/module PE identity → corresponding CPU PDB | Host call stacks help locate detection/recording code; they do not resolve GPU program counters. |

The inspected Sparkle Slang backend separates diagnostic argument strings from actual compiler API options. Logging `-g2` cannot establish that it was applied. Source review must trace the actual option and exported blob; only emitted-artifact inspection and the native inspector probe can qualify source mapping. The plan defers those executable checks to its final validation stage rather than making them a circular prerequisite to building the collector.

Keep source-path remapping, transitive includes, entry points, specialization/permutation and optimization in the bundle identity. An inspector displaying the correct source file is insufficient if it belongs to another shader binary. Source mapping in optimized code can be many-to-one; GCD-4 uses an identifiable fault expression without requiring a fictitious unique line for every possible production crash.

## Technical Presentations And Their Limits

These are engineering presentations, not peer-reviewed experimental papers. Their case studies explain design tradeoffs; they do not measure Sparkle.

| Material inspected | Useful finding | Limit / permitted transfer |
| --- | --- | --- |
| Alex Dunn, NVIDIA, [GDC 2017: A New Way of Debugging Crashes on the GPU](https://developer.download.nvidia.com/assets/gameworks/downloads/regular/GDC17/NVIDIAAFTERMATHNEWWAYOFDEBUGGINGCRASHESONTHEGPU_GDC17_FINAL.pdf), slides 6–12 | A per-queue marker prototype located work, but copy-based insertion and forced ordering disturbed timing. | Keep semantic checkpoints and preserve queue behavior. Old initialization APIs, executable naming and distribution advice are obsolete, not implementation instructions. |
| Sawicki, Mulay and Bouterse, AMD/Nixxes, [GDC 2024: Post-mortem GPU Crash Analysis](https://gpuopen.com/download/GDC2024_Radeon_GPU_Detective.pdf), slides 14–18, 29–39 | GPU execution is asynchronous/pipelined; breadcrumb writes can precede or trail a fault. Resource history and concise marker reports improve investigation. | Retain multiple clues and raw data. The presentation's quick page-fault classification is a heuristic; absent data is not a definitive crash diagnosis. Use current manuals for support requirements. |
| Danylo Piliaiev, [XDC 2023: Debugging GPU Faults](https://indico.freedesktop.org/event/4/contributions/177/attachments/125/185/xdc-2023-danylo-piliaiev-debugging-gpu-faults.pdf), slides 4–15 and 28–35 | Driver breadcrumbs and replay can help unrecoverable hangs, but synchronization, multi-queue replay and instrumentation alter the experiment. | Preserve missing/partial evidence and observer-mode identity. Do not add network acknowledgements, busy-wait breadcrumbs, Linux driver code or replay infrastructure to Sparkle. |

The NVIDIA GDC 2018 *Advances in GPU Crash Debugging* PDF was located but its signed download returned HTTP 403. It is not counted as read; the accessible 2017 presentation, current SDK and pinned source support the decisions above. The Epic Aftermath workflow tutorial returned no readable body, so no implementation facts are attributed to it.

## Mechanism Comparison

This table is synthesis from the linked sources, not a vendor benchmark or a promise of portable support.

| Mechanism | Strength | Cost / failure boundary | Sparkle disposition |
| --- | --- | --- | --- |
| Aftermath dump plus exact shader artifacts | Native exception/warp and shader-location investigation on supported NVIDIA routes | Early setup, SDK/driver limits, debug-data overhead; source location may be unavailable | Primary opt-in collector, D3D12 and Vulkan |
| D3D12 DRED | Independent native operation/page-fault facts; existing Sparkle implementation | Finite history, optional names/context, pipelined progress, no universal shader mapping | Reuse existing owner; preserve missing/truncated state |
| NV Vulkan checkpoints | Queue-stage progress using existing semantic scope hooks | Opaque-token lifetime; last marker per supported stage, not an entire frame trace | Required Vulkan breadcrumb lowering |
| Epic/FidelityFX custom GPU-written breadcrumbs | Explicit begin/end progress and readable hierarchy | Surviving memory, native writes, queue ordering and retirement; duplicates selected mechanisms | Architectural reference; no extra collector/buffer now |
| RGD/RDP plus AGS | Rich AMD analysis, marker trees, resources and code objects | AMD hardware/driver/mode qualification; different dump and symbol support | Research only; no automatic AMD dependency |
| `VK_EXT_device_fault` / `VK_KHR_device_fault` | Standardized post-loss facts and optional vendor data | Requires advertised feature/extension and appropriate decoder; no `.nv-gpudmp` or source mapping guarantee | Future complementary route, not an implicit Aftermath substitute |
| CPU debug layer / GPU validation | API misuse and instrumented GPU-access checks | Different cost and timing; may change shader execution and interposer compatibility | Separate qualified sessions, not default combined instrumentation |
| PIX / external frame capture | Inspect successful work, bindings and surrounding state | Active PIX conflicts with Aftermath; failed-frame/replay restrictions | Existing capture owner, separate session |

Supporting contracts: [DRED and its caveats](https://learn.microsoft.com/en-us/windows/win32/direct3d12/use-dred), [DRED context strings](https://devblogs.microsoft.com/directx/dred-v1-2-supports-pix-marker-and-event-strings-in-auto-breadcrumbs/), [NV checkpoint query](https://docs.vulkan.org/refpages/latest/refpages/source/vkGetQueueCheckpointDataNV.html), [checkpoint stage data](https://docs.vulkan.org/refpages/latest/refpages/source/VkCheckpointDataNV.html), [EXT device fault](https://docs.vulkan.org/refpages/latest/refpages/source/VK_EXT_device_fault.html) and [KHR device fault](https://docs.vulkan.org/refpages/latest/refpages/source/VK_KHR_device_fault.html). Querying NV queue checkpoints requires an already-lost device; it cannot be used as a per-frame reclamation poll. EXT fault queries are post-loss; KHR adds asynchronous report queries while keeping extended post-mortem debug data loss-bound. Those are different APIs, not interchangeable aliases or admitted Sparkle dependencies.

## Decision Crosswalk

Candidate transfer decisions below are enacted only by the [design](GpuCrashDebuggingDesign.md), not by importing reference code. Existing `AC/FM/CHK-GCD` identities remain the acceptance authority.

| Decision | Evidence / tradeoff | Design consequence | Stage / check |
| --- | --- | --- | --- |
| `D-GCD-01` One startup and collection owner | SDK order; NVRHI device boundary | One pre-device RHI session, existing Application/Renderer composition; no pass-level vendor branches | GCD-0/2; CHK-01/07 |
| `D-GCD-02` Minimal failure callback | NVIDIA tracker shows decoding/file work in the callback; Sparkle Tasks owns execution | Contain callback failures; bound bytes/count; prepare one scoped Tasks dispatch before callbacks, then coalesce BlockingIo writes; no new thread; external analysis | GCD-2; CHK-05/06 |
| `D-GCD-03` Stable semantic tokens | NVIDIA histories and Epic/FFX lifetime concerns | Immutable session dictionary, collision-safe identity, no frame-age eviction; no per-frame token growth | GCD-2; CHK-04/05 |
| `D-GCD-04` Begin/end observation truth | Epic/FFX markers, Vulkan stage queries and local verbosity gates | Admit baseline scopes independently of timing/capture; instrument both boundaries; CPU End is not GPU completion; no fabricated total order or unique Vulkan frame | GCD-2/4; CHK-04/07 |
| `D-GCD-05` Full exact symbol bundle | NVIDIA lookup; AMD format differences; Epic generation/export split | Actual compiler API options, exact exported/loaded code, native debug IDs separate from SHA-256 and CPU identity; native mapping qualified only in GCD-4 | GCD-1/4; CHK-03 |
| `D-GCD-06` Preserve named resource clues | RGD resource history; Aftermath resource metadata; existing Sparkle names | Reuse backend naming/driver tracking; retain VA/type/lifetime facts without another allocation journal | GCD-2/4; CHK-02/04 |
| `D-GCD-07` Collect debug data without a hidden SDK cache | Installed SDK's deferred-callback memory tradeoff | Immediate `.nvdbg` callbacks, bounded queued writes; measure SDK/driver overhead separately | GCD-0/2/4; CHK-05/06/07 |
| `D-GCD-08` Bounded terminal failure | NVIDIA asynchronous finalization; Epic report separation | Collect before immediate fatal abort; cooperative budget, quiescence and settled writes; blocked native calls need external test containment, no recovery/UAF | GCD-2/4; CHK-05/06 |
| `D-GCD-09` Explicit diagnostic policy | Aftermath limitations, error-reporting contract and observer effects | No active PIX/capturer combination; shader-error reporting enabled, automatic call stacks off; fault-policy changes recorded | GCD-0/3/4; CHK-01/07 |
| `D-GCD-10` Keep ordinary development thin | NVRHI optional dependency; existing Sparkle feature enclosure/artifact contract | SDK absent from ordinary builds; canonical Launcher stays outside diagnostic variants; no new registry, shader database, runtime or settings family | GCD-0/2/4; CHK-07 |
| `D-GCD-11` Local artifacts with explicit consent | Epic producer/client/server split; raw vendor data | Prebuilt safe context, private raw files, sanitized default export; no uploader/support service | GCD-2/3; CHK-06 |
| `D-GCD-12` Test limitations as well as successes | All sources distinguish clues from causes | Wrong symbols, lagging queues, missing names/page faults and external-reset cases remain inconclusive when appropriate | GCD-4; CHK-02/03/04/05 |

`CHK-01` through `CHK-07` above abbreviate the design's `CHK-GCD-01` through `CHK-GCD-07` only; they are not new check identities.

## Fault And Evidence Matrix

| Fault / controlled input | Useful observation | Forbidden conclusion / required negative |
| --- | --- | --- |
| Known compute or ray-tracing fault | Dump shader identity, PC/source mapping and relevant queue scope | A CPU stack or disassembly alone cannot pass source attribution. |
| Use-after-free or stale descriptor | Native address facts, live/recently-freed resource candidates, shader data | Matching an address is not unique proof of ownership or cause. Exercise address reuse/alias ambiguity. |
| Hang with no reported page fault | Last native progress and available active warps | No page-fault record does not prove an infinite shader loop. |
| API misuse before GPU submission | HRESULT/validation message and no submitted faulty work | Do not manufacture a GPU dump requirement for a CPU-side API failure. |
| Reset caused outside this process | Device-loss reason and missing/inconclusive local fault information | Do not blame the last local pass merely because it was active. |
| Parallel queue with delayed completion | Per-queue/stage markers and recording identity | Do not sort CPU timestamps into a GPU causal order. |
| Command reset/destruction or repeated semantic labels | Stable token resolution after late callbacks | No frame-count retirement, pointer reuse or hash-only marker match. |
| Long normal run | Stable semantic dictionary occupancy after warm-up | A marker table that grows once per frame fails the thin-path requirement. |
| Wrong compiler/permutation/PDB or missing `.nvdbg` | Explicit identity mismatch or unavailable source mapping | Never search a nearby build and present it as matched. |
| Disk full, quota exhausted or late callback | Partial artifacts, exact missing section, bounded terminal result | SDK Finished alone does not prove durable file publication. |
| CPU kill or system-wide hard lock | Initial session metadata and whatever files reached disk | No guaranteed GPU dump or recovery promise. |
| Instrumented failure disappears | Exact mode/candidate and changed observer costs | Record inconclusive reproduction; do not declare a renderer fix. |

## Cost And Qualification Questions

The initial limits in the design are **proposed Sparkle budgets**, not values validated by these sources. Keep them distinct from SDK/driver-owned memory, which the application cannot cap with its own queue limit. GCD-0 verifies API/input compatibility; GCD-4 measures the actual workloads. A measured overflow or incompatible lifetime reopens the owning design decision before increasing complexity.

Measure these separately: cold and warm shader preparation; CPU recording time; GPU frame time; application/driver memory; `.nvdbg`/symbol/incident sizes; writer throughput; fault-to-callback latency; callback-to-durable-publication latency; normal and faulted shutdown. Use matched feature settings and optimization. No percentage taken from a vendor sample, paper or AAA-engine measurement becomes a Sparkle budget.

Required implementation checks must include immediate concurrent shader-debug callbacks, SDK Finished before writer drain, nested scope exits, delayed queues, repeated command-list reuse, stable long-run dictionary occupancy, resource-name truncation and absent DRED context. These are the specific counterexamples that can invalidate the selected architecture.

## What This Study Does Not Establish

No SDK collector was enabled, driver setting changed, GPU fault injected or paused hang reproduced. No AMD hardware route, Epic private source or inaccessible video/PDF content was qualified. Current SDK/runtime redistribution remains a separate input-admission task. The production stages and all native dump-to-source criteria remain open.
