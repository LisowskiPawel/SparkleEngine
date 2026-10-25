# GPU Shader And Native Execution Evidence

[Docs](../../../../../README.md) / [Up](../StudyProtocols.md) / [Implementation](ImplementationPlan.md)

**Status:** selected evidence contract; actual baseline, experiment and native results remain unproved

**Outcome:** PGE-04/05/06/08/09/10; PGD-03/04; CASE-02/03; AC-WS-02 and CHK-WS-02. This folder owns the domain-specific protocol and delivery; [common study rules](../StudyProtocols.md) own sampling/observer policy, [parent acceptance](../WorkloadStudiesDesign.md#local-acceptance-and-failures) owns AC/FM/CHK, and [Graphics Workloads](../../../../../Acceptance/GraphicsWorkloads.md) owns CASE/WL results.

## Current Owners And Missing Proof

Targeted source landmarks at `9bf955414bfe98fbb9062f3563a9654f42e64e1e`, 2026-10-25; recheck the live dirty tree before execution. Required lighting/reuse/resolve shaders and their C++ registration/binding owners already exist, including `Engine/Renderer/Private/Passes/Lighting/Restir/` and `Engine/Assets/Shaders/Passes/Lighting/Restir/`. `ShaderSystem` owns compiler/cook/ABI identity. Select an actually executed hot event rather than inventing a kernel or profiler.

## Protocol

**Question:** what limits one important executed rendering shader, and can a semantics-preserving kernel change improve frame cost?

Choose a real measured pass, preferably a required lighting/reuse/resolve kernel with enough frame impact. Freeze exact source/include/options -> DXIL/SPIR-V -> native pipeline/event identity. Obtain supported GPU Trace/source correlation and counters before selecting a register/live-state, memory-traffic/layout, divergence or arithmetic hypothesis. A stall sample at a consumer instruction does not by itself identify its producer or root cause.

Derive a cost model: useful operations and bytes per pixel/ray/sample, reuse/cache assumptions, register/shared-memory pressure and theoretical occupancy limits. Compare those predictions with actual counters/time and explain discrepancies. Treat peak-throughput/roofline numbers as bounds, not observed bandwidth or a guaranteed speedup. Vary one mechanism, keeping the same estimator, quality and scene inputs.

Record each evidence level honestly: compiler IR/bytecode; source-to-instruction-range correlation; full native ISA only when the actual installed tool/edition supports it. Missing native disassembly cannot be relabeled as obtained SASS or solved by inventing a decoder. The [Nsight documentation](https://docs.nvidia.com/nsight-graphics/UserGuide/shader-profiler.html) distinguishes these access levels; qualify the actual tuple. Counter/source/IR evidence can support a scoped hypothesis while a stronger ISA claim remains blocked.

**Exit:** one predicted/measured limiter, executed source/artifact join, low-level inspection at its stated level, bounded candidate and alternative, numerical/raw image controls, GPU and whole-frame distributions and memory/observer context on both APIs. Reject a local dispatch win that loses the required frame/quality result.

## Freeze Before Execution

| Decision / owner | Required value and consequence |
| --- | --- |
| Select one shader | Renderer owner chooses a measured frame-relevant pass, entry point and frozen output domain. Join source/includes/compiler/options to exact DXIL/SPIR-V and native pipeline/event. |
| Model and quantities | Predict operations and bytes per pixel/ray/sample, cache/reuse assumptions, registers/shared memory and occupancy constraints. Compare to counters/time; do not confuse peak throughput with measured work. |
| Access and causality | Freeze available source/IR/instruction-range/full ISA and metric support. A stall at a consumer is not proof of the producer cause. Missing SASS stays missing; tool entitlement or a new SDK is not a delivery requirement. |

REL-11/M2 and WS-D0 remain execution prerequisites. Stage 0 can prepare the card earlier. Actual numeric practical-effect/quality, allowed exclusions, API/profile/hardware and available native tools must be frozen with the workload owner; this page cannot supply unknown measured values or current tool entitlement.

## Evidence And Failure Controls

Use the [workload-owned artifact home](../../../../../Acceptance/GraphicsWorkloads.md#evidence-folders-and-delivery-owners) with package ID `WS-GPU`: identity index, arithmetic/traffic model, original shader/native captures and supported counters, numerical/raw controls, derivative `analysis/`, candidate/alternative, per-run GPU and whole-frame verdict. Record the actual inspection level/tool tuple. No original capture, source scene or sensitive dump is copied into maintained Docs. The accepted report references artifacts with hashes and exact candidate identity.

`CHK-WS-02` for this domain must reject a mixed candidate/settings tuple, distinguish a null/noise comparison, preserve equivalent raw output and retain an alternative/rejected conclusion. Original manifest/hash and read-only reanalysis controls use `CHK-WS-01`. A missing field is unavailable, not zero; an instrumented timing is not an optimized baseline. A useful negative result needs a tested premise and controls, not merely failure to collect data.

**Risk and contingency:** Renderer owner: an apparent kernel win changes sampling, material quality or source/bytecode identity. Match raw products and uncertainty, seed a settings/hash mismatch and reject the comparison when semantics or artifacts differ. Retire the risk only with the relevant controlled negative and positive retained-path evidence. Results go to the existing CASE/WL/feature report; no result is passed here.

## Scope And Stage Budget

One domain-specific question and existing production owner; no framework is selected. Prefer proof or removal of unnecessary work. Each repair records changed state/API/hooks/copies/dependencies, preserves the required frame and removes replaced or rejected mechanisms before its own exit. No-source evidence work justifies zero production deletion and removes probes. Final closure verifies earlier cleanup; it cannot defer it.
