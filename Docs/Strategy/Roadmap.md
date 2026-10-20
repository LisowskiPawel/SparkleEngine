# Implementation Roadmap

[Docs](../README.md) / [Up](README.md) / [Design targets](Requirements.md) / [Release implementation](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md)

**Status:** master roadmap; planning priorities, not completion results

**Scope:** the order to follow from the current release work through advanced graphics and portfolio delivery

**Updated:** 2026-10-20; progress summarizes recorded work, not release approval

Choose one task below, open its **implementation stage**, and copy that stage's prompt. The linked **design** explains the behavior and checks to preserve. This roadmap owns priority and brief progress; implementation plans own the steps.

## How To Execute A Stage

1. Open the stage linked under **Where We Are And What Is Next**.
2. Read its prerequisites and linked design. A blocked prerequisite stays blocked; completed source work is reused.
3. Expand **Copy implementation prompt**, paste it, and work only that stage.
4. Run its exit checks. Record the candidate result in Acceptance and update the short progress row here.
5. Continue to the next admitted stage in the plan. Source changes alone do not pass a release gate.

## Where We Are And What Is Next

**Next: [PD-1: finish Game/Editor journeys](../Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md#pd-1--game-and-editor-product-shells).** GPU crash tooling is qualified. Collect Modern Sponza with matched product/shader/symbol receipts, fix its owner and verify quit. Then regenerate the archive and continue PD-2/3/4. The fresh-repository rehearsal does not establish clean-OS or independent-adopter acceptance.

| Current work | Design / acceptance reference | Implementation stages |
| --- | --- | --- |
| Fix and verify the remaining product/source journeys | [Approved product scope](../Acceptance/FirstRelease.md#approved-release-scope) | [PD-1: Game/Editor](../Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md#pd-1--game-and-editor-product-shells), [PD-2: source/Launcher](../Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md#pd-2--source-build-cook-and-launcher) |
| Freeze source and regenerate the three-product archive | [Package design](../Architecture/Modules/BuildAndPackaging/PackagingAndInstallation.md#reviewed-package-inputs) | [PD-3](../Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md#pd-3--package-and-installation) |
| Verify diagnostics and close the product reports | [Self-service contract](../Architecture/Modules/BuildAndPackaging/PackagingAndInstallation.md) | [PD-4](../Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md#pd-4--support-incident-response-and-product-closure) |

**Renderer work:** [Lit/Reference recovery](../Architecture/Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ImplementationPlan.md#required-execution-order) retains its scope. Use the qualified crash tooling for GPU failures. Recheck dated failures against current source before fixing their owner.

**Final step before publication:** [FR-07](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-07--publish-and-verify) owns the deferred Windows certificate, signing/timestamping, signed-byte verification and public redistribution closure. These are outside PD-1 through PD-4.

## Release Progress

| Work | Implemented / recorded | Remaining |
| --- | --- | --- |
| Scope / FR-00 | Six product choices approved. | Remaining technical qualification; public rights close at FR-07. |
| Identity / FR-01 | Identity, dependency pins and notices implemented; earlier baseline recorded. | Qualify the final source candidate. |
| GPU crash tooling / GCD | Delivered: 18 native checks, 32 normal journeys, four SDK-free builds; `ITER-GCD-04-06`. [Scope and evidence](../Architecture/CrossModule/PerformanceDiagnostics/GpuCrashDebugging/GpuCrashDebuggingDesign.md#acceptance-and-controlled-failures). | None for the qualified tooling candidate; use in PD-1. |
| PD-1: Game / Editor | Product shells/shared settings implemented; seven-map archive cook passed. | Native crash/quit regression, controls/settings/failure and Editor edit/save checks. |
| PD-2: source / Launcher | Cold clone configured, synced and built products/tools; Empty cook passed. | Current cook/failure/GUI-equivalence checks and independent adoption. |
| PD-3 / FR-02: package | Unsigned archive `de0cf4bc…` verified; selected seven-map cook passed on this laptop. | Regenerate after fixes; final archive/offline/precompiled/Qt replacement and minimum-machine checks. |
| PD-4: diagnostics | Local GPU tooling qualified; earlier consent/privacy/review fixtures passed on `de0cf4bc…`. | Final-archive self-service, incident and replacement rehearsals. Automatic CPU crash/hang collection remains unavailable. |
| FR-03 to FR-08 | Plans retained. | Feature/map/performance/backend/adoption checks, publication and finite release review. |

Full product acceptance remains open; GCD's scoped pass does not close PD-1–4. Exact results stay under local `artifacts/validation/releases/`; the [report contract](../Acceptance/FeatureCompletionReports.md) binds them to candidate identities. [Release gates](../Acceptance/FirstRelease.md#release-gates) decide shipment.

## Ordered Delivery

The first release comes first. Existing reference recovery can proceed while its scope is completed; release/package claims still wait for their prerequisites.

```mermaid
flowchart LR
    Prepare[Scope, rights and package] --> Features[Required features]
    Features --> Prove[Frame proof and adoption]
    Prove --> Publish[Publish and stabilize]
    Publish --> Advanced[Advanced graphics]
```

| Step | What you deliver | Open the implementation stage |
| --- | --- | --- |
| 0. Define | A finite product, map set, feature scope and supported presets. | [Scope and discovery](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-00--scope-and-discovery) — `REL-00` |
| 1. Establish trust | Clear asset/dependency rights, product identity and a reproducible source build. | [Identity and baseline](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-01--identity-rights-and-baseline) — `REL-01/02` |
| 2. Package | One manifest-owned build, cook, stage and package route. | [Package spine](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-02--package-spine) — `REL-03` |
| 3. Finish features | Required rendering and product behavior, including failures and resource lifetime. Follow the feature order below. | [Feature closure](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-03--feature-closure) — `REL-04` |
| 4. Prove map quality | Correct content, trustworthy references and stable images on the selected maps. | [Release maps](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-04--release-maps) — `REL-05` |
| 5. Prove the whole frame | Realtime >=30 FPS, bounded memory, stability, D3D12/Vulkan parity and principal-level frame review. | [Performance and native proof](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-05--performance-stability-and-native-proof) — `REL-06/07` |
| 6. Obtain independent approval | Frozen package bytes and successful consumer/source journeys without private help. | [Candidate and adoption](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-06--candidate-freeze-and-independent-adoption) — `REL-08/09` |
| 7. Publish | The approved release, verified after download. | [Publication](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-07--publish-and-verify) — `REL-10` |
| 8. Stabilize | Finite maintainer closeout, resolved release blockers and approval to start the next program; no user support service. | [Release closeout](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-08--stabilize-and-close) — `REL-11` |
| 9. Build advanced evidence | Measured classical studies, an owned neural shader path and reproducible case studies. | [Advanced graphics below](#retained-advanced-graphics-roadmap) |
| 10. Expand selectively | A justified contribution or deeper transfer activity. | [Conditional work below](#conditional-and-excluded-work) |

Each plan states when its work may start and how it exits. The [release contract](../Acceptance/FirstRelease.md) owns the actual criteria, including the [required rendering and frame-review package](../Acceptance/FirstRelease.md#required-rendering-closure).

## Required Feature Closure Priority

Use this order within release step 3. A demonstrated dependency defect comes before the feature it blocks. Reuse completed work and valid proof; do not replay entire implementations.

| Order | Focus and intended result | Implementation stages |
| --- | --- | --- |
| 1 | **Reference correctness.** Restore usable lighting, reject invalid samples and qualify raw reference output before relying on it. | [Reference recovery](../Architecture/Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ImplementationPlan.md#required-execution-order), [reference closure](../Architecture/Modules/Engine/Renderer/FirstRelease/LightingImplementationPlan.md#lgt-3--preserve-discovery-and-close-the-required-reference) |
| 2 | **Blocking dependencies.** Fix proven content, shader, foundation or native-lifetime defects needed by the selected frame. | [Foundation/content](../Architecture/CrossModule/FirstRelease/FoundationImplementationPlan.md#first-release-foundation-and-content-implementation-plan), [RHI](../Architecture/Modules/Engine/RHI/FirstReleaseImplementationPlan.md), [Shader System](../Architecture/CrossModule/ShaderSystem/ImplementationPlan.md) |
| 3 | **Frame execution.** Reliable frame admission, scene/view publication, graph dependencies, barriers, aliasing and retirement. | [Frame and scene](../Architecture/Modules/Engine/Renderer/FirstRelease/FrameAndSceneImplementationPlan.md) — `FS-0–4` |
| 4 | **Geometry and materials.** Correct residency, raster/ray inputs, TLAS and material correspondence. | [Geometry and ray tracing](../Architecture/Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md) — `GR-0–4` |
| 5 | **Realtime transport.** Correct primary/secondary paths, direct/indirect lighting, temporal reuse and disocclusion behavior. | [Temporal/history](../Architecture/Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-1--temporal-sampling-history-resolution-and-aa-truth), [lighting](../Architecture/Modules/Engine/Renderer/FirstRelease/LightingImplementationPlan.md) — `DSP-1`, `LGT-0–2` |
| 6 | **Exposure and reconstruction.** Finish exposure, DLSS SR/RR inputs, resets and native lifetime on both APIs. | [Exposure](../Architecture/Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-2--exposure), [DLSS/RR](../Architecture/Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-3--image-reconstruction-and-provider-integration) — `DSP-2/3` |
| 7 | **Tone and output.** One correct color/tone/output transform; display adjustments must not conceal bad transport. | [Tone mapping and presentation](../Architecture/Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-4--tone-mapping-encoding-and-presentation) — `DSP-4` |
| 8 | **Decals and grading.** Complete the required material/display features with raw, temporal and backend checks. | [Decals](../Architecture/Modules/Engine/Renderer/FirstRelease/GeometryAndRayTracingImplementationPlan.md#gr-5--deferred-gbuffer-decals), [grading](../Architecture/Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-5--color-grading) — `GR-5`, `DSP-5` |
| 9 | **Basic lens effects.** Independently prove chromatic aberration and vignette, including no work at neutral settings. | [Lens effects](../Architecture/Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-6--chromatic-aberration-and-vignette), [vignette plan](../Architecture/Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/Vignette/ImplementationPlan.md#basic-vignette-remaining-delivery-plan) — `DSP-6` |
| 10 | **HDR and SDR recovery.** Finish output behavior; HDR proof requires a qualified display/OS tuple. | [HDR output](../Architecture/Modules/Engine/Renderer/FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-7--hdr10-display-output) — `DSP-7` |
| 11 | **Product controls and capture.** Responsive settings/UI/debug/capture/latency consumers with one state authority and honest tool support. | [Runtime/diagnostics](../Architecture/Modules/Engine/Renderer/FirstRelease/RuntimeAndDiagnosticsImplementationPlan.md), [product controls](../Architecture/CrossModule/FirstRelease/ProductImplementationPlan.md#first-release-product-implementation-plan), [external capture](../Architecture/CrossModule/PerformanceDiagnostics/ExternalCapture/ImplementationPlan.md) |
| 12 | **Whole-frame closure.** Complete integration, failure checks, review findings and the remaining feature reports. | `FS-4`, `LGT-4`, `DSP-8`, `RD-6` in their plans; [aggregate closure](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#fr-03--feature-closure) |

Check D3D12 and Vulkan throughout these stages. Measure cost as each feature changes; release step 5 closes the fixed whole-product performance protocol. Reference Stage 10 still waits for its release inputs and package prerequisites.

## Retained Advanced Graphics Roadmap

Start this program after release stabilization (`REL-11`). Each milestone depends on the one before it. Reuse valid release evidence rather than repeat it; these milestones remain unaccepted at this planning snapshot.

| Milestone | What you build or demonstrate | Detailed work |
| --- | --- | --- |
| **M0 — Reproducible entry** | A reviewer can obtain the product/source and reproduce the supported result. | Reuse product build/adoption proof; [portfolio preparation](../Architecture/CrossModule/GraphicsPortfolio/ImplementationPlan.md#graphics-portfolio-delivery-plan) |
| **M1 — Traceable evidence** | Frozen scene/camera/settings identities, content losses and original capture joins. | Revalidate `MAP-00`; [workload sequence](../Acceptance/GraphicsWorkloads.md#workload-gate-sequence) |
| **M2 — Correct content and references** | Bistro/San Miguel results with a qualified raw reference and stated uncertainty. | Reference, material/light and map checks in their existing owners |
| **M3 — Causal classical studies** | One analyzer, three matched-API studies and one difficult incident. Retain a rejected or inconclusive alternative. | [Workload Studies](../Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md#remaining-stages) — `WS-S0–3`; freeze discovery and real producer inputs first |
| **M4 — Owned data and model** | One diffuse-indirect dataset/model and a repeatable training recipe. | [Neural plan](../Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md#remaining-stages) — `NG-S0–2`; accepted neural discovery first |
| **M5 — Model to shader** | Correct exported operators and FP32 shaders, one runtime, then measured optimization. | [Neural plan](../Architecture/CrossModule/NeuralGraphics/ImplementationPlan.md#remaining-stages) — `NG-S3–5`; conformance precedes optimization |
| **M6 — Evaluation and transfer** | Held-out results, three headline cases and independent reproduction/review. | Neural `NG-S6`, study `WS-S3`, [portfolio assembly](../Architecture/CrossModule/GraphicsPortfolio/ImplementationPlan.md#graphics-portfolio-delivery-plan) |

The [delivery catalog](FeatureDeliveryCatalog.md) gives concrete artifact examples for each professional output. Original workload and feature reports retain the measurements and conclusions.

## Conditional And Excluded Work

After M6, select additional work only for a demonstrated need:

- **Upstream contribution or technical submission:** a real issue/recipient and bounded output. Use the [contribution card](../Architecture/CrossModule/GraphicsPortfolio/GraphicsPortfolioDesign.md#upstream-graphics-contribution).
- **Teaching, a second adopter or maintenance transfer:** actual audience, exercise and feedback. Use the [knowledge-transfer card](../Architecture/CrossModule/GraphicsPortfolio/GraphicsPortfolioDesign.md#graphics-knowledge-transfer). M6's first independent review remains required.

**Not selected:** native Linux support; AMD hardware acquisition or AMD specialist profiling; new physics/networking/audio/scripting systems; installer/updater; stable plugin SDK; broad Editor or ML frameworks. Additional hardware needs its own admitted question; [profiling guidance](../Engineering/Verification/ExternalProfiling.md) records available routes.

Geometry-cache animation, volumetrics, frame generation and meshlets/DGC/SER remain outside the selected release. Their [geometry-cache](../Architecture/CrossModule/GeometryCacheAnimation/ImplementationPlan.md), [volumetric](../Architecture/Modules/Engine/Renderer/Features/Lighting/VolumetricLighting/ImplementationPlan.md) and [research](Research/RenderingReferenceExamples.md) documents preserve future context.

## Dependency And Work-In-Progress Rule

Work on **one implementation task and one independent decision/discovery item**. Choose the first unfinished dependency; within it, prefer the fix that unlocks the most required downstream proof. Use the [risk register](#release-risk-register) to break ties.

If a prerequisite is missing, identify its owner and the next concrete action. Continue independent work that can proceed. A necessary shared repair comes before its consumer, with an existing owner and reviewed change/deletion/copy bounds. Missing HDR hardware or an independent reviewer delays the affected proof, not all source work.

Keep required rendering outcomes required. Any scope change needs an explicit owner decision and invalidates the affected evidence. New neural, tool and platform features wait for their admission gates.

## Reference Path Tracer Truth First

Preserve accepted `PTD-00-R1` and delivered source stages. The [remaining plan](../Architecture/Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/ImplementationPlan.md) owns recovery; [Discovery](../Architecture/Modules/Engine/Renderer/Features/Lighting/ReferencePathTracer/Discovery.md) owns the accepted design. Reopen only changed decisions. Remaining implementation/proof uses `PTD-02`; release-map reference adoption uses `PTD-03`.

A finite-depth diagnostic, accumulation count or post-processed image does not qualify reference truth. Dependent quality checks need the accepted raw result and uncertainty.

## One Authority Per Question

| To understand… | Open… |
| --- | --- |
| What to do next | This Roadmap |
| What the professional output should demonstrate | [Feature Delivery Catalog](FeatureDeliveryCatalog.md) |
| What the release promises | [Release scope](../Acceptance/FirstRelease.md#approved-release-scope), [capability dispositions](../Acceptance/ReleaseScope/CapabilityDispositions.md) |
| Exact implementation steps | The feature plan linked from the chosen task |
| Correct behavior and passed proof | The owning feature contract and [Release Acceptance](../Acceptance/FirstRelease.md) |
| How to perform and submit the change | [Change Lifecycle](../Engineering/Workflow/ChangeLifecycle.md), [Change Integration](../Engineering/Workflow/ChangeIntegration.md) and selected standards |

Update the result in its owner, then update this page's current/next pointer. Local plans detail the chosen work; this Roadmap retains the global order.

## Planning Coverage And Fillable Files

All 49 feature families retain their [existing phase assignments](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#feature-work-package-registry). Reuse these plans. The remaining small scaffolds are [Vignette](../Architecture/Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/Vignette/ImplementationPlan.md#basic-vignette-remaining-delivery-plan), [Graphics Portfolio](../Architecture/CrossModule/GraphicsPortfolio/ImplementationPlan.md#graphics-portfolio-delivery-plan) and the contribution/transfer cards above.

To fill a missing plan, use the [remaining-work template](../Engineering/Workflow/Templates/RemainingWorkPlan.md). Resolve its decision owners and check cards before calling it implementation-ready. Retire completed instructions after preserving useful design and evidence.

## Stage Target And Evidence Traceability

Use this section when opening or closing an iteration. The everyday task order is above.

<details>
<summary>Expand milestone IDs and reporting links</summary>

| Milestone | Existing workload/output identities |
| --- | --- |
| M0 | `PGD-01` |
| M1 | `WL-01`, `PGD-02` |
| M2 | `WL-02/03`, `PGD-02` |
| M3 | `WL-04`, `PGD-03/04/05` |
| M4 | `WL-05`, `PGD-06` |
| M5 | `WL-06`, `PGD-07` |
| M6 | `WL-07/08`, `CASE-*`, `PGD-08` |
| Conditional ecosystem work | `PGD-09` |

The [catalog](FeatureDeliveryCatalog.md) maps outputs to `PGE-*`; the [phase registry](../Architecture/CrossModule/FirstRelease/ImplementationPlan.md#feature-work-package-registry) maps `FCR-*`; [Graphics Workloads](../Acceptance/GraphicsWorkloads.md) owns `MAP/WL/CASE` proof. Join these in the [iteration record](../Engineering/Workflow/ChangeLifecycle.md#create-the-iteration-control-record) with the selected objective, risk, `AC/FM/CHK`, candidate and artifact.

At handoff, record results/invalidation in their owner, retire delivered instructions, apply [Code Style](../Engineering/Foundations/CodeStyle.md) and update the next task here. Documentation checks do not pass runtime, GPU, performance, package or adoption criteria.

</details>

## Release Risk Register

Consult this when competing fixes need a priority decision. The retained assessment keeps all 13 risks open; revalidate its source indicators before choosing treatment.

<details>
<summary>Expand the full risk assessment and retirement requirements</summary>

This register owns release-wide risk priority and treatment. Stable feature-specific technical risks live with the owning Architecture feature dossier, while the applicable [`FCR-*` report](../Acceptance/FeatureCompletionReports.md#iteration-traceability-and-coverage) records candidate exposure and disposition; the [release acceptance contract](../Acceptance/FirstRelease.md#failure-mode-acceptance-matrix) owns controlled release-wide failure behavior.

Likelihood is qualitative and justified by current evidence: **High** means the unsafe condition is observed or expected on the unproven route, **Medium** means a plausible untested interaction, and **Low** requires evidence that controls are already effective. Impact is **Critical** when trust, security, data, or release integrity can be lost; **High** when a required release gate or primary promise fails; **Medium** only when a bounded workaround can preserve the advertised product. These ratings prioritize work; they never weaken acceptance.

Current state at the roadmap's source snapshot: all `RISK-REL-*` rows below are **Open**. No risk was retired by documentation or source inspection.

| ID | Risk and current indicator | Likelihood | Impact | Accountable owner and gate | Required treatment and retirement evidence |
| --- | --- | --- | --- | --- | --- |
| `RISK-REL-01` | Scope or audience remains ambiguous; user-reachable features can escape classification. | High: scope and complete inventory are not approved. | High | Release owner; `REL-00` | Freeze audiences, promises, non-goals, budgets, selectors, and every feature disposition; retire only with approved inventory and zero unmatched public selector. |
| `RISK-REL-02` | Identity, rights, dependency provenance, signing, or redistributable payload is invalid or mutable. | High: placeholder identity, moving inputs, and unapproved signing/content routes are observed. | Critical | Product, build, and content/provenance owners; `REL-01` | Pin/hash inputs, clear rights, produce notices/SBOM/threat model, allowlist and sign expected payload; retire with independent inventory/signature review. |
| `RISK-REL-03` | A dirty, cached, private, or machine-specific source route creates non-reproducible bytes. | High: no accepted cold/warm source-adopter record exists. | High | Build owner; `REL-02` | Prove empty-cache and warm/offline tagged-source routes on the declared toolchain; retire with raw provenance and non-author reproduction. |
| `RISK-REL-04` | Stage/package omits dependencies, includes private/debug files, mutates install bytes, or relies on repository paths. | High: no owned release package exists and package-mode mutable paths are observed. | Critical | Package owner; `REL-03`, `REL-08` | Implement one manifest-owned Build-Cook-Stage-Sign-Verify-Package path and per-user state; retire with package diff, signatures, read-only/offline clean-machine proof. |
| `RISK-REL-05` | Source-present or selectable behavior is mistaken for a complete feature; silent fallback manufactures plausible success. | High: features are unclassified and required-content fallback to `Empty` is observed. | Critical | Feature owners; `REL-04` | Complete every `FCR-*` trace/AC/FM/check ledger, expose requested-versus-active state, fix or make the route unreachable; retire with zero orphan or silent-fallback path. |
| `RISK-REL-06` | Shipped examples contain material, transform, lighting, temporal, encoding, or backend-specific artifacts. | Medium: broad source paths exist but staged-package map/reference evidence does not. | High | Content and graphics-quality owners; `REL-05` | Freeze cameras/references/thresholds, inspect debug outputs and motion on each backend, resolve `S0`/`S1`; retire with approved per-map packages. |
| `RISK-REL-07` | The 30 FPS promise hides CPU/GPU stalls, load cost, memory growth, thermal effects, or lifecycle instability. | Medium: timing primitives exist but candidate-bound measurements do not. | High | Performance and feature owners; `REL-06` | Measure frozen routes with p50/p95/p99/worst, timelines, memory high-water, repeat/soak and causal controls; retire only when every supported row passes. |
| `RISK-REL-08` | D3D12/Vulkan capability, synchronization, lifetime, shader ABI, or device-loss behavior diverges. | Medium: both source backends exist without paired native-validation evidence. | Critical | RHI/Renderer owners; `REL-07` | Execute focused paired backend and native validation, capability rejection, delayed completion/retirement, and incident diagnostics; retire with zero uncategorized findings. |
| `RISK-REL-09` | Game consumers or source adopters require author knowledge, elevated privileges, private caches, unstable contracts, or undocumented repair. | High: neither independent journey has passed. | High | Product, documentation, and source-adoption owners; `REL-08`, `REL-09` | Run both journeys from public instructions in clean states, record confusion/interventions, freeze compatibility/support boundary; retire with independent acceptance. |
| `RISK-REL-10` | Published bytes, policy, support, security intake, patch/withdrawal, or stabilization response fails after delivery. | High: public operations and response evidence do not exist. | Critical | Release and evidence-review owners; `REL-10`, `REL-11` | Verify immutable remote bytes, accurate no-support/self-service instructions and known-defect dispositions; rehearse versioned replacement/withdrawal during finite release review; retire at approved closeout. |
| `RISK-REL-11` | Evidence is stale, non-detecting, cherry-picked, generated without review, or bound to different bytes/configuration. | High: current evidence is source-only and candidate artifacts do not exist. | Critical | Evidence reviewer; all gates | Map every AC/FM to a defect-detecting `CHK-*`, hash artifacts, record invalidation triggers and unavailable checks; retire per claim only after independent review. |
| `RISK-REL-12` | Scope creep or parallel feature work consumes capacity before the current gate closes. | High: the repository has many source-present unfinished surfaces. | High | Release owner; all gates | Enforce one primary gate, key-check-first iteration, explicit exclusion/deletion, and WIP review; retire only when `REL-11` unlocks new features. |
| `RISK-REL-13` | A shared, truncated, biased, unconverged, or post-processed reference is treated as ground truth and approves wrong PBR/lighting/map results. | High: source stages are delivered, but recovery, invalid-sample/preflight and oracle evidence remain open. | Critical | Renderer and evidence owners; `PTD-00`, `REL-04`, `REL-05` | Preserve accepted `PTD-00`; close the remaining `FCR-REN-08` recovery and raw analytic/minimal/independent/backend evidence; retire only when every dependent claim names a defect-detecting oracle and uncertainty. |

Default contingency for every open release risk is to hold the affected gate and either repair the same candidate or reduce advertised scope through its owning acceptance decision. Integrity/security failure requires candidate quarantine or withdrawal. No contingency may relabel missing evidence, silent fallback, or a different configuration as a pass.

A risk stays **Open** until its retirement evidence is linked from the current iteration and accepted at the named gate. Mitigation work without proof changes activity, not risk state. A realized risk also receives an `FM-*`/defect identity and invalidates every dependent result.

</details>

## Scope Protection

Correctness, lifetime safety, paired APIs and required rendering are release work. Extra presentation polish, second hardware and generalized tooling can wait. Preserve qualified reference truth, held-out evaluation, the classical fallback and honest limitations.

When a candidate fails, repair its owner or explicitly revise scope and rerun affected proof. Publish only approved bytes; begin the advanced program only after `REL-11`.
