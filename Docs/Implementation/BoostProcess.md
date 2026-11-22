# Boost.Process: remaining code acceptance

[Retained ownership decision](../ThirdPartyMigration.md#child-processes) | [Current termination/caller result](../../build/validation/boost-process/continuation-20261122-r2/termination-01/REPORT.md) | [Original delivery](../../build/validation/boost-process/continuation-20261122-r2/REPORT.md)

**The nested-cancellation defect is corrected and verified on production Core.** Changing only Core makes the original failing probe/tool executables pass: both retained tools signal before return. Caller corrections reject zero-exit settlement errors in AssetCooker and preserve cancellation diagnostics in Launcher. Shared/Debug/static Core regressions, real shader workflows and focused caller checks pass. Remaining: the latest root Launcher/AssetCooker rebuild, blocked by concurrent ShaderMake configuration work, and the earlier automatic rejection of trial cleanup. Completed stages remain outside this action plan.

## Remaining delivery

| Stage | State | Required exit |
| --- | --- | --- |
| [4 - Final caller qualification](#stage-4--final-caller-qualification) | Focused PASS; final root caller rebuild OPEN | Corrected Core passes strict before-return and actual workflow checks. Current standalone caller code builds and its result policy passes; rebuild latest shared Launcher/AssetCooker after root configuration repair. |
| [5 - Retirement and code closure](#stage-5---retirement-and-code-closure) | OPEN | Finish root caller qualification, verify identities/ledger, then resolve the exact-trial cleanup rejection through an approved route. |

Source identity is Boost 1.92.0 root `afdfa32505af73e3d208144b3f623f0096cb62b6`, Process `696b785c77352491ebe0e7c4a8dc348bbc9d5d53`, Asio `4fa4abee89a62fdeeccac2585caece625f40647e`, Boost CMake `7bce8efdc57134dc86e70ac3cd914e390a6cf30d`. [Current identities/results](../../build/validation/boost-process/continuation-20261122-r2/termination-01/result.json) bind four corrected Windows-owner files and five caller files. Earlier reports retain their products and unaffected scope. Concurrent shader/animation/task/Launcher orchestration work is preserved and excluded from task credit.

The [retained responsibility table](../ThirdPartyMigration.md#child-processes) owns the boundary. Reuse the [R2 design](../../build/validation/boost-process/continuation-20261122-r2/DESIGN.md), [original checks](../../build/validation/boost-process/continuation-20261122-r2/checks.json) and [readiness follow-up](../../build/validation/boost-process/continuation-20261122-r2/readiness-01/REPORT.md) only for unaffected inputs. The original [strict failure](../../build/validation/boost-process/continuation-20261122-r2/consumers-01/nested-cancel-strict-1.json) is preserved; the [same old-tools/new-Core comparison](../../build/validation/boost-process/continuation-20261122-r2/termination-01/production-strict-nested-cancel.json) now passes. Eventual exit, root/pipe completion or an empty job query remains insufficient.

Root/pipe/handler settlement cannot stand in for a descendant using a private output pipe. Preserve the synchronous public contract, full command-line lookup/serialization, compatible child writer, creation-time job membership, native rollback until managed adoption, accepted readiness handoff and caller-driven private context. Keep the stop callback `SetEvent`-only and settlement allocation-free. A comparable new local supervisor must be costed and may reject the boundary.

The [current ledger](../../build/validation/boost-process/continuation-20261122-r2/termination-01/costs.json) charges the 110-line termination correction and four net caller lines. Windows changes from 1,014 to 1,077 physical lines; including nine metadata and four adapter lines, connected C++ grows **76 lines**. CMake adds 68, giving **144 C++/CMake growth**; notices add 30. The unchanged 259-line historical credit is missed by conservative 479-line new/rewritten accounting. No LOC saving is claimed. The reviewed value is upstream environment/reader/dispatcher ownership plus demonstrated lifecycle correctness; the native job wait has no scheduler, service thread or PID registry. The measured Core DLL delta is **115,712 bytes** over the frozen baseline; no new performance distribution or cold/package claim is made.

## Positive and negative guidelines

| Do | Avoid |
| --- | --- |
| Use the unmodified exact pin and supported target closure through existing cache/package ownership | Upstream patches, private APIs, ad-hoc archive rules or a second dependency registry |
| Keep direct ownership and one caller-driven context; settle every handler before return | A second scheduler, service thread owner, root/EOF/result mirrors or returning while callbacks remain |
| Preserve actual arguments, synchronous-tool output and whole-tree/readiness semantics | Reusing R0's rejected range encoding, changing the child writer mode, post-creation ownership races or a fallback backend |
| Reuse matching executable evidence, preserve failures and report all retained/new costs | Compile-only acceptance, inferred GUI/GPU/package passes, scope expansion after measurement or quota-driven redesign |
| Observe retained process handles at return and account for spawn races, errors and timeouts | Treating an empty job list, zero active count, root exit, fixed delay or a later successful wait as complete tree settlement |

## Qualification outside code closure

Interactive Qt progress/control, Editor shader compile/reload, texture/scene cooking, responsive application/GPU readiness, death during shader publication, full profile/API/image/cold-cost matrices and relocated package/clean-machine/release acceptance remain **UNVERIFIED** with the existing Roadmap/Rendering/release owners. Executed shader publication/readback and failure preservation retain their recorded scope.

## Stage 4 — Final caller qualification

The public Launcher path invokes ShaderCompiler directly; the distinct nested path is AssetCooker `cook-shaders` -> ShaderCompiler. Both execute with genuine profile/variant bundles in a marked workspace with spaces. Current strict checks observe both retained tools signaled at return; negative workflows preserve accepted files, and readback resolves DXIL/SPIR-V. Shared/static regression and Debug CRT checks pass. The static Launcher diagnostic case uses a controlled plan/result with the real executor; it qualifies error policy, not planner readiness or GUI behavior.

**Retained design:** associate a private completion port with the existing job before creation. Terminate, wait for the root and a received empty-job completion with zero aggregate count under one five-second wait budget. Cache the native outcome for repeated teardown. Missing/error/timeout completion becomes a useful failure; there is no accounting/root-only fallback. Public Asio cancellation slots settle pending coroutine operations on observer failure. Readiness releases the port association before accepted detach. The stop callback remains `SetEvent`-only; native settlement attempts no C++ allocation. No hard finite OS cleanup guarantee is claimed. Completed native/failure checks, [source/lifetime analysis](../../build/validation/boost-process/continuation-20261122-r2/termination-01/REPORT.md#design-and-references) and rejected/inconclusive injectors remain in the report.

**Next acceptance:** finish the latest root `SparkleLauncher`/`AssetCooker` rebuild in `build/variants/boost-r2-delivery` after its concurrent ShaderMake recipe is corrected. Last failure: undefined `sparkle_configure_host_tool_target`; earlier missing-cache and native-regeneration failures are preserved. Do not patch/bypass that owner's work or disable a feature to fabricate acceptance. Earlier root/Core/tool builds precede the final caller edits; standalone compiles those edits and focused result cases pass. Refresh final products and run only invalidated caller cases: cancellation-diagnostic preservation, nested success/failure and strict cancellation. Matching Core/shared/static/Debug evidence remains reusable.

```text
Continue the remaining Stage 4/5 gates in Docs/Implementation/BoostProcess.md. Read termination-01/REPORT.md, result.json and costs.json under build/validation/boost-process/continuation-20261122-r2; preserve the original consumers-01 failure and rejected-injector records. Native completion observation and caller error propagation are implemented. Production Core strict cancellation, shared/Debug/static regression, shader workflows and focused caller policies pass on bound products. No LOC saving is claimed: connected C++ grows 76 lines; C++/CMake grows 144; historical credit stays 259.
Inspect the concurrent ShaderMake/root configuration first. Last root caller rebuild failed on undefined sparkle_configure_host_tool_target after its missing cache appeared. Do not alter or bypass that owner's work to force acceptance. Once corrected, rebuild root SparkleLauncher/AssetCooker in the existing boost-r2-delivery variant, bind new identities and execute only invalidated caller/strict-cancellation cases. Preserve profile/variant identity and failures; stop at a decisive new defect. Cleanup is separately rejected by automatic approval review: do not bypass it using another deletion command. Update exact stages, missing cases and next action before handoff. Close/delete the plan only after final root caller qualification and approved exact-trial removal. Do not infer GUI/GPU/package/release acceptance or reopen rejected R0/R1 candidates.
```

## Stage 5 - Retirement and code closure

- [ ] Complete stage 4's final root caller rebuild and invalidated caller checks; reuse matching passed Core/strict-cancellation evidence and preserve earlier failures and accepted outputs.
- [ ] Resolve the earlier [automatic cleanup rejection](../../build/validation/boost-process/continuation-20261122-r2/cleanup.json) for **only** `build/private/boost-process-r2` through an approved route and verify absence. Do not bypass that rejection with another deletion command; preserve reports, caches, products and concurrent work.
- [ ] Record final source/product identities, transfer broader unverified qualification to its existing owners, remove incoming links and delete this plan under Engineering's documentation rule.
