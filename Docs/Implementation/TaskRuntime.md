# Task-runtime replacement: remaining delivery

[Delivery rules](../ThirdPartyMigration.md#code-delivery-and-qualification) | [Ownership](../ThirdPartyMigration.md#task-execution) | [Footprint scope](../RepositoryFootprint.md#task-runtime)

**Next: stage 5.** Reuse `C:/s/sparkle-task-delivery-20261122`, the [frame/world receipt](../../build/validation/task-runtime/frame-20261122/REPORT.md), [native fit](../../build/validation/task-runtime/runtime-reopen-20261121/REPORT.md) and [host/dependency inputs](../../build/validation/task-runtime/delivery-20261122/REPORT.md). Bind enkiTS v1.12 base `0289cf6ffce86697e1f46cb4eeca1c946bffec71` plus authorized patch SHA-256 `5b25c43e28c9023a6542a9f175f80e735eed4317cbe470d28af323007ac4d004`. Reconcile newer primary edits before publication; preserve the working runtime until all callers convert. The transitional candidate cannot ship.

## 5. Convert loading, residency and cook batches

- [ ] Convert read/decode/finalization and residency joins to the proved native resources/edges. Preserve two live asset admissions, separate aggregate allocation/residency budgets, mappings/read scopes through final worker access, generations, accepted scenes and GPU retirement.
- [ ] Replace ShaderCompiler/TextureCooker/AssetCooker flat graphs, handle vectors and per-node lookup with native ranges/dependencies over indexed results. Keep hash plus input equality, texture conflict policy, progress, actual-thread COM, processing/source byte leases, successful siblings and complete staged publication.
- [ ] Prove actual bounded loads/cooks and frame progress while admitted I/O/CPU blockers remain held. Cover malformed compile/decode, cancellation at read/decode/publication, stale completion after replacement, byte/backlog pressure and mapping lifetime. Release each reservation once; failed output cannot overwrite accepted output.

## 6. Convert operation lifetimes and remove Tasks

- [ ] Finish Editor and remaining Launcher/process/incident wiring using typed results, standard stop and proved native completion. Preserve UI thread/run identity, busy release, retry, exactly-once completion, child/output settlement and callback admission/teardown. Reuse converted Launcher/writer bodies.
- [ ] Finish `LevelSession` and Renderer asset public contracts; retain native GameWorld/frame contracts. Update callers, exports and dependency visibility, removing temporary executor/scope forwarding and outcome mirrors.
- [ ] Delete `Engine/Tasks`, `SparkleTasks`, API/DLL/package/configuration and relocated scheduling/graph/context/scope/event/dispatch equivalents. Reconcile every caller, fresh generated membership, imports/staging and navigation. Preserve existing tests and domain/GPU ownership.
- [ ] Check cancellation/skipped-body cleanup, parent teardown, retry, callback close/rearm and fatal timeout on actual changed owners. No renamed scheduler, compatibility facade or second process/progress/publication/budget controller survives.

## 7. Complete code delivery and close

- [ ] Build affected owners, standalone Launcher, a changed shared-library consumer and existing `ShaderCompilerCliValidation`; check style/readability, backend membership, exports and dependency/notices/package inputs against final identities.
- [ ] Verify changed-owner ordering, serial/worker parity, physical settlement, publication and blocked-I/O frame progress. Trace a frame through GPU retirement, a cancelled operation through final capture release and each retired mechanism to its sole native/domain owner. Reuse matching evidence and fix regressions.
- [ ] Reconcile complete native/non-native/fork accounting and pin-upgrade checks; review misses once. Remove probes through permitted routes, transfer broader unrun qualification to existing owners, repair incoming links and delete this plan once code and required closeout pass.

## Constraints for the remaining conversions

One upstream runtime owns queues/workers/stealing/wakeup/ranges/readiness. Keep typed kernels, admission, process supervision, publication and GPU fences/leases with existing owners. Use native dependencies/prepared completion directly; no graph interpreter, I/O pool, waiter thread, runnable queue, copied scheduler or replacement facade.

Preserve failure propagation to dependents, independent sibling progress, exactly-once cleanup, stop callbacks through child settlement and sealed admission before drain. Keep callbacks outside locks they can reenter; old completion cannot touch the next operation. Result readiness is not physical settlement: hold terminal bodies/earlier post callbacks to test timeout/destruction and retain storage until final native readers settle. Use one absolute deadline for zero/finite/infinite waits; no hidden blocking destructor or worker joining itself.

Audit TLS, locks, COM, floating-point state and exclusive scratch/leases at waits/nesting. Preserve stable item/chunk order and strict partition caps; native minimum grain does not bound callbacks. Cover `0/0/0`, `1/1/1`, `2/1/1`, `4/2/2` and defaults with exact native capacity accounting. Zero workers uses direct caller-thread kernels before asynchronous initialization; reject unsupported mixed counts. Keep inline/separate coordinator ownership distinct and fail explicit diagnostic admission without a rescue worker.

Prepared notification stays nonthrowing/allocation-free after admission, including cold external threads. Preserve idle/queued coalescing, at most one accepted running rerun, no overlap, self-close, failure/cancellation with pending rerun and rejection after settlement. Seal callback admission and wait for in-flight callbacks before destruction; keep payload/byte/batch bounds, final manifest and ten-second fatal live-storage retention. Reuse native/writer proof; exercise these cases where a converted owner changes them.

## Delivery constraints and fixed budgets

Keep the fixed gross scope of **3,017 code / 3,566 physical lines in 42 files** separate from net removal. Count all changed/relocated caller, adapter and build infrastructure plus maintained fork costs; no credit for moves, unrelated cleanup or upstream lines. The [ledger](../../build/validation/task-runtime/frame-20261122/INTEGRATION-LEDGER.md) owns observations/misses. Limits are review targets; fix defects/material regressions without quota-driven redesign loops.

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

Broad measurements remain separate qualification: 600 warm-up frames, 3,000 measured frames and five fresh processes per fixed API/config/input; paired run-level differences and correlated-frame 95% intervals, with a crossing interval INCONCLUSIVE. CPU improvement needs both >=5% and >=0.2 ms. Preserve equal worst-frame sample counts; charge upstream/build/package costs separately and follow Engineering's performance/image rules. Source/build proof does not certify GUI/GPU/package/release acceptance.

```text
Read Docs/Implementation/TaskRuntime.md and apply Docs/ThirdPartyMigration.md#code-delivery-and-qualification.
Complete the requested remaining conversion and prerequisites in the qualified candidate. Reuse matching evidence, finish actual owners and remove their old plumbing. Verify the exits, remove each completed stage/action immediately and continue through closure for full delivery. Preserve concurrent work and explicit qualification limits.
```
