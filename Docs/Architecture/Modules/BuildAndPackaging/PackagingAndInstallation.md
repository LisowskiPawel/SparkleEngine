# Packaging And Installation

[Docs](../../../README.md) / [Up](BuildAndPackagingCapabilities.md)

**Status:** package contract and current implementation route; consumer package acceptance remains blocked

**Scope:** define release inputs, deterministic packaging, clean-machine operation and local diagnostics/replacement

**Owner:** repository build and release-delivery configuration; product contributors are Launcher, Showcase, Application, Tools, and the engine modules whose runtime bytes enter the package

**Strategy and acceptance sources:** [`PGE-01`, `PGE-13`, `PGE-14`, `PGE-15`](../../../Strategy/Requirements.md), [First Release](../../../Acceptance/FirstRelease.md), and `FCR-PROD-02`/`FCR-PROD-05` in [Feature Completion Reports](../../../Acceptance/FeatureCompletionReports.md)

**Acceptance:** blocked until the exact archive and clean-machine journey pass. See [Release progress](../../../Strategy/Roadmap.md#release-progress) for the current archive identity and checks.


Use [reviewed inputs](#reviewed-package-inputs) to prepare the package, [package command](#current-package-entry) to assemble it, [package checks](#fcr-prod-05-package-contract) to qualify it, and [local diagnostics](#local-diagnostics-and-release-review) for incident/export/replacement behavior.

## Current Package Entry

`CMake/Packaging` is the Build And Packaging owner. Root CMake generates product inputs from actual targets and registers `release_package`. It invokes existing CMake builds and AssetCooker commands, then copies only reviewed file entries into a new stage. No install rule exposes internal targets, and Launcher/Renderer/Application do not own packaging policy.

```powershell
cmake -S . -B build -DSPARKLE_RELEASE_INPUTS=<reviewed-inputs.json>
cmake --build build --config ShippingEditor --target release_package
```

The input contract is in [Release Inputs](#reviewed-package-inputs). Dependency and symbol admission stops before internal build/cook/stage. Each retained owner receipt must pass and identify the exact source candidate. Public rights and Windows publisher signing are checked in FR-07 before publication. Discovery/content-store acceptance follows the produced archive; it is not a prerequisite that requires an archive before one can be made.

After build/cook, `PackageOutputs.ps1` rechecks the six PE/PDB pairs and consumes the final cook's returned shader-export receipt. It freezes new hashes only for configured executables and admitted cooked outputs. Reviewed DLLs, helpers, recipes, notices and other external inputs must retain their admitted hashes. The transaction updates SBOM checksums for those generated outputs; its report distinguishes the reviewed payload from the final frozen payload. Compiler-generated identifier ordering need not be byte-identical across cooks.

The stage verifier checks paths, hashes, notices, components, read-only attributes, x64 PE headers and vendor signatures. It reads ordinary/delay imports and requires app-local closure or explicit Windows/driver prerequisites; Game cannot import Editor/tool dependencies. SPDX file hashes/licenses and provenance product/source identities must match the final manifest. These checks do not replace legal review or complete SPDX schema validation.

ZIP creation uses sorted membership and fixed timestamps. Independent ZIP enumeration checks membership, size, hashes and attributes before publication. The manifest excludes its own digest; ZIP verification checks its exact staged bytes, and the external report binds the archive hash.

Outputs stay under `artifacts/releases/<version>/`. Assembly starts under `.partial-<id>/`; the verified ZIP, report and retained evidence publish together into `<archive-sha256>/` by one directory move. A report-write failure publishes nothing; an existing destination is never overwritten. Only `SparkleEngine-v<version>-windows-x64-unsigned-internal.zip` is the product archive. Internal assembly does not pass REL-03; trusted signing and public redistribution obligations remain pre-release steps.

Core owns manifest-relative paths and product size/hash verification. Launcher prefers its installed location over the current directory and rejects checkout overrides. First launch verifies and transactionally seeds `%LOCALAPPDATA%/SparkleEngine/Content/Showcase/`: project recipes into `Source/Showcase/`, the immutable `Engine/Cooked/` baseline into `Cooked/`. Initial shader readiness verifies the copied baseline against the package manifest without a recook signal; later recooks require their completion signal. Repeat launches preserve selections; a changed or incomplete package identity requires an explicit store reset. Sync/cook/run use the existing typed owners and packaged helpers; source build/cleanup actions are unavailable in installed mode. Missing or malformed package metadata cannot select a checkout.

The installed route is implemented but not accepted. The [roadmap](../../../Strategy/Roadmap.md#release-progress) summarizes qualified archive checks and remaining product, replacement and adoption work. Run CHK-PKG-02 against the archive regenerated after those fixes; public redistribution and publisher signing remain FR-07 work.

## At A Glance

| Product boundary | Current state | Required result |
| --- | --- | --- |
| development artifacts | implemented workspace-oriented output/copy routes | remain inputs only; never advertised as a release package |
| immutable staging | source route and structural rejection checks exist | qualify exact three-product bytes through the complete route |
| trust/publication | integrity, import and metadata verification exist | complete notices/SBOM/source obligations, symbols and pre-release signing |
| installation and writable state | manifest discovery, transactional store seeding and existing sync/cook/run paths | qualify seven-map selection, offline use, failure/reset and removal |
| relocation and clean-machine use | unqualified | retained standard-user rehearsal of the exact archive |

The internal route is one directional transaction: Build -> Cook -> Stage -> Verify -> Package -> Extract/Run. FR-07 adds publisher signing and verifies the resulting final bytes. Each step consumes a declared predecessor product and must never scan unrelated development directories to infer missing membership.

## Capability Identity

| ID | Capability | Current state |
| --- | --- | --- |
| `PKG-01` | Deterministic immutable staging tree and manifest | current internal assembly/integrity verified; final product proof open |
| `PKG-02` | License/SBOM, signature, integrity, and publication verification | internal integrity and symbol identities checked; public obligations/signing open |
| `PKG-03` | Standard-user extract/run/remove with separate writable state | local Empty/Cornell journeys checked; removal and full consumer proof open |
| `PKG-04` | Relocated, read-only, offline, clean-machine product operation | local offline/read-only/relocation checks recorded; clean environment remains open |

## Capability Boundary

The CMake package owner consumes reviewed development outputs and creates a separate immutable stage/archive. An installer, registry integration and binary SDK are outside scope. Source discovery remains a separate development route; package consumers must not inherit its fallback or cleanup behavior.

The capability uses the same route for internal qualification and final publication; publisher signing is a final-publication step. It must classify the runtime, editor, Launcher, host tools, symbols, source/debug material, licenses, vendor binaries, cooked content, configuration, and writable per-user state independently.

## Inputs, Outputs, Ownership, And Lifetime

- Inputs: frozen source revision, profile/toolchain/options, dependency identities, selected product, cooked-content manifest, redistribution policy, explicit signature status (publisher identity at FR-07), and package version.
- Outputs: immutable staging tree, byte/permission/hash manifest, license/SBOM inventory, signature status, portable archive, verification result, and clean-machine evidence reference.
- Build/release configuration owns membership and publication. Individual modules own declarations of their required game products; no consumer may scan development directories to infer membership.
- Package bytes are immutable. Logs, settings, caches, captures, saves, and crash data live in an explicit per-user writable root and survive or reset only by declared policy.

## Requested And Active State

The source packaging route is implemented but unqualified. A development artifact directory is not a package. The manifest and completion report must name the exact products/profiles, backend/provider support, content set, candidate and signature status before acceptance.

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
- A requested terminal product executes only the dependency nodes it requires; every node has explicit inputs, required predecessors, produced outputs, result, and retained artifact identity.
- Shipping membership deliberately differs from development membership, including removal of developer console/debug/authoring surfaces.
- cancellation/failure remains visible at the owning operation, and a failed prerequisite or node prevents dependent publication.

Sparkle does **not** adopt UAT, BuildGraph XML, nodes/agents/triggers as public vocabulary, Unreal's directory/Pak layout, Editor-driven packaging, or Launcher-owned package policy. The amended three-product bundle includes Launcher, with package orchestration owned by Build And Packaging. Local CMake/tool owners and the frozen Sparkle manifest remain authoritative.

## `FCR-PROD-05` Package Contract

**Proof allocation:** PD-3 qualifies an explicitly unsigned internal archive; PD-4 reuses that exact archive for adoption/replacement. Publisher signing and public redistribution close in FR-07, with affected checks repeated for the changed bytes. [Product delivery progress](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) retains the earlier archive result without qualifying the current candidate.

| ID | Binary acceptance criterion | Proof phase |
| --- | --- | --- |
| `AC-PKG-01` | One command consumes a frozen revision/configuration/content set and produces Build -> Cook -> fresh Stage -> Verify -> Package results (publisher Sign is added in FR-07) with explicit predecessor/output identities; failure at any node prevents archive publication. | PD-3 internal assembly; FR-07 publisher signing |
| `AC-PKG-02` | `manifests/sparkle-package-manifest.json` accounts for every staged path, byte size, hash, permission/role, owner, origin, license, signature expectation, and component; independent verification rejects missing, extra, changed, absolute, escaping, or forbidden entries. | PD-3 stage/archive verification |
| `AC-PKG-03` | The bundle contains all three manifest-owned Launcher, Editor and Game components with exact release configurations and declared helper closure. Game imports no Editor/authoring dependency. Selected content uses a verified writable store separate from immutable product bytes. Symbols are separate; installer/SDK/source/debug products and undeclared helpers are absent. | PD-3 product/helper/content membership |
| `AC-PKG-04` | All three products start as a standard user with immutable extraction bytes read-only; Editor/Game run offline after selected content sync, from Unicode/spaced/relocated paths with no repository/build/SDK/environment/admin dependency; writes occur only in the frozen per-user root. | PD-3 clean standard-user rehearsal |
| `AC-PKG-05` | Normal extract/first/repeat run, side-by-side replacement policy, archive removal, retained-user-data policy, and missing/corrupt dependency/content paths are documented and stay within frozen size/time/memory budgets. | PD-3 lifecycle/removal; PD-4 replacement |
| `AC-PKG-06` | SBOM/provenance, notices, explicit unsigned-internal status, vendor PE signatures, checksums, archive/source/candidate identity, and native/shader symbol correlation refer to the same immutable candidate. Public rights and trusted publisher signing are FR-07 publication checks. | PD-3 internal identity; FR-07 public trust/rights |

| ID | Cause/injection, safe result, and affected criterion |
| --- | --- |
| `FM-PKG-01` | Seed stale/extra/source/debug or undeclared helper files, or omit any declared Launcher/Editor/Game component or required input. Fresh staging plus manifest verification rejects the tree before archive publication (`AC-PKG-01`–`03`). |
| `FM-PKG-02` | Alter a copied PE/archive/manifest/license/signature after staging. Verification quarantines the output and no candidate/archive success is emitted (`AC-PKG-02`, `AC-PKG-06`). |
| `FM-PKG-03` | Make package bytes read-only, remove overrides, relocate, and block network. Any package/repository write or external-path/SDK dependency fails `AC-PKG-04`. |
| `FM-PKG-04` | Deny/fill/corrupt the per-user root. The runtime names the path/space, preserves immutable bytes and last accepted state, and exits or disables only the optional operation (`AC-PKG-04`, `AC-PKG-05`). |
| `FM-PKG-05` | Remove each allowlisted DLL/content file from isolated archive copies. Startup identifies the missing component and exits without partial success or fallback content (`AC-PKG-03`–`05`). |
| `FM-PKG-06` | Search output names/docs/files for installer or SDK semantics. Any bootstrapper/registry/install API/binary ABI implication without an admitted owner fails `AC-PKG-03` and blocks publication. |

| Check | Claims falsified | Smallest route and fixed oracle |
| --- | --- | --- |
| `CHK-PKG-01` | `AC-PKG-01`–`03`, `AC-PKG-06`; `FM-PKG-01`, `FM-PKG-02`, `FM-PKG-06` | Generate from a clean destination, retain per-node results, independently enumerate/verify allowlist, denylist, hashes, signatures, SBOM/notices/symbol identity, then inject stale/extra/missing/tampered inputs. No archive may exist after a failed node. |
| `CHK-PKG-02` | `AC-PKG-04`, `AC-PKG-05`; `FM-PKG-03`–`05` | On a clean standard-user minimum machine, extract to Unicode/spaced and read-only locations, test user-selected sync, then block network/remove overrides, inventory all file/network/process activity, run consumer first/repeat/fault cases, remove archive, and compare budgets/state hashes. |

No acceptance row is satisfied by this documentation pass. [`BUILD-E03`](../CapabilityEvidencePlan.md#product-workflow-and-delivery-evidence) is the smallest central proof destination; candidate-bound results belong in the release artifact path required by [First Release](../../../Acceptance/FirstRelease.md).

## Release Inputs

This page owns the input choices needed to implement the package spine. [Release Scope](../../../Acceptance/FirstRelease.md#approved-release-scope) owns product promises; [First Release](../../../Acceptance/FirstRelease.md) owns candidate verdicts. Keep the supported seven maps and Launcher entry; start Empty until the user selects content.

**PD-4 package inputs:** `ExportDiagnostics.ps1`, `IncidentIdentity.ps1` and `PackageVerification.ps1` under `Tools/Diagnostics/` (Shared/Helper); root `SUPPORT.md`, `SECURITY.md` and `KNOWN_ISSUES.md` under `docs/` (Shared/Metadata). Require source hashes, notices and SBOM entries. `ReviewRelease.ps1` stays maintainer-only; PDBs stay in the separate symbol companion. Local prerequisites: PowerShell 5.1 and System32 DbgHelp; no symbol-server lookup. [Diagnostic owner](#current-local-route).

**Open-source license boundary:** Sparkle-owned code remains MIT, including commercial use. Free educational delivery describes the maintainer's intent, not an added use restriction. Third-party content and SDK terms remain separate.

**Free educational delivery (2026-10-11):** preserve all approved features, both backends and seven map choices. License compliance changes notices, source availability and how content is acquired; it must not disable DLSS/Ray Reconstruction, remove a map, substitute assets or silently lower quality. Selected publisher downloads stay inside Launcher, with progress, retry and offline reuse after acquisition. A free release still needs the applicable component terms. Certificate enrollment and public redistribution closure belong to FR-07 immediately before publication; they do not block unsigned internal product implementation.

### Product And Helper Membership

| Component | Source product/profile | Admission rule |
| --- | --- | --- |
| Sparkle Launcher | `SparkleLauncher`, `ShippingEditor` | Mandatory; package-relative discovery and selected sync must work without a source checkout. Include Qt's exact release DLL/plugin closure and its LGPL notices/source/relink obligations. |
| Sparkle Editor | `ShowcaseEditor`, `ShippingEditor` | Mandatory; use the same identity/configuration owner as Game. Include its exact declared runtime closure; no undeclared developer SDK requirement. |
| Sparkle Game | `ShowcaseGame`, `ShippingGame` | Mandatory; no Editor/tool-only payload. Include the nine pinned Streamline runtime DLLs, including `sl.dlss_d.dll` and the Vulkan Reflex dependency `NvLowLatencyVk.dll`; keep NVIDIA/NGX/Reflex terms and signatures. |
| Selected-content cook helpers | `AssetCooker`, `TextureCooker`, `ShaderCompiler`, `ShippingEditor` | All three helpers and their exact runtime closure are mandatory. ShaderCompiler declares its Slang support DLLs, `slang.slang` and every standard-module file; the audit hashes them and the manifest requires them. Headers, compiler development tools, examples and arbitrary SDK directories are excluded. |
| Acquisition helper | Portable CMake `4.3.3`, used by the existing sync owner; publisher archive SHA-256 `935ade9e5e8723583c07f44c5592cea2a1c8f65c56ca7e07b34c025c880e0bd6` | Include `Tools/CMake/bin/cmake.exe`, the publisher's `Tools/CMake/share/cmake-4.3/Modules/CMake.cmake` root-discovery module and its license. The three sync scripts use built-in commands; no general CMake development package is admitted. Qualify this helper layout through the sync owner, without a host installation. |
| Acquisition scripts | Existing `SyncAssetPack.cmake`, `SyncAssetPackFiles.cmake`, `SyncAssetPackArchive.cmake` under `Tools/Content/` | Mandatory Shared/Helper entries bound to their Launcher source files; verify included scripts before execution. |
| Initial content store | Allowlisted project recipes under `Projects/Showcase/`; qualified engine baseline under `Engine/Cooked/` | Mandatory marker/catalog, shader map, library and dependency manifest; include required engine textures. `recook.signal` is writable cook-completion state and is excluded. Seed only verified files. Never seed stale or unqualified selected-map output. |
| Rendering defaults | `Config/DefaultEngine.ini`, Shared/Defaults | Bind the source-owned file into the manifest. Installed Game/Editor verify it before applying shared per-user settings; missing/corrupt defaults stop startup. |
| Symbols / private intermediates | Separate archive / private build tree | AssetCooker requests shader debug information; DXIL PDBs stay outside cooked products. The existing ShaderCompiler `--debug-artifacts <private-dir>` route retains compile/source records for DXIL and SPIR-V correlation. No PDB, private source/path, cache, build tree or development executable belongs in the consumer archive. |

Release identity remains `0.1.0`, publisher Pawel Lisowski, copyright 2026. Use the existing identity owner; source and package signing identities must agree. Public window/display behavior and the laptop performance criterion remain in Release Scope. Do not replace the 30 FPS proof with this input inventory.

### Package And Writable Paths

The archive preserves CMake's binary layout relative to `artifacts/dev`. `ReleaseProducts.cmake` derives paths from target output properties; it does not maintain a second package layout. Each product keeps its app-local DLL/plugin closure.

| Product | Path under `artifacts/dev` or the extracted package |
| --- | --- |
| Launcher | `Tools/SparkleLauncher/Binaries/ShippingEditor/SparkleLauncher.exe` |
| Editor | `Projects/Showcase/Binaries/ShippingEditor/ShowcaseEditor.exe` |
| Game | `Projects/Showcase/Binaries/ShippingGame/ShowcaseGame.exe` |
| Cook helpers | `Tools/<tool>/Binaries/ShippingEditor/<tool>.exe` |

Shared engine targets emit to `Engine/Binaries/<module>/<profile>` when enabled; their required DLLs are staged beside each consuming executable. Static/import libraries remain separate development outputs and are excluded from the product archive.

Portable CMake remains under `Tools/CMake/`. Core's `ProductPaths` selects the source or installed root once; Launcher run/cook operations and AssetCooker use those functions. A source checkout uses its normal content and generated outputs. An installed package verifies manifest membership and uses its separate writable content store.

### Work With Precompiled Binaries

Import a verified archive for the checkout's exact Git revision, then use Launcher normally:

```powershell
& ./CMake/Packaging/UsePrecompiled.ps1 -Archive <archive.zip> -Sha256 <published-sha256>
artifacts/dev/Tools/SparkleLauncher/Binaries/ShippingEditor/SparkleLauncher.exe
```

Select the Shipping profile for these precompiled products. To replace only selected products, add `-Products SparkleLauncher,ShowcaseEditor`. The import verifies archive membership, hashes, metadata and dependency closure before replacing complete product directories. Failed replacement restores previous directories; an unfinished rollback retains them in the named transaction directory. Close affected products and finish active builds before importing.

Local compilation writes the same executable/DLL locations. It does not switch to another binary directory or consult an installed manifest. Import leaves source, cooked content, user settings and symbols unchanged; use the existing sync/cook operations for content. Development and Debug profiles keep their own paths and require their own builds. The first precompiled route uses the six frozen Shipping products; no published archive or automatic download feed exists yet.

Source use still requires Git for archive-to-checkout identity and CMake for content sync. Visual Studio, Qt and the Vulkan SDK are build prerequisites; running precompiled products uses their staged runtime dependencies and the graphics driver. Cook readiness checks the helpers beside their executables, independently of a configured build tree.

This follows Unreal's [precompiled-binary workflow](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-precompiled-binaries-in-unreal-game-sync-for-unreal-engine) and [shared binary locations](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-directory-structure). Sparkle reuses CMake and its manifest verifier rather than adding Unreal Game Sync, Perforce or a second build system.

Selected source and cooked content live under `%LOCALAPPDATA%/SparkleEngine/Content/Showcase/`, outside the extraction. Downloads are explicit Launcher selections, verified and transactionally published by the existing acquisition/cook owners. Keep source and cooked subtrees distinct; use the existing project/asset identities without introducing asset or shader versions. Existing settings/log owners remain per-user. The installed route implements this layout; PD-3 still must qualify offline use, read-only extraction, containment and helper discovery. Source workspaces keep their existing source/cooked paths.


### Reviewed Package Inputs

The [package owner](#current-package-entry) consumes a reviewed JSON input file. Keep it outside the source snapshot, under ignored release evidence. CMake generates exact product paths under `build/private/release-products/<configuration>.json`; package membership is not inferred from development directories.

Prepare those inputs with the same owner:

```powershell
cmake -S . -B build
powershell -NoProfile -File CMake/Packaging/Package.ps1 -AuditInputs
```

The audit retains `artifacts/releases/0.1.0/.partial-<id>/report.json`: binary/DLL hashes and imports, compiler-selected app-local CRT hashes/signatures and native PE/PDB identities for the six configured products. Missing or mismatched inputs remain blocked. It exits 1 without building, cooking or publishing an archive; discovery does not approve rights, complete dependency closure or shader symbols. Build the named targets in their declared configurations before repeating the audit. Shipping MSVC profiles trim the repository prefix from source names and retain only PDB filenames in every linked product, including compiled dependencies. Private PDBs stay separate; recheck source checksums and binary paths after rebuilding.

| Input field | Required meaning |
| --- | --- |
| `Revision`, `SourceInputsSha256` | Exact Git revision and deterministic hash of repository-relative tracked/untracked nonignored file hashes, including deletions. The admission run retains `source-inputs.json` and the fingerprint in `report.json`; review that snapshot before admitting payloads. |
| `Admissions` | `DependencyClosure`, `SymbolCorrelation`: each names repository-relative `Report`, `Size`, `Sha256`. The hashed JSON owner receipt carries `Gate`, `Status`, `Revision`, `SourceInputsSha256`, `PayloadSha256`; it must pass for the exact source and payload. `PayloadSha256` hashes the UTF-8 bytes of the generated manifest's compressed PowerShell JSON (`ConvertTo-Json -Depth 20 -Compress`), including membership, file hashes, products and system imports. Changing any of those inputs requires new receipts. A license filename or successful build is insufficient. |
| `Files` | Explicit complete allowlist; each entry has repository-relative `Source`, package-relative `Path`, `Size`, lowercase `Sha256`, `Component`, `Role`, `Permission`, `Owner`, logical `Origin`, SPDX `License`, package-relative `Notice`, `Signature`. Admission verifies these reviewed bytes. After build/cook, only configured executable and admitted cooked-output hashes are refreshed through the output owner; external inputs remain pinned. Membership, terms and source identity cannot change. No absolute source path goes into the package manifest. |
| `SystemImports` | Exact DLL names supplied by Windows or the supported graphics driver; each has `Name`, `Owner` (Windows/GraphicsDriver), `Requirement`. CRT, Qt, NVIDIA plugins and compiler-helper DLLs cannot use this exemption. Verify the declared import closure before assembly; qualify it on the minimum machine against the resulting archive. It never scans the host to supply missing files. |

Components are Launcher, Editor, Game, the three admitted cook helpers, CMake, Engine and Shared. Roles are Runtime, Helper, EngineContent, Recipe, Defaults, Notice and Metadata. Permission is ReadOnly. Signature policy is UnsignedBuild, VendorSigned or NotApplicable (non-PE only); unsigned first-party archives are internal development products. VendorSigned bytes require valid Authenticode. Required entries include all six generated executables, `Tools/CMake/bin/cmake.exe`, `notices/ThirdPartyNotices.txt`, `manifests/sparkle-sbom.spdx.json` and `manifests/sparkle-provenance.json`. Notices must themselves be admitted members. The consolidated notice retains compiled libraries, DXC/Slang/SPIR-V components and Microsoft terms; it does not authorize public distribution. Windows 11 supplies `D3DCompiler_47.dll`; the package does not ship an extra copy.

The SPDX 2.3 SBOM must identify its document/software components and enumerate every payload file with SHA-256 and the manifest license. Its own file and the provenance document are omitted from that list to avoid digest cycles; the package manifest binds both. Provenance names `Version`, `Revision`, `SourceInputsSha256` and the six exact `Products` (Component/Target/Configuration/Path). Static/embedded dependency completeness, redistribution terms and symbol correlation still require owner review. The verifier checks these bindings; it does not infer grants or replace a complete SPDX schema check.

The manifest owns `ProjectPath=Projects/Showcase`, `EnginePath=Engine`, `ContentStore=Content/Showcase`. ContentStore is relative to the platform per-user SparkleEngine root. Core uses it for `Cooked/`; immutable recipes include `.sparkle-project` and `Levels.catalog`. Recipe/engine membership is checked before assembly. Launcher store population and package-relative helper/product discovery are implemented but remain unqualified against a release archive. Runtime acceptance follows that archive through CHK-PKG-02; no archive-bound runtime receipt is required before archive creation. Current input admissions remain unresolved.

### Content And Source Distribution

- Product archives carry engine-owned mandatory defaults and only legally admitted content. A requested map remains available through a verified user-selected acquisition recipe; no automatic first-launch download is required.
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
- Sign staged first-party PE files with the issued certificate's explicit thumbprint, SHA-256 file digest and RFC 3161 SHA-256 timestamp at `http://time.certum.pl/`, as in [Certum's cloud signing instructions](https://files.certum.eu/documents/manual_pl/Podpisywanie_narzedziem_signtool_i_jarsign_Code_Signing_w_chmurze.pdf). Keep valid third-party signatures. The package owner supplies the allowlisted file paths; signing is FR-02 work after issuance. Verify every signature with Authenticode policy, timestamp and expected publisher. Unsigned local builds are development evidence.
- No configured signing certificate exists on the audited host. The route is decided; execution remains unavailable until issuance and SimplySign activation. The [candidate report](../../../Acceptance/FirstRelease.md#current-fr-01-blockers) records this distinction. A self-signed certificate does not close it.

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

Package and native-incident acceptance remain open.

### Current Local Route

**Evidence:** [Product reports](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) retain archive-bound export/symbol/review checks and outstanding adoption/native/replacement proof. Public instructions: [recovery/privacy](../../../../SUPPORT.md), [known issues](../../../../KNOWN_ISSUES.md), [security](../../../../SECURITY.md).

BuildAndPackaging owns `CMake/Packaging/{ExportDiagnostics,IncidentIdentity,ReviewRelease}.ps1`. [Release Inputs](#release-inputs) defines shipped helper/policy membership; review remains a maintainer source-side task.

| Responsibility | Current owner and boundary |
| --- | --- |
| Logs/retention | Core Logger rotates logs; Launcher owns operation logs. [Public retention/removal rules](../../../../SUPPORT.md#retention-and-replacement) apply. Keep private symbols with candidate evidence through release review. |
| GPU faults | D3D12 resource service drains supported bounded DRED records into logs; Vulkan records device loss. No automatic recovery. |
| Export | Local fixed projection of verified package identity and counts; preview digest binds consent. Originals stay local. [User command and privacy limits](../../../../SUPPORT.md#preview-and-export-local-diagnostics). |
| Symbols | Shipping MSVC emits `/Zi`/full PDBs into the separate symbol root. DbgHelp checks PE/PDB GUID-age; embedded paths are excluded. Archive-to-companion hashes and native source attribution are separate checks. |
| Crash/hang | User-reported categories; automatic collection unavailable. Native incident rehearsal remains required. |
| Review | Pawel Lisowski reviews each proposed release/replacement and closeout. Unresolved S0/S1 blocks approval. Results are local review records; publication and aggregate acceptance are separate. |

Replacement review requires verified old/new archives, different versions/hashes, unchanged three-product configuration scope and a replacement ledger retaining known IDs/severity. Critical defects must be resolved with matching candidate receipts. Review never mutates products or user state. Replacement/withdrawal invalidates all six product reports and affected release gates. [Manual rollback/settings rules](../../../../SUPPORT.md#retention-and-replacement) apply; interrupted replacement and public withdrawal remain unproved.

Invoke `ReviewRelease.ps1` with `-PackageRoot`, `-ArchivePath`, `-ArchiveSha256`, `-LedgerPath`, `-Decision Approve|Replace|Withdraw` and an existing local `-OutputDirectory`. Replacement also needs `-ReplacementRoot`, `-ReplacementArchive`, `-ReplacementSha256`, `-ReplacementLedgerPath` and a state policy.

Ledger: `ArchiveSha256` and `Defects` with `Id: ISSUE-<number>`, `Severity: S0..S3`, `Disposition: Unresolved|Resolved`. Resolved entries require `EvidencePath` to a bounded receipt with matching `ArchiveSha256`, `DefectId` and `Status: PASS`. Only its hash is exported.

### Diagnostics: Capability Identity

| ID | First-release boundary |
| --- | --- |
| `ADOPT-01` | Public identity, requirements, controls, first use, reset and exit for Launcher, Editor, Game and source adoption. |
| `ADOPT-02` | Actionable local startup/content/backend failure and self-service recovery. |
| `ADOPT-03` | Bounded, inspectable local diagnostics and candidate/symbol correlation; intake/upload service excluded. |
| `ADOPT-04` | Finite maintainer release review and immutable replacement/withdrawal; ongoing response/maintenance service excluded. |

### Diagnostics: Capability Boundary

A non-author can reach the promised product result or understand its failure without private guidance. Core/RHI/Application own diagnostic production; release delivery owns public instructions, local-data privacy, candidate-to-symbol identity and release review. No reporting channel or automatic upload is implied.

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
| `AC-ADOPT-01` | A non-author reaches onboarding for all three products and source adoption; identifies exact version, requirements, controls, reset/exit, known issues and the absence of a user support service without private guidance. | PD-2 source adopter; PD-4 three-product self-service |
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
| `CHK-ADOPT-01` | `AC-ADOPT-01`; `FM-ADOPT-01` | Non-author packaged-product and source journeys through success and a missing prerequisite. Private repair, dead onboarding or misleading support claims fail. |
| `CHK-ADOPT-02` | `AC-ADOPT-02`, `AC-ADOPT-03`; `FM-ADOPT-02`, `FM-ADOPT-03` | Isolated local startup/crash/hang/GPU diagnostics, with private/mismatched/oversized/unavailable fixtures. Inspect identity, symbols, redaction, bounds and absence of network uploads. |
| `CHK-ADOPT-03` | `AC-ADOPT-04`; `FM-ADOPT-04` | Review exact candidate known-defect ledger and public policy, then inject an unresolved critical defect and a false support promise. Both must block closeout; no live inbox submission is required. |
| `CHK-ADOPT-04` | `AC-ADOPT-05`; `FM-ADOPT-05` | Apply/interrupt a versioned replacement on copies with representative user state; verify applicability, atomicity, state policy, rollback/withdrawal and unchanged public bytes. |

Candidate results remain in `FCR-PROD-06`; fixtures cannot close an unavailable required native or adoption check. Missing intake channels alone are not a blocker.
