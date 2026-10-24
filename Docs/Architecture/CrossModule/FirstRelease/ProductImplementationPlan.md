# First Release Product Implementation Plan

[Docs](../../../README.md) / [Up](ImplementationPlan.md)

**Status:** open; remaining product checks, fixes and final-archive qualification only. Completed work stays in the [roadmap](../../../Strategy/Roadmap.md#release-progress).

Deliver Launcher, Editor and Game in one unsigned internal archive, with selected content and self-service recovery. Public-release prerequisites belong to [FR-07](ImplementationPlan.md#fr-07--publish-and-verify).

**Next:** finish [PD-1](#pd-1--game-and-editor-product-shells): resolve Modern Sponza's rendered output through the [lighting owner](../../Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ImplementationPlan.md#required-execution-order), then close shutdown and interaction checks. Use [qualified GPU crash tooling](../PerformanceDiagnostics/GpuCrashDebugging/GpuCrashDebuggingDesign.md#acceptance-and-controlled-failures) with matching receipts if a native GPU failure recurs.

Reuse existing implementations and valid results; repeat only checks invalidated by changed inputs. Follow [Change Lifecycle](../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate). Record exact source/configuration/artifact identities in the [candidate reports](../../../Acceptance/FeatureCompletionReports.md). A stage stays open while a required check is failed or unavailable; historical passes do not qualify changed binaries.

## Delivery Boundary For Every Stage

Apply the [renderer-first product direction](../../../Strategy/ExecutiveSummary.md#product-identity) and [priority order](../../../Strategy/Requirements.md#target-proposition): product work makes the selected rendering workloads correct, reproducible and usable by another engineer. Launcher, Editor, packaging and self-service receive the smallest complete repair needed for their approved journeys. Required failure, lifetime and adoption checks remain binding; they do not authorize broader tooling products.

Before each repair, trace the failing journey through its existing owner, producers, consumers and shutdown path. Choose reuse, extension, local refactoring or replacement with deletion before adding a concept. Keep the [complexity budget and touched-path reconciliation](../../../Engineering/Workflow/ChangeLifecycle.md#define-the-outcome-and-complexity-budget) in the existing iteration record: name the necessary addition, the duplication or obsolete path removed, and the check that could expose a regression. Generalize only a demonstrated shared invariant; keep distinct workflow policy with its owner.

| Stage | Production boundary |
| --- | --- |
| PD-1 | Repair rendering through its feature owner; keep product hosts as lifecycle/composition and panels as presentation. Reuse operation slots, capture tickets, transactions and settings authority. |
| PD-2 | Repair the existing build/cook/sync/run operation. Core owns process encoding and lifetime; Launcher owns workflow intent and recovery. Keep compiler/dependency diagnostics with their source owner wherever possible. |
| PD-3 | Extend the existing packaging route and manifests. Regenerate disposable outputs; do not introduce another staging, installation or artifact authority. |
| PD-4 | Exercise the existing local export/review/replacement routes. Preserve the no-support policy; do not grow a support service, telemetry system or incident framework. |

Check IDs call for evidence, not a permanent runner, fixture, report generator or production fault-injection API. Use existing commands and disposable local probes under [Validation And Evidence](../../../Engineering/Verification/ValidationAndEvidence.md#submitted-test-code); restore temporary catalog/settings changes before handoff.

Every copyable prompt below includes this boundary. End each owner-sized repair with [responsibility refinement](../../../Engineering/Workflow/ChangeLifecycle.md#finish-with-responsibility-refinement), the [integration-hook ledger](../../../Engineering/Foundations/ModuleOwnership.md#feature-enclosure-and-integration-hook-budget), review of every changed file and its direct dependencies, and the stage source-style gate. A file split must remove knowledge from its caller; relocating a god function or adding forwarding layers does not close the stage. Preserve supported features and the approved release scope while leaving one coherent production path.

## `PD-1` — Game And Editor Product Shells

**Owners/checks:** [Game](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Editor](../../Modules/Engine/Editor/EditorCapabilities.md); `FCR-PROD-01/04`.

- Qualify first correct frames, starting with Modern Sponza's noisy/saturated output. Requested-map readiness and exit code do not prove image correctness.
- Resolve native startup/backpressure and the retained D3D12 Game shutdown misses. Pre-device initialization and render waits delay close handling; preserve supported providers and safe GPU retirement. Qualify idle/load shutdown within 5 s for Game and 10 s for Editor.
- Finish input/focus, display/DPI, unsupported-provider, missing/corrupt-content recovery and quit-during-load checks. Rendering settings stay in Editor and are shared with Game.
- Finish Editor native stale-edit and capture cancellation; qualify remaining profile journeys. Exact archive/component proof belongs to PD-3.

**Exit:** the source-workspace journeys above pass for the recorded configuration, preserving display-selected resolution and shared defaults. Archive-dependent `FCR-PROD-01/04` checks close in PD-3/4; their overall verdicts stay open until then.

```text
Execute only PD-1 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md.
Apply this plan's Delivery Boundary For Every Stage to each repair and its final source-shape review.
Start with Modern Sponza's rendered output through its existing lighting owner. Preserve retained shutdown failures and the 5 s Game / 10 s Editor limits. Finish the remaining journeys above using matched crash tooling when needed. Record exact candidate results; do not repeat unaffected work or treat map readiness as visual proof.
```

## `PD-2` — Source Build, Cook, And Launcher

**Owners/checks:** [source build](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md), [Launcher](../../Modules/Tools/Launcher/LauncherCapabilities.md); `FCR-PROD-02/03`.

- Finish network-blocked warm replay, seven-map and selected-subset source coverage.
- Finish remaining build/cook faults, locked/stale outputs, Unicode source-adopter journeys, cleanup and runtime handoff; verify CLI/GUI equivalence. Reuse the qualified process-boundary quoting, cancellation, readiness and log-failure checks.
- Revalidate affected owner-runner positive/negative results against the accepted source candidate.

**Exit:** `FCR-PROD-02/03` pass the owning source/Launcher criteria. This laptop's fresh clone cannot supply independent-adopter or clean-OS proof; those requirements remain open.

```text
Execute only PD-2 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md.
Apply this plan's Delivery Boundary For Every Stage to each repair and its final source-shape review.
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
Apply this plan's Delivery Boundary For Every Stage to each repair and its final source-shape review.
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
Apply this plan's Delivery Boundary For Every Stage to each repair and its final source-shape review.
Use existing export/review tools to finish the archive-bound checks above. Reconcile all six reports; retire this plan only after closure. Broader release gates remain in the mother plan.
```
