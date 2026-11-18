# JSON integration: remaining acceptance

[Migration strategy](../ThirdPartyMigration.md#json) | [Developer guide](../Engineering.md) | [Code style](../CodeStyle.md)

Temporary implementation plan; delete it at closure.

**Status, 2026-11-18:** stage 6 is **COMPLETE: all four broader format families are explicitly retained**. The lasting decisions and owners are in the [migration guide](../ThirdPartyMigration.md#retained-text-formats). No additional JSON format, dual reader, upgrade engine or settings DOM was introduced. The unused Launcher settings-file declaration is removed. Texture request serialization now rejects unrepresentable path characters before publication; the existing protocol and concurrent HDR extension are preserved.

Stages 2–5 implementation is delivered: upstream C++ JSON owns parsing/values/serialization, the six Core reader/writer/tokenizer files and JSON escaping are gone, freshness reads/parses once, and transferred fragment/access paths are retired. Their completed instructions and historical checkpoint narratives have been removed from this active plan. Earlier missing or failed acceptance checks are consolidated below; removing completed instructions does not mark those gates passed.

**Only stage 7 acceptance remains. The migration is not yet accepted.** Cache-cold remains **WAIVED / UNVERIFIED** by the user's explicit instruction; it is not a measured PASS. Full rendering quality, estimator correctness, GPU performance and vendor crash-provider qualification remain separate from the mandatory migration checks.

Current stage 6 evidence is in ignored `build/validation/json/stage6-20261118/report.md`: frozen dirty source inputs, the affected texture-only baseline refresh, before/after binary and fixture bytes, owner results, native package-subset checks, commands and limitations. The original request baseline had nine fields; concurrent HDR work extended it to ten while retaining `TextureCookRequests|1`. Stage 6 tests that actual ten-field representation without reverting HDR work or crediting it as JSON savings. The 20 authored levels and 20-level/18-pack catalog retain their formats. Real parser/writer, cook-discovery and selection checks pass, as do eight path/publication regressions. Full GUI, category cooking and complete prepared-package qualification are still required below.

Sealed earlier evidence remains separate under `build/validation/json/`: `stage1`, `remaining-preparation-20261117`, `deduplication-20261117`, `stage4-20261117` and `stage5-20261118`. Stage 5 records 216 owner comparisons, three regression checks, static/shared boundaries and a stage-local reduction of 520 lines/14,384 UTF-8 bytes. These scoped observations are not final whole-migration cost or maintenance acceptance. The removed plan text is preserved in the stage 6 source archive and `plan-before.md`.

## Delivery constraints

- Freeze source, relevant dirty/untracked inputs, toolchain, fixture bytes and executable/package identities before comparison. Isolate concurrent builds/CLI changes; refresh only an affected owner's baseline after its inputs settle. Preserve prior products and receipts separately.
- Reuse evidence only while its required inputs still match. Regenerate actual PE/PDB/source receipts and retained copies for the final products. A source scan, parser probe or ordinary owner build cannot qualify an unrun workflow or a different configuration.
- Keep JSON grammar, Unicode decoding/escaping, values, lookup and serialization upstream. Keep bounded consumed-key/depth/byte admission and schema/type/range/path/hash meaning at existing owners. Native PowerShell/CMake and importer/library formats retain their own facilities.
- Parse each input snapshot once, consume upstream subvalues or existing domain records, compose typed output and serialize once. Keep JSON types private. Preserve raw receipt bytes only where exact identity needs them. No per-property reparse, synthetic document, dump/reparse, mirrored writable representation or operation-spanning polling DOM cache.
- Reuse an existing counterpart before adding a helper. Each function has one cohesive responsibility and abstraction level; preserve readable paragraphs and cohesive callback state. No generic JSON service, schema DSL/registry, old-interface facade, parser fallback, dual write, compatibility switch, copied upstream internals or new ownership database.
- Preserve exact uint64/hash wire types, required-field duplicate rejection, optional per-field fallback, complete-document admission and existing publication/settlement lifetimes. Unknown-member tolerance is distinct from consumed-field ambiguity. Treat accidental malformed-input acceptance as a defect, not a compatibility requirement.
- Keep machine JSON strict. Do not enable comment acceptance globally or add comment stripping/token patching. Retained non-JSON formats are outside the Core JSON replacement; their contracts do not create a second JSON owner.
- Compare independent expected integers, native hash strings and hashes of actual candidate bytes. Permit only individually named timestamps/cook IDs, output/workspace roots and regenerated source/compiler/symbol identities. Never blanket-ignore fields by names containing hash/id/path/time. Execute native-script numeric cases rather than inferring their behavior.
- Preserve unrelated work. Coordinate diagnostic payload/settlement ownership with [TaskRuntime](TaskRuntime.md); do not count its deletions while hiding replacement state. No unrelated renderer redesign is part of JSON acceptance.
- Keep commands, actual exits, source/product/fixture identities, diagnostics, costs and limitations with ignored evidence. Remove temporary probes from the repository, retain inert reproduction recipes, reuse existing checks and preserve existing tests. Add no permanent test framework or progress registry.

## Architecture and removal gate

The final call path must read as bounded file access → one upstream parse → existing domain use, or typed values → upstream composition → one serialization/publication. Review final files, headers/PCHs, callers, enabled build membership and package imports. A zero-match old-symbol search cannot detect renamed or relocated infrastructure.

The sole final maintenance threshold remains fixed before implementation:

| Owned common-infrastructure measure | Archived removal floor | Required net saving across the connected change |
| --- | --- | --- |
| Physical lines | 629 | At least 40%, rounded up to 252 lines |
| UTF-8 bytes | 15,678 | At least 40%, rounded up to 6,272 bytes |

Charge every added/expanded first-party JSON policy, caller, adapter and build/package hook, including templates/inline code, storage, exception translation and forwarding. Count moved code once; exclude upstream and unchanged domain algorithms. Do not credit unrelated CLI/task/texture changes, compress code or delete necessary validation to reach the floor. Use the original per-file/function scope and arithmetic. Require fewer representations/authorities and simpler actual callers as well as the numeric floor.

Reconcile all six retired Core files, JSON escaping/control helpers, 16 retired callable entries and eleven named fragment/access-pattern groups against their upstream operation or necessary retained domain invariant. Account separately for code, public surface, representations, forwarding, mutable state and failure paths. Each retained helper/state field needs a current consumer and an invariant upstream does not supply. Stage-local removal is not this final accounting.

The adoption decision remains conditional: nlohmann/json covers general reading and writing; the already-considered simdjson alternative does not provide the same complete writer fit. Do not construct a competing integration. Stop or simplify if required preservation/cost/reduction gates fail or substantial first-party machinery is needed; do not change the denominator or budgets after measuring.

## Fixed comparison budgets and configurations

The unchanged source of policy is `build/validation/json/stage1/policy.md`. Compare matching fixture bytes, toolchain, worker count **4**, machine, settings and output membership. Required missing/unstable measurements remain OPEN/BLOCKED for their claims.

| Metric | Acceptance budget |
| --- | --- |
| Tiny completion ≤1 KiB, parse plus consumed fields | Warm median ≤ baseline ×1.25 +0.005 ms; p95 ≤ baseline ×1.25 +0.010 ms |
| Typical export/request 1–256 KiB, complete parse/load | Warm median/p95 ≤ baseline ×1.15 +0.050 ms |
| Valid receipt 32 MiB−1 KiB, ≤65,536 files | Warm median/p95 ≤ baseline ×1.10 +1 ms; cache-cold ≤ baseline ×1.15 +5 ms |
| Rejected wide/deep input | No accepted malformed input; rejection ≤ baseline ×1.10 +1 ms and ≤2 s absolute; include 65,537 valid-prefix records and depths 63/64/65/1,024 |
| Peak process private commit | ≤ baseline +max(16 MiB, 4×document bytes); ≤256 MiB additional at 32 MiB |
| Cold owner build, populated cache/empty intermediates | ≤ baseline ×1.15 +5 s per owner/configuration |
| Incremental / no-work build | ≤ baseline ×1.15 +1 s / ≤ baseline ×1.10 +0.5 s |
| Executable plus owned runtime binaries per product | ≤ baseline +2 MiB **and** ≤ baseline ×1.10 +256 KiB |
| Identical-membership package growth | Uncompressed ≤ baseline +4 MiB; compressed ≤ baseline +2 MiB; no runtime JSON DLL |

Parse/load protocol: monotonic wall clock, five warmups and thirty independent process observations; keep all samples, median/p95 and separately measured startup. Tiny timing uses 1,000 operations/process. File-cache warm requires a complete pre-read. Genuine cache-cold needs OS restart/recorded eviction and five repetitions; process-cold is not cache-cold. Its unavailable-privilege waiver changes only that gate's disposition. Record peak private-commit sampling interval; working set is secondary.

Build protocol: three populated-cache cold builds, five single-caller incremental and five no-work builds; separate configure/generation and retain successful exits/output hashes. Use isolated output namespaces. Concurrent unrelated builds invalidate timing; do not delete accepted products to make a cold baseline.

| Required configuration | Owner coverage still to reconcile against final source |
| --- | --- |
| DevelopmentEditor/static | ShaderCompiler, Launcher core/GUI, MeshCooker, Editor/ApplicationEditor, enabled diagnostic RHI owners |
| DevelopmentGame/static | Application/Game |
| ShippingEditor/static | Tools and Editor |
| ShippingGame/static | Game |
| DevelopmentEditor/shared | Real Core/module consumers and affected owners |
| DevelopmentGame/shared | Real Core/module consumer and affected owners |

Include diagnostics ON/OFF with the correct separate artifact contexts and matching regenerated ShaderCompiler/Editor/Game receipts, plus D3D12/Vulkan writer membership. Optional SDK absence must remain supported. Confirm actual enabled target membership (`ShaderCompiler`, `SparkleLauncherCore`/`SparkleLauncher`, `MeshCooker`, `SparkleEditor`, `SparkleApplication`/`SparkleApplicationEditor`, `SparkleRHIDiagnostics`, both RHI backends and `SparkleCore`). Reuse matching completed rows; do not substitute products across configurations or add combinations without a distinct claim.

## 7. Remaining acceptance work

```text
Complete stage 7 of Docs/Implementation/JSON.md under the common delivery rules.
Verify the remaining workflows, final ownership/removal, offline delivery and fixed-baseline costs.
Close only when all required gates below pass; preserve the explicit cache-cold waiver.
```

Every row below remains required. Completed software instructions are intentionally absent.

| Open gate | Required evidence / next action |
| --- | --- |
| Controlled runtime time/memory | Run the final complete owner operations and required rejection fixtures against frozen baselines under the fixed protocol. The earlier near-limit export sample was 187.26 ms against a 117.80 ms limit, with concurrent-build interference; it is not a PASS. Resolve/retest the actual owner cost without internals, double parsing, lower limits or a new framework. |
| Final build/configuration and growth | Complete/reconcile the matrix above, controlled cold/incremental/no-work distributions, per-product PE/runtime growth and candidate-bound receipts. Existing scoped stage 3–5 builds do not qualify later concurrent build/source changes. |
| Whole-migration maintenance | Finish the common-scope ledger and source/caller review against the unchanged 252-line/6,272-byte net floors. Include all integration, retained policy, public APIs, representations, state and failure paths; classify remaining JSON-like source/script matches. |
| ShaderCompiler → Editor/Launcher | Matching DXIL/SPIR-V exports, complete bundle discovery, source/reflection/list/inspect, required failure diagnostics/replay behavior and parameter verification; interactive Recompile Shaders/CompileShaders through Launcher with symbols on/off; preserve failure/no-work completion, hashes, limits and accepted output. Reuse only matching current producer/consumer evidence; do not credit unrelated feature removal as JSON simplification. |
| Cookers → Editor/runtime | Bounded project/category cooks, mesh metadata presentation and Editor level load/edit/Save All/reload using the retained formats. Preserve catalog selection/release restrictions, scene membership, texture/HDR policy and supported Unicode paths. Owner probes and the relocated 22-file recipe subset do not qualify the full workflow. |
| Launcher freshness/diagnostic exchange | Actual configuration/source invalidation, candidate/receipt admission, completion records, child handoff and incident verification/export on supported D3D12/Vulkan paths. Retest required malformed/duplicate/range/count/containment, interrupted/enqueue/write/callback failure and final settlement cases against final products. Synthetic incidents do not qualify vendor-native capture. |
| Reference tracer artifact workflow | Bounded real export with manifest/image/checkpoint association, all statuses, hashes, paths and exact counters. CPU artifact comparisons do not establish the Editor/GPU export journey or estimator quality. |
| Complete source/prebuilt delivery | Exact staged membership and release restrictions, notices/SBOM/provenance, source dependency/scripts/inputs, compressed/uncompressed growth, relocated prepared products offline, then source configure/build and representative cook with the populated cache. Reject changed products instead of refreshing hashes during admission; no new runtime library/download. |
| Dependency lifecycle | Final Launcher full Sync Code and individual nlohmann/json Sync/Clean, missing-cache readiness, interrupted/failed sync, dirty/wrong-pin preservation and warm/offline reuse agree with CMake and the selected pin. Native-script numeric behavior must match actual exchanged fields. |

Repeat failure/no-work/missing/nonzero-tool, cancellation and failed-publication checks through affected workflows. Reject false completion, partial bundles/incidents, stale candidate/source hashes, reappearing explicitly deleted sources, dangling views, unreleased payload budgets, damaged accepted outputs and leftover active work. Inspect actual generated membership, exports/imports, package inputs and retained helpers; no competing implementation/authority may remain.

For a changed pin/configuration, repeat affected decoded-duplicate/callback, depth/NUL, exact-number/type, encoding/order, snapshot/lifetime, exception/publication and build/package checks. Remove release-specific repair/fallback or admission glue once supported upstream APIs supply equivalent proved behavior. Retain required domain policy and existing module boundaries.

The final ignored report must contain the adoption decision, before/after owner paths, commands/actual exits, input/source/product identities, fixed-cost results, maintenance arithmetic, demonstrated naive-substitution failures, pin/upgrade checks and limitations. Answer the hardest remaining preservation/ownership assumption with a fix/retest or a named unresolved gate. Reproduce in a prepared fresh workspace using existing source/package checks; identify self-review and assisted reproduction accurately.

**Complete when:** every mandatory row passes with matching final identities, the explicit waiver remains accurately labeled, and final review finds simpler orchestration with one authority per responsibility. Keep unchanged rendering/provider/release qualification separate. Then fold any remaining durable guidance into existing guides, remove plan links and delete this file under the [documentation rules](../Engineering.md#writing-documentation).
