# Performance Diagnostics — Acceptance

[Docs](../../../README.md) / [Up](PerformanceDiagnosticsDesign.md) / [Implementation](ImplementationPlan.md)

**Status:** feature-local acceptance contract; not proof that diagnostics or workloads have passed

**Scope:** feature-local diagnostic evidence fields, baseline experiments, benchmark artifacts, reviewer paths, and verification gates

**Feature architecture:** [Performance Diagnostics Architecture](PerformanceDiagnosticsDesign.md)

**Delivery authority:** [Performance Diagnostics Delivery Plan](ImplementationPlan.md)

**Release/workload orchestration:** [Graphics Workloads](../../../Acceptance/GraphicsWorkloads.md)

**Engineering evidence rules:** [Validation, Performance, and Evidence](../../../Engineering/Verification/ValidationAndEvidence.md)

This file is part of the Performance Diagnostics architecture set and owns the diagnostic evidence expected from workloads, baseline experiments, reviewer artifacts, and verification gates. Candidate results remain in the release-level completion report and must retain exact commands, configuration, environment, artifacts, and limitations.

**Current readiness:** **20/100** — foundations only; this acceptance contract contains no executed candidate result. See [Current Feature Readiness](../../../Acceptance/CurrentReadiness.md#explicit-missing-or-not-yet-admitted-capabilities).

## Acceptance At A Glance

Apply this contract to the exact [selected delivery scope](ImplementationPlan.md#delivery-at-a-glance). Metric validity, provenance, comparison, observer and failure rules remain mandatory for any claimed result. Live groups, histories, internal GPU capture and their UI checks apply only when that capability is admitted; absent unselected views are not defects and do not require implementation. Record inapplicability with the scope reason, distinct from missing evidence for a required cell. Release measurement can use qualified external tools and existing producers without completing this broader product.

| A trustworthy result must answer | Required evidence shape |
| --- | --- |
| what interval and work were measured? | stable frame/range identity, mode, queue/thread/phase, validity, counts, and raw samples |
| is the run representative? | observer-state disclosure, normal-topology proof, warm-up/readiness, background-work state, and rejected-sample reasons |
| is the result repeatable and comparable? | exact build/hardware/driver/API/workload/configuration plus p50/p95/p99/worst and variance/uncertainty |
| what caused the cost? | engine correlation first, then a linked native capture whose identity joins the same frame/range and configuration |
| did measurement remain bounded and honest? | overhead/loss accounting, queue/ring capacity, memory high-water, unavailable states, and no fabricated zeros |
| can a reviewer act on it? | concise verdict, threshold and regression direction, artifact links, limitations, and next diagnostic question |

The live dashboard is orientation, not benchmark proof. A benchmark summary without raw samples and provenance is not reproducible; a native capture without matching engine identity is not attributable; a faster result obtained only because instrumentation changed the execution topology is not representative.

## Acceptance-Workload Diagnostic Contract

The selected workload evidence is an explicit immutable result. A future live presenter or focused internal capture would be a different optional product; it does not have to exist to accept native captures or benchmark records. Apply the relevant depth below only when that consumer is selected, with the same exact metric/identity/validity semantics.

| Depth | Product | Required statistics | Retention/output | Claim strength |
| --- | --- | --- | --- | --- |
| Optional live orientation | Only an independently admitted presenter | Latest, rolling p50/p95/max, sample count/span, current/high-water memory, validity and observer state. | Bounded in-memory rings; no file by default. | Chooses the next question; never proves causality. |
| Optional internal frame/range | Only an independently admitted frozen capture | Selected-frame/range timing, inclusive/exclusive marker hierarchy, counts, validity, configuration. | One retained capture plus bounded recent detail. | Attributes marked time; does not prove hardware cause. |
| Benchmark evidence | `MAP-00` and accepted workload route | Per-run and combined p50/p95/p99/worst, run-to-run variance/uncertainty, high-water identity, event/count totals, comparisons and threshold verdict. | Explicit raw samples, summary, manifest, capture links under the workload owner. | Supports a reproducible performance claim after controls and quality checks. |
| Specialist capture | Native external tool | Scheduler/API/resource/counter/source/ISA/allocation/residency/crash facts. | Tool-native artifact linked from the same manifest. | Supports a scoped cause on the captured platform/configuration. |

### Benchmark Evidence Families

Each accepted run records the following families where the workload declares them applicable. `Unavailable`, `Unsupported`, and `NotInstrumented` are valid states; omission or zero is not.

| Family | Required measurements and identities | Primary presentation |
| --- | --- | --- |
| Frame, pacing, latency | CPU begin-to-begin and submitted-frame interval, GPU per-queue span/overlap, present/throttle classification, discontinuities, p50/p95/p99/worst. Input-to-present requires a real `InputSampleId -> simulation FrameId -> submitted PresentId -> displayed/presented result` chain; until that chain exists the field is `NotInstrumented`, never estimated from frame time. | Overview distribution/threshold plus raw samples; system/PIX timing trace for displayed latency and pacing cause. |
| CPU ownership | Host/game/editor, extraction, culling, frame setup, graph setup/compile, command recording, submit, present; physical thread, logical phase, wait/backpressure, tasks, ready delay, critical interval, serial/1/2/N worker policy. | CPU lanes/aggregates and workload table; WPA, Nsight Systems, or uProf for scheduling/stacks/microarchitecture. |
| GPU execution | Graphics/compute/copy queue spans, overlap, stable pass/child markers, inclusive/exclusive time, unaccounted span, synchronization identity, delayed-result validity. | GPU live/captured views; PIX/Nsight Graphics/RGP for API and hardware cause. |
| Workload cardinality | Passes, submissions/command lists, draws, dispatches, pipeline and shader-package counts, descriptor writes/binds where owned, barriers/transitions, instances/triangles, uploads, rejected work. | Render/RHI/selected marker tables and raw benchmark row. |
| CPU memory | Process working set and private commit, tracked allocator used/committed where owned, high-water identity, retirement/deferred-free state, load/unload checkpoints. | Memory trend/category table; allocation-stack tool for lifetime cause. |
| GPU memory and residency | Tracked used and allocator blocks, local/non-local usage/budget, committed/resident high-water, transient/upload/readback, uploads/evictions/mip-residency/missing-resource events, retirement. | Memory view and checkpoint table; PIX memory, RMV, or vendor trace for allocation/residency detail. |
| Ray tracing | BLAS/TLAS count and source geometry/instances, build/update/compaction time, scratch/result bytes, rebuild policy, traversal-sensitive route/configuration, RT dispatch and shader identity. | Selected Render/Memory rows and evidence table; RRA, PIX, Nsight, or RGP for structure/traversal cause. |
| Compilation and loading | Cold launch/load, warm load, time to first correct frame, shader compile/package-cache hit/miss, pipeline creation/cache state, asset/upload events, hitch `FrameId` and stable operation token. | Frame annotations/Hitches and evidence table; CPU/system/API capture for cause. |
| Concurrent/background work | Shader compilation/package work, asset I/O/upload, streaming/residency, editor composition, capture/tool activity, and other named engine workers active during the interval; explicit `Quiescent` only when the readiness contract proves it. | Context annotations and manifest; system trace for interference/critical-path cause. |
| Quality and comparability | Scene/route, reference image, settings, sample/reconstruction mode, dynamic resolution, output extent, validation, worker/topology, API, hardware/driver/build/content/config hashes. | Configuration banner, manifest, and before/after table. |

The live product admits a field only when a production owner can publish it within the cost/bounds rules. The benchmark schema may stream additional bounded event/count records during an explicit run. It still may not scan the ECS, renderer caches, descriptors, allocations, or native resources from the UI. Missing production facts remain a visible instrumentation gap and cannot be inferred from neighboring metrics.

### Input-To-Display Measurement Boundary

Input-to-display remains `NotInstrumented` until one bounded identity chain proves `InputSampleId -> simulation FrameId -> PresentId -> displayed result`. CPU begin-to-begin, queue submission, `Present` return, or average frame time are not substitutes. Presentation timing is a separate capability and observer mode because platform/API feedback may allocate queues, delay result availability, expose only some display stages, or require external instrumentation.

Candidate paths have different evidence strength:

| Path | Strength | Limitation / decision |
| --- | --- | --- |
| PresentMon/ETW system trace | Cross-API Windows pacing and displayed-event correlation without embedding a vendor SDK. | Vulkan accuracy/correlation may be reduced relative to DXGI paths; record provider and confidence. Initial external evidence option. |
| DXGI frame-latency/presentation data | Strong Windows D3D/DXGI integration and presentation identity. | D3D/DXGI-specific and still requires the input/simulation join. Initial D3D platform option. |
| Vulkan present-timing capability | API feedback can identify supported presentation stages and time domains. | Capability/surface dependent; the result queue is bounded and can perturb/fail when undersized. Admit only behind an explicit capability and observer mode. |
| Vendor latency SDK | May expose a deeper engine-to-display chain on supported hardware. | Vendor dependency, configuration coupling, and cross-vendor semantic gaps. Defer until an accepted workload needs it. |
| Optical/high-speed measurement | Measures photons and can validate the complete external path. | Specialized equipment and route automation; difficult to attribute internal stages. Use as validation evidence, not a live metric owner. |

Current tool/version caveats and source links live in the [profiler runbook](../../../Engineering/Verification/ExternalProfiling.md#input-to-display-options).

### Comparison And Regression Contract

Every optimization result is a comparison between accepted runs, not between hand-picked screenshots. The workload-owned analysis must:

1. reject incompatible route, resolution, render settings, readiness, backend intent, diagnostic mode, or engine/content/configuration identity unless the changed field is the declared experiment variable;
2. retain all valid raw samples and per-run results, report exclusions/discontinuities, and calculate p50/p95/p99/worst plus the workload's selected uncertainty method;
3. state the predeclared hypothesis, competing cause, serial/control case, one scoped change, quality/correctness result, and architecture scope;
4. link the precise native capture and selected `FrameId`/marker/resource/shader identity that distinguishes the hypotheses;
5. apply a predeclared absolute/relative regression threshold and produce `Pass`, `Regression`, or `Inconclusive`, never a color-only verdict;
6. preserve useful negative results, including `Do not ship` or `Not worth the complexity`, with the rejected alternative and evidence.

The Reference System carries the primary distribution. A materially different GPU architecture is the Comparison System when available. Cross-machine results are not pooled into one percentile distribution.

Three runs of 300 warm valid frames are the current acceptance floor, not an automatic claim of statistical definitiveness. Frame samples are typically autocorrelated; p99 contains few tail observations at this floor, worst values are sample-count-sensitive, and pooling can hide run-to-run shifts. Therefore:

- per-run distributions and run-to-run variation are primary; the combined distribution is secondary and retains run identity;
- the analysis predeclares both an absolute and relative practical-effect band, not only a significance threshold;
- uncertainty uses a declared correlation-aware method such as a justified block bootstrap or permutation at an appropriate independent unit; the method, block/unit choice, confidence level, and assumptions are recorded before reading the outcome;
- a confidence interval that overlaps the practical-effect band produces `Inconclusive` even if the point estimate looks favorable;
- p99 and worst are accompanied by valid `N`, tail-event `FrameId` values, and exclusions. Worst-to-worst comparison requires equal `N` or an explicit sample-count model;
- a p-value, when a tool provides one, supplements rather than replaces effect size, confidence interval, run stability, quality, and the predeclared regression band.

## Baseline Experiments Before Optimization

The first Sponza investigation uses matched camera, resolution, renderer settings, content, and warm state. It records each mode separately:

| Comparison | Question | Important control |
| --- | --- | --- |
| `DevelopmentGame` vs `DevelopmentEditor` | Is the poor frame rate inherent to rendering/gameplay or specific to editor UI/composition? | Same render/output resolution and scene route; do not compare a maximized editor viewport with a smaller game window. |
| D3D12 vs Vulkan | Is the cost distribution backend-specific? | Same adapter, shader path, render settings, route, warm state, and validation state. |
| Threaded depth 1 vs threaded depth 0 | Does overlap help, or does backpressure/input latency dominate? | Compare distributions and input-to-present only when that latency becomes measurable. |
| Threaded vs serial renderer | Is render coordination/queueing involved? | Serial is a causal control, not an intended faster architecture. |
| Normal tasks vs `task.SerialExecution=true` | Does the task graph provide useful parallelism or scheduling overhead? | Same final result; record worker policy and tiny-work crossover. |
| GPU timings off vs basic vs detailed | What is instrumentation overhead? | Never merge samples collected with different timing scope sets. |

The screenshot's visible approximately 6 FPS is enough to justify measurement, but not enough to decide that Sponza is GPU-bound, editor-bound, or renderer-bound. Fixed resolution and active render path are first-order controls because the current editor can run at a maximized client extent.

## Benchmark And Portfolio Evidence

A live Editor screenshot, when used, is an illustration rather than the result. Conceptually, a reviewer-ready performance case includes the following files under the workload-owned run directory; [Graphics Workloads](../../../Acceptance/GraphicsWorkloads.md) remains authoritative for exact names and placement:

```text
artifacts/validation/showcase-levels/<run-id>/<level-id>/
|-- manifest.json
|-- cook.log
|-- timings.csv
|-- summary.md
|-- launch.log
|-- <level>-<route>-frame.png
`-- captures/
    |-- cpu-system-trace.<native-format>
    |-- d3d12-or-vulkan-frame.<native-format>
    `-- vendor-gpu-trace.<native-format>
```

This proposal requires that the manifest link every raw sample and capture to one run/configuration rather than creating a second evidence format. Native captures may remain in an accepted external location when their size or license requires it, but the manifest records the stable reference and provenance.
CPU, GPU, and sampled memory columns may share the workload-owned `timings.csv`; a separate raw file is added only if the acceptance-workload schema explicitly adopts it.

### Example Summary Table

This is a presentation template; values must come from real accepted runs.

| Metric | Baseline | Experiment | Delta | Interpretation |
| --- | ---: | ---: | ---: | --- |
| CPU frame p50 / p95 / p99 / worst | `TBD` | `TBD` | `TBD` | Begin-to-begin, same declared route, window, and mode; uncertainty/run variation linked. |
| Host/game phase wall p50 / p95 / p99 | `TBD` | `TBD` | `TBD` | Physical owner plus logical phase breakdown; not sampled CPU execution time. |
| Render CPU p50 / p95 / p99 | `TBD` | `TBD` | `TBD` | Extract/cull/graph compile/record/submit/present critical contribution. |
| GPU graphics p50 / p95 / p99 / worst | `TBD` | `TBD` | `TBD` | Top-level graphics span; queue overlap stated separately. |
| Top GPU pass p50 / p95 / p99 | `TBD` | `TBD` | `TBD` | Stable semantic pass; no nested double count. |
| RAM working/private high-water | `TBD` | `TBD` | `TBD` | Both definitions reported. |
| GPU tracked/local high-water | `TBD` | `TBD` | `TBD` | Used/block/API/budget scope declared. |
| Compilation/pipeline hitches | `TBD` | `TBD` | `TBD` | Cold/warm state, cache state, event count, and worst correlated `FrameId`. |
| BLAS/TLAS build/update + scratch/result | `TBD` | `TBD` | `TBD` | Applicable RT mode only; structure/traversal capture linked. |
| Input-to-present p50 / p95 / p99 | `NotInstrumented` | `NotInstrumented` | n/a | Report only with an end-to-end input/present identity and supported measurement path. |
| Quality/correctness | `TBD` | `TBD` | n/a | Identical image/reference or explained change. |

The actual case-study table shows the predeclared regression threshold, result state, run count, valid/excluded samples, and uncertainty next to the headline delta. It may omit inapplicable rows but may not silently omit a required workload family.

### Example Incident Narrative

A specialist-facing case should be readable in this order:

1. Result: "Sponza at the fixed route was limited by `<measured domain>`, not by `<competing hypothesis>`."
2. Configuration: commit, product, backend, hardware/driver, resolution, route, settings, validation, workers, and diagnostics mode.
3. Baseline: distributions and memory high-water, not one frame or one FPS number.
4. Competing hypotheses: for example editor UI, game systems, frame-graph compile, command recording, GPU lighting, presentation, or memory pressure.
5. Discriminating evidence: the original correlated measurements and appropriate native CPU/GPU artifact that distinguishes the hypotheses. A joined workspace or ProfileGpu tree is not required; it may be used only if independently admitted and qualified.
6. Root cause: critical path with relevant call stack, pass, queue, resource, counter, or state.
7. Experiment/fix: one scoped change and the control that could falsify it.
8. Outcome: before/after distributions, image/validation result, memory, pacing, and limitations.
9. Adoption: how another engineer reproduces the route and opens the capture.
10. Deletion/negative result: instrumentation, path, or optimization rejected after evidence.

### Reviewer Paths

| Reviewer | Diagnostic evidence they should see |
| --- | --- |
| Recruiter, 60-90 seconds | One clean frame/diagnostics image and one sentence naming the measured bottleneck and outcome. No profiler wall of text. |
| Hiring manager, 10 minutes | Problem, configuration, baseline, relevant native marker/timeline excerpt, one controlled experiment, before/after table and limitation; no internal visualizer required. |
| Graphics specialist, 45-60 minutes | Raw schema, CPU/GPU timing semantics, inclusive/exclusive marker tree and invalidation rules, PIX/RenderDoc/Nsight/RGP/WPA capture, API/backend difference, counters or disassembly where causal, and reproduction steps. |
| Adopter | Exact build/run/route, diagnostic mode, expected summary, capture trigger, fallback, raw files, and issue/reproducer template. |

This directly advances whole-system performance, hard-debugging, low-level concurrency, productization, and communication evidence. It does not advance those requirements to `E3` until the captures and measurements are reproducible.

## Verification Contract

Implementation acceptance requires focused tests and measured runs, as applicable:

### Authoring Isolation And Shipping Erasure

Every vertical slice passes the [authoring-isolation and Shipping-erasure](#authoring-isolation-and-shipping-erasure) checks in the same change. Reviewers inspect the real content/feature path as well as diagnostics files. Acceptance is blocked by a new authoring step, collector selection in production code, diagnostic-only state in a real-work owner, an optional profiler dependency in Shipping, or optimized Shipping object code/data that retains the diagnostic seam. Final hardening repeats this proof; it does not defer it.

### Frontend Workflow And Clutter

Accept the actual selected action, not the former workspace design. Current external capture uses the startup enum and single selected-provider viewport action: None/ready/armed/finalizing/completed/unavailable/busy, disabled reasons, wrong/stale target, cancellation/failure and safe recovery are observable through existing controls. Multiple intent rejects before graphics work; UI owns no duplicate provider state or native calls.

Keyboard/scale labels, visible focus, status and errors must work for those delivered controls. A clean first-use/recovery exercise records confusion or required private intervention. Quick Check, Investigate CPU/GPU/Memory, Customize Stats, Inspector, hitch navigator and Performance tabs are not required routes. If an internal presenter is later admitted, freeze its exact action/state/validity/cost and accessibility checks with the producer; do not inherit every screen in the old mockup.

### Semantics And Correlation

| Actual product being accepted | Required controls |
| --- | --- |
| Any measurement/comparison | Exact owner/unit/interval/frame/configuration, timestamp/provenance/wrap semantics, valid/excluded/lost populations, discontinuities and reset; known-value controls reject missing/stale/invalid fields and fabricated zeros. Preserve physical threads versus logical phases and sampled versus OS-lifetime memory peaks. |
| Current native capture | Exactly-once request outcome, target/frame certainty, native completion/quiescence, symbols/candidate identity, loss/shutdown and marker/source correlation under the External Capture contract. Use actual serial/threaded topology and expose unsupported cells. |
| Future admitted cross-domain join/history | Only if consumed: delayed/out-of-order CPU/GPU results, generation/reset and common-correlated cohorts; use serial and exposed threaded depths 0/1/2 with randomized resolution delays. Prove fixed capacity/loss, no stale-frame substitution and no synchronous collection from UI. |
| Future admitted internal hierarchical capture | Only if consumed: known nested ticks, parent cycles/child escape/sibling overlap/cross-queue/wrap invalidation, deterministic chunk merge under randomized task completion, bounded state-machine settlement and parallel topology. Inclusive/exclusive uses interval unions and exposes unattributed time; marker-only nodes have no fabricated duration. |

These controls use existing commands or disposable local probes. They do not require a committed fuzz/property-test framework, scope registry, session, hierarchy or query product for a native capture or workload record. If those structures are actually selected, token/path collision, recording-local balanced scopes, population/union arithmetic and observer bounds remain required; transient identity or per-frame strings cannot become aggregation keys.

### Backend And Product Matrix

External native-capture criteria/failures/checks are owned by the [External Capture dossier](ExternalCapture/ExternalCaptureDesign.md#acceptance-and-check-contract); the rows below retain the parent cross-diagnostics matrix and require that delegated contract. Capture request failure and native quiescence are distinct; a scene viewport maps to a containing present interval rather than an exclusive native scene capture.

- DevelopmentGame and DevelopmentEditor on D3D12 and Vulkan.
- DevelopmentEditor external capture: PIX on D3D12; RenderDoc on D3D12 and Vulkan; Nsight Graphics Capture on its supported NVIDIA D3D12/Vulkan matrix while explicitly `Experimental`.
- For each provider, test no flag/no injection, requested but missing, passively attached, ready, next-frame capture of the clicked viewport, multi-window target selection, busy/conflict, resize/minimize, device loss, finalization, shutdown, artifact-open/path-unavailable, and clean relaunch without the provider. Test the admitted single-provider route and rejection of invalid multiple-provider intent; native combinations need separate acceptance and remain rejected when unqualified. Reuse the current selected-provider icon and request authority.
- Threaded and serial renderer controls.
- Normal and serial task controls.
- Validation on/off state recorded; native validation passes on supported routes.
- Empty establishes observer overhead; Sponza proves the `MAP-00` calibration path.
- Fixed render/output/client resolution, VSync, provider/fallback, and readiness are visible.

### Profiler Correlation

- An engine CPU phase matches the corresponding ETW/PIX interval within a declared tolerance.
- Engine top-level D3D12 GPU timing matches the corresponding PIX range within a declared tolerance.
- D3D12 and Vulkan captured pass hierarchy and inclusive/exclusive values correlate with the same external marker ranges within a declared tolerance; differences and unavailable timing scopes are explained.
- Engine Vulkan pass names and ordering are visible in RenderDoc and a supported vendor trace.
- One-click PIX, RenderDoc, and supported Nsight Graphics captures start on the intended viewport's next valid frame, contain the same stable Sparkle marker path, and report the requested/captured identity or an explicit provider limitation; no test accepts a capture from another Editor window.
- Every Sparkle-owned OS thread appears with its stable name.
- The `SparkleTasks` ETW provider decodes task names, lanes, run/task identity, outcome, and dependencies.
- One artificial CPU stall, GPU stall, frame-queue backpressure case, and memory growth/retirement case is detected and classified; removing the injected defect clears the signal.

### Workload Evidence And Investigation Method

- `MAP-00` proves fixed resolution, authoritative readiness, named screenshot, raw CPU/GPU samples, process RAM/GPU memory state, manifest identity, and a clean reproduction on Sponza before any flagship claim.
- The accepted measured route contains at least three runs of at least 300 warm valid frames, preserves per-run raw samples, and reports CPU/GPU p50/p95/p99/worst plus the declared uncertainty/run-variation method.
- A synthetic autocorrelated dataset proves the selected uncertainty implementation and `Inconclusive` overlap rule; tail reports preserve `N` and event identities, and unequal-`N` worst comparisons are rejected unless explicitly modeled.
- Cold launch/load, warm load, time to first correct frame, compilation/cache/pipeline hitches, and steady traversal are distinct phases and are never pooled into one warm distribution.
- Benchmark comparison rejects an intentionally mismatched resolution, route, settings, readiness, observer mode, or undeclared configuration change; accepted comparisons emit explicit threshold and `Pass`/`Regression`/`Inconclusive` state.
- Benchmark records cover applicable renderer-stage time, workload counts, process RAM and precise GPU memory high-water, upload/eviction/residency/missing events, BLAS/TLAS build/update/compaction/scratch/result data, and exact engine/content/configuration identity, or visibly mark the production fact unavailable.
- One CPU investigation demonstrates timeline classification, sampled-stack localization, a serial/1/2/N control, and a PMC/IBS or equivalent cache/branch/data-access hypothesis where it is causal; it reports CPU topology and does not generalize beyond measured systems.
- One GPU investigation demonstrates the top-down sequence from activity/queue behavior through unit pressure to selected marker and source/ISA or RT evidence where causal, then validates the predicted whole-route result.
- One memory investigation demonstrates `A Before`, `B Loaded/Warm`, and `C Unloaded/Retired`, reconciles engine totals with event trace/snapshot scope, and distinguishes retained pools/fragmentation from a leak claim.
- `CASE-02` captures the same semantic route on D3D12 and Vulkan and explains queue/resource/barrier/descriptor/pipeline/RT-build differences without requiring identical API event streams.
- At least three measured bottleneck studies satisfy the workload protocol; at least one preserves a justified `Do not ship`/`Not worth the complexity` result. One difficult incident includes competing hypotheses, minimal reproducer, native capture, root cause, scoped fix, regression gate, and limitation.
- `CASE-05` is attempted by another engineer or clean-environment adopter using only the documented route; deviations, missing capabilities, and capture failures become explicit evidence rather than being repaired silently by the author.

### Cost And Bounds

- Ring capacities, queue capacities, string/label storage, and export sizes are asserted/tested.
- Any separately selected presenter/history/query layout proves its actual calibrated capacity, overflow and retained cost. The earlier four-overlay/16-row workspace design is not a minimum product or acceptance prerequisite.
- `LiveBasic` performs no post-initialization per-frame heap allocation.
- Measure only existing/selected observer modes against Off and disclose each actual collection configuration. No mode is implemented merely to complete an overhead table.
- If a live/internal presenter is separately admitted, measure its selected open/closed and collection compositions independently; unselected stat groups and views require no implementation or experiment.
- Any separately admitted internal GPU capture leaves parallel frame-graph recording enabled and preserves submission batches, queue dependencies, worker policy, command order, and output. CPU recording disturbance and GPU timing disturbance are reported.
- Scope planning and recording use fixed storage with no per-scope string allocation, shared-vector growth, or mutex contention between recording tasks; per-queue query capacity is checked before arming.
- Enabling groups with the same minimum mode does not duplicate collection, memory polling, GPU timestamp pairs, or history storage.
- Export failure preserves the previous accepted evidence and reports one actionable error.
- `git diff --check`, applicable builds/tests, architecture boundary check, and exact unavailable hardware/tool paths are reported.

## Stable Completion Mapping

The detailed contracts above remain the authority for fields, statistics, workloads, and reviewer artifacts. The stable IDs below make those obligations traceable from a candidate report without copying the matrices elsewhere.

| Criterion | Pass condition | Checks |
| --- | --- | --- |
| `AC-PERF-01` | Every displayed/exported metric names its physical or logical owner, unit, interval, validity, age, and unavailable state; no neighboring metric is used as a substitute. | `CHK-PERF-01`, `CHK-PERF-04` |
| `AC-PERF-02` | Existing/selected observation modes collect only their declared bounded products and Shipping erases applicable development-only collection; unselected modes are not implemented for this criterion. | `CHK-PERF-02`, `CHK-PERF-06` |
| `AC-PERF-03` | The evidence depths actually selected remain distinct and join immutable frame/run/configuration identity; an absent unselected UI/capture depth is not required infrastructure. | `CHK-PERF-01`, `CHK-PERF-03` |
| `AC-PERF-04` | Accepted comparisons retain raw samples, run counts, uncertainty/variation, thresholds, comparability controls, and `Pass`/`Regression`/`Inconclusive` disposition. | `CHK-PERF-03` |
| `AC-PERF-05` | CPU, GPU, memory, loading/compilation, concurrency, ray tracing, and workload-cardinality families are recorded or explicitly classified unavailable. | `CHK-PERF-01`, `CHK-PERF-05` |
| `AC-PERF-06` | Input-to-display remains `NotInstrumented` until a bounded input/simulation/present/display identity chain exists and is correlated independently. | `CHK-PERF-04` |
| `AC-PERF-07` | D3D12, Vulkan, DevelopmentEditor, and DevelopmentGame routes expose honest requested/active capability, comparable semantic markers, and explicit provider limitations. | `CHK-PERF-04`, `CHK-PERF-05` |
| `AC-PERF-08` | Instrumentation, UI, capture, and export remain within declared CPU/GPU/memory/storage bounds and do not alter the measured ownership or scheduling route without disclosure. | `CHK-PERF-02`, `CHK-PERF-06` |
| `AC-PERF-09` | A second engineer or clean environment can reproduce the documented capture/investigation route using only retained configuration, commands, artifacts, and limitations. | `CHK-PERF-03`, `CHK-PERF-05` |
| `AC-PERF-10` | Candidate evidence satisfies the workload protocol without promoting source presence, a responsive process, one screenshot, or one profiler number into causal or release proof. | `CHK-PERF-03`, `CHK-PERF-05` |

| Failure ID | Injected condition | Required safe result | Check |
| --- | --- | --- | --- |
| `FM-PERF-01` | missing, delayed, invalid, discontinuous, or unsupported measurement | UI/export shows the exact validity state and excludes it from incompatible aggregation or classification | `CHK-PERF-01` |
| `FM-PERF-02` | overflow a ring, query pool, queue, label table, export bound, or capture request | collection rejects or truncates by declared policy without corruption, unbounded growth, or plausible complete evidence | `CHK-PERF-02` |
| `FM-PERF-03` | compare mismatched resolution, route, settings, readiness, backend, observer mode, or sample population | comparison becomes `Inconclusive` or rejected and identifies every mismatch | `CHK-PERF-03` |
| `FM-PERF-04` | capture unavailable/busy/wrong viewport, missing provider, device loss, minimize/resize, or shutdown | request reaches a bounded explicit terminal state and never returns another window/frame as success | `CHK-PERF-04` |
| `FM-PERF-05` | engine metric and external trace/counter disagree beyond tolerance | candidate remains blocked while identities, domains, and limitations are reconciled | `CHK-PERF-05` |
| `FM-PERF-06` | enable overlapping groups or detailed capture under load | collection deduplicates shared work, reports observer cost, and preserves bounded allocation/recording behavior | `CHK-PERF-06` |

| Check ID | Claim-falsifying action | Primary route |
| --- | --- | --- |
| `CHK-PERF-01` | schema-validate live/focused/benchmark records with missing, delayed, invalid, unsupported, and discontinuous fields | metric semantics and evidence families |
| `CHK-PERF-02` | force capacity, export, cancellation, shutdown, and Shipping-erasure cases | bounds and controlled failure |
| `CHK-PERF-03` | run `MAP-00`, repeated warm samples, mismatch controls, synthetic uncertainty data, and clean-environment replay | comparison and reproducibility |
| `CHK-PERF-04` | exercise supported/unavailable capture providers, viewport identity, input-to-display classification, and external marker correlation | capture and latency truth |
| `CHK-PERF-05` | correlate representative CPU, GPU, memory, ray-tracing, and hard-incident evidence with WPA/PIX/RenderDoc/vendor tools as applicable | attribution and causality |
| `CHK-PERF-06` | measure actual selected-mode/storage/export overhead and parallel-recording disturbance; duplicate groups/query capacity apply only when those consumers are admitted | observer cost and noninterference |

## Completion Definition

An admitted diagnostics slice is complete only when every applicable `AC-PERF-01` through `AC-PERF-10` passes, inapplicable capabilities have explicit scope reasons, every applicable `FM-PERF-*` is exercised through its named `CHK-PERF-*`, and exact evidence is recorded in the owning report. Missing tools or hardware for a required cell remain blockers. A narrow capture or measurement pass does not claim the full target diagnostics product is delivered.
