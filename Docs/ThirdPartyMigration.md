# Third-party migration

[Goals](Goals.md#the-three-cases) | [Engineering](Engineering.md#module-boundaries) | [Roadmap](Roadmap.md#supporting-system-simplification) | [Dependency recipes](../ThirdParty/README.md)

Delegate generic mechanisms so more effort goes into correct pixels, measured frames and owned neural graphics. Keep Sparkle's RHI, frame graph, resource lifetime, scene/material meaning and independent Reference renderer.

## Next actions

These support the existing roadmap; they add no first-release prerequisites. Continue [current correctness repairs](Roadmap.md#current-priority) alongside bounded simplification.

| Action | Ownership to transfer or simplify | Completion condition |
| --- | --- | --- |
| **Close Qt scene-view cleanup** | Production uses Qt item views; the old grid, per-card widgets and widget-state maps are retired. [Remaining cleanup](../build/validation/launcher-item-views/candidate-20261122/REPORT.md#required-cleanup-open). | Resolve policy-rejected local probe/fixture cleanup and verify absence. Broader display/product qualification stays with Roadmap. |
| **Trial compiler preprocessing** | Let DXC/Slang evaluate includes and conditions; retire Sparkle's recursive [include expander](../Tools/Shaders/ShaderCompiler/Private/Compiler/ShaderSourcePreprocessor.cpp). Independent of the Qt trial. | Preserve virtual roots, generated parameters, dependency/debug source identity, typed ABI, accepted publication and reload on both shader targets. |
| **Integrate selected rendering SDKs** | Follow the [rendering choices below](#rendering-sdks-worth-considering) in [Roadmap order](Roadmap.md#after-release). | Deliver a measured rendering benefit; retire any production algorithm being replaced. Record complete integration costs. |

New EnTT, FlatBuffers, importer-replacement and hashing dependencies remain conditional ideas without a scheduled migration. Reopen one only for a named consumer and a complete maintenance win. First check the libraries and helpers already available below.

## Reuse already available code

The [source/API audit](../build/validation/third-party-reuse-20261122/REPORT.md) led to the [delivered reuse changes](../build/validation/third-party-reuse-20261122/delivery/REPORT.md). ImGui owns string-input resizing; SPIRV-Reflect owns module destruction; existing Core, Qt and DirectXMath functions replace duplicated hashing, string/path-list handling, run removal and Hermite arithmetic. Raw authored-identity hashes still reject zero, and animation tangents retain their time scaling.

The subsequent [integration owner review](../build/validation/integration-owner-refinement-20261122/REPORT.md) covers all 26 dependency recipes and selected production consumers. It removes capture-state mirrors, animation bridge APIs and Assimp's duplicated tree search, delegates native process utilities to Boost, and closes TinyEXR output ownership. Repeat this review when dependencies or their consumers change: trace producers, callers and lifetime, compare the pinned API's complete contract, retire the displaced mechanism and measure the connected result. A shorter API with weaker behavior is not a saving.

Launcher Workspace dependencies and Clean scopes use stock Qt item views. Each dependency view has one native action button; current identity and readiness are resolved again before dispatch. Operations outlive their widgets. Clean keeps its explicit `clean-all` meaning, available-row selection and confirmation; its directory inventory comes from the maintenance owner and can include preserved files. Keep filesystem inspection outside item data access and painting.

Retain Visual Studio's wider whitespace trimming and the separate zero/general/overflow-checked alignment contracts; similarly named Core functions are not exact replacements. Plain-text log widgets and ImGui list clipping remain conditional responsiveness ideas without an established code-removal benefit. Compiler preprocessing retains its independent trial above. New reuse work must identify an actual consumer and retire the displaced mechanism.

## Native removal estimates

| Pending bounded trial | Reviewed scope before replacement costs | Meaning |
| --- | ---: | --- |
| Compiler preprocessor pair | 253 physical / 221 code lines | Whole reviewed pair; exact source/debug and generated-input work still needs an owner. |

The historical pending inventory before Qt scene conversion spans **992 physical / 873 code lines in 18 files across ten scopes**. Its retired Q fragment retains its original **160 physical / 140 code** ceiling in the Qt ledger and earns no future credit. The inventory is a review ceiling, with **no supported future net forecast**. [Frozen source ranges](../build/validation/third-party-removal-triple-check-20261121/per-file-ledger.csv) retain the original identities; the preprocessor pair was subsequently recounted. Judge broader code movement through [module ownership](Engineering.md#module-boundaries).

## Delivered integrations versus savings

These are frozen, differently scoped delivery results. **Do not add the rows into a repository saving.** Physical lines include comments/blanks; code lines exclude them. CLI11, JSON/parser and texture retirements already included in the original baseline earn no new credit.

| Delivered owner | Recorded cost result | Evidence |
| --- | --- | --- |
| Integration owner refinement | 177 fewer native physical lines across SDL3, Boost, ozz, Assimp and TinyEXR; CMake physical count unchanged. Actual Cesium construction uses 57 fewer SDK allocations with identical sampled poses. Required local probe cleanup remains open after policy rejection. | [Review and limits](../build/validation/integration-owner-refinement-20261122/REPORT.md), [complete costs](../build/validation/integration-owner-refinement-20261122/costs.json) |
| Existing API reuse | 246 fewer native physical / 200 fewer code lines across 23 complete changed owner/caller files; two CMake lines added. Includes named logging and Workspace/Clean presentation; upstream ImGui adapter compilation is separate. Required local probe cleanup remains open after policy rejection. | [Delivery and limits](../build/validation/third-party-reuse-20261122/delivery/REPORT.md), [complete costs](../build/validation/third-party-reuse-20261122/delivery/costs.json) |
| Qt Launcher scene views | 34 fewer native physical lines and one fewer code line across the complete connected UI; CMake physical count unchanged. This is a modest reduction. Required local cleanup remains open. | [Delivery and limits](../build/validation/launcher-item-views/candidate-20261122/REPORT.md), [complete costs](../build/validation/launcher-item-views/candidate-20261122/costs.json) |
| enkiTS maintained fork | 2,118 fewer C++ physical lines; 2,005 fewer complete-infrastructure lines after maintained native additions. Added-code/API/state/cleanup targets missed. | [Cumulative review](../build/validation/task-runtime/srp-20261122/REPORT.md) |
| ShaderMake | 374 fewer native physical / 194 fewer code lines. Maintained ShaderMake/Slang patches, build and payload costs are separate. | [Cost record](../build/validation/shadermake/launcher-workflow-20261122/costs.json) |
| VMA / D3D12MA reuse | Initial reuse removed 75 native physical lines after maintained patch additions. The subsequent cleanup removes another 327 native lines from its frozen scope. CMake costs separate; runtime/CPU-storage costs unmeasured. | [Initial costs](../build/validation/gpu-allocators/delivery-20261122/costs.json), [final integration and limits](../build/validation/gpu-allocators/finalization-20261122/REPORT.md) |
| SDL3 | 397 more native physical lines on the conservative W ledger, including moved callers. | [Delivery ledger](../build/validation/window-input/delivery-20261122/native-cost-ledger.json) |
| Boost.Process | 76 more native physical lines after termination/caller repairs. | [Final cost record](../build/validation/boost-process/continuation-20261122-r2/termination-01/costs.json) |
| ozz-animation | 138 more native physical lines in the preview bridge, plus 18 Launcher metadata lines: 156 more in the recorded scope. | [Bridge costs](../build/validation/animation/fit-20261122/production-cost.json) |

A separate [SDL3/Boost/ozz follow-up](../build/validation/integration-savings-20261122/REPORT.md) removed 81 native physical lines, with two added CMake lines; the delivery ledgers remain frozen. Delegation and code reduction are separate outcomes. Broader GUI/GPU, performance, relocated/offline package and clean-machine limits remain with [Roadmap](Roadmap.md#supporting-system-simplification); no total build-time, runtime or package saving is established.

## Rendering SDKs worth considering

| Selected work | Rendering result and boundary |
| --- | --- |
| **RTXDI DI trial** | Replace one production sampling owner and compare equal-quality cost. Keep Reference and bounded owned research independent. GI/PT need separate decisions; no native host saving is identified. |
| **Existing DLSS, then NRD/RELAX and AMD FSR upscaling** | Qualify DLSS through existing Streamline on D3D12; treat Vulkan separately and add no parallel NGX path. Integrate NRD through Sparkle RHI without NRI. Preserve temporal signals, history/reset, resources and failure behavior; consume/copy borrowed NRD dispatch data before requesting its next batch. Qualify actual FSR API/device paths and official signed payloads. |
| **Later SHaRC, NRC and NTC** | Follow the [post-release roadmap](Roadmap.md#after-release). Finish SHaRC package/reproduction before NRC production. NTC requires classical-study results and does not wait for NRC. These add capability; they have no equivalent local cache/codec to retire. |
| **XeSS** | Deferred until a comparison question remains beyond DLSS/FSR. Frame generation stays excluded. |

Use raw Reference controls and quality-equivalent whole-frame/memory comparisons. Attribute vendor algorithms separately from the [owned model case](Goals.md#the-three-cases). [References](References.md#integration-precedents) holds primary integration studies; exact SDK/tool/device support must be checked for implementation.

## Requirements for adoption

1. Name the consumer, upstream responsibility and local code to retire. Prove decisive fit early, then convert actual callers.
2. Freeze behavior and cost limits before the candidate. Charge adapters, callers, relocated code, shaders, build/scripts and maintained patches; report build/runtime/memory/package costs separately. Moving code earns no removal credit.
3. Check the exact release, maintenance/advisories, tests, Windows/toolchain support, transitive inputs, acquisition and licenses. Review shipped binaries, models/data and media rights separately.
4. Preserve supported workflows, quality, cancellation, physical settlement, publication/recovery and offline use. Check affected D3D12/Vulkan paths separately; remove superseded implementations, targets and payloads.

### Code delivery and qualification

A requested stage includes preparing prerequisites, repairing relevant defects and reaching its verified exit. Full integration requests continue through caller conversion, retirement, focused correctness/failure/lifetime checks and required closeout. Preserve concurrent work and reuse evidence only when source/tool/product inputs match.

Review a missed numerical target once, fix defects or unnecessary machinery and report the result honestly. Unsafe lifetime/capacity/cancellation, incorrect behavior or a second mechanism blocks code completion. A promised simplification also needs its defined economic exit; record NO-GO when it fails. Unrun broader checks remain **UNVERIFIED**, inconclusive results **INCONCLUSIVE**, and only explicit waivers **WAIVED / UNVERIFIED**.

Remove completed actions/stages from the active plan at each verified exit; keep receipts in ignored evidence. Remove the plan and repair links when its last requirement closes. Temporary validation additions stay out of submitted source. Required cleanup remains open after a rejection; never bypass it. Code completion does not establish release readiness. [Engineering](Engineering.md#verification) owns validation and [documentation rules](Engineering.md#writing-documentation).

## Ownership and dependency workflow

| Responsibility | Placement and rule |
| --- | --- |
| Dependency identity and build | `ThirdParty/<name>/Dependency.cmake` owns pins, required inputs, targets/options and package facts. `Bindings/` holds identifiable Sparkle compile carriers; `Patches/` records authorized base revision plus patch hash. Keep upstream compilation separate from native target policy. |
| Domain adaptation | The consuming module's `Private/<capability>` owns semantic/lifetime decisions. Keep linkage/types private where possible; shared globals, allocators and contexts follow the actual library boundary. |
| Acquisition and build state | [CMake/Dependencies](../CMake/Dependencies) owns admission and cache protection. Share admitted source in ignored `build/_deps`; each configured tree owns objects and private patched/generated copies. Preserve the shared source cache. |
| Metadata and delivery | Generate Launcher facts from recipes. [Runtime deployment](../CMake/Artifacts/RuntimeDeployment.cmake) and [Packaging](../CMake/Packaging) consume declared payload/contributor facts, selected configuration, notices and license/source obligations. Retire declarations with their last consumer. |

Sync/configure and cache-affecting Clean use the source-cache writer lock; builds/staging use readers. Build admission is read-only and cannot fetch/repair inputs. Prepared cook/run uses existing products; tool preparation is a separate incremental build. An unchanged compatible build does no dependency compile/archive work; changed source/patch/toolchain/ABI or explicit build cleaning legitimately rebuilds. Verify runtime repair/pruning separately from linking. No package manager or shared writable binary cache is selected.

Review bundled code, installed SDKs, plugins and data independently; matching declared source-package lists cannot discover undeclared contributors. The [qualification receipt](../build/validation/third-party-integration-20261122/stage4/REPORT.md) retains identified implementation results and the accepted cleanup disposition. Final package/legal/clean-machine acceptance stays with [Roadmap](Roadmap.md#before-the-first-release).

## Retained decisions

### Launcher scenes

Existing Qt Widgets owns scene placement, wrapping, navigation and selection. One private list model owns presentation rows; a delegate paints cards and one shared native Source/primary button pair serves the current item. Domain owners retain catalogue meaning, readiness, request construction and operation lifetime. Keep stable-identity dispatch and refresh the existing scene page; do not restore per-row widget trees, status/button maps or a second reflow path. The complete conversion is nearly flat in code size; no general runtime, memory or package saving is established. [Source owner](../Tools/Launcher/SparkleLauncher/Private/Gui/Pages/Scenes/LauncherSceneView.cpp).

### Task execution

The authorized maintained enkiTS fork owns execution; Tasks is retired. Sparkle retains typed kernels, bounded domain admission, stop/results, publication and GPU lifetime under the [execution contract](Engineering.md#execution-ownership). Unmodified enkiTS, oneTBB and Taskflow mappings remain rejected; add no second scheduler or compatibility facade. [Recipe](../ThirdParty/enkits/Dependency.cmake) owns the approved base and patch identity.

### Window and input

One shared SDL3 runtime and stock ImGui SDL3 backend own window/event/pointer mechanics. InputSystem's optional capture layer owns routed capture; SDL owns cursor hiding, without Core input-state or camera-controller mirrors. Core/InputSystem retain SDL-free values, routing and cancellation; RHI retains native presentation and GPU retirement before window destruction. Queries/callbacks stay on the creating thread. Launcher remains SDL-free. Do not restore a Win32 decoder, camera warp loop or SDL rendering layer. [Recipe](../ThirdParty/sdl3/Dependency.cmake).

### Child processes

Core's synchronous [request/result](../Engine/Core/Public/Process/ChildProcess.h) uses private Boost.Process/Asio mechanics and Boost.Scope guards. Keep command-line compatibility, creation-time job membership, process-tree termination/readiness policy and physical settlement before borrowed state dies. Stop callbacks only signal; missing/error/timeout completion remains a reported failure, with no hard finite OS cleanup guarantee. Keep one caller-driven context and no fallback backend. [Recipe](../ThirdParty/boost-process/Dependency.cmake).

### Animation

GameFramework's skeleton and animation loaders own typed admission, allocation-budget reservation and immutable ozz construction together. Payload appenders consume runtime resources directly; decoded records stay local to their loader. Preserve bind defaults, original skin order, four morph weights and external zero-duration behavior. Ozz owns sampling, hierarchy evaluation and the clip name; prepared pose work owns exclusive sampling contexts. Keep unsafe upstream archive loading outside this boundary. [Recipe](../ThirdParty/ozz_animation/Dependency.cmake).

### Logging

Core owns bootstrap, sink/file policy and serialized named creation. spdlog's registry owns named loggers; clones share the configured base logger's sinks, level and error-flush policy. Acquire categories through Core even when other modules link their own static spdlog copy. Keep no parallel named-logger map.

### JSON

nlohmann/json owns JSON grammar/values/serialization; Core owns bounded admission and domains own schema/types/publication. Keep upstream types private, parse snapshots once and add no compatibility parser or writable mirror. The [recipe](../ThirdParty/nlohmann_json/Dependency.cmake) owns the explicitly approved unreleased identity; this is no blanket approval for development dependencies. Authored levels, catalogue editing, texture requests and INI/vendor formats retain their current owners and editing contracts. Cache-cold qualification remains **WAIVED / UNVERIFIED**.

### GPU allocation

Follow AMD's [D3D12MA ownership](https://gpuopen-librariesandsdks.github.io/D3D12MemoryAllocator/html/quick_start.html) and [VMA integration](https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html): one allocator per native device, direct allocation handles and resource destruction before memory release. D3D12MA uses COM ownership and owns descriptor free-range placement; VMA owns allocation, host-copy/cache and budget mechanisms. RHI retains identity, native descriptors, synchronization and GPU retirement. Keep alias ownership explicit and account physical allocations once. Preserve the [VMA mapping-lifetime patch](../ThirdParty/vma/Patches/VmaCopyMappingLifetime.patch) in a private build copy. This integration covers the existing resource, aliasing, upload/readback, descriptor and budget paths.
