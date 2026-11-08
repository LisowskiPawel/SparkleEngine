# First Release Acceptance Contract

[Docs](../README.md) / [Up](README.md)

Use this contract to decide whether exact release bytes may ship. Make product choices in [Release Scope](#approval-and-next-work); choose implementation work in [Roadmap](../Strategy/Roadmap.md#where-we-are-and-what-is-next).

**Status:** acceptance contract for the first public engine release; requirements are not completion claims

**Release target:** SparkleEngine `v0.1.0`, Windows x64

See [Roadmap progress and next work](../Strategy/Roadmap.md#release-progress). This page owns the checks required to ship; it does not repeat the work log.

Read [approved scope](#approved-release-scope), [shipping gates](#release-gates), or [public rights and trust](#public-rights-and-trust). Progress and next work stay in the Roadmap.

## Public Rights And Trust

Internal FR-01 is complete for its recorded candidate. The remaining public obligations below belong to FR-07 and keep formal `REL-01` acceptance open; they do not reopen the internal source stage.

- Historical public asset rights remain unresolved; acquisition recipes do not qualify old repository bytes.
- Exact Qt/NVIDIA/helper/CRT redistribution and symbol admission remain open. [Input contract](../Architecture/Modules/BuildAndPackaging/PackagingAndInstallation.md#release-inputs).
- [FR-01 source requalification](#fr-01-source-requalification) is complete for the recorded internal source candidate: empty-cache acquisition/build/cook/Empty and exact-product warm/offline replay pass. The public obligations above remain final FR-07 work; historical PD-2 passes retain their original identities.

Certificate enrollment is deferred until before public release. It does not prevent unsigned internal package development. [Roadmap](../Strategy/Roadmap.md#where-we-are-and-what-is-next).

### FR-01 Source Requalification

**`ITER-FR01-COLD-RECOVERY-20261031` — PASS, remaining internal source work closed.** [Result and control record](../../artifacts/validation/releases/0.1.0/fr01-cold-recovery-20261031/completion.json). The fresh isolated source has all 2,834 input hashes of `accba4ea654341270e218075db1874870436ed76`, fingerprint `41906b4d…`. [Candidate identity](../../artifacts/validation/releases/0.1.0/fr01-cold-recovery-20261031/candidate.json) retains the earlier base/patch and scope export. The private commit is not a public tag or product archive; concurrent shared-worktree changes retain their own identity.

| Cell | Recorded result / remaining boundary |
| --- | --- |
| Identity, pins, notices and content inputs | Reuse the unchanged [input audit](../../artifacts/validation/releases/0.1.0/655f5fe8-fr01-20261031/static-validation.json). The [fresh cold cache](../../artifacts/validation/releases/0.1.0/fr01-cold-recovery-20261031/cold-inputs.json) passes all 13 Git pins and both archive hashes. [Actual payload provenance](../../artifacts/validation/releases/0.1.0/fr01-cold-recovery-20261031/current-payload-provenance.json) binds 82 new PEs and unchanged private notice inputs: 21 exact receipts and the separately hashed first-party license newline variant. Three historical URL receipts remain unrefreshed. Final notice-byte binding and public rights stay with FR-07. |
| REL-00 scope export | The [reviewed `release-scope.md`](../../artifacts/validation/releases/0.1.0/655f5fe8-fr01-20261031/release-scope.md) is exported and hash-bound by [candidate identity](../../artifacts/validation/releases/0.1.0/655f5fe8-fr01-20261031/candidate.json). All six choices and accepted PTD-00 are preserved; final candidate freeze rechecks changed dispositions and identity. |
| Architecture / source style / shader-compiler owners | Architecture passes. Two long RHI calls required formatting only; [unchanged-token/layout review](../../artifacts/validation/releases/0.1.0/655f5fe8-fr01-20261031/formatter-repair.json), focused check and full 2,315-file style recheck pass with clang-format/clang-tidy 22.1.3. [ShaderCompilerCliValidation](../../artifacts/validation/releases/0.1.0/655f5fe8-fr01-20261031/shader-cli-retry.json) passes paired DXIL/SPIR-V cooking, deterministic replay and cancellation/traversal/unknown-shader controls. The original interrupted baseline invocation remains inconclusive; its failed style observation is retained with the repair. |
| Empty-cache source acquisition / build / cook / Empty | PASS: [cold route](../../artifacts/validation/releases/0.1.0/fr01-cold-recovery-20261031/cold-source-result.json) starts with empty dependency/build/cook caches and supplies none. Public sync/configure, ShippingGame, three helpers and `cook.all` return native exit 0; Empty is visually confirmed and closes normally. Sync takes 251.457 s; the route through reviewed launch takes 1,573.199 s. Earlier Assimp/Streamline/PIX transport failures remain in their [original report](../../artifacts/validation/releases/0.1.0/655f5fe8-fr01-20261031/completion.json); fresh success supersedes their blocking status without changing pins, adding retries or relabelling cached proof. |
| Exact-product warm offline replay | PASS: [offline replay](../../artifacts/validation/releases/0.1.0/fr01-cold-recovery-20261031/offline-source-result.json) reuses only this cold cache and passes the same sync/build/cook/Empty route in 64.651 s. No adapter is active during the boundary probes, HTTPS fails before/after replay, then the original Wi-Fi/HTTPS connection is restored. All 99 checked product/cooked/symbol files retain their hashes; native Game/Launcher exit 0. Compatible [missing-Qt/dirty-input controls](../../artifacts/validation/releases/0.1.0/655f5fe8-fr01-20261031/source-negative-controls.json) are reused for unchanged owners. Seven-map, installed and feature-quality acceptance remain with their existing gates. |
| Public rights and trust | BLOCKED for formal public release: historical source/content rights, exact Qt/NVIDIA/helper/CRT redistribution, corresponding-source/notice and applicable mark obligations, final payload provenance and trusted signing remain FR-07 / `REL-01` review. This source iteration grants no public redistribution or publication approval. |

**Budget and preservation:** recovery needs no production change or new dependency manager, baseline runner or test framework. The earlier two-call formatter repair retains its mechanical/authored Stage Source-Style PASS; this evidence/documentation continuation has no added or modified owned production source. All six choices, seven examples plus Empty, approved defaults, display-selected resolution and accepted PTD-00 remain intact. Original PD-2/package binaries are not relabelled.

The [resource observation](../../artifacts/validation/releases/0.1.0/fr01-cold-recovery-20261031/source-resource-observations.json) records 27.761 GiB maximum sampled system commit across cold/warm work and 11.223 GiB owned storage at completion, within the source budgets. These are sampled/completion observations, not instantaneous peaks, first-interactive-frame latency or release-performance acceptance. [Cleanup](../../artifacts/validation/releases/0.1.0/fr01-cold-recovery-20261031/cleanup.json) removes temporary probe/observer source, stops the sampler and retains logs, products and symbols. Automatic approval review again rejected generated-checkout removal as “blocked by policy”; old/new validation roots and their isolated state are retained. This is recorded local artifact retention, not a passed removal check. Unrelated state/edits are preserved.

**`ITER-FR01-CLOSEOUT-20261108` — PASS, internal-stage planning closeout (2026-11-08).** The [closeout record](../../artifacts/validation/releases/0.1.0/fr01-closeout-20261108/completion.json) binds this documentation iteration to `23eab7bdb` and preserves concurrent documentation changes. The [input audit](../../artifacts/validation/releases/0.1.0/fr01-closeout-20261108/input-audit.json) rechecks all 2,834 retained source hashes, the exact scope export and 11 reused evidence receipts. Current identity/license/notice content, the level catalog and all 13 declared Git pins are preserved; line-ending differences retain distinct byte hashes. Subsequent source/build/publication changes are recorded as changed inputs, without promoting the old cold/warm/runtime pass to current source. The completed FR-01 roadmap entries are retired; no production change or new executable qualification is claimed.

**Remaining / next:** no internal FR-01 work remains. FR-06 owns affected source acquisition/build/cook/Empty and warm/offline requalification against the replacement candidate, together with its archive/product/symbol checks. Final notice-byte binding, certificate/signature execution and exact public-rights/provenance closure remain FR-07. Those later obligations do not keep FR-01 open. The global next item follows [Roadmap](../Strategy/Roadmap.md#where-we-are-and-what-is-next); retained local validation roots remain listed in the cleanup record.

## Product Plan And Public Release

[PD-1 through PD-4 reports](FeatureCompletionReports.md#product-delivery-progress) retain five admitted technical passes and completed PD-4 controls for one **unsigned internal** archive; aggregate product acceptance remains blocked by the known scene-publication defect. Source-only checks run before an archive exists; installed checks consume the archive. Author/agent rehearsal is accepted under the [actor scope](FeatureCompletionReports.md#report-closure-and-review).

**Accepted internal rehearsal:** use the existing `MIN-01`/`REF-01` laptop with an isolated source checkout, fresh owned extraction/content state and a standard-user process. This applies the owner's existing-laptop decision to the product contracts' clean/minimum-machine checks. Record the installed toolchain, remove discovery overrides and verify dependency independence, offline operation, writable paths and budgets. A fresh Windows installation is outside this internal rehearsal; broader public-host qualification remains in the release gates.

**Product boundary, approved 2026-10-25:** PD-1..4 prove requested map/content identity, expected scene/camera and geometry/material bindings, presented output, controls, failure and lifecycle. Lighting quality, including Modern Sponza noise/clipping, is deferred to renderer/map/release gates. Crashes, wrong/missing content, blank frames and false Empty activation still block the product plan. Defaults remain unchanged; this deferral does not qualify lighting or approve publication.

All six reports must explicitly pass their admitted scope before product acceptance. The owner has retired the product implementation document while retaining unresolved acceptance in the mother plan; document retirement does not waive that conjunction. Public-release qualification remains open. Certificate issuance, signing and public redistribution stay listed as pending FR-07 obligations, not failed product-stage prerequisites or exclusions. Signing changes byte identities; final integrity/trust and affected consumer checks must run again before public release. Package input identity, internal integrity and truthful failure behavior are not deferred.

Broader feature/reference/performance/native acceptance remains under REL-04 through REL-07; recorded candidate review, publication and closeout remain REL-08 through REL-11. Reuse valid product evidence there without treating this implementation plan as whole-release approval. [Product delivery progress](FeatureCompletionReports.md#product-delivery-progress) records completed checks and remaining work.

**Accepted PD-3 closure, 2026-10-31:** the scope owner's instruction, "consider this is done lets close this stage", accepts unsigned archive `a0f34c8b...` with only actual selected-extraction removal [EXCLUDED](../../artifacts/validation/releases/0.1.0/241ef037-product-20261025/pd3-repaired-20261031/accepted-close-20261031/scope-decision.json). `FCR-PROD-05`/`REL-03` pass that admitted internal scope. The verified folder remains retained; no deletion or physical-removal pass is claimed. Existing technical proof and unchanged user-state/archive/symbol inventories are preserved. This candidate-specific exception does not alter the broader release contract.

**FR-02 final recheck, 2026-10-31 — PASS:** [verification record](../../artifacts/validation/releases/0.1.0/241ef037-product-20261025/fr02-final-20261031/completion.json) uses the unchanged package owner to verify all 434 archive members, retained extraction/layout/signatures, SBOM/provenance and exact native/shader companion identities. No invalidated admitted input, stale package member or internal FR-02 check remains. Existing build/cook/installed proof is reused; the archive is not regenerated or replaced. Current unrelated checkout edits retain their own identity. PD-4 technical replay is complete, but its known coherence defect blocks closeout; final FR-07 public qualification remains separate.

**PD-4 reconciliation, 2026-10-31 - BLOCKED:** [six-report result](../../artifacts/validation/releases/0.1.0/a0f34c8b-pd4-20261031/six-report-reconciliation.json) retains five technical report passes and completed native/export/version-replacement controls for `a0f34c8b...`, fingerprint `94ea02c2...`. Original user-state inventories are restored exactly. Concurrent FWC-4 interruption proof has 12 identical producer/reader source inputs and establishes applicable unresolved `S1` / `ISSUE-3`; the existing release owner blocks actual closeout. The owner retired the product implementation document and later resumed FWC-4. Its [recorded source publication repair](FeatureCompletionReports.md#foundation-contract-progress) passes for that source identity. The simplified publication implementation passes its affected source/native controls; a new candidate/symbol companion and affected reconciliation remain required. Selected content proof follows FR-04 under the [portfolio demo boundary](#portfolio-demo-delivery-boundary). Unresolved acceptance remains in the mother plan; document removal and the earlier package-removal exclusion do not waive this defect.

## Approved Release Scope

This page defines the first release: three products, seven selectable maps, user-selected display resolution, a 30 FPS acceptance target on your laptop, and no user support service.

Delivery is bounded by these promises and [Roadmap scope protection](../Strategy/Roadmap.md#scope-protection). Inventory coverage does not require finishing every conventional engine capability, implementing every target in an Architecture dossier, or building internal tooling to produce each check. Preserve the six approved choices below; propose any further reduction in this scope owner before changing a required result. Evidence and safety thresholds remain binding for the scope retained.

[Roadmap](../Strategy/Roadmap.md#release-progress) shows progress. [Capability Dispositions](ReleaseScope/CapabilityDispositions.md) owns feature classifications; [First Release](#release-gates) owns shipping checks.

### Portfolio Demo Delivery Boundary

**Scope-owner direction, 2026-10-31:** deliver a reliable rendering portfolio demo and cut work without a selected rendering or reproduction consumer. The six choices below, seven maps plus Empty, required rendering, both APIs, rights and publication gates remain binding. Accepted PTD-00 is preserved.

- Retire the separate foundation execution plan. Its completed source slices retain their exact [results](FeatureCompletionReports.md#foundation-contract-progress); remaining checks follow the rendering consumer, rather than an independent subsystem-completeness sequence.
- [FR-04](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-04--release-maps) owns source/cooked fidelity for the semantics reached by the selected maps, poses and animations. Required decoder, dependency, determinism and budget checks apply to those inputs and changed shared producers. Unused format/semantic combinations are outside this release proof; they receive no whole-family acceptance. [FR-06](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-06--candidate-freeze-and-reproduction) joins existing candidate reports and proves package/adopter coherence; no separate foundation closure journey is required.
- File-checkpoint loading and resuming a Reference session from disk are **Excluded** from this release. PTD-00's optional checkpoint does not require a new reader, restore UI or migration protocol. Retain in-memory pause/resume, exact raw/statistical output and checks of any emitted snapshot; do not advertise file restore. This leaves the admitted progressive reference and numerical/raw export obligations intact.
- Reuse the reviewed, pinned external rendering precedent. Revisit affected correspondence only when its admitted source, implementation or claim changes; discovering new upstream capabilities at packaging time does not expand this release.

This is scope reconciliation, not a technical pass. Known rendering defects, interrupted publication, affected current-source checks, necessary cleanup and repaired-archive requalification still block their owning gates. Broader unqualified capabilities remain unqualified.

**Reconciliation result:** the duplicate foundation plan and prompts are removed, incoming routes are repaired, and remaining work follows FR-03 rendering recovery, FR-04 selected-map proof and FR-06 candidate reconciliation. The [changelist review and documentation checks](../../artifacts/validation/rendering-demo-scope-20261031/completion.json) record the cuts and retained rendering dependencies. This documentation-only slice adds no production code and grants no runtime or release pass. The [all-plan reconciliation](../../artifacts/validation/rendering-portfolio-plan-20261101/completion.json) removes deferred infrastructure prompts, new capture integrations and per-feature delivery campaigns; one shared candidate qualification preserves the required rendering and public-release evidence.

#### Minimum Supporting Scope

The release deliverable is the rendered demonstration and its reproducible source, captures and measurements. Keep the existing Launcher, Editor and Game as the ways to obtain, inspect and run it. The table bounds new work; it does not delete qualified behavior or waive a reachable defect.

| Area | Enough for this portfolio release | Work this release does not require |
| --- | --- | --- |
| Renderer, RHI and shaders | Correct selected rendering, trustworthy raw/reference output, D3D12/Vulkan lifetime and ABI safety, image quality and measured cost. | Unselected future rendering tiers or every optional backend/provider combination. Required rendering depth keeps its existing owner and order. |
| Showcase, Game and world | Seven approved maps plus Empty; required camera, controls, poses/animation and correct cooked-to-frame data. | A gameplay SDK, arbitrary ECS/plugin extension system, physics/networking/audio/scripting program or general scene editor. |
| Editor and Launcher | Existing inspection/render controls, qualified edits/undo/save, map selection/sync, build/cook/run, progress/cancel and actionable failure. | An asset-authoring suite, second workflow engine, installer, updater, account system or additional release settings UI. |
| Core, Platform, Tasks, import and cooking | Existing paths, settings, logs, input, safe task/load lifetime and deterministic products needed by those journeys. Reject incomplete generated output; clean and recook disposable data. | General library completeness, an asset database, resumable cook journal, cache migration/versioning, scheduler rewrite or exhaustive unused-format certification. |
| Package and source entry | Portable three-product release with a complete admitted cooked base, matching symbols/hashes/notices/signing and optional Launcher source rebuild from matching precompiled/cooked outputs. | A release service, installer/updater, package manager, local-file version database, binary SDK or second build/cook owner. |
| Diagnostics and maintenance | Existing inspectable local logs, available native GPU diagnostics, truthful unavailable categories, manual reset/re-extract, known issues and one finite maintainer review. | A crash-collection platform, telemetry/uploads, intake/response service, operational dashboard or general incident-management framework. |

Supporting work enters the plan only for a named rendering or documented reviewer-run promise, a demonstrated failure, or a bounded deletion preserving that promise. First use the current owner and existing tools. New persistent state, public APIs, settings or services need a concrete invariant that the existing path cannot satisfy. Once the promised journey and its affected positive/failure checks pass, stop that slice and return to the rendering consumer; report aggregation is not permission to reopen the module.

#### Minimum Release Workflow

For each new release, start with empty owned build/cooked/stage outputs, build and fully cook the selected inputs, generate one final inventory and verify/package the result. Supply the matching precompiled products and cooked base with that release. Run the prebuilt and optional source-rebuild journeys, then sign/publish and verify downloaded bytes. Keep exact archive/source/symbol identity and the approved rights, privacy and data-preservation boundaries. Final signing changes bytes and needs its affected checks; it does not require replaying unchanged source or rendering results.

Manual extraction/re-extraction, manual side-by-side replacement and clean/recook are sufficient recovery mechanisms. Use existing package scripts and ordinary filesystem/GitHub release operations. Reuse compatible onboarding, export, fault, replacement and rollback receipts; execute affected checks on changed candidates and the actual replacement path when shipping a replacement. No new updater, recovery protocol, support service or synthetic release series is needed merely to complete reports. Preserve all required AC/FM oracles for the admitted promises.

#### Shared Delivery Evidence

Qualify one final candidate through the existing product, source-build, package and native-tool routes. Feature plans contribute their rendering-specific controls, numerical/native results and changed-dependency checks to those same runs. A feature's package/adoption stage does not require its own archive, synthetic version, fresh-machine campaign, public CLI, support workflow or release tool. Share observations only where candidate inputs and the actual exercised oracle match; changed bytes and missing required checks still need evidence.

Evidence follows technical prerequisites; formal approval follows the ordered REL gates. Prepare the exact unsigned archive and selected-map artifacts needed by feature/package checks while FR-03 is open, using existing owners and admitted inputs. Reference Stage 10 and other feature delivery checks join that shared rehearsal before FR-06's final candidate reconciliation; REL-08/09 acceptance is not their entry prerequisite. Freeze/reuse matching bytes and observations, or invalidate affected proof after changes. This authorizes no public distribution and bypasses no rights/trust gate.

The first public version ends at FR-08 / REL-11. M0-M7 studies, neural work, renderer-depth extensions and professional teaching/transfer exercises remain later work; their completion is not a condition for publishing v0.1. Preserve all already approved rendering features, maps, products, API, rights and trust gates.

#### Prebuilt Base And Optional Rebuild

**Scope-owner amendment, 2026-11-01:** Launcher remains the user-facing entry for both journeys. Users can run the release's precompiled products and prebuilt content without source assets, a build toolchain, a previous installation or older local content. They can also obtain the matching source tag, optionally use those precompiled/cooked outputs as their starting base, edit code/assets/shaders and use Launcher to Build/Cook/Run. The existing source-only build path remains available. Sync and local generation write the same binary/cooked files beneath `artifacts/dev` in both a checkout and an extraction. Open the extraction's `LaunchSparkle.cmd`; only Launcher/tooling knows preparation or packaging policy. Engine uses ordinary markers/paths, without package-manifest parsing, installed mode or a release-version state branch. Editor retains inspection and explicit shader reload, and may request shader recompilation with private symbols through the same Launcher/tool operation. Compilation policy stays in tooling; no source watcher is required. Consumers retain no prebuilt/local origin flag, seed receipt or alternate runtime path. Runtime registers ordinary level documents without preparation selection/raw-source gates; Launcher/Cooker alone consume the tool catalog. Run is direct; optional Sync/Build/Cook actions stay explicit.

The [package owner](../Architecture/Modules/BuildAndPackaging/PackagingAndInstallation.md#prebuilt-base-and-source-rebuild) defines the minimal workflow and deletions. Freshness is enforced when publishing each version; local editing uses ordinary explicit Build/Cook actions over the same source-workspace outputs. Do not add cross-release content reuse, per-file local version tracking, migrations, an updater or a second build system. Ordinary settings/logs/authored data retain their existing owners and are not deleted by a fresh release.

This supersedes the requirement for an installed shared content store and compulsory end-user source acquisition/cooking. Public source-content rights remain unresolved where recorded: acquisition-only assets cannot be included as cooked derivatives without admission. Keep all seven selected maps as delivery obligations; a missing right or prebuilt dependency remains open, without substituting a required source cook for the promised prebuilt run. Earlier candidate passes keep their older scope. The [documentation reconciliation](../../artifacts/validation/prebuilt-release-docs-20261101/completion.json) records this amendment; implementation and affected candidate proof remain open in FR-06.

#### Consumer Effort And Game Presentation

**Scope-owner decision, 2026-11-01:** prepare binaries, complete admitted cooked content, runtime dependencies, defaults and authored cameras before release. The ordinary journey is download/extract one ready-to-run consumer bundle -> open Launcher -> choose a supplied scene -> Run. No build, recook, source acquisition, SDK installation, command line, manual hash/file matching or shared-store repair is a prerequisite. Publisher verification, public rights and signing remain required. Matching source, private symbols and optional rebuild tools are separate developer choices; no installer, downloader or update service is required.

Game is the rendering window with existing camera input and native close/Alt+F4. No Game ImGui host, first-run menu, HUD, help/settings panel, loading/progress/debug overlay or Reference control/export UI is selected for this release. Launcher owns scene/API selection and launch feedback; Editor retains its existing controls, inspection, editing and Reference progress/pause/export. Camera/window/input behavior, requested content identity, responsive loading, safe failure/shutdown and rendering quality remain required. Use existing Launcher feedback, local logs and native failure paths; add no Game failure frontend.

This supersedes earlier Game menu/help and interactive Game Reference/export obligations. Keep host-independent Renderer/session/estimator and native identity controls. Remove the Game-only host and wiring through the existing Application owner, without a dormant UI flag or a second renderer; preserve Editor's shared ImGui support. The Game-only host/console and UI flags have been deleted from current source; Editor retains its own UI path. Earlier menu receipts retain their original candidate identity. [FR-06](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#prebuilt-delivery-simplification) owns the affected Game/Editor runtime checks. A future Game UI requires a new scope decision. The [documentation result](../../artifacts/validation/render-only-game-docs-20261101/completion.json) records the original documentation amendment; the [source cleanup](../../artifacts/validation/release-owner-slim-20261101/completion.json) records separate bounded checks, with new-archive qualification open.

### Approval And Next Work

**All six choices are approved.** The 2026-11-01 amendments update rows 1, 5 and 6 to a publisher-prepared prebuilt bundle with optional Launcher source rebuild, and row 3 to Launcher/Editor controls with a render-only Game. Rendering, Windows, display behavior and defaults keep their approved scope. This is a decision record, not another approval request.

| # / decision | Approved value | Delivery obligation |
| --- | --- | --- |
| 1. Packaging | Portable Windows x64 `v0.1.0`: Launcher, Editor and Game, complete admitted prebuilt content, matching source tag and optional precompiled source-workspace base. | Qualify prebuilt Run without source/build/previous content state, and a real user source edit followed by Launcher Build/Cook/Run. No installer, updater or stable plugin SDK. |
| 2. Windows | Windows 11 x64 24H2/build 26100 or newer; named laptop is the initial hardware/performance target. | Qualify the OS boundary and reject missing requirements; do not advertise unqualified hardware. |
| 3. Controls/display | English keyboard/mouse; one display; windowed/borderless; user display resolution, with resolution changes available through Editor/shared settings. Launcher selects scene/API; Editor owns help, camera reset and settings Save/Reset. Game has camera input and native close, with no ImGui UI. | No forced resolution list. Verify display modes, Unicode paths and 100/150/200% DPI where UI exists, plus Game camera/focus/native-close behavior. Controller, exclusive fullscreen, VR, localization and broad accessibility certification remain excluded. |
| 4. Defaults | D3D12; Lit; VSync On; automatic histogram exposure; ACES approximation; SDR; neutral grading; **chromatic aberration On and vignette On**; HDR opt-in. Empty without selected content. | Implement requested/active settings honestly. Freeze effect strengths in their owning feature contracts; no invented numerical values or silent disabled fallback. |
| 5. Updates/settings | Each version is a fresh complete build/cook; manual download/extract and re-extract rollback. Simple settings Save/Reset; no saves or stable source/binary API. | No generated-content migration, older-store prerequisite or local-file version tracker. Preserve authored source/settings; a user can start from the matching prebuilt base and deliberately rebuild local changes. |
| 6. Distribution/privacy | GitHub Releases; one ready-to-run consumer bundle with precompiled products and their complete admitted cooked base, offline after extraction. Optional source acquisition/rebuild stays in Launcher. No telemetry, account, unattended downloads or crash uploads. | Publisher verifies composition, hashes, notices, source obligations and required signatures. No manual content-part matching, support service or mandatory archive-dependent diagnostic helper for ordinary Run. |

#### Work For The Agent And Technical Owners

- Qualify [package inputs](../Architecture/Modules/BuildAndPackaging/PackagingAndInstallation.md#release-inputs): historical asset rights, exact notices/terms, helper/CRT closure, symbols and the current baseline.
- Prove each selected map's cooked materials/reference domain and combined product/content size against unchanged budgets. Do not silently strip assets, reduce quality or substitute a map.
- Certificate enrollment waits until before signed candidate freeze; it does not block unsigned internal packaging. Cornell's optional regeneration ZIP is not a normal GLB-sync prerequisite.

### ReleaseMapSet

The intended examples are fixed; admission still requires rights, material and package checks.

| Map | Purpose / current concern |
| --- | --- |
| `ModernSponza` | Principal showcase example; base plus colorful curtains. License wording and cooked size need resolution. Candle/knight variants are excluded. |
| `Sponza` | Classic lighting scene; exact redistribution terms remain open. |
| `CornellBox` | Reference/lighting scene; tracked GLB and [retained license](../../Projects/Showcase/Assets/Meshes/CornellBox/COPYRIGHT.txt). Normal sync no longer requires the publisher OBJ archive. Optional regeneration receipt and single-emitter/cooked proof remain open. |
| `CesiumMan` | Animation scene; retain attribution/logo notice and freeze an exact pose for reference comparisons. |
| `LPSHead` | Head/material scene; verify conversion/textures and material fidelity. Selection does not establish physical subsurface reference support. |
| `DiffuseTransmissionPlant` | Plant/animation scene; diffuse-transmission fidelity remains open. |
| `ABeautifulGame` | Chess scene; transmission/volume fidelity remains open. |

[Level recipes](../../Projects/Showcase/Levels.catalog) and adjacent asset notices own source identities, cameras and attribution. The accepted reference domain covers surface reflection; transmission, volume, spectral and physical subsurface transport remain excluded. Unsupported material effects cannot be silently dropped.

`Empty` is an Included built-in starting/control map, outside the seven content examples; it needs no downloaded asset pack. Launcher must let users select any subset of the seven, including none. Advertised prebuilt maps resolve complete same-release cooked dependencies without raw source acquisition or cooking. Optional source sync/cook remains available for editing; no missing requested map can masquerade as Empty.

Excluded from release example selection: `DamagedHelmet` (rights unresolved), `ModernSponzaCandles`, `ModernSponzaKnight`, `JungleRuins`, `BistroExterior`, `BistroInteriorWine`, `SanMiguelHigh`, `SanMiguelLow`, `StanfordBunnySubsurface`, `StanfordDragonSubsurface`, `AnisotropyBarnLamp` and `SheenCloth`. Moana is deferred and has no current catalog level. These remain development/source workloads and must be marked development or unavailable in Launcher; no feature is marked Removed. The inspected catalog contains 20 levels and 18 asset packs, with no content selected by default.

### Product And Supported Matrix

<details>
<summary>Exact machine, API and source-build requirements</summary>

| Field | Selected machine / approved boundary |
| --- | --- |
| Products / current source targets | Sparkle Launcher / `SparkleLauncher` and Sparkle Editor / `ShowcaseEditor` (`ShippingEditor`); Sparkle Game / `ShowcaseGame` (`ShippingGame`). The [membership dispositions](ReleaseScope/CapabilityDispositions.md#reconciled-membership-and-check-routes) freeze helpers and content-store boundaries; the reviewed package manifest owns exact shipped files/hashes. |
| Observed OS | Windows 11 Home 25H2, build `26200.9457`; approved target starts at Windows 11 x64 24H2/build 26100 |
| `MIN-01` / `REF-01` | Same AMD Ryzen 9 8940HX laptop, 16 cores/32 logical processors; 67,833,212,928 bytes OS-reported RAM; NVIDIA GeForce RTX 5070 Ti Laptop GPU, 12,227 MiB reported VRAM; driver `610.47` |
| Storage | Patriot P400L 2000GB NVMe SSD, NTFS; at least 16 GiB free for extraction/staging/user data |
| D3D12 | Default; feature level 12_1, SM 6.6, DXR 1.1. Reject missing required features before rendering. |
| Vulkan | Explicitly selectable; Vulkan 1.3 plus the [required feature set](#frozen-platform-and-toolchain-prerequisites). Separate native/provider/performance proof required. |
| Source build | Exact [toolchain](#frozen-platform-and-toolchain-prerequisites): VS Community 2026 18.7.0/MSVC 14.51.36231, Windows SDK 10.0.26100.0, CMake 4.3.3, Git for Windows 2.54.0, Qt 6.11.1 `msvc2022_64`, Vulkan SDK 1.4.350.0/Slang 2026.8, DXC 1.9.0.5347 |

This is an initial named-machine claim. Generic 8 GiB GPU/16 GiB PC floors and other hardware/drivers require qualification before advertisement. Two measurement roles on one machine do not count as documented reproduction. HDR additionally requires a qualified HDR10 display/OS configuration; none is recorded. No hardware purchase is authorized. Debug/Development builds remain source products; all three public products need qualified release configurations. Alternate toolchains remain unqualified.

</details>

### Baseline And Selection

Three presets have distinct purposes: ordinary viewing, realtime path tracing, and progressive reference output. Their implementation and numerical qualification remain delivery work.

<details>
<summary>Exact presets and consumer controls</summary>

- **`REL-NATIVE-1080P`:** 1920x1080 SDR Lit, raster GBuffer, native extent/Linear provider, Ray Reconstruction Off, TLAS/refit On, PTLAS Off, auto batching On, Automatic Histogram exposure, ACES approximation, automatic SDR encoding. Preserve declared direct/indirect diffuse/specular, direct subsurface and shadow defaults. Consumer VSync On; performance VSync Off. Grading neutral, chromatic aberration/vignette On, no baseline decal actors; these remain Included selectable features. HDR10 opt-in after qualification.
- **`REL-RT-1080P`:** fixed 1920x1080 output for laptop acceptance only, traced primary/secondary transport, TLAS, DLSS Quality requested and Ray Reconstruction On; freeze exact SDK-derived internal extent and composition before measurement. Frame Generation and dynamic resolution Off during timing. Other exposed SR/RR modes need their feature proof. Freeze depth/reuse/approximation in the existing discoveries, keeping one final image. Native/RR-Off controls measure cost/quality. Ray GBuffer alone or Linear/Off fallback cannot satisfy this preset; current Lit lighting is only the starting implementation path.

- **`REL-REFERENCE`:** progressive admitted surface transport in the consumer viewport; sample/convergence/invalid state, reset/cancel and reproducible raw-linear export. No DLSS, reconstruction, firefly clamping or artistic post effects in oracle output. Full admitted reflection is required; finite-depth output is diagnostic. Freeze animation pose and reset/reject contributing scene/camera/material/light changes. [Reference semantics](../Architecture/Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/TransportAndEstimator.md#claim-boundary) owns the domain and two distinct products.

The 1080p preset IDs identify controlled laptop acceptance runs, not a product resolution restriction. Normal viewing uses the user-selected display/output resolution and the selected provider's internal extent. Record both extents and qualify display changes. Presented Lit/realtime presets include the approved lens defaults; raw reference/oracle exports keep artistic effects disabled by their existing semantic contract.

At `9bf95541` (2026-10-25), `DefaultEngine.ini` matches the Lit defaults: VSync On, ACES approximation, Linear/NativeAA, reconstruction Off, TLAS, chromatic aberration/vignette On. Per-user settings overlay these values. Intentional Empty startup and failed selected-map activation remain distinct; these source facts do not pass the pending PD-1 journeys.

| Consumer route | Approved behavior / owner |
| --- | --- |
| API | `--graphics-api D3D12` or `--graphics-api Vulkan`; precedence: build default, then `SPARKLE_RHI_BACKEND`, then CLI. Invalid token fails visibly. Application graphics launch owns it. Existing `--renderer`/`--rhi` are development conveniences. |
| Startup/examples | No selected example opens built-in Empty. Launcher keeps the seven map cards and truthful ready/failed states; prebuilt Run consumes current-release cooked closure, while source-workspace actions explicitly acquire/build/cook changed inputs. Modern Sponza uses its authored camera. Failed requested content retains prior valid state or stops visibly. |
| Settings | Editor retains rendering controls, requested/active state, restart indication and shared settings Save/Reset. Game consumes approved defaults/shared settings without a settings frontend or required configuration step. |
| Help/quit | Help is in Launcher, Editor and the accompanying instructions. Game closes through the native window/Alt+F4; no Game help, menu or Quit widget is required. Mouse-look releases on focus/capture loss. |
| Camera | W/S or Up/Down forward/back; A/D or Left/Right strafe; E/Space up; Q/C down; Shift sprint; hold right mouse to look; wheel adjusts speed. Help and optional sensitivity adjustment live outside Game; Editor provides inspection/reset of the authored camera. Focus/capture release and native close remain required; no Game menu/HUD. The [input collector](../../Engine/Application/Private/Input/CameraInputIntentCollector.cpp) and GameFramework camera controller retain movement ownership. |

Game excludes developer console/CVar assignment, startup-map environment selection, capture flags, shader recook, debug panels and authoring commands. Editor and Launcher deliver their admitted editing, settings, diagnostics and sync/cook/launch workflows through the existing owners; debug-build-only external capture stays development-only. Approved display/input limits are decision 3; readable labels, scalable text, keyboard navigation and visible errors remain required basics. Native refresh/VSync must be reported honestly.

</details>

### Mutable Data And Compatibility

Keep documented controls/settings/map IDs stable within `v0.1.x`; source APIs and development keys are unstable. No binary SDK, saves or migration promise. Update by verified extraction into a new directory; back up settings for rollback. Concurrent versions writing shared settings are unsupported. Reset/cache removal must preserve unrelated data; deleting the extraction does not delete user data.

The [path and budget contract](#frozen-paths-mutation-boundary-and-budgets) owns per-user folders, immutable package bytes and deadlines. [Installed compatibility](#compatibility-boundary-created-by-shipping) owns the shipping boundary.

### Budgets And Observation

Use the unchanged [startup, size, memory and operation limits](#frozen-paths-mutation-boundary-and-budgets) and [30 FPS protocol](#thirty-fps-performance-floor). Measure combined product bytes and the seven-map content closure; freeze combined release/base-content accounting before REL-03. User-selected display resolution remains separate from the fixed laptop benchmark. Both APIs and all seven maps require proof; no dynamic resolution, fallback or post-result retuning can manufacture a pass. Reference tracing has convergence/cancellation/resource checks instead of a 30 FPS promise. [Principal frame review](#principal-frame-review) remains required.

### Network, Security, Publication And Support

Decision 6 is approved. No user support service, response deadline or maintenance window is offered. Publication still requires the [trust/approval rules](#approval-rules): trusted Authenticode on every shipped PE, SHA256SUMS, manifest, dependency/content notices, software bill of materials, build provenance and vulnerability/malware disposition. Keys/accounts stay outside source/archives; unsigned builds remain developer prereleases.

Source/content acquisition uses public HTTPS inputs with pinned revisions/hashes and no private credentials. Game and Editor run offline with the supplied prebuilt content; Launcher network work is user-selected. Logs/captures/crashes may contain paths, scene content or memory; keep them local and explain privacy before any voluntary sharing. A separate symbols archive follows the same privacy boundary. No reporting channel or automated uploader is assumed.

Publish new immutable patches and rerun affected gates; retain version hashes/notices/known issues/advisories. Withdraw unsafe payloads with a visible advisory and verified rollback instructions. Never silently replace a published signed archive.

Excluded product promises: general NVIDIA/AMD/Intel-wide support, neural training/inference platform, multiplayer/networking, mods, stable SDK, installer/updater, Linux/macOS, mobile and VR. No additional scene families or AMD specialist profiler work. PTLAS and source-only external capture remain experimental within their declared vendor/debug limits.

## Release Gates

Gates are strictly ordered. A later gate may be prepared, but it cannot be accepted while an earlier gate is red.

| Gate | Exit criteria | Required artifact |
| --- | --- | --- |
| `REL-00 Scope and freeze` | Launcher/Editor/Game and source-adopter promises, platform/no-support/budget matrix, feature classifications, public/compatibility/non-goals, and release roles are frozen. | Approved [Release Scope](#approved-release-scope) and [Capability Dispositions](ReleaseScope/CapabilityDispositions.md), exported into candidate `release-scope.md` with audience journeys and compatibility decisions; exact rights receipts remain required. |
| `REL-01 Identity, rights, supply chain, and map set` | Version/publisher/license are real; exact `ReleaseMapSet`, immutable dependency closure, signing/distribution route, threat model, SBOM plan, notices, and redistribution decisions are reviewed. | Identity/signing decision, threat model, dependency/license/SBOM inventory, notices audit, and map manifest. |
| `REL-02 Clean reproducible baseline` | Clean tagged-source candidate performs empty-cache dependency sync, configure, named Shipping build, approved checks, and warm/offline-cache replay; validation and any explicit test authorization are recorded. | Environment/provenance manifest and raw sync/build/check logs. |
| `REL-03 Package spine` | Existing Launcher/CMake owners prepare a fresh Build/full Cook; one package command freezes/stages/verifies/archives the finished files, with signing deferred to FR-07 immediately before public release. Unsigned internal archives qualify the product implementation route; they do not qualify public distribution. The allowlisted staged tree has immutable content, per-user writable state, dependency closure, SBOM, hashes, and no repository path dependency. | First non-candidate archive, package inventory, explicit signature status, SBOM, and created-path record; final signatures required before `REL-10`. |
| `REL-04 Current feature closure` | Every current surface is `Included`, `Experimental`, `Excluded`, or `Removed`; each included/experimental promise has a candidate-bound report and passes its applicable integration/failure criteria within frozen scope. Reuse existing implementations and evidence; do not implement unselected target capabilities. | Approved feature-closure matrix and candidate-bound completion reports, including local cleanup/deletion disposition. |
| `REL-05 Map correctness and PBR` | Every release map passes end-to-end, artifact, PBR, debug-view, reference, and temporal review on each included backend. | Per-map evidence packages and defect disposition. |
| `REL-06 Performance and stability` | Every release map meets the 30 FPS floor; load/soak/repeat/resize/switch/shutdown and memory gates pass on the supported matrix. | Raw timings, captures, memory records, and stability log. |
| `REL-07 Native backend diagnostics` | D3D12 and Vulkan validation passes are clean or every finding is resolved/classified; crash/device-loss diagnostics are actionable. | API validation logs, captures, driver matrix, and incident records. |
| `REL-08 Clean-machine candidate` | The frozen internal bundle starts all three products as a standard user, runs the supplied prebuilt maps and completes Editor/Game journeys offline without source sync/build/cook on minimum/reference machines, with read-only install, per-user state, integrity, compatibility, and failure checks. | Candidate hashes and explicit unsigned-internal status, clean-machine record, filesystem/network inventory, screenshots, and logs. |
| `REL-09 Candidate release review` | The author or agent completes both consumer and source-adopter journeys from public instructions; all failure-mode checks pass; zero open `S0`/`S1`; waivers and known issues are approved. | Signed go/no-go checklist, consumer report, source-adopter report, and negative-test ledger. |
| `REL-10 Publish and verify` | The installed-consumer compatibility policy is effective; tag/artifact bytes are immutable; uploaded hashes/signatures/attestation match; fresh downloads pass all three product journeys and shortest source checks; notes/notices and the no-support statement are accurate. | Published URL/tag, checksums/signatures/attestation, retrieval logs and final policy. |
| `REL-11 Stabilize and close` | A bounded maintainer release review is completed, known candidate defects are dispositioned, any replacement/withdrawal has exact provenance, and no unresolved `S0`/`S1` remains. No ongoing user support or response window is required. | Release-review ledger, any patch/withdraw records, retrospective and approval to unlock new feature work. |

### Gate Failure And Recovery Matrix

| Gate | Representative failure modes | Required response before retry |
| --- | --- | --- |
| `REL-00` | Audience omitted; unlimited feature promise; no minimum hardware; runtime/source/SDK claims confused; package/update responsibility or no-support boundary unspecified. | Reconcile the changed surface, classify it and assign owners/checks under the six approved choices. Only a change to those promises needs a new scope-owner decision; do not reapprove settled scope for a technical inventory gap. |
| `REL-01` | Placeholder identity; unclear map/dependency redistribution; moving dependency; no signing path; debug/vendor payload assumed redistributable. | Remove/replace/pin the input, obtain and record rights/signing route, regenerate inventory/notices, and reapprove. Unknown rights are a stop, not a waiver. |
| `REL-02` | Dirty source; dependency resolves differently; empty-cache configure/build fails; warm cache hides missing input; warnings/check failures. | Fix the authoritative build/dependency path, recreate from clean state, retain the failed log, and rerun both cold and warm routes. |
| `REL-03` | Missing DLL/content/manifest; extra debug/source/private file; writable install assumption; wrong signature; repository-relative lookup; nondeterministic membership. | Reject the stage, fix its single owner, regenerate/sign/verify/package from source, and never patch the archive manually. |
| `REL-04` | Selectable unclassified feature; silent fallback; requested/active mismatch; happy path only; unsupported option schedules partial work. | Fix and prove the feature or remove it from package/UI/docs. Update every producer/consumer and invalidate dependent evidence. |
| `REL-05` | Missing asset; PBR/reference mismatch; NaN/Inf; temporal artifact; backend divergence; fallback `Empty`; threshold chosen after viewing result. | Stop the map, classify owner, fix or remove map/feature, refreeze thresholds when the contract—not the result—was wrong, then rerun the full map/backend record. |
| `REL-06` | Presented/CPU/GPU p95 over 33.33 ms; hitch cluster; load/startup budget miss; memory growth/thrash; thermal/dynamic-quality contamination. | Mark failed, capture bottleneck evidence, optimize the owner or reduce the preapproved scope/preset honestly, then rerun all three runs and stability loops. |
| `REL-07` | D3D12/Vulkan validation message; device removal; descriptor/resource lifetime error; crash record lacks context; validation path changes behavior. | Fix the API/lifetime owner, preserve reduced evidence, rerun focused validation then affected maps; do not waive an uncategorized native error. |
| `REL-08` | Smart App Control/antivirus block; no-admin/read-only/offline failure; settings write into install; corrupt-cache crash; unexpected network/file mutation; removal damages user data. | Reject candidate bytes, correct package/runtime ownership or supported scope, produce a new candidate ID, and rerun every invalidated gate. |
| `REL-09` | The documented route needs hidden repair; source build cannot reproduce; instructions omit a prerequisite; negative check misses its injected defect; open `S0`/`S1`. | No-go. Fix product/docs/evidence, generate a new candidate if bytes changed, and repeat affected acceptance with a clean state. |
| `REL-10` | Uploaded bytes/hash/signature/tag differ; compatibility policy still assumes no consumers; broken release/onboarding link or false support promise; fresh download fails. | Stop announcement or withdraw affected assets, preserve incident evidence, publish corrected bytes only under a new version/candidate, then verify again. |
| `REL-11` | Known `S0`/`S1`, security/rights defect, repeat crash or public-byte mismatch at closeout. | Block closure, preserve exact evidence, then fix/revalidate or withdraw affected bytes. No public response deadline is promised. |

## Purpose And Authority

Classify every reachable capability as Included, Experimental, Excluded or Removed in [Release Scope](ReleaseScope/CapabilityDispositions.md). Included/Experimental features need their applicable checks; an unclassified or silently broken option blocks release. Code proves implementation; candidate reports prove only their recorded configuration.

## What The First Release Delivers

The default binary delivery is a portable Windows x64 archive. An installer is out of scope unless a later gate proves it is necessary. The release has four independently identifiable outputs:

1. an annotated release tag and immutable source snapshot, signed when the approved signing policy requires it;
2. `SparkleEngine-v0.1.0-windows-x64.zip`, one ready-to-run consumer bundle with Launcher, Editor, Game, exact runtime dependencies, defaults, built-in Empty, notices/instructions and the complete admitted cooked base. Matching source/symbols and optional developer rebuild tools may be separate companions; no manual binary/content-part matching is required;
3. `SparkleEngine-v0.1.0-symbols.zip`, retained or published separately according to the local-diagnostics/privacy policy;
4. `SHA256SUMS.txt`, release notes, known issues, and exact build provenance. The product bundle contains the authoritative `manifests/sparkle-package-manifest.json` used by package discovery.

All three products and their current-release cooked base are required outputs. Launcher selects prebuilt maps and launches Editor/Game without source assets or older local content. With no selected example, start Empty. Optional matching source/base import, source sync and Build/Cook/Run use the existing workspace owners. Freeze exact runtime/base/optional-helper membership and configurations; add no installed-store or version-tracking requirement.

### Frozen Product And Output Classification

The release-owner amendment replaces the earlier Game-only PD-0 freeze. Public names are Sparkle Launcher, Sparkle Editor and Sparkle Game. Current source targets are `SparkleLauncher`, `ShowcaseEditor` and `ShowcaseGame`; bind public names to exact files/configurations in the package manifest. Approved format/platform/privacy choices are recorded in [Release Scope](#approval-and-next-work).

| Product or output | Classification | Exact `v0.1.0` boundary |
| --- | --- | --- |
| Sparkle Game and cleared cooked Showcase content | **Included** | Game product, currently `ShowcaseGame` in `ShippingGame`. Built-in Empty starts without example downloads; the seven selected maps require complete verified content closure before activation. |
| Showcase project source, catalog, and authored levels | **Included** | Part of the immutable source tag/archive and the source-adopter route; only cleared game products enter the binary archive. |
| Sparkle Launcher | **Included** | Preserve selection/previews, readiness, progress/cancel, Run and optional source Build/Cook through existing operation owners. All seven release maps and required rendering remain obligations; Launcher does not own packaging policy. |
| Sparkle Editor | **Included** | Shipped authoring/inspection product, currently `ShowcaseEditor` using `SparkleEditor`. Its clean-machine workspace/edit/settings/capture and required tool routes must pass; Game retains structural Editor isolation. |
| ShaderCompiler, AssetCooker, texture/mesh/material/scene cookers, SourceImporters, and ToolSupport | **Included for source rebuild; bounded optional precompiled helper membership** | Preserve Launcher source acquisition/import/cook/rebuild. Supply only the helpers consumed by a ready-to-rebuild base, or build them with documented prerequisites. Ordinary prebuilt Run needs no cooker/compiler/helper SDK. |
| Immutable tagged source tree | **Included** | Public source-adopter output with unstable `0.y.z` source API, no binary ABI promise, and no installed integration contract. |
| `SparkleEngine-v0.1.0-windows-x64.zip` | **Included; format approved** | One complete portable consumer bundle, within existing combined budgets. Publisher builds/cooks/prepares every runtime input; ordinary use needs no manual parts assembly or previous content. Freeze final membership after fresh build/cook. |
| `SparkleEngine-v0.1.0-symbols.zip` | **Included, separate** | Exact-candidate native and shader symbols retained or published under the local-diagnostics/privacy decision; never mixed into the Game archive. |
| Installer, updater, registry integration, file associations, and prerequisite bootstrapper | **Excluded** | The user extracts/removes the portable archive. Launcher is included; no installer or prerequisite bootstrapper is implied. |
| Stable binary SDK/plugin ABI, installed integration headers/libraries or project SDK/templates | **Excluded** | A matching precompiled/cooked starting base for rebuilding this source is included scope; it creates no stable external binary-link, ABI, installer or plugin contract. |
| Game/editor developer console, shader recook, capture/debug panels, and authoring commands in `ShippingGame` | **Excluded** | They must be unreachable and absent from the runtime package/import surface; native Game close and external Launcher/Editor help/settings paths cannot depend on them. |

No requested product is classified **Removed** at `PD-0`. `Removed` remains the required disposition for a later-discovered reachable product or claim that has neither an admitted audience nor an owner; it may not be relabelled Developer-only to avoid deletion.

### Frozen Platform And Toolchain Prerequisites

These are contract targets, not evidence that the current source enforces or passes them. `REL-02`, `REL-06`, `REL-08`, and `REL-09` retain the executable proof. Packaged start/offline use cannot require an environment override, source checkout, administrator rights or undeclared developer SDK. Launcher may use the network only for user-selected content sync; the source-build toolchain remains a separate prerequisite matrix.

| Boundary | Supported prerequisite for `v0.1.0` | Current implementation consequence |
| --- | --- | --- |
| Runtime OS and account | Windows 11 x64, version 24H2/build 26100 or newer; standard user; long paths enabled only if the final package proves it needs them | No accepted OS/version preflight exists; below-minimum rejection is `FCR-PROD-01` work. |
| Runtime CPU and memory | x86-64 CPU with SSE4.1, at least 4 physical cores/8 logical processors, and 16 GiB system memory | No accepted CPU/RAM preflight or minimum-machine result exists. |
| D3D12 minimum | 8 GiB dedicated GPU memory, D3D feature level 12_1, Shader Model 6.6, and DXR 1.1; D3D12 is the default public API | Source requests feature level 12_1 and rejects less than Shader Model 6.6, but complete capability rejection and the memory floor are unproved. |
| Vulkan minimum | The same memory/RT class plus Vulkan 1.3, swapchain, graphics/compute/copy queues, timeline semaphore, synchronization2, dynamic rendering, formatless storage-image reads/writes, descriptor indexing, buffer-device-address, acceleration-structure, ray-query, and ray-tracing-pipeline support | Source rejects a loader/device below 1.3 and missing core requirements; the full first-release feature matrix remains unproved. |
| Runtime storage/display | SSD; 16 GiB free before extraction; 1920x1080 SDR desktop output is the minimum acceptance presentation | Final archive/content sizes and display behavior remain candidate evidence. |
| Supported source toolchain | Visual Studio Community 2026 `18.7.0` / MSVC `14.51.36231`, Windows SDK `10.0.26100.0`, CMake `4.3.3`, Git for Windows `2.54.0`, Qt `6.11.1` `msvc2022_64`, and Vulkan SDK `1.4.350.0` containing Slang `2026.8` and DXC `1.9.0.5347` | This is the sole `v0.1.0` source-adopter matrix until another row passes `REL-02`. Source-declared CMake `3.20`, Git `2.25`, Qt `6.8`, VS 2022, clang-cl, and Ninja paths remain Developer-only compatibility hypotheses, not public support claims. |
| Source validation tools | clang-format and clang-tidy `22.1.3`, discoverable on PATH or through the [style owner](../Engineering/Foundations/CodeStyle.md#repository-commands) | Required by the approved `code_style_check` baseline gate; not required to launch packaged products. |
| Cold source acquisition | Git and HTTPS network access for pinned source/dependency inputs; no credentials or private caches | Git dependencies use full commit hashes. Configure rejects dirty or mismatched Git caches without resetting or deleting them. Verify pinned inputs for the fresh publisher/source-only route. Optional developer warm/offline observations retain their own scope; they are neither a prerequisite nor proof of a fresh published build. |

The release owner selected this existing laptop as both `MIN-01` and `REF-01` on 2026-10-10: Windows 11 Home 25H2 build `26200.9457`, AMD Ryzen 9 8940HX (16 cores/32 logical processors), 63.17 GiB OS-reported RAM, NVIDIA GeForce RTX 5070 Ti Laptop GPU with 12,227 MiB reported dedicated memory on driver `610.47`, and Patriot P400L 2000GB NVMe SSD/NTFS. The generic CPU/RAM/VRAM floors above are necessary prerequisite targets, **not a supported lower-end machine claim**: initial performance acceptance is on this named laptop. Expanding to another hardware class requires scope approval and its own proof. The [scope record](#product-and-supported-matrix) owns the selected machine/exposure boundary. Hardware inspection and owner selection do not prove graphics support, 30 FPS, an HDR-capable display or documented reproduction.

### Frozen Paths, Mutation Boundary, And Budgets

Freeze Launcher, Editor and Game components, exact executables/configurations/runtime closure and complete package-relative cooked content before REL-03. Ordinary Run reads that release base directly; no writable shared content store is required. Keep the package manifest/notices authoritative; never infer membership by scanning development artifacts. Root `README.md`, `Docs/README.md`, `SUPPORT.md` and `SECURITY.md` remain the public entry/policy owners. Provide truthful self-service/no-support/privacy instructions; no intake service is required.

| State or operation | Frozen location/effect |
| --- | --- |
| Game immutable bytes | User-selected extraction root, including its complete admitted cooked base. All products run with this tree read-only. Optional source cooks write only in their explicit source workspace, never into the consumer extraction. |
| Game mutable root | `%LOCALAPPDATA%\SparkleEngine\Development\<workspace-key>\Showcase\` for either source or extraction. No release-version lookup or migration. |
| Runtime settings | `Settings\EngineRendering.ini`; shipped `Config\DefaultEngine.ini` remains immutable, user values overlay it, and writes affect only the per-user file. **Reset settings** affects only this file and returns documented defaults. |
| Game logs/captures/crashes/cache | `Logs\`, `Captures\`, `Crashes\`, and `Cache\` below the mutable root; each has a separate retention/clear action. No `v0.1.0` save-game product exists. |
| Launcher state | `%LOCALAPPDATA%\SparkleEngine\LauncherState\<repository-key>\{Settings.json,Activity.json,Logs\}`; current source route is repository-keyed; extractions use the same root-keyed state calculation without requiring source files or a release manifest. |
| Source build/cook products | Repository-owned `build\` and `artifacts\`; development mutable state uses the repository-keyed `%LOCALAPPDATA%` root. Editor **Save All** may change the explicitly selected authored `.level` source, while clean/recook/reset may change only previewed generated roots. |
| Remove runtime | Delete the extracted immutable root; user state remains until the user invokes the separately labelled data-removal action. |

| Budget | Frozen threshold and oracle |
| --- | --- |
| First interactive consumer surface | At most 15.0 s from process creation to responsive Launcher/Editor UI or intentional Game rendered output with working input on the minimum machine with cold OS file cache. Built-in Empty is valid; Game needs no menu/overlay. Failed-map fallback is not success. |
| Default accepted example active | At most 30.0 s from explicit selection to published active-level identity and first correct presented frame; any other `ReleaseMapSet` member has a 60.0 s ceiling. |
| Shutdown/cancellation | Game exits within 5.0 s of quit/close; Editor and Launcher settle owned child/task work within 10.0 s after explicit cancellation/close. |
| Game bytes/storage | Game archive at most 4 GiB; extracted immutable tree at most 8 GiB; documented free-space prerequisite 16 GiB. |
| Runtime memory | Process committed-memory high-water at most 12 GiB and tracked local GPU-memory high-water at most 7 GiB on the minimum machine; no unbounded growth across the stability loop. |
| Runtime frame rate | Use the existing [30 FPS release floor](#thirty-fps-performance-floor); this row does not weaken its p95/p99, hitch, or measurement rules. |
| Source-adopter cold/warm route | Cold sync/configure/build/cook at most 120 min, warm no-change replay at most 15 min, peak system memory at most 32 GiB, and owned working storage at most 100 GiB on the supported source machine. |
| Launcher local planning | A non-network readiness/preflight refresh completes within 5.0 s; external process/download time is reported separately and never converted into ready/success. |
| User support | **None offered.** No acknowledgement, fix, security-response deadline or maintained-version window. Publish known issues, local diagnostic/privacy instructions and self-service recovery; release review is a finite maintainer task. |

Product/release scope is owned by the release owner; Application/Showcase own the Game journey; Build And Packaging owns source/package orchestration; Launcher and Editor own their shipped workflows; release delivery owns truthful no-support/privacy documentation, candidate symbols, release review and any versioned replacement/withdrawal. No staffed public channels or ongoing service are required or claimed.

## Release Audiences

Calling this an engine release creates two required acceptance paths. Passing only one does not make SparkleEngine shippable.

| Audience | Required promise | Required shortest journey |
| --- | --- | --- |
| Product user | All three products and supplied prebuilt maps run offline without source/build/previous content or manual setup. | Download/extract one consumer bundle -> Launcher -> choose supplied scene -> Game rendering/camera/native close, or optional Editor inspection/editing -> exit/remove. Publisher verification is not an end-user command prerequisite. |
| Source adopter | The matching source tag builds from public prerequisites; its matching precompiled/cooked base is an optional starting point for editing and Launcher rebuilds. | Obtain matching source -> optionally import its base -> edit one real code/asset/shader input -> Launcher Build/Cook/Run -> observe the changed result. Also retain a fresh source-only build/cook check. |
| Contributor | Contribution workflow is discoverable and failures are actionable. | Optional for `v0.1.0`; if advertised, it receives its own accepted workflow. |
| Binary SDK/plugin consumer | Stable installed headers, libraries, ABI, samples, and compatibility rules. | Excluded from `v0.1.0` unless explicitly added and proven; exported C++ symbols alone are not an SDK. |

The source API may remain unstable during `0.y.z`, but the exact contents of a published version are immutable. Public documentation MUST distinguish source availability from a supported binary SDK and name what compatibility, if any, a patch or later minor release preserves.

## Completion Vocabulary

Use these states in release trackers and feature records. Do not use “done” or “feature complete” as substitutes.

| State | Meaning | Permitted public claim |
| --- | --- | --- |
| `Source present` | Relevant code or configuration exists and was statically inspected. | None about runtime correctness. |
| `Integrated` | The owned producer/consumer path builds or runs in a focused check. | The named integration path exists. |
| `Verified` | Required correctness, quality, performance, and failure evidence passes on the declared matrix. | The exact verified configuration and limitation. |
| `Release candidate` | The frozen commit and staged package pass all pre-publication gates. | Candidate for the named release, not yet shipped. |
| `Shippable` | Independent package acceptance passed and the approval record is complete. | Shippable on the named platform and matrix. |
| `Published` | Immutable artifacts were uploaded and post-publish retrieval was verified. | Released at the exact version and artifact hashes. |
| `Blocked` | A required result is missing, failed, unavailable, or inconclusive. | The blocker only. |

Evidence expires when relevant code, content, configuration, toolchain, dependency, driver, or package bytes change. The affected gate returns to `Blocked` until rerun.

## Traceability And Acceptance Identity

The [Change Lifecycle iteration control record](../Engineering/Workflow/ChangeLifecycle.md#create-the-iteration-control-record) applies to every release iteration. This contract specializes its identifiers without creating duplicate criteria:

| Identifier | Owner and release use |
| --- | --- |
| `NS-*` | The [persona North Star](../Strategy/EngineerPersona.md#north-star) owns the required engineering outcome. Every iteration marks applicable facets. |
| `PGE-*` | [Requirements](../Strategy/Requirements.md#canonical-requirement-matrix) owns persona capability and evidence-level meaning. |
| `REL-*` | The [release-gate table](#release-gates) owns release-level acceptance criteria. Stage prose explains execution; it does not create a weaker parallel gate. |
| `MAP-*` and `CASE-*` | [Graphics Workloads](GraphicsWorkloads.md) owns map, quality, performance, and portfolio-workload criteria. Apply its method to release-selected maps; later Bistro/San Miguel and neural/portfolio gates are not first-release map obligations. |
| `RISK-REL-*` | The roadmap [release risk register](../Strategy/Roadmap.md#release-risk-register) owns cross-release exposure, treatment, trigger, and retirement. Architecture feature dossiers own stable feature-specific risk definitions; candidate reports record their current exposure and disposition. |
| `FCR-*` | [Feature Completion Reports](FeatureCompletionReports.md#initial-completion-report-registry) owns the initial feature-report families and candidate report schema. |
| `AC-<scope>-NN` | One binary feature, change, or iteration criterion beneath the owning `REL-*`/`MAP-*` gate. Stable feature criteria live in the Architecture dossier; the candidate report records the result. |
| `FM-<scope>-NN` | One controlled failure contract. `FM-REL-*` below owns common release faults; Architecture feature dossiers add narrower IDs without copying these rows, and reports record what ran. |
| `CHK-<scope>-NN` | One validation action designed under the [check and test contract](../Engineering/Verification/ValidationAndEvidence.md#check-and-test-design-contract). Feature dossiers own stable mappings; reports own candidate execution. It is not authorization for permanent submitted test code. |

The candidate traceability ledger MUST map each included/experimental `FCR-*` and applicable `RISK-*` to at least one `AC-*` and `FM-*`, then map every criterion/failure to one or more `CHK-*` and retained artifacts. Every `CHK-*` maps back to the claim and risk it informs. An orphan target, risk, criterion, failure, check, result, or waiver keeps the owning `REL-*` gate `Blocked`.

## Release Scope Freeze

`REL-00` produces one version-controlled release-scope record. It MUST name:

The [approved scope](#approved-release-scope) supplies that record, with [512 inventory dispositions, current selector splits and component/helper/store membership](ReleaseScope/CapabilityDispositions.md). [FR-00 reconciliation](#fr-00-reconciliation-result) closes discovery for its recorded snapshot. [FR-01](#fr-01-source-requalification) records the hash-bound source-candidate `release-scope.md` export. FR-06 rechecks final candidate identity and changed dispositions; rights and executable acceptance remain with their later gates.

- public product name, version, platform, architecture, minimum Windows version, and supported GPU/API matrix;
- minimum and reference CPU, memory, GPU, driver, and storage configurations;
- the exact Game executable and default graphics configuration;
- required audiences, supported source toolchain, public API/ABI stability, persisted-data compatibility, reset, update, downgrade, and side-by-side-install policy;
- every selectable render mode, debug view, upscaler, lighting path, post-processing stage, presentation path, and backend as `Included`, `Experimental`, or `Excluded`; deferred decals, Color Grading, Chromatic Aberration, and HDR10 output are required `Included` results, while Frame Generation and Volumetric Lighting remain explicit exclusions;
- the `ReleaseMapSet`, meaningful first-run/default map, consumer map-selection path, fixed cameras/routes, and redistribution disposition;
- exact Launcher/Editor/Game configurations, same-release cooked-base/optional rebuild-helper membership and source-workspace paths;
- network requirements, prerequisite installer policy, immutable installation root, per-user save/cache/config/log/capture/crash locations, reset, and uninstall instructions;
- first-interactive-frame, map-load, shutdown, package-size, installed-size, disk-space, CPU-memory, and GPU-memory budgets in addition to the fixed 30 FPS floor;
- SDR/HDR, resolution/refresh, DPI, monitor, keyboard/mouse/controller, language/locale, and accessibility scope;
- telemetry/network behavior, privacy/consent, security-reporting, dependency/SBOM, vulnerability, malware-scan, and release-provenance policy;
- no-support boundary, local crash-data privacy, known limitations and explicit non-goals; any optional reporting route needs separate approval;
- distribution channel and executable/archive/tag signing policy. Direct public Windows downloads require trusted Authenticode signatures for every shipped PE file; an unsigned build is a developer/pre-release artifact, not this consumer release.

Any later scope addition returns `REL-00` to `Blocked`. Defect fixes within frozen scope do not. An approved reduction updates every affected selector, capability disposition, plan and dependent claim; it invalidates affected evidence without marking removed work as passed. Use [scope and retirement decisions](../Engineering/Workflow/ChangeLifecycle.md#scope-and-retirement-decision). This documentation iteration makes no capability `Removed` and grants no executable acceptance.

### FR-00 Reconciliation Result

**`ITER-FR00-02` — PASS, scope discovery only (2026-10-31).** Owner: release scope; technical closure remains assigned to the existing module/stage owners in the [ledger](ReleaseScope/CapabilityDispositions.md#reconciled-membership-and-check-routes). Input: clean `e44692cce7eb33052b0a1ac763e66137a861639b` plus this documentation diff. [Frozen inputs](../../artifacts/validation/docs-focus/fr00-20261031/inputs-before.json) and [check results](../../artifacts/validation/docs-focus/fr00-20261031/validation.json) bind the observation. This is not a package, runtime, rights or public-release verdict; the subsequent source-candidate export is recorded under [FR-01](#fr-01-source-requalification).

**Outcome and budget:** preserve `NS-REAL`, `NS-EVIDENCE`, `NS-OWNERSHIP` and `NS-ADOPTION`; advance `NS-SIMPLIFY` through one reconciled scope authority. Preserve `PGE-01/13/15` obligations with no evidence-grade increase; other persona/workload gates are not applicable to documentation discovery. Retain the existing scope/ledger owners; replace stale helper, selector and default descriptions. Production state/API/hooks/copies/dependencies, generated builds and runtime exposure: zero. Source deletion is unjustified because this iteration classifies exposure without retiring implementation. No target-only feature or new validator is shipped.

| Criterion / failure / risk | Check and fixed oracle | Result |
| --- | --- | --- |
| `AC-FR00-01`: current inventory, selected levels/packs, view enum, persisted defaults and package composition all have finite dispositions and named check routes. `FM-FR00-01` / `RISK-FR00-01`: a stale/missing selector or helper could escape scope. | `CHK-FR00-01`: compare all 16 inventory ID sets to the ledger, enum/persistence names and INI values to their rows, catalog release/pack closure and generated-product declarations to membership. Removing a classified key or changing release membership in memory must fail. | PASS; 512 unique capability IDs, 18 modes, 27 common settings plus one conditional setting, 20 levels / 18 packs, six executable/profile tuples. No unmatched audited surface. |
| `AC-FR00-02`: all six approved choices, required rendering and accepted PTD-00 remain intact; every technical gap is assigned rather than implemented or submitted for renewed user choice. `FM-FR00-02` / `RISK-FR00-02`: documentation could silently enlarge/narrow scope or reopen discovery. | `CHK-FR00-02`: compare the six decision rows and frozen production/reference inputs, including PTD-00 discovery/transport/plan, to the starting hashes; reconcile any concurrent inventory documentation against its unchanged ID set; inspect every changed line and gap/check route. An altered approved decision must fail the control. | PASS; no production or reference-dossier change. Grading/HDR/decals, reference host reachability, PTLAS exposure and package failure proof retain their later owners. |
| `AC-FR00-03`: stage result, remaining plan, roadmap and navigation agree. `FM-FR00-03` / `RISK-FR00-03`: stale links or completed prompts could restart discovery. | `CHK-FR00-03`: strict UTF-8, local links/anchors, stale-text scan, scoped diff review and `git diff --check`; a missing anchor must fail the link control. | PASS; source-style mechanical/authored checks N/A because only Markdown changed. Documentation layout/authority review applies. |

Risk owners are release scope for `01/02` and documentation ownership for `03`. Likelihood is material because prior rows already drifted; impact is unintended scope or invalid proof. Prevention is the single ledger and unchanged approved rows; detection is the mapped static/negative check. A failure holds discovery, corrects the affected disposition/link and reruns that check. Retirement evidence is the passing scoped report. Changed selectors, membership, defaults or affected source inputs invalidate the corresponding cells; unrelated revisions do not.

**`ITER-FR00-03` — PASS, planning closeout (2026-11-08).** Owner: release scope; input: clean `23eab7bdb` plus this documentation change. FR-00 is complete and its finished entries are retired from Roadmap. Rechecked `ITER-FR00-02`'s recorded PASS and the source-candidate export's SHA-256/178,616-byte identity against its [candidate record](../../artifacts/validation/releases/0.1.0/655f5fe8-fr01-20261031/candidate.json); no source or release acceptance is added. Advance `NS-SIMPLIFY`; preserve `PGE-01/13/15`, the six choices, dispositions, defaults and accepted PTD-00. The closeout check is removal of completed roadmap entries with evidence links and later owners preserved, strict UTF-8/local-link/anchor validation and `git diff --check`. Stale routing could reopen finished work; a failed check requires correcting the route before closeout. No production/build/runtime/source-style check applies to these three Markdown edits. Changed scope or candidate inputs reopen only their affected cells.

**Remaining / next:** no FR-00 work remains. FR-06 owns final candidate identity and changed-disposition checks; FR-07 owns public rights/signing. Those later obligations do not keep FR-00 open. Global next work remains [current Sponza recovery](../Strategy/Roadmap.md#where-we-are-and-what-is-next); reference recovery and Stage 10 retain their prerequisites.

## Required Rendering Closure

The release owner's 2026-10-10 amendment makes the following **Included delivery obligations**, not experimental substitutes. “Feature complete” means complete against each owning feature's frozen, finite contract and all applicable failure, performance, package and adoption checks. It does not admit every possible rendering technique, API extension or material domain. No source, SDK-presence or single screenshot claim closes a row.

| Required outcome | Release proof and existing owner |
| --- | --- |
| Reliable progressive Reference Path Tracer | `FCR-REN-08` and the [accepted transport contract](../Architecture/Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/TransportAndEstimator.md#claim-boundary): camera-originating progressive `SurfaceTransportReference`, raw linear radiance, independent analytic/minimal/external comparisons, converged uncertainty, correct accumulation/reset, invalid-sample rejection and reproducible exports. Interactive mode and raw export are Included in Editor; Game has no Reference selector, progress or export UI. Host-independent estimator, session and both-API checks remain required. A finite-depth diagnostic, denoised image or progress count cannot substitute for reference truth. Close every release-map material/light/camera/geometry domain mismatch before oracle use; explicitly distinguish excluded physical phenomena. Preserve accepted PTD-00 discovery and finish its existing recovery/Stage-10 work. |
| Realtime path tracer at >=30 FPS | `FCR-REN-06/07`, shared ray/surface owners and the [performance contract](#thirty-fps-performance-floor), using `REL-RT-1080P` on MIN-01. Freeze actual traced primary/secondary transport, finite-depth/reuse/approximation policy, quality thresholds and reconstruction topology before results. Existing Lit/indirect seed-replay source is a starting path, not an accepted realtime path tracer. Ray-traced GBuffer alone is insufficient. Static, motion/disocclusion and lobe/guide cases must agree with the accepted reference within the owned tolerances. |
| Complete DLSS Super Resolution and Ray Reconstruction | `FCR-REN-10`: Included on the named NVIDIA hardware for **both D3D12 and Vulkan**. Prove every exposed quality mode and SDK-supported SR/RR combination; exact pinned runtime/header/license, early initialization, viewport/frame identity, exposure/depth/motion/jitter/normal/roughness/albedo/specular inputs, extent/reset, state restoration, queue/resource lifetime, failures and Shipping dependency closure. Optional SDK features outside the declared renderer domain are not implicitly admitted. A visible Linear/Off fallback is safe recovery, never a pass for a required provider cell. |
| Finished exposure and tone mapping | `FCR-REN-09/14/15`: all exposed manual/automatic/metering and Reinhard/ACES approximation/ACES fitted choices, finite/range/step response, view isolation, reset, provider input domains, exactly one tone/output transform, alpha and SDR/HDR handoff. No post-process adjustment may conceal transport/units defects. |
| Basic chromatic aberration and vignette | `FCR-REN-25` now covers two independently verified lens effects: the [chromatic contract](../Architecture/Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/ChromaticAberration/ChromaticAberrationDesign.md) and [basic vignette](../Architecture/Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/Vignette/VignetteDesign.md). Both Included; neutral settings erase effect work, raw reference/exact diagnostics/UI bypass effects, controls/reset/persistence and both-backend numerical/order/cost checks pass. Neither effect inherits the other's verdict. |
| D3D12/Vulkan implementation parity | Same declared release modes, features, settings semantics, scene/camera/pose, providers and products on both APIs. Retain native validation and paired raw/temporal/display/performance/package evidence. Compare within predeclared numeric/statistical tolerances, not universal bitwise identity. A genuinely vendor-specific excluded feature such as PTLAS does not weaken parity for an Included feature; an unavailable required provider remains a blocker, not a quiet exclusion. |
| Complete frame graph for the declared frame | `FCR-REN-03` and [AC-FGS-01–08](../Architecture/Modules/Engine/Renderer/Features/FrameExecution/FrameGraphAndScheduling.md#acceptance-criteria): dependencies/imports/export roots, culling, transient alias/initialization, state/UAV/queue waits, typed pass binding, serial/parallel recording equivalence, histories/rebuilds, external-provider work and token-safe retirement. Exercise negative compilation/recording/submission/lifetime cases. Native clean output alone cannot prove useful overlap or bounded resource/copy cost. |
| Principal-level review of the entire frame | The [review package below](#principal-frame-review) is mandatory before `REL-04`/`REL-07` closure. Review the complete production frame and its failures, not just chosen shaders or a capability checklist. |

Deferred decals, Color Grading and HDR10 remain their existing mandatory obligations; Frame Generation and Volumetric Lighting remain excluded. The amendment does not expand PTD-00's physical transport domain; the seven approved examples plus Empty retain their independent rights/material/reference checks.

### Principal Frame Review

The requested quality standard is a frame that can withstand detailed principal-rendering review. Freeze the packet and criteria below, then retain the actual author/agent reviewer, candidate/artifact hashes, findings and disposition. An external reviewer is optional. A claim that an NVIDIA engineer reviewed the frame still requires that person's confirmed identity and actual review; an author/agent pass cannot claim it. No outreach is authorized by this documentation work.

The packet contains:

1. One frame lineage from authored scene/pose/camera through cook identities, uniforms/material bindings, graph declaration/compilation, recording, submission, temporal/provider evaluation and final present. Each pass identifies inputs/outputs, units/color space, resource lifetime, queue and owner.
2. Native captures on both APIs, with exact scene/frame/target identity, complete pass/resource/pipeline inspection and shader/binary provenance; capture runs are separate from performance controls.
3. Estimator derivations and limits, independently qualified raw reference/convergence artifacts, realtime error/temporal/disocclusion cases and genuine provider guides. Document approximations rather than rely on reservoir or SDK naming.
4. Barrier/wait/alias plan and controlled failures, serial/parallel equivalence, resize/rebuild/provider/shader changes, history invalidation, retirement bounds and shutdown/device-loss disposition.
5. CPU/GPU/present measurements for the named native and realtime presets, pass/copy/allocation costs, memory high-water, recording topology and useful-overlap evidence. Before/after optimization claims use the same candidate inputs and fixed observer protocol.
6. Ownership/dependency/hook and copy ledger, discarded/replaced paths, API-private mechanics, typed feature boundaries, and clean format/readability review under Engineering standards. Frame orchestration must remain readable; no duplicate authority or generic-host provider policy.
7. Source/build/profile/package/Shipping evidence and reproducible consumer/source instructions, with known limitations and exact unresolved cells.

Acceptance requires no unresolved release-critical correctness, lifetime, synchronization, numerical, provenance, parity or performance finding, and a recorded disposition for every finding. Missing reviewer access leaves this gate unrun; it does not prevent dependency-ready development. Changing reviewed frame inputs or code invalidates affected review evidence.

## Release Roles

One person may hold several roles, but the recorded acceptance run MUST be performed by someone who did not prepare the candidate on that machine.

| Role | Decision owned |
| --- | --- |
| Release owner | Scope, supported claims, severity/waiver decisions, candidate go/no-go, publication, and finite release-review boundary. |
| Build/package owner | Reproducible build, stage/install tree, dependencies, manifest, hashes, archive, and symbol retention. |
| Feature owners | Included/experimental/excluded classification, focused correctness/failure evidence, defects, and invalidated-gate reruns. |
| Content/provenance owner | `ReleaseMapSet`, source identity, license/attribution/redistribution record, cook completeness, and PBR/reference review. |
| Evidence reviewer | Measurement contract, raw-artifact integrity, native validation disposition, comparison thresholds, and honest limitations. |
| Security/trust owner | Threat model, dependency/SBOM and vulnerability disposition, signing-key and publication-account controls, signature/attestation verification, and private security intake. |
| Release-review owner | Local diagnostic/privacy instructions, candidate-to-symbol correlation, known-defect review, immutable replacement/withdrawal and `REL-11` closeout; no ongoing public support service. |
| Independent acceptor | Clean consumer and source-adopter public-instructions runs and reproduction reports; does not repair the candidate during acceptance. |

The release owner resolves disagreement by reducing scope or holding the release, never by relabelling missing evidence as a pass.

## Consumer Journey Acceptance

The expected user journey starts through Launcher and is accepted only from the candidate bundle and public instructions. Direct child launch remains a diagnostic/automation check, not a required user action:

- the download page states version, archive size, installed size, free-space requirement, system requirements, supported GPUs/drivers/APIs, known issues, checksum, signer/publisher, and explicit no-support statement before download;
- every shipped `.exe` and `.dll` has an expected trusted Authenticode signature and publisher; signature verification and current Windows Smart App Control/SmartScreen behavior are recorded on a clean machine;
- extraction and first launch require no administrator rights, registry edit, environment variable, developer SDK, compiler, source checkout, network connection, or writable installation directory;
- first launch reaches responsive Launcher/Editor UI or intentional Game rendering/input within the startup budget. A selected prebuilt example activates from its same-release cooked dependencies under the requested identity, without a Game setup/menu UI or fallback to successful Empty;
- Launcher selects any subset of the seven examples, including none, without source sync/cook for the prebuilt path; optional source acquisition/rebuild remains available; Editor/Game activate each requested ready map and advertised backend/mode. Development/unavailable choices are labelled and cannot appear release-ready. A developer-only environment variable is evidence plumbing, not consumer UX;
- Launcher/Editor and accompanying instructions explain Game camera input/native close, scene/API selection and optional Editor settings/reset, capture and HDR requested/active/fallback state where advertised. Game has no help/settings/overlay frontend. Unsupported input/display/accessibility configurations remain explicit;
- settings persist under the declared per-user root, survive a normal restart, reject or reset corruption safely, and can be reset without deleting the installation;
- the runtime makes no undeclared outbound connection and does not collect or upload logs, dumps, hardware identity, or usage data without explicit consent;
- normal removal deletes only installed files. Per-user data retention/deletion is documented and never removes unrelated data.

The first-run record includes screen capture, elapsed startup/map-load/exit times, process exit status, created/modified filesystem paths, network observation, signature results, and the exact package hash.

## Source-Adopter Acceptance

The engine-adopter journey is accepted separately from the binary package:

- a fresh tagged clone or source archive verifies the release identity and contains no generated build products, private credentials, local machine paths, or unlicensed payload;
- every fetched dependency is pinned to an immutable commit or archive hash, appears in the dependency/provenance inventory, and has a resolved license/notice disposition;
- the documented Windows, compiler, SDK, CMake, generator, Git, Qt, Vulkan, and optional-vendor prerequisites distinguish required, optional, and auto-fetched inputs;
- empty dependency/build/cook outputs can perform the documented configure/build/full cook and Showcase run from public inputs without private knowledge. Each published version uses fresh generated outputs; a warm cache is not release proof or a required user prerequisite;
- the matching precompiled products and cooked base can be imported into a fresh source workspace, then a real code/asset/shader edit is rebuilt through Launcher and changes the observed product without a per-file version tracker;
- one minimal command path builds the Game; optional editor, launcher, import/cook, ray-tracing, and vendor-provider paths are separately selectable and fail at configure time when prerequisites are absent;
- `Showcase` is the supported sample project. The instructions identify where to add a project, where generated/cooked data lives, how to clean it, and which interfaces are stable or deliberately unstable;
- warnings, failed downloads, missing tools, unsupported CPUs/GPUs, bad cached dependencies, and cook errors stop with actionable ownership and recovery instructions;
- the author or agent completes the journey and reports elapsed time, download/build/disk cost, commands, failures, and one technical criticism.

No binary SDK, plugin ABI, installed headers/libraries, or compatibility promise may be inferred from source-export macros. If those are excluded, the release page says so plainly.

## Compatibility Boundary Created By Shipping

Publishing creates the first installed consumers and persisted user state. Before `REL-10`, update the [current clean-break policy](../Engineering/Workflow/ChangeIntegration.md#current-clean-break-policy) so it no longer claims those consumers do not exist and so future changes know which external boundaries are durable.

The decision MUST classify:

- release/tag/package identity and the immutable bytes of every published version;
- command-line options, package manifest, map IDs, user-facing settings, file locations, and support-data schema;
- persisted configuration/cache/capture/crash data as preserved, resettable, disposable, or unsupported across patch/minor releases;
- source API stability during `0.y.z` and the explicit absence or presence of a binary ABI/plugin/SDK contract;
- patch, minor, rollback, downgrade, and side-by-side behavior, including how a bad configuration or cache is recovered;
- explicit absence of a user support service or maintained-version window; any voluntarily delivered correction gets a new immutable identity rather than mutating an old release.

`v0.1.0` may declare its source API unstable, but that does not permit replacing published `v0.1.0` bytes, corrupting consumer data, or silently changing its documented behavior. This planning document does not authorize compatibility machinery before the owning policy is approved.

## Supply-Chain, Security, And Privacy Acceptance

- Record a first-release threat model for the package boundary, mutable user state, manifest/catalog/content parsers, DLL/process loading, optional providers, build downloads, signing keys, publication account, and support/crash data. Each trust boundary has an owner, mitigation, controlled check, and residual-risk disposition.
- Freeze every compiler, SDK, dependency, optional provider, model/data artifact, external map archive, license, and build-script input by immutable identity. Moving branches such as `master`/`main` block release reproducibility.
- Produce an SBOM or equivalent machine-readable component inventory for the runtime and a broader source-build dependency inventory. Include name, version/commit, source, hash, license, linkage/distribution role, shipped files, and owner.
- Review known vulnerabilities for shipped/reachable components. Known exploited or critical issues block; a high-severity waiver requires reachability analysis, mitigation, owner, expiry, and public impact statement.
- Scan source and staged output for credentials/private keys, unexpected executables, malware, debug/provider DLL contamination, absolute developer paths, and files absent from the allowlisted stage manifest.
- Sign first-party Windows PE files with the approved trusted identity, timestamp where applicable, then verify every first- and third-party PE after staging. Signing occurs after final binary mutation; the verified hashes become candidate identity.
- Verify the intended Shipping compiler/linker mitigations and PE flags for every first-party executable/DLL, including stack protection, DEP/NX compatibility, ASLR/high-entropy VA, and Control Flow Guard where supported. Record any disabled protection and rationale; keep PDB/private source paths outside the Game archive.
- Restrict DLL and child-process resolution to trusted application/system or explicitly registered directories. Package manifests, catalogs, settings, and command-line/environment paths must reject traversal, root escape, unexpected reparse targets, oversized input, and untrusted binary side-loading before use.
- Publish cryptographic hashes and verifiable build provenance. When GitHub Actions produces the release, publish and independently verify an artifact attestation; an attestation proves provenance, not software safety.
- Runtime networking is deny-by-default for `v0.1.0`. Any required endpoint, payload, authentication, retention, retry, offline behavior, and privacy notice must be explicitly approved and tested.
- Publish a truthful no-support/no-response-guarantee statement, known issues and privacy-safe local diagnostic instructions. No public/private reporting service is required. Any optional reporting route must be separately approved and must not imply an acknowledgement or fix deadline.
- Retain release artifacts, symbols, provenance, SBOM, notices, validation summaries, and signing records so a later vulnerability or crash can be reproduced without altering the public release.

## Release Map Set

The shipped map set is a curated product surface, not every catalog entry. Each member MUST be runnable offline from package-relative files and have a verified redistribution/license record. Large third-party workloads may remain reproducible downloads and acceptance inputs without being redistributed in the archive.

The minimum release set covers these roles:

| Role | Current candidate | What it must expose |
| --- | --- | --- |
| Empty/control | `Empty` | Startup, clear/output encoding, camera/input, shutdown, and no-content behavior. |
| Compact metallic-roughness PBR | `DamagedHelmet` | Base color, normal, metallic, roughness, occlusion, image-based lighting, and framing. |
| Skinning/animation | `CesiumMan` | Skeleton/skin data, animation, transforms, temporal stability, and authored materials. |
| Alpha/two-sided/transmission | `DiffuseTransmissionPlant` | Alpha handling, double-sided policy, normals, shadows, and layered foliage response. |
| Lighting/reference | `CornellBox` or `Sponza` | Direct/indirect light, shadowing, exposure, tone mapping, and reference comparison. |
| Representative scene | one legally cleared broader scene | Geometry, material variety, residency, traversal, load time, and stable 30 FPS delivery. |

The [scope record](#releasemapset) owns the current seven-map/default selection and deferred complexity. The rows above are workload examples, not a second map set. `REL-01` admits selected bytes only after provenance, embedded license, attribution, modification and redistribution review. Bistro, San Miguel, Modern Sponza, LPS Head, and other downloaded packs MUST NOT be bundled merely because the catalog can acquire or run them. If rights are incomplete, publish an acquisition recipe and hash instead of the asset bytes.

## Per-Map Acceptance Criteria

Every `ReleaseMapSet` member completes the canonical `MAP-A` through `MAP-H` flow in [Bistro and San Miguel Acceptance Workloads](GraphicsWorkloads.md#per-level-checkpoint-stages), then meets the additional release criteria below from the staged candidate package.

### Content And End-To-End

- A clean cook produces every declared scene, mesh, material, texture, animation, and shader product with zero uncategorized errors.
- The staged archive launches offline as a standard user from a path containing spaces and from a non-repository directory.
- The requested map becomes active, reaches the authoritative settled state, renders the frozen camera/route, switches away and back where supported, and exits normally.
- The public first-run/default route enters a meaningful accepted example, and the public selector reaches every other release map without an environment-variable-only workflow.
- Removing or corrupting the catalog, requested map, cooked scene, texture, or shader product rejects that map with a named actionable error. It MUST NOT fall back to `Empty` or plausible default output and report success.
- Logs contain no missing asset, missing shader, failed pipeline, device removal, fatal diagnostic, or silent fallback. Every allowed warning is listed by stable signature and justification.
- Package files remain immutable. Runtime-generated cache, settings, captures, logs, and crash artifacts land only under their documented per-user locations.

### Artifact-Free Visual Result

An “artifact” is a visible or numerical defect not present in the accepted reference or intentionally documented presentation. Examples include missing or corrupt geometry, texture fallback, UV/tangent seams, inverted normals, NaN/Inf pixels, fireflies beyond the declared reference policy, shadow acne/light leaks, alpha sorting/cutout errors, skinning explosions, stale history, ghosting, disocclusion trails, flicker, banding, unintended clipping, double encoding, unstable exposure, or backend-specific divergence.

For every frozen view and included backend:

- zero unresolved severity `S0` or `S1` visual defects are permitted;
- geometry, transforms, normal/tangent orientation, UVs, material bindings, alpha/double-sided state, animation, lights, shadows, exposure, tone mapping, and output encoding have explicit `Pass`/`Fail` observations;
- base color, normals, metallic, roughness, ambient occlusion, emissive, alpha, and transmission fields are inventoried where applicable and compared with authored intent;
- albedo/base-color, world-normal, roughness, metallic, depth, motion, direct-light, indirect-light, and final-output debug views are captured where applicable;
- the lit result is compared with a declared high-sample renderer path or a named external/reference image using identical camera, exposure, output transform, and crop assumptions;
- each map freezes objective full-frame and region-of-interest image-error metrics and thresholds before candidate measurement; the record names the metric/tool version, reference/candidate inputs, masks, and excluded pixels;
- temporal inspection covers camera motion, animation, occlusion/disocclusion, scene switch, and at least 300 settled frames;
- all comparisons preserve original captures, thresholds, excluded pixels, and reviewer disposition. A pleasing screenshot is not proof.

Defect severity is release impact:

| Severity | Definition | Release treatment |
| --- | --- | --- |
| `S0` | Crash, hang, data loss, security issue, device removal without controlled failure, or package cannot start/load/exit. | Always blocks. |
| `S1` | Wrong image/content, common-path corruption, major PBR error, recurrent severe hitch, unsupported silent fallback, or broken primary workflow. | Always blocks. |
| `S2` | Bounded defect with a practical workaround that does not invalidate the advertised path. | Fix or explicitly waive with owner, evidence, user impact, and target release. |
| `S3` | Cosmetic or documentation issue that does not misrepresent behavior. | May ship when recorded. |

### Thirty-FPS Performance Floor

The first release floor is 30 FPS for every shipped example map on the named minimum machine. Because FPS alone hides stalls and CPU/GPU attribution, acceptance uses frame times:

- `ShippingGame` on the named laptop, fixed 1920x1080 benchmark output: Lit uses native input; realtime uses the predeclared DLSS Quality/RR topology and recorded SDK-derived internal extent. User viewing resolution follows the selected display/mode and is not forced by this benchmark. Include approved chromatic aberration/vignette On with fixed effect strengths; record output/internal/composition extents. Generated frames, dynamic resolution, silent fallback and post-result quality changes cannot satisfy the target. Use VSync off, fixed camera/route and validation disabled;
- at least 300 post-settle warm-up frames followed by at least 300 valid measured frames, repeated three times per map/backend;
- presented/application frame-time p95 is at or below 33.33 ms; CPU and GPU p95 are recorded separately and neither may exceed 33.33 ms;
- p99, worst frame, one-percent-low FPS, load time, shader/pipeline warm-up, process memory, tracked GPU memory, local/non-local budget, and excluded samples are reported rather than hidden;
- no recurring hitch cluster, memory growth across repeat loops, thermal throttling, dynamic-quality change, or background compilation may make the pass misleading;
- D3D12 and Vulkan are measured separately. An API may be excluded at scope freeze, but an advertised API may not borrow the other API's result;
- a lower-spec system is reported as informational until it is added to the supported matrix.

Raster/hybrid and ray/path research claims retain the stricter mode-specific resolutions, timing distributions, and capture contract in the [canonical workload](GraphicsWorkloads.md#performance-contract). Reference rendering is evaluated for deterministic convergence and quality, not forced into the real-time floor unless it is selectable as an advertised real-time mode.

## Failure-Mode Acceptance Matrix

Positive runs prove only the happy path. Each applicable failure contract MUST be exercised with a controlled copy, VM, OS facility, or temporary local harness. Fault injection must not mutate the approved candidate, trusted release archive, repository source, or unrelated user data. Permanent test code still requires explicit authorization under the [submitted-test policy](../Engineering/Verification/ValidationAndEvidence.md#submitted-test-code).

| ID | Failure class | Controlled setup | Accepted behavior | Evidence and release consequence |
| --- | --- | --- | --- | --- |
| `FM-REL-01` | Unsupported OS/CPU/GPU/driver/API | Run one named unsupported or below-minimum configuration; request an invalid backend token/capability. | Reject before partial renderer/content startup; identify missing requirement and supported recovery. No unknown-backend crash or silent API/provider switch. | Message, exit code, log, no child/leaked process. Silent fallback or crash is `S1`/`S0`. |
| `FM-REL-02` | Signature/integrity failure | Verify normal package, then alter a copied PE/archive byte or checksum. | Normal bytes verify to expected publisher/hash; tampered copy fails verification and is never promoted. | Signature/hash/attestation logs. Unexpected signature state or accepted mismatch blocks `REL-10`; unsigned internal state is explicit. |
| `FM-REL-03` | Missing runtime dependency | Remove each allowlisted first- or third-party DLL from a copied stage. | Startup fails once with the missing component and recovery link; no ambiguous loader disappearance or partial product. | File-specific result and process tree. Unnamed failure is `S1`. |
| `FM-REL-04` | Path traversal or binary side-load | Put a copied unsigned lookalike DLL on the current/PATH directory; add `..`, absolute, reparse-point, and outside-root paths to disposable manifest/catalog/settings inputs. | Load only allowlisted trusted binaries and canonical package/per-user paths; reject root escape and unexpected reparse traversal before file use. | Loaded-module and file-access inventory. Executing or reading an attacker-controlled outside-root target is `S0`. |
| `FM-REL-05` | Missing/corrupt manifest, shader, or map | Remove/truncate each required class in a copied package. | Reject the requested product before publication/use; do not render `Empty`, defaults, stale bytes, or a different backend as apparent success. | Expected failure signature and zero false pass. Plausible output is `S1`. |
| `FM-REL-06` | Unwritable install/user path or disk full | Make installation read-only; deny/create-low-space per-user output location. | Immutable product still runs when user storage is available. A required-write failure names the path/space and exits or disables only the optional operation without corrupting state. | Created-file inventory, error, previous-file hash. Corruption/data loss is `S0`. |
| `FM-REL-07` | Corrupt/stale settings or cache | Truncate, edit out of range, or substitute a stale copied settings/cache file. | Validate before apply; reset/quarantine disposable state through the documented path; preserve last-known-good user settings when promised. | Before/after hashes and user message. Crash, unsafe value, or hidden wrong rendering is `S0`/`S1`. |
| `FM-REL-08` | Optional GPU/provider unavailable | Run without the vendor feature, on unsupported hardware, and with its DLL removed when the provider is excluded. | Select policy before scheduling: use the one advertised fallback or mark unavailable. UI/requested/active state agrees; no provider DLL is required for an excluded feature. | Capability/provider matrix and active-state capture. Silent quality change is `S1`. |
| `FM-REL-09` | Device removal or graphics initialization failure | Use supported diagnostic injection/tooling where safe; preserve a real incident when available. | Terminate or recover only as advertised; flush actionable DRED/Vulkan context, build/map/backend identity, and support instructions without hanging. | Log/dump/breadcrumb record. Hang or context-free device loss is `S0`. |
| `FM-REL-10` | CPU/GPU memory pressure | Exercise the heaviest release map near the declared minimum budget and use safe low-resource simulation where available. | Stay within the frozen ceiling or fail a bounded operation before OS/device instability; no unbounded growth, thrash loop, or corrupt scene publication. | Time-series/high-water and rejection identity. OOM/device removal is `S0`; recurring severe thrash is `S1`. |
| `FM-REL-11` | Window/display/input lifecycle | Resize, minimize/restore, alt-tab, DPI/monitor move, supported display-mode change, focus loss, and quit during load. | Preserve or safely recreate presentation state, resume input, cancel/complete owned work, and exit without stale frame, inversion, hang, or device removal. | Screen/log/timing record. Broken primary control or output is `S1`. |
| `FM-REL-12` | Network unavailable or unexpected | Block outbound traffic and observe runtime connections. | Shipped examples run offline; no undeclared endpoint or repeated retry stall. Approved online operations fail with bounded actionable state. | Network observation and retry duration. Undeclared collection/connection is `S0`/`S1` by impact. |
| `FM-REL-13` | Crash/hang/support capture | Use an approved non-production fault or OS verifier on a disposable candidate copy. | Produce the declared WER/local dump or support bundle with version/build/backend/map/log identity and consent boundary; no secret or unrelated user data. | Dump/bundle inventory and symbolization attempt. Missing actionable identity blocks diagnostic acceptance. |
| `FM-REL-14` | Source dependency/build failure | Start with empty cache; block network, corrupt a copied cache, omit a tool, or provide a moving/mismatched dependency revision. | Configure/sync fails before compilation with dependency/tool identity and recovery. Warm offline cache accepts only verified pinned inputs. | Raw sync/configure logs and cache hashes. Using unexpected source is `S1`. |
| `FM-REL-15` | Publication mismatch | Download every draft/final asset from the intended public route and compare tag/version/hash/size/signature. | Any mismatch stops or withdraws publication; never edit bytes beneath an existing version. | Retrieval log and release-page snapshot. Mismatched public bytes are `S0`. |
| `FM-REL-16` | Patch/rollback/side-by-side | Install/extract candidate with an older supported copy and representative user state according to scope. | Follow the frozen preserve/reset/reject policy; do not corrupt either copy or claim downgrade support when absent. | File/state diff and versioned result. Undocumented destructive interaction is `S0`. |

Every negative check records its `FM-*` and `CHK-*`, expected failure, observed failure, exit status, bounded duration, created/modified files, logs/dumps, and cleanup. A negative test passes only when it detects the injected defect and the product response matches the contract.

## Subsystem Feature-Closure Matrix

`REL-04` creates one row for every current user-visible capability and subsystem and one candidate-bound report under the [feature completion report contract](FeatureCompletionReports.md). A subsystem may be marked `Verified` only when every applicable item below links to evidence; “compiled” alone means `Integrated`.

| Owner/surface | Required closure evidence |
| --- | --- |
| Build and dependencies | Versioned Shipping configuration; compiler/SDK/dependency lock with no moving refs; clean empty-cache and warm/offline-cache configure/build; warnings policy; dependency licenses/SBOM; logical package reproducibility; no undeclared runtime binary. |
| Core and platform | Supported OS/CPU detection; standard-user startup; Unicode, spaced, and declared-length paths; canonical root/reparse checks; safe DLL/process resolution; immutable package and per-user writable roots; clock/input/window/DPI behavior; clean exit; actionable unsupported-platform failure. |
| Tasks and lifecycle | Startup/shutdown ownership; cancellation and destruction with work in flight; 1/2/N worker behavior where applicable; bounded queues/memory; no thread, handle, or process leak over repeat runs. |
| Application and world | Prebuilt first run/default map; Launcher scene/API selection; Game rendered frame/camera/focus/native close with no ImGui UI; shared settings consumed from defaults/Editor; deterministic level activation, lifecycle, resize/minimize/restore/alt-tab, safe failure and repeated launch/load/exit. |
| Projects and content | Catalog truth; packaged-versus-download-only content; redistribution record; startup-level selection; explicit required-content failure; no release option that targets absent content or silently falls back to `Empty`. |
| Source import | Clean supported-format import; provenance and hashes; deterministic semantic inventory; transforms/materials/animation fidelity; malformed/missing input rejection. |
| Cooking | Clean scene/mesh/material/texture/asset cook; deterministic products or explained nondeterminism; bounded work/memory; stale and incompatible product rejection; no repository-relative dependency in the package. |
| Shader delivery | Clean shader cook for the release matrix; program/library/ABI identity; missing/incompatible artifact rejection; cold and warm pipeline behavior; no runtime dependency on source shaders/compiler; Shipping erasure of recook, symbols, and private diagnostic paths. |
| D3D12 RHI | Debug layer and GPU-based validation on focused workloads; descriptor/resource/state/queue lifetime; timestamp/capture path; resize; capability rejection; DRED evidence for device-removal incidents. |
| Vulkan RHI | Core, synchronization, GPU-assisted, and best-practices validation where supported; queue/resource/swapchain lifetime; timestamp/capture path; resize; capability rejection; zero uncategorized validation findings. |
| Renderer | Each included view mode, lighting path, ray/raster route, reconstruction/upscaler/provider, exposure, tone mapper, post-processing stage, presentation/output path, and debug view has a feature row, compatible-map result, backend disposition, requested/active/fallback state, quality evidence, and performance cost. Deferred decals, Color Grading, Chromatic Aberration, and HDR10 output must pass `FCR-REN-23` through `26`; Frame Generation and Volumetric Lighting remain explicitly excluded; development-only console/diagnostic surfaces are absent or classified. |
| Editor | Required shipped product: clean-machine start; viewport/camera/settings/edit/capture workflows; save/generated-content ownership; user-visible support matches actual behavior. |
| Launcher | Required shipped product: source/toolchain discovery; configure/build/cook/run; asset-pack sync; progress/cancel/failure; selectable scope matches support; no repository-only path assumptions; child-process handoff and shutdown. |
| Third-party providers | Exact feature/provider need; immutable SDK/runtime identity; license and redistribution; release-only DLL allowlist; signature/publisher; unsupported-hardware and missing-provider behavior; no debug/provider payload leakage. |
| Runtime package | Dependency closure; package-relative immutable content; per-user writable state; offline start; clean-machine consumer journey; trusted PE signatures; manifest, hashes, provenance, SBOM, and allowlist; no authoring/training/source/debug assets; extraction/removal behavior. |
| Source release/adopter | Immutable source tag/archive; public prerequisite and dependency sync; clean build/cook/run; supported sample project; source API/ABI statement; no private path/tool/credential; documented reproduction. |
| Crash/security/support | Local process/hang/device-loss diagnostics, candidate symbol correlation, consent/privacy, known issues and explicit no-support statement. Release-side vulnerability disposition and immutable replacement/withdrawal remain required; no intake service or response clock. |
| Documentation/support | Runtime and source-adopter quick starts; requirements; controls; map/mode/provider matrix; API/compatibility policy; notices/SBOM; release notes; known issues; troubleshooting; integrity verification; crash/security/reporting instructions; privacy; explicit absence of ongoing support. |

The feature-closure record MUST include owner, public surface, classification, supported configurations, positive case, negative/failure case, evidence link, open defects, last evidence revision, and approval. If a current feature cannot meet this bar, exclude it from the release surface before proceeding.

## Candidate Stability Matrix

The candidate package, not a development-tree executable, is the subject of final acceptance.

- Cold launch, default-map settle, each release-map load, and clean exit: 10 consecutive loops per included backend.
- Map switch/reload loop covering every release map: 30 minutes without unbounded memory growth, stale content, crash, hang, or device removal.
- Fixed representative traversal: 30-minute soak per included backend while collecting frame time and memory high-water.
- Publisher-prepared first run; Launcher scene/API selection and feedback; Game rendering/camera/native close without ImGui UI; Editor controls/help/settings/reset/capture and each advertised error/recovery action.
- Resize, minimize/restore, alt-tab, focus/input recovery, supported full-screen/window modes, supported DPI/scaling, monitor changes, and the required SDR/HDR transition and fallback matrix.
- Keyboard/mouse and every advertised controller path; English plus one non-dot-decimal Windows locale; Unicode user and extraction paths.
- Offline use with outbound traffic observed and blocked, standard-user account, non-system drive where available, read-only installation directory, and full/disk-write-denied mutable root.
- First run with empty caches and warm repeat run; pipeline/shader compilation behavior is visible and bounded.
- Minimum and reference CPU/GPU/driver combinations, plus integrated/discrete or hybrid-GPU selection when the supported matrix includes them. Unsupported hardware and stale drivers must reject before partial rendering begins.
- Trusted-signature verification for every PE file, clean-machine Smart App Control/SmartScreen observation, antivirus/malware scan disposition, archive/hash verification, and detection of one deliberately modified candidate copy.
- Every controlled failure in the [failure-mode acceptance matrix](#failure-mode-acceptance-matrix), including missing/corrupt content, settings/cache corruption, optional-provider absence, device removal, memory pressure, and publication mismatch.
- Crash or device-removal reproduction captures build ID, backend, adapter/driver, active map/mode, logs, and diagnostic breadcrumbs without collecting undeclared private data.
- One clean source-adopter run from the tag with empty dependency/build/cook caches, one warm rebuild, and one deliberate prerequisite/dependency-integrity failure. Private caches or local patches may not rescue the accepted route.
- Microsoft Application Verifier Basics and the applicable limited-resource/limited-user checks for the unmanaged Windows product where the selected toolchain supports them. These are diagnostic runs, never performance measurements, and every finding needs a disposition.

A failed loop invalidates the affected candidate. Fixes produce new candidate bytes and require rerunning every gate touched by the change.

## Release Evidence Package

Retain raw evidence under:

```text
artifacts/validation/releases/<version>/<candidate-id>/
    manifest.json
    release-scope.md
    iteration-control.md
    traceability.csv
    risk-register.md
    feature-closure.csv
    features/<feature-id>/completion.md
    environment/
    build-cook-package/
    source-adopter/
    maps/<level-id>/<backend>/
    performance/<level-id>/<backend>/
    native-validation/<backend>/
    stability/
    consumer-journey/
    failure-modes/
    compatibility/
    security/
        sbom/
        signatures/
        provenance/
        scans/
    filesystem-network/
    support/
    legal/
    known-issues.md
    approval.md
```

`manifest.json` binds the commit, dirty state, configuration, toolchain, dependency revisions, content/artifact hashes, feature/map matrix, hardware/driver matrix, command lines, start/end time, operator, and result. Large disposable captures remain outside version control; intentional summaries and small reviewer artifacts may be promoted to documentation. The release archive itself contains user-facing notices and instructions, not internal raw validation data.

## Approval Rules

- No release evidence is collected from a dirty tree unless the manifest names and hashes every deliberate patch; the final candidate MUST come from a clean tagged commit.
- Zero open `S0` and `S1` defects are permitted. An `S2` waiver names owner, user impact, workaround, evidence, and target release.
- “Works on my machine,” responsive process state, one screenshot, average FPS, a successful compile, or a historical run never closes a gate.
- A warning is not waived by repetition. It has a stable identity, owner, cause, impact, and disposition.
- Every pass names the exact platform, backend, build, content, map, camera/route, and candidate hash. Unsupported or unavailable configurations are reported, not simulated.
- Both required audiences pass: binary consumer acceptance cannot substitute for source adoption, and a source build cannot substitute for the signed package journey.
- Every included or experimental feature has an approved completion report that traces the exact product path and passes every applicable polish dimension; report length, source presence, or a family-level summary cannot substitute for evidence.
- The candidate traceability ledger has no orphan `NS-*`, `PGE-*`, `REL-*`, `RISK-*`, `FCR-*`, `AC-*`, `FM-*`, `CHK-*`, result, artifact, defect, or waiver; every risk accepted or retired names the approving owner and evidence.
- A controlled negative test passes only when the product or verifier detects the injected fault and produces the contracted user action. Merely surviving an invalid state is not a pass.
- Every release input is immutable and in the SBOM/provenance record; every packaged PE is expected and signature-verified; every outbound connection and mutable filesystem path is declared.
- Direct public Windows distribution is blocked by an unsigned, self-signed, untrusted, expired, mismatched, or unexpectedly signed PE. Any accepted channel-specific alternative must be frozen at `REL-00` and proven end to end.
- The installed-consumer compatibility, persisted-data, update, rollback, no-support, and local-diagnostic/privacy rules MUST be effective before `REL-10`; publication cannot rely on the repository's pre-consumer clean-break premise.
- Scope may shrink to reach a trustworthy release. Acceptance thresholds may not be weakened after observing a failure without recording a reviewed contract change and rationale.
- The release owner gives the final go/no-go only after an separate reviewer completes `REL-09`.
- `REL-10` proves publication; it does not unlock new features. Only the approved `REL-11` stabilization closeout does.

## Reference And Ownership

<details>
<summary>Related authorities and evidence routes</summary>

**Responsibility:** define the complete evidence and approval boundary for the first public SparkleEngine release

**Review the approved release:** start with the [six settled choices](#approval-and-next-work) and [FR-00 discovery result](#fr-00-reconciliation-result). The [capability ledger](ReleaseScope/CapabilityDispositions.md) owns dispositions. [FR-01](#fr-01-source-requalification) records the source-candidate scope export; final candidate recheck, rights and executable release gates remain open without reopening accepted discovery.

**Roadmap and sequence:** [F. Release-First Principal Graphics Roadmap](../Strategy/Roadmap.md)

**Implementation orchestrator:** [First Release Implementation Plan](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md)

**Detailed graphics workloads:** [I. Bistro and San Miguel Acceptance Workloads](GraphicsWorkloads.md)

**Validation policy:** [Validation, Performance, and Evidence](../Engineering/Verification/ValidationAndEvidence.md)

**Per-feature polish and explanation:** [First Release Feature Completion Reports](FeatureCompletionReports.md)

**Release risk and iteration order:** [Release Risk Register](../Strategy/Roadmap.md#release-risk-register)

</details>
