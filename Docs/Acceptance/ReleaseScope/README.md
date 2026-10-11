# First Release Scope

This page defines the first release: three products, seven selectable maps, user-selected display resolution, a 30 FPS acceptance target on your laptop, and no user support service.

**Use context (2026-10-10):** Pawel Lisowski describes Sparkle as a noncommercial learning project, with no plan to earn from it. This supports evaluating personal/educational asset terms for local work; it does not supply a redistribution grant or replace the approved public-delivery requirements.

**Status:** all six product proposals **approved** by the release owner on 2026-10-10, with the amendments below. No product-choice answer is pending. `REL-00` remains **BLOCKED** for technical scope closure and exact rights receipts; approval is not executable or redistribution evidence.

**Record:** `REL-00-v0.1.0-01`; latest approval `ITER-REL-00-06`, based on `d1c8c5b2a61768a0460983c854264380d5c1f6dc` plus the recorded documentation changes. [Evidence and earlier iterations](Evidence.md) retain their exact revisions and results.

**Authority:** this page owns product choices; [Capability Dispositions](CapabilityDispositions.md) owns individual feature classifications; [First Release Acceptance](../FirstRelease.md#release-gates) owns the proof required to ship.

## Decision Summary

**Already decided by you:**

- Use your existing Ryzen 9 8940HX / RTX 5070 Ti Laptop GPU / 64 GB laptop for minimum and reference measurements.
- Ship **Sparkle Launcher, Sparkle Editor and Sparkle Game**. Game is the standalone product. Source targets are `SparkleLauncher`, `ShowcaseEditor` and `ShowcaseGame`; packaging must bind each public name to its exact executable and configuration.
- Users are expected to start through **Sparkle Launcher**, then choose Editor or Game. Launcher lets users choose what content to sync and which product/map to launch. Start with built-in **Empty** if no example is selected. Deliver the seven release maps and all features they require; mark other Launcher choices development or unavailable. Selecting a missing or failed map must report failure.
- The seven maps are Cornell Box, Cesium Man, classic Sponza, Modern Sponza, LPS Head, Diffuse Transmission Plant and A Beautiful Game. Modern Sponza remains the principal showcase/performance example, rather than a mandatory first-launch download. Postpone Bistro and Moana.
- Use the user's display resolution and allow resolution changes; do not force 1080p or a fixed internal render extent. The selected rendering/upscaling mode owns internal resolution. **30 FPS on the named laptop** is the release acceptance target, measured with a fixed recorded benchmark configuration; it is not a guarantee at every user resolution or on unqualified hardware.
- Offer **no user support service**: no response deadlines, supported-version maintenance period or guaranteed fixes. Publish requirements, known issues and self-service troubleshooting. No public bug/security intake is promised by this decision.
- Require progressive reference tracing, realtime path tracing at >=30 FPS, complete DLSS Super Resolution/Ray Reconstruction, exposure/tone mapping, decals, color grading, chromatic aberration, vignette, HDR10, D3D12/Vulkan parity, a complete frame graph and principal-level frame review. The [rendering contract](../FirstRelease.md#required-rendering-closure) defines the exact obligations.

These are delivery requirements. Map selection does not establish permission to redistribute the assets or prove they render correctly.

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

These tasks require investigation or implementation evidence. You do not need to approve a hash or declare a license valid.

| Task / owner | Concrete missing result | Next action |
| --- | --- | --- |
| Cornell optional regeneration - content owner | Normal map sync uses the tracked GLB and retained notice; the unnecessary publisher-pack prerequisite is removed. Original ZIP reacquisition still returns HTTP 406. | Preserve the pinned recipe for explicit source regeneration. Do not block normal GLB acquisition on this optional archive; qualify actual cooked/package output through its owners. [Current check](Evidence.md#fr-01-exit-recheck-iter-rel-01-06). |
| Asset/dependency rights â€” rights owner | Modern Sponza notices mix CC BY 4.0 with narrower usage wording; classic Sponza names CRYENGINE-specific terms. Complete source-tree/dependency notices remain open, including Cesium's separate logo notice. | Inspect exact terms and bytes, retain obligations and resolve ambiguous permission before redistribution. |
| Publisher identity â€” release owner | Owner supplied **2026 Pawel Lisowski** for copyright and **Pawel Lisowski** for publisher; root MIT copyright is corrected. Versioned product metadata, public repository identity and signing credentials remain unqualified. | Wire and verify the approved `v0.1.0` identity through the exact product/package owners; establish the signing route. Do not invent credentials or a support service. |
| Selected-map correctness â€” content/rendering owners | Plant and chess declare material effects outside the accepted reference domain; Cornell emitter interpretation, Cesium reference pose, LPS Head materials and exact cooked closure need proof. | Qualify them in the existing feature/workload owners. Any labelled reduced-domain variant needs your explicit decision. |
| Package size and baseline â€” build/package owners | Cooked seven-map size, immutable dependencies and clean source/build/cook/run reproduction are unproved. Modern Sponza's source downloads alone exceed 4 GiB. | Measure cooked output against the unchanged limits; pin inputs and retain exact commands. Do not change budgets or strip assets silently. |

**What can happen now:** source/dependency/rights discovery may gather these receipts. Product choices are approved. The [release input owner](../../Architecture/Modules/BuildAndPackaging/ReleaseInputs.md) freezes product profiles, mandatory helpers and package/content-store paths. Exact rights/distribution decisions remain admission requirements. Owner amendment, 2026-10-11: certificate enrollment is deferred to the [final pre-release roadmap task](../../Strategy/Roadmap.md#where-we-are-and-what-is-next), before signed candidate freeze/adoption; its absence does not block FR-01 or unsigned internal package development. Preserve every approved feature and map. Unresolved rights or mutable inputs still stop payload publication; approval does not supply a license grant.

## ReleaseMapSet

The intended examples are fixed; admission still requires rights, material and package checks.

| Map | Purpose / current concern |
| --- | --- |
| `ModernSponza` | Principal showcase example; base plus colorful curtains. License wording and cooked size need resolution. Candle/knight variants are excluded. |
| `Sponza` | Classic lighting scene; exact redistribution terms remain open. |
| `CornellBox` | Reference/lighting scene; retained license and GLB format verified in [ITER-CORNELL-01](Evidence.md#cornell-license-and-format-iter-cornell-01). Normal sync no longer requires the publisher OBJ archive. Optional regeneration receipt and single-emitter/cooked proof remain open. |
| `CesiumMan` | Animation scene; retain attribution/logo notice and freeze an exact pose for reference comparisons. |
| `LPSHead` | Head/material scene; verify conversion/textures and material fidelity. Selection does not establish physical subsurface reference support. |
| `DiffuseTransmissionPlant` | Plant/animation scene; diffuse-transmission fidelity remains open. |
| `ABeautifulGame` | Chess scene; transmission/volume fidelity remains open. |

[Exact source hashes, authored cameras, notices and upstream metadata](Evidence.md#selected-content-receipts) are retained separately. The accepted reference domain covers surface reflection; transmission, volume, spectral and physical subsurface transport remain excluded. Unsupported material effects cannot be silently dropped.

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

<details>
<summary>Exact settings, update, rollback and removal promises</summary>

The [path/budget contract](../FirstRelease.md#frozen-paths-mutation-boundary-and-budgets) owns immutable package bytes and `%LOCALAPPDATA%\SparkleEngine\Showcase\v0.1\` user data: separate Settings, Logs, Captures, Crashes and Cache folders. A standard user can run from read-only extraction; no repository, SDK or writable package root is required.

Published tag, manifest schema identity, archive/checksums and cooked-generation hashes are immutable per release. The seven map IDs above remain the `v0.1.x` public IDs. Schema/bytes change only under a new release identity.

Documented Included settings/controls remain interpretable within `v0.1.x`; invalid values visibly reset safely, optional keys may be added, and development keys are unstable. Cache is generation/version-qualified and disposable; clearing it preserves settings. No save-game/migration/account-state promise. Source APIs may change across `0.y.z`; no binary ABI/plugin SDK. Patches cannot rename documented controls or silently change accepted map semantics. `--graphics-api` and the documented backend environment selection retain their values/precedence within `v0.1.x`; other development flags remain unstable.

Update by downloading, verifying and extracting a new directory. Back up Settings before a patch; roll back by launching the previous verified extraction and restoring its settings backup. Patch versions share `v0.1` settings; different minor versions have separate roots. Concurrent versions writing shared settings are unsupported. Reset through the product or remove only its settings file; clear optional data folders separately. Delete the extraction to uninstall; deleting user data is a separate opt-in action. No automatic update or destructive migration is admitted.

</details>

## Budgets And Observation

Keep the existing [operation limits](../FirstRelease.md#frozen-paths-mutation-boundary-and-budgets): first UI <=15 s, first example <=30 s, other map <=60 s, quit <=5 s, archive <=4 GiB, extracted <=8 GiB, process peak commit <=12 GiB, tracked local GPU peak <=7 GiB, no unbounded growth. Source/Launcher deadlines remain in that owner. The three-product/selected-content split does not waive size or memory limits; measure combined product bytes and the complete seven-map content closure, then freeze archive/store accounting before REL-03.

Native and realtime presets must each satisfy the [30 FPS contract](../FirstRelease.md#thirty-fps-performance-floor) on all seven maps and both APIs: ShippingGame, fixed 1920x1080 benchmark output (Lit native; realtime frozen DLSS Quality/RR internal extent), VSync Off, >=300 warmup and >=300 valid sampled frames, three repeats; presented/app, CPU and GPU p95 each <=33.33 ms. Use AC power, fixed Windows/OEM performance mode and steady thermals. Record p99, 1% low, worst frames, memory/clocks, map/cooked generation, authored camera/animation time, render/output extents, settings, driver and backend. No capture/validation/profiler in timing runs, generated frames, dynamic resolution, fallback or post-result retuning. Lit uses Linear; realtime uses its frozen reconstruction topology. Chromatic aberration and vignette are On in the presented performance presets; effect parameters are fixed before results. Modern Sponza's detailed motion/disocclusion workload is frozen by `MAP-00`. Reference has convergence/cancellation/resource budgets rather than a 30 FPS promise. [Principal frame review](../FirstRelease.md#principal-frame-review) uses separate captures and observer-controlled timings.

## Network, Security, Publication And Support

Decision 6 is approved. No user support service, response deadline or maintenance window is offered. Publication still requires the [trust/approval rules](../FirstRelease.md#approval-rules): trusted Authenticode on every shipped PE, SHA256SUMS, manifest, dependency/content notices, software bill of materials, build provenance and vulnerability/malware disposition. Keys/accounts stay outside source/archives; unsigned builds remain developer prereleases.

Source/content acquisition uses public HTTPS inputs with pinned revisions/hashes and no private credentials. Game and Editor run offline with synced content; Launcher network work is user-selected. Logs/captures/crashes may contain paths, scene content or memory; keep them local and explain privacy before any voluntary sharing. A separate symbols archive follows the same privacy boundary. No reporting channel or automated uploader is assumed.

Publish new immutable patches and rerun affected gates; retain version hashes/notices/known issues/advisories. Withdraw unsafe payloads with a visible advisory and verified rollback instructions. Never silently replace a published signed archive.

Excluded product promises: general NVIDIA/AMD/Intel-wide support, neural training/inference platform, multiplayer/networking, mods, stable SDK, installer/updater, Linux/macOS, mobile and VR. No additional scene families or AMD specialist profiler work. PTLAS and source-only external capture remain experimental within their declared vendor/debug limits.

## FR-00 Review Package

The [reconciliation and iteration evidence](Evidence.md#fr-00-review-package) retains the source identities, license observations and historical checks. The latest audit accounted for 512 capability IDs, 20 levels, 18 view modes and 26 persisted names. Its static checks passed; product choices are approved; technical scope closure and rights remain **BLOCKED**. Accepted path-tracer development discovery remains preserved; reference release-oracle, native, build, cook, package, performance and adoption results remain unaccepted.
