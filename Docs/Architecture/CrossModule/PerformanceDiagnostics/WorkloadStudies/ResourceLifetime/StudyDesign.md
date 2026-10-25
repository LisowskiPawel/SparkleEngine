# Resource Lifetime And Queue Evidence

[Docs](../../../../../README.md) / [Up](../StudyProtocols.md) / [Implementation](ImplementationPlan.md)

**Status:** selected evidence contract; actual baseline, experiment and native results remain unproved

**Outcome:** PGE-05/06/08/09/10; PGD-03/04; CASE-02/03; AC-WS-02 and CHK-WS-02. This folder owns the domain-specific protocol and delivery; [common study rules](../StudyProtocols.md) own sampling/observer policy, [parent acceptance](../WorkloadStudiesDesign.md#local-acceptance-and-failures) owns AC/FM/CHK, and [Graphics Workloads](../../../../../Acceptance/GraphicsWorkloads.md) owns CASE/WL results.

## Current Owners And Missing Proof

Targeted source landmarks at `9bf955414bfe98fbb9062f3563a9654f42e64e1e`, 2026-10-25; recheck the live dirty tree before execution. Source owners include `Engine/Renderer/Private/Frame/Retirement/FrameExecutionRetirementQueue.*`, `Engine/Renderer/Private/FrameGraph/Compiler/`, `Engine/Renderer/Private/Resources/Residency/`, mesh/texture cache residency and native RHI submission/resource owners. Their existence proves neither safe retirement nor a measured residency bottleneck.

## Protocol

**Question:** does a resource-lifetime/residency/queue edge cause the observed pressure, wait or frame tail?

Select one real condition: representative scene switch/reload or residency pressure, descriptor/resource replacement, AS update/build, or cross-queue dependency. Name the actual owner, generation and submission tokens. Freeze the expected bytes alive/retired and dependency/order timeline before testing. API budget, allocator blocks, owned live bytes and process memory are distinct quantities.

Use normal native captures, current allocator/lifetime facts and one controlled mutation. Preserve queue/order/correctness; do not manufacture a benefit by disabling parallel work or lowering content. Reject premature reuse, stale generation, leaks, false completion and unsupported hidden fallback. A fix must remove the replaced wait/state/lifetime path rather than add a second manager.

**Exit:** expected-versus-observed lifetime/byte/dependency model; native positive and failure controls; frame/queue/memory-tail results; serial or delayed-completion control where applicable; exact current consumer and scoped conclusion. Driver/API/hardware root cause requires discriminating evidence, not a slow Present call or similar-looking pixels.

## Freeze Before Execution

| Decision / owner | Required value and consequence |
| --- | --- |
| Select one condition | Choose an observed scene-switch/reload, residency pressure, descriptor/resource replacement, AS build/update or cross-queue edge. Record request, active generation, consuming queues and completion tokens. |
| Model and quantities | Predict bytes alive/retired, overlapping generations, required waits and dependency critical path. Keep API budget, allocator blocks, owned live bytes, usage and high-water quantities distinct. |
| Falsifier | Delayed completion, stale generation, premature reuse, budget pressure or a mismatched frame must detect the wrong lifetime/order claim. A slow Present or similar pixels cannot establish driver root cause. |

REL-11/M2 and WS-D0 remain execution prerequisites. Stage 0 can prepare the card earlier. Actual numeric practical-effect/quality, allowed exclusions, API/profile/hardware and available native tools must be frozen with the workload owner; this page cannot supply unknown measured values or current tool entitlement.

## Evidence And Failure Controls

Use the [workload-owned artifact home](../../../../../Acceptance/GraphicsWorkloads.md#evidence-folders-and-delivery-owners) with package ID `WS-LIFE`: generation/token/lifetime note, expected-versus-observed bytes/timeline, original native records, delayed/serial controls, derivative `analysis/`, quality/frame tails and causal verdict. Reuse original allocator/provider records. No original capture, source scene or sensitive dump is copied into maintained Docs. The accepted report references artifacts with hashes and exact candidate identity.

`CHK-WS-02` for this domain must reject a mixed candidate/settings tuple, distinguish a null/noise comparison, preserve equivalent raw output and retain an alternative/rejected conclusion. Original manifest/hash and read-only reanalysis controls use `CHK-WS-01`. A missing field is unavailable, not zero; an instrumented timing is not an optimized baseline. A useful negative result needs a tested premise and controls, not merely failure to collect data.

**Risk and contingency:** Renderer/RHI owner: reclaiming on the wrong queue or hiding pressure loses a live resource. Trace all consumers and delay the last completing queue; retain the existing lifetime path or fix its owner when the negative fails. Retire the risk only with the relevant controlled negative and positive retained-path evidence. Results go to the existing CASE/WL/feature report; no result is passed here.

## Scope And Stage Budget

One domain-specific question and existing production owner; no framework is selected. Prefer proof or removal of unnecessary work. Each repair records changed state/API/hooks/copies/dependencies, preserves the required frame and removes replaced or rejected mechanisms before its own exit. No-source evidence work justifies zero production deletion and removes probes. Final closure verifies earlier cleanup; it cannot defer it.
