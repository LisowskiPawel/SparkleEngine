# Build, packaging and repository automation: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

**Execution order: 09 of 21 (first focused visit).** Start: Stages 0-1 in Wave 3. Resume / dependency: Stages 2-3 in Wave 8; stage 4 after ThirdParty and the final product changes. Follow the [stage queue](README.md#suggested-execution-order); do not finish this whole file before a listed prerequisite. Dominant owners may change this module earlier as part of a complete system.

## Outcome and ownership

Deliver one fast maintainer toolchain and a minimal complete reviewer runtime, retiring alternate compiler/generator/linkage and release-process obligations.

**Retain:** clang-cl, Ninja Multi-Config, Rider direct CMake, engine DLLs/private PCHs, six configurations, standalone Launcher, both APIs, intentional native diagnostics, required runtime closure/notices/identity and local refinable style tooling.

**Target flow:** One selected toolchain + targets/profile -> parallel incremental CMake/Ninja build -> owned runtime deployment/inventory -> one complete offline Launcher/Editor/Game package.

Dominant execution owner for: LA-04, LA-17, LA-18, BU-01, BU-02, BU-03, BU-04, BU-05, BU-06, BU-07, BU-08, BU-09, BU-10, BU-11, BU-12. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Adapt the surviving design

- **Selected composition directly:** qualify then express clang-cl, Ninja Multi-Config and engine DLLs as the supported route. Remove compiler/generator/static-engine selection and exclusive arguments/cache facts across CMake, Launcher, presets and Rider integration. Keep six profiles, private PCHs, standalone Launcher, both APIs and required Windows ABI/SDK inputs; they remain intentional dimensions.
- **One runtime bundle inventory:** after extra modes/automation and developer package payloads retire, narrow target/output staging and package metadata to actual Launcher/Editor/Game dependencies, selected content, identity and notices. Maintainer tools remain in the workspace. Remove obsolete schema/option plumbing rather than replacing the bespoke process with a package framework; preserve required source/license companions and native debug identity.

**Structural acceptance:** show reduced options/target edges/metadata through real DLL consumers, no-op/one-file iteration and a relocated offline package. Repair missing staged files without treating a no-compile build as deployment proof. A one-option compiler abstraction, hidden static route, forced developer SDK payload or lost runtime/license dependency fails.

**Reference application:** [P3](README.md#reference-choices-and-limits) and the [Windows/Rider and CMake precedents](../../References.md#windows-build-and-rider-selection) guide local composition. No copied feature matrix, package stack or claim of an internal NVIDIA tool standard.

Apply the [shared before/after design gate](README.md#reshape-the-surviving-design) within the existing owning stages; these requirements do not authorize additional feature cuts.

## Footprint and reduction obligation

Containing module baseline: **50 files / 3,207 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **3 files / 118 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| LA-17 | 17 files / 1,974 lines |
| LA-18 | 5 files / 506 lines |
| BU-08 | 2 files / 185 lines |
| BU-09 | 5 files / 197 lines |

## Named retirement closure

These are source-audit seeds for the owning stages, including indirect code/build/data paths. Follow the shared last-consumer fixed-point procedure; names alone are not a blind delete list. Refresh actual callers before editing. Remove a closure row only when its owning stage is verified complete; split a partially delivered row rather than retain a completed diary. Full third-party runtime/build removals require their own last-consumer proof.

| Decision / owning stage | Direct system and secondary retirement to close | Protected surviving consumers / boundary |
| --- | --- | --- |
| LA-17; stage 1 | cl.exe frontend selection and exclusive compile policy. Trace WorkspaceCompiler values/defaults/settings, GUI/CLI selection, -T ClangCL routing, /MP-only policy, generated constants, freshness/reset signatures and dependency-specific compiler options. Update callers to explicit clang-cl. | Selected Windows/MSVC ABI, /MD-family runtime agreement, SDK/STL/CRT and applicable compiler-compatibility conditions. |
| LA-18; stage 1 | Visual Studio/MSBuild generator/solution route and alternate generator policy. Retire CMakeGeneratorModel choices used only by removed generators, -A/-T/MSBuild arguments, solution-path/open/freshness checks, VS_* project conveniences and mirrored generator state across Launcher/CMake/docs. | Ninja Multi-Config, six profiles, standalone Launcher, Rider direct CMake and necessary SDK/resource environment setup. |
| BU-09; stage 2 | Unneeded subsystem/compiler/configuration combinations. Retire unsupported options, cache/CMake definitions, conditional source/dependency lists, generated feature flags and extra test/package combinations; remove split state after selected composition is direct. | Both APIs, suite/standalone Launcher, six profiles, DLLs/PCHs and intentional GPU capture/crash/debug composition. |
| BU-10; stage 2 | CodeStyle.bat wrapper. Retire file and every live Rider/README/CodeStyle/Engineering/CI command reference; use the existing CodeStyle.ps1 interface. Check external-tool arguments/quoting and source-family selection. | Formatter/rules, style scripts and local checks; no deletion of the refinable PowerShell implementation. |
| BU-11; stage 2 | Hosted GitHub source-check workflow. Retire .github/workflows/repository-checks.yml and only workflow-exclusive hooks/scripts/badges/status promises after actual consumer checks. | Local source/style checks, .clang-format/.clang-tidy/.editorconfig and required script owners; no blanket .github/config purge. |
| BU-12; stage 2 | Optional sanitizer build mode. Retire CMake/Build/Sanitizers.cmake, its include, ENABLE_SANITIZERS option, exclusive compile/link/runtime staging flags and obsolete configure instructions. | Normal debug/PDB/errors, retained native GPU validation/crash modes and threaded production execution. |
| BU-07; stage 3 | Cooker/compiler payloads in reviewer archive. Close ReleaseProducts product selection, runtime support/source-package inventories, package generators/validators and staging/deployment references that force developer components into runtime bundles. | Tools remain built/usable in source workspace; Launcher/Editor/Game and their actual DLL/data/notices/debug identity ship completely. |
| BU-08; stage 3 | Mandatory bespoke SBOM/provenance workflow. Trace package generators, SPARKLE_RELEASE_INPUTS metadata, PackageInputs/Metadata/Manifest/Outputs and validation/archive callers. Retire exclusive SPDX/provenance schemas/writers/settings after minimal inventory owns consumed identity/notice requirements. | SPARKLE_SOURCE_PACKAGES/Qt-source metadata may still support retained runtime closure/licenses: prove each last use. Compiler source provenance and crash identity are independent retained contracts. |
| BU-01 / BU-02 / LA-04; stage 1 | Retire SPARKLE_BUILD_SHARED as an alternate engine-mode choice after DLL is the direct model; close static-only exports/registration/global-state branches, selected IDE metadata and generated settings. Update EngineModule and all module callers, source groups, deployment/PDB inventory and documentation. | Keep non-engine static/object contributors and selected external ABI/runtime linkage. DLL export/registration correctness is mandatory; moving state into global services is not simplification. |

## Recorded maintainer workflow and build selection

This is the pre-reduction workflow and observed environment, retained for Stage 0 comparison. The versions and commands describe the recorded baseline, not the selected future route or current-release qualification. The approved Clang/Ninja/Rider/DLL decisions and their conditional exits above remain authoritative.

### Requirements

The recorded Windows baseline is below. This is existing build evidence, not a second compiler/IDE support commitment:

| Tool | Version |
| --- | --- |
| Visual Studio Community / MSVC | 2026 18.7.0 / 14.51.36231 |
| Windows SDK | 10.0.26100.0 |
| CMake / Git for Windows | 4.3.3 / 2.54.0 |
| Qt | 6.11.1, MSVC x64 |
| Vulkan SDK | 1.4.350.0, with Slang 2026.8 and DXC 1.9.0.5347 |

Observed candidate tools on 25 November: **clang-cl/lld-link 22.1.3, Ninja 1.13.2 and Rider 2026.1.2**; the installed CMake remains 4.3.3. The [build-stack recommendation](../PortfolioScopeDecisions.md#recommended-windows-build-stack) compares generators, names the retained Windows compatibility inputs and leaves Rider/native-debug/DLL qualification open. These versions describe the observed setup, not the latest available releases.

The offline shader tool stages ShaderMake, `dxc.exe`, `slangc.exe` and their DLL/module payloads through its CMake owner. `DXC_SDK_ROOT`, `SLANG_SDK_ROOT` and `SPIRV_TOOLS_SDK_ROOT` select explicit SDK inputs; ShaderMake does not acquire compilers. The tool's `slang-standard-module-*` trees are managed SDK payload: unchanged preparation preserves them; SDK changes repair/prune their contents. Slang 2026.8 full-PDB CLI export needs the pinned [loop-location](../../../ThirdParty/slang/Patches/SlangLoopSourceLocations.patch) and [debug-format](../../../ThirdParty/slang/Patches/SlangDebugInformationFormat.patch) corrections. Use short paths: the SDK directory exporter can omit long-path artifacts, which the cooker rejects. Ordinary SDK setup does not amend qualified GPU-crash receipts.

The project and standalone Launcher minimum is CMake 3.30, including its configuration-aware transitive dependency metadata; the [Visual Studio 18 2026 generator](https://cmake.org/cmake/help/latest/generator/Visual%20Studio%2018%202026.html) below requires 4.2+. Launcher currently uses four top-level build jobs, with separate MSVC/Visual Studio per-project compile policy. The proposed Ninja starting bound on this 16-core/32-thread, approximately 64-GB machine is 16 compile jobs and at most two links, subject to timing/peak-memory checks. It is not a measured optimum. Private PCHs do not replace explicit includes.

### Build Launcher

These are the **current baseline commands**, retained until the Clang/Ninja migration is qualified; they are not the selected future workflow. Use a short checkout path. Run them from the repository root with your Qt path. Stop if a command fails.

```powershell
cmake -S Tools/Launcher -B build/private/tools/SparkleLauncher -G "Visual Studio 18 2026" -A x64 -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/msvc2022_64
cmake --build build/private/tools/SparkleLauncher --config DevelopmentEditor --target SparkleLauncher --parallel 4
& ./artifacts/dev/Tools/SparkleLauncher/Binaries/DevelopmentEditor/SparkleLauncher.exe
```

Launcher selects content and builds, cooks and runs Editor or Game. Select scenes before cooking. Empty requires no example download.

### Command-line example

With only Empty selected in `Projects/Showcase/Levels.catalog`, prepare and run an Empty Game. Stop if any command fails:

```powershell
$launcher = './artifacts/dev/Tools/SparkleLauncher/Binaries/DevelopmentEditor/SparkleLauncher.exe'
$profiles = '--editor-profile', 'ShippingEditor', '--game-profile', 'ShippingGame'
& $launcher @profiles --run workspace.sync-code
& $launcher @profiles --run workspace.build.game
& $launcher @profiles --run cook.tools.prepare
& $launcher @profiles --run cook.all
& $launcher @profiles --level Empty --product game --graphics-api d3d12 --run levels.run
```

`--level` selects sync/run, not cook scope. Only selected assets download. Offline use requires completed preparation.

Build and dependency files are under `build/`; binaries and cooked content are under `artifacts/dev/`. [SparkleArtifactContract.cmake](../../../CMake/SparkleArtifactContract.cmake) defines the paths. Source and extracted products use the same workspace layout.

The root [CMakeLists.txt](../../../CMakeLists.txt) composes the project. Shared policy lives under `CMake/`; [EngineModule.cmake](../../../Engine/CMake/EngineModule.cmake) owns module linkage/exports. Modules declare sources, dependencies and SDK/deployment rules. Runtime inventories follow linked targets, including private transitive dependencies; owners explicitly stage dynamic plugins/data. Keep one artifact layout and dependency inventory.

Cook cleanup affects the current project's selected output categories. Scenes And Meshes includes dependent scene assets and the registry; textures and shaders have separate scopes. Clean Cook All and Cooked Outputs remove the project's entire cooked tree.

### Build-selection sources

The current [build-stack decision](../PortfolioScopeDecisions.md#recommended-windows-build-stack) applies these precedents to the installed Windows machine; they are not a measured Sparkle speedup or a company-wide NVIDIA tool mandate.

| Primary source | Decision relevance |
| --- | --- |
| [Rider 2026.1 CMake support](https://www.jetbrains.com/rider/whatsnew/2026-1/) | Direct CMake opening/editing/building became available as Beta. Installed Rider is 2026.1.2; Sparkle navigation/build/native debug and its six profiles still need qualification. Earlier .sln-only advice does not establish the current feature boundary. |
| [CMake Ninja Multi-Config](https://cmake.org/cmake/help/v4.3/generator/Ninja%20Multi-Config.html), [Ninja execution/pools](https://ninja-build.org/manual.html) | Keep one tree with explicit --config selection and bounded native scheduling. Plain Ninja uses one build type per tree. Actual elapsed time and memory remain local measurements. |
| [Falcor pinned Windows presets](https://github.com/NVIDIAGameWorks/Falcor/blob/759aad033ff610fb0d82c74f7e0a508d0096d5f2/CMakePresets.json) | NVIDIA graphics-project precedent for Ninja Multi-Config on Windows, alongside a VS preset. The Windows Ninja preset uses MSVC; it does not justify claiming an internal clang-cl standard. |
| [clang-cl configuration](https://clang.llvm.org/docs/UsersManual.html#clang-cl), [Windows ABI compatibility](https://clang.llvm.org/docs/MSVCCompatibility.html), [lld-link DLL/PDB support](https://lld.llvm.org/windows_support.html) | Native Clang can retain Microsoft-compatible libraries/runtime/SDK. Ninja needs explicit compiler/environment selection. DLL exports, PDB/source debugging and crash identity must be verified for the selected stack. |

## Existing packaging and publication constraints

Prepare the release base with a complete build and cook. [ReleaseProducts.cmake](../../../CMake/Packaging/ReleaseProducts.cmake) declares `release_package`; [Package.ps1](../../../CMake/Packaging/Package.ps1) requires canonical `build/`, reviewed `SPARKLE_RELEASE_INPUTS`, matching source/product hashes and the final `--shader-symbols` export. The current implementation publishes an **unsigned internal** archive with a path, SHA-256, manifest, SBOM and provenance. The selected BU-08 reduction replaces the bespoke SBOM/provenance workflow with bounded inventory/identity/notices under [BuildPackaging stage 3](09_BuildPackaging.md); do not treat the old format as a new target requirement. Staging rejects changed qualified bytes. Signing and public-release qualification remain open in [Roadmap](../../Roadmap.md#before-the-first-release).

The combined package requires **both ShippingEditor and ShippingGame inventories**. Multi-config builds produce both; a single Shipping configuration supplies only its own inventory.

The existing [SyncProducts.ps1](../../../CMake/Packaging/SyncProducts.ps1) imports prepared products for a maintainer workspace. General cross-workspace product import is a removal recommendation under BU-05, with its answer still pending; it is not a reviewer delivery requirement. Your source/dependency/content sync remains distinct and retained. Installer/updater, contributor onboarding, old-store migration and a freshness service are outside scope.

Cookers stage complete validated output before publication. Core owns containment, locks, incomplete markers and in-process rollback. Readers retain guards through their final filesystem access, including worker reads; decoding may continue from owned bytes afterward. Interrupted publication or failed rollback requires cleanup and recook, with source/settings outside disposable output. Persisted journal replay and power-loss recovery are unsupported.

## Local style-check workflow

Check every added/modified source, header and shader after the responsibility review:

```powershell
CMake\CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>"
git diff --check
```

Repeat per file. Use `-Staged` only when it covers the whole change; it checks working-tree content, so inspect the actual index for partial staging.

Record the tool, coverage, mechanical result and manual layout review. Repeat affected checks after edits and identify unchecked files. Documentation changes use documentation checks.

## Stage 0: freeze the actual responsibility and smaller target

Inspect current Visual Studio-generator/static-engine caches without replacing them. Trace SparkleBuildProfiles/Foundation/WorkspaceFeatures, EngineModule exports, RuntimeDeployment and package inventory readers. Distinguish MSVC ABI/SDK/STL/CRT/Qt from cl.exe/IDE policy. Freeze source/build identity and cold/no-op/one-file/public-header/link baselines; no full rebuild is evidence of fast iteration.

Start at these current sources (entry/retirement seeds, not a blind delete list):

- [CMake/SparkleBuildProfiles.cmake](../../../CMake/SparkleBuildProfiles.cmake)
- [CMake/SparkleBuildFoundation.cmake](../../../CMake/SparkleBuildFoundation.cmake)
- [CMake/SparkleWorkspaceFeatures.cmake](../../../CMake/SparkleWorkspaceFeatures.cmake)
- [Engine/CMake/EngineModule.cmake](../../../Engine/CMake/EngineModule.cmake)
- [CMake/Packaging/ReleaseProducts.cmake](../../../CMake/Packaging/ReleaseProducts.cmake)
- [CMake/Packaging/Payload/PackageMetadata.ps1](../../../CMake/Packaging/Payload/PackageMetadata.ps1)

Before coding, apply the shared Stage 0 protocol to the complete producers/callers and write the minimal target ownership/data-flow sketch plus measured deletion/replacement budget. Preserve every 🟢 contract; ⚫* cuts stay conditional. Inputs and requirements not exercised by the source audit remain unqualified. Reject a candidate that removes useful parallel work, duplicates state or merely moves the same code.

**Implementation prompt:**

```text
Prepare Stage 0 of Docs/Implementation/PortfolioReduction/09_BuildPackaging.md: freeze the actual responsibility and smaller target.
Read this module's outcome, retained contracts, source entry points and Stage 0 questions; read Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md, Docs/Roadmap.md and Docs/Implementation/PortfolioReduction/README.md. Refresh current HEAD/working-tree identities and respect concurrent work. Audit the named retirement closure/participation in this plan, including narrowed retained capabilities; prepare a transitive retire/retain-with-consumer/unresolved manifest and rerun it after each layer disappears. Do not implement the production refactor in this stage.

Acceptance criteria:
- Specify this plan's Adapt the surviving design contract: vanished assumption, concrete smaller API/data/control flow, caller changes, selected reference lesson and local limits. Budget all replacement/caller code and name the retained behavior/failure check.
- an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.
- Map each proposed changed function to one cohesive responsibility/owner and an intent-based name; distinguish workflow orchestration from implementation mechanisms. Specify callers/contracts/files/build outputs to retire and the sole target path; no legacy/backward-compatibility reader, alias, migration, disabled stub or old/new implementation may be planned.
- Define stage-specific success, invalid-input/failure/cancellation/lifetime checks and measured bounds before coding; retain all approved parallel/data-oriented/game/render/RHI contracts. No unsupported performance or qualification claim.

Failure modes to reject:
- The feature disappears but surviving contracts retain obsolete generality, or an upstream framework is copied without a local invariant.
- Historical BuildPackaging paths, footprint counts or old receipts are treated as current without tracing actual callers/build inputs.
- Mixed retained/retired responsibilities or CPU/GPU reader lifetimes are omitted, leaving no safe complete-system cut or measurable replacement budget.
- A generic wrapper/framework, serial execution, retained compatibility path or source relocation is substituted for a smaller complete design. A first-order-only dependency scan is insufficient.

Deliver the concrete design/ownership map, bounded change and net footprint budget, planned checks and open conditions in ignored stage evidence. Keep CodeStyle's function/namespace/layout rules explicit in the design; permanent test-only additions are not authorized. Do not close an unproven implementation stage or change a recorded decision.
```

## Stage 1: Deliver the sole Clang/Ninja/Rider DLL build route

Decisions: BU-01, BU-02, BU-03, LA-04, LA-17, LA-18.

**Implementation prompt:**

```text
Implement Stage 1 of Docs/Implementation/PortfolioReduction/09_BuildPackaging.md: Deliver the sole Clang/Ninja/Rider DLL build route.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Configure controlled migration output with explicit clang-cl C/C++, Ninja Multi-Config, captured x64 SDK/library/resource environment and required Qt/SDK roots. Qualify lld-link full PDBs before making it the only linker. Retire cl.exe selection, Visual Studio/MSBuild generator/solution-opening, alternate static-engine mode and their exclusive flags/IDE fields through Launcher/CMake/targets. Keep required ABI/runtime conditions, vswhere/vcvars discovery and non-engine static/object contributors. Preserve six profiles, registration/exports and standalone Launcher. No legacy switch or internal ABI version is needed.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Use real DLL consumers and inspect exports/registration/runtime closure. Rider loads/navigates/builds/debugs actual Launcher/Editor/Game; all profiles configure/build relevant targets. Private PCHs, no-op/one-file/public-header invalidation and parallel independent native jobs work. Start job tuning from 16 compiles/two links on the recorded machine, then measure. Replace canonical incompatible cache explicitly only after the replacement passes; do not claim a speedup from choosing Ninja alone.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- A generated solution, cl.exe choice, static-engine fallback or old generator remains necessary.
- PCH/DLL registration, CRT/Qt closure, six-profile outputs or Rider source breakpoints fail on the sole selected route.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 2: Retire optional feature matrices and automation entry points

Decisions: BU-09, BU-10, BU-11, BU-12.

**Implementation prompt:**

```text
Implement Stage 2 of Docs/Implementation/PortfolioReduction/09_BuildPackaging.md: Retire optional feature matrices and automation entry points.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Remove unneeded subsystem combination switches while retaining suite/standalone Launcher composition, both APIs and supported capture/crash modes. Remove the batch style wrapper, hosted GitHub source workflow and sanitizer-only flag/helper/options. Update Rider/manual/style callers to PowerShell. Do not delete formatter rules, local checks, PDB/native debugging or parallel workers with those optional modes.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- No removed option/wrapper/workflow remains as a live documented command; supported configure/build combinations work. Local CodeStyle/rules are still refinable and callable. Both native debug and GPU-crash modes retain required dependencies, symbols and breadcrumbs.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Removing build switches accidentally removes native debug/capture support or a retained API.
- The removed batch/CI/sanitizer entry still has live callers, or local refinable style tooling disappears.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 3: Reduce reviewer package payload and bespoke metadata process

Decisions: BU-07, BU-08.

**Implementation prompt:**

```text
Implement Stage 3 of Docs/Implementation/PortfolioReduction/09_BuildPackaging.md: Reduce reviewer package payload and bespoke metadata process.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Exclude developer AssetCooker/TextureCooker/ShaderCompiler executables and source-only dependencies from reviewer package composition while keeping all workspace tools. Replace mandatory bespoke SBOM/provenance artifact workflow with one bounded file/dependency inventory, revision/input identity and required notices. Update every generator/stager/validator reader together; do not remove runtime closure checks or matched shader/native debug identity. Package support metadata must describe what is actually shipped.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- Combined ShippingEditor/ShippingGame inventories produce a complete relocated offline Launcher/Editor/Game bundle. No compiler/source acquisition/cook runs are required by the reviewer. Exact shader symbols and binary identity are retained where required for diagnostics; unused SDK binaries are not blindly shipped. Package-byte reduction is measured independently of authored source-line savings.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- Developer tools remain required/shipped for reviewer launch, or validators still require retired SBOM/provenance artifacts.
- Minimal inventory drops a real runtime DLL, notice or exact shader/native diagnostic identity.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Stage 4: Close the inspection bundle and maintainer iteration loop

Decisions: BU-04.

**Implementation prompt:**

```text
Implement Stage 4 of Docs/Implementation/PortfolioReduction/09_BuildPackaging.md: Close the inspection bundle and maintainer iteration loop.
Read that plan, its Stage 0 evidence/dependencies, Docs/Implementation/PortfolioReduction/README.md, and Docs/CodeStyle.md, Docs/Engineering.md, Docs/Goals.md, Docs/References.md and Docs/Roadmap.md. Refresh actual source/callers and preserve unrelated/concurrent changes. Scope stays within the recorded decisions. Exhaust this stage's named retirement closure/participation: follow both outgoing dependencies and incoming users through code/data/build/runtime/docs, repeating until no newly orphaned owned node remains. Keep one owner per system.

Required delivery:
Reconcile artifact layout/deployment after all system owners remove their products. Preserve locked/correct accepted-output behavior, English keyboard/mouse windowed/borderless walkthrough and source-entry links. Resolve rights and existing Vulkan/image blockers before representing an external package as qualified. BU-05 product import is not authorized for deletion by its unanswered cell; keep current behavior without building more generality.

Implementation rules:
- Apply this plan's Adapt the surviving design contract to surviving APIs, data, states, orchestration and callers. Use its bounded reference lessons; remove assumptions made obsolete by this stage.
- Enforce every CodeStyle.md rule. Each changed function has one cohesive responsibility/invariant and an intent-based name. Orchestration states domain steps with explicit inputs/results; implementation operations own substantial I/O, decoding, math, validation, native or UI details. Do not create forwarding-only/numbered helpers or context/service bags to imitate separation.
- Complete the cross-module producer/consumer change, registrations, settings/CLI/UI, schemas, build/dependency/package membership and docs. No legacy or backward-compatibility support for replaced owned contracts: no aliases, migrations, version gates, disabled stubs, old readers or dual implementations. Regenerate disposable output. Preserve required selected external SDK/ABI contracts.
- Preserve parallel work, data-oriented layout, separate game/render ownership, immutable publication, worker settlement and GPU queue/fence lifetime. Simplify mechanisms, never concurrency or a retained feature.

Acceptance criteria:
- Prove the applicable survivor-design checks through real retained callers; show the before/after contracts and net result. Justify unchanged boundaries by retained invariants; no unused general protocol or replacement framework.
- From an extracted package, run every promised scene in Editor/Game on D3D12/Vulkan, approved controls and basic animation offline as a standard user. Check relocation/writable state/failures, actual runtime dependency inventory and source matching. Maintain separate source-build/cook and package evidence. Public release/signing/rights limits remain explicit until their own evidence closes.
- Review each changed function and neighboring callers for responsibility, abstraction level, naming and actual data/lifetime ownership; remove duplicated policy/state and now-unused files/APIs. Run CMake/CodeStyle.ps1 -Mode Check -Path "<repo-relative-file>" for each changed owned C++/header/shader with clang-format 22.1.3, plus git diff --check. Use documentation/language checks for other files; formatting alone cannot pass the responsibility review.
- Run the stage's positive and failure/lifetime checks on identified inputs and the affected real targets/APIs. An absent observation remains open. Provide a closure manifest classifying every discovered node as retired, retained with a named live consumer, or unresolved (stage stays open). No obsolete caller, registration, output expectation or compatibility route remains; include replacement/addition/moved code in the net footprint result.

Failure modes to reject:
- Old general machinery survives with fewer users, callers still carry retired policy, or code moves without exploiting the reduced scope.
- An extracted bundle works only on the source machine, online, or after automatic recooking.
- One map/API/product silently fails, or source-build success is represented as package/image qualification.
- Renaming/moving code without simplification, mixed-responsibility functions, a surviving old/new path, serial replacement or premature CPU/GPU destruction fails delivery. Orphan helpers, target edges, feature flags, generated products or docs left behind after the visible feature disappears also fail delivery.

Deliver the complete change and identity-bound commands/results, manual responsibility review, net code/state/API/dependency delta and remaining limits in ignored stage evidence. Remove temporary probes; do not add permanent test-only infrastructure. For a failed conditional cut, remove the failed candidate, keep one authorized working path and leave the cut open; do not ship a compatibility fallback. Prune only the stage whose acceptance actually passed and repair links.
```

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

Use the implementation prompt attached to the next unmet stage; complete that stage's acceptance and failure checks before pruning it.
