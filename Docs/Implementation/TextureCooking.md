# Texture cooking migration

[Migration strategy](../ThirdPartyMigration.md#texture-cooking) | [Developer guide](../Engineering.md) | [Code style](../CodeStyle.md)

Temporary delivery plan. Keep it until final acceptance, then fold lasting guidance into the migration guide and remove it. Historical plans, failures and recipes remain in the evidence archives below.

## Decision and progress

Microsoft DirectXTex is the selected CPU processor. The user waived exact Kaiser equivalence, not color, alpha, normal, HDR, resource, publication or quality requirements. There is one processor and no public filter selector or decoder retry fallback. AMD Compressonator Framework and NVTT 3 remain permitted alternatives, not installed processing paths. No NVTT SDK or terms acceptance is needed for this selection.

| Stage | State |
| --- | --- |
| 1. Capture | Complete as a baseline, including its failures and gaps. Original executable revision: `6e44a2d4ec11ad66d00b2f7b92a8be73a1679360`. |
| 2. Select | DirectXTex selected after the filter revision. Final quality acceptance remains subject to stage 6; the original Kaiser/no-go results remain historical. |
| 3. Dependencies and delivery | Complete with stage 4: official pins, readiness, explicit Sync/Clean, cold/warm/offline acquisition, enabled host builds, ABI/link/import closure, relocation and tool inventory qualified. Stage 3 has no independent completion claim. |
| 4. Replace processing | Complete. Native images and complete encoded chains stay upstream-owned. Local parsing, mip/block/half-conversion algorithms, redundant publication and unused cancellation plumbing are retired. |
| 5. Fault qualification | Complete under the recorded standalone-tool policy: Core owns child-process cancellation; admitted 4K uses the aggregate pool. Fault, memory, publication and real-reader checks pass within their observed scope. |
| 6. Native and release acceptance | **Open with an HDR quality failure.** Shader sampling and array views pass; source-quality, visual, timing and final release gates below remain. |

Current net retirement is **38468 bytes / 1400 lines (35.83% / 41.84%)**, charging tool, caller, Core, runtime, build and packaging changes against the original **107355-byte / 3346-line** boundary. Both 35% floors pass. TextureCooker has 30 C++ source/header files against the original 56. Numerical retirement does not justify duplicate authority or unnecessary integration code.

## Ownership and admitted behavior

| Responsibility | Authority |
| --- | --- |
| Material/default policy, identity and paths | Existing producers and neutral `TextureCookShared` requests. The protocol has nine fields; the old filter field/parser is retired. Recook generated content. |
| Request orchestration | `Cooking/CookTextureRequestFile.cpp` loads requests, protects inputs, runs the batch and publishes. `TextureCookBatchExecutor` owns task settlement, worker COM lifetime and stage cleanup. No stateless processor class or forwarding cook-command layer remains. |
| DDS, WIC/HDR, conversion, mips and BC encoding | DirectXTex metadata/load/Convert/GenerateMipMaps/TransformImage/Compress APIs and `ScratchImage` allocations. DirectXMath supplies native vector/color operations. No local DDS parser, block loop, decoded-normal kernel, SDK patch or per-image encoding/copy loop remains. |
| Demonstrated provider gaps | Existing pinned stb for PSD/PIC/PNM/TGA, TinyEXR plus zlib for flat single-part EXR, and native `ktx_read` for KTX1/2. No provider fallback chain. The stb TGA route retains 16-bit gray-plus-alpha support rejected by the native TGA loader. |
| Admission | Source metadata checks and checked reservation estimates before decoded allocation. Core's bounded binary reader owns file acquisition; native image buffers retain processing reservations through writing. |
| Serialization and publication | The writer validates native canonical rows/slices and closes the supplied generation stage. One borrowed publication list supplies staging, cleanup and Core FileSetPublisher commit/rollback. Validate protected source/request paths before workers can remove stages. |
| Runtime consumption | Renderer holds a FileSetReadScope through immutable-byte acquisition. RHI owns format rows, complete upload validation, resource creation and native flat/array/cube views. Runtime loading acquires no processing SDK. |

SDK configuration/types and providers stay private to TextureCooker. `TextureCookShared`, public requests, Core, Renderer, RHI, Game and Editor acquire no processing runtime. Locate counterparts in Core, ToolSupport, peer cookers and the selected SDK before adding another helper, type, allocator or publisher.

Admit 2D textures/arrays with axes 1..16384, up to 2048 layers and one square six-face cube, subject to byte admission. Source extensions are DDS, EXR, HDR/HDRI, PNG, JPG/JPEG, BMP, TGA, GIF, PSD, PIC, PNM/PPM/PGM and KTX/KTX2. Reject 1D containers, volume/3D, cube arrays, animation/multiframe input and multipart/deep EXR. Worker COM apartments last for the thread, because the native WIC factory is retained. PNG/JPEG/BMP/GIF rely on built-in Windows codecs, not optional codec packs; other routes do not require WIC extensions.

KTX admits RGBA8 linear/sRGB, RGBA32F, BC1/2/3 linear/sRGB, BC4/5 UNORM/SNORM, unsigned BC6H and BC7 linear/sRGB. Reject all supercompression, Basis/UASTC/ETC/ASTC, undefined/unsupported encodings and signed BC6H. Preserved compressed KTX requires right/down orientation; native transformations handle admitted uncompressed orientation. DDS interpretation remains with the native parser.

Chains are item/face-major and mip-minor, with faces +X,-X,+Y,-Y,+Z,-Z. NoMips selects each top level; PreserveExisting retains supplied levels; Generate rebuilds from top levels. Color groups use native cubic; data/HDR/normals use native linear. Generated normal levels are normalized through the native row-transform API; authored/top-level normals remain untouched. BC5 normals reconstruct positive Z; signed BC5 normals convert to the engine's unsigned convention. Processed signed BC4/5 data retains signed float values.

Byte processing follows requested color policy; float input remains linear. RGB transfer never transforms alpha. Inspect all effective subresources before format selection. Preserve meaningful HDR alpha in RGBA32F, reject nonfinite values and BC6H positive overflow above 65504, and retain unsigned-codec negative clamping. Reject sRGB intent on linear-only formats. Preserve supplied compressed blocks only when channels, mip policy and normal interpretation permit it.

## Resources and cancellation

| Limit | Delivery policy |
| --- | --- |
| Source file | At most 64 MiB, checked before resize/read. Separate source storage reserves 128 MiB per background worker; workers are bounded to 1..4. |
| Processing | 1 GiB aggregate pool. Each request must fit available capacity. Estimates include native float chains, decode/filter storage and descriptors; reservations are separate from measured process memory. |
| Measured process caps | 1.25 GiB private / 1.5 GiB working set. Observed native 4K processing is about 630 MB private; four-worker stress about 631 MB. These are observed bounds, not allocator interception or a worst-case guarantee. |
| Cancellation | Core's existing owned Windows Job terminates the standalone process tree and drains output. Keep the unchanged 250 ms stop-work / one-second settlement limits. Twelve actual Debug/Shipping static/shared child runs settle in about 2-3 ms, with matching PID, no remaining descendants and preserved accepted output. |

There is no embeddable/cooperative CLI cancellation transport or unused per-function stop-token plumbing. Process termination can leave stages or an incomplete generation; readers fail closed, and explicit fixture cleanup/recook restores access. Distinct output directories, including nested ones, retain their own guards. Packages include each `.stex` directory's `.publication-lock`, including read-only installed content.

The original 256 MiB per-request estimate and cooperative-function deadline are retired trial conditions, not passed or overwritten. The 4K trial estimated 807624258 bytes; wide BC6H cooperative trials took about 12-14 seconds, and a Debug case exceeded 120 seconds. Retaining 4K under the aggregate pool and Core's existing cancellation owner were recorded engineering defaults; no explicit numerical waiver is attributed to the user. Original budgets and failed results remain unchanged.

## Dependency and delivery contract

Use the current CMake/Launcher cache, required-file checks, immutable pins, explicit individual Sync/Clean and full Sync Code. Do not introduce another dependency manager or processor. Keep source in the ignored owned cache and generated inputs in each variant's `_deps/<name>-build`; variants must not overwrite one another's binaries.

| Input | Identity and composition |
| --- | --- |
| DirectXTex | Official `may2026`, commit `4feb3e11a020f35b796fc769a74216a555d4f5ef`, header version 211. Unmodified upstream static CPU target. Tools, samples, DX11/DX12, GPU/OpenMP and optional external codecs disabled. |
| KTX-Software | Official `v4.3.2`, commit `91ace88675ac59a97e55d0378a6602a9ae6b98bd`, sparse static `ktx_read`. No writer, encoder, tools, tests, bindings or GPU-upload target in the tool closure. Bundled reader components still count and carry notices. |
| Retained inputs | Existing pinned stb, TinyEXR and zlibstatic. No separately acquired processor binary, plugin or runtime. Source package versions/hashes and acquisition URLs are recorded in `source-reconstruction.json` and package metadata. |

Native static libraries are `DirectXTex.lib`, `ktx_read.lib` and `zlibstatic.lib`; stb/TinyEXR compile into the tool. Existing integration inputs are ToolSupport, TextureCookShared, Tasks, Core and spdlog; Core/Tasks use import libraries in shared builds. RHI contracts supply headers without rendering backend linkage. Inspect actual linked projects/objects and DLL imports; a private CMake edge or an upstream sample build alone proves neither ABI nor isolation.

Shipping's permitted CRT bundle is `msvcp140.dll`, `msvcp140_1.dll`, `msvcp140_2.dll`, `msvcp140_atomic_wait.dll`, `msvcp140_codecvt_ids.dll`, `vcruntime140.dll`, `vcruntime140_1.dll` and `concrt140.dll`; inspect the imported subset. Shared TextureCooker also needs Core/Tasks DLLs. Shared AssetCooker also needs GameFramework; both AssetCooker configurations retain `assimp-vc145-mt.dll`. Neither tool gains RHI/Platform or processor/plugin DLLs. Debug CRT is a prepared development prerequisite, not a Shipping redistributable.

The existing notices contain DirectXTex MIT; KTX Apache/Basis/miniz, BSD uthash/zstd and Ericsson ETC terms; and retained provider notices. Source reconstruction compiles the pinned processing libraries rather than taking opaque SDK binaries. Qualified installed MSVC CRT binaries and their hashes remain declared external inputs; Windows supplies built-in WIC codecs. Actual x64, MSVC/CRT, Debug/Shipping and static/shared identities must agree across headers, objects, tool binaries and runtime/package membership.

Failure checks use disposable cache fixtures: missing header/license/runtime/plugin, wrong revision or dirty/interrupted cache, unexpected network access, accidental GUI/full-framework/GPU targets, leaked global compiler/output settings, stale codec targets and SDK edges into Game/Editor. Preserve the active cache and unrelated edits. Existing cold, warm/offline, Launcher/workspace readiness, explicit sync and relocated enabled-source checks passed; they do not establish a physical clean OS or complete release package.

## Evidence and current acceptance

Evidence roots below are under ignored `build/validation/texture-cooking/`. Each root binds source/tool/input hashes, commands, results and scope. Frozen original `budgets.json` SHA-256 remains `c1b56b60c13d735029470a18d6c3b8506be6f8d428ac0a98f045f130aa33609b`.

| Root | Evidence retained |
| --- | --- |
| `20261112-stage1` | Frozen source/executable/corpus, budgets, baseline defects, costs and independent oracle identities. No later candidate rewrites these inputs. |
| `20261116-prerequisites`, `20261116-stage2`, `20261117-stage2-closure` | Source/processor prerequisites and historical selection/no-go results under the original filter requirement. |
| `20261117-directxtex-migration` | Combined stages 3-4: four enabled host consumers, real default/material workflows, permitted-file relocation, ABI/import/link/notices, cache failures and Launcher sync/readiness. Replay has 212 cases: 182 accepted and 30 expected rejects. Native KTX has 36 containers / 144 subresources; extra source/preservation and independent BC5 checks pass. |
| `20261117-structure`, `20261117-deduplication` | Provider/SRP cleanup, Core bounded-reader reuse, retired pixel conversions and single staged-publication authority. Exact replay, shared/static API checks and fault preservation. The intermediate structure snapshot reopened the byte floor; later deduplication closed it. Historical footprint values remain snapshot-bound. |
| `20261117-stage5` | Reproduced source/stage collision and single-worker WIC COM defects, fixes, source bounds, deterministic worker counts, native allocation/read/write/close failures and interrupted publication/recovery. Cooperative/per-request trial failures remain failed. |
| `20261117-prerequisite-closure` | Delivery policy, actual Core process cancellation, memory/fault/publication checks and real reader/validator checks. Four Renderer/RHI builds; 26 cases / 104 actual engine uploads; separate native D3D12 byte oracle. Warm timing remains inconclusive. |
| `20261118-stage6` | Actual engine sampling/input readback, backend array-view repair, independent source/native-filter references, HDR failure, focused image review, startup and tool binary size. `stage6-results.json` is the verdict; `sampling-protocol.json` retains initial probe errors and their corrected reference model. |
| `20261118-cleanup` | Stateless batch shell retired in favor of the direct operation, owning files/CMake/caller renamed together, unused qualifier and empty legacy directories removed. Four tool builds and 278 replay checks pass, preserving exits/bytes and known quality outcomes. Debug compares with its preserved Debug predecessor: the normal-map difference from Shipping predates cleanup. Historical plan narrative is preserved in `before.zip`; source identity, membership, style and footprint receipts qualify this snapshot. |

The source/staging collision fix validates protected paths before cleanup. Worker-lifetime COM fixes mixed-image one-worker replay. Fault qualification covers allocation/read/write/close failures, mixed batches, duplicate/conflicting IDs, path aliases, malformed metadata, 64 MiB file admission, 1x16384/2048-layer bounds and a forced interruption after the first backup in a 1024-file publication. Incomplete generations remain unavailable until explicit cleanup/recook. The separate Core explicit-flush injection is not evidence of an additional texture-writer flush operation or physical disk-full/power loss.

Sampling uses the actual Renderer reader/factory and RHI resource/view/upload/capture services, with disposable native diagnostic compute pipelines. Both backends now create array views for multi-layer inputs and retain cube views. All **182 accepted corpus textures plus seven supplemental fixtures / 669 subresources** pass decode/view checks on D3D12 and Vulkan in Debug/Shipping: **2676 sampled readbacks**, with identical floats across APIs/profiles. The actual engine-created D3D12 input resources read back byte-exactly for all 669 subresources in each profile. These cover arrays, labeled cube faces, NPOT BC, native formats and authored/generated chains. Execution uses a private non-input desktop.

On the observed NVIDIA RTX 5070 Ti Laptop GPU, unaligned top-level BC support is enabled. This is device/runtime evidence, not a guarantee for other devices. Hardware decode comparisons use Microsoft's [BC interpolation tolerance](https://microsoft.github.io/DirectX-Specs/d3d/archive/D3D11_3_FunctionalSpec.htm#19.5.2%20Error%20Tolerance) and [sRGB precision](https://microsoft.github.io/DirectX-Specs/d3d/archive/D3D11_3_FunctionalSpec.htm#3.2.3.7%20SRGB%20-%3E%20FLOAT). Correcting the probe's initial fixed linear/LSB bound did not change source-to-encoded quality budgets.

All four static/shared Debug/Shipping cookers produce identical supplemental bytes. Constant HDR, meaningful HDR alpha, positive-Z normal reconstruction, NPOT linear filtering and cubic color filtering pass independent source-derived references and applicable limits. Cubic references use independent polynomial interpolation with clamped boundaries. Codex reviewed the native-size/4x source/output/signed-difference and labeled cube/array figures. HDR uses common display transfer; numerical HDR failures remain visible. This is focused image review, not complete material/interactive rendering acceptance.

| Remaining gate | Result and required work |
| --- | --- |
| BC6H HDR quality | **FAIL.** An admitted opaque 8x8 varying-HDR EXR fails unchanged limits at mips 0-2 in all sampled configurations. Top-level normalized RMS is **0.1605**, limit **0.04**; old encoder **0.1486** also fails. Candidate regression exceeds the original **0.005** allowance, and lower mips regress further. Uploaded bytes are exact and GPU/CPU decode agree, locating the issue in encoded quality. Preserve `source-quality.json` and `hdr-quality-comparison.json`; resolve within the selected processor policy. The pinned CPU BC6H entrypoint ignores compression flags, so an unqualified quality selector is not a remedy. |
| Complete visuals | Finish representative material shading, normal angular maps, alpha over black/white/checker, remaining corpus and interactive rendering review. Focused figures do not close these claims. |
| Warm/cold/build costs | Warm result is **INCONCLUSIVE**: ten alternating pairs, about 25.6 ms baseline / 30.6 ms candidate; paired interval crosses the fixed allowance. Keep raw pairs. Cold needs five controlled reboot/cache pairs without purging the shared workspace; clean/incremental builds remain unqualified. |
| Final delivery | Finish product package/notices membership, imports and relocation; run the admitted source set on a prepared machine without optional codec packs or SDK installation; execute source reconstruction using pinned source and declared binary/toolchain inputs. Tool-only import/guard tests do not establish these gates. |

Startup passes its frozen allowance: two discarded warmups, ten alternating pairs on AC, about **18.2 ms old / 18.6 ms new**, paired 95% interval **0.22-0.96 ms**, allowance **10 ms**. Permitted tool executable/DLL bytes also pass the 20 MiB added-size allowance; neither result is full product acceptance. These measurements remain bound to the stage-6 binary snapshot.

Retain original cost limits: warm max(10%, 2 ms), cold max(15%, 10 ms), startup max(20%, 10 ms), clean build max(25%, 30 s), incremental build max(10%, 2 s), added fetch 200 MiB, upstream build 512 MiB and tool package 20 MiB. Two warmups, ten alternating pairs and paired bootstrap 95% intervals govern warm/startup comparisons; overlap is inconclusive. Do not relabel uncontrolled first runs as cold or waive failed budgets implicitly.

Temporary validation sources, targets, fixtures and binaries are archived and removed before handoff; cleanup receipts identify each archive and removal. Active dependency caches, product build variants, unrelated worktrees and edits are preserved. The full pre-cleanup plan remains in the cleanup archive, so consolidation does not discard failures or numerical decisions.

**Next action:** resolve the demonstrated HDR quality failure before final acceptance, then finish the open visual, controlled cost, release-package/clean-machine and source-reconstruction gates. Keep one processor and the existing dependency workflow. Record any deliberate requirement revision separately from original budgets and rerun affected checks. Delete this plan and update the roadmap only after all final gates close.
