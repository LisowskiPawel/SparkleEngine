# First Release Product Implementation Plan

[Docs](../../../README.md) / [Up](ImplementationPlan.md)

**Status:** remaining work for PD-1–4. Implemented paths are summarized in the [roadmap](../../../Strategy/Roadmap.md#release-progress); exact results stay in candidate reports.

Deliver Launcher, Editor and Game in one unsigned internal archive, with selected content and self-service recovery. Public-release prerequisites belong to [FR-07](ImplementationPlan.md#fr-07--publish-and-verify).

**Prerequisite:** close the [remaining GPU crash check](../PerformanceDiagnostics/GpuCrashDebugging/ImplementationPlan.md#gcd-4--prove-native-crash-debugging). Then resume PD-1 with matched tooling. Collect Modern Sponza evidence, fix the owning defect and rerun affected journeys. This product plan remains open; keep the existing owners and reuse completed systems.

Each stage follows [Change Lifecycle and its source-style gate](../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate). Reuse evidence only when its candidate/configuration still matches; record changed-input invalidation and unavailable required checks before advancing.

## `PD-1` — Game And Editor Product Shells

**Owners/checks:** [Game](../../Modules/Projects/Showcase/ShowcaseCapabilities.md), [Editor](../../Modules/Engine/Editor/EditorCapabilities.md); `FCR-PROD-01/04`.

Remaining:

- Prove Empty and all seven maps through Launcher → Game/Editor on D3D12/Vulkan. Fix native failures; measure Game quit ≤5 s and Editor quit ≤10 s.
- Exercise help, input/focus, display resize/DPI, shared settings/reset, unsupported providers and missing/corrupt content. Loading failure must remain visible.
- Prove the admitted Editor edit/undo/redo/save/reload journey and Game's isolation from Editor dependencies.

**Exit:** the owning product criteria pass for the recorded configuration. Preserve display-selected resolution, shared rendering settings and approved defaults; new effect toggles remain deferred.

```text
Execute only PD-1 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md. Use the qualified GPU crash tooling with matching product/shader/symbol receipts for native failures.
Reuse the current product shells and settings owner. Reproduce the remaining failures, fix their owners, and exercise the journeys above. Record native child exits, timings and candidate-bound FCR-PROD-01/04 results.
```

## `PD-2` — Source Build, Cook, And Launcher

**Owners/checks:** [source build](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md), [Launcher](../../Modules/Tools/Launcher/LauncherCapabilities.md); `FCR-PROD-02/03`.

Remaining:

- Replay affected checks on current source in the separate fresh clone. Preserve the earlier cold-build result; prove warm build/cook, seven maps and a selected subset.
- Exercise missing tools/bad paths, child failure, cancellation, locked output, stale cook, Unicode/quoting, bounded cleanup and runtime handoff. Verify direct and GUI routes agree.
- Retain the approved owner-check runner's truthful result states. Bind its existing positive/negative checks to the candidate being accepted.

**Exit:** source-adopter and Launcher criteria pass with exact commands, artifacts and cleanup. The approved fresh-repository rehearsal uses this laptop's installed tools; it does not prove an independent actor or clean operating system.

```text
Execute only PD-2 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md.
Use the existing fresh clone and typed Launcher operations. Reuse valid cold-build and owner-check results; close the remaining warm, failure and GUI-equivalence checks. Record FCR-PROD-02/03 results and rehearsal limits.
```

## `PD-3` — Package And Installation

**Owner/checks:** [packaging](../../Modules/BuildAndPackaging/PackagingAndInstallation.md); `FCR-PROD-05`, `REL-03`.

Remaining:

- Rebuild affected Shipping products and regenerate admissions, symbols, notices/SBOM and provenance for one frozen source candidate. Produce and verify the archive through `CMake/Packaging/Package.ps1`.
- Repeat affected stale/extra/missing/corrupt payload and transactional-failure checks. Verify precompiled import and local compilation replace the same binary paths.
- Exercise extract/start/selective-sync/offline-run/remove using that archive and its verified separate content store. Complete dependency/helper closure, Qt replacement and package-write checks.

**Exit:** manifest/archive hashes agree; failed staging emits no publishable archive; the standard-user route and its environment limits are retained. A provisional Editor regression copy cannot pass this stage. Independent/minimum-machine adoption remains required by the release contract.

```text
Execute only PD-3 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md.
Freeze source after the product fixes. Use the existing packaging owner to regenerate and verify the unsigned three-product archive. Exercise the remaining negative, precompiled and standard-user routes; bind FCR-PROD-05/REL-03 results to its hash.
```

## `PD-4` — Support, Incident Response, And Product Closure

**Owner/checks:** [self-service and diagnostics](../../Modules/BuildAndPackaging/PackagingAndInstallation.md); `FCR-PROD-06`.

Remaining:

- Against the final archive, exercise onboarding, known-issue review and recovery for all three products. Recheck local diagnostic consent, privacy, retention and native symbol identity.
- Retain the distinction between synthetic crash/hang/GPU projections and real native incidents. Record automatic crash/hang collection and shader-symbol export as unavailable where unsupported.
- Complete critical-defect review and a valid immutable replacement/withdrawal rehearsal, including applicability, rollback and evidence invalidation. Preserve the no-support policy and no response deadlines.
- Reconcile `FCR-PROD-01` through `06` against the same source/configuration/archive; name every failed or unavailable required check.

**Exit:** all six reports pass their admitted scope with residual limits stated. Then remove this plan, update incoming links to the enduring owners, and keep only a short completion row in the roadmap. Required reports/logs belong in ignored artifacts, not new documentation files.

```text
Execute only PD-4 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md.
Use the verified archive and existing export/review tools. Reuse applicable fixture checks; finish self-service, incident and replacement rehearsals. Reconcile all six product reports. Delete this plan only after its remaining work is closed; broader release gates remain in the mother plan.
```
