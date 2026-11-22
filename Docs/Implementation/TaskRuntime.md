# Task-runtime replacement: remaining delivery

[Repository footprint](../RepositoryFootprint.md#task-runtime) | [Migration strategy](../ThirdPartyMigration.md#task-execution) | [Developer guide](../Engineering.md) | [Code style](../CodeStyle.md)

Temporary code-delivery plan. Remove completed actions as their evidence passes; delete this file after the code-completion exit below. Keep original commands, identities, results and rejected designs in the ignored reports. Broader product qualification does not keep a completed implementation plan open.

The deliverable is less task infrastructure to maintain so Sparkle can focus on its [three rendering cases](../Goals.md#the-three-cases). Follow the footprint study's [first retirement priority](../RepositoryFootprint.md#cuts-before-expansion): transfer the complete execution mechanism upstream and simplify its real callers. Its frozen scope is **3,017 code / 3,566 physical lines in 42 native files**, including the scene-preparation adapters; this is a gross retirement target, not measured net savings. Keep that denominator separate from later source freezes and all-infrastructure accounting. Graphics-share checkpoints are investigation context, not migration quotas.

**Next: stage 4 frame/world conversion in the qualified isolated candidate.** Stages 2-3 are complete for their recorded owner shapes and removed from the action plan. Reuse `C:/s/sparkle-task-delivery-20261122`; the primary checkout still uses Tasks. Unconverted candidate callers are temporarily synchronous with zero Tasks workers, so it cannot ship until stages 4-6 finish and Tasks is removed.

The authorized enkiTS fork is upstream v1.12 `0289cf6ffce86697e1f46cb4eeca1c946bffec71` plus patch SHA-256 `5b25c43e28c9023a6542a9f175f80e735eed4317cbe470d28af323007ac4d004`. Reuse the [runtime-fit report and native/owner evidence](../../build/validation/task-runtime/runtime-reopen-20261121/REPORT.md), [dependency/composition delivery](../../build/validation/task-runtime/delivery-20261122/REPORT.md) and [qualification status](../../build/validation/task-runtime/delivery-20261122/qualification-status.json). The unmodified pin remains rejected. Host/native linkage, capacity, rollback, staging and first-owner checks passed only for their frozen inputs; full world/recording/loading/cook behavior remains to prove.

Reconcile newer primary process/animation/shader changes against the candidate's input freeze before eventual publication; preserve concurrent work. The [focused baseline](../../build/validation/task-runtime/focused-baseline-20261121/REPORT.md) and original caller ledger remain the comparison owners. Reuse matching evidence and rerun affected checks. The candidate build uses the recorded `CMAKE_CXX_SCAN_FOR_MODULES=OFF` workaround after PCH access failures; default scanning is not qualified. Production savings remain zero. The delivery fragment misses the twelve-field/twelve-failure-site targets (25/89); full-migration accounting and performance/package budgets remain unqualified, without another quota-driven reduction experiment.

| Remaining stage | Required delivery |
| --- | --- |
| [4 - Frame/world conversion](#4-convert-frame-and-world-workloads) | Convert world, preparation, visibility and recording directly, preserving rendering ownership and deterministic caller-thread execution. |
| [5 - Loading/cook conversion](#5-convert-loading-residency-and-cook-batches) | Convert loading, residency preparation and cook batches while preserving admission, byte budgets, generations, publication, COM and item results. |
| [6 - Operations/removal](#6-convert-operation-lifetimes-and-remove-tasks) | Finish Editor/Launcher/incident lifetimes; delete Tasks, its API/target/DLL and all replaced caller/build/package plumbing. |
| [7 - Code completion](#7-complete-code-delivery-and-close) | Finish affected builds, focused correctness checks and ownership/removal review; record remaining qualification separately, fold lasting guidance and delete this plan. |

## Delivery constraints and fixed budgets

Follow [code delivery and qualification](../ThirdPartyMigration.md#code-delivery-and-qualification). Code completion requires the actual caller conversions, supported native primitives, correct cancellation/settlement/capacity, affected builds and complete removal of Tasks and replacement facades. Missing code, demonstrated races/hangs/starvation, premature destruction, false completion and material regressions remain blockers.

The numerical limits below remain fixed comparison and review targets, not automatic reasons to prolong a completed implementation. Report the complete accounting and any miss once, with its cause and impact; correct actual defects or needless infrastructure rather than repeatedly redesigning to hit a line count. Never compress code, hide moved costs or claim an unmet target passed. Full timing distributions, cold runs, profile/API matrices, image campaigns and relocated package/clean-machine checks are separate qualification: record **UNVERIFIED**, **INCONCLUSIVE** or an explicitly authorized **WAIVED / UNVERIFIED** and retain them with the existing report/release owner. Do not invent a waiver.

One upstream runtime must own runnable queues, workers, stealing, wakeup, range scheduling and dependency readiness. Existing owners retain rendering, publication, generation checks, resource budgets, process supervision and operation lifetime. Keep the Renderer GPU frame graph, coordinator, bounded frame admission, exclusive command leases, submission/barriers/fences and GPU retirement.

Delete the entire Tasks module, public API, `SparkleTasks` target/DLL and package surface. Remove generic graph/context/scope/event/dispatch machinery, padding graphs, redundant result/status translation, forwarding-only classes and obsolete configuration. Do not retain aliases, a compatibility facade, a dormant backend or a generic synchronous graph interpreter. Count relocated mechanisms as integration; earlier cleanup earns no migration credit.

Use the supported upstream facility directly before writing an adapter. The integration targets are ceilings to minimize, not allowances to fill. A bridge needs an actual consumer and a named ABI, resource or lifetime requirement that native/standard facilities do not already satisfy; keep it private to that owner. Unsupported fit is a reason to reject the candidate, not to rebuild Tasks around it.

The user authorized an enkiTS fork on 21 November 2026. Identify it by the original upstream commit plus exact patch/source hashes; count maintained fork changes and any added API/state/failure paths explicitly. This exception permits native repairs; the remaining ownership, capacity, allocation, lifetime and preservation requirements still apply.

| Existing mechanism to remove | Intended authority after conversion |
| --- | --- |
| `TaskExecutorImplementation`, lane queues, stealing, wakeup and worker loops | The selected upstream scheduler and its supported capacity resources; host composition owns their bounded lifetime. |
| `TaskGraphBuilder`, compiled storage, readiness/dependent counters and `ScheduledTaskExecution` | Native dependencies, ranges and completion actions; actual typed domain inputs/results stay with callers. No local graph compiler or readiness engine. |
| `ParallelFor` node expansion, graph-only capacity caches and `RenderScenePreparationTasks` | Native range work over actual spans; existing kernels retain thresholds, stable indices, scratch and ordered merge. |
| `SerialTaskExecution` | Direct caller-thread domain order using the same kernels, selected before native asynchronous initialization. |
| Generic contexts, scope trees, events and task outcome translation | Typed captures/results, standard stop facilities and supported native settlement at existing feature/host owners. Preserve used behavior; unused generic APIs need no replacement. |
| `TaskDispatch` preparation/queue/rearm machinery | Supported prepared native work/completion plus the incident sink's one bounded notify/coalesce/close policy. No generic dispatch replacement. |

Search existing counterparts before adding or relocating a function, type, state field or workflow. Give each mutable fact one writer/authority, readers, publication boundary, invalidation and final borrowed access. Shared ownership alone does not synchronize access. Reuse existing process/progress/publication/byte-account owners. Review [module boundaries](../Engineering.md#module-boundaries), single responsibility and [logical paragraphs](../CodeStyle.md#formatting-and-paragraphs) across callers and helpers. Keep unrelated renderer/serialization redesign outside this change and reconcile concurrent migrations without duplicating their owners.

| Measure | Fixed comparison / review target |
| --- | --- |
| First-party infrastructure | At least 1,528 net physical lines removed; at most 2,065 added/relocated integration lines anywhere, including CMake/templates and synchronous orchestration. |
| New surface/state/failure paths | At most 10 public execution-boundary declarations, 12 mutable infrastructure fields and 12 rejection/cleanup sites; enumerate nested/inline state individually. |
| Clean / incremental build regression | Both <=10% and <=30 s / <=2 s respectively. |
| Startup / shutdown regression | Both <=5% and <=50 ms. |
| Executable / package growth | Both <=5% and <=10 MiB. |
| Each run's frame p50/p95/p99 regression | Both <=3% and <=0.2 ms. |
| Load / cook regression | Both <=5% and <=50 ms. |
| Peak private commit / working-set growth | Both <=5% and <=32 MiB. |
| Cooperative cancel-to-cleanup | p99 <=100 ms; regression both <=10% and <=10 ms. |
| Native crash settlement | Preserve the ten-second policy and live-storage retention on a missed deadline. |
| Optimized measurement overhead | Both <=1% and <=0.1 ms. |

Exclude unchanged domain kernels and upstream source from added integration, reporting upstream build/package costs separately. Use the same formatting and counting scope before/after; do not compress code or remove necessary checks to meet a budget. A smaller line count cannot excuse a local scheduler, readiness engine, state mirror or unnecessary abstraction.

Keep APIs/runs/machines separate. Before a performance claim, bind the bottleneck, inputs, warm-up/repeats and sample method; the retained extended protocol is 600 warm-up frames, 3,000 measured frames and five fresh processes per fixed API/config/input. It is separate qualification, not a prerequisite to continuing caller conversion. Use paired run-level differences and correlated-frame 95% intervals; crossing a limit is inconclusive. CPU improvement claims require both >=5% and >=0.2 ms. Preserve equal sample counts for worst-frame comparisons. Follow [performance and image rules](../Engineering.md#performance-and-images); worker/task residence, CPU work, GPU execution and presented latency are different measurements, as are logical bytes, allocator storage and process high-water.

## Remaining caller conversions

| Owner / call path | Required change and retained policy |
| --- | --- |
| [Application composition](../../Engine/Application/Private/Game/GameApplication.cpp), [GameApplication](../../Engine/Application/Private/Game/GameApplication.h), [Renderer configuration](../../Engine/Renderer/Public/Concurrency/RendererExecutionConfig.h) | Keep the direct host resource/stop ownership and native frame borrows completed in stages 3–4. Finish level/residency/operation parameter conversion and truthful public/private visibility; remove temporary executor/scope owners with their last callers. No pool per operation or replacement facade. |
| [Scene load work](../../Engine/GameFramework/Private/Level/Loading/SceneLoadWorkState.h), mesh/texture residency | Delegate read/decode/finalization edges and joins. Retain two live scene-asset admissions, the separate [aggregate allocation budget](../../Engine/GameFramework/Private/Assets/Loaders/CookedAssetAllocationBudget.h), [residency accounts](../../Engine/Renderer/Private/Resources/Residency/AssetResidency.h), publication read scopes, generations and accepted-scene protection. Remove competing task-status/result controllers. |
| ShaderCompiler / TextureCooker / AssetCooker | Replace flat graphs/handles with native ranges or real stage dependencies and existing indexed results. Retain producer hash plus input equality, [texture request dedup/conflict policy](../../Tools/Cooking/TextureCooker/Private/Requests/TextureCookRequestSet.cpp), progress, actual-thread COM lifetime, processing/source byte leases and staged publication. No second batch framework or scheduler-level dedup/limiter. |
| [Editor operation slots](../../Engine/Application/Private/Editor/Operations/EditorOperationSlot.h), [Launcher service](../../Tools/Launcher/SparkleLauncher/Private/Gui/Operations/LauncherOperationService.cpp) | Replace single-job execute/cleanup graphs with proved native submission/completion. Keep the Editor's polled typed UI result and Launcher's run-ID/callback/busy/retry policies with their owners; do not merge distinct lifetimes into a shared operation manager. |
| [Launcher process execution](../../Tools/Launcher/SparkleLauncher/Private/Operations/LauncherOperationExecution.cpp) | Delete `TaskContextProcessRunner`; pass standard cancellation into existing [ChildProcessRequest](../../Engine/Core/Public/Process/ChildProcess.h) producers. Reuse Core process-tree supervision/output/readiness, [progress writer](../../Tools/Support/ToolSupport/Public/ToolWorkProgressWriter.h), [codec](../../Tools/Support/ToolSupport/Public/ToolWorkProgress.h) and [Launcher stream assembly](../../Tools/Launcher/SparkleLauncher/Private/Gui/Operations/LauncherProgressStreamDecoder.cpp). |
| Cooked/artifact publication | Reuse [FileSetPublisher/FileSetReadScope](../../Engine/Core/Public/Files/FileSetPublication.h) through final worker access and [artifact-bundle publication](../../Engine/Application/Private/Editor/Artifacts/ArtifactBundlePublication.cpp)'s completion-last new-directory transaction. Keep their distinct contracts; add no task-owned publisher or duplicate low-level file/hash/path helpers. |
| [GPU-crash writer](../../Engine/Application/Private/Diagnostics/GpuCrash/GpuCrashIncidentSink.cpp) | Delegate prepared runnable scheduling; retain bounded payload ownership and one sink notify/coalesce/close gate. Remove generic dispatch/scope machinery without adding a runnable queue, callback pool or waiter. |

Use the existing proposed caller/state ledger in the actual prototypes, then reconcile every conversion against its definitions, consumers and build membership. Inspect additions absent from the original inventory too. Do not recreate unused generic event/nesting APIs; preserve the actual range/join behavior through native primitives.

## Candidate behavior and proving conditions

These are unexecuted replacement acceptance checks; baseline passes do not satisfy them.

### Ordering, cancellation and lifetime

- Preserve chains/diamonds, independent branches, returned failure and thrown exceptions. Failure blocks affected consumers, permits independent work to settle, takes precedence over cancellation and retains useful attribution.
- Use cooperative domain stop intent for normal work and keep required cleanup outside native cancellation suppression. Cover rejection before admission and unexpected native invocation failure. Cancellation before body start must still release busy/admission state and invoke required completion exactly once; RAII inside a skipped body cannot do that.
- Preserve parent-to-child stop, child-only stop with usable parent/siblings, already-stopped admission and rejection after closure. Use standard stop sources/tokens/callbacks and existing owners, not a generic scope tree or competing native cancellation authority.
- Keep stop callbacks alive through child settlement. Exercise reentrant callback destruction/retry; invoke neither stop nor user completion while holding a lock it may acquire. Old completion must not clear or access the next operation's state. Keep GUI/coordinator-only work on its owner.
- Prove zero/finite/infinite host waits with one absolute deadline across children. Result readiness, callback/body exit and safe storage destruction are separate observations. Timeout retains live work and borrowed/owned captures; later join must settle them without a hidden blocking graph/future destructor, helper waiter or `std::async` workaround.
- Hold a terminal body after publishing its result, then test timeout, graph/future destruction and final release. Native completion counters can precede dependency processing: prove that nodes, dependents and captures are no longer accessed before reuse/destruction. Release unused prepared handles on idle close and keep native groups/contexts alive through physical settlement.
- Hold an earlier post-completion callback while a downstream terminal action completes. A native fan-in can become ready before earlier post callbacks return; it cannot alone certify graph/capture release. Publish successor inputs before their native readiness and retain shared inputs through every final reader.
- Close admission before drain/cancel shutdown. Drain finishes admitted work; cancel suppresses unstarted normal side effects while running work and required cleanup settle. Cover repeated shutdown and prohibit a worker joining its own capacity.

### Reentrancy, storage and serial execution

Audit locks, TLS, COM, floating-point state, borrowed scratch and exclusive leases across every native wait/nested algorithm. Control a case that would reenter a held non-recursive lock or overwrite TLS through unrelated queued work. Release unnecessary locks/nesting; prove a supported isolation boundary where needed. Do not hide the conflict with a new dispatch layer or lock substitution.

Distinguish required affinity from incidental old-pool placement before changing it. Native slots are not OS thread IDs or persistent scratch indices. Initialize COM on actual executing threads. Preserve stable item/chunk identity, bounded storage, non-overlap and ordered merge/publication across odd/large ranges, nested work and repeated reuse; native grain size may not bound a body range or imply one body per worker.

Retain the captured frame/background/I/O configurations `0/0/0`, `1/1/1`, `2/1/1`, `4/2/2` and product defaults, recording their exact upstream capacity mapping. With all counts zero, choose direct synchronous domain orchestration before initializing/submitting native asynchronous work. Use the same kernels in fixed dependency order; launch returns settled and bodies/finalizers stay on the caller. Test both inline and separate render-coordinator arrangements: `r.ThreadedRenderer` owns a different lifetime. Reject unsupported mixed counts/disabled lanes. Preserve ordinary zero-worker startup and fail-closed explicitly requested diagnostic preparation before SDK callbacks/readiness; never create a hidden task worker to rescue it.

### I/O capacity and frame progress

Specify numeric process-wide capacity for frame/background/I/O work, entering hosts, external callbacks, blocked waiters and upstream parallelism, including overlapping controls. Prove the upstream public mechanism behind the bound; separate arenas, names, priorities or a passing stress run do not establish dedicated frame capacity. Include process output/readiness waits, file reads, crash writes and cook memory throttles.

At smallest supported counts, defaults, worker sweeps and global limits, hold every admitted I/O blocker on a controlled latch and require a real frame batch/readback to finish before releasing it. Repeat under CPU-background pressure, cross-lane read/decode completion, cancellation, shutdown and dispatch notifications. A self-opening timeout or external deadline is failure, not progress. Account for waiting/coordinating tasks and enforce existing domain item/byte admission before native submission, releasing it once on every failure path.

Use existing external asynchronous completion through supported native dependency bridges where available, balancing every reservation on success/cancel/failure. Such a bridge does not make synchronous OS calls nonblocking. Reject designs requiring a new I/O pool, worker/waiter loop, callback scheduler, generic dispatcher or broad process/file rewrite to make the runtime fit.

### Prepared notification and callback teardown

Prepare/admit bounded dispatch storage before native callbacks can occur. Keep notification nonthrowing and its scheduling portion allocation-free on cold external threads and under pressure, separately from existing payload copies. No graph rebuild, executor/future per notification, warmed-allocator assumption, private native API or inline file drain on the notifying thread.

| Action | Required observation |
| --- | --- |
| Idle notify / queued notify | One queued invocation; later notifications coalesce. |
| Running notify | At most one accepted rerun; no overlapping invocation. |
| Idle close | Admission seals and settles without another notification. |
| Queued/running close | Later notify rejects; already admitted work and accepted rerun finish. |
| Cancellation/body failure | Admission closes, further reruns stop and accepted work settles; unstarted normal side effects are suppressed. |
| Closed/settled notify | False, with no new body or retained capture. |

Drive every phase, self-close, notify during drain, failure with rerun pending, parent cancellation, concurrent callback admission, delayed native callbacks and notification/admission/rearm allocation failure. Test far beyond any prepared-handle count; consumed or still-running native nodes cannot be reused as notification storage. Inspect the complete supported scheduling path as well as executable results.

Preparation failure rolls back before callback enablement. Failed notify marks collection incomplete and leaves storage settleable. Preserve payload count/byte bounds, bounded drain batches and final manifest publication. Disable/seal callback admission and wait for in-flight callbacks before writer/sink destruction; preserve the ten-second fatal retention policy on missed settlement. Swallowed exceptions, false success, lost wakeup, stranded accepted work or unbounded buffering fail adoption.

## Remaining stages

Work in isolated source/build/artifact roots and preserve concurrent edits. Finish stages 4–6 in the existing candidate; temporary mixed implementation cannot ship. Follow the [dependency workflow](../ThirdPartyMigration.md#ownership-and-dependency-workflow), existing checks and engineering rules; preserve existing tests and keep new probes local, removing them before submission.

Keep one ignored report tying each required code claim to its owner, actual upstream call/source contract, deterministic proving case, observed branch/counter, source/product/config/input identity, command and child exit. Record expected/actual outputs, diagnostics, thread IDs, task order, admission/parallel counters, cancellation, join duration and shutdown; distinguish callback completion, scope settlement and admission release. Use latches/barriers at admission, body start, result publication and final release; stress supplements controlled races. Catch hangs with an external deadline, release controlled work so storage settles, and retain the failure. Separate source guarantees, implementation observations and assumptions. An unproved safety/correctness contract blocks its dependent conversion; an outstanding broad benchmark or release observation does not. Reuse matching evidence and repeat only checks affected by changed inputs, failures or unresolved concerns.

### 4. Convert frame and world workloads

**Completed in the isolated delivery candidate, 22 November 2026.** Source identities, exact before/after bundles, native arguments, commands, retained failures and actual cost are in `build/validation/task-runtime/frame-20261122/REPORT.md`, `INTEGRATION-LEDGER.md` and `qualification-status.json`. Reuse `C:/s/sparkle-task-delivery-20261122`; loading/cook/operation callers and Tasks removal remain connected stages, so this candidate cannot ship yet.

- [x] Convert world/scene/view/recording with the public C task sets, dependencies and pinned work. Delete five graph/adapter files, the world count/graph cache and generic task-context entry points; retain typed kernels and direct serial order. Native ranges operate over bounded logical tiles: native minimum range size alone does not preserve a maximum partition count. Original grains, thresholds and partition/recording caps remain.
- [x] Compare actual Ozz camera/animation/pose/morph/skinning/transforms/extraction outputs at native 1/2/4 workers against direct execution; 62,617 assertions pass. Same owners grow/shrink and rebind generations while retained publication stays immutable. Held workers exclude structural edits. All ten world range/dependency preparation failures and a real failed pose settle storage, prevent publication and permit retry.
- [x] Run real Vulkan/D3D12 frames and readback with all five preparation/visibility ranges above their thresholds, persistent mesh/joint/morph/RT buffers and two native recording workers using distinct leases/partitions/contexts. Both coordinator arrangements, 1/1/1 with held mapped-I/O/CPU blockers and all-zero frame execution pass. Ordered merge/RT indices, moving-object continuity, visibility count changes and actual scratch/frame-slot storage reuse pass. Fix moved-from continuity input and stop failed recording before submission; wrong-thread access rejects and an open failed lease is discarded on its worker.
- [x] Prove registered render-constructor rollback/retry and physical worker join; retain registration until GPU/pipeline/host teardown. Actual shared Application/GameWorld/Renderer frame/readback uses one native DLL. Static/shared Game and shared Editor Application build with fixture injection removed; changed-source style/membership checks pass. Archive and remove local executable probes after qualification.

Stage 4 adds/relocates 512 lines and removes 618, net -106 across 35 owner/build files. Cumulative first-party additions/relocations are 1,301; maintained native additions remain 319. The first-party comparison is still net +287 with Tasks retained; eligible production savings are zero. The ledger identifies seven public execution declarations, sixty owner/prepared/local storage declarations and 125 boundary cleanup/rejection sites including earlier owners. The twelve-state/site targets are missed; complete-removal and performance budgets remain unqualified.

**Exit achieved for the recorded owner shapes.** No local scheduler, extra frame handoff worker or duplicated domain kernels remains in the converted paths. Full image/API/timing, global worker-limit sweeps, loading/cook/operation overlap, current-primary reconciliation and release/provider journeys remain unverified. Continue actual loading/residency/cook conversion; do not repeat runtime selection or a full cost matrix.

### 5. Convert loading, residency and cook batches

```text
Convert read/decode/finalization, residency preparation and shader/texture/scene cook batches directly.
Preserve existing admission, byte budgets, generations, publication and per-item results.
```

- [ ] Use native edges/joins/ranges and the proved cross-capacity completion bridge; bind work to intended native resources rather than merely renaming lanes. Add no per-edge queue/counter engine.
- [ ] Keep publication read scopes/source mappings alive through final worker access, two-asset in-flight admission, separate aggregate decoded allocation, stale-generation rejection, accepted-scene preservation and native GPU residency completion/retirement.
- [ ] Replace the three cookers' flat graph builders, task-handle vectors and per-node result lookups with upstream work over existing indexed item storage. Reuse producer dedup/equality, progress, actual-thread COM, memory leases and staged output; add no shared batch framework. Success/failure/cancel releases all reservations once; successful siblings retain their results and failed publication cannot overwrite accepted output.
- [ ] Compare real bounded loads/cooks and frame progress during loading. Exercise read/decode/publication cancellation, malformed compile/decode, stale completion after replacement, live-byte/backlog pressure and premature mapping release.

**Exit:** actual load/residency/cook owners preserve results, progress, accepted outputs, bounds and settlement with frame progress under overlap.

### 6. Convert operation lifetimes and remove Tasks

```text
Finish Editor/Launcher/process/incident conversions with the proved completion and callback teardown design.
Delete the entire old runtime and every replaced public/build/package surface before final acceptance.
```

- [ ] Reuse the converted Launcher and incident writer. Finish Editor operation lifetimes and remaining process/publication/incident wiring. Preserve child-process cancellation/output drainage, busy release, exactly-once completion, retry, GUI result delivery, parent teardown and crash callback admission/settlement; keep live storage on missed deadlines.
- [ ] Finish the remaining task-shaped public signatures in `LevelSession` and Renderer asset configuration; retain the native GameWorld/frame contracts completed in stage 4. Update producers/consumers and truthful CMake visibility, and remove executor/scope forwarding and redundant outcome translation. Private implementation must not leak through a replacement public handle facade.
- [ ] Delete `Engine/Tasks`, `SparkleTasks`, includes/exports, old configuration, generic scheduling/dependency/context/scope/event/dispatch APIs and copied/relocated equivalents. Update `Engine/CMakeLists.txt`, module/tool links, Application dependencies, Launcher registration, staging/package membership and `Docs/Rendering.md` navigation. Keep no alias, empty module or old/new selector.
- [ ] Reconcile every inventory row with a converted caller or retained domain invariant. Regenerate membership and build/stage in fresh candidate outputs so stale libraries/headers/DLLs cannot mask missing conversion. Preserve existing tests; adapt owned interfaces instead of deleting tests as residue without agreement.

**Exit:** every consumer uses the winner or direct synchronous kernels; complete removal and focused callback/cancel/timeout/retry/shutdown checks pass. Whole-change footprint accounting is reported with any deviations. Renamed first-party scheduler machinery fails removal.

### 7. Complete code delivery and close

```text
Finish code review, affected builds and focused owner correctness/removal checks.
Record broader unrun qualification honestly, fold lasting guidance and delete this plan once the code is complete.
```

- [ ] Build affected owner targets in the relevant development configuration, standalone Launcher and `ShaderCompilerCliValidation`; check a real shared-library caller when that boundary changes. Review backend/build membership, dependency visibility, owned-source style and `git diff --check`. Do not expand this into the full profile/API matrix without a new affected claim.
- [ ] Run focused actual-owner checks for changed scheduling, deterministic zero-worker order, cancellation/skipped-body cleanup, physical settlement, callback close/retry, publication and blocked-I/O frame progress. Reuse matching evidence; known correctness/lifetime failures must be fixed. Broader GUI journeys, both-API image/recording campaigns and hardware-provider qualification remain separate and must not be inferred from controlled software fixtures.
- [ ] Finish dependency identity, notices, staging and package-input changes in code, including removal of the old runtime from generated membership/imports. Record final source/product identities and the checks actually run. Full relocated prebuilt/offline/source rebuild-cook qualification and broad cost distributions belong to the existing report/release owner; leave unrun checks explicitly unverified.
- [ ] Audit actual calls from one frame to GPU retirement, one cancelled operation to last capture release, and every deleted task mechanism to its sole native primitive/domain owner. Reconcile the mechanism table, every caller row and all seven incoming native owners; inspect new definitions, public signatures, nested state, generated membership, package/imports and strings for hidden schedulers, mirrors, forwarding layers or duplicate process/progress/publication/budget controllers. Confirm that upstream owns the execution implementation and callers need less task plumbing, including code relocated outside `Engine/Tasks`.
- [ ] Explain the resulting ownership and caller simplification with the complete cost ledger, including missed numerical targets, unsupported claims and unverified measurements. Do not manufacture a savings/performance PASS or keep revisiting rejected designs solely to meet a number.
- [ ] Record the affected pin-upgrade checks for callback allocation/rearm, waits/destructors, cancellation, reentrancy, ranges/capacity and loaded-runtime identity. Remove obsolete workarounds rather than accumulate version branches. Once code review, focused checks and old-runtime removal pass, fold only lasting guidance into existing guides, remove incoming links and delete this file. Preserve the recorded qualification limits and unrelated release blockers; add no replacement acceptance plan.

**Exit:** the winner owns scheduling/dependencies, all caller conversions and required integration code are complete, Tasks and replacement facades are gone, and affected builds/focused correctness checks pass. Close and delete the implementation plan at that point. Unrun broader qualification remains explicitly unverified and is not a delivery blocker by itself; a demonstrated missing runtime, orphan process, live callback after destruction, stale publication, starvation, missed cleanup or second scheduler remains a code defect that prevents closure.
