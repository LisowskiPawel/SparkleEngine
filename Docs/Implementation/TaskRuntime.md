# Task-runtime replacement: remaining delivery

[Repository footprint](../RepositoryFootprint.md#task-runtime) | [Migration strategy](../ThirdPartyMigration.md#task-execution) | [Developer guide](../Engineering.md) | [Code style](../CodeStyle.md)

Temporary code-delivery plan. Remove completed actions as their evidence passes; delete this file after the code-completion exit below. Keep original commands, identities, results and rejected designs in the ignored reports. Broader product qualification does not keep a completed implementation plan open.

The deliverable is less task infrastructure to maintain so Sparkle can focus on its [three rendering cases](../Goals.md#the-three-cases). Follow the footprint study's [first retirement priority](../RepositoryFootprint.md#cuts-before-expansion): transfer the complete execution mechanism upstream and simplify its real callers. Its frozen scope is **3,017 code / 3,566 physical lines in 42 native files**, including the scene-preparation adapters; this is a gross retirement target, not measured net savings. Keep that denominator separate from later source freezes and all-infrastructure accounting. Graphics-share checkpoints are investigation context, not migration quotas.

**Next action — stage 2:** run the decisive executable enkiTS fit check on the frozen Launcher/incident owners, then convert callers if its supported ownership/capacity/lifetime design passes. Focused baseline binding is complete; outstanding distributions and package measurements do not block the prototype. Executable runtime qualification and production conversion remain unexecuted; `Engine/Tasks` and its consumers still exist.

| Stage | Work remaining |
| --- | --- |
| 2 — Runtime fit | Prove the remaining enkiTS mapping on actual operation/incident owners, including I/O capacity and physical settlement; review the complete integration and select one winner. |
| 3 — Dependency/composition | Integrate the winner through existing dependency, staging and package owners; implement supported linkage/offline inputs and verify the actual loaded runtime in focused host checks. |
| 4 — Frame/world conversion | Convert world, preparation, visibility and recording directly, preserving rendering ownership and deterministic caller-thread execution. |
| 5 — Loading/cook conversion | Convert loading, residency preparation and cook batches while preserving admission, byte budgets, generations, publication, COM and item results. |
| 6 — Operations/removal | Convert Editor/Launcher/incident lifetimes; delete Tasks, its API/target/DLL and all replaced caller/build/package plumbing. |
| 7 — Code completion | Finish affected builds, focused correctness checks and ownership/removal review; record remaining qualification separately, fold lasting guidance and delete this plan. |

The repeated focused baseline check passed in `build/validation/task-runtime/focused-baseline-20261121/REPORT.md` / `manifest.json`: source/toolchain/configuration, five owner fixture binaries, held-I/O frame fixture and 89 input files are bound to the existing observations. Its source archive distinguishes the corrected isolated baseline from current primary bytes, including 19 recorded baseline corrections and the newer JSON pin; reconcile differences and rerun affected owner checks before accepting the candidate. The original `current-20261121/REPORT.md`, `CALLER-LEDGER.md`, failures, unsupported cases and broader comparisons remain preserved. Use these matching fixtures for stage 2; do not restart the full baseline campaign. The sequential source preflight and excluded mappings remain in `build/validation/task-runtime/fit-20261116/REPORT.md`. Baseline behavior and source research do not qualify the replacement.

## Delivery constraints and fixed budgets

Follow [code delivery and qualification](../ThirdPartyMigration.md#code-delivery-and-qualification). Code completion requires the actual caller conversions, supported native primitives, correct cancellation/settlement/capacity, affected builds and complete removal of Tasks and replacement facades. Missing code, demonstrated races/hangs/starvation, premature destruction, false completion and material regressions remain blockers.

The numerical limits below remain fixed comparison and review targets, not automatic reasons to prolong a completed implementation. Report the complete accounting and any miss once, with its cause and impact; correct actual defects or needless infrastructure rather than repeatedly redesigning to hit a line count. Never compress code, hide moved costs or claim an unmet target passed. Full timing distributions, cold runs, profile/API matrices, image campaigns and relocated package/clean-machine checks are separate qualification: record **UNVERIFIED**, **INCONCLUSIVE** or an explicitly authorized **WAIVED / UNVERIFIED** and retain them with the existing report/release owner. Do not invent a waiver.

One upstream runtime must own runnable queues, workers, stealing, wakeup, range scheduling and dependency readiness. Existing owners retain rendering, publication, generation checks, resource budgets, process supervision and operation lifetime. Keep the Renderer GPU frame graph, coordinator, bounded frame admission, exclusive command leases, submission/barriers/fences and GPU retirement.

Delete the entire Tasks module, public API, `SparkleTasks` target/DLL and package surface. Remove generic graph/context/scope/event/dispatch machinery, padding graphs, redundant result/status translation, forwarding-only classes and obsolete configuration. Do not retain aliases, a compatibility facade, a dormant backend or a generic synchronous graph interpreter. Count relocated mechanisms as integration; earlier cleanup earns no migration credit.

Use the supported upstream facility directly before writing an adapter. The integration targets are ceilings to minimize, not allowances to fill. A bridge needs an actual consumer and a named ABI, resource or lifetime requirement that native/standard facilities do not already satisfy; keep it private to that owner. Unsupported fit is a reason to reject the candidate, not to rebuild Tasks around it.

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

Keep APIs/runs/machines separate. Before a performance claim, bind the bottleneck, inputs, warm-up/repeats and sample method; the retained extended protocol is 600 warm-up frames, 3,000 measured frames and five fresh processes per fixed API/config/input. It is separate qualification, not a prerequisite to stage 2. Use paired run-level differences and correlated-frame 95% intervals; crossing a limit is inconclusive. CPU improvement claims require both >=5% and >=0.2 ms. Preserve equal sample counts for worst-frame comparisons. Follow [performance and image rules](../Engineering.md#performance-and-images); worker/task residence, CPU work, GPU execution and presented latency are different measurements, as are logical bytes, allocator storage and process high-water.

## Remaining caller conversions

| Owner / call path | Required change and retained policy |
| --- | --- |
| [Application composition](../../Engine/Application/Private/Concurrency/ApplicationTaskRuntime.cpp), [GameApplication](../../Engine/Application/Private/Game/GameApplication.h), [GameWorld](../../Engine/GameFramework/Public/World/GameWorld.h), [Renderer configuration](../../Engine/Renderer/Public/Concurrency/RendererExecutionConfig.h) | Compose native capacity at the host/invocation boundary; remove task-only getters/parameters and the runtime service bag. Pass the actual upstream resource or standard stop token with truthful public/private visibility. No pool per operation or replacement facade. |
| [World systems](../../Engine/GameFramework/Private/World/Systems/GameWorldSystems.cpp) | Use native dependencies/ranges around the same camera, playback, pose, morph, skinning, transform and extraction kernels. Remove context unpacking and generic graph/result adapters; preserve structural-edit exclusion, stable indices and immutable publication. |
| [Scene preparation adapters](../../Engine/Renderer/Private/Scene/Preparation/RenderScenePreparationTasks.cpp), [merge/RT preparation](../../Engine/Renderer/Private/Scene/Preparation/RenderScenePreparationMerger.cpp), [visibility](../../Engine/Renderer/Private/View/RenderViewVisibility.cpp) | Delete scheduling-only adapters and capacity/padding graphs. Use actual typed spans and existing kernels for serial/native work; retain domain scratch/capacity reuse, ordered merge and RT planning. |
| [CPU recording](../../Engine/Renderer/Private/FrameGraph/Execution/FrameGraphRecordingExecutor.cpp) | Delegate chunk dispatch/join; retain coordinator-only and initialization work, exclusive lease/thread/partition ownership, native context bounds, ordered aggregation/submission and GPU retirement. |
| [Scene load work](../../Engine/GameFramework/Private/Level/Loading/SceneLoadWorkState.h), mesh/texture residency | Delegate read/decode/finalization edges and joins. Retain two live scene-asset admissions, the separate [aggregate allocation budget](../../Engine/GameFramework/Private/Assets/Loaders/CookedAssetAllocationBudget.h), [residency accounts](../../Engine/Renderer/Private/Resources/Residency/AssetResidency.h), publication read scopes, generations and accepted-scene protection. Remove competing task-status/result controllers. |
| ShaderCompiler / TextureCooker / AssetCooker | Replace flat graphs/handles with native ranges or real stage dependencies and existing indexed results. Retain producer hash plus input equality, [texture request dedup/conflict policy](../../Tools/Cooking/TextureCooker/Private/Requests/TextureCookRequestSet.cpp), progress, actual-thread COM lifetime, processing/source byte leases and staged publication. No second batch framework or scheduler-level dedup/limiter. |
| [Editor operation slots](../../Engine/Application/Private/Editor/Operations/EditorOperationSlot.h), [Launcher service](../../Tools/Launcher/SparkleLauncher/Private/Gui/Operations/LauncherOperationService.cpp) | Replace single-job execute/cleanup graphs with proved native submission/completion. Keep the Editor's polled typed UI result and Launcher's run-ID/callback/busy/retry policies with their owners; do not merge distinct lifetimes into a shared operation manager. |
| [Launcher process execution](../../Tools/Launcher/SparkleLauncher/Private/Gui/Operations/LauncherOperationExecution.cpp) | Delete `TaskContextProcessRunner`; pass standard cancellation into existing [ChildProcessRequest](../../Engine/Core/Public/Process/ChildProcess.h) producers. Reuse Core process-tree supervision/output/readiness, [progress writer](../../Tools/Support/ToolSupport/Public/ToolWorkProgressWriter.h), [codec](../../Tools/Support/ToolSupport/Public/ToolWorkProgress.h) and [Launcher stream assembly](../../Tools/Launcher/SparkleLauncher/Private/Gui/Operations/LauncherProgressStreamDecoder.cpp). |
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

Work in isolated source/build/artifact roots and preserve concurrent edits. Prepare stages 3–6 as one candidate; temporary mixed implementation cannot ship. Follow the [dependency workflow](../ThirdPartyMigration.md#ownership-and-dependency-workflow), existing checks and engineering rules; preserve existing tests and keep new probes local, removing them before submission.

Keep one ignored report tying each required code claim to its owner, actual upstream call/source contract, deterministic proving case, observed branch/counter, source/product/config/input identity, command and child exit. Record expected/actual outputs, diagnostics, thread IDs, task order, admission/parallel counters, cancellation, join duration and shutdown; distinguish callback completion, scope settlement and admission release. Use latches/barriers at admission, body start, result publication and final release; stress supplements controlled races. Catch hangs with an external deadline, release controlled work so storage settles, and retain the failure. Separate source guarantees, implementation observations and assumptions. An unproved safety/correctness contract blocks its dependent conversion; an outstanding broad benchmark or release observation does not. Reuse matching evidence and repeat only checks affected by changed inputs, failures or unresolved concerns.

### 2. Prove executable runtime fit

```text
With the focused inputs frozen, prototype the remaining pinned enkiTS mapping on actual Launcher completion and GPU-crash writer bodies.
Run decisive admission/lifetime/I/O checks first, review the actual integration cost, and select or reject the candidate promptly.
```

- [ ] Start from enkiTS [v1.12, 0289cf6ffce86697e1f46cb4eeca1c946bffec71](https://github.com/dougbinks/enkiTS/commit/0289cf6ffce86697e1f46cb4eeca1c946bffec71). Check maintenance, license and Windows/toolchain support before execution. Reopen an excluded oneTBB/Taskflow mapping only with a complete supported public-API argument; do not repeat an excluded design or use preview/private internals, a fork or relaxed preservation rules.
- [ ] Implement complete caller simplification for Launcher and the incident writer, applying the proposed ledger and mechanism table. Trial upstream C completion actions, dependencies, prepared pinned work and ranges before adding Sparkle equivalents; use one coherent supported native interface. The pinned-source findings are in `build/validation/task-runtime/current-20261121/ENKITS-SOURCE-REVIEW.md`. Map every behavior to the native primitive, retained domain owner/state, bound, failure path and proving case. Show which existing definitions/state disappear and justify each remaining bridge against the upstream facility. Count actual prototype additions/relocations, public surface, mutable fields and rejection/cleanup sites; generic demos or zero prototype lines cannot prove reduction. Reuse successful integration.
- [ ] Prove cold concurrent allocation-free notification, repeated idle/run/rearm/close, accepted rerun plus close, physical native completion and timed observation/destruction first. Specifically hold outcome publication before native completion/dependency release and forbid early reuse/destruction. Reject unsupported capacity/lifetime guarantees or extra scheduler/waiter machinery before converting easy ranges.
- [ ] Prove occupied-I/O real-frame progress at the smallest supported counts/global limits, with blockers still held at completion; include CPU-background pressure and actual read/decode/dispatch/cancel/shutdown paths.
- [ ] Cover the candidate behavior relevant to these first actual owners, including skipped-body cleanup, cancellation, deadlines, physical settlement and callback reentrancy. Preserve the remaining ordering/range/TLS/COM/serial checks above for their caller-conversion stages; do not require the full frame/cook/operation campaign before selecting the runtime.
- [ ] Complete the retained integration/publication model and review ownership in one frame path and one cancelled operation through destruction. Report actual infrastructure/API/state/failure-path accounting against the targets. Select one supported design and convert callers; do not run open-ended reduction experiments or a full cost matrix before conversion. Evaluate another candidate only after a concrete rejection. If none supports the required contracts, record failed adoption and keep the working implementation without adding another runtime.

**Exit:** one runtime passes the decisive owner behavior and supported I/O-capacity/lifetime checks, with a reviewed complete caller design and honest cost accounting. Reduced shapes qualify only those shapes; check remaining behavior as its actual owner is converted. Hidden serial workers, missed cleanup, premature storage release, false success, starvation, unbounded queues or replacement scheduler machinery reject the candidate.

### 3. Integrate the winning dependency and composition

```text
Integrate only the runtime accepted in stage 2 through existing dependency and product owners.
Prepare this with stages 4–6 as one candidate; no unused dependency or compatibility facade may ship.
```

- [ ] Register one verified source pin/cache through `FetchDependencies.cmake`, standalone Launcher allowlist and `SourceDependencyState.cpp`. Support individual Sync/Clean, full Sync Code and warm/offline reuse; keep upstream unmodified under `ThirdParty`, enabling only required components.
- [ ] Configure supported linkage/profile mapping, native capacities and standard stop lifetime at actual host/invocation owners; delete `ApplicationTaskRuntime` and its forwarding getters rather than replace the service bag. Scope upstream build options so they cannot change Sparkle targets; avoid allocator replacement, optional dependencies and duplicate runtime copies/pools. Check representative affected compositions; the full profile matrix is separate qualification.
- [ ] Update notices, runtime staging, package dependency ownership, SBOM and provenance. Verify independent Launcher/workspace configurations and actual loaded module paths/hashes/configuration after process handoff, including transitive SDK imports; link success or filenames cannot prove which runtime ran.
- [ ] Exercise affected acquisition/Sync/Clean failure handling in disposable caches, reusing unchanged existing workflow evidence. Preserve user caches/accepted artifacts; reject false readiness, pin/metadata disagreement, unrelated SDK requirements and stale PATH/app-directory resolution. Record unrun cold-fetch/offline variants without making them an all-configurations prerequisite.

**Exit:** both entry points resolve the same pin; affected host checks execute the intended single runtime with proved capacity/stop ownership and correct dependency/staging code. Required linkage/offline paths are implemented; broader unrun product/configuration campaigns remain explicitly unverified.

### 4. Convert frame and world workloads

```text
Convert world, preparation, visibility and recording directly to the winner using existing typed kernels.
Remove replaced task plumbing in each checkpoint and compare owner outputs before continuing.
```

- [ ] Complete the world/scene/view/recording rows in the caller table using native ranges/dependencies directly. Delete `RenderScenePreparationTasks.*`, task-context merger/visibility entry points, `AddSystemRange`/`ExecuteSystemTask` translation and graph-only padding/capacity caches as they are displaced; retain actual typed kernels, scratch/recording bounds and baseline serial thresholds. Native graph reuse needs measured justification and no mutation/reset while live.
- [ ] Implement all-zero deterministic domain order with the same kernels. Verify camera/animation/pose/morph/skinning/transforms/extraction dependencies, structural exclusion, stable indexing and immutable outputs across zero/default/multiple workers and both coordinator arrangements.
- [ ] Prove nontrivial preparation/visibility ranges, ordered merge/RT planning, capacity reuse and actual parallel recording: at least two frame workers and two eligible exclusive-lease chunks above the unchanged cost threshold. Include persistent scene-buffer resources, legal lookup/retention, exclusive lease partitions and retirement; transient-only examples cannot prove those contracts. Keep RHI authority, coordinator-only/initialization work and submission order with their owners; no incomplete result reaches submission.
- [ ] Check deterministic serial/parallel CPU outputs and focused backend/lease behavior for the changed paths. Include capacity changes, scratch/TLS races, stale captures, wrong-thread lease use and frame work under blocked background/I/O pressure. Record available D3D12/Vulkan smoke observations separately; full image/API/timing campaigns are qualification, not a prerequisite to subsequent caller conversions.

**Exit:** typed outputs/order, real parallel branches, storage reuse, ownership and submission/retirement pass focused checks, with affected backend build membership verified. No local scheduler or duplicated kernels remains. Unrun hardware/API campaigns stay unverified without blocking independent conversions.

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

- [ ] Complete the operation/process/publication/incident caller rows. Preserve child-process cancellation/output drainage, busy release, exactly-once completion, retry, GUI result delivery, parent teardown and crash callback admission/settlement; keep live storage on missed deadlines.
- [ ] Replace task-shaped public signatures in `GameWorld`, `LevelSession` and `RendererExecutionConfig` with actual upstream/domain contracts, update producers/consumers and truthful CMake visibility, and remove executor/scope forwarding and redundant outcome translation. Private implementation must not leak through a replacement public handle facade.
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
