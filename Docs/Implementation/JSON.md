# JSON integration

[Migration strategy](../ThirdPartyMigration.md#json) | [Developer guide](../Engineering.md) | [Code style](../CodeStyle.md)

Temporary implementation plan; delete it at closure.

**Status:** planning only, reviewed 2026-11-08. All seven stages remain open; next is stage 1. The source inventory used `63f7aec15d0207658a9dd7b9ce061bb25b5f2153` with concurrent CLI edits. No JSON baseline, integration or runtime qualification is complete; reconcile the live files before implementation.

Replace local JSON infrastructure with nlohmann/json and simplify its connected callers. Measure the entire maintained implementation, including integration code.

| Responsibility | Sole owner |
| --- | --- |
| JSON grammar, decoding/escaping, value storage, lookup and serialization | Upstream library; remove equivalent local mechanics. |
| Admission and domain meaning | Existing capability/schema owner; retain required duplicate/depth/size, type/range, hash, path and candidate rules. |
| File access, sequencing, cancellation and publication lifetime | Existing file/task/feature owner; preserve its invariants. |

Parse each document once per operation and consume upstream subvalues or existing domain records. Compose upstream objects/arrays and serialize at the output boundary. Keep JSON types inside implementation contracts. Maintain one authoritative representation per lifetime; retain raw receipt bytes separately only where exact identity requires them.

Each converted C++ path uses the upstream library exclusively. Update connected contracts/callers together, delete replaced helpers/state and stop at boundaries already consuming valid domain values. During stages 3–5, the old parser serves only callers still awaiting conversion against the same wire contract. No runtime parser selector, fallback, dual writes or synchronization between representations is permitted. Stage 5 deletes the old implementation; coexistence cannot pass final delivery.

Keep common admission mechanics in one bounded callback where consumers share the invariant, and each schema rule with its existing owner. Reuse those rules at necessary trust boundaries; remove duplicate implementations and forwarding layers. Do not introduce a JSON service/facade, schema registry/DSL, generic codec framework, mirrored model or parallel dependency inventory. Every retained helper must own required behavior the library does not provide.

Capture stage 1; deliver stages 2 and 3 together, then continue in order. Stage 6 resolves optional format consolidation. The [adoption requirements](../ThirdPartyMigration.md#requirements-for-adoption), [dependency workflow](../ThirdPartyMigration.md#ownership-and-dependency-workflow) and these rules apply to every prompt:

- Leave owning tools usable and test new output with any unconverted consumers. An intentional wire change includes all affected readers, writers, scripts and package inputs in that delivery.
- Update this file before handoff with PASS/BLOCKED, source/executable/input identities, commands/artifacts, removed/retained mechanisms, remaining gaps and the next action. Keep raw evidence in ignored `build/validation/json/`. Required missing checks block dependent conversion.
- Preserve unrelated edits and reuse evidence while relevant inputs match. Format/check changed source and review ownership, lifetime, readable orchestration and actual caller simplification before recording PASS.
- Reuse existing checks; remove temporary probes before submission and preserve existing tests. Add no validation framework or progress registry.

Use the [engineering goals](../Goals.md#north-star) and [principal-role expectations](../References.md#employer-role-study) to assess delivered code, measured tradeoffs and reproducible handoff. Builds/probes establish only their tested scope.

## 1. Capture behavior and the removal boundary

```text
Capture stage 1 of Docs/Implementation/JSON.md without production edits.
Apply the common delivery rules. Establish the baseline, ownership/removal map, comparison budgets and format decisions below.
```

Freeze reproducible source, binary and fixture identities, including relevant dirty/untracked inputs; preserve baseline products separately. Isolate concurrent CLI changes or refresh only the affected tool's baseline after they settle. Regenerate diagnostic receipts/symbol identity for the actual build.

Run current producers/consumers and retain input/output bytes, decoded values, diagnostics, fallback and exit behavior. For each family, record root/member types, required/optional and unknown fields, missing/null/empty behavior, duplicates, resource limits, error disposition and semantic versus byte equality. Include optional metadata fallback. Source inspection alone is insufficient.

Record a concise adoption decision: maintenance problem, complete read/write fit, costs, the credible alternative already considered in the migration guide, assumptions and stopping conditions. Reuse existing research rather than constructing competing integrations.

Fix numerical budgets for parse/load time, peak memory, cold/incremental builds and executable/package growth before measuring. Include tiny completion records, typical exports, large valid receipts near limits and wide/deep rejected input. Record warm/cold conditions, repeats and required Editor/Game, shipping and shared/static configurations. Separate mandatory migration checks from unchanged rendering/provider qualification. Stages 2–3 must meet these budgets before wider conversion.

Compare exact wire types and independent expected uint64/hash values; check generated syntax with an existing host JSON facility. Permit only named changes such as timestamps, output roots and regenerated identities, verifying new hashes against actual candidate bytes.

Start with this inventory, then search tracked first-party code/scripts/build inputs for additional manual JSON:

| Document family | Producers, consumers and behavior to capture |
| --- | --- |
| Shader debug exports, compile requests, replay and validation output | [ShaderCompiler publication](../../Tools/Shaders/ShaderCompiler/Private/Cooking/Publication), [failure replay](../../Tools/Shaders/ShaderCompiler/Private/Cooking/Compilation/ShaderCompileFailureReplay.cpp), [verification](../../Tools/Shaders/ShaderCompiler/Private/Verification) and [cook command](../../Tools/Shaders/ShaderCompiler/Private/Cli/CookShadersCommand.cpp) write JSON. [Editor export discovery](../../Engine/Editor/Private/Shaders/ShaderExportDiscovery.cpp) reads manifests/requests. Capture job membership/order, source/compiler identities, hashes, reflection records, escaped paths and completion output. |
| GPU diagnostic inputs, receipts and incidents | [Application diagnostics](../../Engine/Application/Private/Diagnostics/GpuCrash), [RHI diagnostics](../../Engine/RHI/Private/Diagnostics/GpuCrash), [D3D12](../../Engine/RHI/Private/D3D12/Diagnostics/GpuCrash), [Vulkan](../../Engine/RHI/Private/Vulkan/Diagnostics/GpuCrash), [Launcher diagnostics](../../Tools/Launcher/SparkleLauncher/Private/GpuCrash), [process output](../../Tools/Launcher/SparkleLauncher/Private/Operations/ProcessRunner.cpp) and [diagnostic scripts](../../CMake/Diagnostics). Capture candidate identity, unsigned counts/PIDs, explicit null source-deletion records, bounded file membership, path containment and native shader hash strings. |
| Build freshness | [Freshness state](../../Tools/Launcher/SparkleLauncher/Private/Build/State/BuildFilesFreshness.cpp) writes a stamp and repeatedly calls the [per-property file reader](../../Tools/Launcher/SparkleLauncher/Private/Build/State/BuildWorkspaceStateFiles.cpp). Capture every stamp field, missing/malformed behavior and configuration/source-change invalidation. |
| Mesh diagnostic metadata | [MeshCooker writer](../../Tools/Cooking/MeshCooker/Private/CookedMeshAssetWriter.cpp) and [Editor catalog](../../Engine/Editor/Private/AssetInspection/MeshDiagnosticMetadataCatalog.cpp). Preserve schema recognition, optional display/source fields and accepted cooked output. |
| Reference Path Tracer artifacts | [Artifact encoding](../../Engine/Application/Private/Editor/ReferencePathTracer/ReferencePathTracerArtifactEncoding.cpp) assembles a manifest from already serialized fragments. Capture artifact names, integer/sample values, hashes, status and optional checkpoint fields; distinguish JSON changes from the binary checkpoint codec. |
| Build/package exchange | [Source identity](../../CMake/SourceIdentity.ps1), [packaging](../../CMake/Packaging) and CMake-generated product declarations exchange JSON using their host tools. Inventory exact field types, encoding, byte-sensitive hashes, notices, SBOM/provenance and staged membership. Keep native PowerShell/CMake and third-party importer facilities where they already own the responsibility; do not embed a C++ parser into scripts or rewrite cgltf/Assimp JSON handling. |

Delete all six files under [Core/Public/Json](../../Engine/Core/Public/Json) and [Core/Private/Json](../../Engine/Core/Private/Json): `JsonReader.h/.cpp`, `JsonWriter.h/.cpp`, `JsonTokenReader.h/.cpp`. Also remove `Strings::EscapeJsonString` and its control-escape helper from [StringUtils.h](../../Engine/Core/Public/Strings/StringUtils.h) and [StringEscaping.cpp](../../Engine/Core/Private/Strings/StringEscaping.cpp). Retain unrelated CSV escaping, neutral hex formatting and domain validation.

Map producer, consumers, lifetime and build target for each affected path, including private text/fragment handoffs. Count owned code, public surface, intermediate representations, forwarding layers, mutable state and failure paths separately from upstream headers. Identify the single remaining owner and deletion for each transferred responsibility; no new ownership database is needed.

Inventory levels, catalogs, cook requests and actual persisted settings separately: syntax, editing/comments, readers/writers/scripts and package membership. Record complete-conversion or retention decisions for stage 6.

**Complete when:** required comparisons, fixed budgets/configurations, ownership/deletion map and format decisions are reproducible. Missing evidence blocks the affected conversion.

**Failure checks:** reject moving baselines, stale products, missing caller pairs and unknown script numeric behavior. Record accidental malformed-input acceptance as a defect; do not build a compatibility parser around it.

## 2. Verify the release, admission policy and dependency

```text
Prepare stage 2 and deliver it with stage 3 of Docs/Implementation/JSON.md.
Verify the selected release APIs and compile focused local probes against that exact pin before relying on callbacks, numeric conversion or serialization.
Use the existing dependency workflow; deliver registration, Launcher readiness/sync, target integration and the first complete producer/consumer conversion together.
```

Start with the official [nlohmann/json 3.12.0 release](https://github.com/nlohmann/json/releases/tag/v3.12.0). Its release documentation/source were inspected for this plan; that is API research, not an executed Sparkle comparison. At integration, verify maintenance, relevant defects/advisories, supported Windows compilers and MIT license, and pin the full verified commit through the existing cache. Use versioned upstream sources rather than assuming current website examples exist in this release. A different selected release needs the same checks again.

Register `nlohmann_json` consistently in [FetchDependencies.cmake](../../CMake/Dependencies/FetchDependencies.cmake), the [standalone Launcher allowlist](../../Tools/Launcher/CMakeLists.txt) and [SourceDependencyState.cpp](../../Tools/Launcher/SparkleLauncher/Private/Dependencies/SourceDependencyState.cpp), using one `nlohmann_json-src` cache, display version and required header/license list. It becomes required for Launcher independently of optional content or GPU SDKs. Use the upstream [`nlohmann_json::nlohmann_json` target](https://github.com/nlohmann/json/blob/v3.12.0/docs/mkdocs/docs/integration/cmake.md); disable unused tests/examples/install work. Prefer private target usage. If a justified shared policy header exposes library types, declare its actual usage requirements and keep those types out of domain APIs. Use consistent library configuration across translation units and shared/static consumers. Add identity/license to existing notices and package/SBOM inputs; introduce no runtime JSON DLL, copied vendor tree or dependency manager.

Before conversion, settle the callback owner, target usage, exception translation and stage 1 duplicate policy. Use public upstream APIs; inspecting `nlohmann::detail` does not authorize depending on it. Keep callback state local to one parse, count object/array nesting and release keys when their object ends. Apply the common ownership rules; a substantial facade blocks adoption.

The release's [callback contract](https://github.com/nlohmann/json/blob/v3.12.0/docs/mkdocs/docs/features/parsing/parser_callbacks.md) makes `false` a filtering decision. It does not mean document rejection. In the [callback DOM implementation](https://github.com/nlohmann/json/blob/v3.12.0/include/nlohmann/detail/input/json_sax.hpp), key callbacks run before object insertion. Duplicate detection must therefore observe decoded keys during parsing, with separate key state for each active object. Throw an explicit admission error on a forbidden duplicate or excess depth and translate it at the owning operation boundary. Do not filter bad input into a successful partial document, inspect an already collapsed DOM for duplicates, or run a validation parse followed by a second DOM parse.

Prove the following against the exact release and baseline, using temporary probes before converting consumers:

| Contract | Required implementation and discriminating cases |
| --- | --- |
| Duplicate keys | Reject repeated required/consumed members before overwrite, including equal values and escaped-equivalent names such as `"id"` and `"\u0069d"`. Same names in separate sibling objects are valid. Record unknown-member policy explicitly: the old reader rejects duplicates of the requested key, not every key. Reject all duplicates for an owned schema only after checking supported producers/inputs; otherwise preserve its narrower policy. Test duplicates in nested objects, objects inside arrays and unused subtrees. |
| Exact unsigned fields | Check `is_number_unsigned()` before extracting `std::uint64_t`; check destination bounds before narrowing. Exercise 0, 2^32 boundaries, 2^53−1/2^53/2^53+1, 2^63, UINT64_MAX and UINT64_MAX+1. Reject negative values including `-0`, fractions, exponent notation, booleans, null and numeric strings for these fields. The release can represent integer overflow as floating point, and [numeric conversion is not range validation](https://github.com/nlohmann/json/blob/v3.12.0/docs/mkdocs/docs/features/types/number_handling.md). Avoid implicit conversion and defaults that select a smaller numeric type. |
| Hashes and identity | Preserve each owner's string spelling, width, case/prefix acceptance and full-consumption hex validation. Keep SHA-256 and hex IDs as strings on the wire; never route them through double or JSON numbers. Preserve explicit null where it means a deleted source, distinct from missing/wrong-typed hash. |
| Field conversion | Read from const values using non-inserting lookup, then check the expected type and domain range. Keep required-field errors distinct from documented optional fallback. Test wrong root/container/element types, missing/null/empty values, integer/string/bool substitutions and empty arrays versus null. Construct explicit output objects/arrays, including empty ones; avoid unchecked conversions, catch-all defaults and reads that insert missing members. |
| Whole-document admission | Parse a bounded complete byte range once; require the expected root type. Reject truncation, trailing non-whitespace/second documents, trailing commas, malformed literals/escapes, invalid UTF-8 and bad surrogate pairs. Preserve accepted leading UTF-8 BOM and whitespace behavior where required. Test malformed content after an otherwise valid required field and inside ignored fields. Use strict [`parse`](https://github.com/nlohmann/json/blob/v3.12.0/docs/mkdocs/docs/api/basic_json/parse.md), not stream extraction that can leave a suffix. |
| Raw NUL | Reject raw zero bytes in the supplied document before parsing. The release [lexer treats NUL as end of input](https://github.com/nlohmann/json/blob/v3.12.0/include/nlohmann/detail/input/lexer.hpp), so a bounded range alone does not prove suffix rejection. Test a valid object followed by NUL and garbage. This byte check is admission policy, not another lexer; valid escaped `\u0000` remains a decoded string character subject to domain rules. |
| Nesting and resource limits | Preserve the existing `SkipValue` depth guard of 64 with baseline fixtures, not an assumed matching callback depth. The old reader starts each root-object member at depth 0; root depth and primitive-value callbacks therefore matter as well as container starts. Exercise empty containers, primitive leaves, mixed object/array chains and boundaries 63/64/65 under the old convention. Enforce depth during parsing, including unused subtrees, before unbounded DOM growth. Preserve owner-specific byte and array/file-count limits, including 32 MiB receipts and 65,536-entry bounds where currently used. Bound the input read before DOM creation and measure callback/key-state plus DOM memory for wide near-limit documents; a post-parse array count is not a bound on parsing allocations. |
| Exact receipt comparison | Preserve raw bytes alongside parsed values where identity requires them. [Product receipt verification](../../Engine/Application/Private/Diagnostics/GpuCrash/GpuCrashInputs.cpp) compares retained/startup receipt text exactly; do not replace that with DOM equality or a reserialized value. Upstream [JSON equality](https://github.com/nlohmann/json/blob/v3.12.0/docs/mkdocs/docs/api/basic_json/operator_eq.md) also converts numeric types for comparison. Test whitespace/key-order changes, byte mismatches and integer/float substitutions independently of ordinary semantic comparisons. |
| Lifetime and failure | The DOM owns parsed values; references/views into it last only while that unmodified DOM lives. Return/move typed owned strings, paths and records when work outlives parsing. Exercise delayed/cancelled work and malformed reads; keep existing read/publication scopes until dependent file access and workers finish. Catch library/admission errors at the existing failure boundary; no abort, silent partial success or mutation of accepted state. |
| Writing and byte identity | Use typed objects/arrays and upstream [`dump`](https://github.com/nlohmann/json/blob/v3.12.0/docs/mkdocs/docs/api/basic_json/dump.md) with strict encoding behavior. Cover control characters, quotes, backslashes, non-ASCII paths, empty arrays/objects, booleans, null, uint64 and non-finite floats. Check finite domain values before writing; do not turn a bad value into an apparently valid null. Compare decoded values where layout is incidental; preserve required order, canonical bytes, newline/encoding and byte-derived hashes where consumers depend on them. Hash and verify the actual published bytes without reserializing them. |

**Complete when:** the exact-pin probes pass; the first converted family builds in the recorded host-library configurations and meets the stage 1 cost budgets; workspace/standalone Launcher use the same cache; full Sync Code and individual Sync/Clean work; warm/offline configure reuses the pin; and notices/SBOM identify the delivered library. Verify single-dependency sync returns before unrelated SDK setup. Land this dependency with stage 3 rather than as an unused registration. Stop further conversion if the first delivery fails preservation or maintenance/cost requirements; repair it or retain the working implementation without leaving an unused dependency.

**Failure checks:** callback filtering masquerading as rejection, duplicate overwrite, narrowing/wrap, float-mediated IDs, raw-NUL suffix acceptance, wrong depth counting, dangling DOM views or inconsistent compile definitions block adoption. In a disposable workspace, also test missing headers/license, wrong/dirty revision, failed/interrupted sync and cache cleanup. Readiness checks and CMake pin/cleanliness checks must each behave correctly and preserve failed caches for inspection. If preservation requires a substantial replacement parser or a fork, stop adoption under the migration guide.

## 3. Convert shader artifacts and their readers

```text
Implement stage 3 with the prepared stage 2 work in Docs/Implementation/JSON.md.
Convert ShaderCompiler JSON production, replay, verification and completion output together with Editor shader-export discovery, source-receipt consumption and Launcher completion-line reading.
Use direct upstream values, preserve identity/publication behavior and run the baseline plus stage failure checks.
```

Replace manual objects, string-array serialization, quoted fragments and reflection JSON assembly in the shader owners inventoried in stage 1. Adjust private writer contracts that currently return pre-serialized fragments so the owning document composes upstream values and serializes once. Parse the compiler source receipt once, preserving deletion/null semantics and candidate membership. Parse each export manifest and compile request once; pass DOM subvalues or typed records to private domain conversion instead of rescanning substrings. In Editor discovery, consume job strings directly and remove the synthetic `{"path":...}` document used to decode an array element. Preserve relative-path containment, target/backend/type/compile identity and newest matching export selection.

Keep artifact admission, source-file hash checks, debug-source capture, failure replay meaning and file-set publication with their current owners. Preserve completion-line framing consumed by Launcher. Output formatting may change only where the recorded contract permits it; update byte-sensitive identities and all consumers together if an owned representation changes.

Convert [ReadProcessOutputPath](../../Tools/Launcher/SparkleLauncher/Private/Operations/ProcessRunner.cpp) in this delivery. `SPARKLE_SHADER_EXPORT ` and `SPARKLE_GPU_CRASH_INCIDENT ` carry a JSON string after a text prefix, not an object. Parse that complete suffix directly with the upstream library, require a string and remove the synthetic object. Preserve line/prefix rules and rejection of repeated matching records; test both producers, CRLF, absent/final newlines, unrelated output, escaped paths and trailing garbage. A library parser accepting scalar roots does not relax object-root requirements for receipts. The incident writer remains usable before its stage 4 conversion.

**Complete when:** representative DXIL and SPIR-V cooks, debug exports, list/inspect, failure replay and parameter verification produce equivalent typed values; Editor finds the correct complete bundle; Launcher consumes both completion record types; malformed/duplicate/overflow cases fail as specified; and replaced scans/fragment writers in this family are gone. Also run the still-unconverted Application diagnostic consumer and native scripts against new shader output. Run existing `ShaderCompilerCliValidation` and accepted-output preservation checks on the actual merged CLI/JSON files. Stages 2 and 3 close together only after their behavior and early cost gates pass.

**Failure checks:** test missing/malformed manifest or request, duplicate job/identity fields, escaped Unicode paths, path traversal, incorrect source hashes, explicit deleted source reappearing, stale candidate identity and interrupted/failed cook. Reject partial bundles and false completion, preserve accepted output and hold read scopes through dependent work. Shader parser/build checks alone do not prove native shader execution or Editor interaction.

## 4. Convert diagnostic receipts and incident exchange

```text
Implement stage 4 of Docs/Implementation/JSON.md after stages 2 and 3 pass.
Convert Application, RHI, backend and Launcher diagnostic JSON as a connected producer/consumer family, including native-script exchange.
Preserve admission bounds, candidate/hash identity, process settlement and publication behavior; run the baseline and failure cases.
```

Convert Application input verification, shader input discovery, incident files/sink, RHI session/native observations and D3D12/Vulkan diagnostic writers. Convert Launcher candidate/launch receipt parsing and verify the already converted completion consumer. Keep parsed receipt/job/file subvalues together through each operation; eliminate repeated property searches, synthetic objects and object/array text slicing. Retain domain rules for candidate/profile/API match, native hash strings, PID age/state, bounded bytes/member counts, containment, missing artifacts and explicit source deletion. Parse each newly read manifest snapshot once; do not cache one DOM across polling observations of a changing incident.

Exercise PowerShell receipt producers and incident verification/export against the new C++ outputs. Continue using native script JSON facilities; update field types, paths and package declarations together for any intentional contract change. Do not serialize and reparse subtrees to share them between validation functions. Do not change crash capture/provider algorithms to complete a parser migration.

Adjust the private [input-verification contracts](../../Engine/Application/Private/Diagnostics/GpuCrash/GpuCrashInputVerification.h) and their callers to consume parsed subvalues or validated domain inputs. Retire text-based property/array adapters rather than moving their scans behind another entry point. Replace the serialized candidate identity held by [GpuCrashInputs](../../Engine/Application/Private/Diagnostics/GpuCrash/GpuCrashInputs.h) and inserted by [incident manifest construction](../../Engine/Application/Private/Diagnostics/GpuCrash/GpuCrashIncidentFiles.cpp) with one authoritative domain/upstream value that the manifest composes directly. Remove the raw-fragment handoff; retain raw receipt bytes only for the separately required exact identity checks. Keep validation and lifetime with their existing owners.

Keep JSON construction/serialization inside the existing exception boundaries of `GpuCrashSession::ShaderIdentity` and both backends' `ObserveLoss`. These functions are `noexcept`; library allocation/encoding failures must mark collection incomplete and still reach required settlement, without terminating the process or reporting Ready/Collected success. Preserve callback borrowing/sealing, the collection deadline, bounded enqueue accounting and copying of queued bytes before local strings/DOMs die. Exercise a bounded failure through these owners and verify drain/shutdown after it; testing only the serializer's thrown exception is insufficient.

Build diagnostic owners with `SPARKLE_WITH_GPU_CRASH_DEBUGGING=ON` and confirm these source files are compiled. Normal builds filter them out, so a passing ordinary RHI/Application build provides no diagnostic-writer evidence. Also verify that feature-off builds retain their existing optional-SDK behavior. Use the actual diagnostic variant and its matching receipts for required native/script exchange checks.

**Complete when:** valid and invalid receipt/incident comparisons agree across Launcher, Application and scripts; raw receipt equality and candidate-bound hashes remain enforced; typed uint64 values and hash strings survive the real exchange; both backend writers serialize representative records; and incident publication/export, callback failure and child-process results settle correctly. Identify any unavailable provider/hardware cases as open evidence, separately from fixture checks, using the required/excluded scope fixed in stage 1.

**Failure checks:** exercise duplicate required fields, excessive member counts/depth/bytes, out-of-range PID/counts, missing/null/wrong-typed fields, mismatched executable/source/export hashes, traversal, incomplete bundles and nonzero/missing/cancelled children. Include invalid UTF-8 passed to a diagnostic writer and enqueue/write failure during collection; reject exception escape, incorrect terminal state, unreleased budget or a dangling payload. Failed admission must not launch the wrong candidate, publish a partial incident, replace accepted output or report success. A synthetic incident does not establish a successful native GPU crash capture.

## 5. Finish existing JSON and delete Core machinery

```text
Implement stage 5 of Docs/Implementation/JSON.md after the preceding conversions pass.
Convert remaining existing JSON, including Launcher freshness, mesh metadata and Reference Path Tracer manifests.
Delete the complete Core JSON reader/tokenizer/writer and JSON escaping APIs, remove stale callers/build membership, and verify every affected owner.
```

Load the build freshness stamp once into the existing owning operation, validate its fields and compare all configuration/source values from that parsed document. Replace `ReadBuildFilesFreshnessStampValue` and its repeated disk reads; do not retain a per-property facade implemented with a fresh DOM. Serialize the matching stamp with the library. Preserve missing/malformed stamp invalidation and the current string-valued CMake booleans unless all producers/consumers change together.

Convert MeshCooker metadata and Editor inspection together, retaining optional metadata fallback and mesh/metadata publication as one accepted generation. Convert the Reference Path Tracer manifest directly from typed values, removing its vector of pre-serialized property strings and manual assembly loop. Include optional checkpoint fields and exact sample counters. Verify `Complete`, `Checkpoint` and `PartialPrefix` manifests against their actual image/checkpoint files; preserve any required sorted keys and byte-derived identity. Keep binary mesh/checkpoint serialization, image encoding and rendering algorithms with their owners. Finish every additional handwritten C++ JSON reader/writer found in stage 1.

Delete the six inventoried Core JSON files, JSON-specific escaping functions and unused includes/exports/build references. Regenerate CMake membership and repair documentation links to deleted files/directories. Search first-party sources for `Json::`, old header paths, `EscapeJsonString`, manual JSON fragments and repeated parse/read patterns. Review matches by responsibility; valid native-script JSON, upstream importers, domain hex conversion and format-independent strings are not deletion targets. Remove the old directories when empty. Do not replace them with another owned reader/writer pair or vendored library copy.

**Complete when:** freshness invalidation, mesh metadata round-trip/Editor presentation and reference artifact manifests pass their comparisons; all remaining JSON owners build; shared/static boundary checks pass where affected; no replaced mechanism/caller remains; and the first-party removal report includes integration and retained admission/domain code. Show that each relevant operation performs one document read/parse, using focused temporary instrumentation where source review cannot establish it; no per-field parse, subtree dump/reparse or cache added to preserve the old access pattern remains. Build success does not substitute for the owning GUI/artifact workflows.

**Failure checks:** test stale/wrong configuration stamps, missing/duplicate fields, escaped toolchain paths, missing metadata, optional fields and absent/present checkpoint output. Detect accidental numeric/string changes, non-finite output, wrong artifact hashes, dangling views, stale DLL exports or generated source lists, hidden old parsing and a dependency added without net maintenance reduction.

## 6. Resolve broader text-format consolidation

```text
Complete stage 6 of Docs/Implementation/JSON.md under the common delivery rules.
Convert only complete families whose maintenance benefit and preservation are demonstrated; otherwise record the concrete retention reason.
```

| Existing family | Complete conversion boundary |
| --- | --- |
| Authored `.level` files | [Level parsing/writing](../../Engine/GameFramework/Private/Level/Parsing), project level files, AssetCooker discovery, Editor load/edit/Save All, scripts and staged level inputs. Preserve camera/sky/lighting/scene semantics, field order where meaningful, normal authoring and failed-save protection. |
| `Levels.catalog` | [ToolSupport catalog reader/validator/selection writer](../../Tools/Support/ToolSupport/Private/Projects), project catalogs, Launcher selection and asset-pack operations, cook discovery and package filtering. Preserve selection, membership, release restrictions, names/paths and required comments. |
| Texture cook request lists | [Request codec/list](../../Tools/Cooking/TextureCooker/Private/Requests), [MaterialCooker request producer](../../Tools/Cooking/MaterialCooker/Private/TextureCookRequestBuilder.cpp), project/category cooking, inspect/cook commands and scripts. The current format has a `TextureCookRequests\|1` header and pipe-separated fields; preserve exact asset IDs, paths and all texture-policy meanings. |
| Settings and external formats | Identify actual persisted owners first. Keep Qt/ImGui/vendor-owned formats with their library. Convert an owned settings file only with every load/save/default/edit/package path; keep runtime settings objects independent of a JSON DOM. |

Keep machine JSON strict. The release's [comment option](https://github.com/nlohmann/json/blob/v3.12.0/docs/mkdocs/docs/features/comments.md) discards `//` and `/* */` comments; it does not preserve them on save or handle existing `#`/`;` syntax. Retain a format when required editing/comments would need comment stripping, token patching, another syntax tree or parser. Any allowed commented input is an owner-specific contract supported by its scripts too.

Deliver one selected family at a time under the [compatibility policy](../Engineering.md#compatibility): update owned files/extensions, readers, writers, scripts and package inputs, regenerate disposable output and remove the replaced lexical path. Keep existing format when preservation or cost cannot be satisfied. Add no upgrade engine, legacy switch or dual reader.

**Complete when:** every family is fully converted with lexical removal or explicitly retained. A retained non-JSON format is outside the Core JSON replacement and introduces no second JSON owner.

**Failure checks:** exercise load–edit–save–reload, selection, comments, Unicode/separator-containing paths, malformed/missing/duplicate fields, failed write/cook and relocated package lookup. Loss of required editing, membership or publication behavior leaves that conversion incomplete.

## 7. Verify the complete workflow and finish

```text
Complete stage 7 of Docs/Implementation/JSON.md under the common delivery rules.
Verify final workflows, ownership/deletion, offline delivery and fixed-baseline costs. Close only when all required gates pass.
```

Use final source/executable/input identities and the stage 1 budgets. Inspect real outputs on these paths, reusing matching evidence:

| Workflow | Required result |
| --- | --- |
| ShaderCompiler → debug artifacts → Editor inspection | Representative DXIL/SPIR-V export, source identity, complete bundle selection, readable reflection and list/inspect. Exercise Editor Recompile Shaders/`CompileShaders` through Launcher with symbols enabled/disabled and preserve failure/no-work completion. |
| Cookers → metadata/requests → Editor and runtime | Bounded project/category cooks, mesh metadata inspection and every converted request/catalog/level family. Check equivalent domain values, edited save/reload and accepted-output protection. |
| Launcher freshness and diagnostics | Correct build invalidation after configuration/source changes; receipt/candidate admission; completion-line processing; child handoff; incident verification/export. Exercise the supported D3D12/Vulkan paths and identify hardware/provider gaps precisely. |
| Reference Path Tracer artifact output | A bounded real export preserves manifest/checkpoint association, status, hashes, paths and exact counters; independently parse the delivered JSON. Parser fixtures alone do not qualify rendering quality or estimator correctness. |
| Source and prebuilt delivery | Verify notices/SBOM/provenance, source dependency membership and required scripts/inputs. Run prepared products offline from a relocated workspace and reproduce source configure/build plus representative cook using the populated cache. No new runtime JSON library/download is required. |
| Dependency lifecycle | Final Launcher full Sync Code, individual nlohmann/json Sync/Clean, missing-cache readiness, interrupted/failed sync, dirty/wrong-pin preservation and warm/offline reuse agree with CMake. |

Regenerate build membership and complete the stage 1 matrix. Current targets include `ShaderCompiler`, `SparkleLauncherCore`/`SparkleLauncher`, `MeshCooker`, `SparkleEditor`, `SparkleApplication`, `SparkleApplicationEditor`, `SparkleRHIDiagnostics`, `SparkleRHI_D3D12`, `SparkleRHI_Vulkan` and `SparkleCore`; confirm enabled membership. Include diagnostic-on/off variants, required shipping/shared/static coverage, a real shared consumer for changed public/export boundaries and the existing RHI architecture check. Avoid combinations without a distinct claim.

Compare final costs against fixed budgets. Report net owned code/public surface, representations, forwarding layers, state and failure paths across integration and callers. For each retained rule/model, name its sole authority and lifetime; remove overlapping validation, competing state, unused compatibility paths, scaffolding and stale build/dependency metadata. A wrapper around the old interface or a parser-only line reduction does not pass.

The final ignored report contains the adoption decision, before/after call paths, commands/expected results, costs and uncertainty, one demonstrated failure a naive substitution misses, pin/upgrade checks and limitations. Verify reproduction in a prepared fresh workspace using existing source/package checks.

**Complete when:** stages 2–5 remove the old machinery, stage 6 decisions are resolved, required workflow/cost checks pass and final review finds one owner per responsibility with simpler callers. Fold lasting guidance into existing guides, remove plan links and delete this file under the [documentation rules](../Engineering.md#writing-documentation). Migration evidence does not qualify rendering or a release.

**Failure checks:** repeat the common admission cases through affected workflows, plus missing/nonzero tools, cancellation, interrupted publication and spaces/Unicode paths. Reject false success, damaged accepted output, stale/dangling values, leftover work, offline fetches, missing package inputs/notices, budget excess and any competing implementation/authority.
