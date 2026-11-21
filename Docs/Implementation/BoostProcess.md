# Boost.Process staged implementation plan

[Third-party migration](../ThirdPartyMigration.md#native-removal-estimates) | [Engineering](../Engineering.md) | [Stage 0 evidence](../../build/validation/boost-process/stage0-20261121/refinement-01/REFINEMENT.md)

**Stage 0 is ready for a bounded Stage 1 fit trial of R1. Production remains on Win32.** R1 uses Boost.Process v2 for creation/environment/process observation and Asio for reads/completion. Sparkle retains its existing argument serializer, synchronous child-pipe endpoint creation, job ownership, logs and readiness policy. This is the smallest candidate selected for investigation, not an adopted dependency or proven saving.

**Correction to the earlier decision:** R0, the argument-range/fully upstream pipe-creation mapping, remains rejected. That does not rule out reusing the existing serializer through the public native-command-line overload. R1 charges the retained code and tests the smaller boundary. Its mixed gross ceiling is **259 physical lines**, below the original **342 physical / 312 code lines**. R1 code lines were not recounted; accepted retirement is **0** and net benefit is unmeasured.

The refined inputs are bound to HEAD `2c14f97d32b4fe728700fb91da645e0a466ac4de` and 167 file hashes in the [refinement manifest](../../build/validation/boost-process/stage0-20261121/refinement-01/manifest.json). The [original report](../../build/validation/boost-process/stage0-20261121/REPORT.md) remains historical evidence. No Boost fit, build, runtime or package check has run. **Next action: Stage 1, slice A**, using the prompt below; do not start production conversion.

[Responsibility split](#responsibility-split) | [Guidelines](#positive-and-negative-guidelines) | [Failure/acceptance cases](#failure-modes-and-acceptance) | [Stage 0 design](#stage-0--deep-integration-design-before-implementation) | [Implementation prompts](#staged-delivery)

## Responsibility split

| Area | Transfer to Boost.Process / Asio | Keep in SparkleEngine |
| --- | --- | --- |
| Inputs and launch | Environment block, native process creation and stdio binding | Executable/cwd/arguments, existing Windows command-line serialization and override intent |
| Output | Adopted read handle, asynchronous reads and completion | Compatible endpoint creation, merged stdout/stderr, captured bytes, callback/log/progress meaning |
| Completion | Observe process, read and event completion | Success/failure policy and physical settlement before return |
| Cancellation | Await the cancellation event and cancel outstanding I/O | Standard stop token, non-throwing event signalling and whole-tree termination policy |
| Lifetime | Manage process/read/event handles after explicit adoption | Job/startup-attribute/temporary native-guard ownership and accepted handoff |
| Readiness | Provide process observation | Named event/value, durable file, validator and distinct Ready/Rejected/ReleaseWithoutReadiness outcomes |

R1 can retire the custom environment-block builder, selected CreateProcess orchestration, overlapped reader and output-event dispatcher. It retains two small existing mechanisms rather than claiming to replace them. The actual benefit must survive all new context, attributes, guards, cancellation, dependency and caller costs.

## Benefits, costs and alternatives

| Expected benefit | Cost or limit to establish |
| --- | --- |
| Upstream reads/waits replace the handwritten overlapped reader/event dispatcher | A caller-driven Asio context introduces completion and teardown state. No service thread or second scheduler is justified. |
| Upstream owns process creation and environment-block mechanics | R1 still owns argument serialization, a native endpoint bridge, job setup and handoff. Those lines earn no transfer credit. |
| One Core path serves Launcher, AssetCooker and shader workflows | Boost remains private; do not add Qt/Tasks dependencies to Core or expose upstream types. |
| Established source/tests and platform APIs provide inspectable mechanisms | Exact-pin Windows/MSVC, binary/build/memory/package costs remain unverified. Public source is not runtime acceptance. |

Boost 1.92.0 is separately compiled. Its supported CMake graph reaches Asio's Context/date_time support and other Boost modules; `std::filesystem` does not erase that closure. The inspected conservative CMake-reference closure has 52 modules including conditional branches, not 52 compiled libraries. Keep acquisition, configured targets, compiled targets, linked artifacts and runtime files distinct. [Process target](https://github.com/boostorg/process/blob/696b785c77352491ebe0e7c4a8dc348bbc9d5d53/CMakeLists.txt), [Asio target](https://github.com/boostorg/asio/blob/4fa4abee89a62fdeeccac2585caece625f40647e/CMakeLists.txt).

| Alternative | Decision |
| --- | --- |
| Current Win32 | Keep as production until a qualified replacement exists; retain as the baseline, without a new runtime qualification claim. |
| Qt QProcess | Reject for this common boundary: it would add Qt to non-Qt consumers or leave two implementations. Qt source is a study reference. |
| R0: upstream argument range / connected child pipe | Reject the observed range-encoder mismatch and do not presume an overlapped child writer supports ordinary synchronous tools. |
| **R1: native input + retained endpoint bridge + caller-driven Asio** | **Select for one local fit trial.** Exact retained scope and hard rejection conditions follow. |
| Shared context/service, patched upstream, local replacement reader/dispatcher | Reject: added authority/lifetime or comparable machinery without a current consumer. No alternate production backend is selected. |

This support-code migration adds no rendering capability, GPU performance or Linux support claim. Keep current roadmap priorities and release blockers with their owners.

## What would actually be replaced

The original P scope remains a historical mixed ceiling. R1's smaller count is tied to the existing files and subtracts retained scoped lines before any new-code costs:

| Existing owner | R1 action | Physical gross ceiling before costs |
| --- | --- | ---: |
| [WindowsArguments cpp](../../Engine/Core/Private/Process/Windows/ChildProcessWindowsArguments.cpp) and [header](../../Engine/Core/Private/Process/Windows/ChildProcessWindowsArguments.h) | Retire only `BuildWindowsChildEnvironment` (lines 40–86) and its declaration. Keep serializer/conversion/file structure: conservatively 52 of the original 100 lines. Charge the new override merge. | 48 |
| [WindowsLaunch](../../Engine/Core/Private/Process/Windows/ChildProcessWindowsLaunch.cpp) | Replace the selected `CreateSuspendedWindowsChild` scope. Charge all new job attributes, native-handle capture/adoption and diagnostics. Later removal of assignment/resume implementation gets no extra credit without a named scope. | 45 |
| [WindowsPipe cpp](../../Engine/Core/Private/Process/Windows/ChildProcessWindowsPipe.cpp) and [header](../../Engine/Core/Private/Process/Windows/ChildProcessWindowsPipe.h) | Retire `WindowsChildOutputReader` and its declarations. Retain `CreateChildOutputPipe` (lines 23–52) plus one declaration: 31 lines of the earlier 142-line scope. Keep captured-output/callback/log policy with output. | 111 |
| [WindowsOutput](../../Engine/Core/Private/Process/Windows/ChildProcessWindowsOutput.cpp) | Retire `ChildOutputEvent` and `WaitForChildOutputEvent`; charge the complete Asio read/process/event/cancellation lifetime. | 55 |
| **R1 mixed gross ceiling** | **342 − 52 − 31**; before other retained/new/caller costs | **259** |

Retain the [public contract/result](../../Engine/Core/Public/Process/ChildProcess.h), [dispatch](../../Engine/Core/Private/Process/ChildProcess.cpp), job/tree termination, [readiness owner](../../Engine/Core/Private/Process/Windows/ChildProcessWindowsReadiness.cpp), [child signal](../../Engine/Core/Private/Process/ProcessReadiness.cpp), [native handles](../../Engine/Core/Private/Process/Windows/WindowsProcessHandle.cpp) and diagnostics. Narrow their implementation only where an actual mechanism moves upstream.

[CommandLineUtils](../../Engine/Core/Private/Process/CommandLineUtils.cpp) remains the one shared escaping/current-argument authority. [RelaunchCurrentProcess](../../Engine/Core/Private/Process/ProcessRelaunch.cpp) preserves its separate raw-command-line/OS-lifetime contract; it is outside this count. Keep existing launch/output/readiness files; do not manufacture a facade or forwarding file family. Source moves require regenerated CMake membership.

## Real callers and workflows to preserve

| Path | Required behavior |
| --- | --- |
| [CMake requests](../../Tools/Launcher/SparkleLauncher/Private/Build/CMakeWorkflowProcessRequests.cpp) -> [process runner](../../Tools/Launcher/SparkleLauncher/Private/Operations/ProcessRunner.cpp) -> Core | Configure/build/sync arguments, cwd/environment, live bytes, logs, native failure and cancellation. Preserve command display using the existing quoting authority. |
| [Cook requests](../../Tools/Launcher/SparkleLauncher/Private/Cook/CookOperationProcessRequests.cpp) -> Core -> [AssetCooker child dispatch](../../Tools/Cooking/AssetCooker/Private/Dispatch/AssetCookerToolProcess.cpp) | Nested tree ownership and live console/exit behavior. AssetCooker currently returns the launched child's ExitCode rather than calling Succeeded(); do not silently change that caller contract. |
| [Editor shader compilation](../../Tools/Launcher/SparkleLauncher/Private/Editor/EditorShaderCompilation.cpp) -> Launcher -> compiler | Standard cancellation, durable log, missing-Launcher diagnostics and accepted-output publication/reload rules in their existing owners. |
| [Run-level request](../../Tools/Launcher/SparkleLauncher/Private/Levels/LevelRunOperationProcessRequests.cpp) -> readiness -> [GPU-crash validator](../../Tools/Launcher/SparkleLauncher/Private/GpuCrash/GpuCrashLaunch.cpp) | Product/API/profile/session/value identity; accepted child survives return and keeps logging; rejected child terminates; release without readiness remains a failed operation with child-owned incident finalization. |
| [GUI operation execution](../../Tools/Launcher/SparkleLauncher/Private/Gui/Operations/LauncherOperationExecution.cpp) -> [queued presentation](../../Tools/Launcher/SparkleLauncher/Private/Gui/App/LauncherBackend.cpp) | Callback stays on the invoking BlockingIo worker; existing byte copy/Qt queue/progress decoder remains. Tasks owns its cancellation adapter and gets separate retirement credit. |
| Shell / Git / vswhere | Same synchronous public API, captured output and errors. Dry-run verifies planning only; it cannot qualify child execution. |

`Succeeded()` remains launched, not cancelled, no failure reason, and Ready or exit 0. Current readiness wait precedence for already-signalled handles is process, readiness, cancellation. EOF and root exit are independent; descendants may still hold stdout after the root exits. These facts must remain explicit during conversion.

## Positive and negative guidelines

| Do | Avoid / reject |
| --- | --- |
| Transfer complete named mechanisms and charge each retained bridge by owner/consumer/lifetime. | Renamed/moved machinery, a second serializer/reader/dispatcher or an inflated gross-removal claim. |
| Keep one owning handle authority and one result; use public native adoption/release deliberately. | Wrapping the same handle twice, detaching on failure or assuming a job destructor closes the process handle. |
| Keep Boost/Asio private and retain the synchronous request/result boundary. | New public upstream types, Core depending on Qt/Tasks/Launcher, a service locator or process framework. |
| Use existing files and cohesive lifetime state; name phases/locals under [CodeStyle](../CodeStyle.md). | God contexts, boolean modes, one-method collaborators, dense setup expressions or cosmetic fragmentation. |
| Keep the stop callback non-throwing and non-allocating; serialize actual cleanup on the invoking context. | Allocating `asio::post`, logging/formatting or result mutation inside the stop callback; `io_context::stop()` as settlement. |
| Preserve the native synchronous child writer and durable readiness log. | Depending on a short successful overlapped write, creating separate output streams or changing file sharing/creation semantics. |
| Reuse matching source-bound evidence and focused failure tests; record numerical misses once. | Endless benchmarking/redesign for a quota, changing inputs after measuring or promoting source/build evidence to GUI/GPU/package readiness. |
| Keep generic CMake options/CRT scoped to the dependency; use existing cache and package owners. | Upstream patching, machine-installed fallback Boost, global flags, a second pin registry or silent dirty-cache replacement. |

## Failure modes and acceptance

The [refined checks](../../build/validation/boost-process/stage0-20261121/refinement-01/checks.json) bind vectors, injection points, counts, limits and future real-owner commands. All executable cells remain **UNVERIFIED**.

| ID | Negative case | Acceptance observation |
| --- | --- | --- |
| P1 | Wrong/dirty/incomplete closure, escaped override, mismatched CRT or source-required prebuilt use | Reject invalid source while preserving inspection state; root/standalone pin, options, metadata, notices and package inputs agree. |
| P2 | Quotes/backslashes/Unicode/empty arguments or environment differ; absent stdin/bad cwd mishandled | Actual child receives exact values from the **native full command-line path**; parent environment is unchanged and useful native failure classification survives. |
| P3 | Job/attribute/creation/adoption failure; parent already in a job; process executes outside ownership | Job association precedes execution; a two-entry job/handle attribute list works. No escaped child, leak or double-close through constructor failure. Observe actual creation in result fields; do not invent a process when setup failed. |
| P4 | Pipe fills, final chunk fragments, EOF/root order reverses or a descendant holds stdout | One combined stream reaches capture/callback/log with exact bytes; process/read completion settle separately. Expected native broken-pipe EOF is distinguished from real read failure. |
| P5 | Stop races launch/read/root exit/continuous output/final return | Non-allocating event callback and context cleanup terminate the owned tree and settle work within the applicable 10-second product limit. A 15-second observation watchdog is never a pass or new product timeout. |
| P6 | Early exit, validator throw/reject, release fails or destructor kills an accepted child | Preserve Ready, Rejected and ReleaseWithoutReadiness, including ready false/exit -1 for release without readiness; accepted child survives and continues durable logging. |
| P7 | Callback/log/async initiation throws or stop coincides with allocation failure | Failure remains useful; callback cannot throw across stop registration; every borrowed result/buffer/handle/context survives final completion. |
| P8 | Probe passes while actual caller, queued progress or shared Core boundary breaks | Verify the named build/cook/shader/discovery/handoff consumers and real DLL usage; no public dependency leak or changed callback thread. |
| P9 | R1 adapters/closure erase the benefit | Report connected native removal against **259**, full native/non-native/dependency costs and frozen local review targets. No net-benefit claim without observations. |
| P10 | Retired reader/dispatcher survives elsewhere or old code/member registration is falsely deleted | Named mechanisms are gone, retained serializer/endpoint creator stay honestly charged, regenerated membership and final evidence match actual sources, temporary test-only additions are removed. |

Existing local review targets remain proposed: positive connected native net; short-call p95 overhead at most max(5 ms, 10% of baseline); shared Core growth at most 1 MiB; settled private-memory delta at most 8 MiB for one invocation / 32 MiB for eight. Protocol: 10 warmups, 100 calls per run, three separate runs, four batches of eight, three repetitions per forced failure and a 4 MiB deterministic output payload. Keep bytes separate from chunk boundaries and OS-thread high-water separate from persistent per-batch growth. These are not adopted new product/release requirements. Acquisition/build/package costs have separate report-only units; broad cold/profile/API/image/clean-machine campaigns stay with the existing release owners.

## Staged delivery

| Stage | Deliverable | Current state |
| --- | --- | --- |
| 0 | Guide/source study, concrete R1 design, reduced scope and frozen checks | **PASS — refined design ready for one local fit trial; R0 remains rejected** |
| 1 | Ordered local executable proof of R1 | **READY / NOT RUN**; start slice A |
| 2 | Exact dependency/private linkage and distribution inputs | NOT STARTED; requires 1 |
| 3 | Upstream launch/environment with creation-time job ownership and readiness handoff | NOT STARTED; requires 2 |
| 4 | Upstream reads/completion/cancellation through real callers | NOT STARTED; requires 3 |
| 5 | Complete connected retirement, focused validation and code closure | NOT STARTED; requires 4 |

Each prompt executes **one** stage. Do not cancel or compete with another session's shared build; wait or use a truly isolated source/build/artifact setup. Preserve unrelated edits and existing tests. At every handoff update this table and remaining actions with the result, pin/source/configuration identity, evidence, missing/failed cases, invalidation conditions and exact next step. Keep original reports/manifests in ignored output; reconcile other pointers only when their actual state changed. Remove new temporary probe source, executables and test-only registrations before handoff, preserving original non-executable results. Do not create another progress registry or equate a passed stage with release readiness.

### Stage 0 — Deep integration design before implementation

**Completed design, not executable qualification.** The [refinement report](../../build/validation/boost-process/stage0-20261121/refinement-01/REFINEMENT.md), [reference index/snapshots](../../build/validation/boost-process/stage0-20261121/refinement-01/references.json), [input manifest](../../build/validation/boost-process/stage0-20261121/refinement-01/manifest.json) and [checks](../../build/validation/boost-process/stage0-20261121/refinement-01/checks.json) own the detailed delivery. The original rejection/counterexample is preserved, with its scope corrected to R0.

| Guide | Applied decision |
| --- | --- |
| [CodeStyle](../CodeStyle.md) | Cohesive `.cpp` owners, direct typed operations, meaningful locals, narrow private headers and manual readability review. |
| [Engineering](../Engineering.md#module-boundaries) | One authority per invariant/lifetime, private upstream composition, borrowed inputs kept through settlement, complete connected accounting. |
| [Goals](../Goals.md#north-star) | Serve rendering developers' reproducible build/cook/run path and leave less support maintenance; do not optimize dependency count. |
| [README](../README.md#guides) | One plan here; original evidence ignored; durable decisions in existing owners. |
| [References](../References.md#integration-precedents) | Pin actual source examples and state the lesson/limits; do not use popularity or a sample as acceptance. |
| [Rendering](../Rendering.md#sdk-integration-and-owned-research) | Preserve RHI, FrameGraph, independent Reference, shader publication and GPU lifetime. |
| [RepositoryFootprint](../RepositoryFootprint.md#cuts-before-expansion) | Count R1's retained 83 scoped lines and every new adapter; reject comparable machinery or forced LOC wins. |
| [Roadmap](../Roadmap.md#supporting-system-simplification) | Keep current rendering blockers/priorities and cancellation/offline obligations. Add no first-release prerequisite. |
| [ThirdPartyMigration](../ThirdPartyMigration.md#requirements-for-adoption) | Prove decisive fit before production, keep exact dependencies/delivery and separate code completion from broad qualification. |

**1. Existing integrations and callers.** Followed actual [CLI11 recipe/consumer](../../CMake/Dependencies/Targets/cli11.cmake), [JSON admission and direct parsing](../../Engine/Core/Private/Json/JsonInputAdmission.cpp), [DirectXTex private/options composition](../../CMake/Dependencies/Targets/directxtex.cmake), [spdlog compiled composition](../../CMake/Dependencies/Targets/spdlog.cmake), [cache](../../CMake/Dependencies/DependencyCache.cmake), [standalone Launcher](../../Tools/Launcher/CMakeLists.txt) and [package graph](../../CMake/Packaging/ReleaseProducts.cmake). Their useful precedent is upstream mechanism ownership with a small, justified retained boundary. Do not copy spdlog's public linkage where Boost has no public-type consumer. The original report traces both Core modes, six direct external call sites and indirect operation families.

**2. Best primary references and limits.** Prefer the following sources for the exact decision, using the stored hashes/pins rather than a general library feature list:

| Reference | Decision it supports / important limit |
| --- | --- |
| [Pinned Boost launcher](https://github.com/boostorg/process/blob/696b785c77352491ebe0e7c4a8dc348bbc9d5d53/include/boost/process/v2/windows/default_launcher.hpp) | Public native input, startup attributes and adoption ordering. R0's encoder counterexample does not exercise the native overload. |
| [Microsoft job-list contract](https://github.com/MicrosoftDocs/sdk-api/blob/docs/sdk-api-src/content/processthreadsapi/nf-processthreadsapi-updateprocthreadattribute.md), [Chen's creation-time job explanation](https://devblogs.microsoft.com/oldnewthing/20230209-00/?p=107812) | Assign the one owned job before execution; retain attribute values through list destruction. Do not infer broader creator-interruption/multi-job transactional guarantees. |
| [WriteFile](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile), [CancelIoEx](https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-cancelioex) | Preserve the synchronous child writer and wait for cancellation completion. |
| [Pinned Asio native adoption](https://github.com/boostorg/asio/blob/4fa4abee89a62fdeeccac2585caece625f40647e/include/boost/asio/basic_readable_pipe.hpp) | Public read-handle assignment/read/cancel; explicit raw-to-managed failure proof remains required. |
| [C++ stop-callback exception rule](https://eel.is/c++draft/except.terminate) | Keep the callback allocation-free and non-throwing; event observation performs cleanup on the invoking thread. |
| [Pinned CMake process chain](https://github.com/Kitware/CMake/blob/06cc1d04d8d5a4e13abdadfe20f6937787b1968e/Source/cmUVProcessChain.cxx), [its libuv endpoint selection](https://github.com/Kitware/CMake/blob/06cc1d04d8d5a4e13abdadfe20f6937787b1968e/Utilities/cmlibuv/src/win/pipe.c) | Study loop/resource boundaries and parent/child endpoint modes. Do not copy the chain framework, global job or best-effort ownership policy; no libuv dependency is selected. |
| [Pinned Qt Windows process source](https://github.com/qt/qtbase/blob/59c81a3c2247b821b9b84b4eb8d939b77e07e276/src/corelib/io/qprocess_win.cpp) | Study real stdio/environment/cleanup assumptions. Its overlapped endpoints do not prove ordinary synchronous-child compatibility; Qt remains outside Core. |

Boost superproject pin: `afdfa32505af73e3d208144b3f623f0096cb62b6`; Process `696b785c77352491ebe0e7c4a8dc348bbc9d5d53`; Asio `4fa4abee89a62fdeeccac2585caece625f40647e`; Boost CMake `7bce8efdc57134dc86e70ac3cd914e390a6cf30d`. CMake/Qt example revisions above are references, not new product dependencies. The advisory/CI scan is limited evidence; exact toolchain/closure acceptance remains unrun.

**3. Selected shape and state/lifetime.** Keep synchronous RunChildProcess and one caller-driven io_context. Retain the existing full UTF-16 command-line builder and compatible pipe endpoint creator. Use one job at creation, one managed process owner, one reader, one cancellation-event observer and the existing result. Root exit, EOF and cancellation are independent facts; retain only state necessary to settle them. Readiness keeps its native wait/tail/validator policy and does not require a second event framework.

**4. Concrete launch/settlement design.** Before launch prepare inputs, job and a two-entry JOB_LIST/HANDLE_LIST attribute list in the launch owner. Supply it through public startup_info; the pinned launcher's default one-entry list is insufficient. Use the public native-command-line/error-code overload with explicit executable and no-window behavior. A non-throwing hook captures native rollback identity; release that guard only after managed adoption succeeds. No resume-in-hook or child outside job control is selected. The existing pipe creator supplies a synchronous child writer; public Asio adopts its parent reader. A SetEvent-only stop callback wakes the context handler. Normal root-exit plus EOF must disarm the callback, cancel the otherwise pending event wait and drain its completion before return. Failure/cancellation must also settle every handler before event/buffer/context destruction. Readiness releases job kill-on-close plus managed attachment only after acceptance. The report gives the full ordering, native error cases and attribute storage lifetime.

**5. Final owner/dependency/retirement map.** Keep the existing Public API and private launch/output/readiness/arguments/pipe owners; remove the displaced mechanisms in place under the 259-line gross ledger. Retained policy/serialization/endpoint creation is explicit. Prefer the existing pinned modular Git-cache admission; an archive path needs equivalent integrity/preservation before selection. Use private static Process linkage, supported transitive targets, standard filesystem and the existing CRT with generic options scoped locally. No private upstream calls, patched source, second registry or fallback backend. The SDK contains JOB_LIST under its Windows-10 target guard; Stage 1 must check the effective compile target rather than assume it.

**6. Frozen readiness for the next stage.** The refined P1–P10 specification fixes vectors, guards, metrics, failure points and future commands. R0 is excluded; R1 is ready only for the ordered A/B/C fit below. Unresolved gates are named: native overload/argv[0], attribute coexistence, raw adoption, synchronous-child read, native EOF, stop/teardown, accepted handoff and whole-integration cost. Stage 0 has not passed any of them by writing the design. Preserve matching evidence and rerun changed inputs only.

The [documentation verification](../../build/validation/boost-process/stage0-20261121/refinement-01/verification.json) records valid local links/anchors, UTF-8/whitespace, matching reference hashes and unchanged source inputs. `git diff --check` passes. Native style/build checks are not applicable to this documentation-only refinement; executable qualification remains unrun.

```text
Execute or refine Stage 0 of Docs/Implementation/BoostProcess.md only. Read all nine named guides and the current refinement report/reference index. Trace the current source, callers, existing CLI11/JSON/DirectXTex/spdlog/cache/package composition and exact upstream public APIs. Reconcile R0's argument-range rejection with R1's explicitly retained serializer/endpoint scope; do not assume every line must move upstream or hide retained costs.
Verify the selected creation-time job, two-entry attributes, public native input/adoption and SetEvent-only cancellation against pinned primary sources. Record the consumer/state/owner/dependency/deletion maps, alternative decision, frozen checks and exact unresolved fit gates. Keep it a design stage: no production dependencies, code, probes or new product requirements. Preserve concurrent work. Before handoff update this plan's result, input/reference identities, remaining work and exact next action; do not claim executable or release acceptance.
```

### Stage 1 — Prove the decisive fit locally

Use the refined pin/design/checks and an isolated ignored local trial. Execute **one candidate in three stop-on-failure slices**:

| Slice | Proof | Stop if |
| --- | --- | --- |
| A — input and ownership | Actual child argument values, job/handle attribute coexistence, job membership before work, absent stdin, parent-in-job, native/managed-construction failure cleanup | Unsupported APIs, changed arguments, unowned execution, survivor, leak or uncertain adoption ownership |
| B — output and cancellation | Retained endpoint creator with Asio reader, exact combined bytes, both exit/EOF orders, descendant-held writer, native EOF, callback/log failures and non-allocating stop/settlement | Comparable local reader/dispatcher is needed, bytes are lost, callback can throw or state outlives return |
| C — handoff and full cost | All readiness decisions/post-return logging, actual Core-shaped caller expectations, supported root/standalone dependency/CRT/SDK and complete adapter ledger | Workflow loss, patch/fallback, second authority or unjustified comparable glue/cost |

**Acceptance:** all decisive slices execute on bound inputs, with exact results and explicit handle/callback lifetimes. Compile-only is insufficient. Keep failed/inconclusive evidence. A failure ends this candidate without production conversion; full cold benchmarking is not a prerequisite. Remove temporary probes before handoff, retaining original logs, identities and observations.

```text
Execute Stage 1 of Docs/Implementation/BoostProcess.md only, using R1 and refinement-01/checks.json. Start slice A: prove the public native full-command-line overload and job-at-creation/two-attribute/managed-adoption lifetime on real child observations. Do not reuse rejected R0 range encoding. Then, only if A passes, run B with the retained synchronous writer/Asio reader and SetEvent-only stop callback, exact bytes and complete settlement. Only if B passes, run C for readiness handoff, supported closure and the complete retained/new/caller ledger against the 259-line gross ceiling.
Use public immutable upstream source and isolated ignored inputs; no production recipe/caller changes or selectable fallback. Stop at a decisive failure and explain it. Preserve concurrent work/builds, follow the plan's common rules and remove temporary probes. Before handoff update the stage result, exact source/pin/evidence, missing cases, remaining work and next permitted action; do not claim later package/GPU/release acceptance.
```

### Stage 2 — Integrate the pinned dependency and delivery inputs

**Entry:** Stage 1 passed every decisive slice. Add the exact closure through one existing [dependency recipe](../../CMake/Dependencies/Targets), [selection](../../CMake/Dependencies/FetchDependencies.cmake), [standalone allowlist](../../Tools/Launcher/CMakeLists.txt) and [private Core linkage](../../Engine/Core/CMakeLists.txt). Keep source/build outputs owned and upstream unmodified. Scope standard-filesystem/static/CRT options, disable unused builds and verify the actual supported closure rather than pruning target dependencies.

Update Launcher identity/required files and sync/clean handling, notices and target-derived package inputs together. **Acceptance: P1** passes root/standalone and invalid/dirty/incomplete-cache checks; prebuilt use needs no source fetch. Broader relocated-package tests remain explicit. Process behavior has not yet been converted and earns no retirement credit.

```text
Execute Stage 2 of Docs/Implementation/BoostProcess.md only after the R1 Stage 1 exit passes. Integrate its exact verified closure through existing recipes, cache/selection, standalone Launcher and private Core linkage. Scope generic options/CRT, keep source unmodified, and update identity/required files, sync/clean metadata, notices and package inputs together. Verify P1 with actual root/standalone and invalid-cache cases; preserve prebuilt/source separation. Do not convert process behavior or grow a dependency framework. Preserve concurrent work/builds. Update this plan's result, evidence identities, remaining work and exact next stage before handoff; leave unrun broader qualification explicit.
```

### Stage 3 — Replace launch/environment and preserve handoff

**Entry:** Stage 2 supplies the verified private dependency. Convert both exit/readiness launch paths using R1's existing command-line serializer and public native-input overload. Replace environment-block mechanics, native creation and the old assignment/resume choreography with the proved creation-time job/attribute/adoption design. Preserve true creation/result/error semantics, durable readiness file flags and distinct release decisions. Retain serializer and endpoint creator honestly; their files are not whole-file retirement targets.

**Acceptance: P2/P3/P6**, launch-related P5 and actual consumer smoke checks pass. Inject setup/native/adoption/release/validator failures. Accepted children survive parent-scope teardown and continue logging. Keep both modes functional; old reader mechanics remain only until Stage 4. Review source membership, exports and private boundaries after changes.

```text
Execute Stage 3 of Docs/Implementation/BoostProcess.md only after Stage 2 passes. Convert actual shared launch/environment for both modes to R1: existing full command line, public native input, one job assigned during creation, two attributes, explicit raw-to-managed transfer and preserved result/native failures. Remove superseded block/creation/assignment/resume code without claiming retained serializer/endpoint files as deleted. Preserve all readiness decisions, durable logs and managed detach on accepted release. Pass P2/P3/P6, launch P5 and real-consumer smoke checks. Keep one handle/result owner and both modes functional. Follow style/membership rules, preserve concurrent work/builds and remove temporary checks. Before handoff reconcile result, source/evidence, remaining deletions and next action.
```

### Stage 4 — Replace reads/completion and verify real callers

**Entry:** Stage 3 runs both modes safely. Adopt the retained native read endpoint into Asio; replace WindowsChildOutputReader and ChildOutputEvent/wait dispatch with the selected context read/process/event completions. Preserve shared bytes, logs and caller-thread callbacks. Signal stop through the pre-created event, terminate the owned tree and settle every handler before return. Do not move the retired reader into a new adapter.

**Acceptance: P4/P5/P7/P8**, affected P6 and current style/membership checks pass through GUI/shell, nested cooker/shader, discovery and handoff consumers. Exercise repeated/concurrent lifetimes and inspect resource stabilization. Coordinate with Tasks without converting or crediting its separate adapter. All new native state/caller costs belong in P9.

```text
Execute Stage 4 of Docs/Implementation/BoostProcess.md only after Stage 3 passes. Keep the compatible endpoint creator and transfer its reader through public Asio adoption. Delete the actual overlapped reader and output event dispatcher; replace them with R1's read/process/event lifetime. Preserve merged bytes/logs/callback thread, allocation-free SetEvent cancellation, whole-tree termination and physical settlement. Prove P4/P5/P7/P8 and affected P6 on actual named consumers, including descendant-held stdout and callback/log/stop-teardown failures. Do not add a reader/dispatcher facade, service thread, fallback or new product policy. Follow style/membership rules, preserve concurrent work/builds and remove temporary checks. Update this plan's result, bound evidence, missing cases, remaining work and exact next action before handoff.
```

### Stage 5 — Review complete retirement and close the code initiative

Audit the whole connected change against Stage 0 and all nine guides: public/private direction, state/lifetime, filenames/owners, caller adapters, retained serializer/creator, old-mechanism deletion and generated dependency/build/package membership. Finish affected builds, real shared Core consumption and focused P1–P10 checks, reusing matching evidence and rerunning only invalidated checks. Apply source style per changed file plus manual responsibility review, doc links/UTF-8/whitespace and `git diff --check`.

Report `old scoped lines actually retired - new/relocated native integration lines`, with the retained 83 scoped lines already outside R1's 259 ceiling. Keep CMake/scripts, upstream/source acquisition, compiled/linked/runtime artifacts and memory/build/runtime cost separate. Review a numerical miss once; fix defects and needless machinery without redesign solely for a quota. Missing conversion, unsafe lifetime, a fallback or comparable glue blocks completion. A justified rejection restores only task-owned changes against the frozen baseline, preserving concurrent edits.

**Acceptance:** supported upstream ownership, complete caller conversion/deletion, dependency/notices/package inputs, affected builds and focused correctness/failure/lifetime are complete with a demonstrated net benefit. Remaining broad qualification has exact owners/statuses. Fold lasting decisions into existing guides, retain release limits with Roadmap and delete this temporary plan at code closure. Closing code does not certify package/GPU/release acceptance.

```text
Execute Stage 5 of Docs/Implementation/BoostProcess.md only after Stage 4 passes. Audit the complete R1 change against the Stage 0 source/design references, all nine guides and P1-P10. Verify actual caller conversion, retained serializer/endpoint accounting, deletion rather than relocation, sole state/handle owners, private dependencies and current build/package membership. Finish affected root/standalone/tool/shared-consumer builds and focused lifetime/failure checks with matching evidence. Report full native and separate dependency/build/runtime/package costs against the frozen 259-line ceiling and local review targets. Fix defects/needless machinery; report misses honestly without endless redesign. Run style/manual/doc/whitespace checks, remove temporary test-only additions and preserve concurrent work. Before handoff reconcile results, remaining qualification owners and next action. If the bounded code exit passes, retain lasting decisions and delete this plan; do not claim broader release readiness.
```
