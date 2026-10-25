# Color Grading Delivery Plan

[Docs](../../../../../../../../README.md) / [Up](ColorGradingDesign.md)

Stages: [Stage 0](#stage-0--close-discovery) / [Stage 1](#stage-1--freeze-the-first-parameter-slice) / [Stage 2](#stage-2--deliver-the-parameter-only-grade) / [Stage 3](#stage-3--add-the-typed-lut-source-and-cooked-contract) / [Stage 4](#stage-4--join-residency-and-apply-the-lut) / [Stage 5](#stage-5--complete-the-development-experience) / [Stage 6](#stage-6--evidence-package-and-close).

Add predictable color controls and a typed LUT workflow, with no rendering work when the grade is neutral.

**Status:** conditional implementation plan; Stage 0 is permitted, Stage 1 freezes documentation after discovery, and production Stages 2-6 require `CGRD-00 PASS` and `DSP-5` prerequisites

**Current readiness:** **0/100** — plan presence is not implementation or evidence.

## Work This Plan

| What to do | Working route |
| --- | --- |
| First action | [Stage 0 discovery](#stage-0--close-discovery): close CGRD-00 domain, parameter/LUT and ownership decisions. |
| Result to reach | An accepted design and numeric checks before the first complete parameter grade and admitted LUT cook/residency slices. |
| Start / stop | Stage 0 planning is permitted; production waits for CGRD-00 PASS and DSP-5 prerequisites. HDR cells retain their own output contract. |

## Outcome

Deliver one narrow, source-backed, scene-referred color-grade feature whose neutral path costs no pass, whose parametric and optional LUT results match an independent oracle, whose View/request/resource generations remain truthful through edits and failures, and whose exact cooked configuration reaches admitted packaged products. Completion is the `FCR-REN-24` verdict for one candidate, not the existence of a shader or editor panel.

## Gate And Dependency Graph

```mermaid
flowchart LR
    D0[CGRD-00 discovery] --> S1[Stage 1 first-slice freeze]
    DSP0[DSP-0 domain vocabulary] --> S1
    DSP4[DSP-4 reconstruction edge] --> S1
    S1 --> S2[Stage 2 parameter grade]
    D0 --> S3[Stage 3 typed LUT cook]
    S2 --> S4[Stage 4 resident LUT application]
    S3 --> S4
    S4 --> S5[Stage 5 development experience]
    S5 --> S6[Stage 6 candidate evidence]
    HDR[HDR output contract if admitted] --> S6
    S6 --> FCR[FCR-REN-24]
```

Stages 2 and 3 may proceed in separate changes after their shared accepted decisions exist, but Stage 4 cannot integrate either against an unfrozen semantic/cooked identity. Release ordering does not authorize a stage whose local prerequisites are missing.

## Planning Envelope

CGRD-00 freezes working domain, parameter and .cube subset, owner/files, parser token/dimension/byte bounds, residency/upload/retirement high-water and pass/variant cost. Keep neutral identity, oracle, parser safety, raw products, paired-backend/package and failure evidence. Stop if a general color framework becomes necessary; no estimated hours authorize that expansion.

## Universal Execution Contract

Each stage passes the [Stage Source-Style Gate](../../../../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate) after the final edit, retaining exact formatter coverage and authored spacing/responsibility review. No-source work records justified N/A and its artifact/docs checks.

Every stage starts from current status/revision/dirty state, maps its touched `AC-CGR-*`, `FM-CGR-*`, `RISK-CGR-*`, and `CHK-CGR-*`, and names the smallest claim-falsifying check. Apply the [stage complexity budget](../../../../../../../../Engineering/Workflow/ChangeLifecycle.md#define-the-outcome-and-complexity-budget) to existing settings, View, frame-graph, shader, asset/cook, residency, editor, capture, and package owners: retain/narrow/delete their touched paths and justify each consumed addition. Use one scene-grade semantic contract on D3D12 and Vulkan. Delete replaced names/paths in the same stage; unjustified additions or postponed cleanup block exit. Add no compatibility reader, alternate settings authority, generic color framework, permanent test-only code, or silent fallback.

Every stage stops when an accepted discovery decision is missing or contradicted; working space/order is ambiguous; requested and active state cannot be distinguished; source/cooked/runtime identity forks; a stale/partial generation can publish; an excluded feature becomes reachable; or the selected check cannot detect its seeded defect.

## Cross-Stage Invariants

1. The accepted semantic revision and working-space identity travel with settings, cooked bytes, runtime state, captures, and evidence.
2. Source, cooked, resident, requested, resolved, active, View, frame, backend, and device generations remain distinguishable.
3. Only complete current generations publish; reset/cancel makes older completions permanently ineligible.
4. Neutral parameters plus no LUT omit grade work exactly.
5. Scene grade, target tone/gamut mapping, output encoding, debug policy, UI composition, and presentation stay separate authorities.
6. One Renderer semantic implementation serves D3D12 and Vulkan; RHI contains no grade policy.
7. Temporary probes and fixtures remain local-only unless the user separately authorizes submitted test additions.
8. No stage adds volume blending, masks, curves, multiple LUTs, OCIO runtime, arbitrary packaged file loading, or player-facing controls.
9. An earlier stage's evidence is invalidated whenever a later change alters its semantic, identity, ordering, representation, or owner assumptions.
10. Every handoff records current revision, dirty boundary, exact checks/results, unrun checks, limitations, and deletions.

## Stage Map

| Stage | Result | Prerequisites |
| --- | --- | --- |
| 0 | `CGRD-00` decision package | source/research access |
| 1 | first parameter-slice contract/check/file freeze, documentation only | Stage 0 `PASS`, contributing display domains |
| 2 | parameter-only scene-referred grade | Stage 1 |
| 3 | typed `.cube` source-to-cooked contract | accepted LUT semantics and asset-owner decision |
| 4 | residency join and LUT-applied grade | Stages 2-3 |
| 5 | editor, capture, two-view, and automation experience | Stage 4 |
| 6 | backend/package/performance evidence and FCR handoff | Stage 5 and release candidate identity |

## Stage Contract Matrix

<details>
<summary>Expand stage contract matrix</summary>

| Stage | Required inputs | Primary owned output | Non-goals | Minimum falsifiers |
| --- | --- | --- | --- | --- |
| 0 | current source trace, pinned research, template contract | exact accepted dossier revisions and `CGRD-00` result | production code or readiness credit | `CHK-CGRD-01` through `10` and independent review |
| 1 | accepted semantics and current settings/View/display owners | reviewed producer-to-output state/binding/check map | production carriers, registrations, inactive seam | actual consumer, neutral/failure oracle and bounded file/deletion list |
| 2 | accepted parameter math/graph edge and Stage-1 map | complete settings/View/typed-binding/shader route producing one parameter-only grade | LUT/source/cook, fusion, output-profile policy | CPU/GPU hand cases, defect mutations, alpha/stage/extent, backend shader/native checks |
| 3 | accepted grammar/layout/capacity/provenance | deterministic typed source-to-cooked artifact | runtime upload/render, extra formats, OCIO dependency | valid/malformed/hostile corpus, deterministic recook, dependency discovery, transaction failure |
| 4 | proved parameter pass and cooked artifact | immutable resident LUT generation applied by the one grade pass | editor polish, multiple/blended LUTs | known cells/interiors, stale completion, OOM/device failure, two Views, retirement high-water |
| 5 | stable runtime request/result contract | complete editor/manifest/capture/support workflow | player UI, file browser, color suite | first use, every state/action, rapid edit/cancel/reset, accessibility, automation parity |
| 6 | frozen candidate and predeclared manifests | exact acceptance evidence and clean closure | threshold tuning or feature expansion | all `CHK-CGR-*`, seeded faults, package/clean-machine, paired backend, cost, exclusion audit |

Every stage is independently reviewable. Its report must list production files changed, generated/build/package membership, old paths deleted, local-only probes created/removed, acceptance IDs advanced, and any downstream evidence invalidated.

</details>

## Stage 0 — Close Discovery

**Work:** execute `CGR-EXP-01` through `07`; disposition `CGRD-01` through `12`; freeze semantic, architecture, experience, budget, evidence, rights, and deletion revisions. Reconcile target wording when decisions change.

**Exit:** all `AC-CGRD-*` pass, no implementation-shaping unknown remains, and the accepted document revisions and stage addition/retirement budgets are named. Remove disposable experiments and superseded decision instructions; no production carriers or new planning framework land. Otherwise record `Blocked` with the exact owner and next experiment.

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/ColorGrading/ImplementationPlan.md#stage-0--close-discovery; apply its execution contract, named stage work/exit and Stage Source-Style Gate.
Execute only Color Grading Stage 0. Do not change production code. Inspect current Renderer/display/settings/shader/asset/cook/residency/editor/capture/package owners; run the seven discovery experiments; freeze every CGRD decision, risk, fixture, tolerance, budget, and invalidation trigger. Update only the owning feature documents. Stop rather than choosing working space, negative math, LUT grammar/layout/interpolation, ownership, UX, or evidence thresholds by convenience.
NON-NEGOTIABLE: production remains untouched, every implementation-shaping unknown is closed/excluded/blocked explicitly, and the report names exact accepted revisions or records `Blocked`.
```

</details>

## Stage 1 — Freeze The First Parameter Slice

**Work:** finish the accepted grade parameters, selected LUT boundary, settings/View identity, graph edge, numeric oracle and actual file/build map. This is a documentation/check-design slice. Do not add public types, hashes, result carriers, shader registrations or inactive graph seams without a real consumer. Reuse current settings, publication and display contracts.

**Exit:** the first effect slice has a reviewed exact producer-to-output change and neutral/failure oracle; no dormant production vocabulary has been added.

```text
Execute only ColorGrading Stage 1 from Docs/Architecture/Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/ColorGrading/ImplementationPlan.md. Freeze the concrete first-slice contract and checks without production changes. Carry required state, shader and build changes into Stage 2 with their real consumer; do not land an inactive framework. Apply the owning execution contract and its Stage Source-Style Gate.
```

## Stage 2 — Deliver The Parameter-Only Grade

Stage 2 lands the complete settings/View/typed-binding/shader/build route with its first actual effect output; it also deletes replaced state or intermediates. Stage 1 supplies decisions, not a separate inactive implementation.

**Work:** implement accepted SOP and saturation semantics in one shared HLSL owner, checked by a local independent CPU oracle; insert the distinct scene-referred pass at the accepted boundary; preserve alpha/product identity and exact debug policy. Reuse current settings/publication state, add only parameters consumed by this transform and delete replaced parameter/transform/intermediate authority now. LUT fields, resource state and registrations wait for their own consumer. Exercise identities, edge values, order mutations, non-finite inputs, backend shader compilation, and output-resolution extents.

**Exit:** `AC-CGR-01/02/05` parameter portions and `FM-CGR-01/03` pass; seeded wrong order, weights, negative rule, channel, alpha, and double-application defects are detected; cost remains within the Stage-0 budget. One consumed parameter route/pass remains, with no LUT scaffolding, shadow settings or permanent oracle code; retained display/raw behavior and deletion searches pass.

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/ColorGrading/ImplementationPlan.md#stage-2--deliver-the-parameter-only-grade; apply its execution contract, named stage work/exit and Stage Source-Style Gate.
Execute only Color Grading Stage 2. Implement the accepted CGR-MATH parameter transform once, after the accepted reconstruction/exposure boundary and before target tone mapping. Use typed binding and an independent double-precision oracle. Keep LUT absent. Exercise neutral omission, analytic colors/ramps/negative/HDR/non-finite values, alpha sentinel, wrong-order/weight/rule defects, debug classifications, output extents, D3D12/Vulkan shader paths, native validation, and pass cost. Stop on any semantic or stage ambiguity.
NON-NEGOTIABLE: one accepted semantic transform, one exact stage, independent defect-detecting values, no LUT/fusion, and unchanged unrelated histories/SDR behavior.
```

</details>

## Stage 3 — Add The Typed LUT Source And Cooked Contract

**Work:** extend the owning asset/cook route with the accepted bounded `.cube` subset, canonical representation, source hash, schema/semantic identity, transactional write, deterministic recook, errors, and package dependency discovery. Reuse existing dependency/publication/error mechanisms; remove prototype parsers, source-to-runtime loaders and replaced cooked writers in this slice. No format plug-in registry or future-format schema is selected. Use parser/math probes locally; do not add submitted test files unless separately authorized.

**Exit:** valid fixtures produce deterministic bytes; malformed/hostile/unsupported cases fail before publication/allocation; dimension/domain/count/axis data are unambiguous; package planning sees the logical dependency; old schema paths do not survive.

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/ColorGrading/ImplementationPlan.md#stage-3--add-the-typed-lut-source-and-cooked-contract; apply its execution contract, named stage work/exit and Stage Source-Style Gate.
Execute only Color Grading Stage 3. Add the accepted .cube source parser and canonical cooked grade-LUT format at the existing asset/cook owner. Enforce checked counts and byte/dimension limits, finite/domain/directive rules, deterministic bytes, provenance, transactional publication, and clear errors. Exercise identity/asymmetric known-cell plus malformed/truncated/extra/non-finite/overflow/unsupported fixtures. Do not upload, render, embed OCIO, or accept extra formats.
NON-NEGOTIABLE: every token/count/allocation is bounded before publication, cooked identity is deterministic, runtime source loading is absent, and no unaccepted grammar/schema survives.
```

</details>

## Stage 4 — Join Residency And Apply The LUT

**Work:** load/validate the cooked contract, reuse the existing residency/upload/retirement path, bind the immutable LUT generation, and implement accepted domain/axis/texel/interpolation/composition semantics in the existing grade pass. Delete temporary direct-upload handles, duplicated requested/active state and replaced sampling branches; do not create a grade cache, uploader, job system or second pass. Exercise stale completion, upload/capacity/device failure, repeated replacement, two views, and CPU/GPU known-cell parity.

**Exit:** `AC-CGR-03/04/06` runtime portions pass; no partial/stale generation activates; requested/active identities stay truthful; LUT application detects axis, half-texel, interpolation, domain, order, and stale-generation defects. Current residency owns lifetime and the one grade pass consumes the LUT; replaced handles/state/branches and local probes are gone.

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/ColorGrading/ImplementationPlan.md#stage-4--join-residency-and-apply-the-lut; apply its execution contract, named stage work/exit and Stage Source-Style Gate.
Execute only Color Grading Stage 4. Reuse existing residency to publish a checked cooked LUT generation and bind it to the one grading pass. Implement the accepted LUT mapping/composition exactly. Exercise known asymmetric cells and interpolation points, boundary/out-of-domain values, missing/corrupt cooked data, upload/OOM/capacity/device loss, randomized replacement completion, two views, and delayed GPU retirement. Preserve the explicit last-good/default state and never claim the pending request active.
NON-NEGOTIABLE: only complete current immutable generations activate, CPU/GPU axis/domain/filter/order agree, failures are transactional, and retirement follows real completion identity.
```

</details>

## Stage 5 — Complete The Development Experience

**Work:** implement the [User Experience](UserExperience.md) through the current rendering settings/editor surface, source asset selection, state/error presentation, reset, live edit/cancel, capture metadata, and noninteractive manifest. Present existing runtime truth; remove replaced UI/config/status copies instead of adding a grade read-model, workspace or observer registry. No player-facing menu or file browser is admitted.

**Exit:** first use, all control states, keyboard/text access, rapid edit, failure/recovery, reset, two-view isolation, capture lineage, and editor/manifest equivalence pass; no console-only step, silent substitute or competing UI/settings authority remains. The actual UI/API/copy delta fits the frozen stage budget.

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/ColorGrading/ImplementationPlan.md#stage-5--complete-the-development-experience; apply its execution contract, named stage work/exit and Stage Source-Style Gate.
Execute only Color Grading Stage 5. Add the accepted DevelopmentEditor controls and automation fields over the existing settings and active-state owners. Show semantic domain, neutral values, LUT metadata/identity, requested versus active/pending/unavailable reason, and reset. Exercise first use, invalid source, recook/upload failure, rapid edits/cancel, two views, capture metadata, package-missing state, accessibility, and manifest equivalence. Do not create a color-suite, volume system, runtime player UI, or alternate state cache.
NON-NEGOTIABLE: visible/editor/automation truth derives from the same runtime owners, reset reaches exact omission, no silent substitute exists, and excluded controls/workflows remain unreachable.
```

</details>

## Stage 6 — Evidence, Package, And Close

**Work:** run all predeclared `CHK-CGR-*` against the exact candidate; compare D3D12/Vulkan raw stage products, SDR/HDR joins, package operation, memory/cost, clean machine, and excluded-surface audit using current tools. Remove local probes and verify preceding stages' replacement deletions and addition budgets; audit CMake, generated shaders, assets, package allowlists, rights, docs, and dirty work. A production residue/defect returns to its owner and invalidates affected proof; this stage adds no runner, report framework or calibration product. Submit `FCR-REN-24`.

**Exit:** all applicable `AC-CGR-01` through `08` pass conjunctively, every failure/risk has detecting evidence, all excluded surfaces remain absent, and the acceptance owner records the real verdict. Documentation, a successful build, or one attractive screenshot cannot close the feature.

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/Modules/Engine/Renderer/Features/PostProcessing/DisplayPipeline/ColorGrading/ImplementationPlan.md#stage-6--evidence-package-and-close; apply its execution contract, named stage work/exit and Stage Source-Style Gate.
Execute only Color Grading Stage 6 for one frozen candidate. Run CHK-CGR-01 through 06 with seeded defect controls, raw pre/post-grade products, backend comparisons, two-view and replacement stress, package/clean-machine route, cost/memory, capture lineage, and excluded-feature audit. Remove temporary test-only code and reconcile source/header/shader/generated/CMake/asset/package/docs membership. File FCR-REN-24 with exact PASS/BLOCKED/EXCLUDED evidence and limitations; do not tune thresholds after candidate output.
NON-NEGOTIABLE: one immutable candidate/manifest/threshold set, all applicable criteria conjunctive, all temporary probes removed, and any missing/mismatched evidence produces `Blocked` rather than a partial score.
```

</details>

## Phase-To-Acceptance Traceability

<details>
<summary>Expand phase-to-acceptance traceability</summary>

| Acceptance claim | Earliest establishing stage | Closing stage/checks | Invalidated by |
| --- | --- | --- | --- |
| `AC-CGR-01` exact neutral/omission | 2; contract freeze in 1, re-proved with LUT in 4 | 6: `CHK-CGR-01/02/04` | neutral predicate, pass topology, representation, or fallback change |
| `AC-CGR-02` parametric semantics | 2 | 6: `CHK-CGR-01/04` | working domain, order, range, luma, precision, shader/compiler change |
| `AC-CGR-03` typed LUT/correct sampling | 3 source/cook; 4 GPU application | 6: `CHK-CGR-03/04` | grammar/schema/layout/interpolation/format/sampler change |
| `AC-CGR-04` View state and isolation | 2 state; 4 resident generation; 5 workflow | 6: `CHK-CGR-04/05` | precedence/digest/publication/retirement/settings change |
| `AC-CGR-05` exact stage/domain | 2; re-proved with LUT in 4 | 6: `CHK-CGR-02/04` | graph ordering, debug/capture, reconstruction/exposure/tone join change |
| `AC-CGR-06` transactional failures | 2 refusal, 3 parser/cook, 4 runtime, 5 user recovery | 6: `CHK-CGR-03/05` | any failure/fallback/result-vocabulary change |
| `AC-CGR-07` backend/package/cost | incremental measurements 2-5 | 6: `CHK-CGR-04/05` | shader/RHI/package/capacity/candidate change |
| `AC-CGR-08` controls/exclusions | 0 contract; 5 reachability | 6: `CHK-CGR-06` | public/editor/manifest/source/package/docs surface change |

</details>

## Deletion And Preservation Ledger

| Surface | Preserve | Delete before stage handoff |
| --- | --- | --- |
| tone mapping/output encoding/debug/capture owners | their separate contracts and current behavior unless explicitly updated by owner | temporary grade aliases or duplicated transform branches |
| settings/View route | existing default/override and immutable frame preparation patterns | experimental grade singleton, shadow config, observer registry |
| asset/cook/package | existing dependency, transaction, and generated-artifact mechanisms | prototype direct filesystem/runtime loader, old schema reader/writer, copied source artifact |
| residency/RHI | existing upload, generation, completion, and retirement mechanisms | grade-specific RHI policy or editor-owned resource handles |
| shader/program catalog | typed registration/generation path | ad hoc string binding, unchecked null/identity descriptor workaround |
| evidence | useful original manifests/artifacts and negative results under their exact candidate identity | local probes, temporary fixtures/classes/executables and disposable output with no retained proof consumer |
| repository | unrelated user-owned dirty work | only superseded paths introduced or explicitly replaced by this feature |

## Stop And Escalation Rules

Stop the current stage and record `Blocked` when:

- a required `CGRD-*` disposition, owner, semantic constant, capacity, tolerance, or fallback rule is missing;
- current source contradicts the accepted owner/edge and resolving it would broaden the stage;
- another View, temporal history, output profile, or presentation owner must change beyond the accepted contract;
- a source/cooked/runtime identity cannot be made stable without a new compatibility or global-cache system;
- input can cause unchecked work/allocation, unbounded diagnostics, partial publication, or path escape;
- the independent oracle shares production parsing/layout/math or a seeded defect is not detected;
- one backend/package requires different color semantics rather than a mechanism adapter;
- a clean-break deletion would touch external/user-owned data whose version/scope has not been classified;
- concurrent changes overlap an owned path and cannot be reconciled without changing intent;
- the smallest relevant check is unavailable and the unresolved claim is necessary for the stage exit.

Escalation returns to the narrowest owner: Discovery for policy, Semantics for math/data interpretation, Execution Architecture for ownership/lifetime, User Experience for observable behavior, release planning for product scope, or Acceptance for evidence disposition. Do not solve the ambiguity inside code or by weakening a check.

## Completion Rule

This plan completes only when `FCR-REN-24` is decided against one immutable candidate and evidence set. A parameter-only pass is not LUT completion; a valid LUT parser is not runtime activation; a visually pleasing image is not colorimetric proof; and an identity fallback is never the requested grade.

## Reference And Ownership

<details>
<summary>Exact owners, source snapshots and related contracts</summary>

**Responsibility:** order discovery, semantics, ownership, parametric grade, LUT workflow, editor/game integration, evidence, packaging, and closure for `FCR-REN-24`

**Authority boundary:** [Discovery](Discovery.md) authorizes the accepted design revision; [README](ColorGradingDesign.md) owns feature acceptance; [Display And Reconstruction](../../../../FirstRelease/DisplayAndReconstructionImplementationPlan.md#dsp-5--color-grading) owns release-wide order; this plan owns only feature-local delivery

</details>
