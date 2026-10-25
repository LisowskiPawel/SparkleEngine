# Principal Graphics Study Protocols

[Docs](../../../../README.md) / [Up](WorkloadStudiesDesign.md) / [Delivery](ImplementationPlan.md)

**Status:** selected target protocols for WS-D0; no experiment, bottleneck, fix or performance result is claimed

**Responsibility:** make the three classical studies complementary and falsifiable. [Graphics Workloads](../../../../Acceptance/GraphicsWorkloads.md) owns scenes/sample policy; [Diagnostics Acceptance](../Acceptance.md#comparison-and-regression-contract) owns statistics/observer semantics. These protocols do not add runtime tooling or change those thresholds.

## Common Experiment Card

Use the same admitted Bistro/San Miguel route, output domain/quality, exact source/cook/compiler/settings, Windows hardware/driver and named D3D12/Vulkan cell before and after one change. Freeze the practical effect, uncertainty, warm-up/repeats, validity/exclusion rules and rollback before results. Preserve original runs/captures and actual serial/threaded/queue topology. Native validation, instrumented analysis and optimized timing are separate observations.

The existing floor remains at least three independent runs with at least 300 warm valid samples per selected route segment; obey the owner's separate readiness/warm-up requirements. Do not turn frame samples into independent runs or silently pool tail events. No quality/settings change, missing field, mismatched shader or unknown reference may manufacture a speedup.

## Owned Evidence Folders

Each domain has its own design and staged plan. Common sample/observer rules stay above; the child owns only the domain-specific question and execution. These folders deliver evidence through current production owners, not new runtime systems.

| Evidence stream | Contract and first local action |
| --- | --- |
| CPU preparation and scheduling | [Protocol](CpuPreparation/StudyDesign.md), [CPU-0](CpuPreparation/ImplementationPlan.md#cpu-0-freeze-the-causal-question) |
| GPU shader and executed code | [Protocol](GpuShader/StudyDesign.md), [GPU-0](GpuShader/ImplementationPlan.md#gpu-0-freeze-the-kernel-question) |
| Resource/queue lifetime | [Protocol](ResourceLifetime/StudyDesign.md), [LIFE-0](ResourceLifetime/ImplementationPlan.md#life-0-freeze-the-resource-and-completion-claim) |
| Difficult incident | [Protocol](IncidentAnalysis/IncidentDesign.md), [INC-0](IncidentAnalysis/ImplementationPlan.md#inc-0-freeze-the-failed-candidate-and-question) |

## WS-CPU: Preparation, Recording And Scheduling

The [CPU contract](CpuPreparation/StudyDesign.md#protocol) owns the critical-path/item/byte/cache/topology model, serial/worker controls and whole-frame verdict. Its plan closes the first of the three complementary studies.

## WS-GPU: One Rendering Shader And Its Limiter

The [GPU contract](GpuShader/StudyDesign.md#protocol) owns executed source/IR/native identity, the measured limiter, access limits and numerical/whole-frame controls. Its plan cannot require unavailable full ISA or a new profiler.

## WS-LIFE: Resources, Queues And A Real Constraint

The [lifetime contract](ResourceLifetime/StudyDesign.md#protocol) owns the resource/generation/last-consumer model, real byte/queue facts and delayed-completion controls. Its plan preserves required concurrency and avoids a second manager.

## Incident And Negative Evidence

The [incident contract and plan](IncidentAnalysis/IncidentDesign.md) own the actual failed candidate, competing hypotheses, first wrong product, minimal repair and controlled counterfactual. Reuse valid historical release evidence instead of manufacturing another failure.

At least one rejected/inconclusive alternative remains required across the three studies. All three domains need actual evidence; one shader profile does not establish CPU or lifetime depth. A question without a measurable premise returns to WS-D05 before candidate results, rather than inventing three wins. Parent CASE/WL review reuses one real reproduction exercise; the folder split does not multiply adopters or add a results service.

## Inputs, Tool And Delivery Boundaries

WS-S0 freezes actual producer samples and exact per-study cards. A necessary missing record follows the existing bounded parent Phase-4 prerequisite, then returns to WS-D0; no internal profiler/data spine is selected. WS-S1 provides one offline parser/comparison consumer. WS-S2 executes these protocols. WS-S3 retains the incident, independent reproduction and explanation.

`AC-WS-02/03`, `CHK-WS-02/03` and relevant `WL-04/07` own the verdicts. Remove disposable probes, experiment switches and rejected product mechanisms at each handoff. Keep original artifacts, unsuccessful findings and useful minimal reproduction instructions. This page defines proof obligations, not candidate results.
