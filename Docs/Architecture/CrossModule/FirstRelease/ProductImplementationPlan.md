# First Release Product Implementation Plan

[Docs](../../../README.md) / [Up](ImplementationPlan.md)

**Status:** open. Product, build/cook, package and local-export routes exist; remaining work is repairs and qualification. [Recorded progress](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) separates passed local checks from missing final proof.

**Deliverable:** one unsigned internal Launcher/Editor/Game archive, selected content and self-service recovery. Public rights, publisher certificates, signing and publication remain [FR-07](ImplementationPlan.md#fr-07--publish-and-verify).

**Next:** PD-1 **Frames**: resolve Modern Sponza's incorrect output using the [recorded raw/reconstructed controls](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress), then finish Launcher map coverage. Normal Editor quit passes do not close the retained startup-close failures. Use [matched crash tooling](../PerformanceDiagnostics/GpuCrashDebugging/GpuCrashDebuggingDesign.md#acceptance-and-controlled-failures) if a GPU failure recurs.

## How To Close This Plan

| Stage | Current position | What permits exit |
| --- | --- | --- |
| [PD-1](#pd-1--game-and-editor-product-shells) | Read-only asset inspector repaired; rendering, lifecycle and remaining journeys stay open. | Its four source-workspace checks below pass. |
| [PD-2](#pd-2--source-build-cook-and-launcher) | Source reproduction and operations pass; independent adoption remains open. | Source, operation and independent-adopter checks pass. |
| [PD-3](#pd-3--package-and-installation) | Current assembly/integrity checks pass for `0db84502…`; installed Empty/Cornell and explicit-default failure checks pass locally. | Regenerate after PD-1 repairs; finish seven-map, budget, removal and clean/minimum-machine proof. |
| [PD-4](#pd-4--support-incident-response-and-product-closure) | Local diagnostics/review paths exist; final-archive proof is missing. | Self-service, replacement and report reconciliation pass. |

**External proof:** a non-author source/product rehearsal and the required clean/minimum-machine run remain necessary. Another author-run checkout cannot close them.

Work on one unchecked item at a time. Each handoff names the item closed, its candidate report/evidence and the next open item. **Unchecked means incomplete:** record whether it failed, was not run or waits for a prerequisite. Reuse historical passes only for unchanged inputs; do not relabel their binaries or rerun unaffected matrices.

Use the shipped profiles (`ShippingGame`, `ShippingEditor`) on both APIs for product proof. Development results cover their recorded cells only; repeat Development checks when the changed contract or owner check requires them. Source-workspace success closes the relevant PD-1/2 portion; package counterparts wait for PD-3. A report stays blocked while a required portion of its declared scope is missing; public-only obligations remain separately pending for FR-07.

## Delivery Boundary For Every Stage

Follow [scope protection](../../../Strategy/Roadmap.md#scope-protection), [Change Lifecycle](../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate) and the existing owner contracts. Deliver the approved journey or a [bounded simplification](../../../Strategy/Roadmap.md#portfolio-focus-and-existing-code-simplification) that preserves it. Keep hosts as composition, panels as presentation, Core as process authority, Launcher as workflow authority and `CMake/Packaging` as delivery authority. Launcher is the explicit portfolio-presentation exception: useful first-run/scene/readiness/progress/recovery polish belongs in PD-2/3; general Editor/gameplay breadth does not. Reuse shared settings, tasks, operations and manifests; add no support service or validation framework.

Name the failing owner, necessary change, deletion or justified zero-deletion result, and regression check. Apply the [stage complexity budget](../../../Engineering/Workflow/ChangeLifecycle.md#define-the-outcome-and-complexity-budget) to touched existing code and additions: keep only consumed state/API/hooks/copies/dependencies, narrow unnecessary breadth, and delete replaced paths before that stage exits. Compare the actual diff with that budget; a working journey cannot pass with speculative infrastructure or postponed cleanup. Finish with responsibility/whitespace review and the source-style gate. Preserve unrelated work, approved defaults, display resolution and safe retirement; restore fixtures/settings. Every prompt inherits these rules.

## `PD-1` — Game And Editor Product Shells

**Owners:** [Game](../../Modules/Projects/Showcase/ShowcaseCapabilities.md#fcr-prod-01-game-consumer-contract), [Editor](../../Modules/Engine/Editor/EditorCapabilities.md#fcr-prod-04-editor-contract); `FCR-PROD-01/04`.

**Prerequisite:** matching source, product, cooked content and shader/symbol receipts. No archive is needed for these source-workspace checks.

- [ ] **Frames:** Launcher activates Empty and each of the seven maps in Game/Editor on D3D12/Vulkan, with the requested identity and first correct frame. Diagnose Modern Sponza before accepting its cell. (`AC-PROD01-02`, `AC-PROD04-01`.)
- [ ] **Lifecycle:** startup meets the frozen budget; idle, startup-close and quit-during-load settle within **5 s Game / 10 s Editor**, with native child exits and no orphan/partial writes. Retained failures must be repaired. (`AC-PROD01-01/05`, `AC-PROD04-01`.)
- [ ] **Interaction/failure:** help, input/focus, resize, 100/150/200% DPI, Editor settings Save/Reset and Game consumption work; corrupt/denied settings preserve accepted state. Unsupported providers and missing/corrupt content fail visibly; explicit recovery works without false Empty activation. (`AC-PROD01-02/03`, `AC-PROD04-03`.)
- [ ] **Editing/isolation:** edit -> undo -> redo -> Save -> reload preserves the expected result; stale/rejected edits, failed saves and cancelled recook/capture preserve accepted state. Preserve supported transform/visibility/variant edits and read-only asset identity. Game has no Editor/authoring dependency. (`AC-PROD01-04`, `AC-PROD04-02/03/04`.)

**Phase exit:** all four checks pass for their recorded source configuration. Archive/independent portions remain assigned below; a shell repair does not require completing an unrelated rendering research plan.

```text
Execute only PD-1 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md. Apply the owning execution contract and its Stage Source-Style Gate.
Apply Delivery Boundary For Every Stage. Complete the next unchecked item, starting with Frames. Record candidate-bound cells, failures, recovery and native exits; update the item/FCR and name the next item. Reuse unchanged proof.
```

## `PD-2` — Source Build, Cook, And Launcher

**Owners:** [source build](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md#fcr-prod-02-source-adopter-contract), [Launcher](../../Modules/Tools/Launcher/LauncherCapabilities.md#fcr-prod-03-launcher-contract); `FCR-PROD-02/03`.

**Prerequisite:** pinned inputs/public prerequisites and a recorded source candidate. Source validation may proceed independently of a rendering repair; successful launch cannot pass a failed image cell.

Keep Launcher a polished portfolio front door over its existing workflows: a reviewer can recognize/select the examples, understand readiness and dependency work, build/cook/run the requested product/API, follow progress/cancel and recover from a real failure. Improve those existing surfaces when needed, preserving previews and useful operation history; no new dashboard/workflow framework is implied. Fold duplicate Qt/Launcher Core settings, status or operation policy back into the current authority in the same slice. Presentation work is bounded by that first-use/reproduction result rather than a separate cosmetic backlog.

- [x] **Reproduction:** documented cold build/cook/run, approved owner gates and network-blocked warm replay cover Empty, seven maps and a subset; manifests/products agree logically and no author path/cache is required. (`AC-PROD02-01/02/03/04`.)
- [x] **Operations:** direct and GUI requests agree on effects, artifacts and result. Missing tools, bad/Unicode paths, failed children, cancellation, locked/stale outputs and clean containment have observed outcomes; no false success, source deletion or orphan child. Reuse the current process-boundary checks; finish remaining workflow negatives and preserve distinct failure/cancel/timeout/skipped/unavailable/missing-artifact results. (`AC-PROD02-05`, `AC-PROD03-01` through `05`.)
- [ ] **Adopter:** a non-author completes the documented source route without private repair. This laptop's author-run fresh clone is useful evidence, but cannot close this item. (`AC-PROD02-01/02`, `CHK-PROD02-02`.)

**Recorded:** [PD-2 reports](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) retain fresh source clones, a spaced Unicode checkout, all-map/subset offline cooks and native launches, GUI/CLI parity, owner gates and failure/cancel/clean checks. Cold route: **20 min 29 s**; offline replay: **2 min 41 s**, plus **2 min 5 s** for eight native map checks. OS system-commit high-water: **26.24 GiB**; observed owned storage: **25.48 GiB**. **Next: a non-author performs the documented rehearsal.**

**Phase exit:** these source/operation checks pass in the owning reports. Installed Launcher checks remain PD-3; public tag/download verification remains FR-07.

```text
Execute only PD-2 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md. Apply the owning execution contract and its Stage Source-Style Gate.
Apply Delivery Boundary For Every Stage. Complete the next unchecked source/operation/adopter item or the Roadmap-selected bounded Launcher presentation/simplification slice. Preserve useful scene/previews/readiness/progress/recovery and one Core operation authority; build no general workflow platform. Reuse valid evidence; record candidate, commands, artifacts, cleanup and next item. An author rehearsal cannot pass independent adoption.
```

## `PD-3` — Package And Installation

**Owner:** [packaging](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#fcr-prod-05-package-contract); `FCR-PROD-05`, `REL-03`.

**Prerequisite:** PD-1/2 technical results and reviewed final inputs. Earlier archives can test packaging mechanics; final acceptance waits for repaired products.

- [x] **Assembly mechanics:** `Package.ps1` produced unsigned `0db84502…21d957` with three products, admitted helpers, shared defaults, engine baseline, recipes, manifests, notices/SBOM, provenance and a separate matched symbol companion. Generated products freeze after build/cook; external inputs remain pinned. [Candidate results](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress). (`AC-PKG-01/02/03/06`, assembly scope.)
- [x] **Failure integrity:** owner stage/archive and full-command negatives reject stale, extra, missing, corrupt, escaping and forbidden inputs without publication. Six-product precompiled import and local Game compilation use the same owned binary paths. (`CHK-PKG-01`.)
- [ ] **Final archive/rehearsal:** regenerate after PD-1 repairs. Exact-archive Launcher -> Editor/Game must cover Empty, all seven maps and a subset, selective sync/offline run, relocation/read-only extraction, Qt replacement and removal as a standard user. Local Empty/Cornell checks passed on both APIs; removal was blocked by automatic approval review. Finish the clean/minimum-machine, no source/SDK/admin/override/author-cache, user-state and budget checks. (`AC-PKG-04/05`, installed portions of `FCR-PROD-01/03/04`, `CHK-PKG-02`.)

**Phase exit:** `FCR-PROD-05` and `REL-03` pass the unsigned internal scope for that archive. This is not permission to publish.

```text
Execute only PD-3 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md. Apply the owning execution contract and its Stage Source-Style Gate.
Apply Delivery Boundary For Every Stage. Complete the next unchecked archive/integrity/rehearsal item through the existing owner. Bind results to one archive hash; unavailable required checks stay open.
```

## `PD-4` — Support, Incident Response, And Product Closure

**Owner:** [self-service/diagnostics](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#fcr-prod-06-support-and-incident-contract); `FCR-PROD-06`.

**Prerequisite:** PD-3's exact verified archive and symbol companion. Earlier diagnostic fixtures qualify only their original inputs.

- [ ] **Self-service/diagnostics:** a non-author completes all three product journeys; supported startup/crash/timeout/GPU routes retain consent, privacy, bounds and exact symbol identity. Verify unavailable categories/manual recovery truthfully; automatic CPU crash/hang collection is not required. (`AC-ADOPT-01/02/03`.)
- [ ] **Review/replacement:** critical-defect review blocks approval; immutable replacement/withdrawal rejects wrong identities, preserves state and supports the declared rollback. No support service or deadline is advertised. (`AC-ADOPT-04/05`.)
- [ ] **Closure:** reconcile `FCR-PROD-01` through `06` against one final source/configuration/archive, with every applicable AC/FM/CHK passed or explicitly excluded by its scope owner. Failed, not-run and unavailable required checks keep this item open. No unresolved defect blocking that product scope may be waived by prose.

**Phase exit:** all six reports pass their admitted internal-product scope. Retire this plan, repair incoming links and leave one roadmap completion row. Retain useful evidence in ignored artifacts. Broader rendering/performance, public signing/rights and publication gates remain in the mother plan.

```text
Execute only PD-4 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md. Apply the owning execution contract and its Stage Source-Style Gate.
Apply Delivery Boundary For Every Stage. Complete the next unchecked archive-bound item using existing tools. Reconcile six reports; retire this plan only after its phase exit. Public approval remains separate.
```
