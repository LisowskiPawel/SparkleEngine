# Release Scope Evidence

**Current FR-01:** [ITER-REL-01-12](#current-candidate-baseline-iter-rel-01-12) passes the local baseline at `fec29052?`: all three products and required cook helpers build, owner checks pass, Empty cook/activation passes and cached replay preserves hashes. Rights and exact package payload admission remain blocked. Earlier records keep their original candidate and scope.

**Status:** dated source audit and iteration records; release acceptance remains blocked

**Responsibility:** retain source identities, rights observations and historical checks behind the [scope decisions](README.md).

**Snapshot:** reconciled 2026-10-10 at `aa9fcd94f219e8a3d4dbe7c51457680ad9300783`. Earlier iterations retain their own revisions and dirty-state descriptions.

These records prove only the inspection or document checks they name. Current product choices and next actions belong in [Release Scope](README.md#approval-and-next-work); this page does not grant redistribution permission or build/runtime acceptance.

## Current Candidate Baseline: ITER-REL-01-12

**Candidate:** `fec29052c6f6195547f2062da9def86e520dc0df`, clean start, 2026-10-11. Owner BuildAndPackaging / Acceptance; FR-01 / REL-01 / REL-02. `NS-OWNERSHIP`, `NS-EVIDENCE`, `NS-ADOPTION`; graphics targets preserved. Existing earlier records retain their own candidates. Risks: reusing stale baseline evidence or treating source/notice identity as asset permission; checks below bind the fresh path and leave unknown permissions blocked.

**Fresh path:** detached worktree `C:/src/SparkleFR01-fec29052`, initially absent build/dependency/artifact roots. Original branch/index/history are unchanged. Commands/environment/input revisions/hashes/output hashes: `build/validation/fr01-readiness-fec29052/`. Raw logs: `C:/src/SparkleFR01-fec29052-*.log`, enumerated in `control.json`. Retain the worktree and logs for review; generated outputs remain in its ignored build/artifact roots. No temporary harness source was added.

| Check | Result and limit |
| --- | --- |
| `CHK-REL0112-01` cold configure | **PASS:** VS 18 2026 x64 / Qt 6.11.1; configure exit `0`, 83.0 seconds. Fifteen fresh source groups inventoried, revisions and root-notice hashes retained. Zlib's generated deletion of tracked `zconf.h` is disclosed in `source-inputs.json`; it is not hidden as a clean dependency tree. |
| `CHK-REL0112-02` Shipping build | **PASS:** ShippingGame / ShowcaseGame; ShippingEditor / ShowcaseEditor, SparkleLauncher, AssetCooker, TextureCooker, ShaderCompiler; all build commands exit `0`. All three product PE identities are `0.1.0`, Pawel Lisowski. Editor build is now observed for this candidate; Editor activation is not. |
| `CHK-REL0112-03` owner checks | **PASS:** initial architecture/style checks, then existing `RunBaselineChecks.ps1 -Configuration ShippingEditor`, exit `0`; includes shader CLI validation and 37 typed registrations. No owner semantics changed. |
| `CHK-REL0112-04` cook / activation | **PASS, Empty only:** cook exits `0`, six products. Native Launcher subprocess exits `0`; new Game PID `23484` closes normally, exit `0`, with requested D3D12 / ShippingGame / Empty. First launch also closed normally but PowerShell failed to retain Launcher exit code; its receipt is retained, not counted as a fully passing probe. Corrected native exit capture is in `launcher-native.json` / `activation-native.json`. No rendered-frame, seven-map, FPS or clean-machine claim. |
| `CHK-REL0112-05` cache replay | **PASS:** `FETCHCONTENT_FULLY_DISCONNECTED=ON` configure and incremental ShippingGame build exit `0`. All 147 inventoried product/helper file hashes are unchanged; source status stays clean. This tests cached FetchContent replay, not an OS-level network denial or independent bit-identical rebuild. |
| `CHK-REL0112-06` input preparation | **PASS, identities:** portable CMake 4.3.3 archive matches publisher SHA-256 `935ade9e5e8723583c07f44c5592cea2a1c8f65c56ca7e07b34c025c880e0bd6`; version and license members retained. Installed compiler/CRT bytes are hashed; pinned DXC, Slang and SPIRV-Tools source notices retained. These observations do not prove the exact SDK binary/source mapping or complete transitive package/notice closure. The SDK licensing URL returned a site shell; it is not retained as a completed component license review. |
| `REL-01` / ordered `REL-02` admission | **BLOCKED:** no new applicable grant for historical Sponza bytes was found. [Khronos's model license](https://github.com/KhronosGroup/glTF-Sample-Assets/blob/main/Models/Sponza/LICENSE.md) separates model files under CryEngine terms from CC BY metadata. Existing Intel archive wording remains unresolved. Exact redistributable/helper admission is still unqualified; package-relative lookup and clean-machine archive tests remain FR-02 work. Certificate issuance is deferred. |

**Decision:** the current candidate has a complete recorded local baseline path. The stale-current-baseline finding is superseded. Full FR-02 admission is not approved: publisher rights and remaining exact payload/notice review cannot be inferred from successful builds. No asset replacement/removal, remote visibility change, history rewrite, archive publication or signing was performed.

## FR-02 Readiness Review: ITER-REL-03-03

**Control:** 2026-10-11; candidate `611974ecaadeb8be8502037f26ceaa73c3c752e2` plus retained FR-02 notice/prompt changes. Owner BuildAndPackaging / Acceptance; FR-02 / REL-03; `NS-OWNERSHIP`, `NS-ADOPTION`, `NS-EVIDENCE`; preserve graphics targets. Scope: source/license identity, deployment and admission review. Risk: wrong-version source instructions or stale deployed notices; detect with byte/version checks and dedicated deployment targets.

| Review | Result |
| --- | --- |
| Source and terms | **PASS, notice identity:** retained Qt Base source hash matches the prior receipt; both tracked LGPL/GPL texts match archive entries byte-for-byte. Instructions distinguish audited 6.11.1 source from other supported development versions and retain Qt replacement rights. NVIDIA restrictions remain component-specific. No complete package rights verdict follows. |
| Deployment | **PASS:** focused Launcher DevelopmentEditor build, root configure, `ShowcaseGameNvidiaNotices` in ShippingGame and `SparkleLauncherQtNotices` all exit `0`. All source/deployed notices match; `QtRuntimeVersion.txt` records `6.11.1`. Dedicated dependency targets refresh notices even without C++ changes. `build/validation/fr02-20261011/triple-review.json` retains hashes. No Game build was required for notice deployment. |
| Admission versus phase work | **BLOCKED for full admission:** historical asset rights, exact redistributable/helper closure and current-candidate baseline admission remain open. Package manifest/runtime discovery and transactional stage/archive are FR-02 implementation tasks; compatible Qt replacement, source-companion publication and exact-archive clean-machine runtime are package acceptance checks. Their absence alone is not a separate pre-implementation permission requirement. Certificate issuance stays deferred. |

**Decision:** notice deployment is verified; full FR-02 readiness is not claimed. Next permitted work is exact component/payload admission and the manifest/discovery design. No archive or clean-machine result exists. UTF-8/local-file links and `git diff --check` pass; no temporary test source introduced. Retained notices/build receipts are not public source-companion publication.

## Redistribution Notices: ITER-REL-03-02

**Candidate:** `611974ecaadeb8be8502037f26ceaa73c3c752e2` plus retained patch, 2026-10-11. Owner: BuildAndPackaging. Start state retains ITER-REL-03-01 report/prompt changes. Scope: fix missing Qt deployment notices and explicit NVIDIA component terms without dropping features; `NS-OWNERSHIP`, `NS-ADOPTION`, `NS-EVIDENCE`; graphics targets preserved. REL-03 remains unaccepted.

**Risk / acceptance:** do not apply NVIDIA restrictions to Sparkle MIT or Qt LGPL rights; do not describe source links as fulfilled corresponding-source publication. Qt notice staging must preserve exact license bytes and name source identities/replacement rights. Missing package review stays a blocker.

| Check | Result / boundary |
| --- | --- |
| Qt notice artifacts | **PASS:** LGPLv3/GPLv3 texts extracted from the previously hash-verified Qt Base source archive; Launcher deployment copies them and source/replacement instructions into `Notices/Qt`. Root README identifies Qt. Source-byte hashes and staged matches: `build/validation/fr02-20261011/qt-notice-stage.json`. |
| Launcher build | **PASS:** `cmake --build build/private/tools/SparkleLauncher --config DevelopmentEditor --target SparkleLauncher --parallel 4`, exit `0`; `SparkleLauncher.exe --help`, exit `0`. This is development deployment, not a portable package or compatible-library replacement test. |
| NVIDIA terms | Component-specific notice added alongside retained full SDK license texts through the existing Streamline staging owner. Root `cmake -S . -B build` exits `0`. No DLL or feature removed. The new NVIDIA notice copy command was generated but its product post-build staging was not run in this iteration. No end-user agreement enforcement or full redistribution verdict is claimed. |
| Remaining acceptance | **BLOCKED:** exact package payload/notice closure, corresponding Qt source publication, compatible Qt replacement, package-relative discovery and the clean-machine exact-archive route remain unfinished. Historical asset grants remain unresolved; no history or visibility changes were made. |

**Checks / cleanup:** license/source inspection, byte comparison, focused Launcher build/help, root configure, UTF-8/local-link checks and `git diff --check`. Retained build/notice receipts under `build/validation/fr02-20261011/`; no temporary source harness introduced. These results supersede the missing Qt notice artifacts only, not REL-01/REL-02/REL-03 admission.

## Package Spine Admission: ITER-REL-03-01

**Candidate:** `611974ecaadeb8be8502037f26ceaa73c3c752e2`, clean start, 2026-10-11. Owner: BuildAndPackaging / Acceptance. Scope: FR-02 / ProductAndDelivery PD-3 / REL-03 admission inspection. `NS-OWNERSHIP`, `NS-ADOPTION`, `NS-EVIDENCE`; graphics targets preserved, no evidence increase.

**Criterion / risk:** implement only from admitted payload identities and an agreed discovery contract. A repository-dependent archive or unreviewed redistribution is a failure, not an internal-package pass. Detect through input-owner and discovery inspection; stop before staging and keep source unchanged except the conflicting prompt correction.

| Check | Result / next action |
| --- | --- |
| Rights and payload admission | **BLOCKED:** ReleaseInputs still marks NVIDIA distribution terms, Qt prominent notices/source publication/replacement and complete admitted payload review unqualified. FirstRelease retains unresolved historical asset rights. Public MIT code and free delivery do not qualify those inputs. Finish the applicable terms/notice and payload review before staging. Certificate enrollment is deferred and is not this blocker. |
| Runtime discovery | **BLOCKED:** `RepositoryLocator.cpp` requires workspace/engine markers and `Engine`, `Tools`, `Projects` directories. Current repository lookup is not manifest-owned package lookup. ReleaseInputs names target package/content paths but no implemented consumer discovery route is established. Resolve that contract and its Launcher/Application/tool consumers before claiming repository-free operation. |
| Component contract | **PASS, document reconciliation:** corrected the old PD-3 execution prompt to include required Launcher/Editor/Game and only admitted acquisition/cook helpers, matching current scope and ReleaseInputs. Removed the conflicting blanket exclusion of Editor/Launcher. |
| Exact archive / clean standard-user route | **BLOCKED:** no FR-02 archive was produced; no archive hash, relocated runtime or independent clean-machine result exists for this candidate. No build/cook/package work ran after the stop condition. |

**Decision:** FR-02 / PD-3 / REL-03 remains BLOCKED at admission. No package owner implementation, release gate pass or phase advance is claimed. Inspection commands: `git status --short`, `git rev-parse HEAD`, scoped `rg`, reads of ProductAndDelivery, ReleaseInputs and RepositoryLocator; document check: `git diff --check`. Receipts are this report and the scoped prompt diff; no temporary harness or generated stage needs cleanup.

## Public Contributor Checks: ITER-REL-01-11

**Control:** 2026-10-11, owner BuildAndPackaging / Engineering, candidate `7cfceb32d10e487637ed482fbc80d6dcce5f88e2` plus retained patch. Start state: prior FR-01/open-source changes already modified `.gitattributes`, `.gitignore`, dependency/artifact CMake, FirstRelease, ReleaseScope, release inputs, roadmap and docs index; README, CONTRIBUTING and `.github` were untracked. Preserve them. Scope: contributor bootstrap, bounded CI and documentation; `NS-ADOPTION`, `NS-EVIDENCE`, `NS-OWNERSHIP`; graphics targets preserved with no evidence increase. Delivery: FR-01 / REL-01 / REL-02 source workflow, not gate closure.

**Risk / criterion:** avoid engine-wide acquisition during Launcher bootstrap and avoid presenting source checks as a qualified build. Acceptance: onboarding names the actual standalone owner and canonical path; automation invokes existing architecture assertions with no secrets/write permission; local links and whitespace pass. Failure controls: owner exit codes remain failures, job times out/cancels rather than passing; unavailable hosted/build results stay explicit. Maintainer owns review and recovery.

| Check | Result and boundary |
| --- | --- |
| `CHK-OSS11-01` bootstrap inspection | **PASS, source:** README now uses `Tools/Launcher` and `build/private/tools/SparkleLauncher`, matching artifact contract and standalone dependency allowlist. Commands were inspected, not fresh-built in this iteration. |
| `CHK-OSS11-02` owner invocation | **PASS:** `cmake -P CMake/ArchitectureBoundaryCheck.cmake`, exit `0`, no violations. An initial invocation with relative `SPARKLE_REPO_ROOT=.` failed because the script expects an absolute override; the final command lets the owner resolve its root. No check semantics changed. |
| `CHK-OSS11-03` workflow preparation | Commit-pinned checkout `de0fac2e4500dabe0009e67214ff5f5447ce83dd` verified against publisher tag `v6.0.2`. Ordinary PR trigger, read-only contents, credentials disabled, bounded job and cancellation. **BLOCKED for hosted execution:** workflow remains local/uncommitted; no hosted run or branch protection configured. No build, style, shader, asset or GPU coverage claimed. |
| `CHK-OSS11-04` document/export hygiene | `.gitattributes` retains active normalization and export exclusions while removing unused template comments. UTF-8, local links/anchors and `git diff --check` pass. No tracked asset or generated file removed by this slice. |

**Decision:** contributor setup and local architecture checks pass within these boundaries. REL-01/REL-02 release verdicts are unchanged. Extra-repository deletion remains unavailable as recorded in ITER-REL-01-10; no blog, repository visibility, signing, original history or remote branch was changed. No temporary harness or new test source was introduced.

## Single Public Repository Setup: ITER-REL-01-10

**Candidate:** `7cfceb32d10e487637ed482fbc80d6dcce5f88e2` plus retained worktree changes, 2026-10-11. Owner decision: keep SparkleEngine and the existing blog; remove the extra source repository.

| Check / decision | Result and boundary |
| --- | --- |
| Public contributor setup | Root README, contribution guide, defect and PR templates route public development to SparkleEngine. MIT commercial-use permission is retained; third-party terms remain separate. PDBs are ignored for future additions. No feature, map or build target is changed by this slice. |
| Extra repository removal | **BLOCKED:** GitHub DELETE for `stoleckipawel/SparkleEngine-Source` returns HTTP `403`; the credential has `repo` but lacks required `delete_repo` scope. Main SparkleEngine remains public; blog and original history are untouched. Owner must delete the extra repository through GitHub Settings. No deletion is claimed. |
| Verification boundary | Documentation and whitespace checks only; no build/cook/Game/performance result or hosted CI qualification. Existing rights and package acceptance blockers remain. |

## Public Repository Restored: ITER-REL-01-09

**Owner decision, 2026-10-11:** keep the original project public and open source; supersede the private-history/public-source split.

**Candidate:** `7cfceb32d10e487637ed482fbc80d6dcce5f88e2` plus retained worktree changes. This iteration changes repository visibility and documents only.

| Check / decision | Result and boundary |
| --- | --- |
| Original repository visibility | **PASS:** GitHub API changed `stoleckipawel/SparkleEngine` from private to public; an unauthenticated API read confirmed public access. Existing history, branch, index and origin are preserved. Receipt: `build/validation/fr01-nonprofit-20261011/repository-restored-public.json`. |
| Source workflow | **SUPERSEDED:** the private-history plan in ITER-REL-01-08 is no longer approved. The original SparkleEngine repository is primary; the extra source repository is no longer part of the workflow. No history rewrite or feature/map removal accompanies this decision. |
| Release gates | **BLOCKED:** historical asset rights and admitted-component redistribution review remain unresolved. Public source availability is restored; it does not grant asset permissions. No build, cook, Game, performance or package result is added. Certificate enrollment remains scheduled before final release. |

## Feature-Preserving Licensing: ITER-REL-01-08

**Candidate:** `7cfceb32d10e487637ed482fbc80d6dcce5f88e2`, initially clean, plus this iteration's retained patch, 2026-10-11. Owner: BuildAndPackaging / Acceptance. Scope: preserve the approved features/maps while fixing runtime notices, identifying Qt corresponding source and scheduling certificate enrollment. `NS-OWNERSHIP`, `NS-EVIDENCE`, `NS-ADOPTION`; preserve graphics capability targets, no rendering-level evidence increase. Controls, exact commands, hashes and cleanup: `build/validation/fr01-nonprofit-20261011/`. Risks: mistaking notice/source preparation for package acceptance; removing features or imposing undisclosed Windows warnings. Missing notices fail before staging; unknown rights remain blocked rather than substituted or waived.

| Check / decision | Result and boundary |
| --- | --- |
| `CHK-FR0108-01` NVIDIA staging | **PASS:** existing `sparkle_stage_nvidia_streamline_runtime` owner configures/builds in a focused Release probe; all eight DLLs and five notice files copy byte-for-byte. SDK acquisition now requires the notices. Missing notice rejects configuration, exit `1`; restored SDK configuration exits `0`. `notice-verification.json`, `notice-negative.json`, raw configure/build logs. No feature switch or DLL removed. First negative attempt stopped at artifact-namespace validation; corrected probe reaches the intended missing-notice detector. No new permanent test target. |
| `CHK-FR0108-02` Qt source identity | **PASS, provenance:** publisher qtbase/qtsvg ZIP SHA-256 and embedded `.tag` revisions match installed vendor SBOM source revisions. License texts/SBOMs are retained as a compliance companion. All 70 checked Qt DLLs match vendor SBOM SHA-1 after reconstructing the unsigned PE (remove trailing certificate, zero checksum/security-directory fields); raw signed-file hashes differ and are retained separately. This identifies signed vendor content, not bit-identical local Qt builds or a full license verdict. `qt-source-verification.json`, exact archives and `qt-compliance/`. |
| `CHK-FR0108-03` scope and UX | **PASS, source boundary:** seven map choices, rendering code, feature switches, dependencies and Launcher controls remain unchanged by this patch. No map replacement, quality fallback or first-launch legal modal was added. Source companions are optional downloads for users, separate from the product. Actual package-relative selected sync/offline use and compatible Qt replacement remain FR-02/consumer checks, not source-review claims. |
| Signing schedule | **Owner approved:** certificate enrollment/payment/identity verification happens in final pre-release work before signed candidate freeze/adoption (`FR-06`). Its absence does not block FR-01 or unsigned internal packaging. The final public consumer release still requires trusted signatures and clean-machine trust/UX checks. The [roadmap](../../Strategy/Roadmap.md#where-we-are-and-what-is-next) owns scheduling; Windows warnings on unsigned development builds are not promised absent. |
| Public history remediation | **SUPERSEDED by ITER-REL-01-09. Historical observation:** owner approved keeping old history private and publishing reviewed clean source. `stoleckipawel/SparkleEngine` is now private (unauthenticated API `404`); [SparkleEngine-Source](https://github.com/stoleckipawel/SparkleEngine-Source) is public. Initial public commit `bce458263ef920f7063435cc55c9d2eed37f542d` has no parents; 2664 published files exclude the targeted unqualified asset payloads and PDBs. Reviewed ZIP SHA-256 `84a96ba25f32f0a1d485ce8dd11a1dfee33e27332939bfea3dc0a2c5e0bf6e25`; 3222 ZIP entries include directories. `.gitattributes` now excludes generated compiler PDBs. Root history/index/branch/origin remain intact. This stops exposure through the controlled repository; it supplies no retrospective grant and cannot recall third-party copies. Exact API/command receipts: `repository-migration.json`, `public-source-git.log`, `source-export.json`. |
| Gate outcome | **Historical verdict; repository split superseded by ITER-REL-01-09.** The previous public-history and immediate-certificate blockers are **SUPERSEDED** by the executed repository split and owner-approved enrollment schedule. License routes/source/notices preparation are verified; no asset grant was fabricated. `REL-01` / `REL-02` final candidate admission remains **BLOCKED** pending complete admitted-component notice/terms review and binding baseline checks to the revised public source. Prior clean baseline evidence stays under its original candidate. Full notice bundle, Qt prominent notice/source publication/replacement journey, NVIDIA distribution terms, signed package and consumer UX require their owning later checks. |

**Validation and cleanup:** exact focused CMake command arrays and source/output hashes are retained in `commands.json` and `notice-verification.json`; archive hashes/revisions in `qt-source-verification.json`. Documentation UTF-8/local links/anchors and `git diff --check` are recorded in `document-verification.json`. Temporary probe sources and scripts are removed. Automatic approval review rejected recursive cleanup and the explicit executable deletion without a more specific reason; the ignored probe executable/generated build trees remain, disclosed in `cleanup.json`. Receipts, SDK notice copies, reviewed source ZIP and Qt source/license companions remain for review. No Game/Editor build, cook, runtime, native backend, FPS, independent adoption, product-release publication or rewrite of the original history was performed. The explicitly authorized source-repository creation and visibility changes above were performed. Do not promote earlier baseline results to this revised candidate.

## FR-01 Closure Work: ITER-REL-01-07

**Decision, 2026-10-11:** baseline observations **PASS**; `REL-01` and ordered `REL-02` admission **BLOCKED**. No FR-02 acceptance or publication. The remaining external actions are listed below; do not repeat the closed Cornell/dependency/baseline blockers.

**Exact candidates:** root `9734e6d469c09f65fcfe66ce3ee481075e6e3925` plus retained worktree edits; clean local baseline commit/tag `263dfbf8ee509b883a78d43d09d47a50a48190b8` / `validation/fr01-closed-baseline-20261011`; source-export child `bbec6c12ae34a31f159961a12ad64079cb589ea1` / `validation/fr01-source-export-20261011`. The child changes export policy/documentation only and was not cold-built. Tags/commits are local validation artifacts; the user's branch/index was not committed or pushed. Five pre-existing modified files are recorded in `control.json` and preserved.

| Check / oracle | Candidate-bound result and receipt |
| --- | --- |
| `CHK-FR0107-01` immutable acquisition | **PASS:** absent build/dependency cache; full configure exit `0`, 108.169 s. Sixteen acquired source groups retain revision/notice hashes; SDK archives and 14 required extracted members match their pinned hashes. Font Awesome font/license now use immutable commit URLs and expected SHA-256. `closed-cold-configure.*`, `closed-input-inventory.json`, `closed-sdk-archive-verification.json`. |
| `CHK-FR0107-02` Shipping build | **PASS:** clean `ShippingGame` / `ShowcaseGame`, exit `0`, 263.498 s; `ShippingEditor` / Launcher and three cook helpers, exit `0`, 378.125 s. PE identity is `0.1.0`, Pawel Lisowski for Launcher/Game. They are unsigned. Editor was not built in this baseline. `closed-shipping-*-build.*`, `closed-output-inventory.json`. |
| `CHK-FR0107-03` owner checks | **PASS:** existing architecture boundary, code-style and ShaderCompiler CLI targets, wrapper exit `0`, 69.990 s; warm disconnected replay exit `0`, 73.821 s. No gate semantics or new test executable. `closed-owner-checks.*`, `closed-offline-owner-checks.*`. |
| `CHK-FR0107-04` cook and activation | **PASS, built-in Empty:** cook exit `0`, six products; Launcher exit `0`, exact newly started Game PID `11860` closed normally with exit `0`. Source status remains empty. This observes activation/normal shutdown, not a rendered frame, seven-map correctness or FPS. `closed-empty-cook.*`, `closed-local-activation.*`. |
| `CHK-FR0107-05` cache replay | **PASS:** `FETCHCONTENT_FULLY_DISCONNECTED=ON` configure and Shipping Game rebuild exit `0`; owner replay above passes. Process HTTP-denied configure/build/owner-check/cook replay all exit `0`, recorded separately as `denied-replay-*`; probes establish failure of attempted CMake/Git HTTPS acquisition. Executable/cooked payload hashes remain unchanged; only two `recook.signal` notification files change (`replay-output-comparison.json`). This is not an OS firewall, an all-protocol offline-host test or independent-build bit identity. |
| `CHK-FR0107-06` content boundary | **PASS, bounded membership:** classic Sponza's 71 old tracked mesh/texture files are replaced by a pinned 72-file acquisition recipe; fresh acquisition verifies every byte/hash. Empty has no initial selected packs. Source export excludes Sponza bytes/thumbnails, DamagedHelmet and unqualified development thumbnails; recipes/notices remain. Reviewed ZIP SHA-256 `4f8db1d1cb5343a5e9b7c82b6f368f77d64421602e8be88edaf447d25a1d77b7`, 3223 entries. `sponza-acquisition.*`, `reviewed-source-export.json`. |
| `CHK-FR0107-07` provenance / failure controls | **PASS:** retained selected-model notices, exact publisher matches for four Poly Haven skies, Font Awesome cache corruption rejection with bytes preserved, traversal rejection before download, non-Empty missing-mesh rejection, Empty missing-shader rejection. `selected-model-notices.json`, `sky-publisher-identities.json`, `negative-controls.json`, `readiness-negative-controls.json`. Cornell's tracked licensed GLB remains unchanged; optional OBJ archive receipt remains unavailable and is not a launch prerequisite. |
| `REL-01` external closure | **BLOCKED:** the existing GitHub repository is public and its raw history still distributes uncleared historical asset bytes. Reviewed release export exclusions do not clear that surface. An applicable grant or an owner-authorized repository/history remediation is required; no remote visibility/history change was made. SDK/Qt redistribution obligations are inventoried, but a complete admitted notice/payload review is still required. `repository-visibility.json`, notice inventories and [release input owner](../../Architecture/Modules/BuildAndPackaging/ReleaseInputs.md). |
| Signing decision / availability | **PASS for route choice; issuance unavailable:** owner delegated selection; choose Certum Standard Code Signing in the Cloud for individual Pawel Lisowski, Windows SDK SignTool, SHA-256 and RFC 3161 timestamp. Both signing certificate stores are empty. Purchase/identity verification/issuance must be completed directly with Certum before signing can run; no purchase, enrollment or test certificate was substituted. Signing execution/package verification belongs to FR-02. |
| `REL-02` ordered admission | **BLOCKED by REL-01:** the baseline path above passes locally under its exact clean candidate. Fresh-host installed prerequisites, Editor, seven maps, native validation/FPS, staged package/SBOM/signatures and consumer adoption are unqualified. |

**Run the baseline:** on a sufficiently short Windows source/build path, with CMake `4.3.3`, Git `2.54.0.windows.1`, VS 18 2026 / MSVC `19.51.36247`, Windows SDK `10.0.26100.0`, Qt `6.11.1` and Vulkan SDK `1.4.350.0`; owner style check uses clang-format `22.1.3`. This host is Windows 11 Home build `26200`, Ryzen 9 8940HX / RTX 5070 Ti Laptop, NVIDIA driver `32.0.16.1047`. Exact installed paths/versions are in `environment.json`; executable/cooked hashes in `closed-output-inventory.json`.

```powershell
# $taskCheckout is the clean, tagged candidate root; its build/artifacts initially do not exist.
cmake -S $taskCheckout -B "$taskCheckout/build" -G "Visual Studio 18 2026" -A x64 -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/msvc2022_64
cmake --build "$taskCheckout/build" --config ShippingGame --target ShowcaseGame --parallel 4
cmake --build "$taskCheckout/build" --config ShippingEditor --target AssetCooker TextureCooker ShaderCompiler SparkleLauncher --parallel 4
powershell -NoProfile -File "$taskCheckout/CMake/RunBaselineChecks.ps1" -Configuration ShippingEditor
& "$taskCheckout/artifacts/dev/tools/AssetCooker/ShippingEditor/AssetCooker.exe" cook-project Showcase ShippingGame --tool-profile ShippingEditor --root $taskCheckout
& "$taskCheckout/artifacts/dev/launcher/ShippingEditor/SparkleLauncher.exe" --root $taskCheckout --level Empty --product game --game-profile ShippingGame --graphics-api d3d12 --run levels.run
cmake -S $taskCheckout -B "$taskCheckout/build" -DFETCHCONTENT_FULLY_DISCONNECTED=ON
# Repeat Game build and owner checks, retaining the same source/cache/configuration.
```

**Artifacts and cleanup:** `build/validation/fr01-closure-20261011/` retains exact per-step command/exit/time/status JSON, raw logs, input/output hashes, local tags/worktrees, exports and byte-verified backups of retired content. Task-created Game processes were closed normally; negative-control modifications restored. Temporary validation harnesses are removed at handoff. `commands.md`, `handoff-manifest.json` and `cleanup.json` provide the final inventory/cleanup boundary. No assets/shaders gained internal versions. Root/validation build trees and Launcher diagnostic state remain for review; they are not source or consumer payload.

**Failed attempts retained, superseded for this baseline:** earlier cold style checks exposed checkout CRLF; later configure exposed Git path handling; a longer build path hit MSBuild `MSB3491`/MAX_PATH. Fixes are explicit LF policy for CMake C++ sources, scoped Git `core.longpaths=true`, pinned Assimp CMP0175 default, scoped Qt deployment environment, and a shorter validation root. Zlib's upstream configure renames tracked `zconf.h` to ignored `zconf.h.included`; `generated-input-state.json` verifies the retained bytes against the pinned object after declared EOL normalization. This generated cache state is disclosed, not claimed clean. Arbitrarily long Windows checkout/build paths are not qualified.

## FR-01 Exit Recheck: ITER-REL-01-06

**Candidate:** `9734e6d469c09f65fcfe66ce3ee481075e6e3925`, clean at start on 2026-10-10; tests use that detached checkout, then the exact Cornell catalog patch. Scope: phase admission, two formerly mutable dependency acquisitions and removal of Cornell's unused map prerequisite. Owner: Launcher catalog/acquisition and Acceptance. Preserve seven maps, GLB bytes, optional archive recipe/hash, existing owner semantics and prior bounded results. No asset/shader versioning, public asset removal, package implementation or release publication.

| Check / criterion | Candidate-bound result |
| --- | --- |
| `AC/CHK-REL0106-01`: fresh immutable dependency acquisition | **PASS, bounded:** fresh detached checkout and initially absent `build/_deps`; existing CMake single-dependency owner acquires `stb` at `2c980bb59875b0d32144a71867fbdebb2f77cd20` and sparse Compressonator at `f4b53d79ec5abbb50924f58aebb7bf2793200b94`. Both source trees have clean tracked status and exact HEAD. Cold and cache replay commands exit `0`; `stb` replay uses `FETCHCONTENT_FULLY_DISCONNECTED=ON`, Compressonator's existing-cache branch validates without cloning. No OS network isolation or whole dependency/Shipping baseline is asserted. |
| `AC/CHK-REL0106-02`: Cornell map has only real source prerequisites | **PASS:** level source refers to `CornellBox/CornellBox`; tracked GLB is self-contained and already license/format-verified. Only its catalog `AssetPack = CornellBox` line is removed. Optional pack definition, ZIP hash, map ID/selection and GLB stay unchanged. Before: fresh checkout dry-run schedules unavailable publisher ZIP. After: dry-run schedules no acquisition; actual `levels.sync --level CornellBox` exits `0` with no Publisher directory. Both use the existing Launcher operation owner, not a new no-op implementation. |
| Original ZIP check | **Unavailable for optional regeneration:** four further publisher requests (HTTPS, www, HTTP and download query) return `406`. The original ZIP is not replaced or certified. It is no longer a normal Cornell map dependency; this does not assert recovered archive provenance. |
| `REL-00` / `REL-01` | **BLOCKED:** exact technical package/helper/configuration/content-store freeze and public asset-byte/payload rights closure remain unaccepted. Public source/archive inclusion of classic Sponza still needs an exclusion policy or an applicable grant; modern samples can follow the existing acquisition-recipe rule. Full notices/dependency/SDK/SBOM/signing decisions remain open. |
| `REL-02` / phase transition | **BLOCKED:** clean tagged-source complete cold configure/Shipping build/check and warm/offline replay is unrun. The two-dependency probe is prerequisite evidence only. FR-02 must not start with unresolved rights/payload identity/discovery. |

Exact CMake command shape, using `$taskCheckout = <repo>/build/validation/fr01-exit-recheck-20261010/checkout` and `$taskDependency = stb` then `compressonator`:

```powershell
cmake -S $taskCheckout -B "$taskCheckout/build/_dependency-sync/$taskDependency" -G "Visual Studio 18 2026" -A x64 "-DSPARKLE_SYNC_SOURCE_DEPENDENCY=$taskDependency" "-DFETCHCONTENT_BASE_DIR=$taskCheckout/build/_deps"
# Replay the same command with -DFETCHCONTENT_FULLY_DISCONNECTED=ON.
& .\artifacts\dev\launcher\DevelopmentEditor\SparkleLauncher.exe --root $taskCheckout --level CornellBox --dry-run levels.sync
& .\artifacts\dev\launcher\DevelopmentEditor\SparkleLauncher.exe --root $taskCheckout --level CornellBox --run levels.sync
```

**Environment / control:** CMake `4.3.3`, Visual Studio 18 2026 x64, MSVC `19.51.36247.0`, Windows SDK `10.0.26100.0`; cache selects local Qt `6.11.1`. The single-dependency probe does not qualify those installed SDKs on a fresh host. Existing Launcher executable SHA-256 `b60951c09b713ed972480130c5502fe514d7298a67d85f869a20eed2885c504c`, from the earlier locally built candidate; its source owner is unchanged. This is not a Launcher build from the fresh checkout. `build/validation/fr01-exit-recheck-20261010/` retains `control.json`, exact command/exit/time records, cold/replay logs, before/after sync, HTTP failures and a final hash manifest. Wrong dependency revision, renewed unused acquisition, changed GLB or unresolved rights stops qualification (`FM-REL0106-01..03`). The real absent-cache and absent-Publisher observations detect warm-machine contamination. Isolated checkout/cache and Launcher diagnostic state are retained for replay; the original workspace caches were not modified. Decision: two acquisition uncertainties and Cornell's unnecessary prerequisite addressed; next stage remains blocked at the named gates.

## Cornell License And Format: ITER-CORNELL-01

**Candidate:** `03409713168d1fcface79236f805b5d19a5fccfd`, 2026-10-10, plus the preceding two-document FR-01 report patch. Scope: retained Cornell license/attribution and expected conversion form only; preserve the existing archive identity and selected map. Owner: Cornell conversion / Acceptance. `NS-EVIDENCE`, `NS-OWNERSHIP`, `NS-ADOPTION`; preserve graphics proof boundaries, advance `REL-01` input clarity. Control and hashed command/input/output receipts: `build/validation/cornell-license-20261010/`.

| Check | Result |
| --- | --- |
| `AC/CHK-CORNELL-01`: retained license and attribution | **PASS for the retained notices:** dataset notice explicitly names Guedis Cardenas/Morgan McGuire, Williams College 2011, CC BY 3.0; original OBJ/MTL additionally state public domain. [CC BY 3.0](https://creativecommons.org/licenses/by/3.0/) permits educational use, adaptation and redistribution with attribution. Exact notice is retained in [COPYRIGHT.txt](../../../Projects/Showcase/Assets/Meshes/CornellBox/COPYRIGHT.txt); [conversion owner](../../../Projects/Showcase/Assets/Meshes/CornellBox/Conversion/README.md) supplies title, source/license links and modifications. This is a retained-notice review, not a recovered archive receipt or package audit. |
| `AC/CHK-CORNELL-02`: expected conversion form | **PASS:** existing converter reproduces the tracked `CornellBox.glb` byte-for-byte, SHA-256 `a2af112abeaa1b08087009afa3efef2c6b665c59bd7a6efe70d703e8f2349d91`. GLB 2.0 JSON/BIN boundaries, 8 materials/primitives, 72 vertices, 36 triangles, finite vertex data, index/buffer bounds and embedded author attribution checked. One scene/mesh, no animation/skin/textures, `KHR_materials_emissive_strength`; no new variant or output hash. |
| Independent source comparison | The retained original OBJ/MTL match files at pinned mirror `yknishidate/single-file-vulkan-pathtracing`, commit `df3fda0d362fa3e90671c8e24208315fadaf05b5`, after CRLF normalization only. Exact byte hashes differ. The mirror corroborates source text/public-domain headers; it does not provide the original ZIP or archive-level notice. |
| `AC/CHK-CORNELL-03`: acquisition boundary | **BLOCKED:** catalog ZIP receipt remains absent; prior publisher requests returned HTTP `406`. Original `224880`-byte / `f27eaf47afec74b5236ce3095ac02d4256e8ac8610bfdb5a53560c67c2e0d8f1` identity is unchanged. No fabricated ZIP, alternate digest or cache receipt was substituted. |
| Gate / cleanup | `REL-01` and `REL-02` remain **BLOCKED** at their other named inputs/baseline checks. No native cook/render/FPS or package claim. Conversion output used only an ignored validation path; tracked GLB was unchanged. Temporary regenerated GLB removed after comparison; retained logs/manifests remain. |

Exact command from the repository root:

```powershell
python Projects/Showcase/Assets/Meshes/CornellBox/Conversion/ConvertCornellBox.py --output build/validation/cornell-license-20261010/CornellBox.glb
```

The owning control maps missing attribution, changed conversion hash and unknown archive provenance to rejected admission; detects false clearance through distinct license/format/acquisition verdicts. Decision: license/attribution and local conversion form verified; exact archive reacquisition and cooked/package proof remain open.

## FR-01 Blocker Follow-up: ITER-REL-01-05

**Candidate:** `03409713168d1fcface79236f805b5d19a5fccfd`, initially clean and still clean during executable checks on 2026-10-10. The report patch changes documentation only. Scope: prerequisite/blocker follow-up; full FR-01 admission and `REL-00` technical closure remain unaccepted. Owner: Acceptance, invoking existing BuildAndPackaging/Launcher/shader-check owners. No production changes, new tests, feature fixes or asset/shader versioning.

| Check | Observation / disposition |
| --- | --- |
| `CHK-FR0105-01`: original Cornell input | **BLOCKED:** exact publisher URL plus five variants returned HTTP `406`, with Apache Mod_Security response bodies. An inspected pinned mirror provides OBJ/MTL files but no matching archive or embedded-notice receipt. Neither its different byte hashes nor the local conversion replace the required `224880`-byte archive / `f27eaf47afec74b5236ce3095ac02d4256e8ac8610bfdb5a53560c67c2e0d8f1`. `cornell-fetch.json`, `cornell-source-probes.json`, `cornell-mirror-comparison.json`. |
| `CHK-FR0105-02`: exact rights and delivery boundary | **BLOCKED for asset-byte redistribution:** Modern Base/Curtains notices expressly permit educational use but retain the previously observed mixed wording. The user again confirms educational intent; no grant or scope-removal approval was supplied. [Intel's page](https://www.intel.com/content/www/us/en/developer/topic-technology/graphics-research/samples.html) does not replace those exact notices. [Pinned classic license](https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Assets/edc7c9e67c639d230715049ee31f9a96a6babbbe/Models/Sponza/LICENSE.md) assigns model bytes to CRYENGINE terms, distinct from its CC BY metadata. The [existing contract](../FirstRelease.md#release-map-set) permits acquisition recipes instead of bundling uncleared bytes. Preserve seven selected maps; qualify recipe/notices and exclude uncleared bytes from public payloads. Classic has 71 tracked mesh/texture files (`classic-tracked-files.json`); source archives need this review too. Exact Modern notice hashes/preambles are retained in `retained-rights-notices.json`. No publisher was contacted or asset removed. |
| `CHK-FR0105-03`: owner gates | **PASS**, exit `0`: architecture boundaries, style (2190 selected sources) and ShaderCompiler CLI validation. Shader checks exercise 37 registrations and both DXIL/SPIR-V routes, including the owner's determinism and negative/cancellation controls. `owner-gates.log`, `owner-gates-exit.txt`. These checks use existing caches. |
| `CHK-FR0105-04`: PE identity | **PASS** on existing Launcher/Editor `DevelopmentEditor` and Game `ShippingGame` executables: correct product names, `0.1.0`, publisher Pawel Lisowski, 2026 copyright. `product-identities.json`; exact binary hashes in `local-inputs.json`. This does not qualify signatures, complete package contents or build provenance from a fresh checkout. |
| `CHK-FR0105-05`: Launcher/Game activation and cleanup | **PASS, local warm Empty only:** Launcher operation `levels.run` reports requested activation, exit `0`, 4.139 s. Game PID `29708` was identified by exact executable and creation within this invocation; its parent was Launcher shadow PID `29216`, not the initial shell PID. `CloseMainWindow()` and `WaitForExit(10000)` completed, Game exit `0`. `local-game-launch.json/.log`, `local-game-cleanup.json`. Shipping Game log is empty; no rendered-frame, API-validation or FPS observation is asserted. |
| `CHK-FR0105-06`: report consistency | **PASS:** UTF-8, 83 local links, 112 incoming document links and `git diff --check`; `document-verification.json`. Documentation checks do not qualify executable or release behavior. |
| `REL-01` | **BLOCKED:** original Cornell receipt, exact selected-byte redistribution decisions/qualified payload, complete dependency/license/notices/SBOM/signing closure. Educational intent and acquisition recipes do not certify bundled asset rights. |
| `REL-02` | **BLOCKED:** no clean tagged-source cold acquisition/configure/Shipping build/check route and warm/offline replay was executed. Unresolved admission/input evidence stops release qualification; the warm local checks above are retained separately. |

**Exact local commands** (repository root, Windows 11 build `26200`; MSBuild `18.7.1+024f5b037`, selected Windows SDK `10.0.26100.0`, Qt cache `6.11.1`, Vulkan SDK cache `1.4.350.0`; executable versions/cache paths in `environment.json`; owner style check used clang-format `22.1.3`, although it is not on the shell PATH):

```powershell
powershell -NoProfile -File CMake/RunBaselineChecks.ps1 -Configuration ShippingEditor
& .\artifacts\dev\launcher\DevelopmentEditor\SparkleLauncher.exe --root $PWD.Path --level Empty --product game --game-profile ShippingGame --graphics-api d3d12 --run levels.run
```

**Control and artifacts:** `build/validation/fr01-unblock-20261010/control.json` owns `AC-FR0105-01..03` (exact archive or attributed failure; rights receipt or explicit blocker; exact local owner/activation result). Failure controls reject wrong archive identity, ambiguous rights, owner-gate failure or wrong requested process identity; no simulated new production tests were added. Warm-state contamination and false legal clearance remain the risks. `local-inputs.json` retains clean revision/configuration plus six executable/cooked-shader byte identities; `manifest.json` hashes retained receipts and records final report-only dirty state. Shader validation regenerated local shared cooked outputs. The test Game was closed normally; caches and receipt artifacts remain. No clean cache, seven-map cook, full backend matrix, package, independent host or performance check passed or was claimed. Decision: bounded local checks complete; both release gates remain blocked at the named owners.

## Editor/Game Naming: ITER-GAME-NAMING-01

**Scope:** replace the former standalone product name across Application/console hosts, Showcase targets and entry files, Launcher GUI/shell/build/clean plans, catalog fields and documentation. No aliases, asset/shader versions, migrations or feature changes. Technical execution and third-party library uses of `runtime` retain their meaning.

**Candidate:** source/build candidate committed by the user as `c087aa6078ba05a62adbc683a79cb0c406c30398` during verification, plus the recorded final Launcher-label and evidence edits. Started at `408e8df21fbb365d61b4db3a8624d5e8766a74e8` on 2026-10-10. The starting worktree was clean. Existing Application, Showcase, Launcher and catalog owners retain their responsibilities. Preserve `REL-01` identity and `PGE-01/07/13`; no feature evidence-level increase or release acceptance is claimed.

| Check / criterion | Evidence and decision |
| --- | --- |
| `AC-GAME-01` / `FM-GAME-01`: one current vocabulary; old profile/operation rejected | `GameApplication`, `GameConsole`, `ShowcaseGame`, `--game-profile`, `workspace.build.game`, `GameSupported`/`GameBlocker`; shell receipts include old-name rejection. No compatibility spellings. |
| `AC-GAME-02` / `FM-GAME-02`: producers and consumers compile; plans resolve the renamed executable | **PASS:** `SparkleLauncher`/`ShowcaseEditor` in `DevelopmentEditor`, `ShowcaseGame` in `ShippingGame`, and `AssetCooker` in `ShippingEditor` built. Launcher configuration refresh and the final Empty/D3D12 Game readiness plan passed. Game run planning targets `artifacts/dev/projects/Showcase/game/ShippingGame/ShowcaseGame.exe`. Dry-run is not native launch proof. |
| `AC-GAME-03`: owner checks and navigation retain their meaning | **PASS:** architecture owner; C++ style owner for 2,044 files; 2,268 local links across 99 modified documents with no missing file/anchor; final whitespace check. |
| Risk and cleanup | A missed role, field, include or target breaks planning/compilation. Owner checks, both host builds and catalog planning detect it; repair at the existing owner. Obsolete product executable, DLL copies, IDE target files, object/log files and empty output directories were removed individually after replacement builds. Historical receipts and unrelated state were preserved. |
| Release boundary | `REL-01`/`REL-02` remain **BLOCKED** by rights/archive receipts, payload qualification and a clean candidate baseline. Noncommercial learning intent supplies no public redistribution grant. This iteration does not execute a seven-map cook, FPS test or clean-machine acceptance. |

**Decision: PASS for the product rename.** Native launch, rendering/FPS, seven-map cook and clean-machine results are outside this rename check. Qt deployment still warns that `VCINSTALLDIR` is unset; the initial Launcher link also reported missing debug PDBs. Neither warning is package qualification.

**Receipts:** `build/validation/game-rename-20261010/` contains the control record, exact commands/exits, shell observations, owner logs, link checks and final candidate hashes. Historical sections below retain original candidate decisions; navigable code names use current spelling, while raw receipts retain observed spelling.

## Scope Approval: ITER-REL-00-06

**Candidate context:** 2026-10-10, `d1c8c5b2a61768a0460983c854264380d5c1f6dc` plus the preserved 16 pre-existing modified documents and this approval edit.

**Owner decision:** all six proposals approved. Resolution follows the user's display/selection; no product resolution is forced. Users are expected to launch through Sparkle Launcher. D3D12/Lit/VSync/histogram/ACES approximation/SDR/neutral grading/HDR opt-in defaults are accepted, with **chromatic aberration On and vignette On**. Other approved packaging, OS, controls, compatibility and distribution/privacy choices are in [the decision owner](README.md#approval-and-next-work). No further answer to those six choices is pending.

**Superseded:** earlier unanswered-choice status, mandatory native 1080p internal rendering for normal viewing, and lens effects Off. The 1080p presets remain controlled laptop acceptance configurations, with exact render/output/provider extents recorded; the 30 FPS threshold and seven-map/both-API evidence obligations are preserved. This decision does not pass a performance test. Earlier iterations below are historical snapshots.

| Gate | Current disposition |
| --- | --- |
| `REL-00` | **BLOCKED for technical closure only:** product choices approved; exact rights receipts and reviewed package/helper/configuration/content-store closure remain. No pending product approval is a blocker. |
| `REL-01` | **BLOCKED:** real publisher/license/version, immutable dependencies, content rights and Shipping payload qualification remain unresolved. |
| `REL-02` | **BLOCKED:** clean tagged-source cold/warm build/cook/runtime baseline not executed; input/admission prerequisites remain. |

**Documentation checks: PASS.** 362 local links, 131 incoming links, stable identifiers and four negative controls passed; `git -c core.safecrlf=false diff --check` passed.

**Evidence boundary:** documentation changes only. Exact pre-edit texts, control, validation command and changed-input hashes are retained in `build/validation/release-scope-approval-20261010/`. No production code, dependency acquisition, build, cook, runtime or package validation was performed; generated state was preserved.

## Scope Amendment: ITER-REL-00-05

**Candidate context:** 2026-10-10, `d1c8c5b2a61768a0460983c854264380d5c1f6dc` plus this documentation amendment. The initial dirty tree contained only the identity/baseline preflight in `Docs/Acceptance/FirstRelease.md`; that report and its input snapshot are preserved.

**Release-owner decisions:** ship Sparkle Launcher, Sparkle Editor and Sparkle Game; Launcher selects/syncs any subset of the seven release maps and required features, including none with intentional Empty startup; mark other choices development or unavailable. Render internally at native 1920x1080, then scale for the selected display; measure 30 FPS on the named laptop. Offer no user support service. The owner confirmed both the native-resolution interpretation and seven-map boundary in follow-up replies.

**Superseded:** the single game consumer package, source-only Launcher/Editor, mandatory Modern Sponza first launch, subnative DLSS Quality realtime performance preset and public support/security response clocks. Earlier dated receipts and inspections below remain historical evidence; their source identities/rights observations are unchanged. The earlier PD-0 PASS does not approve the amended product, path, helper or service boundary.

| Gate | Candidate-bound disposition |
| --- | --- |
| `REL-00` | **BLOCKED, partially approved:** the decisions above are settled. Remaining package format/platform/controls/defaults/compatibility/distribution proposals, helper/configuration/content-store freeze and exact rights receipts still need closure. |
| `REL-01` | **BLOCKED:** unresolved publisher/license identity, moving dependencies, selected-content rights and qualified payload closure remain. No rights or dependency fix was made. |
| `REL-02` | **BLOCKED:** no clean tagged-source cold/warm build/cook/runtime baseline was executed. The amended three-product baseline must be admitted before execution. |

**Documentation checks: PASS.** Sixteen documents were reconciled; 485 local links, 288 incoming links, stable identifiers and four negative controls passed. `git -c core.safecrlf=false diff --check` passed. Exact baseline, iteration control, changed-input hashes, validation command and results are retained in `build/validation/release-scope-amendment-20261010/`. This is scope/contract evidence only; no build, cook, runtime, GPU, performance, package or publication proof.

## Selected Content Receipts

This is the seven-map source/rights snapshot from the reconciliation. [ReleaseMapSet](README.md#releasemapset) owns intended membership. Cameras and input identities below are retained for the owning workload and content checks.

| Level | Product classification | Fixed view/route | Input and rights decision |
| --- | --- | --- | --- |
| `CornellBox` | Included; reference/lighting example | Authored camera `(0, 1, 3.8)`, yaw pi, pitch 0, FOV 45; fixed camera, 300 warmup + 300 sampled frames, three repeats | Existing publisher archive revision 2011-07-27; catalog SHA-256 `f27eaf47afec74b5236ce3095ac02d4256e8ac8610bfdb5a53560c67c2e0d8f1`. Inspected local publisher notice states CC BY 3.0; original OBJ additionally states public domain. [Owned conversion/attribution record](../../../Projects/Showcase/Assets/Meshes/CornellBox/Conversion/README.md) names Guedis Cardenas/Morgan McGuire. Current GLB hash matches its documented `a2af112abeaa1b08087009afa3efef2c6b665c59bd7a6efe70d703e8f2349d91`. Retain complete redistribution/cook receipts in `REL-01`; rect-light behavior still needs its real feature proof. |
| `CesiumMan` | Included; animation example | Authored camera `(0, 1.5, -3)`, yaw/pitch 0, FOV 60; stationary camera, animation running; same sampling protocol | [Pinned upstream metadata](https://github.com/KhronosGroup/glTF-Sample-Assets/blob/edc7c9e67c639d230715049ee31f9a96a6babbbe/Models/CesiumMan/README.md) identifies CC BY 4.0 and separate logo/trademark notice. Local model JSON matches this revision after external URI relocation; both referenced buffer/texture bytes match exactly. Retain attribution and separate logo notice; no endorsement claim. Cooked animation/material fidelity and final notices remain packaging proof. |
| `Sponza` | Included; classic lighting example | Existing authored camera; freeze exact static and motion/disocclusion observations in MAP-00 | [Pinned primary metadata](https://github.com/KhronosGroup/glTF-Sample-Assets/blob/edc7c9e67c639d230715049ee31f9a96a6babbbe/Models/Sponza/README.md) names a CRYENGINE-specific agreement. Selection is not redistribution permission; REL-01 must establish exact usable terms and local dependency identity. |
| `ModernSponza` | Included; first-run/default | Authored camera `(5.39, 6.53, 0.26)`, yaw 4.77, pitch -0.58, FOV 66.9; `REL-NATIVE-1080P` | Current level uses base plus colorful curtains, not candle/knight variants. Catalog pins both archives; [publisher samples](https://www.intel.com/content/www/us/en/developer/topic-technology/graphics-research/samples.html) describe attribution licensing. Both installed embedded notices contain CC BY 4.0 text alongside narrower personal/educational/limited-commercial wording. REL-01 must resolve that inconsistency with the rights owner and retain exact dependency/notices identity; publisher-page wording alone cannot clear distribution. Combined source archives are 4,774,507,032 bytes; source size is not cooked package size or GPU complexity proof. |
| `LPSHead` | Included; head/material example | Existing authored camera, fixed pose | [Owned conversion record](../../../Projects/Showcase/Assets/Meshes/LPSHead/Conversion/README.md) names source attribution, geometry conversion and omitted bump-map interpretation. Retain pinned archive/texture/GLB identity and prove actual material fidelity; selection does not certify subsurface transport. |
| `DiffuseTransmissionPlant` | Included; plant example, material qualification open | Existing authored camera; freeze animation pose for any admitted reference comparison | [Pinned primary metadata](https://github.com/KhronosGroup/glTF-Sample-Assets/blob/edc7c9e67c639d230715049ee31f9a96a6babbbe/Models/DiffuseTransmissionPlant/README.md) describes diffuse transmission and animated punctual lights. Qualify the exact local importer/rendered material contract before package admission; unsupported effects cannot be silently dropped or treated as reference truth. |
| `ABeautifulGame` | Included; chess/material example, material qualification open | Existing authored camera, static view | [Pinned primary metadata](https://github.com/KhronosGroup/glTF-Sample-Assets/blob/edc7c9e67c639d230715049ee31f9a96a6babbbe/Models/ABeautifulGame/README.md) names transmission/volume and attribution terms. Close exact local source/dependency identity and material support; an explicitly authored reduced-domain variant requires a separate owner decision. |

## FR-00 Review Package

**Reconciled:** 2026-10-10 at `aa9fcd94f219e8a3d4dbe7c51457680ad9300783`, initially clean tree. This is source, document and receipt inspection only.

**Owner map revision incorporated:** the two-map proposal is superseded by seven selected maps and Modern Sponza/native 1080p Lit first run. Bistro and Moana are postponed. The named laptop and mandatory rendering requirements remain approved. The finite Windows ShippingGame/source product and compatibility/support proposals still need approval; selected maps need rights, material and package-budget qualification. No runtime acceptance is inferred.

### Facts Closed By Inspection

- All 13 retained production/configuration inputs are byte-identical to the preceding scope audit. The ledger remains 512 IDs, 16 inventory owners, 20 catalog levels, 18 view modes and 26 persisted names. No new classification is needed for these unchanged surfaces.
- Cornell Box has eight source materials, no skin or animation, and only `KHR_materials_emissive_strength`. Cesium Man has ordinary metallic/roughness materials, no declared glTF extensions, one skin and one animation. Neither level declares a Sky section. This closes the Cornell/Cesium source-domain inspection only, not importer fidelity, a cooked dependency manifest or radiance correctness. Material values and exact input hashes are retained in the receipt artifact.
- All eight scene-source files used by the seven selected levels are available locally and hashed, including Modern Sponza base and curtains. Plant declares diffuse transmission and an animation; A Beautiful Game declares transmission/volume. These are source facts, not proof the importer or renderer implements them. Modern Sponza and LPS Head installed notices are retained by exact hash; Modern Sponza usage wording needs rights-owner resolution.
- The accepted reflective reference domain is preserved. It does not expand to transmission, volume, spectral or physical subsurface transport. Cesium reference comparisons require a frozen pose; Cornell's emissive panel and authored rect light require a single physical emitter interpretation in the existing reference/light owners. These are executable qualification checks, not a restart of `PTD-00`.
- Streamline is pinned to 2.11.1 by the owning dependency CMake. Current initialization explicitly rejects Vulkan. Current RR evaluation uses `sl::kFeatureDLSS_RR` after setting SR/RR options; this does not prove native support or a second final-image composition. Both-API SR/RR remains mandatory work in `DSP-3`; Linear fallback cannot satisfy the realtime preset.
- `REL-NATIVE-1080P`, `REL-RT-1080P` and `REL-REFERENCE` keep separate purposes and the existing budgets. SDK-derived render extent, realtime depth/reuse controls and exact source-to-output composition must be frozen in the owning feature discoveries before measurement. No arbitrary numeric setting or performance result is invented here.

### REL-00 Clause Coverage

| Contract area | Reviewable record | Remaining decision or proof |
| --- | --- | --- |
| Product, executable, version, audiences and developer contents | [Product/selection sections](README.md#decision-summary) and [capability ledger](CapabilityDispositions.md) | Owner approves finite runtime/source promise |
| Windows, GPU/API, machines, storage and source toolchain | [Product matrix](README.md#product-and-supported-matrix); named laptop approved | Owner approves proposed broader OS boundary; later qualification is unrun |
| Rendering surface and default configuration | [Baseline/selection](README.md#baseline-and-selection); mandatory amendment; every view/setting disposition | Owner approves consumer defaults; numerical feature controls close before their runs |
| Maps, first run, routes and redistribution | [ReleaseMapSet](README.md#releasemapset), cameras and selected input receipts | Membership/default selected; exact rights, material/domain and size receipts remain |
| Compatibility, reset, update and side-by-side | [Mutable-data/compatibility section](README.md#mutable-data-and-compatibility) | Owner approves publication promises; no compatibility implementation added |
| Budgets, observation, reference and >=30 FPS | Budgets/observation and acceptance owner | Existing thresholds preserved; executable results unrun |
| Display/input/language/accessibility and non-goals | Baseline/selection and product limitations | Proposed finite consumer surface awaits approval; HDR needs display qualification |
| Network, privacy, supply chain, signing and distribution | [Network/security/support section](README.md#network-security-publication-and-support) | Policy approval; publisher/contact/credentials and complete receipts in REL-01 |
| Support, security response, patch and withdrawal | Same finite support policy | Owner approves service promises; working channels remain unprovisioned |
| Accepted tracer discovery versus release oracle | Accepted PTD-00-R1 plus reference feature plan | Development discovery PASS preserved; PTD-02/03 and Stage 10 remain unaccepted |

Current decisions and next actions are maintained once in [Release Scope](README.md#approval-and-next-work). Cornell's required archive is `224880` bytes with SHA-256 `f27eaf47afec74b5236ce3095ac02d4256e8ac8610bfdb5a53560c67c2e0d8f1`; both reconciliation publisher requests returned HTTP 406 and inspected Launcher caches had no matching archive. Cesium buffer/texture bytes matched retained upstream pins; [refreshed pinned metadata](https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Assets/edc7c9e67c639d230715049ee31f9a96a6babbbe/Models/CesiumMan/README.md) retained distinct attribution/logo terms. Neither observation grants redistribution permission.

Artifacts: `build/validation/fr00-reconciliation-20261010/` retains baseline source identities, selected material/asset receipts, exact commands and refreshed coverage results. Earlier receipts remain useful under their recorded identities.

## ITER-REL-00-03 Current-Source Reconciliation

| Field | Record |
| --- | --- |
| Scope / start | FR-00 only; release/document owner, `aa9fcd94f219e8a3d4dbe7c51457680ad9300783`, initially clean. No production/CMake/configuration edits. |
| North Star / persona | Preserve NS-EVIDENCE, NS-OWNERSHIP and NS-MATH-DATA; improve the review route toward NS-ADOPTION. PGE-01/13/15 scope clarity only; no evidence-level or readiness increase. |
| Decision and risks | Missing owner decision or unavailable archive may be mistaken for approval. Detect through explicit proposal labels, primary/pinned identities and the unchanged acceptance gate. Retain the exact missing owner/input; do not substitute assets or weaken Included obligations. |
| AC / FM / checks | Existing AC-SCOPE-01..04, FM-SCOPE-01..04 and CHK-SCOPE-01..03; exact inventory/classification and input hashes, three in-memory omission/duplicate/weakened-class negatives, local links/anchors, UTF-8/whitespace and git diff --check. |
| Rights/domain inspection | Selected level/source identities plus available glTF/GLB material, extension and animation inventory; publisher archive requests and current workspace cache inspection; current Cesium hashes and primary metadata. No cook, legal clearance, oracle or native qualification inferred. |
| Static result | **PASS:** 118 checks across three changed documents, 147 local links, 104 anchors, 13 unchanged production inputs and three rejected in-memory negative controls. Seven exact selected map IDs and all eight available scene-source identities checked. This is document/source consistency only. |
| Disposition | Static coverage and source-domain inspection complete; final scope decision and exact rights remain BLOCKED. Original development PTD-00 PASS preserved; no source/native/package/release claim promoted. |

## ITER-REL-00-01 Control Record

| Field | Record |
| --- | --- |
| Scope / owner | Stage 0 only; repository maintainer/release owner. Start revision `93e86b8113c3fcd0c4921729c3513538b95bc794`, clean tree. Documentation/source/host inspection, no production or build configuration changes. |
| North Star / persona | `NS-REAL`, `NS-EVIDENCE`, `NS-OWNERSHIP`, `NS-ADOPTION`, `NS-SIMPLIFY`; `PGE-01`, `PGE-05`–`10`, `PGE-13`, `PGE-15` advance scope clarity only; mathematical/graphics proof preserved, not increased. |
| Delivery mapping | `REL-00`, `FR-00`, `RISK-REL-01/05/09/11/12/13`; all inventory owners and `FCR-*` routes retained; `PTD-00` accepted development discovery preserved, not rerun or promoted to oracle proof. |
| Complexity / preservation | Two subject-local scope/ledger documents; navigation edits only. No new production API, data copy, dependency, implementation plan, test harness, legacy alias or runtime mechanism. Preserve PD-0 budgets and mandatory feature obligations. |
| Risk `RISK-SCOPE-01` | Historical inventory/count/default mistaken for live support -> overbroad product claim. High likelihood from dated snapshot; high impact. Detect via live catalog/settings/enums/target membership and exhaustive ID reconciliation; reduce/fix proposal and hold approval. Release owner retires with approved ledger and retained source inputs. |
| Risk `RISK-SCOPE-02` | Third-party sample availability mistaken for redistribution/material fidelity -> invalid public package. High likelihood from mixed license and extension metadata; critical impact. Detect through primary metadata, asset hashes/notices and material-consumer trace; exclude ambiguous content pending receipts. Content owner retires through `REL-01`, not this source audit. |
| `AC-SCOPE-01` | Every existing module inventory capability ID has exactly one disposition and module owner; all 20 level entries, 18 view enum values and 26 persisted selector names (25 common plus one conditional Editor setting) have an explicit distribution/classification decision. |
| `AC-SCOPE-02` | Product/audiences, named machines, baseline, maps, budgets, mutable roots, compatibility/security/support policies are concrete and trace to the acceptance contract. Mandatory Included targets remain Included; no executable acceptance is inferred. |
| `FM-SCOPE-01` | Missing/duplicate/unmatched row, new catalog entry or selector -> coverage check fails, retain source input, revise ledger before approval; an unexplained omission is a scope blocker. |
| `FM-SCOPE-02` | Unknown asset right or unqualified feature -> explicitly unresolved/excluded, never approved by source presence; retain primary metadata and request owner disposition before package work. |
| Checks | `CHK-SCOPE-01`: exact ID/set coverage, source-reference existence, catalog/view/settings reconciliation and mandatory-feature assertion. `CHK-SCOPE-02`: local Markdown links/anchors, UTF-8, stale names and whitespace. `CHK-SCOPE-03`: review against each `REL-00` clause, public-versus-source boundary and existing PD-0 constants. Negative controls reject missing/duplicate IDs and an invalid disposition using in-memory inputs; no permanent probes. |
| Result / invalidation | **SUPERSEDED for amended documents:** the original 511-ID static slice remains retained in `build/validation/release-scope-20261010/`; amended requirements/classifications are rechecked in ITER-REL-00-02 below. Rights receipts remain useful within their stated identities. No build/cook/runtime/native/performance/package/adopter verdict is added. |

## ITER-REL-00-02 Rendering Amendment

| Field | Record |
| --- | --- |
| Scope / start | Owner-requested revision of Stage 0 only, at `93e86b8113c3fcd0c4921729c3513538b95bc794`; the preceding eight-document scope draft is pre-existing owned work and preserved. No production/CMake/configuration changes. |
| Outcome / mappings | `NS-REAL`, `NS-MATH-DATA`, `NS-EVIDENCE`, `NS-OWNERSHIP`, `NS-ADOPTION`; `PGE-02/05/06/08/09/13/15` advance requirement precision only. `REL-00/04/06/07`, FCR-REN-02/06/07/08/09/10/14/15/25 and existing domain/feature checks. No score or native acceptance increase. |
| Structural budget | Amend existing scope/ledger/feature/plan/report owners; one small Vignette dossier and target inventory ID REN-POST-14. FCR-REN-25 owns two independently proved lens results; retain 49 families, remove weaker Experimental/source-only classifications for mandatory rendering, preserve PTD-00 and separate graph/feature/native authority. |
| `RISK-SCOPE-03` | Required reference/realtime/provider/parity work remains labelled experimental or source-only -> release escapes the requested quality target. High likelihood from previous proposal; critical impact. Detect via disposition/surface/mode assertions, numeric preset review and owner/phase crosswalk; fix docs and hold acceptance. Release owner retires through approved scope plus real feature/performance/review reports. |
| `AC-SCOPE-03` | Required tracer/reconstruction rows and Reference mode are Included/runtime; vignette has one owner/ID/dossier; two independent lens proofs preserve the 49-family registry. Existing exclusions remain explicit. |
| `AC-SCOPE-04` | Native/realtime/reference presets, >=30 FPS and reference truth have distinct proof contracts; parity requires both APIs; principal review requires retained independent identity/findings and full frame evidence. No fallback, finite diagnostic, generated frame, unrun reviewer or metadata can pass those gates. |
| `FM-SCOPE-03` | Missing vignette/duplicate ID or a mandatory row reverted to Experimental -> static coverage/required-row checks reject; revise before freeze, no production mutation. |
| `FM-SCOPE-04` | Source/SDK/example output treated as feature completeness -> retain the current D3D12-only initialization gap, PTD domain/uncertainty limits and unrun review/performance status; acceptance stays blocked. |
| Checks / artifacts | Exact inventory/ledger/class/mode/settings coverage, negative in-memory omission/duplicate/weakened-class controls, local links/anchors, UTF-8, whitespace, production-input hash preservation and `git diff --check`; retained commands/results in `build/validation/release-scope-amendment-20261010/`. Native builds/runs/review are not appropriate proof for this documentation-only amendment and remain unrun. |
| Static result | **PASS:** 138 checks, 22 changed/new documents, 622 local links, 152 anchors, 13 unchanged production inputs and three rejected in-memory negative controls at the revision above. Coverage is 512 capability IDs across 16 inventory owners; the existing 49 report families remain unchanged. This accepts documentation consistency only. |
| Disposition / next work | Rendering requirements incorporated; REL-00 remains BLOCKED for remaining scope/rights/domain/preset freeze. Execute dependency-ready source/SDK/domain discovery through existing owners; implementation requires its own authorized phase and checks. Preserve numerical budgets and do not retune them to candidate results. |

## ITER-REL-00-04 Readability

| Field | Record |
| --- | --- |
| Scope / start | User-requested scope-document cleanup at `aa9fcd94f219e8a3d4dbe7c51457680ad9300783`; pre-existing changes in ReleaseScope/README.md, FirstRelease.md and the first-release plan preserved. Owner: documentation/release maintainer. |
| Outcome / targets | Advance NS-SIMPLIFY and NS-ADOPTION through concrete decisions; preserve NS-OWNERSHIP/NS-EVIDENCE and PGE-13/15 contracts. `REL-00` readability only; no feature or release evidence promotion. |
| Change boundary | Scope README owns decisions; this evidence page owns receipts/history. Acceptance and stage-plan entry links point to the decisions. No runtime exposure, code, build configuration, threshold or approval changes. |
| RISK-SCOPE-04 | Dense audit text hides real commitments or cleanup erases a requirement. High likelihood given reader feedback; high impact if publication promises change. Prevent with explicit unanswered proposals and retained exact contracts/receipts; detect through contract, link and negative-control checks. Documentation owner restores missing requirements before handoff; retire when checks pass. |
| AC-SCOPE-05 | Reader can identify settled choices, seven unanswered proposals with values/consequences, and agent investigations without reading audit history. Scope/rights remain BLOCKED. |
| AC-SCOPE-06 | Seven map IDs, exclusions, operation/performance limits, precise contracts, source identities and previous iteration results remain available through one owning route. Incoming document links resolve. |
| FM-SCOPE-05 | Cleanup omits a selected map, labels an unanswered row approved, changes a budget or breaks a route. Static check rejects; restore the requirement/link. Documentation regression; no source/candidate mutation. |
| CHK-SCOPE-04 | In-memory contract assertions, UTF-8/whitespace, scoped Markdown links/anchors plus incoming links, rejected missing-map/false-approval/changed-budget/broken-link controls and git diff --check. AC-SCOPE-05/06, FM-SCOPE-05 and RISK-SCOPE-04. |
| Decision / evidence | **PASS for document readability/consistency:** scoped local and incoming links, UTF-8/whitespace, contract assertions, four rejected in-memory negative controls and git diff --check. Exact command and hashed results: `build/validation/release-scope-readability-20261010/commands.md` and `verification.json`. Existing REL-00 remains BLOCKED. No build/cook/runtime/GPU/package acceptance is claimed. Recheck on relevant document changes. |
