# Graphics Workload Study Delivery Plan

[Docs](../../../../README.md) / [Up](WorkloadStudiesDesign.md)

Stages: [WS-S0](#ws-s0---freeze-discovery) / [WS-S1](#ws-s1---one-analysis-consumer) / [WS-S2](#ws-s2---causal-paired-api-studies) / [WS-S3](#ws-s3---incident-and-technical-handoff) / [WS-S4](#ws-s4---close-documented-study-reproduction).

Produce the classical technical handoff at M3, then close independent study/incident reproduction at M6 using the same original evidence.

**Status:** remaining discovery, analysis and study work; no calendar or capacity estimates

**Prerequisites:** `REL-11` before new production tooling; correct workload/reference prerequisites for `M3/WL-04`; [WS-D0](WorkloadStudiesDesign.md#discovery-gate-ws-d0) before implementation. Existing release/capture repairs retain their own authorization; this plan cannot bypass the parent diagnostics `P1-GATE` to add internal collectors.

## Work This Plan

| What to do | Working route |
| --- | --- |
| First action | [WS-S0 freeze](#remaining-stages): select actual producer records; if a necessary fact is missing, freeze and route its smallest producer prerequisite, then close WS-D0 before building an analyzer. |
| Result to reach | M3: analyzer, three causal studies and the cleared incident/replay packet with local checks. M6: actual documented reproduction closes AC-WS-04; it does not block the neural work needed by that final exercise. |
| Start / stop | New production tooling waits for REL-11, WS-D0 and workload/reference inputs. This route cannot bypass parent capture gates or invent a collector. |

## Stage Contract

Every stage and copied prompt follows [stage completion and resume](../../../../Engineering/Workflow/ChangeLifecycle.md#stage-completion-and-resume). Close its finite local exit once; keep later aggregate proof separate.

Apply [Change Lifecycle](../../../../Engineering/Workflow/ChangeLifecycle.md), [Change Integration](../../../../Engineering/Workflow/ChangeIntegration.md) and the [Feature Delivery Package](../../../../Engineering/Workflow/Templates/FeatureDeliveryPackage.md). Freeze prerequisite revisions, file allowlist, consumer/deletion/hook/copy budget, AC/FM/CHK and numeric rules before results. Before every implementation handoff enforce [Stage Source-Style Gate](../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate), applicable formatting/logical-spacing checks, narrow owner validation and `git diff --check`; boundary checks only for an actual Renderer/RHI boundary change. Remove disposable probes.

## Subject Delivery Owners

This parent owns WS-D0, the one analysis consumer and aggregate CASE/WL joins. The child plans own their actual studies; WS-S2/3 below delegate rather than repeat their implementation work.

| Selected evidence | Implementation owner |
| --- | --- |
| Python analysis consumer / PGD-05 | [WS-S1](#ws-s1---one-analysis-consumer), [fixed command/input/output proposal](WorkloadStudiesDesign.md#bounded-offline-analysis-consumer) |
| CPU / GPU / lifetime / PGD-03 | [CPU plan](CpuPreparation/ImplementationPlan.md), [GPU plan](GpuShader/ImplementationPlan.md), [lifetime plan](ResourceLifetime/ImplementationPlan.md) |
| Causal incident / PGD-04 | [Incident plan](IncidentAnalysis/ImplementationPlan.md) |
| Original artifacts and final reproduction | [Workload evidence homes](../../../../Acceptance/GraphicsWorkloads.md#evidence-folders-and-delivery-owners); [portfolio stage 3](../../GraphicsPortfolio/ImplementationPlan.md#3-reproduce-review-and-revise) owns the real exercise |

## Remaining Stages

| Stage | Work and exit | Deliberate scope and stop |
| --- | --- | --- |
| [`WS-S0`](#ws-s0---freeze-discovery) Freeze discovery | Close five `WS-D0` cells, actual producer sample, statistics/observer/size cards, API/hardware matrix and current-script retain/narrow/delete decisions. If a necessary field is absent, freeze its producer/check card, route the admitted prerequisite through parent Phase 4, qualify the original record and return here. | WS-S0 stays documentation/local probe only; the producer uses its own bounded stage, not the still-blocked analyzer gate. Remove probes; no guessed data/schema, duplicate collector or full internal spine. Missing unowned inputs/thresholds still block WS-D0. |
| [`WS-S1`](#ws-s1---one-analysis-consumer) One analysis consumer | Implement only the selected offline CLI over frozen workload records; publish per-run tables/plots and source joins. Pass `AC-WS-01`, input/failure/cleanup parts of `AC-WS-04`, `AC-PY-01..04`. | No collector/export/history or Python framework. Delete the replaced ad hoc supported analysis route; original evidence stays intact. |
| [`WS-S2`](#ws-s2---causal-paired-api-studies) Causal paired-API studies | Execute only the first unfinished admitted stage in the [CPU](CpuPreparation/ImplementationPlan.md), [GPU](GpuShader/ImplementationPlan.md) or [lifetime](ResourceLifetime/ImplementationPlan.md) owner. Parent WS-D05 selects all three questions before results; each child supplies model/baseline/experiment/paired verdict. | Join all three domain results under AC-WS-02/WL-04/07. Do not duplicate child steps or demand three wins; preserve their rejected/inconclusive evidence, actual tool limits and same-stage cleanup. |
| [`WS-S3`](#ws-s3---incident-and-technical-handoff) Incident and technical handoff | Use the [incident plan](IncidentAnalysis/ImplementationPlan.md) for missing causal/local replay proof, then join the analyzer and all three study packets. | M3 admits M4 with AC-WS-01..03 and local input/failure/cleanup controls from 04; rehearsal 04 remains open for WS-S4 at M6. No adoption verdict is inferred. |
| [`WS-S4`](#ws-s4---close-documented-study-reproduction) Documented reproduction closure | At M6, consume the actual [portfolio stage 3](../../GraphicsPortfolio/ImplementationPlan.md#3-reproduce-review-and-revise) observations for the frozen study/analysis and incident packet. | Close AC-WS-04/CHK-WS-04 only for actual supported results; keep unavailable cells open. Reuse one exercise instead of another adopter per child. |

## Reference-Guided Study Selection

Use the [catalog's exercises](../../../../Strategy/FeatureDeliveryCatalog.md#research-informed-delivery-exercises) and [pinned source cards](../../../../Strategy/Research/RenderingReferenceExamples.md) to choose hypotheses in WS-S0. The domain designs own the actual CPU, shader and lifetime protocols; source examples neither select another feature nor supply local thresholds.

Before results, record the exact source/assumption, local differences, rights and falsifier through the existing [reference-use record](../../../../Strategy/Research/RenderingReferenceExamples.md#delivery-reference-use-record). Keep rejected/inconclusive evidence and actual native-tool limits. No copied sample architecture, mandatory vendor feature or three-success requirement follows from the examples.

## Ready-To-Use Prompts

### WS-S0 - Freeze Discovery

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md#ws-s0---freeze-discovery and apply its Stage Contract, including the Stage Source-Style Gate.
Execute WS-S0 only in Docs/Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md. Research/planning only. Inspect actual runtime/native records, GraphicsWorkloads, diagnostics comparison rules, Python dossier and current tools. Prefer existing records. If a required field is absent, freeze its minimal producer/check/observer/capacity card and hand off the prerequisite to parent Phase 4 under its own admission, without waiting for completed analyzer WS-D0; resume this freeze after that real record is qualified. Close WS-D01..05 with exact source/sample, units/identity/schema, missing-data rules, numeric/observer bounds and hook/copy/deletion ledger. Preserve REL-11 and parent gates. No production code, guessed exporter or internal diagnostics ladder in WS-S0; report the exact next owner/action while blocked.

Select one actual question in each StudyProtocols.md domain: WS-CPU, WS-GPU and WS-LIFE. Freeze each cost model, native observation, source/IR/instruction or timeline/retirement joins, falsifiers and available tool-access limits. For each selected NVR card record exact source, local differences, rights and measurable output before results. Do not mandate a vendor feature or manufacture a defect.
```

</details>

### WS-S1 - One Analysis Consumer

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md#ws-s1---one-analysis-consumer and apply its Stage Contract, including the Stage Source-Style Gate.
Execute WS-S1 only after accepted WS-D0 and production admission. Ratify and implement the bounded command/input/output/failure contract in WorkloadStudiesDesign.md, using actual frozen producer samples and exact versions/capacity rules; the planned command is not current source. Extend the existing Tools owner with one selected offline analysis CLI and real consumer. Reuse workload schemas; preserve run-level distributions and candidate/frame/settings/native-artifact joins. Reject partial/mismatched/missing/oversized inputs, preserve originals and clean interrupted publication. Run frozen AC-WS-01/04 and AC-PY controls, code style and narrow owner checks. Delete replaced supported analysis paths and remove probes. Stop on unowned schema, lost identity or duplicate collector/export. No runtime Python or public framework. Handoff exact commands/results/limitations.
```

</details>

### WS-S2 - Causal Paired-API Studies

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md#ws-s2---causal-paired-api-studies and apply its Stage Contract, including the Stage Source-Style Gate.
Execute only the first unfinished admitted stage in this plan's CpuPreparation, GpuShader or ResourceLifetime ImplementationPlan.md. Apply that child's exact prerequisites, design/check card, model, baseline, controlled experiment, paired verdict and cleanup. This parent joins the three original results under AC-WS-02 and relevant WL cells; it does not redefine their protocols. Preserve rejected/inconclusive results and actual tool/quality/observer limits. No forced topology change, duplicated study runner or feature expansion.
```

</details>

### WS-S3 - Incident And Technical Handoff

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md#ws-s3---incident-and-technical-handoff and apply its Stage Contract, including the Stage Source-Style Gate.
Use IncidentAnalysis/ImplementationPlan.md for the first missing causal/local replay stage; reuse valid original release evidence. Join the analyzer, all three study domains and incident/replay packet with AC-WS-01..03 and the local failure/cleanup portion of 04. This is the M3 technical handoff for M4; documented reproduction is not its prerequisite. Keep AC-WS-04's rehearsal result open for M6 / WS-S4. Verify cleanup and retain original identities/negative conclusions; no new runner, adopter or report portal.
```

</details>

### WS-S4 - Close Documented Study Reproduction

**Start:** M3 technical packets and the relevant M6 neural technical/package inputs are qualified; portfolio stage 2 has assembled the cleared package. Final study/neural adoption is not an entry prerequisite.

**Work:** Use the actual portfolio stage 3 exercise to reproduce the selected study/analysis headline and trace original incident/CASE evidence. Record environment, prior involvement, every intervention, failures and actual outputs. Reuse only observations that exercise AC-WS-04/CHK-WS-04; a content-only launch cannot close this cell. A repair returns to its existing owner and invalidates affected proof.

**Cleanup / exit:** Close remaining AC-WS-04 and aggregate study acceptance only after the actual documented result; retain earlier valid local checks when this is blocked. Verify prior deletions, remove disposable exercise code and keep original artifacts. No second exercise/participant per study, new support service or separate readiness store.

<details>
<summary>Copy implementation prompt</summary>

```text
Read Docs/Architecture/CrossModule/PerformanceDiagnostics/WorkloadStudies/ImplementationPlan.md#ws-s4---close-documented-study-reproduction and apply its Stage Contract, including the Stage Source-Style Gate.
Execute WS-S4 only after the technical packets and cleared M6 assembly exist. Obtain the relevant actual portfolio stage 3 observations for AC-WS-04/CHK-WS-04; record original input/candidate identity, rehearsal actor/environment, interventions, failed steps and output. Final adoption is an output, not a prerequisite. Reuse one exercise, route defects to existing owners, requalify changed claims and remove probes. No unexecuted result claims or extra rehearsal per folder or unauthorized external action; absent reproduction stays BLOCKED.
```

</details>

Every prompt inherits this non-negotiable exit: one producer/schema/identity authority; source and native artifact joins; frozen statistical and observer policy; quality-equivalent comparisons; honest missing/inconclusive results; no duplicate live mechanisms; bounded failures and code style. Quote each clause with retained proof or `BLOCKED`. Research and documentation do not count as executed studies.

## Reference And Ownership

<details>
<summary>Exact owners, source snapshots and related contracts</summary>

**Authority:** [workload-study contract](WorkloadStudiesDesign.md), [Graphics Workloads](../../../../Acceptance/GraphicsWorkloads.md), [Diagnostics Acceptance](../Acceptance.md) and [Python dossier](../../../Modules/Tools/PythonAutomationAndAnalysis.md). Stage results belong to existing candidate/workload completion reports.

</details>
