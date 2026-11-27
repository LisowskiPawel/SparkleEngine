# Build, packaging and repository automation: staged reduction plan

[Execution order and shared delivery rules](README.md) | [Closed scope record](../PortfolioScopeDecisions.md) | [Engineering](../../Engineering.md)

Status: planned, source inspected on 25 November 2026 at `039d7b802` with the current documentation working tree. No stage is delivered by writing this plan. The [shared stage protocol](README.md#stage-protocol) applies without duplication here; remove each verified stage in the delivery change.

## Outcome and ownership

Deliver one fast maintainer toolchain and a minimal complete reviewer runtime, retiring alternate compiler/generator/linkage and release-process obligations.

**Retain:** clang-cl, Ninja Multi-Config, Rider direct CMake, engine DLLs/private PCHs, six configurations, standalone Launcher, both APIs, intentional native diagnostics, required runtime closure/notices/identity and local refinable style tooling.

**Target flow:** One selected toolchain + targets/profile -> parallel incremental CMake/Ninja build -> owned runtime deployment/inventory -> one complete offline Launcher/Editor/Game package.

Dominant execution owner for: LA-04, LA-17, LA-18, BU-01, BU-02, BU-03, BU-04, BU-05, BU-06, BU-07, BU-08, BU-09, BU-10, BU-11, BU-12. Cross-module deletion/refactoring is authorized for this owner's complete system; keep touched module dependency/lifetime rules.

## Footprint and reduction obligation

Containing module baseline: **50 files / 3,207 physical code/script lines**. Assigned cross-module whole-file retirement candidates: **3 files / 118 lines**; assigned shared spans: **0 lines in 0 files**. These are unique allocations from the [ledger](README.md#footprint-and-size-targets), not a guaranteed net diff. Required replacements/callers, moved code and new retained behavior are accounted there. Data-only changes do not count as native code savings.

Measured containing scopes that need a smaller replacement (overlapping; **not deletable totals**):

| Decision | Existing shared/replacement scope |
| --- | --- |
| LA-17 | 17 files / 1,974 lines |
| LA-18 | 5 files / 506 lines |
| BU-08 | 2 files / 185 lines |
| BU-09 | 5 files / 197 lines |

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

**Stage 0 exit:** an actionable producer/consumer and lifetime map, final-consumer classification, specific changed files/targets, scoped correctness/performance bounds and a proposed net reduction. This is the implementer's concrete design checkpoint, not a request for a new framework or approval ceremony.

## Stage 1: Deliver the sole Clang/Ninja/Rider DLL build route

Decisions: BU-01, BU-02, BU-03, LA-04, LA-17, LA-18.

Configure controlled migration output with explicit clang-cl C/C++, Ninja Multi-Config, captured x64 SDK/library/resource environment and required Qt/SDK roots. Qualify lld-link full PDBs before making it the only linker. Retire cl.exe selection, Visual Studio/MSBuild generator/solution-opening, alternate static-engine mode and their exclusive flags/IDE fields through Launcher/CMake/targets. Keep required ABI/runtime conditions, vswhere/vcvars discovery and non-engine static/object contributors. Preserve six profiles, registration/exports and standalone Launcher. No legacy switch or internal ABI version is needed.

**Exit evidence:** Use real DLL consumers and inspect exports/registration/runtime closure. Rider loads/navigates/builds/debugs actual Launcher/Editor/Game; all profiles configure/build relevant targets. Private PCHs, no-op/one-file/public-header invalidation and parallel independent native jobs work. Start job tuning from 16 compiles/two links on the recorded machine, then measure. Replace canonical incompatible cache explicitly only after the replacement passes; do not claim a speedup from choosing Ninja alone.

## Stage 2: Retire optional feature matrices and automation entry points

Decisions: BU-09, BU-10, BU-11, BU-12.

Remove unneeded subsystem combination switches while retaining suite/standalone Launcher composition, both APIs and supported capture/crash modes. Remove the batch style wrapper, hosted GitHub source workflow and sanitizer-only flag/helper/options. Update Rider/manual/style callers to PowerShell. Do not delete formatter rules, local checks, PDB/native debugging or parallel workers with those optional modes.

**Exit evidence:** No removed option/wrapper/workflow remains as a live documented command; supported configure/build combinations work. Local CodeStyle/rules are still refinable and callable. Both native debug and GPU-crash modes retain required dependencies, symbols and breadcrumbs.

## Stage 3: Reduce reviewer package payload and bespoke metadata process

Decisions: BU-07, BU-08.

Exclude developer AssetCooker/TextureCooker/ShaderCompiler executables and source-only dependencies from reviewer package composition while keeping all workspace tools. Replace mandatory bespoke SBOM/provenance artifact workflow with one bounded file/dependency inventory, revision/input identity and required notices. Update every generator/stager/validator reader together; do not remove runtime closure checks or matched shader/native debug identity. Package support metadata must describe what is actually shipped.

**Exit evidence:** Combined ShippingEditor/ShippingGame inventories produce a complete relocated offline Launcher/Editor/Game bundle. No compiler/source acquisition/cook runs are required by the reviewer. Exact shader symbols and binary identity are retained where required for diagnostics; unused SDK binaries are not blindly shipped. Package-byte reduction is measured independently of authored source-line savings.

## Stage 4: Close the inspection bundle and maintainer iteration loop

Decisions: BU-04.

Reconcile artifact layout/deployment after all system owners remove their products. Preserve locked/correct accepted-output behavior, English keyboard/mouse windowed/borderless walkthrough and source-entry links. Resolve rights and existing Vulkan/image blockers before representing an external package as qualified. BU-05 product import is not authorized for deletion by its unanswered cell; keep current behavior without building more generality.

**Exit evidence:** From an extracted package, run every promised scene in Editor/Game on D3D12/Vulkan, approved controls and basic animation offline as a standard user. Check relocation/writable state/failures, actual runtime dependency inventory and source matching. Maintain separate source-build/cook and package evidence. Public release/signing/rights limits remain explicit until their own evidence closes.

## Ownership handover

Before closing the last stage, read the surviving implementation with its callers and explain its data flow, invariants, threading, failures and destruction in plain source-oriented terms. Remove newly unused state/types/files/build membership, temporary validation code and duplicated comments/docs. Keep evidence outside active plans and current architecture guidance with its proper owner. The shared completion gate applies even to an all-retained module; do not invent deletions to satisfy a line quota.

**Continuation prompt:** Implement the next unmet stage in this document under the shared protocol. Read its actual source/callers first, preserve concurrent edits and all protected capability/threading contracts, deliver the complete cross-module change, verify the named exits, report net code/state/API change and prune only the stage whose evidence actually passed.
