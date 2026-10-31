# First Release Product Implementation Plan

[Docs](../../../README.md) / [Up](ImplementationPlan.md)

**Status:** open. Product, build/cook, package and local-export routes exist; remaining work is repairs and qualification. [Recorded progress](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) separates passed local checks from missing final proof.

**Deliverable:** one unsigned internal Launcher/Editor/Game archive, selected content and self-service recovery. Public rights, publisher certificates, signing and publication remain [FR-07](ImplementationPlan.md#fr-07--publish-and-verify).

**Next:** [PD-3: installed store recovery](#pd-3--package-and-installation). Archive `3743e72a…` has repaired assembly, matching symbols and installed-member failure proof. Finish store recovery, affected positive reconciliation and removal from its retained receipts. Reuse applicable PD-1/2 source proof and retain the [approved product/lighting boundary](../../../Acceptance/FirstRelease.md#product-plan-and-public-release).

## How To Close This Plan

| Stage | Current position | What permits exit |
| --- | --- | --- |
| [PD-1](#pd-1--game-and-editor-product-shells) | Source-workspace complete; [candidate reports](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress). | Revalidate affected checks if source, products or content change. Installed portions remain PD-3. |
| [PD-2](#pd-2--source-build-cook-and-launcher) | Source-workspace complete; [candidate reports](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress). | Revalidate affected checks if source inputs change. Installed Launcher portions remain PD-3. |
| [PD-3](#pd-3--package-and-installation) | `3743e72a…`: repaired assembly, matching symbols and installed-member failures pass. Earlier installed positives retain `e771a244…` identity. | Finish store recovery, affected positive reconciliation and removal in `FCR-PROD-05`/`REL-03`. |
| [PD-4](#pd-4--support-incident-response-and-product-closure) | `e771a244…` onboarding, native D3D12 removal/recovery, export/symbol checks and replacement/rollback have supporting proof. | After PD-3, requalify affected archive-bound cells and make all six reports pass for one final archive. |

**Rehearsal:** author/agent runs satisfy the [accepted actor scope](../../../Acceptance/FeatureCompletionReports.md#report-closure-and-review). Clean/minimum-machine checks still apply; record the actual environment.

Work on one unchecked item at a time. Each handoff names the item closed, its candidate report/evidence and the next open item. **Unchecked means incomplete:** record whether it failed, was not run or waits for a prerequisite. Reuse historical passes only for unchanged inputs; do not relabel their binaries or rerun unaffected matrices.

Use the shipped profiles (`ShippingGame`, `ShippingEditor`) on both APIs for product proof. Development results cover their recorded cells only; repeat Development checks when the changed contract or owner check requires them. Source-workspace success closes the relevant PD-1/2 portion; package counterparts wait for PD-3. A report stays blocked while a required portion of its declared scope is missing; public-only obligations remain separately pending for FR-07.

## Delivery Boundary For Every Stage

Every stage and copied prompt follows [stage completion and resume](../../../Engineering/Workflow/ChangeLifecycle.md#stage-completion-and-resume). The [state update](../../../Engineering/Workflow/ChangeLifecycle.md#update-stage-state-before-handoff) is part of its exit: record the result, reconcile remaining work and name the next item before handoff. Later aggregate proof remains separate.

Follow [scope protection](../../../Strategy/Roadmap.md#scope-protection), [Change Lifecycle](../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate) and the existing owner contracts. Deliver the approved journey or a [bounded simplification](../../../Strategy/Roadmap.md#portfolio-focus-and-existing-code-simplification) that preserves it. Keep hosts as composition, panels as presentation, Core as process authority, Launcher as workflow authority and `CMake/Packaging` as delivery authority. Launcher is the explicit portfolio-presentation exception: useful first-run/scene/readiness/progress/recovery polish belongs in PD-2/3; general Editor/gameplay breadth does not. Reuse shared settings, tasks, operations and manifests; add no support service or validation framework.

Name the failing owner, necessary change, deletion or justified zero-deletion result, and regression check. Apply the [stage complexity budget](../../../Engineering/Workflow/ChangeLifecycle.md#define-the-outcome-and-complexity-budget) to touched existing code and additions: keep only consumed state/API/hooks/copies/dependencies, narrow unnecessary breadth, and delete replaced paths before that stage exits. Compare the actual diff with that budget; a working journey cannot pass with speculative infrastructure or postponed cleanup. Finish with responsibility/whitespace review and the source-style gate. Preserve unrelated work, approved defaults, display resolution and safe retirement; restore fixtures/settings. Every prompt inherits these rules.

## `PD-1` — Game And Editor Product Shells

**Owners:** [Game](../../Modules/Projects/Showcase/ShowcaseCapabilities.md#fcr-prod-01-game-consumer-contract), [Editor](../../Modules/Engine/Editor/EditorCapabilities.md#fcr-prod-04-editor-contract); `FCR-PROD-01/04`.

**Source-workspace complete:** frames, lifecycle, interaction/failure and editing/isolation pass in the [PD-1 report](../../../../artifacts/validation/releases/0.1.0/241ef037-product-20261025/pd1-source-completion.json). Final replay uses `241ef037` plus the retained patch, source fingerprint `9a1f7d03…`; reused checks retain their original identities. Newer main-branch rendering changes are outside this verdict.

Installed counterparts remain in PD-3. Lighting quality, release performance and public qualification remain in their release gates.

## `PD-2` — Source Build, Cook, And Launcher

**Owners:** [source build](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md#fcr-prod-02-source-adopter-contract), [Launcher](../../Modules/Tools/Launcher/LauncherCapabilities.md#fcr-prod-03-launcher-contract); `FCR-PROD-02/03`.

**Source-workspace complete:** reproduction and operations pass in the [PD-2 report](../../../../artifacts/validation/releases/0.1.0/241ef037-product-20261025/pd2-source-final/completion.json). Cold/offline proof retains `9bf95541` plus its source snapshot; affected owner checks, shell results and actual Launcher GUI child were revalidated on frozen `241ef037` plus patch. The rejected non-author requirement is superseded explicitly; no historical binary is relabelled.

Preserve the [Launcher presentation and operation boundary](../../Modules/Tools/Launcher/LauncherCapabilities.md#at-a-glance). Installed Launcher checks remain PD-3; public tag/download verification remains FR-07.

## `PD-3` — Package And Installation

**Owner:** [packaging](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#fcr-prod-05-package-contract); `FCR-PROD-05`, `REL-03`.

**Prerequisite:** PD-1/2 technical results and reviewed final inputs. Earlier archives can test packaging mechanics; final acceptance waits for repaired products.

- [x] **Assembly mechanics:** unsigned `e771a244…f6b9bd`, frozen `241ef037` plus retained patch, source fingerprint `5ab719d7…`. Three products, admitted helpers, defaults/content recipes, manifests/notices/SBOM/provenance and matched private symbols are verified. [Assembly report](../../../../artifacts/releases/0.1.0/e771a244095431d06663927b7b181ab12710478d630f01919dc9ff37a9f6b9bd/report.json). (`AC-PKG-01/02/03/06`, assembly scope.)
- [x] **Failure integrity:** owner stage/archive and full-command negatives reject stale, extra, missing, corrupt, escaping and forbidden inputs without publication. Six-product precompiled import and local Game compilation use the same owned binary paths. (`CHK-PKG-01`.)
- [x] **Installed positive journeys:** 32 Empty/seven-map Game/Editor/API cells; fresh Cornell-only GUI selection/cook/handoff and four subset cells; offline operation, Unicode/read-only relocation, modified app-local Qt, installed Editor edit/undo/redo/save/reload and sampled memory/quit budgets. [Candidate report](../../../../artifacts/validation/releases/0.1.0/241ef037-product-20261025/pd3-installed-final/completion.json) records the accepted existing-laptop environment and input reuse. Original user state is restored exactly. (`AC-PKG-04`, positive portions of `AC-PKG-05`.)
- [x] **Repaired assembly and symbols:** [archive `3743e72a…`](../../../../artifacts/releases/0.1.0/3743e72aafeb3dd89e410ceb1139056f326e05091503eb1814beb8a4e07d48c9/report.json), frozen `241ef037` plus repaired inputs, fingerprint `15336751…`; [candidate identity](../../../../artifacts/validation/releases/0.1.0/241ef037-product-20261025/pd3-failures-final/final-candidate.json) and [matched symbols](../../../../artifacts/validation/releases/0.1.0/241ef037-product-20261025/pd3-failures-final/final-symbols.json) pass. The shared manifest-member presence/containment guard and cooked-directory recovery are already repaired; [source review](../../../../artifacts/validation/releases/0.1.0/241ef037-product-20261025/pd3-failures-final/source-review.json) is source proof, not installed acceptance.
- [x] **Installed member failures:** `3743e72a…` passes [1,302 startup removals](../../../../artifacts/validation/releases/0.1.0/241ef037-product-20261025/pd3-failures-final/installed-members-completion.json), 434 each for Launcher, direct Game and direct Editor, including 18 `msvcp140_2.dll` cases. Fourteen manifest/identity/requested-content/corruption controls pass at their declared boundaries; case-level logs, native dialog text, exits and restoration are retained. Setup failures and corrected execution oracles remain explicit in the report. (`FM-PKG-04` identity/corruption portions, `FM-PKG-05`, `CHK-PKG-02`.)
- [ ] **Installed store recovery:** finish denied/full-store publication, preserved accepted state, released-fault retry and cooked-directory recreation on the repaired candidate. Start from the [current restoration receipt](../../../../artifacts/validation/releases/0.1.0/241ef037-product-20261025/pd3-failures-final/member-startup-20261031-execution/restoration.json) and reserve the current package user roots again. The [earlier acquisition-marker assertion](../../../../artifacts/validation/releases/0.1.0/241ef037-product-20261025/pd3-failures-final/latest-state-restoration.json) remains an unresolved rehearsal setup failure; validate acquisition receipts before injecting faults. (`FM-PKG-04/05`, `CHK-PKG-02`.)
- [ ] **Repaired-candidate positive reconciliation:** map every required installed cell to applicable proof or revalidation. Earlier `e771a244…` map/subset/Qt/relocation passes keep their identity; the changed admission/recovery binaries need their affected positive and negative checks. Record the exact input comparison and any required replay in `FCR-PROD-05`; do not rerun unrelated source checks or qualify new bytes by renaming old receipts. (`AC-PKG-04/05`.)
- [ ] **Removal and package verdict:** record authorized removal of the selected verified extraction, unchanged current user-state inventories, and the completed `FCR-PROD-05`/`REL-03` matrix for `3743e72a…`. A prior automatic approval rejection left the old extraction retained; it is not a current standing instruction to delete it or a passed removal check. Missing permission or required evidence remains explicit. (`AC-PKG-05`, `CHK-PKG-02`.)

**Phase exit:** `FCR-PROD-05` and `REL-03` pass the unsigned internal scope for that archive. This is not permission to publish.

```text
Execute only PD-3 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md. Apply the owning execution contract and its Stage Source-Style Gate.
Apply Delivery Boundary For Every Stage. Complete the next unchecked archive/integrity/rehearsal item through the existing owner. Bind results to one archive hash; unavailable required checks stay open.
Before handoff, update the stage result, remaining plan and next item.

```

## `PD-4` — Support, Incident Response, And Product Closure

**Owner:** [self-service/diagnostics](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#fcr-prod-06-support-and-incident-contract); `FCR-PROD-06`.

**Prerequisite:** PD-3's exact verified archive and symbol companion. [Retained-archive checks](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) are supporting proof; final acceptance waits for PD-3.

The checked supporting items below are complete for their recorded archives. Once PD-3 accepts the repaired candidate, compare product/helper/package/symbol inputs, reuse unchanged cells with an explicit applicability record, and execute only affected archive-bound onboarding/incident/export/replacement checks. Reconcile those results into the final candidate's reports before closure; a supporting checkmark does not qualify different binaries.

- [x] **Self-service/diagnostics supporting proof:** three-product onboarding/help/exit and manual recovery pass on `e771a244…`. [Native incident proof](../../../../artifacts/validation/releases/0.1.0/2a1bcc8a-pd4-native-20261026/native-incident.json) now exercises actual D3D12 device removal: native `0x887A0005`/fatal diagnostics, unforced exit 3, exact-PDB preview/consented export and same-byte restart/normal quit. Reuse unchanged export/privacy/bounds/symbol checks in the [supporting report](../../../../artifacts/validation/releases/0.1.0/2a1bcc8a-pd4-20261026/features/FCR-PROD-06/completion.json). Ordinary Shipping DRED/Aftermath, automatic CPU crash/hang collection and shader source attribution remain unavailable; Vulkan native loss is not claimed. Final acceptance waits for PD-3. (`AC-ADOPT-01/02/03`.)
- [x] **Replacement supporting proof:** verified `0.1.1` archive `f9b72b48…` passes review and interrupted-extraction rejection. Actual `0.1.0 -> 0.1.1 -> 0.1.0` Game starts exercise separate stores and rollback; settings/authored content are preserved and original user-state inventories restored exactly. [Replacement report](../../../../artifacts/validation/releases/0.1.0/2a1bcc8a-pd4-20261026/replacement-completion.json). Critical-defect, withdrawal, wrong-identity and false-support-promise checks pass locally; no public action or support service is implied. (`AC-ADOPT-04/05`.)
- [ ] **Closure:** the [six-report reconciliation](../../../../artifacts/validation/releases/0.1.0/2a1bcc8a-pd4-20261026/six-report-reconciliation.json) is supporting proof for `e771a244…`, fingerprint `5ab719d7…`, with product verdicts blocked. After PD-3, reconcile `FCR-PROD-01` through `06` against the one final source/configuration/archive and companion symbols, including affected PD-4 requalification. Every applicable AC/FM/CHK must pass or have an explicit scope-owner exclusion; failed/not-run/unavailable required checks keep this item open. Preserve the old missing-`msvcp140_2.dll` failure as the repaired-candidate regression oracle, not a task to rediscover or a defect waived by prose. Concurrent checkout edits retain their own identity.

**Phase exit:** all six reports pass their admitted internal-product scope. Retire this plan, repair incoming links and leave one roadmap completion row. Retain useful evidence in ignored artifacts. Broader rendering/performance, public signing/rights and publication gates remain in the mother plan.

```text
Execute only PD-4 from Docs/Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md. Apply the owning execution contract and its Stage Source-Style Gate.
Apply Delivery Boundary For Every Stage. Complete the next unchecked archive-bound item using existing tools. Reconcile six reports; retire this plan only after its phase exit. Public approval remains separate.
Before handoff, update the stage result, remaining plan and next item.

```
