# GPU Crash Debugging Implementation Plan

[Docs](../../../../README.md) / [Design](GpuCrashDebuggingDesign.md) / [Research](Research.md) / [Roadmap](../../../../Strategy/Roadmap.md#where-we-are-and-what-is-next)

**Status:** GCD-0/1 source gates complete. Next: [GCD-2](#gcd-2--collect-dumps-and-breadcrumbs). Executable/native proof remains GCD-4; Modern Sponza hang investigation stays paused.

**Order:** qualify inputs → preserve symbols → implement collection → connect launch/inspection → validate the integrated tooling. This plan supplies tooling for the remaining product checks; it does not close them.

## Execution Rules

Follow [Change Lifecycle](../../../../Engineering/Workflow/ChangeLifecycle.md), the [design's ownership and change budget](GpuCrashDebuggingDesign.md#change-budget-and-stop-rules) and the [research decision crosswalk](Research.md#decision-crosswalk). Extend existing owners, remove replaced paths and preserve unrelated work. No second diagnostics framework, permanent fault injector, renderer feature reduction or AMD dependency.

**No product builds between stages.** `GCD-0` through `GCD-3` end with source-review gates, not configure/build/link/cook/run commands. Static scripts and formatter checks are allowed. Compilation, executable fixtures and native tests are deferred to one integrated validation phase in `GCD-4`. Advancing a source stage never means its native acceptance criteria passed; record deferred checks explicitly. This trades early compiler feedback for uninterrupted integration and makes the final build a required gate.

### Stage Review Gate

Before advancing, make the final owned changelist ready for submission under [Review and Acceptance](../../../../Engineering/Workflow/ChangeLifecycle.md#review-and-acceptance). Formatting alone does not close review findings:

1. Review every changed producer, consumer, lifetime and CMake membership against the design. Check headers/API signatures against pinned inputs; remove dead paths, forwarding wrappers, duplicate state and orchestration detail. Follow neighboring names, includes and class responsibilities.
2. Apply the [stage source-style gate](../../../../Engineering/Workflow/ChangeLifecycle.md#stage-source-style-gate) to **every added/modified source, header and shader, including untracked files**. Run `CMake\CodeStyle.bat -Mode Check -Path "<file>"` for each applicable path; do not stage user changes to manufacture coverage. Record manual review and any non-applicable files. A docs-only stage records source style as not applicable.
3. Run `git diff --check`, local documentation link/anchor checks and affected static owner checks. When Renderer/RHI boundaries change, run `cmake -DSPARKLE_REPO_ROOT="<repo>" -P CMake/ArchitectureBoundaryCheck.cmake`; this requires no product build. Check explicit optional-source membership, public-header dependencies and obsolete names.
4. Record the exact revision plus dirty-patch identity, reviewed files, commands/results, unresolved failures and checks deferred to `GCD-4`. Missing required static tooling leaves this gate open. Recheck overlapping changes before handing off.

Keep records and disposable fixtures under ignored `artifacts/validation/gpu-crash-debugging`. No Markdown execution diaries or permanent test framework. Native results also identify profile/API/product, compiler/SDK/tool/driver hashes, executable and shader bundle, oracle and artifact hashes. File final `PASS`, `BLOCKED`, `EXCLUDED` or `SUPERSEDED` in owning FCR reports; static review cannot pass native criteria.

| Stage | Deliverable | Prerequisite | Exit type |
| --- | --- | --- | --- |
| [GCD-0](#gcd-0--qualify-inputs-and-freeze-hooks) | Pinned inputs and frozen hooks | Design/current source reviewed | Input inspection + source review |
| [GCD-1](#gcd-1--preserve-exact-shaders-and-symbols) | Exact shader/debug export implementation | GCD-0 source gate | Source review; emitted artifacts unqualified |
| [GCD-2](#gcd-2--collect-dumps-and-breadcrumbs) | Private native collector and bounded publication | GCD-1 source gate | Source review; callback/runtime behavior unqualified |
| [GCD-3](#gcd-3--connect-launch-and-inspection) | Shared launch/readiness/inspection workflow | GCD-2 source gate | Source review; executable journeys unqualified |
| [GCD-4](#gcd-4--prove-native-crash-debugging) | Integrated build and matched native dump-to-source evidence | GCD-0–3 source gates | All seven design criteria |

## GCD-0 — Qualify Inputs And Freeze Hooks

**Complete:** pinned SDK/compiler receipts, variant membership and the hook ledger are recorded in the [design](GpuCrashDebuggingDesign.md#frozen-inputs-and-api-boundaries). The [prepared dispatch contract](../../MultithreadedEngine.md#prepared-task-dispatch) is ready for implementation with the writer in GCD-2.

## GCD-1 — Preserve Exact Shaders And Symbols

**Complete in source:** actual DXC/Slang debug options, exact code/source/symbol export, archive/file receipts and transactional manifest publication. Backend PDB writes and duplicate identity JSON were removed. The [design](GpuCrashDebuggingDesign.md#shader-and-incident-identity) owns the export boundary; emitted artifacts, mismatch exercises and source mapping remain `CHK-GCD-03` work in GCD-4.

## GCD-2 — Collect Dumps And Breadcrumbs

**Goal:** implement collection below RHI and publication in Application, without adding a thread runtime or polluting rendering orchestration.

**Work:**

- Implement the [Tasks prepared dispatch contract](../../MultithreadedEngine.md#prepared-task-dispatch) with the Application sink as its first consumer. Preallocate one scoped lane slot before callbacks; preserve ordinary Launch guards, transactional preparation, allocation-free Notify, fair execution and close/cancel/shutdown settlement. No separate scheduler or parked worker.
- Add one private RHI session with backend-private Aftermath, D3D12 contexts/DRED and Vulkan configuration/checkpoints. Load the exact admitted DLL only for requested Aftermath mode; check required exports/identity before any device. Off and compiled-out paths must not acquire a required SDK import.
- At existing native shader creation, join the exact exported code SHA-256 to driver shader/debug identifiers and `.nvdbg` keys. Keep compiler lookup keys and CPU PE/PDB identity separate; absent native identifiers stay unavailable.
- Lower existing Begin/End scopes to copied D3D12 markers and immutable Vulkan semantic tokens. Extend the neutral scope-admission owner so baseline pass scopes work with timing, detailed markers, PIX events and validation off. Repeated frames cannot grow the dictionary; unknown frame/queue joins stay unknown. No vendor branches in passes or frame orchestration.
- Extend existing backend resource naming and supported SDK metadata updates. Preserve ambiguous aliases/missing data. Do not add an allocation journal, custom GPU buffer or unrelated validation features.
- Route native device-loss observations through the existing failure owner **before** Core's immediate abort. Trace submit/present/fence to the serial/render-thread host, collect only once and preserve the original terminal error. Failure finalization polls thread-safe sink/dispatch state there; no owner-affine join on the render thread or waits in recording/task workers. No global fatal hook, recovery, lost-device idle wait or generic wrapper around every API error.
- Implement the Application sink with one prepared SparkleTasks BlockingIo dispatch: bounded byte/count queue and bounded work per invocation, no task per callback or permanent worker. Reject requested mode when that asynchronous lane is unavailable. Contain exceptions/allocation failures at SDK C callbacks; never wait, decode, log recursively or perform synchronous disk I/O there.
- Implement the design's fresh session identity, state transitions, reserved-slot cleanup and manifest-last publication. SDK Finished, sink-borrow settlement and durable writes are separate obligations. Keep the RHI callback anchor, tokens, callback code and DLL until process exit: Disable is not a documented callback-join barrier. Seal sink admission and settle admitted borrows before destroying Application storage. Bound cooperative polling; do not promise to preempt a blocked SDK/OS call.

**Validate now:** source-review initialization/failure/teardown order, callback ownership, lock ordering, count/byte accounting, cancellation and publication paths; run static boundary checks and the Stage Review Gate. Prepare `CHK-GCD-01/04/05/06/07` fixtures, including preparation rollback, foreign-thread/worker-origin Notify, close/cancel/shutdown versus enqueue/finish, capacity reuse, serial/threaded host finalization versus owner-affine joins, repeated loss, delayed callbacks, 10,000 repeated recording cycles, overflow, ID collisions and failed writes. Run them in `GCD-4`.

**Phase exit criteria:** both backend source routes fit the design, no second worker/execution authority exists, and every failure has a truthful state and safe lifetime. Builds, asynchronous behavior, ordinary-build erasure and native collection remain unqualified.

```text
Execute only GCD-2 from Docs/Architecture/CrossModule/PerformanceDiagnostics/GpuCrashDebugging/ImplementationPlan.md after GCD-1's source gate.
Implement the private session, existing scope/resource hooks and native loss handling before abort. Use the GCD-0-qualified Tasks submission contract for bounded asynchronous publication; current callback-side Launch is forbidden. Audit callback exception containment, session identity, terminal failure and shutdown against the design; remove replaced paths.
Non-negotiable exit: pass the Stage Review Gate and static boundary checks. No intermediate build or fault run. Stop on unsafe callback/context/task/DLL lifetime, an unbounded queue or a second collector. Native and executable fixture checks remain deferred to GCD-4.
```

## GCD-3 — Connect Launch And Inspection

**Goal:** select one mode and expose the child's actual session without duplicating launch or diagnostic policy.

**Work:**

- Consume the returned `shader-export.json` path directly. Bind its hashes to source revision/dirty patch, product/profile/API and executable/CPU symbols in the final session bundle; reject stale or incomplete joins. No directory scan for a plausible export.
- Extend existing direct startup options and Launcher typed requests with Off/Aftermath intent. Reuse product-root discovery for diagnostic Editor/Game binaries; do not move the canonical Launcher into the variant or add a second Launcher.
- Validate selected profile/API, admitted SDK, symbol receipts, capture conflict and reserved output slot before device creation. Bind the shared manifest to a fresh session ID, the launched child and exact product/cook identity. Old, partial or other-child files cannot confirm readiness. Ready requires the child's successful native initialization, not a Launcher PID or requested setting.
- Present final collection state separately from source-mapping qualification. Reuse Open folder and an explicitly configured inspector handoff, with quoted/Unicode paths and no required file association or undocumented tool CLI. Verify bundle receipts and provide manual search-path setup where needed. Missing inspector preserves the incident. Direct launch writes the same manifest and preserves the terminal failure exit.
- Reuse consent-bound export and no-support policy. Default export contains sanitized metadata; raw dump, `.nvdbg`, source and symbols each require explicit category consent. Reuse optional PIX through its existing owner in a separate launch; active capture plus Aftermath rejects before devices.

**Validate now:** trace CLI/GUI to one request and child-owned state, review schema/identity/privacy and typed process arguments, remove replaced UI/options and pass the Stage Review Gate. Defer executable `CHK-GCD-01/06/07` journeys, including stale manifest and failed preflight, to `GCD-4`.

**Phase exit criteria:** one launch/result authority, explicit readiness and consent, and no SDK/tool installation in normal startup. Runtime equivalence and inspection remain unqualified; optional PIX absence cannot fail Aftermath.

```text
Execute only GCD-3 from Docs/Architecture/CrossModule/PerformanceDiagnostics/GpuCrashDebugging/ImplementationPlan.md after GCD-2's source gate.
Connect existing direct/Launcher requests to diagnostic product discovery, child-confirmed session state and inspection. Reuse consent/export and separate optional PIX. Keep the canonical Launcher SDK-free; reject stale identity and capture conflicts without duplicate launch semantics.
Non-negotiable exit: pass the Stage Review Gate with one launch/state authority and explicit privacy boundaries. Do not build or run between stages. Record GUI/CLI/readiness/privacy checks as deferred to GCD-4 and keep the hang investigation paused.
```

## GCD-4 — Prove Native Crash Debugging

**Goal:** validate the integrated implementation before using it on the unresolved scene.

**Freeze the matrix before running:** Windows x64 NVIDIA; `DevelopmentEditor`, `DevelopmentGame`, `ShippingEditor`, `ShippingGame`; D3D12 and Vulkan; exact tool/SDK/compiler/driver receipts. Debug profiles, other vendors and combined capture sessions are outside this qualification. Do not reuse Editor evidence as Game proof.

| Required route | Success / failure oracle |
| --- | --- |
| Integrated build and shader exports | Configure/build only named affected products/helpers after GCD-0–3. Inspect optimized raster/compute/ray-tracing outputs on both compiler backends; exported and loaded code hashes agree, full source-debug data exists. Missing Slang mapping blocks acceptance; no compiler swap. |
| Executable controlled negatives | Run all deferred `CHK-GCD-01/03/04/05/06/07` fixtures: missing/wrong SDK, revalidation after receipt/listed-input changes, absent BlockingIo lane, capture conflict, stale/cross-child manifests, scopes/reset/concurrency, byte/count/dictionary exhaustion, duplicate-ID mismatch, denied/locked/full/partial writes, cancellation and privacy. Run non-fault checks first; symbol-mismatch tests use the first valid dump with isolated inspector search paths/caches. Injected callbacks are not native proof. |
| SDK-absent ordinary builds | All four profiles configure/build without SDK headers/libraries/runtime. Check optional source exclusion and imports/dependency closure. Verify canonical Launcher and ordinary stage membership exclude the SDK/private artifacts; use the existing package verifier if a local stage is produced. No new public archive prerequisite. |
| Normal journeys | Three launch/quit cycles for each of the eight profile/API combinations, including direct and Launcher entry. Verify baseline breadcrumbs with unrelated tooling/validation off, Unicode paths, orderly drain and unused-slot cleanup. Record matched Off/Aftermath CPU/GPU/memory/storage costs separately from release performance. |
| Native faults | Compute and ray-tracing fixtures for each of those eight combinations: **16 required fault cells**. Real SDK callbacks produce hashed dumps; the inspector opens them and resolves exact shader/source plus known scope observations. Freeze expected shader/function and acceptable source-line range before each run; optimized code need not map to one unique instruction/line. |
| Native limitations and shutdown | Apply the [fault/evidence matrix](Research.md#fault-and-evidence-matrix). Check actual callback order, repeated loss, late data and writer drain. Use non-native fixtures for missing/ambiguous/external-reset facts; do not induce system-wide locks or crash other processes. CPU kill or absent fault/frame facts cannot manufacture attribution. |
| Separate PIX | One smoke capture when installed, through the existing capture route with Aftermath off. Absence is an optional limitation; a failed installed route is reported, not passed. |

Use opt-in, non-sensitive, disposable faults and no registry/TDR changes. Confirm a fixture survived optimization before inducing a fault. Start with normal/negative routes, then one native cell; expand only after its safety and publication checks pass. Predeclare a finite attempt count per cell; a non-reproduction stays unqualified, not an endless hang investigation. Arm an external local supervisor **before** submitting the fault, with a 30 s deadline that does not depend on a loss callback or submit returning. Killing that owned child yields a forced/incomplete result, never a successful 10 s collection. Stop further faults after an unrecovered reset or unsafe teardown.

`GCD-4` is one validation phase, not a ban on necessary repair builds. On failure, fix the owning source, repeat its Stage Review Gate, rebuild the smallest affected closure and invalidate dependent results. Do not build the entire workspace or cook all release maps merely to exercise this tooling.

Prefer private fixture content using the exact product modules. Record production module hashes separately from fault shader/fixture hashes. A modified test host cannot stand in for Editor/Game proof. Retain the fixture recipe/patch and hashes privately, then remove injection sources, shaders and executables before handoff. If removal changes a production module, rebuild the final unmodified candidate and repeat the affected checks; results from a different executable cannot be relabeled as final-candidate proof.

**Phase exit criteria:** every required cell and `AC-GCD-01` through `AC-GCD-07` has candidate-bound evidence in the owning reports; optional exclusions and limitations are explicit. A raw dump without matched source is insufficient. Unavailable required checks leave the plan open. Run the Stage Review Gate after any repairs and final cleanup. Keep native artifacts private; mark delivery briefly in the roadmap only after acceptance.

```text
Execute only GCD-4 from Docs/Architecture/CrossModule/PerformanceDiagnostics/GpuCrashDebugging/ImplementationPlan.md after GCD-0–3 source gates.
Freeze and execute the declared four-profile/two-API matrix. Perform integrated targeted builds, shader export checks, non-fault negatives and normal cycles before controlled compute/ray-tracing faults. Use the first valid dump for remaining symbol-mismatch tests. Verify exact code/debug/source and breadcrumbs against predeclared oracles. Bound attempts; external forced termination is incomplete. Repair through the owning source gate, then rebuild only the affected closure.
Non-negotiable exit: all seven criteria pass for the exact final candidate and every required matrix cell. Remove disposable injection material while preserving private reproduction receipts and final module identity. Do not substitute static/fixture/CPU evidence for native source attribution. Keep scene investigation paused until acceptance; report unavailable checks without claiming completion.
```

## After This Plan

Resume [PD-1](../../FirstRelease/ProductImplementationPlan.md#pd-1--game-and-editor-product-shells) with matched tooling: collect the Modern Sponza failure, inspect native shader/resource evidence, fix the owning defect and rerun affected journeys. Then regenerate the archive and continue PD-2/3/4 against its exact identity. Release obligations and final FR-07 signing remain unchanged.

After all tooling criteria pass, retain the design and useful Acceptance evidence, mark delivery briefly in the roadmap and remove this completed plan under the documentation lifecycle. Do not delete the unfinished product plan.
