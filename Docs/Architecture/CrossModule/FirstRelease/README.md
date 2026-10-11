# First Release Implementation Plan

**Status:** release implementation orchestrator; ordered work, not implementation evidence or release approval

Start with [current progress](../../../Strategy/Roadmap.md#release-progress) for delivered work and missing checks. [Roadmap](../../../Strategy/Roadmap.md#where-we-are-and-what-is-next) selects the next task; this page supplies its phase brief.

[First Release Acceptance](../../../Acceptance/FirstRelease.md#release-gates) owns release verdicts. Feature-local Architecture checks and [candidate reports](../../../Acceptance/FeatureCompletionReports.md) own feature proof.

## Release Tasks

### `FR-00` — Scope And Discovery

**Goal:** approve one reviewable release scope of included, experimental, excluded, and removed behavior. Preserve the already accepted `PTD-00` development discovery; do not restart it or treat it as accepted release-map oracle evidence.

**Current handoff:** the owner approved all [six scope decisions](../../../Acceptance/ReleaseScope/README.md#approval-and-next-work): three shipped products, seven selected maps, Launcher entry, user/display-selected resolution, approved defaults with chromatic aberration/vignette On, and no user support service. The laptop retains a fixed recorded 30 FPS acceptance benchmark. No product-choice answer remains pending; exact rights receipts and package/helper/configuration/content-store closure still block FR-00. Accepted PTD-00 development discovery is preserved.

**Non-goals:** implementation, optimistic scoring, adding target-only features, or treating the present reference path as an unbiased oracle.

**Failure modes:** unclassified selector; Architecture target mistaken for release scope; missing redistribution decision; `PTD-00` output incomplete; later work starts despite a blocked discovery gate.

**Phase exit criteria:** `REL-00` criteria are mapped to checks; every reachable option has a product classification; the release package contents are named; `PTD-00` records `PASS` or a blocking/re-scope decision.

### `FR-01` — Identity, Rights, And Baseline

**Goal:** make source, dependencies, assets, licenses, version identity, the initial build/cook/Game baseline, and approved fresh-checkout gate automation reproducible enough to start package work.

**Non-goals:** broad feature fixes, performance tuning, or publication.

**Failure modes:** mutable dependency; unidentified asset; dirty/generated state hidden; author-machine-only prerequisite; runtime result recorded without exact revision/configuration.

**Phase exit criteria:** `REL-01` and `REL-02` candidate records pass or name blockers; one baseline path has exact commands, versions, hashes, outputs, and cleanup.

### `FR-02` — Package Spine

**Goal:** complete `FCR-PROD-05` as one build → cook → stage → verify → package authority.

**Non-goals:** an installer, binary SDK, silent best-effort staging, or distributing authoring tools by accident.

**Failure modes:** source-tree mutation; stale stage contents; undeclared dependency; absolute/user path; manifest/archive mismatch; partial archive after failure; package works only with repository state.

**Phase exit criteria:** [Product phase `PD-3`](ProductAndDelivery.md#pd-3--package-and-installation) passes and `REL-03` has clean-machine runtime evidence for the exact archive.

### `FR-03` — Feature Closure

**Goal:** close every included FCR family against its Architecture-owned contract in dependency order.

**Non-goals:** averaging readiness, accepting source presence, restoring superseded designs, or adding excluded capabilities.

**Failure modes:** orphan FCR; missing feature-local criterion; backend/mode/content gap; silent fallback; stale evidence reused after a change; feature passes but packaged route fails.

**Phase exit criteria:** all 49 primary owners in [Complete Feature Routing](#complete-feature-routing) have a candidate-bound result and `REL-04` passes.

### `FR-04` — Release Maps

**Goal:** turn the frozen `ReleaseMapSet` into representative correctness, artifact, and product-journey evidence.

**Non-goals:** choosing flattering viewpoints, using one screenshot as correctness proof, or adding map-specific renderer hacks.

**Failure modes:** missing workload coverage; reference/candidate mismatch; debug/exposure state omitted; warmed-only capture; cherry-picked output; map fix bypasses asset/renderer owner.

**Phase exit criteria:** every applicable `MAP-*` and specialist case has exact candidate identity, settings, references, artifacts, verdict, and invalidation triggers; `REL-05` passes.

### `FR-05` — Performance, Stability, And Native Proof

**Goal:** prove declared budgets, failure/recovery behavior, soaks, and native D3D12/Vulkan correctness for the same candidate.

**Non-goals:** installing an internal profiler product, presenting average FPS alone, or ignoring unsupported matrices.

**Failure modes:** observer changes result; no cold/steady distinction; validation warning dismissed; device loss called recovered; memory high-water omitted; backend divergence hidden.

**Phase exit criteria:** `REL-06`/`REL-07` pass with causal measurements, native diagnostics, controlled failure evidence, and exact candidate identity.

### `FR-06` — Candidate Freeze And Independent Adoption

**Goal:** freeze one byte-identical candidate and prove both runtime-consumer and source-adopter journeys by non-authors.

**Non-goals:** fixing the candidate in place, giving undocumented assistance, or counting the author as independent adoption.

**Failure modes:** hash drift; cached prerequisite; admin/environment dependency; private path; support instructions fail; issue causes an untracked rebuild.

**Phase exit criteria:** `REL-08` and `REL-09` pass; any fix creates a new candidate and invalidates affected evidence.

**Pre-release prerequisite:** obtain the deferred Windows publisher certificate, sign/timestamp and verify the final bytes before freezing this candidate. Enrollment is scheduled here by the release owner (2026-10-11), not as an FR-01 prerequisite. Unsigned development archives may exercise FR-02 packaging; they cannot satisfy this signed consumer/adoption gate. See the [roadmap task](../../../Strategy/Roadmap.md#where-we-are-and-what-is-next).

### `FR-07` — Publish And Verify

**Goal:** publish exactly the approved candidate and verify its public identity, downloads, documentation, and truthful no-support/self-service entry points.

**Non-goals:** last-minute feature work or replacing an approved artifact after publication.

**Failure modes:** tag/archive mismatch; wrong hashes; missing notices; unavailable download; public docs describe excluded behavior; public instructions invent a support service or omit self-service recovery.

**Phase exit criteria:** `REL-10` passes against downloaded artifacts, not local staging outputs.

### `FR-08` — Stabilize And Close

**Goal:** perform finite maintainer release review, disposition known candidate defects and close the release. No ongoing user support or response window is offered.

**Non-goals:** feature expansion, invisible archive replacement, or declaring success because no telemetry was collected.

**Failure modes:** known critical blocker omitted; replacement lacks provenance; defect cannot be reproduced; public bytes differ without a new version; false support promise.

**Phase exit criteria:** `REL-11` passes with a bounded release-review ledger, any required replacement/withdrawal records, no unresolved release blocker and a final accepted release report.

## Global Stop Conditions

Stop the current iteration when any of these is true:

- the required Architecture behavior or `AC/FM/CHK` is missing, contradictory, or no longer matches source;
- the change needs a product, rights, support, compatibility, or architecture decision outside its authority;
- an implementation would create a second mutable owner, semantic path, selector truth, or evidence truth;
- a required check is unavailable, inconclusive, or bound to a different candidate;
- the only route to passing is a silent fallback, map-specific workaround, compatibility layer, broad mutex, retry loop, or suppressed native diagnostic;
- the Reference Path Tracer work would cross from `PTD-00` discovery into implementation before the gate passes;
- the active candidate, package manifest, release map set, provider version, backend, or configuration changes without evidence invalidation.

## Explicitly Unscheduled Capabilities

The first release does not automatically schedule geometry-cache animation, neural graphics, volumetric lighting, frame generation, native Linux, a binary SDK, an installer, a full CI/regression product beyond the approved deterministic release gates, or an internal performance-dashboard product. Linux is excluded from the roadmap. The other Architecture dossiers retain their negative/target contracts. Deferred decals, color grading, chromatic aberration, and HDR10 output are explicitly scheduled release blockers through `FCR-REN-23` through `FCR-REN-26`; their acceptance remains open until the required implementation and evidence pass. Admit any other capability only by changing scope, adding/updating its FCR and feature-local acceptance, re-evaluating dependencies and risks, and invalidating affected release evidence.
## Phase Card Contract

Follow [Change Lifecycle](../../../Engineering/Workflow/ChangeLifecycle.md) and the owning feature checks. Inspect current source, preserve supported behavior, extend existing owners, delete replaced paths and record exact results. An unavailable check remains blocked. Phase briefs do not authorize scope expansion or broad builds unrelated to the affected claim.

## Complete Feature Routing

Every current `FCR-*` family has exactly one primary execution owner:

| Plan | Primary families | Count |
| --- | --- | ---: |
| [Product And Delivery](ProductAndDelivery.md) | `FCR-PROD-01`–`06` | 6 |
| [Foundation, World, And Content](FoundationWorldAndContent.md) | `FCR-CORE-01`, `FCR-PLAT-01`, `FCR-TASK-01`, `FCR-WORLD-01`–`03`, `FCR-CONT-01`–`03`, `FCR-SHDR-01`, `FCR-TOOL-01` | 11 |
| [RHI And GPU Execution](../../Modules/Engine/RHI/FirstReleasePlan.md) | `FCR-RHI-01`–`06` | 6 |
| [Renderer: Frame And Scene](../../Modules/Engine/Renderer/FirstRelease/FrameAndScene.md) | `FCR-REN-01`–`03` | 3 |
| [Renderer: Geometry And Ray Tracing](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracing.md) | `FCR-REN-04`, `05`, `12`, `17`, `21`, `23` | 6 |
| [Renderer: Lighting](../../Modules/Engine/Renderer/FirstRelease/Lighting.md) | `FCR-REN-06`–`08` | 3 |
| [Renderer: Display And Reconstruction](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstruction.md) | `FCR-REN-09`, `10`, `14`, `15`, `18`, `22`, `24`–`26` | 9 |
| [Renderer: Runtime And Diagnostics](../../Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnostics.md) | `FCR-REN-11`, `13`, `16`, `19`, `20` | 5 |
| **Total** | **all current families** | **49** |

Cross-plan references are dependencies, not duplicate ownership. If the FCR registry changes, update this table and exactly one child owner in the same change.

### Feature Work-Package Registry

This is the exact one-to-one execution assignment. The phase owns implementation order; the linked Architecture owner still owns behavior and acceptance.

<details>
<summary>Expand feature work-package registry</summary>

| FCR | Primary phase | FCR | Primary phase |
| --- | --- | --- | --- |
| `FCR-PROD-01` | [`PD-1`](ProductAndDelivery.md#pd-1--game-and-editor-product-shells) | `FCR-PROD-02` | [`PD-2`](ProductAndDelivery.md#pd-2--source-build-cook-and-launcher) |
| `FCR-PROD-03` | [`PD-2`](ProductAndDelivery.md#pd-2--source-build-cook-and-launcher) | `FCR-PROD-04` | [`PD-1`](ProductAndDelivery.md#pd-1--game-and-editor-product-shells) |
| `FCR-PROD-05` | [`PD-3`](ProductAndDelivery.md#pd-3--package-and-installation) | `FCR-PROD-06` | [`PD-4`](ProductAndDelivery.md#pd-4--support-incident-response-and-product-closure) |
| `FCR-CORE-01` | [`FWC-1`](FoundationWorldAndContent.md#fwc-1--core-and-windows-platform) | `FCR-PLAT-01` | [`FWC-1`](FoundationWorldAndContent.md#fwc-1--core-and-windows-platform) |
| `FCR-TASK-01` | [`FWC-2`](FoundationWorldAndContent.md#fwc-2--task-runtime-and-cancellation) | `FCR-WORLD-01` | [`FWC-3`](FoundationWorldAndContent.md#fwc-3--level-world-and-render-publication) |
| `FCR-WORLD-02` | [`FWC-3`](FoundationWorldAndContent.md#fwc-3--level-world-and-render-publication) | `FCR-WORLD-03` | [`FWC-3`](FoundationWorldAndContent.md#fwc-3--level-world-and-render-publication) |
| `FCR-CONT-01` | [`FWC-4`](FoundationWorldAndContent.md#fwc-4--import-cook-and-engine-assets) | `FCR-CONT-02` | [`FWC-4`](FoundationWorldAndContent.md#fwc-4--import-cook-and-engine-assets) |
| `FCR-CONT-03` | [`FWC-4`](FoundationWorldAndContent.md#fwc-4--import-cook-and-engine-assets) | `FCR-SHDR-01` | [`FWC-5`](FoundationWorldAndContent.md#fwc-5--shader-route-and-shared-tool-output) |
| `FCR-TOOL-01` | [`FWC-5`](FoundationWorldAndContent.md#fwc-5--shader-route-and-shared-tool-output) | `FCR-RHI-01` | [`RHI-1`](../../Modules/Engine/RHI/FirstReleasePlan.md#rhi-1--device-resources-descriptors-and-lifetime) |
| `FCR-RHI-02` | [`RHI-2`](../../Modules/Engine/RHI/FirstReleasePlan.md#rhi-2--command-recording-synchronization-resize-and-present) | `FCR-RHI-03` | [`RHI-3`](../../Modules/Engine/RHI/FirstReleasePlan.md#rhi-3--paired-backend-lowering) |
| `FCR-RHI-04` | [`RHI-4`](../../Modules/Engine/RHI/FirstReleasePlan.md#rhi-4--ray-tracing-and-acceleration-structure-execution) | `FCR-RHI-05` | [`RHI-5`](../../Modules/Engine/RHI/FirstReleasePlan.md#rhi-5--diagnostics-capture-device-loss-and-recovery) |
| `FCR-RHI-06` | [`RHI-5`](../../Modules/Engine/RHI/FirstReleasePlan.md#rhi-5--diagnostics-capture-device-loss-and-recovery) | `FCR-REN-01` | [`FS-1`](../../Modules/Engine/Renderer/FirstRelease/FrameAndScene.md#fs-1--frame-admission-and-coordination) |
| `FCR-REN-02` | [`FS-2`](../../Modules/Engine/Renderer/FirstRelease/FrameAndScene.md#fs-2--scene-view-and-gpu-scene-preparation) | `FCR-REN-03` | [`FS-3`](../../Modules/Engine/Renderer/FirstRelease/FrameAndScene.md#fs-3--frame-graph-compile-and-execution) |
| `FCR-REN-04` | [`GR-2`](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracing.md#gr-2--visibility-draw-preparation-and-raster-gbuffer) | `FCR-REN-05` | [`GR-4`](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracing.md#gr-4--ray-traced-gbuffer) |
| `FCR-REN-06` | [`LGT-1`](../../Modules/Engine/Renderer/FirstRelease/Lighting.md#lgt-1--direct-lighting) | `FCR-REN-07` | [`LGT-2`](../../Modules/Engine/Renderer/FirstRelease/Lighting.md#lgt-2--indirect-lighting) |
| `FCR-REN-08` | [`LGT-3`](../../Modules/Engine/Renderer/FirstRelease/Lighting.md#lgt-3--preserve-discovery-and-close-the-required-reference), then the accepted [staged `PTD-01` plan](../../Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/Plan.md) | `FCR-REN-09` | [`DSP-2`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstruction.md#dsp-2--exposure) |
| `FCR-REN-10` | [`DSP-3`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstruction.md#dsp-3--image-reconstruction-and-provider-integration) | `FCR-REN-11` | [`RD-3`](../../Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnostics.md#rd-3--debug-views-diagnostics-products-and-capture) |
| `FCR-REN-12` | [`GR-3`](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracing.md#gr-3--renderer-tlas-policy-and-publication) | `FCR-REN-13` | [`RD-4`](../../Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnostics.md#rd-4--ui-and-viewport-composition) |
| `FCR-REN-14` | [`DSP-4`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstruction.md#dsp-4--tone-mapping-encoding-and-presentation) | `FCR-REN-15` | [`DSP-4`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstruction.md#dsp-4--tone-mapping-encoding-and-presentation) |
| `FCR-REN-16` | [`RD-1`](../../Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnostics.md#rd-1--pipeline-materialization-and-typed-binding) | `FCR-REN-17` | [`GR-1`](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracing.md#gr-1--mesh-and-texture-residency) |
| `FCR-REN-18` | [`DSP-1`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstruction.md#dsp-1--temporal-sampling-history-resolution-and-aa-truth) | `FCR-REN-19` | [`RD-2`](../../Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnostics.md#rd-2--settings-state-and-persistence) |
| `FCR-REN-20` | [`RD-5`](../../Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnostics.md#rd-5--latency-coordination) | `FCR-REN-21` | [`GR-2`](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracing.md#gr-2--visibility-draw-preparation-and-raster-gbuffer) |
| `FCR-REN-22` | [`DSP-1`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstruction.md#dsp-1--temporal-sampling-history-resolution-and-aa-truth) | `FCR-REN-23` | [`GR-5`](../../Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracing.md#gr-5--deferred-gbuffer-decals) |
| `FCR-REN-24` | [`DSP-5`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstruction.md#dsp-5--color-grading) | `FCR-REN-25` | [`DSP-6`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstruction.md#dsp-6--chromatic-aberration-and-vignette) |
| `FCR-REN-26` | [`DSP-7`](../../Modules/Engine/Renderer/FirstRelease/DisplayAndReconstruction.md#dsp-7--hdr10-display-output) | — | — |

</details>
