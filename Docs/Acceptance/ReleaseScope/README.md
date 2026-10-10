# First Release Scope

This page decides what people get in SparkleEngine's first public release and what you promise to support. Start with the settled choices, then answer the numbered proposals below.

**Status:** scope proposal; `REL-00` (release-scope approval) remains **BLOCKED**. This edit records no new approval.

**Record:** `REL-00-v0.1.0-01`; latest source reconciliation 2026-10-10 at `aa9fcd94f219e8a3d4dbe7c51457680ad9300783`. [Evidence and earlier iterations](Evidence.md) retain their exact revisions and results.

**Authority:** this page owns product choices; [Capability Dispositions](CapabilityDispositions.md) owns individual feature classifications; [First Release Acceptance](../FirstRelease.md#release-gates) owns the proof required to ship.

## Decision Summary

**Already decided by you:**

- Use your existing Ryzen 9 8940HX / RTX 5070 Ti Laptop GPU / 64 GB laptop for minimum and reference measurements.
- Include seven maps: Cornell Box, Cesium Man, classic Sponza, Modern Sponza, LPS Head, Diffuse Transmission Plant and A Beautiful Game. Open Modern Sponza at native 1920x1080 in Lit mode. Postpone Bistro and Moana.
- Require progressive reference tracing, realtime path tracing at >=30 FPS, complete DLSS Super Resolution/Ray Reconstruction, exposure/tone mapping, decals, color grading, chromatic aberration, vignette, HDR10, D3D12/Vulkan parity, a complete frame graph and principal-level frame review. The [rendering contract](../FirstRelease.md#required-rendering-closure) defines the exact obligations.

These are delivery requirements. Map selection does not establish permission to redistribute the assets or prove they render correctly.

## Approval And Next Work

**Your remaining decisions are the proposals below.** All are unanswered. You can answer by number, approve a row, or replace its proposed value. A change to an existing acceptance requirement must also update its owner; this page cannot waive it.

| # / decision | Current proposal | What accepting it commits us to |
| --- | --- | --- |
| 1. What do we publish? | A portable Windows x64 Showcase app (`ShowcaseRuntime.exe`), version `v0.1.0`, plus a public source tag and build instructions. Launcher, Editor and import/cook/compiler tools are built from source; no separate developer archive. | People can download and run the demo without a development environment, or reproduce it from source. No installer, automatic updater or stable plugin SDK. |
| 2. Which Windows versions? | Windows 11 x64 **24H2/build 26100 or newer**, using the named laptop as the initial hardware target. | Your inspected machine runs 25H2. Supporting the earlier 24H2 boundary still needs validation; we cannot infer it from your laptop. Other hardware remains unqualified. |
| 3. Which controls and display options? | English keyboard/mouse UI; one display; windowed/borderless; 720p, 1080p and 1440p when available. Help, example selection, camera reset, settings Save/Reset and Quit. | Build these consumer controls and check Unicode paths and 100/150/200% scaling. Performance is promised at 1080p only. Controller, exclusive fullscreen, VR, localization and broad accessibility certification are excluded. |
| 4. What are the remaining startup defaults? | D3D12; VSync On; automatic histogram exposure; ACES approximation; SDR output; neutral grading; chromatic aberration/vignette Off; HDR opt-in. Exact render defaults are [below](#baseline-and-selection). | Implement these defaults and show requested/active settings and unavailable options clearly. Your Modern Sponza/native 1080p Lit choice is already settled. |
| 5. What survives an update? | Keep documented consumer settings, controls and map IDs stable within `v0.1.x`. Manual download/extract updates, previous-version rollback, optional settings reset. Cache is disposable; no saves or stable source/binary API. | Maintain the [settings and update promises](#mutable-data-and-compatibility). Side-by-side patch versions share settings; concurrent writes are unsupported. |
| 6. How do we distribute and handle privacy? | GitHub Releases; offline runtime; no telemetry, account, automatic asset downloads or crash uploads. Reports are voluntary; sensitive dumps/symbols shared privately with consent. | Verify offline operation and publish versioned archives, hashes and notices. Trusted executable signing is already required by the [acceptance contract](../FirstRelease.md#approval-rules); identity/contact/credentials still need provisioning. |
| 7. What support do you offer? | You, as maintainer, own public bug reports and a private security contact. Support `v0.1.x` until an accepted replacement is published and this line is explicitly retired. | Existing acceptance targets are: acknowledge bugs within **5 business days**, security reports within **3 business days**, and contain/withdraw confirmed critical (`S0`) problems within **1 business day**. These are real commitments, not promised fix dates. No commercial SLA or always-on support. |

### Work For The Agent And Technical Owners

These tasks require investigation or implementation evidence. You do not need to approve a hash or declare a license valid.

| Task / owner | Concrete missing result | Next action |
| --- | --- | --- |
| Cornell source receipt — content owner | The original 224,880-byte archive matching the catalog SHA-256. Publisher requests returned HTTP 406; the inspected cache did not contain it. | Reacquire/verify that archive and connect its source/notices to the local conversion. [Exact receipt](Evidence.md#selected-content-receipts). |
| Asset/dependency rights — rights owner | Modern Sponza notices mix CC BY 4.0 with narrower usage wording; classic Sponza names CRYENGINE-specific terms. Complete source-tree/dependency notices remain open, including Cesium's separate logo notice. | Inspect exact terms and bytes, retain obligations and resolve ambiguous permission before redistribution. |
| Publisher/contact — release owner | Root MIT license still has `[year] [fullname]`; publisher identity, repository URL, support/security contact and signing credentials are not provisioned. | Collect actual identity/contact information and establish the signing route. No invented owner or mailbox. |
| Selected-map correctness — content/rendering owners | Plant and chess declare material effects outside the accepted reference domain; Cornell emitter interpretation, Cesium reference pose, LPS Head materials and exact cooked closure need proof. | Qualify them in the existing feature/workload owners. Any labelled reduced-domain variant needs your explicit decision. |
| Package size and baseline — build/package owners | Cooked seven-map size, immutable dependencies and clean source/build/cook/run reproduction are unproved. Modern Sponza's source downloads alone exceed 4 GiB. | Measure cooked output against the unchanged limits; pin inputs and retain exact commands. Do not change budgets or strip assets silently. |

**What can happen now:** source/dependency/rights discovery may gather these receipts. Full identity/baseline stage (`FR-01`) execution and product implementation wait for scope approval and their own prerequisites. Unresolved rights or mutable inputs stop package work. Approval of the numbered proposals does not itself clear those blockers.

## ReleaseMapSet

The intended examples are fixed; admission still requires rights, material and package checks.

| Map | Purpose / current concern |
| --- | --- |
| `ModernSponza` | First-run scene; base plus colorful curtains. License wording and cooked size need resolution. Candle/knight variants are excluded. |
| `Sponza` | Classic lighting scene; exact redistribution terms remain open. |
| `CornellBox` | Reference/lighting scene; original archive receipt and single-emitter interpretation need closure. |
| `CesiumMan` | Animation scene; retain attribution/logo notice and freeze an exact pose for reference comparisons. |
| `LPSHead` | Head/material scene; verify conversion/textures and material fidelity. Selection does not establish physical subsurface reference support. |
| `DiffuseTransmissionPlant` | Plant/animation scene; diffuse-transmission fidelity remains open. |
| `ABeautifulGame` | Chess scene; transmission/volume fidelity remains open. |

[Exact source hashes, authored cameras, notices and upstream metadata](Evidence.md#selected-content-receipts) are retained separately. The accepted reference domain covers surface reflection; transmission, volume, spectral and physical subsurface transport remain excluded. Unsupported material effects cannot be silently dropped.

Excluded from the initial consumer package: `Empty`, `DamagedHelmet` (rights unresolved), `ModernSponzaCandles`, `ModernSponzaKnight`, `JungleRuins`, `BistroExterior`, `BistroInteriorWine`, `SanMiguelHigh`, `SanMiguelLow`, `StanfordBunnySubsurface`, `StanfordDragonSubsurface`, `AnisotropyBarnLamp` and `SheenCloth`. Moana is deferred and has no current catalog level. These remain development/source workloads; no feature is marked Removed. The inspected catalog contains 20 levels and 17 asset packs.

## Product And Supported Matrix

<details>
<summary>Exact machine, API and source-build requirements</summary>

| Field | Selected machine / proposed boundary |
| --- | --- |
| Executable/profile | `bin/ShowcaseRuntime.exe`, Windows x64 `ShippingGame` |
| Observed OS | Windows 11 Home 25H2, build `26200.9457`; proposed target starts at Windows 11 x64 24H2/build 26100 |
| `MIN-01` / `REF-01` | Same AMD Ryzen 9 8940HX laptop, 16 cores/32 logical processors; 67,833,212,928 bytes OS-reported RAM; NVIDIA GeForce RTX 5070 Ti Laptop GPU, 12,227 MiB reported VRAM; driver `610.47` |
| Storage | Patriot P400L 2000GB NVMe SSD, NTFS; at least 16 GiB free for extraction/staging/user data |
| D3D12 | Default; feature level 12_1, SM 6.6, DXR 1.1. Reject missing required features before rendering. |
| Vulkan | Explicitly selectable; Vulkan 1.3 plus the [required feature set](../FirstRelease.md#frozen-platform-and-toolchain-prerequisites). Separate native/provider/performance proof required. |
| Source build | Exact [toolchain](../FirstRelease.md#frozen-platform-and-toolchain-prerequisites): VS Community 2026 18.7.0/MSVC 14.51.36231, Windows SDK 10.0.26100.0, CMake 4.3.3, Git for Windows 2.54.0, Qt 6.11.1 `msvc2022_64`, Vulkan SDK 1.4.350.0/Slang 2026.8, DXC 1.9.0.5347 |

This is an initial named-machine claim. Generic 8 GiB GPU/16 GiB PC floors and other hardware/drivers require qualification before advertisement. Two measurement roles on one machine do not count as independent adoption. HDR additionally requires a qualified HDR10 display/OS configuration; none is recorded. No hardware purchase is authorized. Debug/Development Editor/Game and Launcher are source products; alternate toolchains remain unqualified.

</details>

## Baseline And Selection

Three presets have distinct purposes: ordinary viewing, realtime path tracing, and progressive reference output. Their implementation and numerical qualification remain delivery work.

<details>
<summary>Exact presets and consumer controls</summary>

- **`REL-NATIVE-1080P`:** 1920x1080 SDR Lit, raster GBuffer, native extent/Linear provider, Ray Reconstruction Off, classic TLAS/refit On, PTLAS Off, auto batching On, Automatic Histogram exposure, ACES approximation, automatic SDR encoding. Preserve declared direct/indirect diffuse/specular, direct subsurface and shadow defaults. Consumer VSync On; performance VSync Off. Grading neutral, chromatic aberration/vignette Off, no baseline decal actors; these remain Included selectable features. HDR10 opt-in after qualification.
- **`REL-RT-1080P`:** 1920x1080 output, traced primary/secondary transport, classic TLAS, DLSS Quality requested, Ray Reconstruction On; fixed SDK-derived render extent/composition before measurement; Frame Generation and dynamic resolution Off. Freeze depth/reuse/approximation in the existing feature discoveries, keeping one final image. Native/RR-Off controls measure cost/quality. Ray GBuffer alone or Linear/Off fallback cannot satisfy this preset; current Lit lighting is only the starting implementation path.
- **`REL-REFERENCE`:** progressive admitted surface transport in the consumer viewport; sample/convergence/invalid state, reset/cancel and reproducible raw-linear export. No DLSS, reconstruction, firefly clamping or artistic post effects in oracle output. Full admitted reflection is required; finite-depth output is diagnostic. Freeze animation pose and reset/reject contributing scene/camera/material/light changes. [Reference semantics](../../Architecture/Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/TransportAndEstimator.md#claim-boundary) owns the domain and two distinct products.

The source snapshot differs: `DefaultEngine.ini` requests VSync Off, ACES filmic, DLSS/NativeAA, reconstruction On and PTLAS preference On; `LevelRegistry::EnsureDefaultLevel` selects Empty. The consumer-shell phase (`PD-1`) must implement the selected startup behavior and safe unavailable-provider handling.

| Consumer route | Proposed behavior / owner |
| --- | --- |
| API | `--graphics-api D3D12` or `--graphics-api Vulkan`; precedence: build default, then `SPARKLE_RHI_BACKEND`, then CLI. Invalid token fails visibly. Application graphics launch owns it. Existing `--renderer`/`--rhi` are development conveniences. |
| Startup/examples | No arguments opens Modern Sponza at its authored camera; Examples menu lists the seven qualified maps. Switching shows progress/failure and returns to the previous valid level. Application host and GameFramework session own this; Empty cannot masquerade as package success. |
| Settings | Resolution/window mode, VSync, tracing preset and Included render/post controls; requested/active status, Save/Reset, restart indication. Application persistence and Renderer features retain authority. |
| Help/quit | F1 help, Escape menu, explicit Quit and native close. Escape releases mouse-look before opening the menu. |
| Camera | W/S or Up/Down forward/back; A/D or Left/Right strafe; E/Space up; Q/C down; Shift sprint; hold right mouse to look; wheel adjusts speed. Discoverable help, sensitivity and authored-camera reset required. [Input collector](../../../Engine/Application/Private/Input/CameraInputIntentCollector.cpp) and GameFramework camera controller own movement. |

Generic console/CVar assignment, startup-map environment selection, capture flags, shader recook, debug views and authoring actions remain source-only and absent from the consumer UI and shipped runtime payload. Consumer display/input limits are decision 3; readable labels, scalable text, keyboard navigation and visible errors remain required basics. Native refresh/VSync must be reported honestly.

</details>

## Mutable Data And Compatibility

<details>
<summary>Exact settings, update, rollback and removal promises</summary>

The [path/budget contract](../FirstRelease.md#frozen-paths-mutation-boundary-and-budgets) owns immutable package bytes and `%LOCALAPPDATA%\SparkleEngine\Showcase\v0.1\` user data: separate Settings, Logs, Captures, Crashes and Cache folders. A standard user can run from read-only extraction; no repository, SDK or writable package root is required.

Published tag, manifest schema identity, archive/checksums and cooked-generation hashes are immutable per release. The seven map IDs above remain the `v0.1.x` public IDs. Schema/bytes change only under a new release identity.

Documented Included settings/controls remain interpretable within `v0.1.x`; invalid values visibly reset safely, optional keys may be added, and development keys are unstable. Cache is generation/version-qualified and disposable; clearing it preserves settings. No save-game/migration/account-state promise. Source APIs may change across `0.y.z`; no binary ABI/plugin SDK. Patches cannot rename documented controls or silently change accepted map semantics. `--graphics-api` and the documented backend environment selection retain their values/precedence within `v0.1.x`; other development flags remain unstable.

Update by downloading, verifying and extracting a new directory. Back up Settings before a patch; roll back by launching the previous verified extraction and restoring its settings backup. Patch versions share `v0.1` settings; different minor versions have separate roots. Concurrent versions writing shared settings are unsupported. Reset through the product or remove only its settings file; clear optional data folders separately. Delete the extraction to uninstall; deleting user data is a separate opt-in action. No automatic update or destructive migration is proposed here.

</details>

## Budgets And Observation

Keep the existing [operation limits](../FirstRelease.md#frozen-paths-mutation-boundary-and-budgets): first UI <=15 s, first example <=30 s, other map <=60 s, quit <=5 s, archive <=4 GiB, extracted <=8 GiB, process peak commit <=12 GiB, tracked local GPU peak <=7 GiB, no unbounded growth. Source/Launcher deadlines remain in that owner.

Native and realtime presets must each satisfy the [30 FPS contract](../FirstRelease.md#thirty-fps-performance-floor) on all seven maps and both APIs: ShippingGame, VSync Off, >=300 warmup and >=300 valid sampled frames, three repeats; presented/app, CPU and GPU p95 each <=33.33 ms. Use AC power, fixed Windows/OEM performance mode and steady thermals. Record p99, 1% low, worst frames, memory/clocks, map/cooked generation, authored camera/animation time, render/output extents, settings, driver and backend. No capture/validation/profiler in timing runs, generated frames, dynamic resolution, fallback or post-result retuning. Native uses no DLSS; realtime uses its frozen Quality/reconstruction topology. Modern Sponza's detailed motion/disocclusion workload is frozen by `MAP-00`. Reference has convergence/cancellation/resource budgets rather than a 30 FPS promise. [Principal frame review](../FirstRelease.md#principal-frame-review) uses separate captures and observer-controlled timings.

## Network, Security, Publication And Support

Decisions 6 and 7 state the proposed public promises. Publication still requires the [existing trust/approval rules](../FirstRelease.md#approval-rules): trusted Authenticode on every shipped PE, SHA256SUMS, manifest, dependency/content notices, software bill of materials, build provenance and vulnerability/malware disposition. Keys/accounts stay outside source/archives; unsigned builds remain developer prereleases.

Source acquisition uses public HTTPS inputs with pinned revisions/hashes and no private credentials. Runtime remains offline. Support links open only on user action. Logs/captures/crashes may contain paths, scene content or memory; explain/redact voluntary reports, and share dumps/private CPU/shader symbols only privately with consent. A separate symbols archive follows the same privacy boundary.

Publish new immutable patches and rerun affected gates; retain version hashes/notices/known issues/advisories. Withdraw unsafe payloads with a visible advisory and verified rollback instructions. Never silently replace a published signed archive.

Excluded product promises: general NVIDIA/AMD/Intel-wide support, neural training/inference platform, multiplayer/networking, mods, stable SDK, installer/updater, Linux/macOS, mobile and VR. No additional scene families or AMD specialist profiler work. PTLAS and source-only external capture remain experimental within their declared vendor/debug limits.

## FR-00 Review Package

The [reconciliation and iteration evidence](Evidence.md#fr-00-review-package) retains the source identities, license observations and historical checks. The latest audit accounted for 512 capability IDs, 20 levels, 18 view modes and 26 persisted names. Its static checks passed; final scope approval and rights remain **BLOCKED**. Accepted path-tracer development discovery remains preserved; reference release-oracle, native, build, cook, package, performance and adoption results remain unaccepted.
