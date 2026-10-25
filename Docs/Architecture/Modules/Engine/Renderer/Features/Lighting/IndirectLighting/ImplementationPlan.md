# Indirect Lighting And ReSTIR GI Staged Implementation Plan

[Docs](../../../../../../../README.md) / [Up](IndirectLightingDesign.md)

Stages: [Stage 0](#stage-0--close-ind-d0) / [Stage 1](#stage-1--prove-initial-transport-and-technique-accounting) / [Stage 2](#stage-2--introduce-the-explicit-path-record-and-delete-seed-replay) / [Stage 3](#stage-3--implement-shift-mappings-and-gris-in-isolation) / [Stage 4](#stage-4--add-temporal-and-spatial-product-reuse) / [Stage 5](#stage-5--expand-to-the-admitted-multi-bounce-and-glossy-domain) / [Stage 6](#stage-6--deliver-portable-reconstruction) / [Stage 7](#stage-7--evaluate-advanced-reuse-guiding-or-caching) / [Stage 8](#stage-8--backend-workload-adoption-and-fcr-closure).

Replace seed replay with an explicit indirect-transport estimator whose reuse and reconstruction can be checked.

**Status:** implementation-ready sequence, blocked by `IND-D0`, applicable release admission, and stage-specific oracle/provider gates

## Work This Plan

| What to do | Working route |
| --- | --- |
| First action | [Stage 0 discovery](#stage-0--close-ind-d0): close IND-D0 path accounting, shift mappings, validity and the initial comparison route. |
| Result to reach | A frozen transport/check contract, followed by explicit path records, isolated shifts and mutation-safe reuse. |
| Start / stop | IND-D0 and applicable release admission block production. Analytic/external pinned checks can support early stages; scene-level claims in Stages 5/8 require accepted Reference output. |

## Outcome

Replace the current seed-replay prototype with a mathematically explicit, testable ReSTIR GI/GRIS path that produces reliable raw indirect diffuse/specular, survives temporal change, reconstructs portably, and expands to broader glossy/multi-bounce transport only after proof.

## Delivery Order

```text
IND-D0 + release admission
  -> IND-1 transport accounting/oracles
  -> IND-2 explicit initial path and clean-break record
  -> IND-3 shifts and GRIS without temporal reuse
  -> IND-4 temporal/spatial reuse and mutation safety
  -> IND-5 admitted multi-bounce/glossy domain
  -> IND-6 reconstruction
  -> IND-8 candidate evidence and FCR-REN-07

IND-6 -> IND-7 -> IND-8 only for separately admitted advanced reuse/guiding.
Required SHaRC -> NRC follows the cache plans after qualified continuation inputs.
```

The accepted Reference Path Tracer is required before scene-level reference claims in Stages 5/8, but analytic/metamorphic and external manifest-pinned references allow Stages 1–4 to progress independently.

## Planning Envelope

Resolve only the admitted technique accounting, path-record/shift/GRIS, glossy-domain and reconstruction decisions in IND-D0. Freeze path/history memory, ray/dispatch cost and quality/convergence thresholds before the corresponding slice. Guiding, caches and extra optimization tracks remain conditional; no effort estimate schedules them.

## Universal Execution Contract

Each stage passes the [Stage Source-Style Gate](../../../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate) after the final edit, retaining exact formatter coverage and authored spacing/responsibility review. No-source work records justified N/A and its artifact/docs checks.

Every stage:

- rechecks revision/status/guidance and inspects exact producers, consumers, lifetime, shader/build/package membership, and concurrent edits;
- follows Change Integration, Change Lifecycle, Renderer, Module Ownership, Data And Memory, and Validation And Evidence;
- applies the [stage complexity budget](../../../../../../../Engineering/Workflow/ChangeLifecycle.md#define-the-outcome-and-complexity-budget) to one private feature capsule and existing scene/material/light/sky/ray/frame-graph owners; uses typed bindings/direct calls, narrows unnecessary existing representations and justifies every persistent addition;
- creates no duplicate path tracer, scene representation, GI framework, compatibility schema, forwarding wrapper, or permanent diagnostics subsystem;
- updates code, shaders, registrations/cooking, generated surfaces, checks, and affected docs together;
- preserves unrelated dirty work, especially the existing user-owned Reference Path Tracer edit;
- chooses the smallest falsifying checks and states all unrun runtime/GPU/backend/performance work;
- stops when an equation, mapping, oracle, admission, ownership boundary, or failure response is unresolved.

New test-only files/classes/executables may be local-only but must not be submitted. Temporary probes are removed before handoff.

## Stage Delivery Contract Matrix

<details>
<summary>Expand stage delivery contract matrix</summary>

This matrix is normative for the stage. Stage 0 replaces responsibility names with exact files, CMake/shader registration/generated members and hooks before Stage 1 is authorized.

| Stage | Prerequisites | Required production delta and deletion | Explicit non-goals | Retained deliverables | Smallest stage falsifier |
| --- | --- | --- | --- | --- | --- |
| 0 | current revision/status/source trace; release/reference owners reachable; bounded probe route | discovery artifacts/probes only; no estimator behavior change; remove temporary probes | renaming seed replay, increasing bounces/candidates, implementing a cache | accepted decision/technique/domain registers, seed-replay equation trace, oracle/metric/profile table, exact hook/deletion map, revised estimates and review | any `IND-D0-*` choice, equation, threshold, owner, rights or first-slice file remains unresolved |
| 1 | accepted `IND-D0-01/02/03/04/09/10/12/14`; admitted work and independent fixtures | qualify CPU/analytic/metamorphic cases and bounded no-reuse GPU output at the current initial-sampling/raw-lobe boundary or through a disposable probe; correct only ledgered shared semantic defects | separate integrator/resolve/registration/toggle, reservoir/path-record replacement, cross-pixel/frame reuse, reconstruction, broader depth | executable technique ledger, raw per-terminal/lobe artifacts, reference/independence manifest, ray/path/finite counters | NEE/emission/environment/depth controls violate predictions, initial mean misses threshold, or proof leaves a second production estimator |
| 2 | Stage 1 pass; frozen logical path record/packing/identity and seed-replay deletion list | establish private feature owner and explicit path/initial-reservoir state; update bindings/shaders/captures/build together; delete float seed payload and replay readers/writers | shifts, temporal/spatial reuse, record compaction beyond proof, compatibility schema | logical/packed record spec, max/round-trip/long-run tests, fresh-only equivalence and stale-symbol/deletion audit | any required probability/generation fact is absent, integer identity loses precision, or old path survives |
| 3 | Stage 2 pass; exact admitted shift domains/inverses/support/Jacobians/GRIS weights and static fixtures | implement only admitted mapping/reservoir functions, deleting superseded mapping/weight formulas; check deterministic same-frame GPU output against a local independent CPU oracle; no CPU runtime transport or persistent history publication | temporal reuse, broader lobes/depth, reconstruction, guiding/cache | hand round trips, rejection taxonomy, enumerated/statistical distributions, destination mean comparison, finite/extreme tests | mapping cannot be derived in one measure, rejected sample adds mass, duplicate semantic authority remains, or statistics exclude reference |
| 4 | Stage 3 pass; frozen reprojection/splat, ordering/neighbors/correlation and mutation policies | add only consumed per-View previous/current path/receiver state, temporal/spatial proposals, confidence and transactional publication/retirement; delete replaced history validity and copied receiver/confidence truth | reconstruction, path-depth/lobe expansion, pass fusion/advanced splatting | mutation/dual-view/cancel/long-run artifacts, duplication/autocorrelation/diversity/error curves, replacement memory | stale/cross-view path publishes, redundant history authority remains, current scene mutation maps without proof, or reuse exceeds error/correlation bounds |
| 5 | Stage 4 pass; next path depth/lobe/terminal class separately admitted; accepted RPT/external reference | extend technique ledger, logical/packed record, initial generator, mapping/GRIS and resolve together for one increment; replace narrower special cases and delete rejected domain/packing variants | multiple increments, unapproved delta/transmission/caustics, reconstruction tuning | roughness-depth-terminal matrix, raw reference/convergence, ray/path/state budgets, explicit exclusions/status | new domain lacks alternate-technique/MIS/mapping support, adds unconsumed record fields, or regresses previously accepted cells |
| 6 | included Stage 5 domain passes; portable reconstruction and optional-provider contract accepted | add portable lobe reconstruction through existing provider owner, consumed guides/confidence/history/status and optional RR adapter; delete replaced filter experiments and duplicate guide/history state | filter feedback into estimator, using denoising to admit unsupported transport, sole vendor path | raw/reconstructed motion/disocclusion/glossy/emissive/sky sequences, guide/fault manifests, lag/detail/time/memory, UX route | raw failure is hidden, duplicate or stale guides/history remain, or mandatory portable profile cannot recover safely |
| 7 | Stage 6 pass; one named failing workload and one optimization admission with frozen A/B | integrate only selected enhanced reuse/splatting/guiding/cache mechanism; update semantics/history/hooks/tests; remove entirely on failure | another GI architecture, bundled optimizations, permanent experiment switch | reproduced baseline failure, equal-time A/B, correlation/lag/time/memory/backend regressions, provenance/hook/removal record | no measured benefit, semantic core changes, duplicate scene/GI state, or any required regression fails |
| 8 | all included stages pass; exact candidate/hardware/content/reference/provider matrix; `FCR-REN-07` owner ready | evidence and stale-state closure only; production fix requires new candidate/re-admission; remove final prototype names/docs | redesign/tuning during evidence, borrowed proof across domains/backends, unrelated repair | full FCR bundle, path/domain/status UX, backend/native validation, workload/raw/reference, convergence/time/memory/package/enclosure results | any included AC/FM/CHK or exclusion lacks candidate-bound disposition, or seed-replay residue remains |

</details>

## Required Stage Handoff

Every stage reports start/end revision and dirty boundary; accepted prerequisites; changed/deleted files by responsibility including build/generated/package; external hook and bounded-removal ledgers; semantic/technique mapping; exact commands/configurations/artifacts/results; cleanup; all unrun checks; remaining domain/quality/backend limits; and a binary next-stage authorization. Compare actual state/API/hooks/copies/variants/dependencies with the frozen stage budget; unjustified additions or deferred replacement cleanup block that stage, even when images pass. Include a positive check of retained selected behavior. Raw estimator, mapping, reservoir and reconstruction verdicts remain separate.

## Copy-Ready Prompt Contract

Every stage prompt is the local quotation plus this mandatory tail; copy both:

> NON-NEGOTIABLE: verify the stage matrix prerequisites and accepted Discovery/Transport/Architecture/User Experience contracts before editing. Keep path generation, records, mappings, reservoirs, histories, diagnostics and feature failures in the frozen feature home; generic owners only expose the ledgered scene/light/environment/ray/frame/build/publication hooks. Preserve the independent no-reuse/raw oracle, perform the listed clean-break deletions atomically, and do not invent a path domain, technique, PDF, shift, Jacobian, GRIS weight, identity, lifetime, fallback, public control, threshold, budget, provider, compatibility path, wrapper, or cache.
>
> STOP: report `BLOCKED` before production mutation if any prerequisite/equation/oracle/threshold/owner/rights/capability is absent, evidence contradicts the contract, concurrent work overlaps the owned boundary, or the change exceeds the single stage. Handoff quotes every deliverable/exit condition with candidate-bound proof, lists hooks/deletions and exact checks/artifacts/results/cleanup/unrun work, and states whether the next stage is authorized.

## Stage 0 — Close `IND-D0`

### Objective

Freeze product/path domain and prove what the current seed-replay route does before replacing it.

### Work

1. Reuse valid IND-X proof and run only experiments consumed by the admitted transport/reuse slice, with exact candidate identity. Explicitly defer unselected guiding/cache/advanced-reuse families rather than prototype them for gate completeness.
2. Trace current per-bounce NEE/emission/environment accounting and seed replay from equations to shader output.
3. Freeze oracles, path/technique domain, initial proposal, record, shift, GRIS weight, correlation/bias mode, histories, reconstruction, budgets, profiles, hooks/deletions, and FCR fields.
4. Reconcile release admission and Reference Path Tracer dependency.
5. Obtain independent gate review.

### Exit Gate

- all `IND-D0-01` through `14` decisions have explicit reviewed dispositions;
- current seed replay is either formally proved for a narrow domain or named for deletion; absence of proof cannot preserve it;
- Stage 1 has frozen tests, thresholds, files/hooks, owner and resource/check bounds.

### Ready-To-Use Prompt

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/IndirectLighting/ImplementationPlan.md#stage-0--close-ind-d0 and apply this plan's shared execution contract. Apply the owning execution contract and its Stage Source-Style Gate.
Execute Indirect Lighting Stage 0 from `IndirectLighting/ImplementationPlan.md`. Treat this as discovery, not an estimator rewrite. Decode and trace the current seed-replay paths, targets, weights, technique accounting, history, and raw outputs; run only the IND-X experiments needed by the admitted slice and defer unselected optimizations; freeze each consumed `IND-D0-*` decision and threshold before candidate observation; name the exact replacement/deletion ledger; reconcile release and Reference Path Tracer gates; and obtain separate review. Report unknowns and unrun checks explicitly.
```

</details>

## Stage 1 — Prove Initial Transport And Technique Accounting

### Objective

Build independent one-bounce truth before reservoirs.

### Work

1. Add CPU/analytic/metamorphic tests for furnace/energy, diffuse and rough-glossy secondary surfaces, isolated NEE, emissive hit, environment miss, occluder insertion, light/environment scaling, path-depth and roulette boundaries.
2. Exercise bounded no-reuse GPU initial sampling using frozen BSDF/light proposals at the current raw-lobe boundary or through a disposable probe. Keep necessary semantic repairs in the existing initial generator for Stage 2 to consume; do not add a standalone estimator, runtime mode or registration for this oracle.
3. Record selection/conditional PDFs, throughput, lobe, terminal technique, MIS, path depth, and finite counters through existing test/capture routes.
4. Compare to independent external or accepted reference artifacts with shared-code analysis where available.
5. Correct shared transport semantic defects only within the accepted Stage-1 hook ledger.

### Exit Gate

- `AC-IND-01/02/07` foundations and `CHK-IND-01` pass;
- adding/removing NEE, emitter, environment, or path depth changes energy exactly as predicted;
- no reservoir reuse or reconstruction contributes to the result;
- any production repair has its real Stage-2 consumer and retained-result check; local oracle/probe code is removed and no parallel estimator remains.

### Ready-To-Use Prompt

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/IndirectLighting/ImplementationPlan.md#stage-1--prove-initial-transport-and-technique-accounting and apply this plan's shared execution contract. Apply the owning execution contract and its Stage Source-Style Gate.
Implement Indirect Lighting Stage 1 only. Establish accepted local CPU/analytic/metamorphic cases and qualify bounded GPU one-bounce initial sampling through the current raw-lobe boundary or a disposable probe, with explicit BSDF, NEE, emissive/environment, MIS, lobe, throughput, depth, and roulette accounting. Produce raw `IndirectDiffuse`/`IndirectSpecular` artifacts without reservoir reuse or reconstruction. Fix only proved semantic defects in the existing initial generator and frozen hook ledger; remove local oracle/probe additions before handoff. Add no standalone estimator, permanent mode or registration. Do not introduce the new reservoir yet or increase path scope.
```

</details>

## Stage 2 — Introduce The Explicit Path Record And Delete Seed Replay

### Objective

Replace the float-packed random identity with one auditable initial path sample and integer/generation-safe state.

### Work

1. Establish the feature capsule and frozen `IndirectPathSample`/initial reservoir layout.
2. Store explicit receiver, secondary vertex, lobe/directions, terminal sample, technique/PDF, depth/termination, target/contribution facts required by the chosen mappings.
3. Add pack/unpack/maximum-value/long-run integer round-trip and format precision tests.
4. Route Stage-1 initial samples into the new record and raw evaluation.
5. Delete the old `RandomPixel/SampleIndex/RandomFrameIndex` float payload and every seed-replay caller/shader/layout in one clean break.

### Exit Gate

- `AC-IND-03`, `FM-IND-01/06`, and record portions of `CHK-IND-02/04/08` pass;
- initial output still agrees with Stage-1 truth;
- no dual record, reader, shader, selector, or compatibility path remains.

### Ready-To-Use Prompt

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/IndirectLighting/ImplementationPlan.md#stage-2--introduce-the-explicit-path-record-and-delete-seed-replay and apply this plan's shared execution contract. Apply the owning execution contract and its Stage Source-Style Gate.
Implement Indirect Lighting Stage 2. Create the accepted private feature owner and explicit path-sample/initial-reservoir record with integer logical identities and the exact factors needed by future shifts. Add packing and long-run round-trip tests. Feed the proved Stage-1 initial estimator through it, then delete the old float-packed seed identity and all seed-replay code in the same change. Keep temporal/spatial reuse disabled; no compatibility schema or legacy selector.
```

</details>

## Stage 3 — Implement Shift Mappings And GRIS In Isolation

### Objective

Prove reconnection/replay mapping and generalized reservoir math before temporal state.

### Work

1. Implement the frozen reconnection mapping with inverse, support, Jacobian, visibility, roughness/lobe, and rejection reasons.
2. Add replay/hybrid mapping only if required by the Stage-3 domain.
3. Implement ratified GRIS contribution/resampling weights, target, stream update, effective count/cap, and final estimate.
4. Exercise enumerated discrete proposals, analytic paths, duplicated/correlated candidates, invalid mappings, zero target, and extreme weights.
5. Run spatial reuse in a deterministic same-frame fixture or CPU kernel; do not publish temporal history.

### Exit Gate

- `AC-IND-04/05` static foundations and `CHK-IND-02` pass;
- mapping round trip/Jacobian/support and statistical frequencies meet predeclared thresholds;
- no invalid or non-finite candidate enters a reservoir.

### Ready-To-Use Prompt

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/IndirectLighting/ImplementationPlan.md#stage-3--implement-shift-mappings-and-gris-in-isolation and apply this plan's shared execution contract. Apply the owning execution contract and its Stage Source-Style Gate.
Implement Indirect Lighting Stage 3 on the explicit Stage-2 path record. Add only the ratified reconnection and any required hybrid/replay mapping, including inverse, support, Jacobian, visibility, and deterministic rejection. Implement the exact GRIS target/contribution/resampling weight and bounded reservoir update. Validate in CPU/deterministic same-frame fixtures and statistical tests without temporal history. Stop if the pinned equation cannot be mapped unambiguously to code.
```

</details>

## Stage 4 — Add Temporal And Spatial Product Reuse

### Objective

Publish generation-safe per-View reuse that handles motion, disocclusion, animation, light/environment change, and correlation.

### Work

1. Add previous/current path receiver, geometry/material/light/environment/shader/provider generation views.
2. Implement frozen reprojection or splat proposal, temporal shift, spatial neighbors, ordering, random dimensions, disocclusion/fresh-candidate policy, and confidence.
3. Keep previous history immutable while writing current; publish only after successful completion and retire by submission.
4. Exercise dual views, cuts, resize, rigid/deformed motion, material/roughness/alpha, moving occluder/emitter, sky rotation/change, reload, provider change, cancel, and long runs.
5. Measure selected-path diversity, temporal autocorrelation, raw error, and cost as reuse grows.

### Exit Gate

- `AC-IND-05/06`, `FM-IND-02/04/05/06`, and `CHK-IND-03/04` pass;
- active correlation/bias mode and effective-count meaning are reported;
- no stale/cross-view/non-finite history is published.

### Ready-To-Use Prompt

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/IndirectLighting/ImplementationPlan.md#stage-4--add-temporal-and-spatial-product-reuse and apply this plan's shared execution contract. Apply the owning execution contract and its Stage Source-Style Gate.
Implement Indirect Lighting Stage 4 after static shift/GRIS proof. Add the accepted per-View temporal proposal, current-scene shift, spatial neighbors, disocclusion policy, confidence, full generation identity, transactional history publication, and retirement. Run the entire `CHK-IND-03/04` mutation and long-run matrix and measure diversity/correlation as well as raw error. Do not add reconstruction or expand path depth in this stage.
```

</details>

## Stage 5 — Expand To The Admitted Multi-Bounce And Glossy Domain

### Objective

Deliver the exact path depth and rough-specular support promised by the product, using ReSTIR GI or generalized ReSTIR PT mappings as frozen. M5R requires multi-bounce diffuse color bleeding and rough-specular continuation, emitter/environment terminals and independently qualified references; diffuse one-bounce alone is insufficient. Freeze depth convention/range and roughness bounds in IND-D0 before each increment.

### Work

1. Add one path depth/lobe class at a time with explicit technique/PDF/MIS/roulette and record changes.
2. Introduce hybrid replay through specular chains only when inverse/support/probability tests exist.
3. Compare every increment against accepted reference AOVs and Stage-1 energy/metamorphic cases.
4. Exercise emissive/environment terminals, hard indirect visibility, roughness sweep, thin/alpha/animated geometry, and depth truncation.
5. Freeze or report exclusions for perfect delta, transmission, and caustics.

### Exit Gate

- `AC-IND-01/07/09` pass for the declared domain and `CHK-IND-01/06` show bounded error/convergence;
- unsupported domains expose exact status;
- path/ray/state cost remains within the Stage-0 budget or the profile is revised through review.

### Ready-To-Use Prompt

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/IndirectLighting/ImplementationPlan.md#stage-5--expand-to-the-admitted-multi-bounce-and-glossy-domain and apply this plan's shared execution contract. Apply the owning execution contract and its Stage Source-Style Gate.
Implement only the next Indirect Lighting Stage-5 path-depth/lobe increment admitted by `IND-D0`. Extend the path record, technique accounting, shift, and GRIS equations together; add hybrid replay only with inverse/support/PDF tests. Compare raw diffuse/specular against accepted independent reference artifacts over roughness, environment/emissive, occlusion, animation, and depth cells. Preserve explicit exclusions for delta, transmission, and caustics.
```

</details>

## Stage 6 — Deliver Portable Reconstruction

### Objective

Make the proved raw indirect estimator product-usable under motion while retaining independent raw evidence.

### Work

1. Implement/adapt the accepted vendor-neutral diffuse/specular reconstruction through the existing provider owner.
2. Bind exact radiance/hit-distance, depth, normals, roughness/material, motion, confidence/disocclusion, exposure, and history generation.
3. Connect optional DLSS RR under the same requested/active/failure semantics.
4. Run static/motion/disocclusion, emissive toggle, environment rotation, glossy detail, thin geometry, rapid camera, and invalid-guide/provider faults.
5. Measure raw and reconstructed quality/lag/time/memory separately.

### Exit Gate

- `AC-IND-08`, `FM-IND-08`, and `CHK-IND-05` pass;
- portable baseline works for the mandatory profile;
- denoiser does not feed filtered values into estimator history or hide missing path support.

### Ready-To-Use Prompt

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/IndirectLighting/ImplementationPlan.md#stage-6--deliver-portable-reconstruction and apply this plan's shared execution contract. Apply the owning execution contract and its Stage Source-Style Gate.
Implement Indirect Lighting Stage 6 using the frozen portable reconstruction contract, then connect optional DLSS Ray Reconstruction through the existing provider boundary. Preserve raw lobe/path/reservoir evidence and separate histories. Exercise invalid guide/provider faults plus static, camera motion, disocclusion, environment/emissive change, glossy detail, and thin geometry. Report raw versus reconstructed error, lag, time, and memory.
```

</details>

## Stage 7 — Evaluate Advanced Reuse, Guiding, Or Caching

### Objective

General advanced reuse/guiding remains optional and initially Deferred; first-release base closure may proceed from Stage 6 to Stage 8. Required later [SHaRC then NRC](../RadianceCaching/RadianceCachingDesign.md) now has separate contracts/plans. Those M5R deliveries cannot be skipped here or implemented as a second cache through this optional stage.

### Candidate Increments

- ReSTIR PT Enhanced reciprocal neighbors, footprint reconnection, or duplication maps;
- reservoir splatting/multilayer disocclusion reuse;
- ReSTIR PG initial-candidate guiding;
- DDGI or another unselected cache remains a future product decision. Required SHaRC -> NRC follows its linked owner and cannot be substituted by DDGI.

### Work And Exit

Run an A/B experiment against Stage 6 with frozen error/lag/time/memory metrics. If admitted, update discovery, research pin, equations, history, hook/deletion ledger, profiles, tests, and docs before code. Pass only if the targeted workload improves without regressing required cells beyond threshold or creating a second scene/GI architecture.

### Ready-To-Use Prompt

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/IndirectLighting/ImplementationPlan.md#stage-7--evaluate-advanced-reuse-guiding-or-caching and apply this plan's shared execution contract. Apply the owning execution contract and its Stage Source-Style Gate.
Evaluate the single Indirect Lighting Stage-7 optimization named by the accepted admission record. First reproduce the failing workload and freeze A/B metrics/budgets. Implement the smallest integration into existing path/reservoir/history owners, update equations and failure tests, and report quality/correlation/lag/time/memory across all required regressions. Remove the experiment if it does not pass; do not bundle another GI system.
```

</details>

## Stage 8 — Backend, Workload, Adoption, And FCR Closure

### Objective

Publish evidence for the exact product claim and close or block `FCR-REN-07` honestly.

### Work

1. Freeze candidate/build/shader/backend/device/driver/content/reference/settings identities and thresholds.
2. For release closure, use analytic/Cornell controls and the approved release-map cells for admitted static/motion/disocclusion/animation/environment/emissive/glossy and failure behavior. Bistro/San Miguel and held-out studies join only after advanced-program admission.
3. Exercise supported D3D12/Vulkan traversal/reconstruction profiles with native validation.
4. Retain raw lobes/path/reservoir/confidence, reconstructed products, reference AOVs, convergence/error statistics, timing distributions, and peak/history memory.
5. Validate requested/active/excluded/degraded UX, build/cook/package, generated surfaces, enclosure/hooks, stale paths, scoped diff, and `git diff --check`.
6. Write only observed results into `FCR-REN-07`.

### Exit Gate

- `AC-IND-01` through `AC-IND-13` have dispositions for the admitted candidate domain; unselected Stage-7 optimization families stay deferred;
- required cells pass or the report remains blocked with exact dependencies;
- the seed-replay prototype and stale documentation are absent.

### Ready-To-Use Prompt

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/Lighting/IndirectLighting/ImplementationPlan.md#stage-8--backend-workload-adoption-and-fcr-closure and apply this plan's shared execution contract. Apply the owning execution contract and its Stage Source-Style Gate.
Execute Indirect Lighting Stage 8 as candidate validation. Freeze exact candidate/configuration/reference identity and metrics before runs. Exercise analytic/Cornell, selected release-map, admitted motion/disocclusion/animation/environment/emissive/glossy/failure and D3D12/Vulkan cells. Do not require later Bistro/San Miguel studies for first-release closure. Retain raw and reconstructed artifacts and separate estimator, convergence, temporal, visual, performance, memory, backend, and release verdicts. Update `FCR-REN-07` only from observed evidence.
```

</details>

## Stage Traceability

<details>
<summary>Expand stage traceability</summary>

| Stage | Acceptance | Checks |
| --- | --- | --- |
| 0 | all scope/gate fields | `IND-X-*`, discovery failures |
| 1 | `AC-IND-01/02/07` foundation | `CHK-IND-01/06` |
| 2 | `AC-IND-03/12` | `CHK-IND-02/04/08` |
| 3 | `AC-IND-04/05` static | `CHK-IND-02` |
| 4 | `AC-IND-05/06/10` partial | `CHK-IND-03/04` |
| 5 | `AC-IND-01/07/09/10` | `CHK-IND-01/06` |
| 6 | `AC-IND-08/10` | `CHK-IND-05` |
| 7 | admitted subset | A/B plus all affected regression checks |
| 8 | `AC-IND-10/11/12/13` closure | `CHK-IND-06/07/08`, `FCR-REN-07` |

</details>

## Stop Rules

Stop if an equation/mapping/domain/tolerance is unresolved; a reference is dependent or unaccepted; one-bounce accounting fails; path identity cannot be retained safely; an optimization exceeds its admission; a backend/provider changes semantics; history cannot remain transactional within budget; or concurrent changes overlap the owned boundary. Preserve the last valid candidate and report the exact blocker—never widen the stage or call a denoised preview proof.

## Completion Rule

Only an evidence-backed `FCR-REN-07` can close this plan. Documentation, compiled code, a working screenshot, or a paper-derived implementation cannot.

## Reference And Ownership

<details>
<summary>Exact owners, source snapshots and related contracts</summary>

**Responsibility:** own dependency order, staged clean break, prerequisites, estimates, non-goals, exit evidence, copy-ready prompts, and stop rules

**Authority boundary:** this plan consumes accepted Discovery/Semantics/Architecture/User Experience without changing them; code/build proves implementation and `FCR-REN-07` owns candidate results

**Plan revision:** `IND-PLAN-02`, deepened 2026-09-13 from repository revision `8b650c7450f8a59fb3bcc18edbb4d217a7b11ed5`

</details>
