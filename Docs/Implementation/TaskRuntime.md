# Task-runtime replacement

[Migration strategy](../ThirdPartyMigration.md#task-execution) | [Developer guide](../Engineering.md) | [Code style](../CodeStyle.md)

Temporary implementation plan. Delete this file when the feature closes.

**Status:** planning and delivery review only, 2026-11-08. All seven execution stages remain open; the next action is the unchanged baseline in stage 1. Inspection used `63f7aec15d0207658a9dd7b9ce061bb25b5f2153` with concurrent CLI and documentation edits present. No runtime trial, performance baseline or migration has passed. Reconcile live files and record affected file hashes before implementation. Reviewing this plan does not close an execution stage.

Replace the complete first-party task runtime with one maintained upstream runtime. The target is deletion of `Engine/Tasks/`, its public API and the `SparkleTasks` target/DLL, including its build and package references. At inspection this directory contained 43 files and 3,820 physical lines, including CMake. Count integration added elsewhere when measuring the reduction. Renaming or relocating these mechanisms cannot satisfy adoption.

Use the [ownership boundary](#ownership-and-removal) throughout: upstream owns execution machinery; existing engine owners retain rendering, publication, budgets and operation lifetime. Adopt the library only when the connected change removes the replaced responsibilities and their caller scaffolding.

## Runtime choice and reviewer familiarity

**Trial oneTBB first; choose by demonstrated fit and code removal.** Its documented use around rendering and content tools gives the strongest evidence here for familiarity to NVIDIA and AMD reviewers. This is an inference from visible projects, not a survey of reviewers or proof that it is the most-used game-engine scheduler. Reviewer familiarity is a tie-breaker after behavior, supported APIs, maintenance cost and measured overhead pass. Public repository stars, CUDA support and a vendor's use of a dependency do not establish suitability for Sparkle.

| Candidate | Relevant evidence | Decision for this engine |
| --- | --- | --- |
| [oneTBB](https://github.com/uxlfoundation/oneTBB) | [OpenUSD's Work library](https://openusd.org/release/api/work_page_front.html) documents its default TBB backend. NVIDIA's [OpenUSD integration](https://github.com/NVIDIA-Omniverse/usd-brep/blob/main/premake5-libraries.lua) explicitly links USD/TBB; AMD's [Blender USD/Hydra build](https://github.com/GPUOpen-LibrariesAndSDKs/BlenderUSDHydraAddon/blob/master/build.py) configures TBB for USD/ProRender. These establish ecosystem exposure, including older TBB integrations. | Preferred trial for CPU ranges, dependency graphs and future renderer/content growth. Modern oneTBB compatibility, cleanup and I/O isolation still require Sparkle evidence. |
| [Taskflow](https://github.com/taskflow/taskflow) | Upstream provides native DAG edges, parallel work, completion futures and [executors with their own workers](https://taskflow.github.io/taskflow/classtf_1_1Executor.html). Its [cancellation contract](https://taskflow.github.io/taskflow/RequestCancellation.html) allows running tasks to finish while suppressing successors. | First alternative if oneTBB requires excessive I/O or lifecycle machinery. Separate executors could isolate blocking work within one library; prove cross-executor completion/dependencies without building a dispatcher. Cleanup still needs explicit treatment. |
| [enkiTS](https://github.com/dougbinks/enkiTS) | Upstream documents range tasks, dependencies, completion actions, priorities, pinned tasks and dedicated I/O-thread examples. Its [public API](https://github.com/dougbinks/enkiTS/blob/master/src/TaskScheduler.h) exposes the scheduler/completion primitives. | A credible lightweight game-oriented alternative. Trial cancellation, hierarchical ownership, timed completion and external callback admission before assuming it removes enough local machinery. |

Start the oneTBB trial from [2023.2.0](https://github.com/uxlfoundation/oneTBB/releases/tag/v2023.2.0), commit [`41158a44ab3fb7ac5df7f99ef82626c87607447f`](https://github.com/uxlfoundation/oneTBB/commit/41158a44ab3fb7ac5df7f99ef82626c87607447f). Recheck maintenance, Windows/toolchain support and license when stage 2 begins. Use the selected release's source for semantic decisions: the online `/main/` documentation can describe a later release. Record any pin change in this plan and rerun affected trial cases.

Select one runtime only after stage 2 passes. If oneTBB fails, test Taskflow and then enkiTS against the same requirements, one at a time in ignored output. Record why each rejected candidate fails and stop investing in that design. Rewrite this plan around the winner before dependency integration. Taskflow's cancellation documentation also marks native cancellation of graphs using `tf::Semaphore` as unsupported; do not select an unsupported combination to implement admission or notification. If none removes the machinery while preserving behavior, record the failed adoption decision and keep the working implementation; adding a library alongside it achieves no cleanup.

CPU task completion and GPU queue completion remain separate. Keep the Renderer frame graph, recording leases, queue submission order, barriers, fences and deferred retirement. CUDA/HIP graph execution is outside this replacement; D3D12 and Vulkan remain the rendering APIs.

## Ownership and removal

One selected runtime and one authority per responsibility are mandatory. This table defines the boundary; the caller map applies it to concrete files.

| Responsibility | Single authority / retained engine policy | Replaced machinery to delete |
| --- | --- | --- |
| Runnable scheduling and partitioning | Upstream owns queues, workers, stealing, wakeup and range scheduling. Host composition sets the total capacity budget. Multiple native arenas/executors need proven isolation under that budget. | `Private/Scheduling/`, custom `ParallelFor`, per-module runtime facades and uncoordinated per-module pools. |
| Dependency readiness | Upstream edges/joins release work; domain owners declare meaningful stage order and required inputs. | `Private/Graph/`, `Public/TaskGraph.h`, local graph compilation/storage, handle validation and prerequisite counters. |
| Cancellation intent and lifetime | Each existing operation owner holds its authoritative stop source and children; standard tokens carry intent. Explicit native cancellation derives from that intent and cannot override cleanup. Native execution failure remains failure, distinct from user cancellation. | Generic `TaskScope` trees, mirrored cancellation flags and competing native/domain cancellation controllers. |
| Results and permission to destroy | The domain owner publishes one typed outcome after required cleanup and borrowed access settle. Native completion primitives observe execution; result readiness and safe destruction remain explicitly distinct. | Generic `TaskExecution`, type-erased context, duplicate result/status registries and polling mirrors of native completion. |
| Prepared notifications | The sink owns payload admission/coalescing/close and its bounded payload queue. Upstream alone schedules invocations. | Generic dispatch subsystem, local runnable queues and duplicate ready-state registries beyond the required sink gate. |
| All-zero reference execution | The same workload owner directly calls the same kernels and domain checks in deterministic order. | `SerialTaskExecution` and any second scheduler, graph interpreter or backend abstraction for the synchronous route. |
| Diagnostics and build integration | Upstream observability plus existing domain diagnostics; direct upstream target usage with truthful public/private visibility. | `Private/Profiling/`, `TasksAPI.h`, Tasks CMake/exports, `SparkleTasks` target/DLL and all registrations/package references. |

Before adding glue, identify the required invariant that native or standard primitives cannot express. Keep it in the existing domain owner; remove forwarding-only classes, empty runtime modules and speculative interfaces. `TaskEvent` and `AddNested` had no production consumers at inspection: verify and delete unused machinery without reproducing it.

For each mutable fact, identify one source and who may change it in the existing report. Copies/views needed by another lifetime must have a producer, publication boundary and invalidation rule; they cannot become another writable authority. Payload queues, resource budgets and generation checks remain domain responsibilities, distinct from runnable scheduling. Reject duplicate dependency-readiness bookkeeping, local runnable-selection engines and competing cancellation/result controllers around upstream.

Stage 1 fixes the reduction budget for total owned infrastructure, public surface and state/failure paths. Count caller-side futures, gates, caches and synchronous orchestration as well as the deleted module; exclude unchanged kernels and fetched source, reporting upstream build/package cost separately. Renamed/relocated mechanisms still count. Full module deletion and the ownership boundary are mandatory regardless of line count. Fill budgets before trial results and keep them fixed.

## Simplify the connected callers

Convert the complete affected call paths: work construction, execution dependencies, result consumption, lifetime closure, public signatures and build/package membership. Remove old API workarounds as their responsibility moves upstream. Each retained abstraction/state field needs a current consumer and domain invariant. Keep unrelated redesign and speculative features outside this migration.

| Connected code | Required simplification / delegation |
| --- | --- |
| [Application composition](../../Engine/Application/Private/Concurrency/ApplicationTaskRuntime.cpp), [GameApplication](../../Engine/Application/Public/GameApplication.h), [GameWorld](../../Engine/GameFramework/Public/World/GameWorld.h), [Renderer execution configuration](../../Engine/Renderer/Public/Concurrency/RendererExecutionConfig.h) | Compose upstream capacity at the actual host/invocation boundary. Remove executor/scope getters and parameters used only for forwarding. Pass the required upstream execution resource or standard cancellation token where a boundary needs it; keep product configuration and lifecycle at their owners. Avoid a replacement runtime service bag, per-module facades or a fresh pool per operation. |
| [World systems](../../Engine/GameFramework/Private/World/Systems/GameWorldSystems.cpp) | Express the actual system order and ranges with native upstream primitives. Remove generic context unpacking, custom range-to-node builders and redundant success/result translation. Preserve typed system work, structural-edit exclusion and immutable publication. |
| [Renderer preparation adapters](../../Engine/Renderer/Private/Scene/Preparation/RenderScenePreparationTasks.cpp), [merge/RT preparation](../../Engine/Renderer/Private/Scene/Preparation/RenderScenePreparationMerger.cpp), scene/view graph owners | Call existing kernels with typed references/spans. Delete scheduling-only wrappers/files; retain substantive merge and RT-planning algorithms with typed inputs. Use actual item ranges instead of padding execution to a cached graph capacity. Remove graph-only capacity state/caches when native range execution makes them unnecessary; retain domain storage/scratch bounds. |
| Scene/texture/mesh loading and residency owners | Delegate ready-work ordering, dependency release and joins. Keep one owner for each request's progress/result, cancellation and generation. Remove scheduler-status mirrors and task-handle lookup machinery; preserve publication scopes, resource reservations, stale-result rejection and GPU retirement. |
| ShaderCompiler, TextureCooker and AssetCooker batches | Use native range execution for independent jobs and native dependencies for real staged work. Remove graph construction for a flat job list and generic per-node result maps when existing indexed domain results suffice. Preserve compiler producer deduplication, per-item diagnostics, memory admission and publication. |
| [Editor operation slots](../../Engine/Application/Private/EditorOperations/EditorOperationSlot.h) and [Launcher process execution](../../Tools/Launcher/SparkleLauncher/Private/Gui/Operations/LauncherOperationExecution.cpp) | Replace single-job graphs with the appropriate upstream submission primitive. Keep busy/retry and typed operation-result lifetime with the existing owner, without a parallel generic task-result registry. Replace `TaskContextProcessRunner` with explicit stop-token propagation to the existing child-process request producers; preserve process supervision and completion routing. |
| GPU-crash writer | Keep its bounded payload queue and the required sink-owned notify/coalesce/close gate. Delegate runnable scheduling to upstream. Retain only state needed for payload ownership, admission and settlement; avoid additional runnable queues or redundant ready-state registries beyond the required coalescing gate. |

For each converted owner, record the upstream primitive taking over, deleted code/state/public surface and the reason for remaining glue. Retain domain scratch, admission and ordering limits. Use the same typed kernels and domain decisions for synchronous and threaded execution.

**Acceptance:** each owner reads as domain orchestration plus direct upstream work. Review constructor parameters, getters, wrappers, caches, result records and CMake visibility after conversion. A build that passes while obsolete caller plumbing remains does not complete that owner or the migration.

## Engineering review and handoff

Review against the [engineering goals](../Goals.md#target-persona), [analysis expectations](../Goals.md#software-and-analysis) and [archived role sources](../References.md#role-sources). Readiness requires implemented behavior and reproducible evidence, beyond library familiarity.

| Reviewer question | Required evidence in the existing ignored report |
| --- | --- |
| Why this change/runtime? | Engine need, delegated responsibilities, measured removal/cost and rejected alternatives. Separate measurements, source-supported guarantees and assumptions. |
| Is concurrency explainable? | Writer/readers, lifetime, publication edge/lock/atomic, lock order and final borrowed access. Shared ownership alone does not synchronize data. Include cancellation/callback race evidence. |
| What happens to rendering? | Equivalent-output before/after CPU preparation/recording, queueing, frame tails, submission and GPU idle gaps, including loading overlap. Microbenchmarks or unchanged GPU-bound frame time alone are insufficient. Bounded cost with maintenance reduction can justify adoption without a speedup claim. |
| Does it scale without more machinery? | Tiny/large inputs and worker sweeps, grain overhead, live-work/storage bounds and oversubscription control. Larger inputs must reuse upstream execution without extra caller boilerplate or state mirrors. |
| Can someone reproduce/maintain it? | Minimal commands/GUI steps, identified inputs/configuration, expected/observed results, source/artifacts, retained glue, limits, packaging and upgrade checks. Trace one frame and one cancellable operation through destruction. |

Use existing tools such as [Nsight Systems focused profiling](https://docs.nvidia.com/nsight-systems/UserGuide/index.html#focused-profiling) for CPU/scheduling regions and supported native captures for D3D12/Vulkan submission. Separate diagnostic capture from optimized timing and follow the [performance rules](../Engineering.md#performance-and-images), including measurement overhead. Reuse the same report and plan; resolve findings in owning files and rerun affected checks. Do not add a profiling dependency, permanent test framework or parallel review-document hierarchy for this migration.

## Behavior the design must preserve

### Ordering, failure and cleanup

[oneTBB graph cancellation](https://uxlfoundation.github.io/oneTBB/main/tbb_userguide/cancel_a_graph.html) prevents unstarted nodes from running. [An exception escaping a node](https://uxlfoundation.github.io/oneTBB/main/tbb_userguide/catching_exceptions.html) can cancel the graph too. Neither operation ensures a successor used for cleanup executes. Taskflow's cancellation has the same relevant successor hazard.

For graphs with required cleanup, use cooperative domain cancellation: normal bodies inspect a stop token, suppress their side effects when stopped or when required inputs failed, and still deliver their typed terminal outcome through upstream edges. Catch expected domain failures/exceptions at the work boundary. Independent branches retain the current opportunity to finish; failure blocks affected consumers. Aggregate failure takes precedence over cancellation and retains useful failure attribution.

Keep the required finalizer outside native cancellation suppression. In the proposed oneTBB design, the completion/cleanup path stays schedulable while domain work observes stop; do not call native graph/context cancellation on that path. Also handle rejection before admission and an unexpected runtime exception around the owning invocation. The owner publishes completion exactly once after running work, finalizers and borrowed captures settle. RAII releases acquired resources, but a destructor inside a body that never started cannot perform its operation-completion callback.

The concrete proof is [LauncherOperationService](../../Tools/Launcher/SparkleLauncher/Private/Gui/Operations/LauncherOperationService.cpp): its cleanup continuation clears `m_bOperationRunning` and invokes completion even if cancellation prevents the operation body from starting. Preserve that behavior, callback routing and retry admission. Never report cancelled/failed work as successfully published output.

Shutdown closes admission first. Drain finishes admitted normal work; cancel suppresses queued normal side effects and lets running bodies and required cleanup settle. Both close prepared dispatches and release captures before runtime destruction. Repeated shutdown must remain safe, and a worker must not join its own execution capacity.

### Parent/child ownership and timed settlement

Application, document/world, asset-generation and tool owners retain their actual children and cancellation lifetime. Forward a parent stop request to those children, close admission and join them before releasing parent data. Cancelling one child must leave its parent and siblings usable. A new request after closure is rejected before side effects. Keep stale-generation rejection and resource accounting with their domain owners.

Use standard stop sources/tokens/callbacks and the existing owner relationships rather than rebuilding `TaskScope` as another generic tree. [oneTBB contexts](https://uxlfoundation.github.io/oneTBB/main/reference/source/task_scheduler/scheduling_controls/task_group_context_cls.html) describe execution-context relationships; they do not automatically reproduce Sparkle's application/document ownership. Use explicit contexts and verify the selected source; do not rely on ambient nesting or construction-thread coincidence.

Keep stop callbacks alive for the full child lifetime, handle a parent already stopped at child admission, and exercise callback reentrancy/destruction races. Do not request stop or invoke user completion under a lock those callbacks can acquire. Reentrant Launcher completion may admit the next operation; settlement of the old operation must not clear or access the new operation's state.

The current consumers use zero-time polling, bounded joins and owner-thread teardown. `graph::wait_for_all()` is an unbounded wait, so it cannot implement a timed join by substitution. Use a domain completion signal/future and an absolute deadline for host observation. A timeout returns with the operation and all borrowed storage still alive; it grants no destruction permission. Close and settle children within the same deadline budget. Keep owned work alive through a later successful join and release captures before reporting settled. Workers express dependencies through upstream primitives instead of blocking on their own executor's completion.

Distinguish result readiness, callback/body exit and permission to destroy storage. Publishing a promise inside a terminal node does not prove the node has returned or the graph is quiescent. The [selected oneTBB graph destructor](https://github.com/uxlfoundation/oneTBB/blob/v2023.2.0/include/oneapi/tbb/flow_graph.h) itself calls `wait_for_all()`. A timed observation must not be followed by a hidden blocking destructor or destruction of live nodes/captures. Specify the last access to borrowed data, the object retaining unfinished work and the final release point. Exercise timeout followed by continued operation, successful later join and teardown. Do not add a helper thread or `std::async` waiter to conceal an unbounded wait.

### Deterministic caller-thread execution

When every worker count is zero, select the synchronous domain path before scheduling or initializing asynchronous work. Call the same kernels in fixed dependency order and use ordinary deterministic range loops. Submission/launch returns settled, every body/finalizer runs on the invoking thread, and no task worker or deferred callback is introduced. The invoking thread can be the render coordinator; `r.ThreadedRenderer` controls that separate ownership thread. Exercise both host arrangements without claiming all-zero task counts eliminate every engine thread. Remove `SerialTaskExecution.cpp`; fixed workload orchestration must not grow into another generic DAG interpreter.

An arena limited to one participant is insufficient evidence of this contract. In particular, [oneTBB enqueue](https://uxlfoundation.github.io/oneTBB/main/reference/source/task_scheduler/task_arena/task_arena_cls.html) can introduce a worker even when the configured worker limit is zero. Record actual thread IDs, operation order and returned settlement. Preserve the current rejection of prepared asynchronous dispatch when its lane has no workers. [Diagnostic startup](../../Engine/Application/Private/Startup/ApplicationGpuCrashStartup.cpp) must fail closed when explicitly requested preparation cannot succeed, before native callbacks/readiness are enabled; silently disabling requested diagnostics changes behavior. Do not silently create workers for the serial reference route. Preserve rejection of unsupported mixed counts and work assigned to a disabled lane.

### Blocking I/O and frame progress

The current runtime has separate frame, background and blocking-I/O workers. oneTBB arenas impose concurrency limits and share scheduler resources; arena priority and reserved application slots do not establish a dedicated I/O pool or guaranteed frame workers. [Global controls](https://uxlfoundation.github.io/oneTBB/main/reference/source/task_scheduler/scheduling_controls/global_control_cls.html) apply process-wide, with the smallest active parallelism limit winning. A per-arena budget must account for that effective limit.

The first trial must define a process-wide budget for every potentially blocking body and waiter, including process output/readiness waits, file reads, crash writes and cook memory throttles. Trial bounded I/O/background arenas with an enforceable cap that leaves frame capacity under every supported worker/global-limit configuration. Document the upstream mechanism supporting that claim and test it while I/O remains blocked. Include the waiting/coordinating tasks in the capacity calculation. Thread names and three arena objects cannot satisfy this requirement.

Record the admitted I/O/background maxima and remaining frame capacity numerically for the smallest configurations, defaults and worker sweep. Include external submissions, nested work, dispatch reruns and cross-arena coordination. An arena concurrency ceiling alone does not bound queued bytes or admitted operations; enforce existing domain admission limits before constructing/submitting upstream work. On failure, release admission exactly once without losing required completion.

Where a consumer already has external asynchronous completion, [oneTBB `async_node`](https://uxlfoundation.github.io/oneTBB/main/reference/source/flow_graph/async_node_cls.html) can connect it to dependencies. Pair every gateway reservation with release on success, cancellation and failure. That node does not make a synchronous OS call nonblocking or supply the external service. Keep blocking calls off frame workers. Do not introduce a homegrown I/O pool, callback scheduler or broad process/file-I/O rewrite to rescue the runtime choice. If the required guarantee cannot be obtained with small integration, fail the oneTBB trial and evaluate Taskflow's separate executors or enkiTS's pinned I/O facilities.

### Prepared notify, coalesce and close

The production consumer is [GpuCrashIncidentSink](../../Engine/Application/Private/Diagnostics/GpuCrash/GpuCrashIncidentSink.cpp). Prepare and admit its dispatch/storage before native callbacks become possible. Keep notification nonthrowing and bounded. Do not rebuild graphs or create executors/futures per callback. A serial upstream node alone does not provide coalescing or close semantics.

Preserve these observed transitions, including races ordered at the admission gate:

| State / action | Required result |
| --- | --- |
| Idle / notify | Admit one queued invocation. |
| Queued / notify | Coalesce into the existing invocation. |
| Running / notify | Request at most one rerun; invocations never overlap. |
| Idle / close | Seal admission and settle without needing another notification. |
| Queued or running / close | Reject later notifications and finish already admitted work, including a rerun accepted before close. |
| Cancellation or body failure | Close admission, suppress further reruns and settle accepted work; cancellation prevents unstarted normal side effects. |
| Closed or settled / notify | Return false; no new body or retained capture. |

Use a small sink-owned admission/coalescing gate with native upstream scheduling if needed. It must contain no generic runnable queue or dependency engine. Preserve the existing allocation-free scheduling portion of `Notify` after preparation, separately from the sink's existing payload ownership copies. Prove it for cold external callback threads, repeated idle/run cycles and resource pressure. The selected oneTBB [callable enqueue](https://github.com/uxlfoundation/oneTBB/blob/v2023.2.0/include/oneapi/tbb/task_arena.h) and [scheduled flow-node body](https://github.com/uxlfoundation/oneTBB/blob/v2023.2.0/include/oneapi/tbb/detail/_flow_graph_node_impl.h) paths allocate runtime task objects; constructing a node or warming an allocator is insufficient proof. An inline/lightweight body that drains files on the notifying native thread also violates the contract. Reject a candidate that cannot meet this requirement with small supported integration.

Every failed notification must remain nonthrowing, mark collection incomplete and leave storage able to settle. Preparation/admission failure must roll back before native callback enablement. Inject failure while a rerun or close is pending; a lost wake, accepted-but-stranded invocation or partial admission fails adoption. Completion releases callback captures before the owner can destroy the sink.

Close native callback admission and wait for in-flight callbacks before dispatch/sink destruction. Preserve payload count/byte bounds, the bounded drain batches, final manifest publication and the ten-second settlement checks. Validate zero-worker startup separately with diagnostics off and explicitly requested; preserve successful ordinary startup and fail-closed diagnostic rejection.

## Delivery stages

Stage 1 captures the unchanged baseline; stage 2 trials adoption. Prepare stages 3–6 as reviewable checkpoints of one candidate, then qualify it in stage 7. Use an isolated checkout and build/artifact roots to preserve concurrent work and the baseline. Reuse matching trial integration. Temporary mixed source during preparation cannot ship: the final call paths use one runtime, with no old/new selector, compatibility facade or dormant legacy backend.

At each stage exit, update this file with commands/results, source/executable/input identity, artifacts, changed/deleted mechanisms, failures and the next item. Apply the [connected-caller simplification](#simplify-the-connected-callers) requirements as each owner converts; retire its obsolete plumbing in that checkpoint. Keep logs and temporary probes in ignored `build/validation/task-runtime/`; remove new test-only code before submission and preserve existing tests. Format/check changed owned source and review ownership, names, logical paragraphs, evaluation order and unnecessary state by hand. The [adoption requirements](../ThirdPartyMigration.md#requirements-for-adoption) and [dependency workflow](../ThirdPartyMigration.md#ownership-and-dependency-workflow) apply throughout. Unrun checks remain open.

Keep one ignored report: case/owner, identified inputs/configuration, expected observation/budget, actual command/exit status, artifacts and verdict. Record capture release separately from result readiness. Harness-killed hangs fail; missing prerequisites remain unrun. Stages 3–6 remain prepared until checks pass on the assembled candidate; delivery requires stage 7. Changed inputs invalidate affected evidence only. Never mix receipts or binaries from different candidates.

### 1. Capture the baseline and all consumers

```text
Capture stage 1 of Docs/Implementation/TaskRuntime.md without changing production code.
Follow Docs/Engineering.md and Docs/CodeStyle.md. Inventory all task consumers and run the current behavior, cancellation, serial and capacity checks required by this plan.
Record identities, comparison budgets and artifacts; update the stage result and remaining gaps in the plan.
```

Record revision and relevant dirty-file hashes, executable/configuration/input identity, toolchain, CPU topology and graphics hardware/drivers where used. Capture zero/default/multiple-worker behavior, outputs, diagnostics, thread IDs, task order, cancellation, join duration and shutdown. Set numerical budgets for first-party reduction, build/startup/package cost, frame-time distributions, load/cook time, peak memory and cancellation latency before testing a replacement.

Include a trace-confirmed CPU-sensitive rendering case and the ordinary rendering configuration, plus a loading-overlap case. Fix input, resolution, renderer/API settings, warm-up, sample counts and repeats for each before/after comparison. Record the bottleneck and the predicted effect of delegation before interpreting timings. Keep worker/task wait time, CPU work and GPU execution intervals distinct; report per-run p50/p95/p99 and memory high-water under the existing measurement rules.

Choose bounded fixtures that enter the exercised branches: an animated/extracting world, nontrivial preparation/visibility ranges, more than two scene assets, residency pressure and representative shader/texture batches. Empty covers startup, not these workloads. Record the actual admission/parallel counters or trace observations. Classify existing behavior failures as baseline defects or unsupported inputs; do not silently call them correct, fix them inside the migration or use a failing case as the correctness oracle. Record any necessary behavior decision before conversion.

| Consumer family | Preservation evidence required |
| --- | --- |
| [World systems](../../Engine/GameFramework/Private/World/Systems/GameWorldSystems.cpp) | Camera, animation/pose/morph/skinning, transforms and extraction dependencies; structural-edit exclusion; stable indexing and immutable publication. |
| [Scene preparation](../../Engine/Renderer/Private/Scene/Preparation/RenderScenePreparationGraph.cpp), [visibility](../../Engine/Renderer/Private/View/RenderViewPreparationGraph.cpp), [recording](../../Engine/Renderer/Private/FrameGraph/Execution/FrameGraphRecordingExecutor.cpp) | Range results, merge and RT-plan ordering, capacity reuse, exclusive command leases, coordinator-only recording and submission order. |
| [Scene loading](../../Engine/GameFramework/Private/Level/Loading/SceneLoadTaskGraph.cpp), mesh/texture residency | Read → decode → finalization; two-asset in-flight admission plus the separate aggregate allocation budget; publication read-scope lifetime, stale generations, failure/cancellation and accepted-scene preservation. |
| Application Editor operations and Launcher operations | Shader compilation, capture/reference artifact work, process output/progress/cancellation, completion exactly once, busy-slot release, retry and parent teardown. |
| GPU-crash artifact dispatch | Prepare/notify/coalesce/rerun/close races, concurrent callback admission, allocation/write failures, final status and storage release. |
| [ShaderCompiler](../../Tools/Shaders/ShaderCompiler/Private/Cooking/Compilation/ShaderCompileBatch.cpp), [TextureCooker](../../Tools/Cooking/TextureCooker/Private/Cooking/TextureCookBatchExecutor.cpp), [AssetCooker](../../Tools/Cooking/AssetCooker/Private/Cooking/AssetCookerSceneBatch.cpp) | Bounded concurrency/memory, producer deduplication, per-item results, progress, thread-local COM lifetime, staged output and failed-publication protection. |

Search all owned sources, public headers, scripts, CMake and package inputs beyond this starting inventory. Record actual consumers of per-node results, scope kinds, events, nested tasks and ETW data before removing capabilities. Existing render-coordinator threads, bounded frame admission and GPU retirement serve different lifetimes; assess their integration without sweeping them into scheduler deletion.

**Complete when:** executable evidence covers every consumer family and the complete deletion/retention map, budgets and supported worker configurations are recorded. **Failure checks:** missing fixtures/tools, unobserved callbacks, source-only cancellation claims or an unmeasured serial route leave the baseline open.

### 2. Prove the runtime fits before adopting it

```text
Run stage 2 of Docs/Implementation/TaskRuntime.md in an isolated checkout, keeping the primary checkout unchanged and storing artifacts in ignored validation output.
Trial the pinned oneTBB release starting with prepared dispatch, timed destruction, I/O headroom and cancellation cleanup. Use real owner bodies where feasible; synthetic graph examples only supplement consumer proof.
Record the adoption verdict and evidence. If oneTBB fails, evaluate alternatives sequentially and update this plan around one winner before proceeding.
```

Run the cheapest decisive checks first. Start with the callback scheduling path, timeout/destructor lifetime and smallest I/O-capacity configuration before broad graph, build or performance comparisons. Inspect the pinned source and prototype the actual Launcher completion and GPU-crash writer paths in the isolated checkout. If a required guarantee needs unsupported internals, a fork, extra worker/waiter machinery or a relaxed preservation rule, reject the candidate before converting the easier range loops. Source inspection of upstream internals is evidence; production integration uses supported public APIs.

Map required behavior to the upstream public primitive, retained domain policy/state, bound, failure path and proving case. Audit it against the ownership table: no responsibility may acquire a competing controller. Count prototype integration against the budget and reuse successful changes. Reduced graph shapes prove only those shapes; full product qualification remains in stages 3–7.

Exercise successful chains/diamonds and independent branches; returned failure and thrown exceptions; cancellation before start, during execution and just before completion; cleanup that must run despite skipped normal work; parent → child stop, child-only stop and admission after closure. Verify zero/finite/infinite host waits, timeout followed by eventual settlement and destruction of borrowed/owned captures. Check both drain/cancel shutdown and repeated shutdown. Exercise native cancellation separately to demonstrate the successor hazard rather than claiming it implements Sparkle cleanup.

Control race ordering with latches/barriers at admission, body start, result publication and final release. Use an external deadline to catch hangs, then release controlled blocking work so storage can settle and the harness can exit. Repeated stress supplements these deterministic cases. Specifically hold a terminal body after producing its result and assert the owner cannot yet destroy its graph/captures; include timeout followed by graph/future destruction. Observe callback completion, scope settlement and admission release separately.

Trace the complete caller-thread route with all counts zero and repeated deterministic results. For threaded mode, block I/O on a controlled latch and require a real frame batch to finish **before releasing that latch**; repeat under CPU-background pressure, smallest supported counts and global limits. Include cross-lane read/decode completion, cancellation, shutdown and dispatch notifications while capacity is occupied.

Drive prepared dispatch through idle/queued/running notification bursts, close in each phase, self-close, notification during drain, failure with rerun pending and parent cancellation. Check bounded storage, no overlapping invocation, exact completion and callback teardown. Inject notification/admission allocation failure and delayed native callbacks. Inspect the selected upstream APIs/source as well as results: a passing stress run cannot establish an unsupported capacity or lifetime guarantee.

**Complete when:** one candidate passes the behavior matrix, has a defensible I/O-capacity design and meets the reduction/cost budgets with no local scheduler/dependency engine. Record the proposed retained integration, shared-state/publication model and decision rationale against the review questions before adoption. **Failure checks:** hidden workers in serial mode, skipped completion cleanup, timeouts that release live storage, false success, unbounded buffering, frame starvation or extensive replacement infrastructure reject the candidate.

### 3. Prepare dependency and composition changes

```text
Prepare stage 3 of Docs/Implementation/TaskRuntime.md after the adoption trial passes.
Use the existing dependency workflow, the selected pin and the demonstrated capacity/lifetime design. Follow Docs/Engineering.md and Docs/CodeStyle.md.
Prepare stages 3–6 as one migration candidate for stage 7 qualification; do not land an unused runtime or a compatibility facade. Record build/dependency evidence and remaining work.
```

Register the chosen dependency through `FetchDependencies.cmake`, the standalone Launcher allowlist and `SourceDependencyState.cpp`; use one verified source pin/cache, individual Sync/Clean, full Sync Code and warm/offline reuse. Keep upstream unmodified and under `ThirdParty` build grouping. Configure only required upstream components and update notices, package dependency ownership, SBOM/provenance and runtime staging.

Keep cold fetch, wrong/dirty revision and interrupted Sync/Clean checks in the disposable checkout/cache. Do not clean the user's shared dependency cache or overwrite accepted artifacts for a failure probe. Verify the dependency closure from both independent entry points; the isolated trial's successful configure cannot establish Launcher readiness or package staging.

For oneTBB, use upstream `TBB::tbb` and a shared upstream runtime in both Sparkle static and shared builds. Its [selected CMake source](https://github.com/uxlfoundation/oneTBB/blob/v2023.2.0/CMakeLists.txt) explicitly discourages unsupported static oneTBB linking. Set `TBB_TEST`, `TBB_EXAMPLES`, `TBBMALLOC_BUILD`, `TBBMALLOC_PROXY_BUILD`, `TBB4PY_BUILD` and `TCM_BUILD` off unless the accepted trial establishes a need. Scope `BUILD_SHARED_LIBS` to upstream composition so it cannot change Sparkle targets. Avoid allocator replacement, optional binding dependencies and a second runtime copy across engine DLLs. Verify actual Debug/Release mapping and runtime filenames instead of assuming the engine profile name matches upstream.

**Complete when:** independent Launcher and workspace configurations resolve the same pin; missing/wrong/dirty caches behave correctly; final composition supplies the proven capacities and stop lifetime; and static/shared consumers resolve one runtime. **Failure checks:** readiness without required headers/license/runtime, dirty-cache destruction, metadata/pin disagreement, unrelated SDK requirements during single-dependency sync and duplicate schedulers/DLLs remain blockers.

### 4. Convert frame and world workloads directly

```text
Prepare stage 4 of Docs/Implementation/TaskRuntime.md using the stage 1 baseline and stage 2 design.
Replace world, scene/view preparation and CPU recording task graphs/range partitioning directly with the selected runtime. Implement the deterministic synchronous path with the same kernels.
Run owner-sized checks and comparisons; record removed mechanisms and evidence before continuing the combined migration candidate.
```

Convert typed world-system dependencies, renderer scene preparation/merge/RT planning, view visibility and recording ranges. Keep actual work together with its owner; remove type-erased task contexts, generic result lookups, scheduling-only adapters and compiled graph caches made unnecessary by upstream algorithms. Preserve useful native graph reuse where measurements justify it, with no concurrent mutation/reset of a live graph. Keep domain storage/admission bounds and baseline serial thresholds; replace graph-padding and custom task partitions with actual ranges and native partitioning while preserving scratch/recording ownership limits.

Prove the parallel recording branch is entered: enable parallel recording, disable diagnostic GPU timing for that check, supply at least two frame workers and two eligible exclusive-lease chunks above the existing cost threshold. A worker-count setting or a successful small scene alone is insufficient. Keep coordinator-only and initialization-lease work on its owner. Run separate serial and diagnostic-timing cases. Use bounded fixtures crossing the range thresholds to detect a replacement that silently falls back to serial everywhere.

**Complete when:** world/extraction/preparation results agree across zero/default/multiple-worker modes; caller thread IDs/order pass; no incomplete frame result reaches submission; and recording owners/leases remain valid on both APIs. **Failure checks:** stale captures after capacity changes, shared scratch races, changed range indexing, mutation during extraction, cross-thread use of an exclusive recording lease and frame dependence on blocking background work fail this stage.

### 5. Convert loading, residency and cook batches

```text
Prepare stage 5 of Docs/Implementation/TaskRuntime.md as part of the same migration candidate.
Convert scene read/decode/finalization, texture/mesh preparation and all three cook batch consumers directly to the selected runtime. Preserve budgets, cancellation, publication and per-item results.
Compare actual bounded loads/cooks and failure cases with the baseline; update the plan with evidence and remaining gaps.
```

Use native edges/joins and range algorithms. For oneTBB, construct/bind graphs in the intended arena; the [selected graph implementation](https://github.com/uxlfoundation/oneTBB/blob/v2023.2.0/include/oneapi/tbb/flow_graph.h) associates a graph with its arena, so merely tagging each body with a former lane name does not route work. Use the proved native asynchronous completion bridge for cross-arena stages. Do not emulate each edge with a local queue/counter. Preserve read-scope ownership through all workers, in-flight and aggregate decoded-memory bounds, residency tokens, staged outputs, compiler producer/result fanout and COM initialization on the actual executing thread.

**Complete when:** real scene/texture/mesh and shader/asset cooks preserve results, progress and accepted output; all budget reservations settle after success/failure/cancellation; and frame work progresses with loading active. **Failure checks:** cancellation during read/decode/publication, failed compile/decode, stale completion after level replacement, leaked admission bytes, successful siblings losing their per-item results and premature release of source mappings remain blockers.

### 6. Convert operation lifetimes and prepared dispatch; delete Tasks

```text
Prepare stage 6 of Docs/Implementation/TaskRuntime.md and finish the combined stages 3–6 candidate for final qualification.
Convert Application/Editor and Launcher operation lifetimes, child-process cancellation and GPU-crash prepared dispatch using the proved design. Preserve completion, timeout and callback teardown behavior.
Delete Engine/Tasks, SparkleTasks and every replaced API/build/package reference. Report complete removal and retained integration before final qualification.
```

Replace task-shaped public parameters with the actual required upstream or domain contracts, update every producer/consumer and declare truthful CMake visibility. Keep domain stop/generation state with the owning operation. Preserve process-tree cancellation, output drainage, busy-slot release, retries and GUI-thread result delivery. Close native crash callback admission before releasing its writer state. Retain live storage on a missed settlement deadline and surface the existing failure policy.

Remove the entire Tasks directory and target. Update `Engine/CMakeLists.txt`, affected module/tool link lists, Application runtime dependencies, standalone Launcher registration and `sparkle_declare_runtime_dll_owner`. Remove obsolete task configuration, includes, exports, copied graph/scheduler machinery and package artifacts. Update `Docs/Rendering.md` source navigation and lasting concurrency/dependency guidance to the actual owners. Do not keep aliases, forwarding task headers, an empty module or an old-backend switch.

Delete the module only after the inventory shows every consumer converted and the retained integration has been reviewed by responsibility. Preserve the frozen baseline outside the candidate; it supplies comparisons without a selectable legacy backend. Regenerate all affected target membership. Build and stage from a fresh candidate output tree so stale `SparkleTasks` binaries, headers or transitive link inputs cannot hide incomplete removal. Preserve existing tests and adapt their owned interfaces where needed; do not delete them as module residue without agreed retirement.

**Complete when:** every production task consumer uses the selected runtime or the explicit synchronous domain route; cancellation cleanup, parent/child teardown and prepared dispatch pass; regenerated builds/package lists contain no `SparkleTasks`; and the total integration meets the recorded reduction budget. **Failure checks:** cancellation before Launcher starts, close during native callbacks, timeout followed by teardown, reentrant completion/retry, dispatch failure during close and shutdown with work queued must settle correctly. A facade with deleted filenames but retained scheduler mechanics fails removal.

### 7. Qualify the final workflow and close

```text
Complete stage 7 of Docs/Implementation/TaskRuntime.md on the final combined migration files.
Run the baseline comparisons, full failure/capacity/dispatch matrix and affected workflow/package checks required by this plan. Inspect the final implementation for relocated scheduling/dependency machinery.
Report exact evidence, code reduction, costs and blockers. Close only when required checks pass, then fold lasting guidance into existing guides and delete this plan.
```

Run final owner builds for GameFramework, Renderer, Application, Launcher, TextureCooker, AssetCooker and ShaderCompiler in the affected supported configurations. Include a real shared-library consumer and static-engine build, standalone Launcher, `ShaderCompilerCliValidation`, the architecture boundary check, owned-source style review and `git diff --check`. Search tracked source/CMake/package inputs for all former task APIs/exports and inspect the new upstream call paths; a string search alone cannot detect a renamed scheduler.

Exercise Launcher GUI/shell → cook/run, Editor shader recompile with symbols on/off → Launcher → ShaderCompiler, scene replacement while loading, residency under pressure, and bounded D3D12/Vulkan runs through actual command recording/submission. Test child failure/cancellation, application/window close during work, GPU-crash writer preparation/drain/close and explicitly requested diagnostic rejection in zero-worker mode. Synthetic sink callbacks prove sink behavior; the diagnostic product's real provider enable/seal path requires its own observation. Compare deterministic CPU outputs and the appropriate image/numerical tolerances with the same baseline inputs. CPU agreement cannot establish GPU correctness.

Verify relocated offline prebuilt launch and matching-source rebuild/cook, notices, runtime staging and dependency Sync/Clean/recovery. Re-run the blocked-I/O frame-progress case on the final files. Compare frame-time distributions, load/cook cost, startup/shutdown, memory, build time and binary/package size against the predeclared budgets. Report measured regressions, uncertainty and unrun hardware/workflows without turning them into passes.

Reconcile concurrent work and freeze final source/dependency hashes, executable/package identities and fixtures. Review the report against every consumer, preservation rule, removal target, budget and [review question](#engineering-review-and-handoff). Trace actual submission, dependency release, cancellation and completion against the [ownership table](#ownership-and-removal), including renamed code outside `Engine/Tasks`. Reject duplicate controllers, mirrored state, forwarding layers, graph-only caches and hidden schedulers. A prototype or previous checkpoint cannot close missing final-owner/provider/package evidence.

**Complete when:** required behavior and workflow comparisons pass on identified final inputs, one upstream runtime owns scheduling/dependencies, `Engine/Tasks` and `SparkleTasks` are gone, the connected callers have shed their replaced responsibilities and obsolete plumbing, the whole retained domain integration meets the reduction budget, the engineering review questions are answered with reproducible evidence, and no required evidence remains open. Carry only lasting ownership/dependency/serial behavior and source-sensitive upgrade checks into existing guides, remove links to this temporary plan and delete it. Preserve existing release blockers; this migration does not establish complete release qualification.

**Failure checks:** a missing upstream DLL, offline download attempt, orphan child process, live callback after sink destruction, stale asset publication, starvation, missed cleanup, exceeded budgets or a hidden second scheduler prevents closure. Builds and upstream tests cannot substitute for these consumer results.
