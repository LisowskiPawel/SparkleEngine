# ShaderMake: remaining integration delivery

[Delivery rules](../ThirdPartyMigration.md#code-delivery-and-qualification) | [Compiler source](../../Tools/Shaders/ShaderCompiler) | [Fit inputs](../../build/validation/shadermake/stage-1/20261122-routing/REPORT.md)

**Next: finish stage 1 technical fit.** Reuse matching [DXC](../../build/validation/shadermake/stage-1/20261122), [Slang](../../build/validation/shadermake/stage-1/20261122-slang/continuation-report.md), [dependency](../../build/validation/shadermake/stage-2/20261122-dependency/dependency-results.json) and [Visual Studio](../../build/validation/shadermake/stage-2/20261122-dependency/vs-results.json) inputs. Recipe/staging preparation is not an action to repeat. Keep direct production compilation until fit passes.

## Stage 1: isolated fit and baseline

- [ ] Assemble/admit the complete production SDK: exact `dxc.exe`/`slangc.exe`, DLLs/modules, source/patch identities and terms through existing SDK/Launcher/package owners. Use the corrected Slang identities; preserve installed SDK bytes and old receipts. Ordinary fit, GPU-crash and distribution qualification are distinct.
- [ ] Qualify matching corrected-Core/compiler products for before-return descendant settlement, concurrent spawn/stop, failure and retry. Reuse [process-owner evidence](../../build/validation/boost-process/continuation-20261122-r2/termination-01/result.json); repeat affected ShaderMake cases without shader-local supervision or eventual-exit acceptance.
- [ ] Finish reordered producer association, remaining metadata shapes, emission-range bounds and canonical effective-option/tool/source identity. Use fixed producer paths/fresh invocation staging; require full JSON declaration metadata, exact output versions, typed ABI and final code/debug/source agreement under serial/bounded execution.

**Exit:** required capabilities, metadata/source/debug/options, bounded settlement, failure preservation and repair/retry pass on eligible matching inputs (S1-A through S1-D behavior). Missing technical fit blocks conversion; inactive policy-denied fixture removal stays at stage 5. Unsafe active work/storage still blocks fit.

## Stage 2: production boundary and retirement

- [ ] Convert actual batch/planning/CLI consumers to admitted immutable toolchain facts, sealed inputs/config, Core execution, typed ingestion, normalization/verification and existing publication.
- [ ] Retire superseded compiler sessions/interface/factory/executor/outer task dispatcher and redundant transfers with callers, files, includes, exports and generated membership. Retain metadata-only SDK readers and source preprocessing unless its separate fit passes.
- [ ] Verify owner builds, normal/failure/retry and accepted-output protection; reconcile admission/provenance, notices, package/runtime inputs and feature-disabled enclosure. One functioning compile owner, no fallback or duplicate source/options/cache/publisher authority (S2-A through S2-D).

## Stage 3: caller, build and code acceptance

- [ ] Review connected structure, copies/lifetimes, SDK exposure, native/non-native/fork costs and complete removal. Regenerate/build affected owners and a changed shared boundary, apply style/manual readability and run existing `ShaderCompilerCliValidation`.
- [ ] Verify full/selected/changed/repeated/debug cooks, explicit backends/targets, inspection/options/exits, Windows arguments/paths, producer equality/fan-out ABI and F01-F12 at actual owners, including publication failure and interrupted-set refusal/recovery.
- [ ] Record final identities and honest costs/qualification limits; require structural removal, builds and focused correctness (S3-A through S3-D). Submitted source contains no test-only additions; policy-denied inactive trial cleanup stays required at stage 5.

## Stage 4: product qualification and cost

- [ ] When requested, exercise Launcher/Editor compile/symbol/reload/failure/cancel/close/retry and native pipeline/raw-signal/debug-source correlation separately on D3D12/Vulkan; qualify relocated prebuilt/matching-source offline tools and declared costs (S4-A through S4-C, F05/F08-F14).
- [ ] Keep unobserved mandatory product cells open. For code-only delivery, transfer broader unrun cells with exact limits/evidence to existing qualification owners and continue stage 5. This does not pass those product checks or reopen passed code stages.

## Stage 5: handoff and plan closure

- [ ] Resolve policy-denied inactive fixtures using exact `cleanup-targets.json`/`cleanup-result.json` records in the dependency/routing packets; preserve rejection evidence and do not evade it. Keep required cleanup open until resolved.
- [ ] Reconcile final retirement, source/tool/product identities, ledger and qualification owners with existing guides; remove probes/incoming links and delete this plan once required delivery/closeout passes (S5-A through S5-C). A supported rejected design closes with its reason, not a shipped-integration claim.

## Constraints for the remaining work

Use base `4f1ffd56dac27fc3d620ad7efeee42b4dd71fa72` and the [compiler-contract patch](../../CMake/Dependencies/Patches/ShaderMakeCompilerContract.patch); bind exact source/patch hashes. Maintain at most **80 native physical lines in one upstream source file**, preserve absent-control defaults and keep the acquired cache clean. Preserve `--no16bitTypes`, `--slangProfile`, `--noDefaultSpirvExtensions`, `--noImplicitDefines`, `--relativePaths` and signature participation. No scheduling/compiler-internals/reflection/publication patch or wrapper executable.

Preserve strictness/HLSL 2021, row-major Slang, bare define value 1, warning/optimization/debug/strip policy, DX layout/storage-image/extensions, exports and capabilities/rejections. Slang uses one exact profile/module `SparkleShader` and explicit entry/stage. Map `spirv_1_4/1_5/1_6` to Vulkan `1.1spirv1.4/1.2/1.3` and verify output headers. DXC library `-E` needs no patch; Slang's optional unused cache key needs no reconstruction. Promote material type spelling and explicit capability/emission repairs in their owners before hashing.

Keep logical `/Engine`, `/Project`, `/Plugin/<name>` identities; derive root-relative compiler `#line` names and hashes once from admitted snapshots. Materialize exact captured originals; reject collisions/inconsistent snapshots. Launch natively through Core with controlled ASCII staged/config/sidecar paths and a same-volume relative compiler path. Prove custom string/macro encoding; `_popen` percent expansion is not a shell serializer. Preserve `__FILE__`/line and Windows spaces/Unicode/metacharacter behavior; no input mutation after hashing or resolver/repro/archive framework.

One authored catalog supplies config and immutable source/options/tool identity. Hash grouping still requires input equality and per-consumer ABI verification after fan-out. Run compatible groups sequentially at allowance 1/default 4, capped at `min(hardware, 8)`; no nested dispatcher. Staging/borrowed inputs outlive descendants and ingestion; typed results own final bytes. Keep Core supervision, publisher/read guards and Renderer/RHI reload/GPU retirement.

Consume full Slang JSON and its associated-artifact hierarchy; validate full-PDB status/runtime hash/native name and retain that `.pdb` name (`.pdb.unknown` is a transport leaf). Preserve the [debug-format correction](../../CMake/Dependencies/Patches/SlangDebugInformationFormat.patch) and loop patch. Retire the flat reader; no recovery compilation. Normalize SPIR-V before final reflection/hash and publish those bytes with matching symbols/captured sources.

## Frozen comparison scope and limits

The mixed inventory is **14 files / 1,575 physical / 1,354 code lines**; the comparison envelope is **244 native files / 19,433 physical / 16,876 code lines plus 45 non-native files**, bound in the [source/retirement inventory](../../build/validation/shadermake/stage-0/20261122-closeout). Charge changed callers outside it. Count maintained C++ physical/code lines using frozen cloc 2.06 `--skip-uniqueness --by-file --json` source lists; report scripts/CMake/shaders/generated/upstream/tool/runtime/package costs separately, including owned patches and IR/container costs. No credit for moves, prior pool removal, preprocessing-C overlap or upstream LOC; require simpler ownership and honest net accounting.

Use ComputeClear, GBuffer VS/PS and inline/RGS Reference types on both default targets (10 planned jobs), explicit Slang GBufferPS on both (2 more), full catalog, selected/changed/repeated/debug and capability rejections with fixed source/options/compiler bytes. Scope counts are not execution proof.

| Comparison | Frozen review limit / method |
| --- | --- |
| Full shader cook wall time | Regression ≤10% **and** ≤2 s, with full catalog and debug/non-debug reported separately. |
| Selected shader cook wall time | Regression ≤20% **and** ≤250 ms, including process startup; selected/repeated/changed requests reported separately. |
| Owning-tool clean / no-change incremental build | Regression ≤10% and ≤10 s clean; ≤10% and ≤1 s incremental. Same configuration, dependency-cache state and compiler toolchain. |
| Cooker startup (`--help`) | Regression ≤10% and ≤50 ms; no claim about GUI startup or frame time. |
| Peak compilation memory | Aggregate private committed memory of cooker + ShaderMake + compiler descendants: regression ≤10% and ≤128 MiB. State sampling interval/limits; do not compare only the parent process. |
| Delivered development tool/runtime payload | Growth ≤5% and ≤20 MiB over the frozen complete shader-tool bundle. Count executables, DLLs, modules and notices; render-only payload gains **0 ShaderMake files/dependencies**. |
| Cancellation and correctness | Zero descendants after reported settlement; existing Editor/Launcher 10 s cancellation/quit requirement retained. Zero unexplained ABI/metadata/debug/source-identity differences or capability cuts. These are correctness requirements, not negotiable benefit targets. |

For requested cost claims: one warm-up then five alternating paired warm runs per cell; three independently reset cold full-cook/build states. Record reset/cache/background limits, each run/median/range; a range spanning a limit is INCONCLUSIVE. Review misses once without weakening correctness or inventing savings. Broader unrun qualification transfers only under the delivery rules.

## Failure checks for the remaining stages

| ID / failure mode | Required behavior and discriminating check | Earliest owner/stage |
| --- | --- | --- |
| **F01: wrong/unqualified/missing tool** | Reject missing compiler/runtime/modules or changed identity; identify the exact path/version/hash. Check both normal and GPU-crash toolchains. No automatic compiler download, receipt alteration or parent-DLL identity substituted for the executing tool. | Toolchain design/fit, 1; admission, 2-3. |
| **F02: effective flags or library contract changes** | Compare actual options/implicit macros, source/module identity, SM/target, optimization/debug/warnings, matrix/16-bit layout, storage-image/extensions, exports and Slang entry behavior. Record ignored baseline request fields; preserve required capabilities/rejections and explain changed code/ABI/hash behavior. | Design/fit, 1; full requests, 3. |
| **F03: hidden or mutable source dependency** | Test conditional/macro/repeated/cyclic includes, generated parameters, virtual roots/case/traversal and files changed during capture; content edits with restored timestamps invalidate identity. Compilers read admitted captured inputs; scanner timestamps are not the content authority. | Input design/fit, 1; real jobs, 3. |
| **F04: missing/corrupt/wrong reflection** | Reject incomplete sidecars, resource/member/type/offset/size/count, vertex/group/push-constant or library-export mismatches. Normalize SPIR-V before final reflection/hash and verify every logical consumer after dedup. No legacy second compilation to obtain metadata. | Metadata fit, 1; verification, 2-3. |
| **F05: symbols/source identify different code** | Require matching published code/hash, full requested DXIL PDB/name, SPIR-V line/source data and original captured bytes. Optional API cache keys may be absent; available keys must retain their true meaning. Detect missing/ambiguous symbols and subprocess provenance mismatch; exercise actual native tool lookup during product qualification. | Debug fit, 1; export, 3; tools, 4. |
| **F06: stale/colliding/misassociated outputs** | Repeated basenames, multiple entries/targets, reordered completion, equal-hash unequal-input producers and duplicate/missing/empty/stale sidecars cannot substitute for the expected producer. Validate named raw/container association, bounded artifact hierarchy, contained paths and exactly one required code/PDB; exit zero alone is insufficient. Verify typed fan-out before publication. | Batch fit, 1; actual owner, 2-3. |
| **F07: duplicate scheduling or unbounded work** | One compile dispatcher respects the invocation allowance across groups; serial and bounded-parallel results agree. No old outer task graph multiplied by ShaderMake workers, local fallback dispatcher or unbounded staging/workspace growth. | Lifetime design/fit, 1; dispatch, 2-3. |
| **F08: failure/cancellation leaves active children** | Syntax failure, child crash/nonzero exit, cancellation/close after partial compile and repaired retry settle descendants before inputs/results/staging retire. No accepted publication or runtime reload on failed/cancelled compile; retained storage outlives missed settlement. | Process fit, 1; owner checks, 3; host workflow, 4. |
| **F09: publication fails or is interrupted** | Returned write/verification/publication failure preserves accepted hashes through the existing supported rollback. A killed publication may leave an incomplete set: readers refuse it and documented cleanup/recook applies. Do not claim restart-safe atomic recovery or introduce a second publisher. | Publication, 2-3; recovery workflow, 4. |
| **F10: architectural retirement is incomplete** | Detect live factory/pool/interface/executor/dispatch remnants, forwarding facades, dual catalogs/options/caches/status or SDK exposure into runtime contracts. Compare final owners/callers/build membership to named retirements; reject comparable replacement machinery even if LOC drops. | Conversion/review, 2-3. |
| **F11: tool builds but offline delivery fails** | Verify feature-disabled enclosure, executables/DLLs/modules, source-cache metadata, notices and inventories. Test relocated prebuilt/matching-source tool use without global discovery/network; render-only products do not require ShaderMake. | Dependency inputs, 2-3; relocation, 4. |
| **F12: CLI/UI behavior or accepted output regresses** | Existing CLI validation plus full/selected/changed/repeated/debug requests, explicit targets/backends, option/exit behavior, Windows spaces/non-ASCII/long commands and progress must work. Failure keeps accepted products; Application reload occurs only on successful cook. | CLI/tool, 3; interactive host, 4. |
| **F13: compile success masks GPU/image regression** | Create actual pipelines and compare matched raw signals/numerics against independent controls on each API. Preserve shader/material/Reference conventions. A shared compiler or tone-mapped agreement cannot establish reference correctness or higher FPS. | Product qualification, 4. |
| **F14: stale evidence or misleading benefit** | Bind results to actual relevant source/tool/flags/products; rerun only affected checks. Report first-party native, shader, script/build, generated/upstream and package/runtime costs separately; charge C overlap once. Predeclared cost misses/inconclusive runs remain visible and do not become invented savings. | Baseline, 1; ledger, 3; costs/handoff, 4-5. |

```text
Read Docs/Implementation/ShaderMake.md and apply Docs/ThirdPartyMigration.md#code-delivery-and-qualification.
Complete the requested remaining stage and prerequisites; for full code delivery continue through fit, conversion, code acceptance and closure. Reuse matching evidence, fix relevant defects and preserve the listed contracts. Remove each finished stage/action at its verified exit. Record real external blockers precisely and retain broader unrun qualification with its existing owner.
```
