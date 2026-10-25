# First Release Implementation Plan

[Docs](../../../README.md) / [Up](../README.md)

Stages: [FR-00](#fr-00--scope-and-discovery) / [FR-01](#fr-01--identity-rights-and-baseline) / [FR-02](#fr-02--package-spine) / [FR-03](#fr-03--feature-closure) / [FR-04](#fr-04--release-maps) / [FR-05](#fr-05--performance-stability-and-native-proof) / [FR-06](#fr-06--candidate-freeze-and-reproduction) / [FR-07](#fr-07--publish-and-verify) / [FR-08](#fr-08--stabilize-and-close).

**Status:** open release plan; remaining work is tracked in the roadmap and linked stage plans

Start with [current progress](../../../Strategy/Roadmap.md#release-progress) for delivered work and missing checks. [Roadmap](../../../Strategy/Roadmap.md#where-we-are-and-what-is-next) selects the next task; this page supplies its phase brief.

[First Release Acceptance](../../../Acceptance/FirstRelease.md#release-gates) owns release verdicts. Feature-local Architecture checks and [candidate reports](../../../Acceptance/FeatureCompletionReports.md) own feature proof.

For product work, follow the [PD-1 through PD-4 exit checklist](ProductImplementationPlan.md#how-to-close-this-plan) and [recorded progress](../../../Acceptance/FeatureCompletionReports.md#product-delivery-progress). Its closure qualifies the admitted internal product scope, not all release gates. [First Release](../../../Acceptance/FirstRelease.md#product-plan-and-public-release) retains clean-environment/minimum-machine requirements and the FR-07 signing/public-rights boundary.

Choose the section for your current task: [release phases](#release-tasks), [Game/Editor/Launcher/package](ProductImplementationPlan.md#first-release-product-implementation-plan), [paths/tasks/world/cook/shaders](FoundationImplementationPlan.md#first-release-foundation-and-content-implementation-plan), or [feature owners](#complete-feature-routing).

## Release Tasks

### `FR-00` — Scope And Discovery

**Remaining work:** reconcile changed selectors and exact component/helper/content-store membership against the six already approved product choices. Keep seven examples plus Empty, shared approved defaults, display-selected resolution, required rendering and no-support policy. Preserve accepted PTD-00; scope discovery does not restart it.

**Exit:** REL-00 has finite Included/Experimental/Excluded/Removed dispositions and named owners/checks. A current inventory gap receives a disposition, not automatic implementation. Exact rights evidence stays with FR-01/final FR-07 review, and technical unknowns are not new user-choice requests.

```text
Execute only remaining FR-00 scope reconciliation from Docs/Architecture/CrossModule/FirstRelease/ImplementationPlan.md. Preserve approved decisions and accepted PTD-00. Close changed membership/selector dispositions and their check routes; do not reapprove settled scope or implement target-only features. Apply the owning execution contract and its Stage Source-Style Gate.
```

### `FR-01` — Identity, Rights, And Baseline

**Remaining work:** reuse existing identity, pins, notices and local baseline; qualify only changed inputs and missing exact-candidate cold/warm/offline source checks through PD-2. Bind source/dependency/content identity, redistribution decisions and provenance to actual payloads. Final public certificate/signature and unresolved public-rights closure remain FR-07 obligations.

**Cleanup:** remove obsolete input/discovery/pin authority in a replacing repair and regenerate disposable output. No second dependency manager, baseline runner or broad automation program is selected.

**Exit:** REL-01/02 reports distinguish validated internal source/baseline work from any outstanding public-rights/trust blocker. Dependency-ready unsigned development can collect evidence; it does not bypass ordered formal release approval.

```text
Execute only remaining FR-01 work from Docs/Architecture/CrossModule/FirstRelease/ImplementationPlan.md. Reuse identity/pin/notice and compatible baseline evidence; qualify changed inputs and missing PD-2 source checks. Retain exact rights/provenance blockers for final release review. Add no baseline framework or unrelated feature repairs. Apply the owning execution contract and its Stage Source-Style Gate.
```

### `FR-02` — Package Spine

**Remaining work:** use PD-3 and the existing CMake/Packaging manifest-owned route to regenerate the unsigned Launcher/Editor/Game archive after product repairs. Earlier archives may check packaging mechanics; only final bytes qualify product behavior. Verify complete admitted payload, per-user mutation boundary, failure-safe publication and exact manifests/hashes.

**Cleanup:** delete stale staged membership and replaced staging/path policy in that repair; no installer, updater, SDK or another packager.

**Exit:** FCR-PROD-05/REL-03 meet their internal archive contract. Clean-machine and source-adopter portions are linked once through PD-3/FR-06, not recreated as a second acceptance journey here. Public release still waits for FR-07 and all required gates.

```text
Execute only remaining FR-02 package work through PD-3 from Docs/Architecture/CrossModule/FirstRelease/ImplementationPlan.md. Regenerate and verify the current unsigned archive with existing tooling, close its exact missing checks and remove stale membership. Reuse shared installed/adopter proof; do not implement another package spine. Apply the owning execution contract and its Stage Source-Style Gate.
```

### `FR-03` — Feature Closure

**Selection:** use Roadmap's required-feature priority to choose one missing consumer result, failed check or [bounded simplification](../../../Strategy/Roadmap.md#portfolio-focus-and-existing-code-simplification). First decide whether the route needs proof, a local repair, narrowing/retirement of unnecessary existing surface, or a specifically admitted missing feature. The 49-family registry routes that decision; it does not authorize 49 audits or redesigns. Simplification preserves a named rendering/product/evidence path and may remove private dead code without inventing a new feature or waiting for REL-11.

**Work:** enter the responsible child/leaf stage with actual prerequisites. Reference recovery precedes dependent quality claims; shared binding/native/content defects precede their affected consumer. Implement only the necessary delta. Each repair deletes its replaced code/state/selector/build/dependency path and proves retained behavior in the same slice. Narrowing a required promise needs its scope-owner decision first.

**Exit:** all family dispositions are reconciled, and every Included/Experimental claim has the exact evidence required for REL-04. Reuse shared artifacts by identity; unselected future tiers need no implementation. Missing numerical/native/failure evidence remains blocked; a successful package or average readiness cannot replace it.

```text
Execute only the next Roadmap-selected FR-03 delta from Docs/Architecture/CrossModule/FirstRelease/ImplementationPlan.md. Name the selected path and consumer, then choose proof, repair, bounded simplification/retirement or admitted feature and run its owning child/leaf stage. For a cut, name the unnecessary existing surface and prove the retained path; no replacement feature is required. Complete local deletion and preservation checks. Reuse valid reports; do not execute the whole family registry or optional tiers. Apply the owning execution contract and its Stage Source-Style Gate.
```

### `FR-04` — Release Maps

**Work:** qualify the approved seven-map set plus built-in Empty through source/cook/runtime/product identity, material losses, raw reference domain/uncertainty and temporal/artifact review. Use the applicable MAP-A through MAP-H method and per-map release contract on required APIs. The later Bistro/San Miguel, neural WL and portfolio CASE completion gates are not additional release maps or prerequisites.

**Cleanup:** repair the actual content/transport/output owner and remove the replaced faulty path; do not add map-specific renderer branches, substitute content silently or expand the reflection oracle domain to excuse unsupported effects.

**Exit:** REL-05 has exact candidate/settings/pose/camera/reference identities, original artifacts, defects and invalidation triggers for each selected map. A map's load/readiness or attractive final screenshot cannot pass its material/raw/temporal checks.

```text
Execute only the selected FR-04 map proof from Docs/Architecture/CrossModule/FirstRelease/ImplementationPlan.md. Use the approved release maps and their applicable checks, not the later Tier-1/portfolio ladder. Freeze raw/reference/settings identity, repair the first wrong owner with local cleanup and record exact results for REL-05. Apply the owning execution contract and its Stage Source-Style Gate.
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
```

</details>

### `FR-06` — Candidate Freeze And Reproduction

**Goal:** freeze one byte-identical candidate and prove the packaged-product and source-build journeys by the author or agent.

**Non-goals:** fixing the candidate in place, giving undocumented assistance, or hiding repairs or machine-specific prerequisites.

**Failure modes:** hash drift; cached prerequisite; admin/environment dependency; private path; support instructions fail; issue causes an untracked rebuild.

**Phase exit criteria:** `REL-08` and `REL-09` pass; any fix creates a new candidate and invalidates affected evidence.

**Candidate:** documented reproduction may use the unsigned internal archive. Windows signing is deferred to FR-07; changed bytes require renewed identity and affected checks before publication.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only FR-06 from Docs/Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-06--candidate-freeze-and-reproduction. Apply the owning execution contract and its Stage Source-Style Gate.
Freeze one package identity after prerequisite gates pass. Run the documented documented consumer and source-adopter journeys for REL-08/09 without private repair or author caches. Reuse existing manifests and operation reports; add no adoption runner or reporting authority. A required repair returns to its owner with same-slice cleanup and invalidates dependent proof if bytes or configuration change.
Check named prerequisites before editing. Use the stage exit criteria; record exact candidate/configuration, commands and results. Stop on unresolved contracts or unavailable required checks.
```

</details>

### `FR-07` — Publish And Verify

**Goal:** publish exactly the approved candidate and verify its public identity, downloads, documentation, and truthful no-support/self-service entry points.

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
Perform the finite maintainer review under REL-11: disposition known defects, rehearse required replacement/withdrawal decisions and reconcile invalidated evidence. Record the closeout decision before admitting the next program; promise no support service.
Check named prerequisites before editing. Use the stage exit criteria; record exact candidate/configuration, commands and results. Stop on unresolved contracts or unavailable required checks.
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

Every child stage and copied prompt uses the same delivery rule: identify the selected promise and actual consumer, reuse compatible code/evidence, then close only its missing production/proof delta or a bounded simplification preserving that path. A deletion slice can be complete with no replacement capability. Global priority comes from Roadmap; local prerequisites come from the owning contract, not a phase-number chain. [Portfolio focus](../../../Strategy/Roadmap.md#portfolio-focus-and-existing-code-simplification) protects rendering depth and necessary support while minimizing general-engine scope; Launcher retains its explicit presentation/reproduction role.

Follow [Change Lifecycle](../../../Engineering/Workflow/ChangeLifecycle.md), including its [stage complexity budget](../../../Engineering/Workflow/ChangeLifecycle.md#define-the-outcome-and-complexity-budget), [scope/retirement](../../../Engineering/Workflow/ChangeLifecycle.md#scope-and-retirement-decision), responsibility refinement and the [source-style gate](../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate). Each stage names the touched existing path to retain, narrow, replace or retire; proposed state/API/hooks/copies/dependencies require a current consumer and a reason existing representations cannot serve it. Its work and exit compare that budget to the actual diff, delete replaced code/state/selectors/build/dependencies, and include a positive retained-path check and a stale-reachability check; justify zero deletion instead of inventing churn. Unjustified additions or postponed cleanup leave that stage open. A final cleanup stage verifies these results and cannot defer them.

Use owning AC/FM/checks, exact candidate/API/profile/content identities and the cheapest falsifying control. Check IDs do not authorize a permanent fixture, runner, report generator or fault-injection API. Preserve useful unique evidence; reuse shared package/adoption artifacts rather than run a new journey per family. Required unavailable proof stays blocked. No broad build/cook/map sweep occurs merely to gain confidence.

## Complete Feature Routing

Every current `FCR-*` family has exactly one primary execution owner:

| Plan | Primary families | Count |
| --- | --- | ---: |
| [Product And Delivery](ProductImplementationPlan.md#first-release-product-implementation-plan) | `FCR-PROD-01`–`06` | 6 |
| [Foundation, World, And Content](FoundationImplementationPlan.md#first-release-foundation-and-content-implementation-plan) | `FCR-CORE-01`, `FCR-PLAT-01`, `FCR-TASK-01`, `FCR-WORLD-01`–`03`, `FCR-CONT-01`–`03`, `FCR-SHDR-01`, `FCR-TOOL-01` | 11 |
| [RHI And GPU Execution](../../Modules/Engine/RHI/FirstReleaseImplementationPlan.md) | `FCR-RHI-01`–`06` | 6 |
| [Renderer: Frame And Scene](../../Modules/Engine/Renderer/FirstRelease/FrameAndSceneImplementationPlan.md) | `FCR-REN-01`–`03` | 3 |
| [Renderer: Geometry And Ray Tracing](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md) | `FCR-REN-04`, `05`, `12`, `17`, `21`, `23` | 6 |
| [Renderer: Lighting](../../Modules/Engine/Renderer/FirstRelease/LightingImplementationPlan.md) | `FCR-REN-06`–`08` | 3 |
| [Renderer: Display And Reconstruction](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md) | `FCR-REN-09`, `10`, `14`, `15`, `18`, `22`, `24`–`26` | 9 |
| [Renderer: Runtime And Diagnostics](../../Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md) | `FCR-REN-11`, `13`, `16`, `19`, `20` | 5 |
| **Total** | **all current families** | **49** |

Cross-plan references are dependencies, not duplicate ownership. If the FCR registry changes, update this table and exactly one child owner in the same change.

### Feature Work-Package Registry

This is the exact one-to-one execution assignment. The phase owns implementation order; the linked Architecture owner still owns behavior and acceptance.

<details>
<summary>Expand feature work-package registry</summary>

| FCR | Primary phase | FCR | Primary phase |
| --- | --- | --- | --- |
| `FCR-PROD-01` | [`PD-1`](ProductImplementationPlan.md#pd-1--game-and-editor-product-shells) | `FCR-PROD-02` | [`PD-2`](ProductImplementationPlan.md#pd-2--source-build-cook-and-launcher) |
| `FCR-PROD-03` | [`PD-2`](ProductImplementationPlan.md#pd-2--source-build-cook-and-launcher) | `FCR-PROD-04` | [`PD-1`](ProductImplementationPlan.md#pd-1--game-and-editor-product-shells) |
| `FCR-PROD-05` | [`PD-3`](ProductImplementationPlan.md#pd-3--package-and-installation) | `FCR-PROD-06` | [`PD-4`](ProductImplementationPlan.md#pd-4--support-incident-response-and-product-closure) |
| `FCR-CORE-01` | [`FWC-1`](FoundationImplementationPlan.md#fwc-1--core-and-windows-platform) | `FCR-PLAT-01` | [`FWC-1`](FoundationImplementationPlan.md#fwc-1--core-and-windows-platform) |
| `FCR-TASK-01` | [`FWC-2`](FoundationImplementationPlan.md#fwc-2--task-runtime-and-cancellation) | `FCR-WORLD-01` | [`FWC-3`](FoundationImplementationPlan.md#fwc-3--level-world-and-render-publication) |
| `FCR-WORLD-02` | [`FWC-3`](FoundationImplementationPlan.md#fwc-3--level-world-and-render-publication) | `FCR-WORLD-03` | [`FWC-3`](FoundationImplementationPlan.md#fwc-3--level-world-and-render-publication) |
| `FCR-CONT-01` | [`FWC-4`](FoundationImplementationPlan.md#fwc-4--import-cook-and-engine-assets) | `FCR-CONT-02` | [`FWC-4`](FoundationImplementationPlan.md#fwc-4--import-cook-and-engine-assets) |
| `FCR-CONT-03` | [`FWC-4`](FoundationImplementationPlan.md#fwc-4--import-cook-and-engine-assets) | `FCR-SHDR-01` | [`FWC-5`](FoundationImplementationPlan.md#fwc-5--shader-route-and-shared-tool-output) |
| `FCR-TOOL-01` | [`FWC-5`](FoundationImplementationPlan.md#fwc-5--shader-route-and-shared-tool-output) | `FCR-RHI-01` | [`RHI-1`](../../Modules/Engine/RHI/FirstReleaseImplementationPlan.md#rhi-1--device-resources-descriptors-and-lifetime) |
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
