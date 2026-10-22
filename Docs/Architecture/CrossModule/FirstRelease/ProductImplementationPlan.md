# First Release Product Implementation Plan

[Docs](../../../README.md) / [Up](ImplementationPlan.md)

**Status:** open; remaining product checks, fixes and final-archive qualification only. Completed work stays in the [roadmap](../../../Strategy/Roadmap.md#release-progress).

Deliver Launcher, Editor and Game in one unsigned internal archive, with selected content and self-service recovery. Public-release prerequisites belong to [FR-07](ImplementationPlan.md#fr-07--publish-and-verify).

**Next:** recheck Modern Sponza in [PD-1](#pd-1--game-and-editor-product-shells). If the failure persists, collect it with [qualified GPU crash tooling](../PerformanceDiagnostics/GpuCrashDebugging/GpuCrashDebuggingDesign.md#acceptance-and-controlled-failures) and matching product/shader/symbol receipts, then fix its owner.

Reuse existing implementations and valid results; repeat only checks invalidated by changed inputs. Follow [Change Lifecycle](../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate). Record exact source/configuration/artifact identities in the [candidate reports](../../../Acceptance/FeatureCompletionReports.md). A stage stays open while a required check is failed or unavailable; historical passes do not qualify changed binaries.

## `PD-1` — Game And Editor Product Shells

**Owners/checks:** [Game](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Editor](../../Modules/Engine/Editor/EditorCapabilities.md); `FCR-PROD-01/04`.

- After the Modern Sponza check, close Launcher → Game/Editor coverage for Empty and seven maps on D3D12/Vulkan. Verify Game quit ≤5 s and Editor quit ≤10 s.
- Finish help, input/focus, resize/DPI, shared settings/reset, unsupported-provider and missing/corrupt-content checks. Loading failure must remain visible.
- Verify Editor edit/undo/redo/save/reload and Game's isolation from Editor dependencies.

**Exit:** `FCR-PROD-01/04` pass the admitted journeys, preserving display-selected resolution and approved shared defaults.

```text
Execute only PD-1 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md.
Start with Modern Sponza; finish the remaining journeys above using existing owners and matched crash tooling. Record native child exits, quit timings and exact candidate results.
```

## `PD-2` — Source Build, Cook, And Launcher

**Owners/checks:** [source build](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md), [Launcher](../../Modules/Tools/Launcher/LauncherCapabilities.md); `FCR-PROD-02/03`.

- In the existing fresh clone, close current-source warm build/cook, seven-map and selected-subset coverage.
- Finish missing-tool/bad-path, child-failure/cancellation, locked-output, stale-cook, Unicode/quoting, cleanup and runtime-handoff checks; verify CLI/GUI equivalence.
- Revalidate affected owner-runner positive/negative results against the accepted source candidate.

**Exit:** `FCR-PROD-02/03` pass the owning source/Launcher criteria. This laptop's fresh clone cannot supply independent-adopter or clean-OS proof; those requirements remain open.

```text
Execute only PD-2 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md.
Reuse valid cold-build and owner-runner results; finish affected source, failure and CLI/GUI checks above. Record exact commands, artifacts, cleanup and rehearsal limits.
```

## `PD-3` — Package And Installation

**Owner/checks:** [packaging](../../Modules/BuildAndPackaging/PackagingAndInstallation.md); `FCR-PROD-05`, `REL-03`.

- After product fixes, freeze source and regenerate affected Shipping products, admissions, symbols, notices/SBOM and provenance. Produce and verify the archive through `CMake/Packaging/Package.ps1`.
- Close affected stale/extra/missing/corrupt-payload and transactional-failure checks; verify precompiled import and local compilation replace the same binary paths.
- Finish extract/start/selective-sync/offline-run/remove with the separate content store, dependency/helper closure, Qt replacement and package-write checks.

**Exit:** `FCR-PROD-05`/`REL-03` qualify the exact archive: matching manifests/hashes, no publishable output on staging failure, and standard-user rehearsal. Independent/minimum-machine requirements remain open until exercised.

```text
Execute only PD-3 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md.
Regenerate the unsigned three-product archive through the existing packaging owner. Finish the checks above and bind results to its hash; name unavailable adoption checks.
```

## `PD-4` — Support, Incident Response, And Product Closure

**Owner/checks:** [self-service and diagnostics](../../Modules/BuildAndPackaging/PackagingAndInstallation.md); `FCR-PROD-06`.

- On the PD-3 archive, finish onboarding, known-issue review and recovery for all three products; recheck diagnostic consent, privacy, retention and symbol identity.
- Complete critical-defect review and immutable replacement/withdrawal with applicability, rollback and evidence invalidation. Keep real incidents distinct from fixtures and automatic CPU crash/hang collection marked unavailable; preserve no-support/no-deadline policy.
- Reconcile `FCR-PROD-01` through `06` against the same source/configuration/archive, including every failed or unavailable required check.

**Exit:** all six reports pass their admitted scope with limits stated. Remove this plan, update incoming links and leave one roadmap completion row. Keep reports/logs in ignored artifacts.

```text
Execute only PD-4 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md.
Use existing export/review tools to finish the archive-bound checks above. Reconcile all six reports; retire this plan only after closure. Broader release gates remain in the mother plan.
```
