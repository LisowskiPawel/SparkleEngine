# First Release Scope

This page defines the first release: three products, seven selectable maps, user-selected display resolution, a 30 FPS acceptance target on your laptop, and no user support service.

**Status:** six product choices approved on 2026-10-10; technical scope and exact rights closure remain blocked. Free educational intent supplies no redistribution grant.

[Roadmap](../../Strategy/Roadmap.md#release-progress) shows progress. [Capability Dispositions](CapabilityDispositions.md) owns feature classifications; [First Release](../FirstRelease.md#release-gates) owns shipping checks.

## Approval And Next Work

**All six choices are approved.** This is the final decision record, not another approval request. The resolution and lens-effect changes below replace the earlier proposals.

| # / decision | Approved value | Delivery obligation |
| --- | --- | --- |
| 1. Packaging | Portable Windows x64 `v0.1.0` bundle: Launcher, Editor and Game, plus source tag/build instructions. Users normally enter through Launcher; optional maps use a verified selected-content store. | Qualify all three products and admitted helper/content closure on a clean machine. No installer, automatic updater or stable plugin SDK. |
| 2. Windows | Windows 11 x64 24H2/build 26100 or newer; named laptop is the initial hardware/performance target. | Qualify the OS boundary and reject missing requirements; do not advertise unqualified hardware. |
| 3. Controls/display | English keyboard/mouse; one display; windowed/borderless; use the user's display resolution, with resolution changes available. Help, map selection, camera reset, settings Save/Reset and Quit. | No forced 720p/1080p/1440p list or fixed internal resolution. Verify supported display modes, Unicode paths and 100/150/200% DPI. Controller, exclusive fullscreen, VR, localization and broad accessibility certification remain excluded. |
| 4. Defaults | D3D12; Lit; VSync On; automatic histogram exposure; ACES approximation; SDR; neutral grading; **chromatic aberration On and vignette On**; HDR opt-in. Empty without selected content. | Implement requested/active settings honestly. Freeze effect strengths in their owning feature contracts; no invented numerical values or silent disabled fallback. |
| 5. Updates/settings | Stable documented settings/controls/map IDs within `v0.1.x`; manual download/extract updates, side-by-side rollback and optional settings reset. Disposable cache; no saves or stable source/binary API. | Preserve the documented compatibility and reset boundaries. Patch copies share settings; concurrent writes are unsupported. |
| 6. Distribution/privacy | GitHub Releases; offline Editor/Game after user-selected sync; no telemetry, account, unattended downloads or crash uploads. Inspectable local diagnostics; no user support service. | Publish verified archives, hashes, notices and required trusted signatures. Any future reporting service needs a separate scope decision. |

### Work For The Agent And Technical Owners

- Qualify [package inputs](../../Architecture/Modules/BuildAndPackaging/ReleaseInputs.md): historical asset rights, exact notices/terms, helper/CRT closure, symbols and the current baseline.
- Prove each selected map's cooked materials/reference domain and combined product/content size against unchanged budgets. Do not silently strip assets, reduce quality or substitute a map.
- Certificate enrollment waits until before signed candidate freeze; it does not block unsigned internal packaging. Cornell's optional regeneration ZIP is not a normal GLB-sync prerequisite.

## ReleaseMapSet

The intended examples are fixed; admission still requires rights, material and package checks.

| Map | Purpose / current concern |
| --- | --- |
| `ModernSponza` | Principal showcase example; base plus colorful curtains. License wording and cooked size need resolution. Candle/knight variants are excluded. |
| `Sponza` | Classic lighting scene; exact redistribution terms remain open. |
| `CornellBox` | Reference/lighting scene; tracked GLB and [retained license](../../../Projects/Showcase/Assets/Meshes/CornellBox/COPYRIGHT.txt). Normal sync no longer requires the publisher OBJ archive. Optional regeneration receipt and single-emitter/cooked proof remain open. |
| `CesiumMan` | Animation scene; retain attribution/logo notice and freeze an exact pose for reference comparisons. |
| `LPSHead` | Head/material scene; verify conversion/textures and material fidelity. Selection does not establish physical subsurface reference support. |
| `DiffuseTransmissionPlant` | Plant/animation scene; diffuse-transmission fidelity remains open. |
| `ABeautifulGame` | Chess scene; transmission/volume fidelity remains open. |

[Level recipes](../../../Projects/Showcase/Levels.catalog) and adjacent asset notices own source identities, cameras and attribution. The accepted reference domain covers surface reflection; transmission, volume, spectral and physical subsurface transport remain excluded. Unsupported material effects cannot be silently dropped.

`Empty` is an Included built-in starting/control map, outside the seven content examples; it needs no downloaded asset pack. Launcher must let users sync any selected subset of the seven, including none, and resolve each selected map's complete dependency closure before Editor/Game activation.

Excluded from release example selection: `DamagedHelmet` (rights unresolved), `ModernSponzaCandles`, `ModernSponzaKnight`, `JungleRuins`, `BistroExterior`, `BistroInteriorWine`, `SanMiguelHigh`, `SanMiguelLow`, `StanfordBunnySubsurface`, `StanfordDragonSubsurface`, `AnisotropyBarnLamp` and `SheenCloth`. Moana is deferred and has no current catalog level. These remain development/source workloads and must be marked development or unavailable in Launcher; no feature is marked Removed. The inspected catalog contains 20 levels and 18 asset packs, with no content selected by default.

## Product And Supported Matrix

<details>
<summary>Exact machine, API and source-build requirements</summary>

| Field | Selected machine / approved boundary |
| --- | --- |
| Products / current source targets | Sparkle Launcher / `SparkleLauncher`; Sparkle Editor / `ShowcaseEditor`; Sparkle Game / `ShowcaseGame` (`ShippingGame`). Exact shipped filenames, Editor/Launcher configurations and component membership require a reviewed package manifest. |
| Observed OS | Windows 11 Home 25H2, build `26200.9457`; approved target starts at Windows 11 x64 24H2/build 26100 |
| `MIN-01` / `REF-01` | Same AMD Ryzen 9 8940HX laptop, 16 cores/32 logical processors; 67,833,212,928 bytes OS-reported RAM; NVIDIA GeForce RTX 5070 Ti Laptop GPU, 12,227 MiB reported VRAM; driver `610.47` |
| Storage | Patriot P400L 2000GB NVMe SSD, NTFS; at least 16 GiB free for extraction/staging/user data |
| D3D12 | Default; feature level 12_1, SM 6.6, DXR 1.1. Reject missing required features before rendering. |
| Vulkan | Explicitly selectable; Vulkan 1.3 plus the [required feature set](../FirstRelease.md#frozen-platform-and-toolchain-prerequisites). Separate native/provider/performance proof required. |
| Source build | Exact [toolchain](../FirstRelease.md#frozen-platform-and-toolchain-prerequisites): VS Community 2026 18.7.0/MSVC 14.51.36231, Windows SDK 10.0.26100.0, CMake 4.3.3, Git for Windows 2.54.0, Qt 6.11.1 `msvc2022_64`, Vulkan SDK 1.4.350.0/Slang 2026.8, DXC 1.9.0.5347 |

This is an initial named-machine claim. Generic 8 GiB GPU/16 GiB PC floors and other hardware/drivers require qualification before advertisement. Two measurement roles on one machine do not count as independent adoption. HDR additionally requires a qualified HDR10 display/OS configuration; none is recorded. No hardware purchase is authorized. Debug/Development builds remain source products; all three public products need qualified release configurations. Alternate toolchains remain unqualified.

</details>

## Baseline And Selection

Three presets have distinct purposes: ordinary viewing, realtime path tracing, and progressive reference output. Their implementation and numerical qualification remain delivery work.

<details>
<summary>Exact presets and consumer controls</summary>

- **`REL-NATIVE-1080P`:** 1920x1080 SDR Lit, raster GBuffer, native extent/Linear provider, Ray Reconstruction Off, classic TLAS/refit On, PTLAS Off, auto batching On, Automatic Histogram exposure, ACES approximation, automatic SDR encoding. Preserve declared direct/indirect diffuse/specular, direct subsurface and shadow defaults. Consumer VSync On; performance VSync Off. Grading neutral, chromatic aberration/vignette On, no baseline decal actors; these remain Included selectable features. HDR10 opt-in after qualification.
- **`REL-RT-1080P`:** fixed 1920x1080 output for laptop acceptance only, traced primary/secondary transport, classic TLAS, DLSS Quality requested and Ray Reconstruction On; freeze exact SDK-derived internal extent and composition before measurement. Frame Generation and dynamic resolution Off during timing. Other exposed SR/RR modes need their feature proof. Freeze depth/reuse/approximation in the existing discoveries, keeping one final image. Native/RR-Off controls measure cost/quality. Ray GBuffer alone or Linear/Off fallback cannot satisfy this preset; current Lit lighting is only the starting implementation path.

- **`REL-REFERENCE`:** progressive admitted surface transport in the consumer viewport; sample/convergence/invalid state, reset/cancel and reproducible raw-linear export. No DLSS, reconstruction, firefly clamping or artistic post effects in oracle output. Full admitted reflection is required; finite-depth output is diagnostic. Freeze animation pose and reset/reject contributing scene/camera/material/light changes. [Reference semantics](../../Architecture/Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/TransportAndEstimator.md#claim-boundary) owns the domain and two distinct products.

The 1080p preset IDs identify controlled laptop acceptance runs, not a product resolution restriction. Normal viewing uses the user-selected display/output resolution and the selected provider's internal extent. Record both extents and qualify display changes. Presented Lit/realtime presets include the approved lens defaults; raw reference/oracle exports keep artistic effects disabled by their existing semantic contract.

The source snapshot differs: `DefaultEngine.ini` requests VSync Off, ACES filmic, DLSS/NativeAA, reconstruction On and PTLAS preference On; `LevelRegistry::EnsureDefaultLevel` selects Empty. `PD-1` must make intentional Empty startup and selected-map failure distinguishable and implement the approved settings/provider behavior.

| Consumer route | Approved behavior / owner |
| --- | --- |
| API | `--graphics-api D3D12` or `--graphics-api Vulkan`; precedence: build default, then `SPARKLE_RHI_BACKEND`, then CLI. Invalid token fails visibly. Application graphics launch owns it. Existing `--renderer`/`--rhi` are development conveniences. |
| Startup/examples | No selected example opens built-in Empty. Launcher offers the seven release maps, shows selected/synced/ready/failed states, syncs only the selected dependency closure and launches Editor or Game with the requested identity. Selecting Modern Sponza uses its authored camera. Failed requested content retains the prior valid state or stops visibly; Empty cannot masquerade as successful activation of that map. |
| Settings | Resolution/window mode, VSync, tracing preset and Included render/post controls; requested/active status, Save/Reset, restart indication. Application persistence and Renderer features retain authority. |
| Help/quit | F1 help, Escape menu, explicit Quit and native close. Escape releases mouse-look before opening the menu. |
| Camera | W/S or Up/Down forward/back; A/D or Left/Right strafe; E/Space up; Q/C down; Shift sprint; hold right mouse to look; wheel adjusts speed. Discoverable help, sensitivity and authored-camera reset required. [Input collector](../../../Engine/Application/Private/Input/CameraInputIntentCollector.cpp) and GameFramework camera controller own movement. |

Game excludes developer console/CVar assignment, startup-map environment selection, capture flags, shader recook, debug panels and authoring commands. Editor and Launcher deliver their admitted editing, settings, diagnostics and sync/cook/launch workflows through the existing owners; debug-build-only external capture stays development-only. Approved display/input limits are decision 3; readable labels, scalable text, keyboard navigation and visible errors remain required basics. Native refresh/VSync must be reported honestly.

</details>

## Mutable Data And Compatibility

Keep documented controls/settings/map IDs stable within `v0.1.x`; source APIs and development keys are unstable. No binary SDK, saves or migration promise. Update by verified extraction into a new directory; back up settings for rollback. Concurrent versions writing shared settings are unsupported. Reset/cache removal must preserve unrelated data; deleting the extraction does not delete user data.

The [path and budget contract](../FirstRelease.md#frozen-paths-mutation-boundary-and-budgets) owns per-user folders, immutable package bytes and deadlines. [Installed compatibility](../FirstRelease.md#compatibility-boundary-created-by-shipping) owns the shipping boundary.

## Budgets And Observation

Use the unchanged [startup, size, memory and operation limits](../FirstRelease.md#frozen-paths-mutation-boundary-and-budgets) and [30 FPS protocol](../FirstRelease.md#thirty-fps-performance-floor). Measure combined product bytes and the seven-map content closure; freeze archive/store accounting before REL-03. User-selected display resolution remains separate from the fixed laptop benchmark. Both APIs and all seven maps require proof; no dynamic resolution, fallback or post-result retuning can manufacture a pass. Reference tracing has convergence/cancellation/resource checks instead of a 30 FPS promise. [Principal frame review](../FirstRelease.md#principal-frame-review) remains required.

## Network, Security, Publication And Support

Decision 6 is approved. No user support service, response deadline or maintenance window is offered. Publication still requires the [trust/approval rules](../FirstRelease.md#approval-rules): trusted Authenticode on every shipped PE, SHA256SUMS, manifest, dependency/content notices, software bill of materials, build provenance and vulnerability/malware disposition. Keys/accounts stay outside source/archives; unsigned builds remain developer prereleases.

Source/content acquisition uses public HTTPS inputs with pinned revisions/hashes and no private credentials. Game and Editor run offline with synced content; Launcher network work is user-selected. Logs/captures/crashes may contain paths, scene content or memory; keep them local and explain privacy before any voluntary sharing. A separate symbols archive follows the same privacy boundary. No reporting channel or automated uploader is assumed.

Publish new immutable patches and rerun affected gates; retain version hashes/notices/known issues/advisories. Withdraw unsafe payloads with a visible advisory and verified rollback instructions. Never silently replace a published signed archive.

Excluded product promises: general NVIDIA/AMD/Intel-wide support, neural training/inference platform, multiplayer/networking, mods, stable SDK, installer/updater, Linux/macOS, mobile and VR. No additional scene families or AMD specialist profiler work. PTLAS and source-only external capture remain experimental within their declared vendor/debug limits.
