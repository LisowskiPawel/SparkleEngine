# Code style

[Docs](../README.md) ? [Engineering](README.md)

Executable policy owns exact formatting and language settings: [.clang-format](../../.clang-format), [.clang-tidy](../../.clang-tidy), [formatter rules](../../CMake/CodeStyle) and module `CMakeLists.txt`. Use the repository formatter; do not mix a broad reformat with a semantic change.

## Readable source

- Write intent in names and ordering. Keep orchestration at one abstraction level and substantive mechanisms in their owner.
- Use one blank line between logical paragraphs, none inside a cohesive group, and no stacked blank lines. Keep a declaration with its immediate use/check; separate independent phases and attach comments to their operation.
- Use explicit braces and multiline control-flow bodies. When a call/declaration/list wraps, put each item on its own line according to the formatter. Do not manually align unrelated declarations or assignments.
- Prefer typed direct code, scoped enums, `nullptr`, explicit constructors and domain values. Use `final` for concrete classes not intended for inheritance; `noexcept` must match real failure behavior.
- Explain non-obvious units, ownership/lifetime, ordering, ABI/backend constraints and measured policy in comments. Remove narrative comments, stale history and unsupported performance/thread-safety claims.
- Validate once at the owner that knows the invariant. Use typed rejection for expected invalid input, assertions for programmer misuse and fatal errors only when continuing is unsafe. Log one actionable failure at its owning boundary.

### Naming

Types/functions use `UpperCamelCase`; parameters/locals use `lowerCamelCase`. Boolean names use `b` plus `UpperCamelCase`, retaining an existing scope prefix (`bReady`, `m_bReady`, `s_bReady`). Private members use `m_`. Apply the same meaning across CPU, shader, serialization and UI contracts; external/native field spellings keep their required names.

Use names for responsibility and units. `Handle` identifies, `Token` proves authority/completion, `Lease` borrows exclusively, `Scope` settles lifetime, `Context` is bounded call state. `Build/Compile` produces a plan; `Record` writes commands; `Submit` transfers work; `Execute` performs it; `Commit` mutates authority; `Publish` exposes accepted immutable state.

Distinguish persistent authored GUIDs, derived source IDs, generated runtime entity/render IDs, contiguous indices and content hashes. A GPU-scene slot is an address, not persistent identity. Preserve physical light quantities, radians, current/previous matrices, joint/skinning and morph terms explicitly. Avoid vague `Manager`, `Helper`, `Data`, `Async`, `ThreadSafe`, `New` and numbered replacement names when a precise responsibility exists.

### C++ and shaders

Runtime CVar consumers query the registered variable at the narrow decision point. Do not pass CVars or values copied solely from them through application-defined parameters, settings/context records or forwarding helpers. Immediate standard-library calls and native descriptor construction may consume the resolved value; owner-held state needed for change detection is distinct from forwarding.

Shader entry points admit inputs, call semantic mechanisms and publish outputs. Keep feature code under its pass owner and reusable BRDF/geometry/material/light/ray operations in their shared domain. Match CPU/GPU layouts, units, matrix conventions, bindings and stage visibility explicitly. Float precision changes, fast math and altered evaluation order require numerical/ABI evidence, not just formatting.

Use focused headers and required API export macros for public out-of-line functions in shared builds. Keep implementation declarations private, include what is used, and preserve authored include/`using` order unless its owner changes.

## Check changed source

After the responsibility/readability pass, cover every added/modified owned source file, including headers and shaders:

```powershell
CMake\CodeStyle.bat -Mode Check -Path "Engine/<module>/<changed-file>"
git diff --check
```

Run the path check for each changed file. `-Staged` is appropriate only when its selection covers the complete stage; it checks working-tree contents, so inspect the actual index diff separately when partially staged. `-ManifestPath` can retain selection evidence. Do not stage unrelated changes to manufacture coverage.

Record mechanical coverage and authored layout review in the existing handoff. Repeat affected checks after subsequent source edits. A formatter cannot prove responsibility, logical grouping or correctness. Documentation-only changes need documentation checks, not a source-format run.
