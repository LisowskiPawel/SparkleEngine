# Graphics Workload Studies

[Docs](../../../../README.md) / [Up](../PerformanceDiagnosticsDesign.md) / [Implementation](ImplementationPlan.md)

**Status:** target workflow and discovery contract; no implemented analysis tool or measured study is claimed

**Scope:** the `PGD-03..05` [strategy outputs](../../../../Strategy/FeatureDeliveryCatalog.md): complementary CPU preparation, GPU shader and resource-lifetime studies, one difficult incident and one useful offline analysis consumer through existing exports and a small script where needed. [Study Protocols](StudyProtocols.md) defines each question, cost model, falsifiers and expected evidence. [Graphics Workloads](../../../../Acceptance/GraphicsWorkloads.md) owns `MAP/WL/CASE` definitions and benchmark schema/policy; [Diagnostics Acceptance](../Acceptance.md) owns comparison and observer semantics. This folder owns only the bounded workflow and its [remaining plan](ImplementationPlan.md).

## Intended Consumer And Boundary

A rendering engineer analyzes immutable workload/native-tool records for one predeclared comparison and retains inspectable per-run tables/plots plus a decision record. Existing tool exports and a small local script are sufficient. No maintained command-line product, live collector, capture-provider state or GPU timing authority is required.

Use actual available runtime or native-tool records. Application owns the existing runtime request/export boundary when needed; a live diagnostics session or serializer is not assumed present or required by the [diagnostics plan](../ImplementationPlan.md). Workloads own experiment intent, schema, sample policy and destinations. Tools owns the offline parser/analysis implementation under [Python Automation And Analysis](../../../Modules/Tools/PythonAutomationAndAnalysis.md). External tools remain under the [profiling runbook](../../../../Engineering/Verification/ExternalProfiling.md). Do not duplicate an exporter, history ring or collector. If a required fact is genuinely absent, select the smallest producer-to-original-record delta through the parent's evidence phase; that supporting slice is permitted, while a universal capture command or whole diagnostics product is not.

## Bounded Offline Analysis Consumer

Start from actual native/runtime exports and standard CSV/JSON/math/plotting tools. Bind input identities, exact analysis command and the calculation in WS-D02/04. Keep a small study-local script and its environment with ignored evidence when that answers the question. Promote a reusable Tools entry point only for demonstrated repeated use; this plan no longer prescribes a new supported CLI, parser framework or runtime dependency.

| Boundary | Required behavior |
| --- | --- |
| Input | Existing workload manifests and their referenced original CSV/native records; preserve run/frame/candidate/API/profile/scene/settings/shader identity, units, observation mode and validity/exclusion reasons. Resolve paths from the owning manifest; reject escaped/missing/hash-mismatched inputs and incompatible joins. Unknown/missing optional measurements stay null/unavailable, never zero. |
| Analysis | Per-run distributions and frozen practical-effect/uncertainty rules from Diagnostics Acceptance, using the same calculation where the three studies need it. No plugin registry, sampler, history database, automatic causal verdict or pooled-tail shortcut. |
| Output | Derivative per-run tables/plots and a short decision summary under the existing study package's `analysis/` folder. Link original inputs with hashes. Valid negative/inconclusive results are successful analysis, not a performance PASS. Existing CASE/WL reports alone accept the claim. |
| Failure | Reject missing/mismatched/invalid input before accepting a comparison. Write derivative output into a fresh operation-owned directory; accept it only after successful completion and output checks. On error or interruption retain the failure and discard only that incomplete derivative output. Original captures and earlier accepted analyses remain untouched; no recovery journal or publication framework. |
| Capacity/dependencies | Declare the actual selected input bytes/rows and analysis peak-memory bound in WS-D04; reject over-bound input without truncating. Read only the needed records/fields using existing tools or standard libraries, with one plotting dependency if needed. Record the actual environment; do not add a general streaming parser or Python runtime to game packages. |

WS-S1 must prove Unicode/relative paths, schema/unit/mixed-settings rejection, null/partial/oversize inputs, deterministic frozen-input output, cancellation and interrupted publication. Freeze a real null/noise comparison and changed-setting negative before a candidate win. Exact schema/metric/bootstrap and installed versions remain technical qualification inputs to WS-D0, not permission to invent a new exporter.

The selected consumer is the study engineer, not a new native ingestion API. AC-PY-03 is satisfied by actual original-input/schema checks and independent reanalysis; it does not authorize a C++ report loader, runtime analysis panel or second results store.

## Discovery Gate `WS-D0`

| Cell | Owner and closure evidence |
| --- | --- |
| `WS-D01` Actual consumer and producer | Workload/Application owners: inspect current launch/capture/export/request routes and select one comparison. Name available fields and missing ones; prefer an existing native/runtime record. For a necessary missing field, freeze its producer/check/observer/capacity card and route the bounded prerequisite through parent Phase 4, plus only a necessary admitted FND producer. No internal spine is presumed. |
| `WS-D02` Input identity and schema | Workload/Tools owners: freeze schema, units, run/frame/scene/settings/candidate join, native artifact namespaces, source/symbol provenance, null/partial/unsupported behavior and size bounds. Reuse the workload schema rather than copying it. |
| `WS-D03` Statistical and observer policy | Workload owner: inherit warm-up/sample/repeat rules; freeze practical-effect bands and correlation-aware uncertainty, independent run treatment, exclusion rules, quality equivalence and observation mode. A profiling capture is not an optimized baseline. |
| `WS-D04` Tool and publication | Tools owner: actual existing tool/script environment and command, input/output paths, deterministic calculations, standard interruption/failure cleanup, selected-input bounds and consumer acceptance. No runtime bindings or tool framework. |
| `WS-D05` Study and architecture budget | Renderer/Tools owners: select one actual question in each [CPU, GPU and lifetime protocol](StudyProtocols.md), including rejected/inconclusive evidence and one real incident. Freeze analytical cost models, falsifiers, hook/copy/deletion ledger, supported tuples and separate review. No manufactured defect or topology win. |

`WS-D0` passes only with a reviewed source/dossier revision, a valid producer sample, numeric decision cards and the exact narrow consumer. If a selected claim lacks an input, WS-S0 first freezes that producer's narrow contract/check card; the admitted [parent Phase 4](../ImplementationPlan.md#phase-4--deliver-reproducible-evidence) can deliver it without waiting for the analyzer's completed WS-D0. Qualify the actual record, then return to WS-S0 and freeze the consumer. This breaks a sample-before-producer cycle without inventing data or admitting the internal diagnostics ladder. Existing parent gates and REL-11 still govern new production work; native records remain preferred where sufficient.

## Technical Handoff And Final Reproduction

M3's technical handoff uses WS-S0..3: AC-WS-01..03, the three complementary original study records, cleared incident replay and local input/failure/cleanup checks from 04. This admits the dependent neural work. AC-WS-04's real rehearsal result remains required at M6 / WS-S4 after portfolio assembly; it cannot be an M3 prerequisite because that assembly also needs the neural result. Full study acceptance still requires all four criteria.

## Local Acceptance And Failures

| ID | Observable criterion and check |
| --- | --- |
| `AC-WS-01` / `CHK-WS-01` | Valid frozen inputs reproduce per-run tables/plots and source joins on repeat; mismatched API/settings/schema/symbol or partial records visibly reject, never synthesize zero metrics. |
| `AC-WS-02` / `CHK-WS-02` | One study in each [CPU, GPU and lifetime protocol](StudyProtocols.md) joins its analytical prediction to matched native observations, baseline/candidate quality, uncertainty and whole-frame context; at least one rejected/inconclusive result is retained. GPU evidence includes exact shader/IR identity and available instruction/counter correlation, with access limits declared. Reanalyze null/noise and changed-setting negatives. Three variants of one shader question do not cover all three domains. |
| `AC-WS-03` / `CHK-WS-03` | One incident has a reproducible symptom, earliest incorrect product, competing/refuted hypotheses, bounded repair and a defect-detecting negative/regression route. Successful launch or an attractive image alone fails the check. |
| `AC-WS-04` / `CHK-WS-04` | The author or agent follows the documented study instructions and traces headline results to original artifacts; interrupted/oversized/missing inputs produce bounded failure and cleanup. Record the actual actor and environment. |

`FM-WS-01` identity/schema mismatch maps to `CHK-WS-01`; `FM-WS-02` changed settings, incomplete observer or invalid statistics maps to `CHK-WS-02`; `FM-WS-03` an unproved incident root cause maps to `CHK-WS-03`; `FM-WS-04` partial publication or missing adopter maps to `CHK-WS-04`. Failures produce an explicit rejection/inconclusive/unrun disposition and preserve original inputs. A missing counter cannot be recast as zero or supported.

Primary performance precedent is recorded in the [role study](../../../../Strategy/Research/GraphicsEngineerProfiles.md#technical-evidence-and-transfer-limits). All thresholds and actual results remain with their existing acceptance owners; this page does not pass `WL-04` or advance `PY-03`.
