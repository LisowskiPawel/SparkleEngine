# Texture cooking migration

[Migration strategy](../ThirdPartyMigration.md#texture-cooking) | [Developer guide](../Engineering.md) | [Code style](../CodeStyle.md)

Temporary implementation plan. Delete it after final acceptance and fold lasting guidance into the migration guide.

## Current decision

**DirectXTex is selected under the user's revised contract of 2026-11-17. Stages 3 and 4 are implemented together and undergoing final qualification; stages 5 and 6 remain open.** The user explicitly removed exact Kaiser equivalence as an adoption condition and directed integration using suitable supported processing. The previous stage 2 no-go applied to the original filter requirement and is superseded. No NVTT installation or personal terms acceptance is needed for the selected Microsoft source library.

Preserve texture meaning, color/alpha/channel intent, meaningful normal filtering, HDR validity, bounded ownership, cancellation and accepted-output publication. Exact legacy kernels are retired. Color groups use native cubic filtering; data and decoded normals use native linear filtering. Generated normal levels use DirectXMath normalization through DirectXTex's row-transform API. NoMips and supplied levels retain their authored normal values. There is no public filter selector or fallback processor.

The allowed processor set remains AMD Compressonator Framework, NVIDIA Texture Tools 3 and Microsoft DirectXTex. Existing stb, TinyEXR and KTX-Software remain only as source providers for demonstrated gaps. Do not introduce another processor, dependency manager or decoding/encoding library.

## Progress and evidence

| Stage | State and evidence |
| --- | --- |
| 1. Capture | Complete for baseline capture, including recorded failures and gaps. Original executable revision `6e44a2d4ec11ad66d00b2f7b92a8be73a1679360`; frozen artifacts remain unchanged. |
| 2. Select | Reopened and selected DirectXTex after the explicit requirement revision. CPU NPOT linear/cubic, BC1/4/5/6H callbacks/abort and Microsoft vector transformation passed. Original Kaiser/no-go results remain historical. |
| 3. Dependency/delivery integration | Implemented with stage 4. Official pinned acquisition, source readiness/identity, private static linkage, notices and package provenance are in place. Final host matrix and Launcher checks are in progress. |
| 4. Replace processing | Implemented. Native buffers reach the staged writer; old pipeline, DDS parser, block packing and filter/format scaffolding are removed. Final consumer, resource, numerical and footprint checks are in progress. |
| 5. Fault qualification | Open. Focused cancellation/admission probes pass; comprehensive allocator/I/O/publication fault injection and measured resource limits remain. |
| 6. Native/delivery acceptance | Open. Final D3D12/Vulkan upload and visual results, clean-machine package/source reconstruction and release acceptance remain. |

Raw evidence lives in ignored `build/validation/texture-cooking/`: `20261112-stage1`, `20261116-prerequisites`, `20261116-stage2`, `20261117-stage2-closure` and `20261117-directxtex-migration`. The latest root contains the explicit requirement revision, copied integration baseline, commands, exit codes, executable/input hashes and isolated candidate results. Earlier baselines and failures are not rewritten into passes. Original `budgets.json` SHA-256 is `c1b56b60c13d735029470a18d6c3b8506be6f8d428ac0a98f045f130aa33609b`.

Production work is isolated in `C:/s/texture-cook-20261117`. Merge only its migration delta against `integration-base.zip`; preserve concurrent changes to Application, Core, RHI, importing, cooking and Launcher. Bind final evidence to file/executable hashes because HEAD alone does not identify this dirty checkout. Temporary probes/targets must be archived and removed before handoff.

## Final ownership

| Responsibility | Owner and replacement |
| --- | --- |
| DDS headers, layouts, source blocks | DirectXTex metadata/load APIs; the Sparkle DDS parser is deleted. |
| Raster decoding | DirectXTex WIC/HDR loaders. The existing stb implementation is compiled only for PSD/PIC/PNM/TGA; TinyEXR imports flat single-part EXR. |
| Native KTX1/KTX2 containers | Existing pinned `ktx_read`; a private format table maps admitted GL/Vulkan encodings to the existing cooked format vocabulary. |
| Decoded pixels, mip chains, compression output | `DirectX::ScratchImage` and its native metadata. No WorkingTexture, TextureLoadResult or duplicate layer/mip byte tree remains. |
| Conversion, decompression and mip kernels | DirectXTex Convert, Decompress, GenerateMipMaps and TransformImage; DirectXMath supplies color conversion and vector normalization. |
| BC encoding and block preparation | CPU CompressEx for each native subresource. Native callbacks provide cancellation; there are no local block/tile/half-conversion loops or encoder option owners. |
| Material/default meaning and identity | Neutral requests in TextureCookShared and the existing MaterialCooker/default producers. Obsolete filter fields are removed from serialization and identity; recook generated content. |
| Resource lifetime and cancellation | Existing task workers and stop tokens. Separately bound encoded-file/probe storage and metadata-sized processing reservations before decoded allocation. The native image and its reservation settle together. |
| `.stex` validation/writing/publication | Validate canonical native row/slice layouts and borrow native bytes into the existing temporary-file and FileSetPublisher flow. Runtime loading acquires no processing dependency. |

Retain the stb TGA family because the native TGA loader rejects 16-bit gray-plus-alpha input (the prepared fixture returns HRESULT 0x80070032); this also keeps one provider authority for TGA subtypes.

Keep the worker COM apartment lifetime. WIC source decoding relies on built-in Windows PNG/JPEG/BMP/GIF codecs, not an optional codec pack. Reject animation/multiframe input. HDR and retained providers do not depend on installed WIC extensions.

## Supported boundary

Admit 2D textures and 2D arrays, axes 1–16384, arrays up to 2048, and exactly six square cube faces, subject to checked memory limits. Native chains are item/face-major and mip-minor. NoMips projects each top level; PreserveExisting retains supplied levels; Generate rebuilds from the top levels.

KTX admits RGBA8 linear/sRGB, RGBA32F, BC1/2/3 linear/sRGB, BC4/5 UNORM/SNORM, BC6H unsigned and BC7 linear/sRGB. Reject supercompression, Basis/UASTC/ETC/ASTC payloads, unsupported/signed BC6H, volumes and cube arrays. Compressed KTX requires right/down orientation for exact block preservation; native uncompressed orientation is transformed by DirectXTex. DDS interpretation belongs to the native parser; do not reinstate the old parser to enforce its historical quirks.

Byte sources use the requested input color policy, with alpha independent of RGB transfer. Float sources remain linear. Inspect all effective subresources before choosing BC1, BC4, BC5, BC6H or an uncompressed format. Preserve meaningful HDR alpha in RGBA32F. Reject non-finite HDR data and positive overflow before BC6H unsigned encoding; negative values follow the unsigned codec's clamp policy. Supplied compressed blocks can pass unchanged when channels and mip policy allow it.

The default processing reservation pool remains 1 GiB. Each source file is capped at 64 MiB. Encoded-file/probe reservations are separate from decoded processing reservations so waiting upgrades cannot deadlock each other. The metadata estimate includes overlapping native float chains, provider decode, filter rows, descriptors and encoded staging. Cancellation interrupts admission waits and BC encoding, is checked between native operations and before/during staged writing, and settles owned buffers before release. Stage 5 must challenge the estimate and the uninterruptible native call bounds; reservations are estimates, not an intercepting third-party allocator.

## 3–4. Combined implementation acceptance

Use the current CMake/Launcher cache, explicit Sync/Clean, immutable Git revisions, required-file checks and existing package/notices metadata. Keep source/generated inputs in ignored dependency storage and preserve the user's active cache. Only the selected processor and required providers belong to TextureCooker's closure.

| Input | Identity and composition |
| --- | --- |
| DirectXTex | `may2026`, commit `4feb3e11a020f35b796fc769a74216a555d4f5ef`, upstream static `DirectXTex` target; CPU only. Tools, samples, DX11/DX12, OpenMP and optional external image libraries disabled. |
| KTX-Software | `v4.3.2`, commit `91ace88675ac59a97e55d0378a6602a9ae6b98bd`, official sparse Git checkout, upstream static `ktx_read`. Writer/encoder/tools/tests/GPU upload/bindings are outside the built tool closure. Its bundled reader components still count and carry notices. |
| Other provider inputs | Existing pinned stb and TinyEXR source headers, existing zlibstatic for TinyEXR. No separately acquired processor binary, plugin or runtime. |

Processor/provider static libraries are `DirectXTex.lib`, `ktx_read.lib` and `zlibstatic.lib`; stb/TinyEXR compile into the executable. Sparkle Core/Tasks/ToolSupport/TextureCookShared are the existing integration closure. TextureCooker consumes RHI contracts without linking rendering backends. DirectXTex/KTX types remain private; TextureCookShared, public requests, Core, Renderer, RHI, Game and Editor acquire no SDK processing edge.

Shipping delivery uses the existing Microsoft CRT bundle: `msvcp140.dll`, `msvcp140_1.dll`, `msvcp140_2.dll`, `msvcp140_atomic_wait.dll`, `msvcp140_codecvt_ids.dll`, `vcruntime140.dll`, `vcruntime140_1.dll` and `concrt140.dll`. The bundle is the permitted runtime inventory; import inspection records the subset used by the tool. Shared Sparkle modules additionally stage SparkleCore.dll through the existing runtime-support contract. There are no DirectXTex, AMD, NVTT, KTX or processing plugin DLLs. Inspect the actual imports and transitive build/link graph for each enabled configuration; a private edge or successful upstream sample alone is insufficient. Debug runtimes are development prerequisites, not redistributable Shipping files.

`ThirdPartyNotices.txt` carries DirectXTex MIT and KTX Apache/Basis/miniz, BSD uthash/zstd and Ericsson ETC terms, alongside retained provider notices. Source reconstruction builds these libraries from the official pinned sources using the recorded MSVC/Windows SDK; they are not opaque binary inputs. Existing CRT and shared Sparkle support remain binary delivery inputs governed by the existing packaging workflow.

Complete stages 3–4 only after real TextureCooker and AssetCooker builds/runs in affected Debug/Shipping static/shared configurations, standalone Launcher/workspace identity/readiness agreement, individual Sync/Clean and full Sync Code, cold and warm/offline cache checks, relocated enabled codec paths, import/link/package inventory, current producer/serialization behavior and the fixed net retirement floor. Run meaningful source/numerical/preservation checks and source formatting; remove obsolete build membership/references. Do not claim native rendering, physical clean-machine or release acceptance from these checks.

The retirement floor remains at least **35% net bytes and physical lines** against the original 107355-byte/3346-line source boundary, charging added native adapters, caller changes and build/Launcher wiring. Each retained mechanism needs its current semantic, trust-boundary or lifetime purpose. A numerical floor does not justify leftover generic algorithms or mirrors.

Failure checks: missing headers/licenses/runtime, wrong or dirty cache, interrupted sync, unexpected network, GPU/full-framework targets, leaked compiler/output settings, stale Compressonator targets and processing imports in Game/Editor. Use disposable fixtures. Failure preserves cache evidence and accepted output; no automatic reset or fallback processor.

## 5. Qualify memory, cancellation and publication

```text
Execute stage 5 of Docs/Implementation/TextureCooking.md against the delivered DirectXTex path.
Follow Docs/Engineering.md and Docs/CodeStyle.md. Preserve frozen inputs and use ignored disposable fixtures.
Challenge metadata admission, the two reservation lifetimes, worker-count determinism, native call cancellation bounds,
allocation/read/write/flush/close/finalization failures, cancellation and worker settlement, duplicate/conflicting requests,
mixed-batch preservation and interrupted publication. Fix the real owners, retain no permanent test-only scaffolding,
and update this plan with file/executable/input identity, raw evidence, failures and the next stage.
```

Test 1×1, skinny and NPOT shapes, maximum admitted dimensions/layers/chains, large encoded files, malformed KTX/DDS/EXR and scalar/alpha changes outside the top level. Measure actual peak memory and cancellation latency separately from reservations. A crash/kill is interruption evidence, not cooperative cancellation. Accepted output must be complete and valid or preserved; no partial generation may be published. Close only with measured passing limits and restored fixtures.

## 6. Verify native output and delivery

```text
Execute stage 6 of Docs/Implementation/TextureCooking.md on the final integrated files.
Verify the admitted source/codec set against independent pixel references and changed native filter semantics;
exercise actual D3D12/Vulkan uploads, arrays/cubes and NPOT BC layouts, visual material/normal/HDR results,
serial/concurrent determinism and the fixed performance/footprint limits. Verify final package membership,
imports/notices, relocation, a prepared machine without optional codecs or an SDK, and source reconstruction.
Keep missing native/visual/clean-machine evidence open. Remove temporary probes and obsolete references.
At closure fold lasting guidance into the owning guide, delete this temporary plan and update the roadmap.
```

Use independent decoders for lossy quality, not the selected encoder's decoder alone. Keep filter differences permitted by the user distinct from color, alpha, normal, HDR, layout and publication defects. The recorded Vulkan row-pitch and D3D12 unaligned-BC admission failures remain native acceptance work until rerun successfully. Do not promote CPU artifact results to GPU/visual or release claims.

**Next action:** finish combined stage 3–4 host/dependency/consumer qualification, merge only the owned migration delta, bind results to the final files, and then execute stage 5.
