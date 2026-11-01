# First Release Implementation Plan

[Docs](../../../README.md) / [Up](../README.md)

Remaining stages: [FR-03](#fr-03--feature-closure) / [FR-04](#fr-04--release-maps) / [FR-05](#fr-05--performance-stability-and-native-proof) / [FR-06](#fr-06--candidate-freeze-and-reproduction) / [FR-07](#fr-07--publish-and-verify) / [FR-08](#fr-08--stabilize-and-close).

**Status:** open release plan; remaining work is tracked in the roadmap and linked stage plans

**Purpose:** show the approved rendering well and make its results reproducible. Execute scene/pass/radiance, GPU correctness, quality and measured-cost work first. Supporting product/foundation changes are bounded repairs to a named rendering or reviewer-run blocker under [Roadmap scope protection](../../../Strategy/Roadmap.md#scope-protection); no subsystem-completeness program is selected. Keep the existing release gates and return to the rendered result after each supporting slice.

Start with [current progress](../../../Strategy/Roadmap.md#release-progress) for completed FR-00–02 internal scope and missing checks. [Roadmap](../../../Strategy/Roadmap.md#where-we-are-and-what-is-next) selects the next task; this page supplies only unfinished phase work. Recheck completed cells only when their admitted inputs change; final public rights/signing remain FR-07.

[First Release Acceptance](../../../Acceptance/FirstRelease.md#release-gates) owns release verdicts. Feature-local Architecture checks and [candidate reports](../../../Acceptance/FeatureCompletionReports.md) own feature proof.

The [product reports](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) retain five technical passes and current PD-4 controls for `a0f34c8b...` with its explicit removal exclusion. [Product closeout](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress) stays blocked by the source-applicable FWC-4 scene-publication defect. The owner requested document retirement and later resumed FWC-4. Its recorded source publication repair passes for that candidate; the simplified lock/rollback/incomplete-marker implementation passes its affected source/native checks. Selected-map content proof follows FR-04, and a repaired archive with affected product reconciliation follows FR-06. This mother plan retains the unresolved acceptance obligation. [First Release](../../../Acceptance/FirstRelease.md#product-plan-and-public-release) retains the environment and FR-07 signing/public-rights boundaries.

Choose the section for your current task: [release phases](#release-tasks), [Game/Editor/Launcher/package](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress), [completed foundation repairs](../../../Acceptance/FeatureCompletionReports.md#foundation-contract-progress), or [feature owners](#complete-feature-routing). The separate foundation plan is retired under the [portfolio demo boundary](../../../Acceptance/FirstRelease.md#portfolio-demo-delivery-boundary); selected content checks belong to FR-04, shader deltas to their existing owner, and candidate reconciliation to FR-06.

## Release Tasks

Execute by [technical inputs before formal approval](../../../Acceptance/FirstRelease.md#shared-delivery-evidence). Selected content/map inputs and a named unsigned archive may be prepared while FR-03 is open; feature delivery observations join the same rehearsal later reconciled at FR-06. Do not require REL-08/09 approval before producing their feature prerequisites. Required public gates keep their strict order.

### `FR-03` — Feature Closure

**Selection:** use Roadmap's required-feature priority to choose one missing consumer result, failed check or [bounded simplification](../../../Strategy/Roadmap.md#portfolio-focus-and-existing-code-simplification). First decide whether the route needs proof, a local repair, narrowing/retirement of unnecessary existing surface, or a specifically admitted missing feature. The 49-family registry routes that decision; it does not authorize 49 audits or redesigns. Simplification preserves a named rendering/product/evidence path and may remove private dead code without inventing a new feature or waiting for REL-11.

**Work:** enter the responsible child/leaf stage with actual prerequisites. Reference recovery precedes dependent quality claims; shared binding/native/content defects precede their affected consumer. Implement only the necessary delta. Each repair deletes its replaced code/state/selector/build/dependency path and proves retained behavior in the same slice. Narrowing a required promise needs its scope-owner decision first.

**Exit:** dispositions are reconciled for the approved release promises, and each admitted Included/Experimental claim has the exact evidence required for REL-04. A family inventory is not a module-completeness checklist; apply the [minimum supporting scope](../../../Acceptance/FirstRelease.md#minimum-supporting-scope). Reuse shared artifacts by identity; unselected future tiers need no implementation. Missing numerical/native/failure evidence remains blocked; a successful package or average readiness cannot replace it.

**Next FR-03 item:** [current Sponza radiance recovery](../../Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ImplementationPlan.md#required-execution-order): freeze one approved camera/settings/source candidate, observe Lit and admitted Reference raw/display products, then select only the first wrong rendering/input owner. [F05-C repairs the shader identity defect](../../../Acceptance/FeatureCompletionReports.md#reference-correctness-progress); completed instructions are retired. The separate F01 analytic-emitter repair and full identity/native/API/frontend/lifecycle/quality gates remain open before dependent FR-04 oracle claims. Accepted PTD-00 and product-closeout obligations are unchanged.

```text
Execute only the next Roadmap-selected FR-03 delta from Docs/Architecture/CrossModule/FirstRelease/ImplementationPlan.md. Name the selected path and consumer, then choose proof, repair, bounded simplification/retirement or admitted feature and run its owning child/leaf stage. For a cut, name the unnecessary existing surface and prove the retained path; no replacement feature is required. Complete local deletion and preservation checks. Reuse valid reports; do not execute the whole family registry or optional tiers. Apply the owning execution contract and its Stage Source-Style Gate.
Before handoff, update the stage result, remaining plan and next item.

```

### `FR-04` — Release Maps

**Work:** qualify the approved seven-map set plus built-in Empty through source/cook/runtime/product identity, material losses, raw reference domain/uncertainty and temporal/artifact review. Use the applicable MAP-A through MAP-H method and per-map release contract on required APIs. The later Bistro/San Miguel, neural WL and portfolio CASE completion gates are not additional release maps or prerequisites.

Use the same map proof for source/cooked fidelity: reached geometry/tangents/axes/units, material/texture dependencies, camera/light and any selected skin/morph/animation semantics. Reuse compatible cook/loader controls; run missing decoder, failure, determinism, budget and serial/worker cells only for admitted inputs or changed shared producers. [Unused combinations are excluded from release proof](../../../Acceptance/ReleaseScope/CapabilityDispositions.md#portfolio-demo-applicability), without a general-importer or whole-family pass. Do not run a separate foundation format matrix before this stage. Package allowlist/source-debug erasure and exact notices remain FR-06/07.

**Cleanup:** repair the actual content/transport/output owner and remove the replaced faulty path; do not add map-specific renderer branches, substitute content silently or expand the reflection oracle domain to excuse unsupported effects.

**Exit:** REL-05 has exact candidate/settings/pose/camera/reference identities, original artifacts, defects and invalidation triggers for each selected map. A map's load/readiness or attractive final screenshot cannot pass its material/raw/temporal checks.

**Remaining / next:** obtain the qualified current raw-reference candidate through FR-03 before dependent map comparisons. The F05 preflight supplies no REL-05 map pass. Keep the approved seven maps plus Empty; select one applicable map proof when its prerequisites are met, remove completed work from this plan and retain results in REL-05.

Source/cooked input checks needed by the current rendering result can run before raw-oracle qualification. Reference comparisons wait for the qualified oracle; their exact map artifacts can then satisfy Reference Stage 10 before formal REL-05 acceptance. Reuse those artifacts rather than making feature closure and map closure wait for each other's verdict.

```text
Execute only the selected FR-04 map proof from Docs/Architecture/CrossModule/FirstRelease/ImplementationPlan.md. Use the approved release maps and their applicable checks, not the later Tier-1/portfolio ladder. Freeze raw/reference/settings identity, repair the first wrong owner with local cleanup and record exact results for REL-05. Apply the owning execution contract and its Stage Source-Style Gate.
Before handoff, update the stage result, remaining plan and next item.

```

### `FR-05` — Performance, Stability, And Native Proof

Use original frame/native/memory evidence and existing tools. This stage closes the fixed release protocol after scoped per-feature cost checks; it does not implement an internal profiler or repeat unchanged feature proofs. Remove disposable probes before handoff. A measured defect returns to its production owner for a bounded repair and same-slice deletion, then invalidates only affected candidate proof; no permanent benchmark service or fault API is added here.

**Goal:** prove declared budgets, failure/recovery behavior, soaks, and native D3D12/Vulkan correctness for the same candidate.

**Non-goals:** installing an internal profiler product, presenting average FPS alone, or ignoring unsupported matrices.

**Failure modes:** observer changes result; no cold/steady distinction; validation warning dismissed; device loss called recovered; memory high-water omitted; backend divergence hidden.

**Phase exit criteria:** `REL-06`/`REL-07` pass with causal measurements, native diagnostics, controlled failure evidence, and exact candidate identity. Actual production additions and deletions satisfy the Phase Card Contract; temporary validation code and registrations are gone.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only FR-05 from Docs/Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-05--performance-stability-and-native-proof. Apply the owning execution contract and its Stage Source-Style Gate.
Run the REL-06/07 budget, stability, recovery and paired-native checks for one candidate and declared laptop benchmark. Separate cold and steady results, observer effects and unsupported rows. Retain raw timings, memory and validation findings.
Check named prerequisites before editing. Use the stage exit criteria; record exact candidate/configuration, commands and results. Stop on unresolved contracts or unavailable required checks.
Before handoff, update the stage result, remaining plan and next item.

```

</details>

### `FR-06` — Candidate Freeze And Reproduction

**Goal:** supply one freshly built precompiled/cooked release base, prove Launcher prebuilt Run, and prove optional source modification/rebuild starting from that matching base or from source alone.

**Non-goals:** fixing the candidate in place, giving undocumented assistance, or hiding repairs or machine-specific prerequisites.

**Failure modes:** hash drift; cached prerequisite; admin/environment dependency; private path; documented run/recovery instructions fail; a fix changes the candidate without affected revalidation.

**Phase exit criteria:** `REL-08` and `REL-09` pass; any fix creates a new candidate and invalidates affected evidence.

The shared unsigned archive/rehearsal supplies package-bound feature inputs before this final gate. Prepare it when its actual content/build prerequisites exist, then freeze/reconcile its applicable results here after feature/map/native gates. This is one evidence journey, without an FR-03 -> FR-06 -> FR-03 approval dependency.

#### Prebuilt Delivery Simplification

**Approved target:** publisher prepares one ready-to-run bundle; ordinary use is extract -> Launcher -> scene -> Run, with no SDK/source/manual assembly/build/cook step. Game has [no ImGui UI](../../../Acceptance/FirstRelease.md#consumer-effort-and-game-presentation); Editor UI remains. Apply the [package/source-rebuild contract](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#prebuilt-base-and-source-rebuild). This is remaining work, not current acceptance. Preserve Launcher's selection/previews/controls, Build/Cook/Run, progress/cancel and failures. Use these existing owners; add no release manager, local-file version tracker or second content/build path.

- [x] **Thin source implementation:** package prepared outputs through the existing owner with one final inventory; remove pre-build admissions/build-cache snapshots and the duplicate build/cook runner. `SyncProducts.ps1` fills the canonical binaries/cooked files local Build/Cook overwrite. Shared installed stores, seed/recook origin checks, runtime manifest-member scans and the Launcher prerequisite graph are deleted. Run is direct; explicit developer tools keep their existing UX.
- [x] **Render-only Game source:** remove the Game-only ImGui host/console, flags and build hooks. Window title is independent of UI; Editor keeps its UI packets/context/input and editing/Reference path. This is source/build proof, not observed Game/Editor runtime acceptance.
- [x] **Finite diagnostic source:** delete the custom release-review script and generic ZIP-dependent export path. Keep logs, manual recovery/known issues and existing source GPU-crash preview/consent/symbol tools.
- [x] **Launcher-owned preparation:** remove Core package discovery/parsing/installed mode and release-version state, move build-output grammar to tool support, and retain the ordinary `artifacts/dev` tree in the archive. Editor no longer owns compiler execution, watches sources or parses recook receipts. Its optional Recompile Shaders action delegates to Launcher through a tooling binding, while Application retains operation lifetime and reloads only on success; existing runtime Reload remains. [ITER-RELEASE-SLIM-03](../../../../artifacts/validation/launcher-owned-delivery-20261101/completion.json) records the source/isolated checks; no rendering pipeline or generic publication guard changed.
- [x] **Runtime admission reconciliation:** [ITER-RELEASE-SLIM-04](../../../../artifacts/validation/runtime-inputs-slim-20261101/completion.json) removes the remaining runtime selected/raw-source gate, moves acquisition/catalog policy from Core to ToolSupport, and removes the unused Editor shader cancellation-file protocol. Runtime discovers ordinary level documents; optional acquisition validates its own inputs. Native registry/session controls cover the previous preflight/runtime mismatch; archive/frame qualification stays below.
- [ ] **One affected delivery rehearsal:** assemble a new exact archive/symbol companion after its named prerequisites. Exercise offline/relocated/read-only Run without source assets, SDKs or old content; Game frame/camera/focus/native close and Editor preservation; missing required CRT/content, interrupted cook, denied output/retry, and manual re-extraction. Import the matching base into a source checkout, edit a real code/asset/shader input, Build/Cook through Launcher and observe the replacement in the same paths. Retain fresh source-only proof or reuse unchanged cells with explicit applicability. Rights admission for bundled maps remains required; no blanket unrelated source/rendering replay.

**Prior source result, 2026-11-01:** [ITER-RELEASE-SLIM-03](../../../../artifacts/validation/launcher-owned-delivery-20261101/completion.json) records Launcher-owned preparation, owner builds, source style and bounded native/sync controls. The [earlier cleanup](../../../../artifacts/validation/release-owner-slim-20261101/completion.json) retains its source identity; its flattened extraction layout is superseded. These are no new-archive or visible-frame passes. Temporary validation-folder cleanup is blocked by automatic approval review; ignored fixtures remain, with production scaffolding removed. **Next supporting item:** one affected delivery rehearsal after its named content/rendering prerequisites. Global rendering work remains FR-03 Sponza Lit/Reference recovery. Historical archive results, the removal exclusion and missing-`msvcp140_2.dll` regression oracle keep their identities.

**Candidate:** recheck the FR-01 `release-scope.md` export against this candidate's membership/selectors, reopening only invalidated disposition cells. Documented reproduction may use the unsigned internal archive. Windows signing is deferred to FR-07; changed bytes require renewed identity and affected checks before publication.

Join admitted foundation/content/shader results here through one shared package-relative content-to-frame journey and source-build rehearsal, following the [shared delivery evidence rule](../../../Acceptance/FirstRelease.md#shared-delivery-evidence). Feature closure contributes its affected controls to these runs; it does not commission another archive or adoption campaign per report. Replace the archive containing `ISSUE-3` and qualify its actual source, symbols and affected product checks; reuse unchanged cells by explicit applicability. This replaces the separate foundation aggregate stage, not the missing proof. A source edit after a recorded repair pass needs its own affected checks before archive qualification.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only FR-06 from Docs/Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-06--candidate-freeze-and-reproduction. Apply the owning execution contract and its Stage Source-Style Gate.
The source simplification and Launcher-owned preparation are complete. Use the workspace-shaped archive and explicit Launcher Build/Cook, with Editor reload of existing outputs; do not restore Engine package discovery or compiler execution. Execute only the affected delivery rehearsal in Prebuilt Delivery Simplification through existing tools; preserve Launcher UX and record Stage Source-Style applicability. Do not restore a store, prerequisite scheduler, release runner or provenance-dependent consumer path. Final freeze waits for its named prerequisites. Then qualify one candidate for REL-08/09: prebuilt Run without source/old content and optional matching-base import -> real source edit -> Launcher Build/Cook/Run, plus source-only fresh build proof. Join applicable foundation/content/shader reports here; no separate foundation closure journey. Reuse existing manifests and operation reports; add no adoption runner or reporting authority. A required repair returns to its owner with same-slice cleanup and invalidates dependent proof if bytes or configuration change.
Check named prerequisites before editing. Use the stage exit criteria; record exact candidate/configuration, commands and results. Stop on unresolved contracts or unavailable required checks.
Before handoff, update the stage result, remaining plan and next item.

```

</details>

### `FR-07` — Publish And Verify

**Goal:** publish exactly the approved portable rendering demo and verify downloaded bytes, public identity, instructions and truthful self-service limits through the existing package/signing operations.

**Non-goals:** last-minute feature work, an installer/updater/publication service, or replacing an approved artifact after publication. Reuse current signing/manifest/package operations; remove obsolete staging inputs and temporary publication probes while retaining final bytes and provenance.

**Failure modes:** tag/archive mismatch; wrong hashes; missing notices; unavailable download; public docs describe excluded behavior; public instructions invent a support service or omit self-service recovery.

**Phase exit criteria:** `REL-10` passes against downloaded artifacts, not local staging outputs.

**Final publication prerequisites:** obtain the Windows publisher certificate, sign/timestamp the admitted PE files, regenerate manifests and archive hashes, and verify the signed bundle. Resolve public redistribution obligations here. Repeat affected integrity, trust and consumer checks against the final bytes before uploading; unsigned internal adoption is not signed-release proof.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only FR-07 from Docs/Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-07--publish-and-verify. Apply the owning execution contract and its Stage Source-Style Gate.
Prepare the already approved candidate for publication under REL-10. Verify signatures, immutable downloads, public identity and truthful self-service policy. Perform authorized publication only, then verify the downloaded bytes against the frozen archive.
Check named prerequisites before editing. Use the stage exit criteria; record exact candidate/configuration, commands and results. Stop on unresolved contracts or unavailable required checks.
Before handoff, update the stage result, remaining plan and next item.

```

</details>

### `FR-08` — Stabilize And Close

**Goal:** perform finite maintainer release review, disposition known candidate defects and close the release. No ongoing user support or response window is offered.

**Non-goals:** feature expansion, invisible archive replacement, or declaring success because no telemetry was collected.

**Failure modes:** known critical blocker omitted; replacement lacks provenance; defect cannot be reproduced; public bytes differ without a new version; false support promise.

**Phase exit criteria:** `REL-11` passes with a bounded release-review ledger, any required replacement/withdrawal records, no unresolved release blocker and a final accepted release report. Earlier stage cleanup is verified; no dormant next-program code, support service or completed implementation instructions remain in the active delivery route.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only FR-08 from Docs/Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-08--stabilize-and-close. Apply the owning execution contract and its Stage Source-Style Gate.
Perform one finite maintainer review under REL-11. Disposition known defects and reconcile invalidated evidence; reuse compatible replacement/withdrawal controls and execute affected checks if an actual replacement is required. Record the closeout decision before admitting the next program; promise no support service.
Check named prerequisites before editing. Use the stage exit criteria; record exact candidate/configuration, commands and results. Stop on unresolved contracts or unavailable required checks.
Before handoff, update the stage result, remaining plan and next item.

```

</details>

## Global Stop Conditions

Stop the current iteration when any of these is true:

- the required Architecture behavior or `AC/FM/CHK` is missing, contradictory, or no longer matches source;
- the change needs a product, rights, support, compatibility, or architecture decision outside its authority;
- an implementation would create a second mutable owner, semantic path, selector truth, or evidence truth;
- a required check is unavailable, inconclusive, or bound to a different candidate;
- the only route to passing is a silent fallback, map-specific workaround, compatibility layer, broad mutex, retry loop, or suppressed native diagnostic;
- a required changed reference-domain decision is unresolved; accepted PTD-00 and delivered source work are preserved rather than reopened by default;
- the active candidate, package manifest, release map set, provider version, backend, or configuration changes without evidence invalidation.

## Explicitly Unscheduled Capabilities

The first release does not automatically schedule geometry-cache animation, neural graphics, volumetric lighting, frame generation, native Linux, a binary SDK, an installer, a full CI/regression product beyond the approved deterministic release gates, or an internal performance-dashboard product. Linux is excluded from the roadmap. The other Architecture dossiers retain their negative/target contracts. Deferred decals, color grading, chromatic aberration, and HDR10 output are explicitly scheduled release blockers through `FCR-REN-23` through `FCR-REN-26`; their acceptance remains open until the required implementation and evidence pass. Admit any other capability only by changing scope, adding/updating its FCR and feature-local acceptance, re-evaluating dependencies and risks, and invalidating affected release evidence.
## Phase Card Contract

Every stage and copied prompt follows [stage completion and resume](../../../Engineering/Workflow/ChangeLifecycle.md#stage-completion-and-resume). The [state update](../../../Engineering/Workflow/ChangeLifecycle.md#update-stage-state-before-handoff) is part of its exit: record the result, reconcile remaining work and name the next item before handoff. Later aggregate proof remains separate.

Every child stage and copied prompt uses the same delivery rule: identify the selected promise and actual consumer, reuse compatible code/evidence, then close only its missing production/proof delta or a bounded simplification preserving that path. A deletion slice can be complete with no replacement capability. Global priority comes from Roadmap; local prerequisites come from the owning contract, not a phase-number chain. [Portfolio focus](../../../Strategy/Roadmap.md#portfolio-focus-and-existing-code-simplification) protects rendering depth and necessary support while minimizing general-engine scope; Launcher retains its explicit presentation/reproduction role.

For supporting modules, apply the [minimum supporting scope and release workflow](../../../Acceptance/FirstRelease.md#minimum-supporting-scope) before selecting a task: name the exact demo/reviewer action, the broken invariant or unnecessary surface, and the smallest retained-path check. Default to the existing owner and zero new service/framework/persistent workflow state. A justified local guard or lifetime contract can remain necessary; a new recovery/validation platform or broader inventory closure is not a substitute. When that bounded path passes, hand control back to its rendering consumer.

Follow [Change Lifecycle](../../../Engineering/Workflow/ChangeLifecycle.md), including its [stage complexity budget](../../../Engineering/Workflow/ChangeLifecycle.md#define-the-outcome-and-complexity-budget), [scope/retirement](../../../Engineering/Workflow/ChangeLifecycle.md#scope-and-retirement-decision), responsibility refinement and the [source-style gate](../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate). Each stage names the touched existing path to retain, narrow, replace or retire; proposed state/API/hooks/copies/dependencies require a current consumer and a reason existing representations cannot serve it. Its work and exit compare that budget to the actual diff, delete replaced code/state/selectors/build/dependencies, and include a positive retained-path check and a stale-reachability check; justify zero deletion instead of inventing churn. Unjustified additions or postponed cleanup leave that stage open. A final cleanup stage verifies these results and cannot defer them.

Use owning AC/FM/checks, exact candidate/API/profile/content identities and the cheapest falsifying control. Check IDs do not authorize a permanent fixture, runner, report generator or fault-injection API. Apply [shared delivery evidence](../../../Acceptance/FirstRelease.md#shared-delivery-evidence): one candidate qualification serves its applicable feature reports; each leaf supplies only the missing feature-specific observation. Preserve useful unique evidence. Required unavailable proof stays blocked. No broad build/cook/map sweep occurs merely to gain confidence.

## Complete Feature Routing

Use this lookup only after selecting a promised result from Roadmap. Each family has one owner; a row does not schedule a separate implementation, audit, onboarding run or report platform. Product/source/native/capture observations can serve multiple applicable reports when their inputs and oracles match. Unused combinations keep their explicit scope disposition rather than generating more engine work.

### Feature Work-Package Registry

This is the exact one-to-one execution assignment. The phase owns implementation order; the linked Architecture owner still owns behavior and acceptance.

<details>
<summary>Expand feature work-package registry</summary>

Product and foundation rows link to enduring owners after plan retirement. Enter a source owner only for a selected consumer's failed check or bounded simplification. The known `FCR-PROD-06` publication blocker needs affected current-source proof and repaired-archive reconciliation under FR-06; selected content proof follows FR-04. Neither is completed acceptance.

| FCR | Primary phase | FCR | Primary phase |
| --- | --- | --- | --- |
| `FCR-PROD-01` | [Game contract](../../Modules/Projects/Showcase/ShowcaseCapabilities.md#fcr-prod-01-game-consumer-contract) | `FCR-PROD-02` | [Source contract](../../Modules/BuildAndPackaging/BuildAndPackagingCapabilities.md#fcr-prod-02-source-adopter-contract) |
| `FCR-PROD-03` | [Launcher contract](../../Modules/Tools/Launcher/LauncherCapabilities.md#fcr-prod-03-launcher-contract) | `FCR-PROD-04` | [Editor contract](../../Modules/Engine/Editor/EditorCapabilities.md#fcr-prod-04-editor-contract) |
| `FCR-PROD-05` | [Package contract](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#fcr-prod-05-package-contract) | `FCR-PROD-06` | [Self-service contract](../../Modules/BuildAndPackaging/PackagingAndInstallation.md#fcr-prod-06-support-and-incident-contract) |
| `FCR-CORE-01` | [Core](../../Modules/Engine/Core/CoreCapabilities.md) | `FCR-PLAT-01` | [Platform](../../Modules/Engine/Platform/PlatformCapabilities.md) |
| `FCR-TASK-01` | [Tasks](../../Modules/Engine/Tasks/TasksCapabilities.md) | `FCR-WORLD-01` | [GameFramework](../../Modules/Engine/GameFramework/GameFrameworkCapabilities.md) |
| `FCR-WORLD-02` | [GameFramework](../../Modules/Engine/GameFramework/GameFrameworkCapabilities.md) | `FCR-WORLD-03` | [GameFramework](../../Modules/Engine/GameFramework/GameFrameworkCapabilities.md) |
| `FCR-CONT-01` | [Selected-map proof](#fr-04--release-maps) | `FCR-CONT-02` | [Selected-map proof](#fr-04--release-maps) |
| `FCR-CONT-03` | [Selected-map proof](#fr-04--release-maps) | `FCR-SHDR-01` | [Shader System](../ShaderSystem/ImplementationPlan.md) |
| `FCR-TOOL-01` | [ToolSupport](../../Modules/Tools/ToolSupport/ToolSupportCapabilities.md) | `FCR-RHI-01` | [`RHI-1`](../../Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-1--device-resources-descriptors-and-lifetime) |
| `FCR-RHI-02` | [`RHI-2`](../../Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-2--command-recording-synchronization-resize-and-present) | `FCR-RHI-03` | [`RHI-3`](../../Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-3--paired-backend-lowering) |
| `FCR-RHI-04` | [`RHI-4`](../../Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-4--ray-tracing-and-acceleration-structure-execution) | `FCR-RHI-05` | [`RHI-5`](../../Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-5--diagnostics-capture-device-loss-and-recovery) |
| `FCR-RHI-06` | [`RHI-5`](../../Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-5--diagnostics-capture-device-loss-and-recovery) | `FCR-REN-01` | [`FS-1`](../../Modules/Engine/Renderer/FirstRelease/FrameAndSceneImplementationPlan.md#fs-1--frame-admission-and-coordination) |
| `FCR-REN-02` | [`FS-2`](../../Modules/Engine/Renderer/FirstRelease/FrameAndSceneImplementationPlan.md#fs-2--scene-view-and-gpu-scene-preparation) | `FCR-REN-03` | [`FS-3`](../../Modules/Engine/Renderer/FirstRelease/FrameAndSceneImplementationPlan.md#fs-3--frame-graph-compile-and-execution) |
| `FCR-REN-04` | [`GR-2`](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-2--visibility-draw-preparation-and-raster-gbuffer) | `FCR-REN-05` | [`GR-4`](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-4--ray-traced-gbuffer) |
| `FCR-REN-06` | [`LGT-1`](../../Modules/Engine/Renderer/FirstRelease/LightingImplementationPlan.md#lgt-1--direct-lighting) | `FCR-REN-07` | [`LGT-2`](../../Modules/Engine/Renderer/FirstRelease/LightingImplementationPlan.md#lgt-2--indirect-lighting) |
| `FCR-REN-08` | [`LGT-3`](../../Modules/Engine/Renderer/FirstRelease/LightingImplementationPlan.md#lgt-3--preserve-discovery-and-close-the-required-reference), then the accepted [staged `PTD-01` plan](../../Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ImplementationPlan.md) | `FCR-REN-09` | [`DSP-2`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-2--exposure) |
| `FCR-REN-10` | [`DSP-3`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-3--image-reconstruction-and-provider-integration) | `FCR-REN-11` | [`RD-3`](../../Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md#rd-3--debug-views-diagnostics-products-and-capture) |
| `FCR-REN-12` | [`GR-3`](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-3--renderer-tlas-policy-and-publication) | `FCR-REN-13` | [`RD-4`](../../Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md#rd-4--ui-and-viewport-composition) |
| `FCR-REN-14` | [`DSP-4`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-4--tone-mapping-encoding-and-presentation) | `FCR-REN-15` | [`DSP-4`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-4--tone-mapping-encoding-and-presentation) |
| `FCR-REN-16` | [`RD-1`](../../Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md#rd-1--pipeline-materialization-and-typed-binding) | `FCR-REN-17` | [`GR-1`](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-1--mesh-and-texture-residency) |
| `FCR-REN-18` | [`DSP-1`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-1--temporal-sampling-history-resolution-and-aa-truth) | `FCR-REN-19` | [`RD-2`](../../Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md#rd-2--settings-state-and-persistence) |
| `FCR-REN-20` | [`RD-5`](../../Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md#rd-5--latency-coordination) | `FCR-REN-21` | [`GR-2`](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-2--visibility-draw-preparation-and-raster-gbuffer) |
| `FCR-REN-22` | [`DSP-1`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-1--temporal-sampling-history-resolution-and-aa-truth) | `FCR-REN-23` | [`GR-5`](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-5--deferred-gbuffer-decals) |
| `FCR-REN-24` | [`DSP-5`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-5--color-grading) | `FCR-REN-25` | [`DSP-6`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-6--chromatic-aberration-and-vignette) |
| `FCR-REN-26` | [`DSP-7`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-7--hdr10-display-output) | — | — |

</details>
