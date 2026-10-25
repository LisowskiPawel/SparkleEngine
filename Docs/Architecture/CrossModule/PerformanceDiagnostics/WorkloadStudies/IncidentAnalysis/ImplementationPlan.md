# Difficult Graphics Incident Delivery Plan

[Docs](../../../../../README.md) / [Up](IncidentDesign.md)

Turn one actual failure into a causal, reproducible incident case without building a diagnostics product.

**Status:** remaining evidence delivery; this plan records no executed study or acceptance result

**Admission:** Existing release incidents reuse their repair gate. New study/tool implementation waits for REL-11 and WS-D0; final incident/transfer evidence joins M3/M6.

## Work This Plan

| What to do | Working route |
| --- | --- |
| First action | [INC-0](#inc-0-freeze-the-failed-candidate-and-question): bind the actual bad candidate and first discriminating observation; reuse completed evidence where valid. |
| Result to reach | A causal original-bad versus fixed result, reduced replay, scoped report and real independent review under AC-WS-03/04. |
| Start / stop | Planning can prepare decisions earlier; execution needs the exact candidate, valid prerequisites and frozen check cards. Missing rights, tools, or external participants keep the affected result open. |

## Execution Contract

Use the [owning design](IncidentDesign.md) and execute only the first unfinished stage below. Inspect current status, source/build owners, producers, consumers, lifetime and valid original artifacts; do not replay accepted work because it appears in a new plan. Production repairs return to their existing feature owner and admission. This evidence plan cannot add a feature, new platform or permanent validation service.

Apply [Change Integration](../../../../../Engineering/Workflow/ChangeIntegration.md), the [stage complexity budget](../../../../../Engineering/Workflow/ChangeLifecycle.md#define-the-outcome-and-complexity-budget) and the [Stage Source-Style Gate](../../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate) after the final edit: exact formatter file coverage, authored spacing/responsibility review and git diff --check. A no-source stage records source-style N/A with its reason and runs the relevant documentation/artifact checks. Check IDs do not authorize permanent fixtures. Evidence work adds no persistent runtime state. Only a demonstrated causal repair can spend a feature-owned addition/deletion budget.

Freeze the candidate, workload/API/profile, units, numeric rule, uncertainty, falsifier, output limits and cleanup before candidate observations. Preserve original evidence and remove experiment toggles/probes and replaced supported paths in their producing stage. Record exact commands, tools, results, limits and next permitted action in the existing result owner; no result or grade is awarded here.

## Remaining Stages

### INC-0: Freeze The Failed Candidate And Question

**Start:** A real incident and its existing evidence/repair admission are identified.

**Work:** Reproduce or independently inspect retained bad output; bind candidate/settings/symbols and expected product. Trace the current owner and smallest safe reduced input. Choose reuse versus remaining investigation; do not reopen a finished repair without a missing claim.

**Cleanup / non-delivery:** Planning only unless the current release gate already authorizes the needed reproduction. No guessed diagnosis, production fault toggle or new tool.

**Exit:** One symptom, bad/good boundary, actual reproduction route, rights and first falsifier are explicit. The Execution Contract's responsibility, cleanup and source-style checks apply before handoff; an unavailable required falsifier leaves this exit open.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only INC-0 from Docs/Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/IncidentAnalysis/ImplementationPlan.md#inc-0-freeze-the-failed-candidate-and-question. Apply its Execution Contract, including the Stage Source-Style Gate and stage-local cleanup.
Freeze one real failed candidate and symptom, original artifacts, expected oracle, current source/native identity and minimal reproduction. Reuse accepted release work; stop on missing bad evidence or invented root cause.
NON-NEGOTIABLE: bind observations to original candidate/input/artifact identity; freeze rules before results; preserve the selected output and one production owner; report missing/failed/inconclusive evidence honestly. Quote each applicable clause with proof or BLOCKED. Handoff exact commands, controls, artifacts, deletions, limitations and next permitted stage. Do not execute neighboring stages or expand the engine to populate a report.
```

</details>

### INC-1: Discriminate Competing Causes

**Start:** INC-0 inputs and required tool/symbol access are valid.

**Work:** Order at least two hypotheses by the next separating observation. Find the earliest incorrect producer product, trace source/cook/native identity and test a counterexample through existing debug/capture tools. Retain failed and refuted hypotheses.

**Cleanup / non-delivery:** Delete diagnostic probes at the owner boundary; keep original logs/captures. No global logging, second capture service or speculative API/driver fix.

**Exit:** The causal conclusion separates alternatives at its stated scope, or remains honestly unproved with a next discriminating action. The Execution Contract's responsibility, cleanup and source-style checks apply before handoff; an unavailable required falsifier leaves this exit open.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only INC-1 from Docs/Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/IncidentAnalysis/ImplementationPlan.md#inc-1-discriminate-competing-causes. Apply its Execution Contract, including the Stage Source-Style Gate and stage-local cleanup.
Investigate the first wrong product using at least two hypotheses and separating controls. Preserve candidate/symbol/frame joins and negative observations. Stop before a repair if the cause is still correlation.
NON-NEGOTIABLE: bind observations to original candidate/input/artifact identity; freeze rules before results; preserve the selected output and one production owner; report missing/failed/inconclusive evidence honestly. Quote each applicable clause with proof or BLOCKED. Handoff exact commands, controls, artifacts, deletions, limitations and next permitted stage. Do not execute neighboring stages or expand the engine to populate a report.
```

</details>

### INC-2: Prove The Minimal Owning Repair

**Start:** INC-1 proves the mechanism and the owning feature gate admits the bounded fix.

**Work:** Repair or reuse the current fix in its production owner. Exercise the original bad counterfactual and fixed oracle, affected native/lifetime/failure controls and required API cells. Record invalidation after changed code/cooked inputs.

**Cleanup / non-delivery:** Delete replaced state/path and disposable regression/injection code in this stage. No compatibility workaround, broad rewrite or permanent test framework.

**Exit:** AC-WS-03/CHK-WS-03 have causal bad-versus-good proof and preserved selected behavior; source/build alone cannot close it. The Execution Contract's responsibility, cleanup and source-style checks apply before handoff; an unavailable required falsifier leaves this exit open.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only INC-2 from Docs/Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/IncidentAnalysis/ImplementationPlan.md#inc-2-prove-the-minimal-owning-repair. Apply its Execution Contract, including the Stage Source-Style Gate and stage-local cleanup.
Prove only the admitted minimal repair in its owner, including original bad/controlled-negative and fixed oracle, native/lifetime preservation and exact artifacts. Remove replaced paths/probes; stop on an unfalsifiable fix or broadened scope.
NON-NEGOTIABLE: bind observations to original candidate/input/artifact identity; freeze rules before results; preserve the selected output and one production owner; report missing/failed/inconclusive evidence honestly. Quote each applicable clause with proof or BLOCKED. Handoff exact commands, controls, artifacts, deletions, limitations and next permitted stage. Do not execute neighboring stages or expand the engine to populate a report.
```

</details>

### INC-3: Publish The Scoped Incident And Replay

**Start:** INC-2 proof or valid historical accepted equivalent is available.

**Work:** Assemble the concise symptom/hypotheses/mechanism/fix/deletion narrative and cleared reduced replay. Link the original CASE-02/FCR artifacts, obtain the parent independent exercise/review and retain interventions and findings.

**Cleanup / non-delivery:** Remove one-off assembly/replay code from submitted source. Verify earlier cleanup and retain unique failure evidence; no issue portal or continuing support commitment.

**Exit:** The incident meets AC-WS-03 and its relevant AC-WS-04 replay; absent external evidence stays open without erasing valid local results. The Execution Contract's responsibility, cleanup and source-style checks apply before handoff; an unavailable required falsifier leaves this exit open.

<details>
<summary>Copy implementation prompt</summary>

```text
Execute only INC-3 from Docs/Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/IncidentAnalysis/ImplementationPlan.md#inc-3-publish-the-scoped-incident-and-replay. Apply its Execution Contract, including the Stage Source-Style Gate and stage-local cleanup.
Deliver the original incident/causal repair packet and cleared replay to the existing CASE-02 and parent reproduction route. Retain actual review/interventions, limitations and deletion proof. Do not invent a reviewer or submit/contact without authorization.
NON-NEGOTIABLE: bind observations to original candidate/input/artifact identity; freeze rules before results; preserve the selected output and one production owner; report missing/failed/inconclusive evidence honestly. Quote each applicable clause with proof or BLOCKED. Handoff exact commands, controls, artifacts, deletions, limitations and next permitted stage. Do not execute neighboring stages or expand the engine to populate a report.
```

</details>
