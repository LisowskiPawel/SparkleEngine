# First Release Inputs

Status: release input contract for FR-01/FR-02; named inputs and obligations, not a staged-package or signing result.

This page owns the input choices needed to implement the package spine. [Release Scope](../../../Acceptance/ReleaseScope/README.md) owns product promises; [First Release](../../../Acceptance/FirstRelease.md) owns candidate verdicts. Keep the supported seven maps and Launcher entry; start Empty until the user selects content.

**Open-source license boundary:** Sparkle-owned code remains MIT, including commercial use. Free educational delivery describes the maintainer's intent, not an added use restriction. Third-party content and SDK terms remain separate.

**Free educational delivery (2026-10-11):** preserve all approved features, both backends and seven map choices. License compliance changes notices, source availability and how content is acquired; it must not disable DLSS/Ray Reconstruction, remove a map, substitute assets or silently lower quality. Selected publisher downloads stay inside Launcher, with progress, retry and offline reuse after acquisition. A free release still needs the applicable component terms. Certificate enrollment is deferred to the final pre-release work before signed candidate/adoption checks; it does not block input discovery or unsigned internal package development.

## Product And Helper Membership

| Component | Source product/profile | Admission rule |
| --- | --- | --- |
| Sparkle Launcher | `SparkleLauncher`, `ShippingEditor` | Mandatory; package-relative discovery and selected sync must work without a source checkout. Include Qt's exact release DLL/plugin closure and its LGPL notices/source/relink obligations. |
| Sparkle Editor | `ShowcaseEditor`, `ShippingEditor` | Mandatory; use the same identity/configuration owner as Game. Include its exact declared runtime closure; no undeclared developer SDK requirement. |
| Sparkle Game | `ShowcaseGame`, `ShippingGame` | Mandatory; no Editor/tool-only payload. Include all eight pinned Streamline runtime DLLs required by the provider owner, including `sl.dlss_d.dll`; keep NVIDIA/NGX terms and signatures. |
| Selected-content cook helpers | `AssetCooker`, `TextureCooker`, `ShaderCompiler`, `ShippingEditor` | All three helpers are mandatory for the selected source-content sync/cook journey, plus their exact redistributable DLL/SDK closure. Headers, compiler development tools, examples and arbitrary SDK directories are excluded. |
| Acquisition helper | Portable CMake `4.3.3`, used by the existing sync owner; publisher archive SHA-256 `935ade9e5e8723583c07f44c5592cea2a1c8f65c56ca7e07b34c025c880e0bd6` | Mandatory for the admitted acquisition scripts; include its license. Package-relative discovery and byte-level qualification are FR-02 work. An author-machine CMake installation is not the consumer contract. |
| Symbols / private intermediates | Separate archive / private build tree | No PDB, source/private absolute path payload, cache, build tree or development executable in the consumer archive. |

Release identity remains `0.1.0`, publisher Pawel Lisowski, copyright 2026. Use the existing identity owner; source and package signing identities must agree. Public window/display behavior and the laptop performance criterion remain in Release Scope. Do not replace the 30 FPS proof with this input inventory.

## Package And Writable Paths

The immutable extraction contains `launcher/SparkleLauncher.exe`, `editor/ShowcaseEditor.exe`, `game/ShowcaseGame.exe`, the three helpers under `tools/<tool>/<tool>.exe`, and portable CMake under `tools/CMake/`. Each component carries its declared release DLL/plugin closure. The package manifest owns these paths; do not reconstruct them from development artifacts.

Selected source and cooked content live under `%LOCALAPPDATA%/SparkleEngine/Content/Showcase/`, outside the extraction. Downloads are explicit Launcher selections, verified and transactionally published by the existing acquisition/cook owners. Keep source and cooked subtrees distinct; use the existing project/asset identities without introducing asset or shader versions. Existing settings/log owners remain per-user. FR-02 must implement this layout and qualify offline use, read-only extraction, containment and helper discovery. These target paths do not describe current source-workspace behavior.


## Reviewed Package Inputs

The [package owner](PackagingAndInstallation.md#current-package-entry) consumes a reviewed JSON input file. Keep it outside the source snapshot, under ignored release evidence. CMake generates exact product paths under `build/private/release-products/<configuration>.json`; package membership is not inferred from development directories.

| Input field | Required meaning |
| --- | --- |
| `Revision`, `SourceInputsSha256` | Exact Git revision and deterministic hash of repository-relative tracked/untracked nonignored file hashes, including deletions. The admission run retains `source-inputs.json` and the fingerprint in `report.json`; review that snapshot before admitting payloads. |
| `Admissions` | `Rights`, `DependencyClosure`, `PackageDiscovery`, `ContentStore`, `SymbolCorrelation`: each has `Status`, repository-relative `Report`, `Size`, `Sha256`. Only an owning candidate review can record `PASS`; a license filename or successful build is insufficient. |
| `Files` | Explicit complete allowlist; each entry has repository-relative `Source`, package-relative `Path`, `Size`, lowercase `Sha256`, `Component`, `Role`, `Permission`, `Owner`, logical `Origin`, SPDX `License`, package-relative `Notice`, `Signature`. No absolute source path goes into the package manifest. |

Components are Launcher, Editor, Game, the three admitted cook helpers, CMake, Engine and Shared. Roles are Runtime, Helper, EngineContent, Recipe, Defaults, Notice and Metadata. Permission is ReadOnly. Signature policy is UnsignedFirstParty, VendorSigned or NotApplicable (non-PE only); unsigned first-party archives are internal development products. VendorSigned bytes require valid Authenticode. Required entries include all six generated executables, `tools/CMake/bin/cmake.exe`, `manifests/sparkle-sbom.spdx.json` and `manifests/sparkle-provenance.json`. Notices must themselves be admitted members.

The SPDX SBOM, provenance, complete dependency closure, redistribution terms and symbol correlation require owner review of exact bytes. The verifier enforces the frozen file contract; it does not infer a legal grant, synthesize a SBOM, qualify a helper from its filename, or establish runtime discovery. Current admissions remain unresolved. Do not create passing receipts until their checks have run.

## Content And Source Distribution

- Product archives carry engine-owned mandatory defaults and only legally admitted content. A requested map remains available through a verified user-selected acquisition recipe; no automatic first-launch download is required.
- Modern Sponza Base/Curtains and classic Sponza are acquisition-only until bundled redistribution is established. Exclude source, textures, cooked derivatives and their thumbnails from product archives. Publisher terms remain applicable to users; an acquisition recipe is not a new license grant.
- Classic Sponza uses the pinned Khronos recipe under `Assets/Meshes/Sponza/SourceFiles.txt`; the level reads `Sponza/Publisher/glTF/Sponza`. The replaced bundled model/texture files are removed from the current source tree. The acquired tree is gitignored.
- Cornell consumes the tracked self-contained GLB with `COPYRIGHT.txt` and conversion attribution. Its publisher OBJ ZIP is optional for regeneration. Do not turn the unavailable regeneration archive into a normal launch prerequisite.
- Retain the selected Cesium/plant/chess copyright, attribution and logo notices; retain LPS Head's publisher/conversion notice. Retain Poly Haven CC0 environment provenance and Font Awesome's font license. Package copies must preserve notices for source and cooked derivatives.
- Public release source is a reviewed exported archive from the exact candidate. `.gitattributes` excludes unqualified Sponza bytes, the excluded DamagedHelmet payload and development thumbnails without reviewed export receipts; new acquisition recipes/notices must be committed and present before export acceptance. Raw repository clones/history are a separate distribution surface, not certified by `export-ignore`.

**Public repository decision (2026-10-11):** [SparkleEngine](https://github.com/stoleckipawel/SparkleEngine) remains the main public, open-source repository with its existing history. The owner superseded the private-history plan; no private source repository or separate publication workflow is required. The owner requested removal of the extra SparkleEngine-Source repository; it is not a development or release destination. Keep production features, seven map definitions and acquisition recipes available publicly. Review historical asset rights separately; public visibility and export exclusions do not establish redistribution permission.

## Supply Chain And Signing

| Input family | Distribution decision / proof still required |
| --- | --- |
| Permissive source dependencies and PIX | Carry exact retained copyright/license notices for the compiled components, including embedded third-party notices. An entire dependency checkout is not the admitted payload. |
| Streamline / NGX / Reflex | Streamline's MIT notice does not license every bundled plugin. Preserve the pinned NGX/Reflex runtime terms and applicable notices; distribute only admitted application runtime binaries, not a standalone SDK. The [NVIDIA component notice](../../../../CMake/Notices/NVIDIA/README.md) preserves the separation from Sparkle's MIT license and is staged with the full retained license texts. Product distribution terms must preserve their restrictions separately from Sparkle's first-party MIT license. The pinned NGX license's distribution section requires this; release terms and their acceptance remain unqualified. Do not include `sl_nvperf.dll` or development capture tools without separate admission. |
| Qt `6.11.1` | Use dynamically linked LGPL-eligible modules/plugins; keep notices/license text and a prominent Qt notice, the exact corresponding library source and a documented replacement/relink route. [Qt's LGPL obligations](https://www.qt.io/development/open-source-lgpl-obligations) apply even with dynamic linking. The source companion below is identified. Launcher now stages LGPL/GPL license texts and [Qt source/replacement instructions](../../../../CMake/Notices/Qt/README.md); root onboarding prominently identifies Qt. Package source publication, full third-party closure and replacement/relink execution still need acceptance. |
| MSVC / DXC / portable CMake | Package only the exact redistribution-authorized runtime/compiler-helper files needed by the admitted journeys, with their terms. Local SDK installation and `windeployqt` output are discovery, not an allowlist or a license receipt. Qualify app-local CRT for the portable, standard-user promise; a staged `vc_redist.x64.exe` does not prove that journey. |

- Resolve declared Git dependencies at full commit hashes; hash-check downloaded SDK archives and both Font Awesome inputs. Wrong cache bytes are a failure, never a substitute input. Keep the failed cache for diagnosis.
- Produce a component inventory from the exact staged bytes: first-party products, linked/static third-party components, dynamic DLLs/plugins, helpers, source/cooked content and notices. Use SPDX JSON for the package SBOM with exact versions/revisions/hashes and relationships; no guessed `NOASSERTION` license is admission evidence. The dependency audit feeds this inventory, not a directory copy of every downloaded SDK.
- Preserve the exact installed toolchain/SDK versions and prerequisites independently of redistributable components. Dependency license files and inline notices (including MikkTSpace, tinyexr and CMP_Core) must reach the notice bundle. NVIDIA SDK redistribution conditions and Qt LGPL/source/relink obligations must be satisfied for their admitted bytes.
- Signing choice (owner delegated selection, 2026-10-11): **Certum Standard Code Signing in the Cloud, individual subscriber Pawel Lisowski**, through SimplySign Desktop and Windows SDK SignTool. This preserves the named publisher without assuming eligibility for an open-source-only certificate. [Certum's individual enrollment](https://support.certum.eu/en/code-signing-required-documents/) requires purchase and identity/address verification. Enrollment, payment and issuance have not occurred; no account or certificate was fabricated. The maintainer completes verification directly with Certum; identity documents and credentials never enter this repository.
- Sign staged first-party PE files with the issued certificate's explicit thumbprint, SHA-256 file digest and RFC 3161 SHA-256 timestamp at `http://time.certum.pl/`, as in [Certum's cloud signing instructions](https://files.certum.eu/documents/manual_pl/Podpisywanie_narzedziem_signtool_i_jarsign_Code_Signing_w_chmurze.pdf). Keep valid third-party signatures. The package owner supplies the allowlisted file paths; signing is FR-02 work after issuance. Verify every signature with Authenticode policy, timestamp and expected publisher. Unsigned local builds are development evidence.
- No configured signing certificate exists on the audited host. The route is decided; execution remains unavailable until issuance and SimplySign activation. The [candidate report](../../../Acceptance/FirstRelease.md#current-fr-01-blockers) records this distinction. A self-signed certificate does not close it.

### Qt Source Companion And Component Terms

The audited Launcher uses Qt Core, Gui, Network, Widgets and Svg with their platform/style/image/TLS plugins. Preserve these modules and plugins. Qt's installed vendor SBOMs identify the following source archives; publish the matching sources as an optional companion download alongside the products. Normal users need not download or build Qt source. Recheck this closure if deployed modules change.

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

## Threat And Failure Boundary

Source/dependency acquisition, publisher content, cache bytes, staged files and signing credentials are distinct trust boundaries. Verify hashes and contained paths before publication; do not execute tools from user content, accept traversal/reparse escapes, stage unknown DLLs, or weaken a digest after failure. Missing content stops the requested map visibly; Empty must not masquerade as successful activation of that map. Keep logs local and preserve the approved offline/privacy/no-support promises.

FR-02 implements transactional stage/verify/archive and package-relative discovery from these named inputs. FR-01 inspection or a successful local source build does not prove the consumer package, complete notices/SBOM, trusted signature or clean-machine journey.
