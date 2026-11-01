# Showcase Product Capability Inventory

[Docs](../../../../README.md) / [Up](../README.md)

**Status:** capability snapshot; current product/catalog view, not evidence that every level is downloaded, cooked, runnable, or releasable

**Snapshot:** 2026-10-04 working tree at base revision `9f1eff12`; catalog, level files, Launcher sync/readiness, source import and cooking inspected; evidence `S` and local tool execution only

**Scope:** shipped project products, startup/selection behavior, cataloged workloads, content provenance/readiness, and their role as capability evidence

**Owner:** `Projects/Showcase`; Engine/Application/Launcher/Cooking own execution infrastructure

**Evidence and disposition:** [Capability Evidence Plan](../../CapabilityEvidencePlan.md) and [First Release Acceptance Contract](../../../../Acceptance/FirstRelease.md)

**Current readiness:** **50/100** for the tracked game product route — project/catalog/source integration exists; content readiness, executable workloads, package, first-use, and release evidence remain open. See [Current Feature Readiness](../../../../Acceptance/CurrentReadiness.md#product-build-and-delivery).

**Advanced evidence delivery:** [Content To Pixel design](ContentToPixel/ContentToPixelDesign.md) and its [CPIX-0..3 plan](ContentToPixel/ImplementationPlan.md) own the missing M1/M2 source/conversion/material/raw-reference proof for Bistro and San Miguel. This future route does not change the dated inventory, first-release map set or current readiness.

## At A Glance

| Product concern | Current state | Evidence boundary |
| --- | --- | --- |
| project products | separate minimal ShowcaseEditor and ShowcaseGame targets | source/build membership, not a successful or packaged launch |
| selection | environment/Launcher selects only registered ready catalog levels; an explicit unknown startup ID reports an error and activates no level | a valid catalog ID and actual first frame still require runtime proof |
| content ladder | compact tracked scenes plus larger downloadable Modern Sponza, Bistro, LPS Head, and Cornell Box routes | selection/download support is not cook/runtime/readiness evidence |
| material previews | Stanford Bunny/Dragon, Anisotropy Barn Lamp, and Sheen Cloth are selected generic-shading previews | launchability does not establish subsurface, anisotropy, clearcoat, transmission, volume, or sheen fidelity |
| acceptance role | levels exercise import, world, Renderer/RHI, diagnostics, performance, and product journeys | one visible level cannot approve the whole feature matrix |

The project should stay thin: it selects content and composes engine/application products, while engine modules and tools retain feature ownership. Scene-specific fixes belong in the owning importer, world, Renderer, or RHI contract unless the behavior is intentionally authored content.

## Product Surface

| ID | Capability | State | Exact current coverage and limit | Evidence |
| --- | --- | --- | --- | --- |
| `SHOW-001` | Project discovery | Implemented path | `.sparkle-project` makes Showcase the current auto-discovered project and Launcher default content. | `S` |
| `SHOW-002` | Editor product | Implemented path | `ShowcaseEditor` is a minimal `main` over `RunEditorApplication`, links ApplicationEditor/GameFramework, stages declared runtime owners and Streamline, and outputs under the editor artifact root. | `S` |
| `SHOW-003` | Game product | Implemented path | `ShowcaseGame` is a minimal `main` over `RunGameApplication`, does not link Editor/ApplicationEditor, and outputs under the game artifact root. | `S` |
| `SHOW-004` | Six-profile build naming | Implemented path | Both products participate in Debug/Development/Shipping profile convention; Launcher resolves the matching target/profile and project working directory. | `S` |
| `SHOW-005` | Startup level selection | Implemented path | `SPARKLE_STARTUP_LEVEL` requests a registered name; an absent value selects `Empty`, while an unknown explicit ID reports an error and activates no level. Only catalog entries marked Selected and currently ready register. | `S` |
| `SHOW-006` | Game level switching | Implemented path | Editor menu/Launcher can choose registered catalog levels; LevelSession cancels an in-flight load when a new request arrives and rejects stale generation results. | `S` |
| `SHOW-007` | Authored level save | Implemented path | Editor Save All writes the active `.level` document through GameFramework. External source assets/cooked products are not authored by this operation. | `S` |

## Level Catalog

The catalog has 20 level records: 17 Selected and 3 not selected. Selection is intent, not readiness; external packs and cooked products still gate registration and launch.

| Level(s) | Selected | Source/readiness class | Intended evidence surface | Important limitation |
| --- | --- | --- | --- | --- |
| `Empty` | Yes | Built-in/authored level; no scene asset | Host startup, window/input/UI, scene reset, no-content baseline | Used by default only when no startup level was explicitly requested. |
| `Sponza` | Yes | Tracked glTF/project textures | General static geometry, textures, PBR lighting | Runtime/cook evidence not produced here. |
| `ABeautifulGame` | Yes | Tracked glTF/project textures | Material variety, composition, static PBR | Exact extension/alpha coverage must be checked. |
| `DamagedHelmet` | Yes | Tracked glTF/project textures | Compact metallic-roughness, normal/AO/emissive material | One asset cannot prove broad material coverage. |
| `DiffuseTransmissionPlant` | Yes | Tracked glTF/project textures | Alpha/transmission-adjacent stress and layered plant source | True transmission/blend rendering is not implemented; the title must not imply it is. |
| `CesiumMan` | Yes | Tracked glTF/project texture | Skeleton, skinning, animation, coordinate normalization | Needs raster/ray deformation equivalence evidence. |
| `ModernSponza`, `ModernSponzaCandles`, `ModernSponzaKnight` | Yes | External packs marked download/game supported, including parent dependencies | Large scene/materials; emissive-instance stress; FBX skeletal animation | Multi-GB acquisition, license verification, cook/runtime and memory evidence remain open. |
| `BistroExterior`, `BistroInteriorWine` | Yes | External Bistro pack marked download/game supported | Large FBX scene, outdoor/indoor lighting/material variants | Import fidelity and memory/performance evidence remain open. |
| `LPSHead` | Yes | External pack marked download/game supported; conversion helper tracked | Skin/head shading | Conversion/provenance and final material correctness remain open. |
| `CornellBox` | Yes | External pack marked download/game supported; conversion helper tracked | Indirect lighting, Reference Path Tracer, and convergence | Conversion and reference oracle must be recorded. |
| `JungleRuins` | No | Download supported, Game unsupported | Future dense USD/out-of-core scene | USD composition and virtualized/out-of-core geometry absent. |
| `SanMiguelHigh`, `SanMiguelLow` | No | Download supported, Game unsupported | Future large-scene tiers | Deterministic OBJ/MTL/PNG-to-glTF conversion absent. |
| `StanfordBunnySubsurface`, `StanfordDragonSubsurface` | Yes | Publisher PLY archives download and cook as generic triangle geometry | Future subsurface material and lighting references | No authored subsurface parameters or lobe; Dragon carries publisher noncommercial-use constraints. |
| `AnisotropyBarnLamp` | Yes | Pinned publisher loose glTF files download and cook as generic metallic-roughness | Future anisotropic brushed-metal reference | Optional anisotropy, clearcoat, transmission, and volume data are explicitly omitted; core textures and emissive strength remain. |
| `SheenCloth` | Yes | Pinned publisher loose glTF files download and cook as generic metallic-roughness | Future cloth-sheen reference | Core texture transforms are translated, but optional sheen data is explicitly omitted and grazing-angle sheen fidelity remains unsupported. |

## Asset-Pack Catalog

The 17 asset-pack records encode root/extraction/required path, parent relation, source URL/page, an archive digest or a tracked per-file acquisition manifest, version, license, and explicit download/game flags/blockers. Loose-file packs verify every pinned byte count and SHA-256 before transactional publication of an acquisition receipt.

- 13 packs are marked DownloadSupported; 11 are also GameSupported. The four new packs are Game-supported only for generic-shading preview, not their named specialty materials.
- Unsupported families are explicit: Modern Sponza Ivy (density/residency/scaling), Trees (alpha foliage/transparency), Flood (Alembic/water/sequence), Explosion (OpenVDB/volume/streaming), Jungle Ruins (USD/out-of-core), and San Miguel Game conversion. Stanford subsurface, Barn Lamp advanced lobes, and Sheen Cloth sheen are tracked as unsupported shading even though their generic previews are selectable.
- External archive license strings are metadata, not proof that redistribution rights and notices are complete.

## Workload-To-Capability Coverage

| Capability slice | Best current Showcase source | What still must be observed |
| --- | --- | --- |
| No-content lifecycle | Empty | startup, minimize/restore, settings, clean exit, no silent content error |
| Compact static PBR | DamagedHelmet | all material channels in raster and ray GBuffer |
| General static scene | Sponza / ABeautifulGame | import/cook/load, variants/alpha boundaries, backend parity |
| Skeletal animation | CesiumMan / ModernSponzaKnight | playback, skinning, motion vectors, BLAS/ray identity |
| Instancing/emissive scale | ModernSponzaCandles | import grouping, draw savings, emissive contribution, memory/frame time |
| Indirect/reference | CornellBox | controlled convergence and ReSTIR/reference comparison |
| Large scene | Bistro / ModernSponza | bounded import/cook/load/residency, stable switch/reload/exit |
| Unsupported-content honesty | Plant, Jungle, San Miguel, future add-ons | blocked/unselected presentation and absence of misleading success |
| Subsurface references | Stanford Bunny / Stanford Dragon | generic PLY geometry cook exists; authored material parameters, direct/indirect component response, and reference captures remain |
| Anisotropy reference | Anisotropy Barn Lamp | generic core-material cook exists; extension import, tangent/material preservation, directional highlight response, and enabled/disabled reference captures remain |
| Cloth sheen reference | Sheen Cloth | generic core-material cook and UV0 texture-transform translation exist; sheen extension import, grazing-angle lobe response, and enabled/disabled reference captures remain |

## Vertical Launch Trace

Launcher loads `Levels.catalog` -> user selects scene/product/API -> `levels.run` checks its level definition, executable and cooked outputs -> starts the product in its project directory -> GameFramework loads the requested identity or reports failure -> Renderer consumes the scene. Raw source assets and acquisition selection do not gate Run. Optional Sync/Build/Cook explicitly replace those same outputs before another Run.

## Game Presentation

Game presents rendering with existing camera/window/focus controls and native close. The Game-only ImGui menu/console host and flags are deleted; there is no HUD, help/settings/loading overlay or Game Reference export UI. Launcher supplies scene/API selection and launch feedback. Editor keeps inspection, editing, settings and Reference controls/export. [Application](../../Engine/Application/ApplicationCapabilities.md#render-only-game) owns composition; [source checks](../../../../../artifacts/validation/release-owner-slim-20261101/completion.json) and future affected runtime proof remain distinct. Earlier PD-1/PD-3 menu receipts keep their original candidates.

## `FCR-PROD-01` Game Consumer Contract

This is the Architecture acceptance owner for the Showcase consumer journey. [Application](../../Engine/Application/ApplicationCapabilities.md#product-contract-boundary) owns host construction and settlement; the [frozen product contract](../../../../Acceptance/FirstRelease.md#frozen-product-and-output-classification) owns release values; [release gates](../../../../Acceptance/FirstRelease.md#release-gates) are linked, not repeated.

**Proof allocation:** [PD-1](../../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) closes source-workspace portions; PD-3 closes installed portions and PD-4 reconciles the same archive/rehearsal evidence. These are portions of the criteria below, not separate acceptance definitions. Results stay in [Product delivery progress](../../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress).

| ID | Binary acceptance criterion | Proof phase |
| --- | --- | --- |
| `AC-PROD01-01` | From the ready-to-run bundle as a standard user, Launcher starts Game in intentional Empty or the selected supplied prebuilt map within the startup budget, without a repository, source assets, cook, toolchain, old content, network, environment override or manual preparation. Record exact product/package/requested identity through existing logs/events; no setup menu is required. | Source startup; current-archive startup |
| `AC-PROD01-02` | Selecting each included example publishes the requested catalog identity and a frame with the expected scene/camera and geometry/material bindings within its load budget. Crashes, blank frames, wrong/missing content and false built-in `Empty` activation fail; intentional Empty selection is valid. Lighting quality remains in renderer/release gates. | PD-1 source maps/failures; PD-3 archive equivalents |
| `AC-PROD01-03` | Game renders without ImGui/UI; existing camera/focus/capture and native close/Alt+F4 work. Help and optional settings inspection/reset are available outside Game through Launcher, Editor and instructions. Editor persists shared settings consumed by both products; ordinary Game Run needs no configuration. | Game presentation/input; Editor shared settings; installed use |
| `AC-PROD01-04` | `ShippingGame` contains and exposes no Game ImGui host/menu/HUD/overlay, console, CVar authoring route, shader recook, editor panel, source importer/cooker, repository marker dependency or developer-only command. Shared Editor UI remains available in Editor. | Source/link checks; package membership |
| `AC-PROD01-05` | Close/quit during idle and level load settles level/task/Renderer/RHI work within the shutdown budget, returns one success/failure exit status, and leaves immutable package/source bytes unchanged. | PD-1 source lifecycle; PD-3 archive lifecycle |

| ID | Cause/injection, detection boundary, safe result, and recovery |
| --- | --- |
| `FM-PROD01-01` | Remove all `SPARKLE_*` overrides and launch from an arbitrary working directory. Package/Application discovery must still resolve the product; any environment or repository dependency blocks `AC-PROD01-01`. |
| `FM-PROD01-02` | Remove or corrupt a copied included level/product. LevelSession keeps the last accepted state or stops and records the failed identity through existing launch/log/native failure paths; retry is a relaunch through Launcher. Silent `Empty` activation fails `AC-PROD01-02`; no Game failure UI is required. |
| `FM-PROD01-03` | Press former menu/help/console shortcuts and inspect `ShippingGame` membership. Any Game ImGui, console or authoring surface fails `AC-PROD01-04`; native window close/Alt+F4 must remain available. |
| `FM-PROD01-04` | Truncate settings or deny their writable root, then reset through Editor or the documented file-reset route. Reject/quarantine invalid state, name the failed path and preserve package/repository bytes; no Game settings frontend is required. |
| `FM-PROD01-05` | Quit during load and repeat launch/exit. New work stops, in-flight work settles/cancels, and no process or partial user file remains after the budget; hang/leak fails `AC-PROD01-05`. |

| Check | Claims falsified | Smallest route and fixed oracle |
| --- | --- | --- |
| `CHK-PROD01-01` | `AC-PROD01-04`; `FM-PROD01-03` | Build the exact `ShippingGame` product when authorized, inspect link/import/file/string membership, then launch and exercise the console shortcut. Zero developer surface is the oracle; any match or overlay is failure. |
| `CHK-PROD01-02` | `AC-PROD01-01`, `AC-PROD01-03`, `AC-PROD01-05`; `FM-PROD01-01`, `FM-PROD01-04`, `FM-PROD01-05` | On the admitted standard-user machine, extract the candidate, launch with overrides/network absent and no source/old content, verify rendered Game without UI, camera/focus/capture/native close, and retained Editor controls/settings/reset with shared Game consumption. Compare mutations, processes and budgets against the frozen contract. |
| `CHK-PROD01-03` | `AC-PROD01-02`; `FM-PROD01-02` | For every included map, predeclare its expected scene/camera and geometry/material bindings, then capture requested/active identity and presented output. Identity alone cannot pass a blank frame or wrong/missing content. Inject missing/corrupt products on isolated copies and verify distinct non-`Empty` failure/recovery. Lighting quality is checked by the renderer/release gates. Source portions run in PD-1, archive counterparts in PD-3. |

Before a map check, freeze the source/cook/shader, camera and settings identities and its expected image from the owning map/Renderer contract. Record activation and first displayed frame separately: identity/readiness alone cannot pass image correctness. Full reference/convergence, lighting-quality and 30 FPS proof stays with the corresponding feature/release gates; a known blocking render defect still prevents accepting that map.

Every criterion and failure above maps to at least one check. Candidate results belong in `FCR-PROD-01`; this source/documentation inspection does not pass any row.

## Explicit Non-Capabilities And Risks

- Showcase is an evidence application, not a general game: no game rules, audio, physics, networking, save data, installer, or end-user onboarding loop exists.
- A catalog description is product intent, not implementation proof. In particular, “Diffuse Transmission” and future volume/water/foliage descriptions exceed current Renderer capability.
- Stanford, Barn Lamp, and Sheen Cloth are selectable generic previews. Their source/texture cooks passed locally in this working tree, but no first-frame, GBuffer capture, specialty-lobe, or release proof is recorded here.
- Catalog Selected does not guarantee source pack presence, successful cook, registered game level, or release disposition.
- This pass locally built SourceImporters, AssetCooker, ShowcaseEditor, SparkleLauncher, and ShaderCompiler and cooked Showcase source scenes, textures, and shaders. All 20 authored level names match their catalog IDs. A stale Showcase shader map initially caused startup contract failure for `Empty`, `Sponza`, and the new levels; a shader cook from the repository root published to `Shared`, not `Showcase`. After rebuilding ShaderCompiler and cooking from the Showcase working directory, D3D12 editor smoke runs activated `Sponza` and each of the four new levels and closed normally. These runs verify requested level activation and basic process lifetime, not correct first frame, GBuffer channel values, specialty shading, timing, Vulkan parity, or packaged-byte behavior.
