# Third-party migration

[Goals](Goals.md#the-three-cases) | [Engineering](Engineering.md#module-boundaries) | [Roadmap](Roadmap.md#supporting-system-simplification) | [Dependency recipes](../ThirdParty/README.md)

Delegate generic mechanisms so more effort goes into correct pixels, measured frames and owned neural graphics. Keep Sparkle's RHI, frame graph, resource lifetime, scene/material meaning and independent Reference renderer.

## Next actions

These support the existing roadmap; they add no first-release prerequisites. Continue [current correctness repairs](Roadmap.md#current-priority) alongside bounded simplification.

| Action | Ownership to transfer or simplify | Completion condition |
| --- | --- | --- |
| **Complete Qt scene item views** | The isolated fit trial passed. Finish production conversion so existing Qt owns Launcher placement/scrolling/navigation. [Active plan](Implementation/LauncherItemViews.md). | Retire custom reflow/per-card widget assembly; preserve information, sizing, DPI, focus/actions and readiness. Prove the complete connected UI is smaller and clearer. |
| **Trial compiler preprocessing** | Let DXC/Slang evaluate includes and conditions; retire Sparkle's recursive [include expander](../Tools/Shaders/ShaderCompiler/Private/Compiler/ShaderSourcePreprocessor.cpp). Independent of the Qt trial. | Preserve virtual roots, generated parameters, dependency/debug source identity, typed ABI, accepted publication and reload on both shader targets. |
| **Integrate selected rendering SDKs** | Follow the [rendering choices below](#rendering-sdks-worth-considering) in [Roadmap order](Roadmap.md#after-release). | Deliver a measured rendering benefit; retire any production algorithm being replaced. Record complete integration costs. |

New EnTT, FlatBuffers, importer-replacement and hashing dependencies remain conditional ideas without a scheduled migration. Reopen one only for a named consumer and a complete maintenance win. First check the libraries and helpers already available below.

## Reuse already available code

The [November 22 source/API audit](../build/validation/third-party-reuse-20261122/REPORT.md) covers all 26 dependency recipes, installed Qt/compiler APIs and existing Sparkle helpers. It identifies the following remaining opportunities; these are research findings, with no verified net saving or additional release prerequisite.

| Priority and consumer | Existing mechanism to use | Local responsibility to remove or retain |
| --- | --- | --- |
| **First: Editor text input** | ImGui's shipped [`std::string` overloads](https://github.com/ocornut/imgui/blob/v1.92.5/misc/cpp/imgui_stdlib.h), compiled once through the existing recipe. | Remove temporary character buffers and copy-back code in Outliner, Inspector, Settings and `EditDetailsText`. Retain change/reset behavior and any intentional input limits. |
| **First: SPIR-V module lifetime** | The pinned SPIRV-Reflect header already supplies `spv_reflect::ShaderModule`. | Replace seven explicit destroy calls across reflection extraction and binding normalization with upstream stack ownership. Retain result checks, binding meaning and cooked ABI; do not introduce another wrapper. |
| **Small, direct reuse** | Core's raw FNV/string helpers, Qt's `QHash::removeIf`, and DirectXMath's `XMVectorHermite`. | Remove duplicate hashing/lowercasing, the dependency-run erase loop and handwritten Hermite basis arithmetic. Preserve reserved-zero identity rejection, operation completion and time-scaled animation tangents. The separate trim near-match needs its whitespace contract checked first. |
| **Bounded trial: named logging** | spdlog already has named lookup/registration and logger cloning. | Retire the local named-logger map and duplicated construction where a complete reduction is demonstrated. Keep bootstrap, sink/file policy, atomic creation and failure handling inside Core; static linkage makes module ownership significant. |
| **Next Qt fit trials: Workspace/Clean rows** | Existing Qt item models/views, standard check roles and delegates; reuse the current clean-plan inventory owner. | Investigate replacing dependency label/button maps, string-valued action properties, repeated row trees and the second directory scan. Retain readiness, run identity, exact clean targets and confirmation. Count model/delegate/accessibility costs; Qt does not supply Sparkle's select-all or cleanup policy. |

Complete the existing scene-view work before broadening Qt conversion. Compiler preprocessing remains the larger independent mechanism trial above. Plain-text log widgets and ImGui list clipping are conditional responsiveness ideas, without an established code-removal benefit.

The audit also checked already delegated import/texture/animation/allocation paths. Do not credit those deliveries again or replace domain validation with a similarly named utility. The report records per-library coverage, precise source fragments and rejected near-matches; a fetched transitive module or SDK header is not automatically an enabled dependency.

## Native removal estimates

| Pending bounded trial | Reviewed scope before replacement costs | Meaning |
| --- | ---: | --- |
| Compiler preprocessor pair | 253 physical / 221 code lines | Whole reviewed pair; exact source/debug and generated-input work still needs an owner. |
| Qt custom reflow class | At most 160 physical / 140 code lines | Historical Q fragment only. Model/delegate/caller costs and any additional card retirement are counted separately by the active plan. |

The historical pending inventory spans **992 physical / 873 code lines in 18 files across ten scopes**, excluding delivered migrations. It is a review ceiling, with **no supported future net forecast**. [Frozen source ranges](../build/validation/third-party-removal-triple-check-20261121/per-file-ledger.csv) retain the original identities; the preprocessor pair was subsequently recounted. Judge broader code movement through [module ownership](Engineering.md#module-boundaries).

## Delivered integrations versus savings

These are frozen, differently scoped delivery results. **Do not add the rows into a repository saving.** Physical lines include comments/blanks; code lines exclude them. CLI11, JSON/parser and texture retirements already included in the original baseline earn no new credit.

| Delivered owner | Recorded cost result | Evidence |
| --- | --- | --- |
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

### Task execution

The authorized maintained enkiTS fork owns execution; Tasks is retired. Sparkle retains typed kernels, bounded domain admission, stop/results, publication and GPU lifetime under the [execution contract](Engineering.md#execution-ownership). Unmodified enkiTS, oneTBB and Taskflow mappings remain rejected; add no second scheduler or compatibility facade. [Recipe](../ThirdParty/enkits/Dependency.cmake) owns the approved base and patch identity.

### Window and input

One shared SDL3 runtime and stock ImGui SDL3 backend own window/event/pointer mechanics. Core/InputSystem retain SDL-free values, routing and cancellation; RHI retains native presentation and GPU retirement before window destruction. Queries/callbacks stay on the creating thread. Launcher remains SDL-free. Do not restore a Win32 decoder, camera warp loop or SDL rendering layer. [Recipe](../ThirdParty/sdl3/Dependency.cmake).

### Child processes

Core's synchronous [request/result](../Engine/Core/Public/Process/ChildProcess.h) uses private Boost.Process/Asio mechanics and Boost.Scope guards. Keep command-line compatibility, creation-time job membership, process-tree termination/readiness policy and physical settlement before borrowed state dies. Stop callbacks only signal; missing/error/timeout completion remains a reported failure, with no hard finite OS cleanup guarantee. Keep one caller-driven context and no fallback backend. [Recipe](../ThirdParty/boost-process/Dependency.cmake).

### JSON

nlohmann/json owns JSON grammar/values/serialization; Core owns bounded admission and domains own schema/types/publication. Keep upstream types private, parse snapshots once and add no compatibility parser or writable mirror. The [recipe](../ThirdParty/nlohmann_json/Dependency.cmake) owns the explicitly approved unreleased identity; this is no blanket approval for development dependencies. Authored levels, catalogue editing, texture requests and INI/vendor formats retain their current owners and editing contracts. Cache-cold qualification remains **WAIVED / UNVERIFIED**.

### GPU allocation

Follow AMD's [D3D12MA ownership](https://gpuopen-librariesandsdks.github.io/D3D12MemoryAllocator/html/quick_start.html) and [VMA integration](https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html): one allocator per native device, direct allocation handles and resource destruction before memory release. D3D12MA uses COM ownership and owns descriptor free-range placement; VMA owns allocation, host-copy/cache and budget mechanisms. RHI retains identity, native descriptors, synchronization and GPU retirement. Keep alias ownership explicit and account physical allocations once. Preserve the [VMA mapping-lifetime patch](../ThirdParty/vma/Patches/VmaCopyMappingLifetime.patch) in a private build copy. This integration covers the existing resource, aliasing, upload/readback, descriptor and budget paths.
