# CPU Preparation And Scheduling Evidence

[Docs](../../../../../README.md) / [Up](../StudyProtocols.md) / [Implementation](ImplementationPlan.md)

**Status:** selected evidence contract; actual baseline, experiment and native results remain unproved

**Outcome:** PGE-05/07/08/10; PGD-03; CASE-02/03; AC-WS-02 and CHK-WS-02. This folder owns the domain-specific protocol and delivery; [common study rules](../StudyProtocols.md) own sampling/observer policy, [parent acceptance](../WorkloadStudiesDesign.md#local-acceptance-and-failures) owns AC/FM/CHK, and [Graphics Workloads](../../../../../Acceptance/GraphicsWorkloads.md) owns CASE/WL results.

## Current Owners And Missing Proof

Targeted source landmarks at `9bf955414bfe98fbb9062f3563a9654f42e64e1e`, 2026-10-25; recheck the live dirty tree before execution. The inspected source has one world task graph in `Engine/GameFramework/Private/World/Systems/`, Renderer scene preparation in `Engine/Renderer/Private/Scene/Preparation/`, and frame-graph compilation/recording in `Engine/Renderer/Private/FrameGraph/`. These are investigation landmarks, not a diagnosis or evidence of a missing scheduler.

## Protocol

**Question:** which CPU ownership/working-set/scheduling edge limits delivery of a correct frame, and does a bounded change improve the whole-frame distribution?

Trace the current world extraction, Renderer preparation, graph compilation/recording and RHI submission owners. Select the observed dominant edge; do not optimize all of them. Retain task/thread/dependency/wait evidence and a serial control. Freeze expected cost as useful item work plus task/dispatch overhead and the actual dependency critical path; sum of worker durations is not frame latency.

The hypothesis card names input cardinality, bytes touched/copied, working set versus cache, actual core/worker topology, work partition, queue depth and predicted crossover. Explain observed contention/false sharing, allocation or oversubscription only when traces/counters discriminate them; a worker count is not CPU-architecture evidence. Candidate examples include removing a duplicate preparation copy or improving an existing range partition. Preserve one Tasks runtime, publication epochs, exclusive writes and deterministic output. No new scheduler, forced global serial mode or fabricated async queue is selected.

**Exit:** source/ownership trace, serial/worker correctness/failure control, native frame identity, predicted-versus-measured crossover, p50/p95/p99 CPU and whole-frame/pacing/memory results. If no useful improvement survives controls, retain the negative result; never claim concurrency correctness as measured speedup.

## Freeze Before Execution

| Decision / owner | Required value and consequence |
| --- | --- |
| Choose the edge | Workload owner selects one measured top CPU cost on the fixed route: extraction/preparation, compile/recording or submit/wait. Choose before candidate edits; preserve the actual dependency and worker topology. |
| Model and quantities | Predict critical-path milliseconds from useful item work plus task/dispatch/wait overhead; record items/frame, bytes touched/copied, worker/core topology, working set and queue depth. Worker-duration sum is a work measure, not frame latency. |
| Falsifier | A serial/worker mismatch, ownership race, changed output or a null/noise comparison incorrectly reported as a win fails the study. Cache/false-sharing causes need discriminating observations, not a thread-count inference. |

REL-11/M2 and WS-D0 remain execution prerequisites. Stage 0 can prepare the card earlier. Actual numeric practical-effect/quality, allowed exclusions, API/profile/hardware and available native tools must be frozen with the workload owner; this page cannot supply unknown measured values or current tool entitlement.

## Evidence And Failure Controls

Use the [workload-owned artifact home](../../../../../Acceptance/GraphicsWorkloads.md#evidence-folders-and-delivery-owners) with package ID `WS-CPU`: input index, cost-model note, original per-run CPU/frame samples, task/native timelines, serial/worker controls, derivative `analysis/`, candidate/alternative and scoped verdict. Reference source/cook/capture manifests rather than duplicate them. No original capture, source scene or sensitive dump is copied into maintained Docs. The accepted report references artifacts with hashes and exact candidate identity.

`CHK-WS-02` for this domain must reject a mixed candidate/settings tuple, distinguish a null/noise comparison, preserve equivalent raw output and retain an alternative/rejected conclusion. Original manifest/hash and read-only reanalysis controls use `CHK-WS-01`. A missing field is unavailable, not zero; an instrumented timing is not an optimized baseline. A useful negative result needs a tested premise and controls, not merely failure to collect data.

**Risk and contingency:** Workload owner: removing a copy/wait changes a real publication or exclusive-write boundary. Reconstruct the lifetime/hazard proof, run the serial/worker negative, and retain or repair the original owner when equivalence fails. Retire the risk only with the relevant controlled negative and positive retained-path evidence. Results go to the existing CASE/WL/feature report; no result is passed here.

## Scope And Stage Budget

One domain-specific question and existing production owner; no framework is selected. Prefer proof or removal of unnecessary work. Each repair records changed state/API/hooks/copies/dependencies, preserves the required frame and removes replaced or rejected mechanisms before its own exit. No-source evidence work justifies zero production deletion and removes probes. Final closure verifies earlier cleanup; it cannot defer it.
