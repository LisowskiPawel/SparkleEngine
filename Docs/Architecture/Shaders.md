# Shader system

[Docs](../README.md) ? [Subjects](README.md)

One registered program travels from virtual source and compiler inputs to cooked code/reflection, then a typed runtime pipeline and native binding. Tools compile; Renderer selects algorithms and data; RHI owns neutral GPU contracts and backend lowering.

## Ownership and identity

| Stage | Owner and contract |
| --- | --- |
| Source | [Engine/Assets/Shaders](../../Engine/Assets/Shaders): entry points plus shared semantic includes. Feature mechanism stays under its pass; includes do not become registration authority. |
| Registration | [Renderer](../../Engine/Renderer): typed program/stage/parameter declarations beside the consuming feature, shared by runtime and cooking descriptor targets. |
| Compilation | [ShaderCompiler](../../Tools/Shaders/ShaderCompiler): validated job inputs, DXC/Slang invocation, DXIL/SPIR-V, diagnostics, reflection and optional debug symbols. |
| Publication | Complete current shader map/library and provenance through the existing [cook/publication](Content.md#publication-and-recovery) path. |
| Runtime | [RHI shaders/pipelines](../../Engine/RHI) and Renderer materialization: validate the cooked contract, create native pipelines/tables, bind typed parameters and retire replaced generations safely. |

Keep source/dependency/options hashes, logical program and stage/export identity, compiler target, reflection/layout, cooked generation and runtime/native product distinct. A directory path, entry filename or generation counter alone cannot identify changed shader bytes. Cache identity follows the actual artifact/inputs; no parallel catalog or compatibility format is needed.

CPU/shader agreement includes field offset/size/alignment, matrix convention, resource kind/count, register/space or set/binding, stage visibility and RT exports/table layout. Backend requirements belong in semantic descriptors and lowering, not duplicated filename predicates.

## Compile and reload

Launcher shader Cook and Editor **Recompile Shaders** use the same existing Launcher/compiler route and ordinary cooked files. Enable **Generate shader symbols** in either frontend or use `--shader-symbols --run cook.shaders`. Editor console accepts `CompileShaders [symbols]`; `ReloadShaders` only loads existing outputs. Build Launcher/cook tools in the Editor's profile for the optional action.

Compile/publication failure preserves the accepted output/generation and gives source-located diagnostics. Successful publication allows reload; old pipelines, binding tables and resources remain alive through every referencing queue's completion. A reload request is not proof that new code executed.

**Dependency-directed Apply Changed is still a target** (`AC-SHS-19`). A full-catalog recompile does not satisfy it. Do not replay completed historical migration phases 0?11 or add compiler services/precache frameworks to prove current behavior.

## Remaining proof

1. Freeze selected registrations, source/dependency/compiler inputs and targets. Check actual DXIL/SPIR-V reflection, stage/export and layout against consumers.
2. Exercise changed dependency invalidation, repeatability, cancellation, malformed/truncated/corrupt/stale products and transactional publication using the existing owner. Regenerate disposable output; no old-format reader.
3. Prove source-to-cooked-to-native lineage on the selected frame. Check missing bindings, wrong types/layouts/targets, reload failure and delayed all-queue retirement.
4. Inspect symbols/debug artifacts against actual code identity; keep private debug bytes and licenses out of public output unless explicitly admitted.
5. Join the applicable feature/runtime/package results for the final candidate (`FCR-SHDR-01`, historical Phase 12), without treating focused compile results as GPU or archive acceptance.

Retain useful original measurements and recipes in ignored evidence. Remove temporary probes and rejected prototypes before handoff; no permanent test runner or public fault API is selected.

For the exact shader set use executable registrations, not a handwritten second catalog. [References](../References.md#shaders-and-native-apis) links compiler/API precedents; [RHI](RHI.md) and [ray tracing](RayTracing.md) own native lifetime and table semantics.
