# Packaging And Installation

[Docs](../../../README.md) / [Up](BuildAndPackagingCapabilities.md)

**Status:** current source contract; bounded source checks recorded, new-archive/public qualification open

**Scope:** define release inputs, deterministic packaging, clean-machine operation and local diagnostics/replacement

**Owner:** repository build and release-delivery configuration; product contributors are Launcher, Showcase, Application, Tools, and the engine modules whose runtime bytes enter the package

**Strategy and acceptance sources:** [`PGE-01`, `PGE-13`, `PGE-14`, `PGE-15`](../../../Strategy/Requirements.md), [First Release](../../../Acceptance/FirstRelease.md), and `FCR-PROD-02`/`FCR-PROD-05` in [Feature Completion Reports](../../../Acceptance/FeatureCompletionReports.md)

**Acceptance:** `FCR-PROD-05`/`REL-03` pass the admitted unsigned internal `a0f34c8b...` scope. Assembly/symbols, installed failures/recovery and positive reconciliation pass; the user accepts the selected extraction remaining retained, so actual removal is `EXCLUDED`. Earlier `e771a244...`/`3743e72a...` receipts keep their identities and accepted-laptop limits. See [candidate results](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress); [Product reconciliation](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) retains technical passes but blocks closeout on source-applicable scene-publication coherence. Those results retain the earlier selected-content/store scope. The 2026-11-01 source cleanup below has its own checks; a new archive still needs affected product proof and rights/signing admission.


Use [reviewed inputs](#reviewed-package-inputs) to prepare the package, [package command](#current-package-entry) to assemble it, [package checks](#fcr-prod-05-package-contract) to qualify it, and [local diagnostics](#local-diagnostics-and-release-review) for incident/export/replacement behavior.

## Prebuilt Base And Source Rebuild

**Approved target, 2026-11-01:** keep Launcher as the polished way to select, inspect and run the renderer, and as the existing frontend for users who choose to modify source and rebuild. Release engineering supplies a fresh, complete base for each published version.

| Journey | Required result | Smallest implementation boundary |
| --- | --- | --- |
| Run the prebuilt release | Extract one ready-to-run consumer bundle, open Launcher, select a supplied scene and Run. Publisher has built/cooked/configured the complete admitted base. | No source, compiler, cook, old content, manual content assembly/hash matching or repair step for ordinary use. Read release-local products/content; keep writable settings/logs/authored saves in existing owners. |
| Modify and rebuild | Obtain the matching source tag; optionally import that release's precompiled products and cooked content, then edit source/assets/shaders and use Launcher Build/Cook/Run. A source-only start also remains supported. | Use `SyncProducts.ps1`, canonical `artifacts/dev` outputs and existing Launcher operations. Rebuilt output replaces the imported base in those same locations; no second layout or ongoing source/binary freshness tracker. |
| Publish the next version | Empty owned build/cooked/stage outputs -> configure/build -> full cook -> final inventory/verification -> archive, with required signing before publication. | Existing CMake/Packaging owner. No previous product, cook cache, stored admission receipt or previous user content is a prerequisite. |

Supply one complete ready-to-run consumer bundle. Source, matched private symbols and optional rebuild tools may be companion downloads; users must not combine binary/content parts to start the rendering. Meet the existing combined size/startup budgets or leave the delivery blocker open. Publisher verification and initial matching source-tag/base import protect developer bootstrap; subsequent local edits use explicit Build/Cook in canonical outputs without a local-file version database or cross-release cache protocol.

Keep the seven approved map choices plus Empty, defaults, APIs and rendering gates. Prebuilt content can contain only admitted redistributable bytes. Existing Sponza/Modern Sponza acquisition-only restrictions remain blockers for their bundled derivatives; source acquisition remains available through Launcher. Do not call a source-download/cook prerequisite a prebuilt run, silently remove a selected map, or treat this decision as a rights grant.

Game follows the [render-only presentation contract](../../../Acceptance/FirstRelease.md#consumer-effort-and-game-presentation); Launcher and Editor retain their existing UI. Rebuild/recook is a deliberate developer choice, never an automatic first-run preparation chain. Publisher runtime closure/defaults/cooked content must make the ordinary launch work without special user setup.

**Current source, 2026-11-01:** Launcher owns preparation; engine consumers use the normal workspace-shaped tree without release or artifact-origin state. `SyncProducts.ps1` verifies a matching archive at the import boundary and fills the same binary/cooked directories that local Build/Cook overwrite. Consumers resolve those files and validate the formats they actually use. There is no seed marker, shared installed store, runtime payload-hash scan or prerequisite graph. Run never acquires, builds or cooks automatically.

Scene/shader publication retains the existing incomplete-write refusal and reader/writer exclusion. The cooked tree includes its passive `.publication-lock`; readers open it read-only without creating files. This is the same coherence contract for every product tree, independent of how its bytes arrived. Ordinary runtime state still uses the existing writable settings/log/save owners.

The [current source result](../../../../artifacts/validation/launcher-owned-delivery-20261101/completion.json) covers owner builds and isolated helper/filesystem controls. [FR-06](../../CrossModule/FirstRelease/ImplementationPlan.md#prebuilt-delivery-simplification) retains one affected new-archive rehearsal; no old receipt is relabeled.

## Current Package Entry

Publisher preparation uses the existing Launcher/CMake build and cook operations in empty owned generated outputs. Packaging does not own another build/cook runner. Root CMake generates product paths from actual targets and registers `release_package`:

```powershell
cmake -S . -B build -DSPARKLE_RELEASE_INPUTS=<reviewed-inputs.json>
# Prepare the selected Shipping products and complete cook through existing operations.
cmake --build build --config ShippingEditor --target release_package
```

`Package.ps1` reads the reviewed input list, freezes prepared product/cooked hashes once, stages the declared files, verifies them and creates the archive. Missing or inconsistent prepared outputs stop publication. `PackageOutputs.ps1` checks the native PE/PDB pairs and the explicitly supplied `ShaderExport` directory against the cooked shader map/library and compiler identity. External DLLs, helpers, notices and other reviewed inputs retain their pinned hashes. One final manifest supplies the stage, SBOM checksums and archive checks; there are no pre-build admission receipts or build-cache snapshots.

The stage verifier checks paths, hashes, notices, components, read-only attributes, x64 PE headers and vendor signatures. It reads ordinary/delay imports and requires app-local closure or explicit Windows/driver prerequisites; Game cannot import Editor/tool dependencies. SPDX file hashes/licenses and provenance product/source identities must match the final manifest. These checks do not replace legal review or complete SPDX schema validation.

ZIP creation uses sorted membership and fixed timestamps. Independent ZIP enumeration checks membership, size, hashes and attributes before publication. The manifest excludes its own digest; ZIP verification checks its exact staged bytes, and the external report binds the archive hash.

Outputs stay under `artifacts/releases/<version>/`. Assembly starts under `.partial-<id>/`; the verified ZIP, report and retained evidence publish together into `<archive-sha256>/` by one directory move. A report-write failure publishes nothing; an existing destination is never overwritten. Only `SparkleEngine-v<version>-windows-x64-unsigned-internal.zip` is the product archive. REL-03 also requires the applicable installed acceptance; the admitted internal result is linked above. Trusted signing and public redistribution remain FR-07 obligations.

Runtime discovers `Levels/<id>.level` documents directly, without cook-selection or raw-source gates. The project/acquisition catalog and its mutable selection are tool-only; ordinary Launcher Run does not require source-acquisition manifests. Explicit acquisition still checks its manifest before download/publication. [Boundary regression controls](../../../../artifacts/validation/runtime-inputs-slim-20261101/completion.json) do not qualify a new archive.

Runtime reads ordinary workspace/project/engine markers and uses `artifacts/dev/Projects/<project>/Cooked` everywhere. The archive retains the checkout-shaped tree, including `.sparkle`, `Engine/.sparkle-engine`, project definitions and `Config/DefaultEngine.ini`; there is no flattened installed layout, package manifest parsing or installed-mode flag in Engine. Package manifests remain transport/assembly evidence used only by package/sync tooling.

Launcher chooses executables and invokes Build/Cook through existing tool processes. Shared host-tool `ToolOutputPaths` owns build/library/helper paths outside Engine. Editor inspects runtime products and may explicitly request the same Launcher shader cook, with optional private symbols, then reload on success. Showcase binds that tool action; Engine owns no preparation paths or compiler arguments. The former compiler process owner, source watcher, recook queue and completion-receipt reader stay deleted. Compilers/cookers remain the actual transformation tools, with no upward Engine dependency on Launcher.

All products use the normal workspace-keyed per-user state. There is no release-version state branch, installed content store or migration. Earlier per-user data is untouched. Core provides generic filesystem/read/write safety; the shared interruption guard protects local cooks too and is not release policy.

The admitted internal route has exact-candidate assembly, failure/recovery and positive reconciliation, including applicable seven-map/subset/offline/relocation proof with original identities retained. Actual selected-extraction removal is explicitly excluded by its scope owner. The [FR-02 recheck](../../../../artifacts/validation/releases/0.1.0/241ef037-product-20261025/fr02-final-20261031/completion.json) confirms the accepted archive, membership and symbol identities remain unchanged. Regenerate only when admitted inputs change; PD-4 technical replay is complete, but its known coherence defect blocks closeout; final FR-07 public qualification retains its own gates.

### Portfolio Release Boundary

Follow [minimum supporting scope](../../../Acceptance/FirstRelease.md#minimum-supporting-scope) and the [minimum release workflow](../../../Acceptance/FirstRelease.md#minimum-release-workflow). Use this existing Build -> Cook -> Stage -> Verify -> ZIP route and the existing signing tools; package/report IDs do not require a release service, installer, updater or general recovery engine. Manual re-extraction, side-by-side replacement and clean/recook of generated data are sufficient. Preserve authored data, settings policy, contained paths, dependencies, candidate/symbol hashes and public notice/trust obligations.

Join existing three-product onboarding, source-build, diagnostics and replacement observations into the applicable reports. Reuse unchanged cells by explicit applicability; run changed boundaries against the exact current archive. A replacement receives its real old/new identity and affected preservation/rollback checks when needed. Do not manufacture extra versions or new support tooling merely to refill an unchanged matrix. Unsupported diagnostic categories stay explicit instead of scheduling a collector.

## At A Glance

| Product boundary | Current state | Required result |
| --- | --- | --- |
| development artifacts | implemented workspace-oriented output/copy routes | remain inputs only; never advertised as a release package |
| immutable staging | exact `a0f34c8b...` assembly, membership/integrity and symbols verified | recheck affected inputs for a new candidate |
| trust/publication | integrity, import and metadata verification exist | complete notices/SBOM/source obligations, symbols and pre-release signing |
| installation and writable state | admitted internal failure/recovery and positive reconciliation pass; original user state restored | actual selected-extraction removal remains explicitly excluded for this candidate |
| relocation and clean-machine use | `e771a244…` standard-user Unicode/read-only relocation checked on accepted MIN-01 laptop | no fresh-OS or broader hardware claim |

Publisher preparation is Build -> full Cook through existing tools. The package transaction is Freeze -> Stage -> Verify -> ZIP, followed by Extract/Run qualification. FR-07 adds publisher signing and verifies the resulting final bytes. Each step consumes a declared predecessor product and must never scan unrelated development directories to infer missing membership.

## Capability Identity

| ID | Capability | Current state |
| --- | --- | --- |
| `PKG-01` | Deterministic immutable staging tree and manifest | admitted `a0f34c8b...` internal assembly/integrity and installed proof accepted |
| `PKG-02` | License/SBOM, signature, integrity, and publication verification | internal integrity and symbol identities checked; public obligations/signing open |
| `PKG-03` | Standard-user extract/run/remove with separate writable state | admitted installed failure/recovery and positive proof accepted; actual selected-extraction removal explicitly excluded |
| `PKG-04` | Relocated, read-only, offline, clean-machine product operation | qualified positive journeys for `e771a244…` under the accepted existing-laptop scope |

## Capability Boundary

The CMake package owner consumes reviewed development outputs and creates a separate immutable stage/archive. An installer, registry integration and binary SDK are outside scope. Source discovery remains a separate development route; package consumers must not inherit its fallback or cleanup behavior.

The capability uses the same route for internal qualification and final publication; publisher signing is a final-publication step. It must classify the runtime, editor, Launcher, host tools, symbols, source/debug material, licenses, vendor binaries, cooked content, configuration, and writable per-user state independently.

## Inputs, Outputs, Ownership, And Lifetime

- Inputs: frozen source revision, profile/toolchain/options, dependency identities, selected product, cooked-content manifest, redistribution policy, explicit signature status (publisher identity at FR-07), and package version.
- Outputs: immutable staging tree, byte/permission/hash manifest, license/SBOM inventory, signature status, portable archive, verification result, and clean-machine evidence reference.
- Build/release configuration owns membership and publication. Individual modules own declarations of their required game products; no consumer may scan development directories to infer membership.
- Package bytes are immutable. Logs, settings, caches, captures, saves, and crash data live in an explicit per-user writable root and survive or reset only by declared policy.

## Requested And Active State

Package acceptance passes the admitted unsigned internal `a0f34c8b...` scope with its recorded removal exclusion. Earlier `e771a244...` and `3743e72a...` results remain bound to their own bytes and input-based applicability; Final support/product closeout is blocked by known source-applicable scene-publication coherence. The manifest and report bind products/profiles, backend/provider support, content, candidate and unsigned-internal status.

## Failure, Capacity, And Observability

Missing, extra, hash-mismatched, notice-incomplete, path-escaping, source-tree-dependent, or package-mutating bytes fail verification. Vendor signatures remain required; maintainer-built bytes are explicitly unsigned internally. Unsupported standard-user paths and unavailable optional providers must produce bounded actionable failures. The report records archive size, installed size, file count, peak stage/package memory, cold/warm duration, and deletion/update behavior.

## Design Decisions And Tradeoffs

| Decision | Benefit | Cost or constraint |
| --- | --- | --- |
| manifest-driven membership | reproducibility and review do not depend on directory scans | every contributing target must declare its game products |
| immutable installed bytes plus separate user state | verification, update, and uninstall behavior stay deterministic | paths/configuration must be designed for relocation and standard users |
| verify before candidate publication | corrupt, unsigned, or license-incomplete output cannot escape as a release | staging adds time, storage, signing, and provenance infrastructure |
| classify editor/game/tools/symbols independently | product contents match the intended audience and redistribution policy | multiple product profiles need explicit package matrices |

## External Precedent And Transfer Boundary

The external reference is **Unreal Engine 5.8 documentation**, retrieved 2026-09-08: [Packaging Unreal Engine Projects](https://dev.epicgames.com/documentation/unreal-engine/packaging-your-project?application_version=5.8) and [BuildGraph](https://dev.epicgames.com/documentation/unreal-engine/buildgraph-for-unreal-engine?lang=en-US&application_version=5.8), including its [script-elements reference](https://dev.epicgames.com/documentation/unreal-engine/buildgraph-script-elements-reference-for-unreal-engine?lang=en-US&application_version=5.8). No Unreal source checkout, code commit, AutomationTool implementation, or Epic service was inspected or adopted.

Only these invariants transfer into Sparkle's local contract:

- Build, Cook, Stage, Verify/Sign, and Package are distinct owned products; Stage is a fresh declared tree outside development outputs, not a rename of `artifacts/dev`.
- Preparation uses the existing build/cook owners; packaging consumes their completed files without a second dependency scheduler.
- Shipping membership deliberately differs from development membership, including removal of developer console/debug/authoring surfaces.
- cancellation/failure remains visible at the owning operation, and a failed prerequisite or node prevents dependent publication.

Sparkle does **not** adopt UAT, BuildGraph XML, nodes/agents/triggers as public vocabulary, Unreal's directory/Pak layout, Editor-driven packaging, or Launcher-owned package policy. The amended three-product bundle includes Launcher, with package orchestration owned by Build And Packaging. Local CMake/tool owners and the frozen Sparkle manifest remain authoritative.

## `FCR-PROD-05` Package Contract

**Target applicability:** the criteria below now qualify the prebuilt base and optional source-rebuild journey. Earlier PD-3 receipts retain their original store/helper scope; changed paths require affected requalification.

**Proof allocation:** PD-3 qualifies the admitted unsigned internal archive; PD-4 consumes that exact archive for adoption/replacement and six-report reconciliation. Publisher signing and public redistribution close in FR-07, with affected checks repeated for changed bytes. [Product delivery progress](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) retains current acceptance, the explicit removal exclusion and earlier receipts with their original identities.

| ID | Binary acceptance criterion | Proof phase |
| --- | --- | --- |
| `AC-PKG-01` | For a frozen revision/configuration/content set, prepare fresh Build/full Cook through existing tools, then run the package command on those outputs. It freezes one inventory, stages/verifies/packages it and publishes nothing on failure; publisher Sign is added in FR-07. | PD-3 internal assembly; FR-07 publisher signing |
| `AC-PKG-02` | `manifests/sparkle-package-manifest.json` accounts for every staged path, byte size, hash, permission/role, owner, origin, license, signature expectation, and component; independent verification rejects missing, extra, changed, absolute, escaping, or forbidden entries. | PD-3 stage/archive verification |
| `AC-PKG-03` | The bundle contains all three manifest-owned Launcher, Editor and Game components with exact release configurations and declared helper closure. Game has no Game ImGui host/menu/overlay or Editor/authoring dependency; Editor keeps its UI. The complete prebuilt cooked base comes from the current release; no older store or source cook is required to run it. Optional rebuild helpers are separately admitted for the source-workspace journey. Symbols are separate; installer/SDK/private-debug products and undeclared helpers are absent. | PD-3 product/helper/content membership |
| `AC-PKG-04` | All three products start as a standard user with immutable extraction bytes read-only; Editor/Game run the advertised prebuilt content offline after extracting the one ready-to-run consumer bundle, from Unicode/spaced/relocated paths with no repository/build/SDK/environment/admin dependency; writes occur only in the frozen per-user root. | PD-3 clean standard-user rehearsal |
| `AC-PKG-05` | Normal extract/first/repeat run, optional precompiled-plus-cooked source bootstrap followed by an actual source edit/rebuild, manual replacement/removal, retained-user-data policy and missing/corrupt required inputs are documented and stay within frozen size/time/memory budgets. | PD-3 lifecycle/removal; PD-4 replacement |
| `AC-PKG-06` | SBOM/provenance, notices, explicit unsigned-internal status, vendor PE signatures, checksums, archive/source/candidate identity, and native/shader symbol correlation refer to the same immutable candidate. Public rights and trusted publisher signing are FR-07 publication checks. | PD-3 internal identity; FR-07 public trust/rights |

| ID | Cause/injection, safe result, and affected criterion |
| --- | --- |
| `FM-PKG-01` | Seed stale/extra/source/debug or undeclared helper files, or omit any declared Launcher/Editor/Game component or required input. Fresh staging plus manifest verification rejects the tree before archive publication (`AC-PKG-01`–`03`). |
| `FM-PKG-02` | Alter a copied PE/archive/manifest/license/signature after staging. Verification quarantines the output and no candidate/archive success is emitted (`AC-PKG-02`, `AC-PKG-06`). |
| `FM-PKG-03` | Make package bytes read-only, remove overrides, relocate, and block network. Any package/repository write or external-path/SDK dependency fails `AC-PKG-04`. |
| `FM-PKG-04` | Deny/fill/corrupt the per-user root. The runtime names the path/space, preserves immutable bytes and last accepted state, and exits or disables only the optional operation (`AC-PKG-04`, `AC-PKG-05`). |
| `FM-PKG-05` | Remove each allowlisted member before package verification and reject the archive; separately remove the current product's required DLL/content inputs and reject at its startup/load boundary. Unused helper members do not become Game startup dependencies (`AC-PKG-03`–`05`). |
| `FM-PKG-06` | Search output names/docs/files for installer or SDK semantics. Any bootstrapper/registry/install API/binary ABI implication without an admitted owner fails `AC-PKG-03` and blocks publication. |

| Check | Claims falsified | Smallest route and fixed oracle |
| --- | --- | --- |
| `CHK-PKG-01` | `AC-PKG-01`–`03`, `AC-PKG-06`; `FM-PKG-01`, `FM-PKG-02`, `FM-PKG-06` | Generate from a clean destination, retain per-node results, independently enumerate/verify allowlist, denylist, hashes, signatures, SBOM/notices/symbol identity, then inject stale/extra/missing/tampered inputs. No archive may exist after a failed node. |
| `CHK-PKG-02` | `AC-PKG-04`, `AC-PKG-05`; `FM-PKG-03`–`05` | On a clean standard-user minimum machine, extract to Unicode/spaced and read-only locations, run the complete prebuilt base without source/build/old content state and with network/overrides absent, then import the matching base into a source workspace, modify one actual source input and use Launcher Build/Cook/Run; inventory all file/network/process activity, run consumer first/repeat/fault cases, remove archive, and compare budgets/state hashes. |

No acceptance row is satisfied by this documentation pass. [`BUILD-E03`](../CapabilityEvidencePlan.md#product-workflow-and-delivery-evidence) is the smallest central proof destination; candidate-bound results belong in the release artifact path required by [First Release](../../../Acceptance/FirstRelease.md).

## Release Inputs

This page owns the input choices needed to implement the package spine. [Release Scope](../../../Acceptance/FirstRelease.md#approved-release-scope) owns product promises; [First Release](../../../Acceptance/FirstRelease.md) owns candidate verdicts. Keep the supported seven maps and Launcher entry; start Empty until the user selects content.

**Delivery inputs:** three products, their real runtime closure, the complete admitted cooked base, source tag, notices and matching private symbols. Optional source-rebuild helpers follow the table below. Public policy files remain shipped metadata; diagnostic helper admission is justified by an actual local action, without a full package-verifier dependency. The custom release-review script/receipt schema is deleted. [Diagnostic owner](#current-local-route).

**Open-source license boundary:** Sparkle-owned code remains MIT, including commercial use. Free educational delivery describes the maintainer's intent, not an added use restriction. Third-party content and SDK terms remain separate.

**Free educational delivery (2026-10-11):** preserve all approved features, both backends and seven map choices. License compliance changes notices, source availability and how content is acquired; it must not disable DLSS/Ray Reconstruction, remove a map, substitute assets or silently lower quality. Optional source acquisition stays inside Launcher with progress, retry and explicit failure. The prebuilt journey consumes this release's cooked base; unresolved cooked redistribution cannot be hidden behind a required end-user source cook. A free release still needs the applicable component terms. Certificate enrollment and public redistribution closure belong to FR-07 immediately before publication; they do not block unsigned internal product implementation.

### Product And Helper Membership

| Component | Source product/profile | Admission rule |
| --- | --- | --- |
| Sparkle Launcher | `SparkleLauncher`, `ShippingEditor` | Mandatory; package-relative discovery and selected sync must work without a source checkout. Include Qt's exact release DLL/plugin closure and its LGPL notices/source/relink obligations. |
| Sparkle Editor | `ShowcaseEditor`, `ShippingEditor` | Mandatory; use the same identity/configuration owner as Game. Include its exact declared runtime closure; no undeclared developer SDK requirement. |
| Sparkle Game | `ShowcaseGame`, `ShippingGame` | Mandatory; no Editor/tool-only payload. Include the nine pinned Streamline runtime DLLs, including `sl.dlss_d.dll` and the Vulkan Reflex dependency `NvLowLatencyVk.dll`; keep NVIDIA/NGX/Reflex terms and signatures. |
| Optional source-rebuild helpers | `AssetCooker`, `TextureCooker`, `ShaderCompiler`, `ShippingEditor` | Keep their actual runtime closure when supplied as a ready-to-rebuild source base, or build them through Launcher with the documented toolchain. They are not prerequisites for ordinary prebuilt Run. No arbitrary SDK directory enters the consumer bundle. |
| Source acquisition/build | Existing Launcher CMake/source workflows and documented host prerequisites | Keep sync/build usable for optional source work. Portable CMake is not mandatory in the run bundle; admit a redistributable helper only for a selected source-workspace action. |
| Acquisition scripts | Existing Launcher `SyncAssetPack*.cmake` source owners | Source-workspace acquisition uses these owners. Run from a complete prebuilt base does not execute acquisition scripts. |
| Prebuilt cooked base | Admitted project/catalog/level metadata and complete cooked scene/material/texture/animation/shader dependencies | Ship the actual same-release base and qualify it without source assets or prior local content. No installed-store seed or persistent package-identity marker. Source-workspace import places the base in the existing canonical cooked output tree. |
| Rendering defaults | `Config/DefaultEngine.ini`, Shared/Defaults | Bind the source-owned file into the manifest. Installed Game/Editor verify it before applying shared per-user settings; missing/corrupt defaults stop startup. |
| Symbols / private intermediates | Separate archive / private build tree | AssetCooker requests shader debug information; DXIL PDBs stay outside cooked products. The existing ShaderCompiler `--debug-artifacts <private-dir>` route retains compile/source records for DXIL and SPIR-V correlation. No PDB, private source/path, cache, build tree or development executable belongs in the consumer archive. |

Release identity remains `0.1.0`, publisher Pawel Lisowski, copyright 2026. Use the existing identity owner; source and package signing identities must agree. Public window/display behavior and the laptop performance criterion remain in Release Scope. Do not replace the 30 FPS proof with this input inventory.

### Package And Writable Paths

The archive preserves the complete ordinary root layout, including the `artifacts/dev` prefix on binaries and cooked content. Open the root `LaunchSparkle.cmd`; its two-line command starts the existing Launcher executable without another installer or bootstrap service. `ReleaseProducts.cmake` derives paths from target output properties; it does not maintain a second package layout. Each product keeps its app-local DLL/plugin closure.

| Product | Path under either checkout or extraction root |
| --- | --- |
| Launcher | `artifacts/dev/Tools/SparkleLauncher/Binaries/ShippingEditor/SparkleLauncher.exe` |
| Editor | `artifacts/dev/Projects/Showcase/Binaries/ShippingEditor/ShowcaseEditor.exe` |
| Game | `artifacts/dev/Projects/Showcase/Binaries/ShippingGame/ShowcaseGame.exe` |
| Cook helpers | `artifacts/dev/Tools/<tool>/Binaries/ShippingEditor/<tool>.exe` |

Shared engine targets emit to `Engine/Binaries/<module>/<profile>` when enabled; their required DLLs are staged beside each consuming executable. Static/import libraries remain separate development outputs and are excluded from the product archive.

Core has no package path API. The existing marker-based roots and ordinary cooked-path getters work for both trees; `LauncherProductPaths` chooses launch/tool paths outside Engine. `LaunchSparkle.cmd` is pinned from `CMake/Packaging/LaunchSparkle.cmd` in reviewed inputs.

### Sync Product Bundles

Fill a source checkout's normal outputs from a verified archive for its exact Git revision:

```powershell
& ./CMake/Packaging/SyncProducts.ps1 -Archive <archive.zip> -Sha256 <published-sha256>
artifacts/dev/Tools/SparkleLauncher/Binaries/ShippingEditor/SparkleLauncher.exe
```

The default sync replaces Launcher, Editor and Game binary directories plus `artifacts/dev/Projects/Showcase/Cooked` (and `artifacts/dev/Projects/Shared/Cooked` when present). Optional `-Products` selects binary targets, including cook helpers when supplied; the complete cooked base is always synced. Source files, settings and private symbols are untouched. Close affected products and finish builds/cooks before sync. Failed replacement restores accepted directories; an unfinished rollback retains them in the named private transaction directory.

Imported outputs are writable. Local compilation/cooking replaces exactly those files; no manifest lookup, imported/generated switch or origin marker participates in ordinary consumption. Shipping profiles use the supplied paths; Development/Debug need their own builds. Sync checks the initial archive/tag identity, not subsequent local source freshness. Build/Cook is the user's explicit refresh decision.

Git is needed for initial checkout identity. Source acquisition/build/cook uses its normal action-specific prerequisites; ordinary Run needs only supplied runtime dependencies and a supported graphics driver. No downloadable qualified archive or automatic update feed is claimed yet.

The UGS-inspired part is shared output locations and explicit rebuild of what the developer changes. Sparkle keeps its current CMake/Launcher tools without adding Perforce, a sync service, version database or another build system. Historical internal archives keep their original store behavior and [candidate results](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress); this source change does not alter those archives.

### Reviewed Package Inputs

**Current input contract:** review the frozen source/configuration/content selection, runtime dependencies and redistribution terms before work. CMake target outputs and existing content recipes supply membership; build/cook then creates the bytes in empty owned generated roots. Generate one final file inventory with path, size/hash, component, runtime/helper role, required notices and signature expectation. Validate dependencies, matching symbols, stage membership and archive bytes directly from that inventory. Generate SBOM/provenance from the same facts; do not maintain another payload graph.

Published archive hashes and source-tag identity are delivery identities, not local-file schema versions. No `Admissions.DependencyClosure`/`SymbolCorrelation` JSON approval protocol, pre-existing executable digest, before/after payload rebind or old build cache is a prerequisite. Required legal/signing review remains real work; writing a PASS receipt cannot supply it.

`Package.ps1` consumes these prepared outputs. `Get-PreparedProductInputs` reads real PE/PDB/runtime facts and fails when a required product is missing; `Freeze-PackageOutputs` updates only declared generated entries, then `New-PackageManifest` creates the single final inventory. `ShaderExport` names the final export directory relative to the source root. No cook-log parsing, build-cache identity, audit mode or admission-status schema is part of this route.

### Content And Source Distribution

- The same release supplies its legally admitted prebuilt content and the matching optional source-rebuild base. A prebuilt advertised map needs no source acquisition/cook. Source recipes remain available for users who deliberately acquire or change inputs and rebuild through Launcher.
- Modern Sponza Base/Curtains and classic Sponza are acquisition-only until bundled redistribution is established. Exclude source, textures, cooked derivatives and their thumbnails from product archives. Publisher terms remain applicable to users; an acquisition recipe is not a new license grant.
- Classic Sponza uses the pinned Khronos recipe under `Assets/Meshes/Sponza/SourceFiles.txt`; the level reads `Sponza/Publisher/glTF/Sponza`. The replaced bundled model/texture files are removed from the current source tree. The acquired tree is gitignored.
- Cornell consumes the tracked self-contained GLB with `COPYRIGHT.txt` and conversion attribution. Its publisher OBJ ZIP is optional for regeneration. Do not turn the unavailable regeneration archive into a normal launch prerequisite.
- Retain the selected Cesium/plant/chess copyright, attribution and logo notices; retain LPS Head's publisher/conversion notice. Retain Poly Haven CC0 environment provenance and Font Awesome's font license. Package copies must preserve notices for source and cooked derivatives.
- Public release source is a reviewed exported archive from the exact candidate. `.gitattributes` excludes unqualified Sponza bytes, the excluded DamagedHelmet payload and development thumbnails without reviewed export receipts; new acquisition recipes/notices must be committed and present before export acceptance. Raw repository clones/history are a separate distribution surface, not certified by `export-ignore`.

**Public repository decision (2026-10-11):** [SparkleEngine](https://github.com/stoleckipawel/SparkleEngine) remains the main public, open-source repository with its existing history. The owner superseded the private-history plan; no private source repository or separate publication workflow is required. The owner requested removal of the extra SparkleEngine-Source repository; it is not a development or release destination. Keep production features, seven map definitions and acquisition recipes available publicly. Review historical asset rights separately; public visibility and export exclusions do not establish redistribution permission.

### Supply Chain And Signing

| Input family | Distribution decision / proof still required |
| --- | --- |
| Permissive source dependencies and PIX | Carry exact retained copyright/license notices for the compiled components, including embedded third-party notices. An entire dependency checkout is not the admitted payload. |
| Streamline / NGX / Reflex | Preserve the pinned runtime terms and component notices; admit application binaries individually, excluding `sl_nvperf.dll` and developer tools. [NVIDIA obligations](../../../../CMake/Notices/NVIDIA/README.md) include protective distribution terms, applicable SDK/mark attribution and prior written approval of proposed marks under RTX/NGX section 7.2(c). This concerns mark usage, not separate permission to develop with the SDK. Public distribution remains unqualified; Sparkle-owned code retains its MIT license. |
| Qt `6.11.1` | Use dynamically linked LGPL-eligible modules/plugins; keep notices/license text and a prominent Qt notice, the exact corresponding library source and a documented replacement/relink route. [Qt's LGPL obligations](https://www.qt.io/development/open-source-lgpl-obligations) apply even with dynamic linking. The source companion below is identified. Launcher now stages LGPL/GPL license texts and [Qt source/replacement instructions](../../../../CMake/Notices/Qt/README.md); root onboarding prominently identifies Qt. Package source publication, full third-party closure and replacement/relink execution still need acceptance. |
| MSVC / DXC / portable CMake | Package only the exact redistribution-authorized runtime/compiler-helper files needed by the admitted journeys, with their terms. Local SDK installation and `windeployqt` output are discovery, not an allowlist or a license receipt. Qualify app-local CRT for the portable, standard-user promise; a staged `vc_redist.x64.exe` does not prove that journey. |

- Resolve declared Git dependencies at full commit hashes; hash-check downloaded SDK archives and both Font Awesome inputs. Wrong cache bytes are a failure, never a substitute input. Keep the failed cache for diagnosis.
- Produce a component inventory from the exact staged bytes: first-party products, linked/static third-party components, dynamic DLLs/plugins, helpers, source/cooked content and notices. Use SPDX JSON for the package SBOM with exact versions/revisions/hashes and relationships; no guessed `NOASSERTION` license is admission evidence. The dependency audit feeds this inventory, not a directory copy of every downloaded SDK.
- Preserve the exact installed toolchain/SDK versions and prerequisites independently of redistributable components. Dependency license files and inline notices (including MikkTSpace, tinyexr and CMP_Core) must reach the notice bundle. NVIDIA SDK redistribution conditions and Qt LGPL/source/relink obligations must be satisfied for their admitted bytes.
- Signing choice (owner delegated selection, 2026-10-11): **Certum Standard Code Signing in the Cloud, individual subscriber Pawel Lisowski**, through SimplySign Desktop and Windows SDK SignTool. This preserves the named publisher without assuming eligibility for an open-source-only certificate. [Certum's individual enrollment](https://support.certum.eu/en/code-signing-required-documents/) requires purchase and identity/address verification. Enrollment, payment and issuance have not occurred; no account or certificate was fabricated. The maintainer completes verification directly with Certum; identity documents and credentials never enter this repository.
- Sign staged first-party PE files with the issued certificate's explicit thumbprint, SHA-256 file digest and RFC 3161 SHA-256 timestamp at `http://time.certum.pl/`, as in [Certum's cloud signing instructions](https://files.certum.eu/documents/manual_pl/Podpisywanie_narzedziem_signtool_i_jarsign_Code_Signing_w_chmurze.pdf). Keep valid third-party signatures. The package owner supplies the allowlisted file paths; signing is FR-07 work after issuance, before public publication. Verify every signature with Authenticode policy, timestamp and expected publisher. Unsigned local builds are development evidence.
- Certificate availability is not established by a source-baseline check. Enrollment/SimplySign activation, signing and signed-byte verification remain final FR-07 obligations under the decided route. The [public rights and trust boundary](../../../Acceptance/FirstRelease.md#public-rights-and-trust) distinguishes unsigned technical proof from public trust; a self-signed certificate does not close that boundary.

#### Qt Source Companion And Component Terms

Launcher requires Qt 6.11.1 exactly. Its deployed Core, Gui, Network, Widgets and Svg libraries and platform/style/image/TLS plugins require the matching source companions below. Publish them alongside the products; normal users need not download or build Qt source. Recheck this closure if deployed modules change.

| Module / exact source revision | Publisher archive / SHA-256 |
| --- | --- |
| qtbase `59c81a3c2247b821b9b84b4eb8d939b77e07e276` | [qtbase 6.11.1 ZIP](https://download.qt.io/archive/qt/6.11/6.11.1/submodules/qtbase-everywhere-src-6.11.1.zip), `3529cc37297a5a7aae4486843b9fd41c30df1d79a770f85e240b537dcc327ca5` |
| qtsvg `2596f43da2dc72d2afecc084355b0f5f87922a6c` | [qtsvg 6.11.1 ZIP](https://download.qt.io/archive/qt/6.11/6.11.1/submodules/qtsvg-everywhere-src-6.11.1.zip), `767730188d4610a89bf8da502f87acf1c8881a3ac54f1e0eb167ab1e08b03a75` |

Package the applicable `LICENSES/` texts, vendor/third-party notices and SBOMs, plus a prominent Qt notice and source-companion location. Allow replacement of the dynamically loaded Qt DLLs/plugins with compatible user builds and provide the build/install instructions necessary to run them; qualify this journey before consumer acceptance. Sparkle's MIT grant covers its first-party source. NVIDIA runtime restrictions apply to those NVIDIA components, not to users' LGPL rights over Qt. Keep NVIDIA notices/terms distinct and preserve signatures; do not describe the whole archive as MIT-only. The existing Streamline staging owner now carries its five runtime notice files beside the unchanged DLL set; this is not full package compliance proof.

After issuance, the package owner invokes these commands for each allowlisted first-party PE (`$signTool` is the exact Windows SDK executable; `$certificateThumbprint` is the issued, verified certificate; `$stagedPe` is a manifest path):

```powershell
& $signTool sign /sha1 $certificateThumbprint /fd SHA256 /tr http://time.certum.pl/ /td SHA256 $stagedPe
& $signTool verify /pa /all /v $stagedPe
```

`/sha1` selects the certificate by thumbprint; the file digest is SHA-256. A failed signing or verification command stops package publication. Record the issued subject/thumbprint, tool version, signed-byte hashes, timestamp and verification output before accepting signatures.

### Threat And Failure Boundary

Source/dependency acquisition, publisher content, cache bytes, staged files and signing credentials are distinct trust boundaries. Verify hashes and contained paths before publication; do not execute tools from user content, accept traversal/reparse escapes, stage unknown DLLs, or weaken a digest after failure. Missing content stops the requested map visibly; Empty must not masquerade as successful activation of that map. Keep logs local and preserve the approved offline/privacy/no-support promises.

FR-02 implements transactional stage/verify/archive and package-relative discovery from these named inputs. FR-01 inspection or a successful local source build does not prove the consumer package, complete notices/SBOM, trusted signature or clean-machine journey.

## Local Diagnostics And Release Review

**Scope:** self-service onboarding, recovery, local diagnostics and release review

**Owner:** release/product delivery; Launcher, Application, Core diagnostics, Docs, and project products provide owned parts of the route

### Diagnostics: At A Glance

Deliver self-service onboarding, diagnostics and recovery. **No user support service** is offered; [Release Scope](../../../Acceptance/FirstRelease.md#approval-and-next-work) owns that decision.

The admitted PD-3 package technical scope is accepted for `a0f34c8b...`; aggregate product closeout remains blocked by the source-applicable scene-publication defect. Exact-archive native incident results and unavailable categories stay in the [product reports](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress).

### Current Local Route

**Evidence:** [Product reports](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) retain archive-bound export/symbol/review checks and outstanding adoption/native/replacement proof. Public instructions: [recovery/privacy](../../../../SUPPORT.md), [known issues](../../../../KNOWN_ISSUES.md), [security](../../../../SECURITY.md).

Ordinary self-service uses product/Launcher logs, documented requirements, known issues and manual re-extraction. It needs no retained ZIP, full package validation or diagnostic helper installation. Missing files/symbols remain unavailable, never verified by omission.

| Responsibility | Current owner and boundary |
| --- | --- |
| Logs/retention | Core Logger and Launcher operation logs; [retention/removal rules](../../../../SUPPORT.md#retention-and-replacement) preserve user data. |
| GPU faults | Existing native logging and the optional source GPU-crash tools. Automatic CPU crash/hang collection and unsupported native categories remain unavailable. |
| GPU export | Source-only `ExportDiagnostics.ps1 -GpuCrashManifestPath ...` retains bounded preview, category consent, matched-symbol checks and unchanged originals. It has no ZIP/package prerequisite. [Privacy limits](../../../../SUPPORT.md#preview-and-export-local-diagnostics) apply. |
| Symbols | Existing PE/PDB GUID-age checks and separate shader artifacts bind the exact inputs; source attribution needs its own actual proof. |
| Review/replacement | Maintainer reviews the candidate, known issues and affected evidence once. Unresolved S0/S1 blocks publication. Manual side-by-side extraction/rollback preserves published identities and user state. |

The custom review/defect-receipt script and generic archive-projection exporter are removed. Do not replace them with another schema, collector or review service. Existing source GPU privacy/export mechanics have real rendering-diagnostics consumers and remain scoped to that job. Historical archive-bound controls keep their original identity.

### Diagnostics: Capability Identity

| ID | First-release boundary |
| --- | --- |
| `ADOPT-01` | Public identity, requirements, controls, first use, reset and exit for Launcher, Editor, Game and source adoption. |
| `ADOPT-02` | Actionable local startup/content/backend failure and self-service recovery. |
| `ADOPT-03` | Bounded, inspectable local diagnostics and candidate/symbol correlation; intake/upload service excluded. |
| `ADOPT-04` | Finite maintainer release review and immutable replacement/withdrawal; ongoing response/maintenance service excluded. |

### Diagnostics: Capability Boundary

The author or agent can reach the promised product result or understand its failure without private guidance. Core/RHI/Application own diagnostic production; release delivery owns public instructions, local-data privacy, candidate-to-symbol identity and release review. No reporting channel or automatic upload is implied.

### Contract

- Inputs: verified product/source identity, selected content, public prerequisites, exact configuration and local diagnostic records.
- Outputs: successful first use or actionable failure, inspectable diagnostics, documented reset/removal and truthful known issues/no-support policy.
- Local logs/captures/crash data are bounded and associated with the exact candidate. Explain sensitive paths/content before any voluntary sharing; retain originals locally.
- Any voluntarily published correction has a new immutable identity, affected-gate revalidation and a documented user-state policy.

### `FCR-PROD-06` Support And Incident Contract

This stable family ID now covers adoption, local diagnostics and release review. Its former operating-service clocks and mandatory intake criteria are superseded by the release-owner amendment. [First Release](../../../Acceptance/FirstRelease.md#frozen-paths-mutation-boundary-and-budgets) owns paths/budgets and the no-support boundary.

**Proof allocation:** use PD-3's exact archive and symbol companion for PD-4; earlier export/review fixtures are not final-archive acceptance. Automatic CPU crash/hang collection is unavailable, not an admitted new implementation task. Exercise supported diagnostic/manual routes and prove that unavailable categories stay explicit; do not call fixtures native incident proof.

| ID | Binary acceptance criterion | Proof phase |
| --- | --- | --- |
| `AC-ADOPT-01` | The author or agent reaches onboarding for all three products and source adoption; identifies exact version, requirements, controls, reset/exit, known issues and the absence of a user support service without private guidance. | PD-2 source adopter; PD-4 three-product self-service |
| `AC-ADOPT-02` | Startup, crash, hang/timeout and GPU/device failure produce bounded inspectable local diagnostics where supported. No automatic upload occurs; unavailable diagnostics and manual recovery are explicit. Automatic CPU crash/hang collection is not required by this amended scope. | PD-4 supported native/manual routes and limits |
| `AC-ADOPT-03` | Native/shader symbols, local logs, consented dumps/captures and manifest/provenance correlate to the exact immutable candidate; mismatched/insufficient data is rejected with recovery. | PD-3 symbol receipts; PD-4 incident correlation |
| `AC-ADOPT-04` | Maintainer release review records known defect severity, affected candidate/version and disposition. Public documentation promises no intake service, response deadline, supported-version period or fix. Known `S0`/`S1` blocks approval/closeout. | PD-4 review and no-support policy |
| `AC-ADOPT-05` | Any replacement uses a new version; hash applicability, user-state preservation/reset, rollback/side-by-side and withdrawal are explicit and exercised on an isolated candidate copy. Published bytes never change under an existing identity. | PD-4 immutable replacement/withdrawal |

| ID | Cause/injection and safe result |
| --- | --- |
| `FM-ADOPT-01` | Omit a prerequisite, break onboarding or insert an unsupported service promise. The journey blocks until accurate instructions/no-support boundaries are restored (`AC-ADOPT-01`, `AC-ADOPT-04`). |
| `FM-ADOPT-02` | Add private paths/tokens, unrelated content, oversized logs or wrong candidate identity to a proposed diagnostic export. Preview/redaction/validation rejects it and leaves originals local (`AC-ADOPT-02`, `AC-ADOPT-03`). |
| `FM-ADOPT-03` | Disable diagnostic collection or omit symbols. Report the unavailable item and manual recovery; empty output is never success (`AC-ADOPT-02`, `AC-ADOPT-03`). |
| `FM-ADOPT-04` | Add a known `S0`/`S1` to a closeout fixture or an invented response deadline to release docs. Review blocks; preserve the record and correct/revalidate or withdraw affected bytes (`AC-ADOPT-04`). |
| `FM-ADOPT-05` | Present a replacement for the wrong hash/version or interrupt it. Reject before mutation; preserve original bytes/user state and existing published versions (`AC-ADOPT-05`). |

| Check | Coverage | Smallest route and oracle |
| --- | --- | --- |
| `CHK-ADOPT-01` | `AC-ADOPT-01`; `FM-ADOPT-01` | Documented packaged-product and source journeys through success and a missing prerequisite. Private repair, dead onboarding or misleading support claims fail. |
| `CHK-ADOPT-02` | `AC-ADOPT-02`, `AC-ADOPT-03`; `FM-ADOPT-02`, `FM-ADOPT-03` | Isolated local startup/crash/hang/GPU diagnostics, with private/mismatched/oversized/unavailable fixtures. Inspect identity, symbols, redaction, bounds and absence of network uploads. |
| `CHK-ADOPT-03` | `AC-ADOPT-04`; `FM-ADOPT-04` | Review the exact candidate's known issues, evidence and public policy, then challenge the decision with an unresolved critical defect and a false support promise. No machine-readable defect-ledger schema is required. Both must block closeout; no live inbox submission is required. |
| `CHK-ADOPT-04` | `AC-ADOPT-05`; `FM-ADOPT-05` | Apply/interrupt a versioned replacement on copies with representative user state; verify applicability, atomicity, state policy, rollback/withdrawal and unchanged public bytes. |

Candidate results remain in `FCR-PROD-06`; fixtures cannot close an unavailable required native or adoption check. Missing intake channels alone are not a blocker.
