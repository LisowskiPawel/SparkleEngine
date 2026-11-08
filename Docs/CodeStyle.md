# Code style

[Documentation](README.md) | [Developer guide](Engineering.md)

These conventions apply to project C++ and shaders. [.clang-format](../.clang-format), [.clang-tidy](../.clang-tidy), [CodeStyle.ps1](../CMake/CodeStyle.ps1), [source checks](../CMake/CodeStyle) and module CMake define the mechanical settings.

## Formatting and paragraphs

Use clang-format **22.1.3**, C++20, Allman braces, tabs at width four, a 200-column limit and left-bound pointers/references. Control flow uses braces and multiline bodies. Wrapped lists use one item per line.

Preserve authored include and `using` order. Do not align consecutive declarations/assignments/comments by padding. C++ comments receive indentation only; shader comments are not reflowed.

Use one blank line between logical paragraphs, none within a cohesive group and no stacked blank lines.

| Context | Spacing |
| --- | --- |
| Function/type definitions | One blank line between definition blocks |
| Wrapped call, assignment, return or aggregate | A separate paragraph; keep continuation lines together |
| Workflow phases | Separate a change of purpose; keep preparation with its immediate use |
| Guards | Group related entry checks; separate normal work and independent recovery/terminal work |
| Declarations/record fields | Group one operation's inputs or one record's fields; declare near use |
| Header APIs | Separate types, lifecycle, API groups and data; no blank immediately after an access label |
| Includes/comments | Keep authored include groups; attach explanations to their operation |
| Blocks/lists | No interior padding or arbitrary gaps inside one expression |

Keep `else`, `catch` and trailing `do/while` attached. Enclosing braces supply a wrapped statement's outer boundary. A wrapped record field still follows the wrapped-statement rule. Use the same spacing in shaders; review logical grouping after formatting.

## Expressions and decisions

Keep readable statements and signatures on one line when they fit. For a wrapped call initializer, keep the declaration, `=` and call head through `(` together, then put each argument on its own line:

```cpp
const auto captureRequest = BuildCaptureRequest(
	provider,
	viewportIdentity,
	presentationGeneration,
	outputDirectory);
```

Shorten or name an expression if its head alone exceeds 200 columns. Prepare meaningful inputs/results before consuming calls. Simple member access, inexpensive `data()/size()`, casts and `std::move` may remain inline when clear.

Use named predicates, handles or scalar comparisons in `if` conditions. **A condition containing a call needs named local results if it spans lines or exceeds 120 characters after whitespace normalization.** This includes accessors, constructors and macro-shaped calls; the general line limit remains 200.

**A boolean initializer/assignment has at most two atomic checks**, including nested groups. Name intermediate facts with small `const bool` expressions and remove redundant gates. A predicate call counts as one check; a variadic fold is one typed producer.

Preserve evaluation order, pointer/bounds guards, short-circuiting and temporary lifetime. Use branch-local work or named results joined by `&&/||` when later work must be skipped. Do not eagerly evaluate both sides, calculate a decision through flag-setting `if/else`, or add a wrapper merely to satisfy the rule.

Separate substantial I/O, decoding, allocation, setup, submission and UI work into named steps. Keep simple access/casts/ternaries readable rather than fragmenting them. Apply these rules in changed code without unrelated whitespace churn.

## C++ structure

Use `nullptr`, scoped enums, explicit constructors, domain values and designated initialization where clear. Use `final` for concrete classes and `noexcept` only for a real no-throw contract. Prefer direct typed code over unnecessary macros, reflection, type erasure or metaprogramming. Use a domain enum/value instead of boolean mode combinations. Classes/structs have at most one direct base.

Prefer direct value bindings and named functions when they make callbacks easier to read. Use a lambda for a short local operation when it is clearer; give substantial validation, conversion and control flow a name.

### Enums

Declare an explicit underlying type. Use `std::uint8_t` for bounded internal sets that fit. Wider storage needs a current flags, cooked, shader, native or serialized requirement; review callers/layouts and regenerate changed output. Do not rely on default `int` or hypothetical future values.

### Headers and namespaces

Headers contain declarations, templates and trivial direct accessors. Put constructors/destructors, algorithms, transforms, validation, orchestration, factories, non-template operators, nontrivial `constexpr` and policy-bearing convenience functions in the matching `.cpp`. Deleted special-member declarations stay. Do not define classes, structs or enum-like implementation records inside functions.

Anonymous namespaces are forbidden. Use an established domain namespace or the actual owner; do not substitute `Detail/Internal/Private/Local/Implementation/Helpers/Common/Misc`. Indent named namespaces and omit namespace-end comments.

Match the filename to its primary type or operation. Independent state/lifetime/responsibility may justify a file pair; small records can stay with their sole owner. Folders express ownership, lifetime, capability or backend.

### Helper placement

Put neutral format, descriptor, resource and stage validity with the neutral contract. Keep native conversion, device queries and backend workarounds in the backend. Search sibling implementations before adding another predicate, switch, record or derived field.

Portable byte, hashing, UTF-8, JSON, identifier and path mechanisms extend Core. Product schemas and feature policy stay with their module. Avoid forwarding namespaces, helper bags and one-method collaborators. When fixing a shared rule, inspect siblings and remove replaced helpers/records/conversions together.

### One-field types

Use the value or existing owner directly when every caller immediately unwraps a record. A one-field type is justified by:

- A strong ID, handle, unit or index with meaningful validity, identity or ownership.
- RAII/PIMPL lifetime, move behavior or an incomplete-type boundary.
- A tagged command, variant or ECS type used by dispatch/schema.
- A required external, cooked, platform, reflection or CPU/shader ABI.
- A fixed mathematical object or generic storage preserving type, lifetime or type erasure.

Future fields, naming symmetry and member-access appearance are insufficient. Remove wrappers that no longer satisfy a real contract and update their callers.

## Naming and vocabulary

Types, functions, methods and enum values use `UpperCamelCase`; parameters/locals use `lowerCamelCase`; private members use `m_`. **All boolean variables, fields, constants and atomics use `bUpperCamelCase`**, preserving existing scope prefixes such as `m_bReady` and `s_bReady`. Apply boolean naming to C# check helpers too. External serialized/native names remain exact.

Use the surrounding context without repeating an operation's entire name. Acronyms use `Rhi/Gpu/Io`; macros serve build/export/preprocessor requirements. Write TLAS/PTLAS in prose/UI and `Tlas/Ptlas` in C++.

Avoid vague `Manager/Helper/Util/Data/Info`, unsupported `ThreadSafe/LockFree/Async` claims and replacement `New/*2/Legacy` names.

| Term | Meaning |
| --- | --- |
| Handle | Identity |
| Token | Completion or authority |
| Lease | Exclusive borrow |
| Scope | Lifetime and settlement |
| Context | Bounded call state |
| Lane | Scheduling policy |
| Graph / Execution / Desc | Reusable topology / one run / immutable description |
| Build or Compile | Create a plan/product |
| Record / Submit / Execute | Write commands / transfer work / perform work |
| Commit / Publish / Apply | Mutate authoritative state / expose immutable results / consume an accepted change |
| Extract / Retire | Derive a representation / delay reuse until completion |
| Find / Resolve | Search for an existing match / determine a canonical value; neither implies load/open/acquire |

Distinguish persistent authoring GUIDs, source/reimport keys, runtime `EntityId`, separate `RenderObjectId`, contiguous indices and content hashes.

Keep `RenderFrame` for frame-slot scene/view data and `RenderFrameGraph*` for topology/resources. `PassCommandContext` is narrower than `RenderCommandContext`; native recording contexts stay backend-private. Name queues for their payload. Avoid general context/service/resource bags.

Neutral APIs use `RenderPipeline`, `GraphicsPipelineDesc/ComputePipelineDesc`, `Create*Pipeline`, `SetPipeline`, `renderTarget(s)` and `depthStencil`; PSO/RTV/DSV stay native. Use `GpuMesh/GpuMeshCache` and `commandContext`, not `GPU*/cmd`. `WorkspaceRoot` includes extracted products; `RepositoryRoot` refers to source/provenance. Finding/resolving, loading and compiling have distinct meanings.

Keep physical units and CPU/shader names explicit: illuminance/intensity/luminance, radians, `DeltaTimeSeconds`, `FovY/NearZ/FarZ`, `WorldMatrix/PreviousWorldMatrix/WorldInverseTranspose`, directional temporal matrices, `CurrentJitterNdc/PreviousJitterNdc`, `ViewportSizeInv`, `JointMatrix/Skinning/SkinInfluence`, `MorphTarget/MorphWeight` and `GpuSceneSlot`. A GPU slot is not identity. Rename mirrors together and check layout and target compilation.

## Coordinates and units

World coordinates are left-handed: +X right, +Y up, +Z forward. Use metres, seconds and radians. Degree-valued UI/source fields say `Degrees` and convert once. Positions, light ranges, animation translations and camera clip distances use the same units.

Positions multiply matrices as row vectors: `point * matrix`. Compose local transforms as scale * rotation * translation; a child's world transform is local * parent world. Normals use the inverse transpose.

Mathematical convention and byte layout are different contracts. Pack or transpose once at the shader/native boundary and check a nontrivial known transform. Importers normalize source axes/units once, including winding, tangent signs, camera/light directions, animation and bind transforms. Cooked/runtime data stays canonical. State space and unit when the type/name cannot carry them.

## Runtime CVars and shader rules

Read a runtime CVar where its subsystem decides policy. Do not forward the CVar or values copied solely from it through application-defined parameters, settings/context records or helpers. Standard-library calls and immediate output construction are ordinary value use.

A topology owner may remember what it built for change detection, without sending that cache back as policy. Editor/persistence may capture/apply complete settings for their actual role.

A frontend must not mutate Renderer CVars across threads. Global changes use the sequenced owner-thread control path. Per-view choices use one `RenderViewMode` stored in the viewport/View, without mirrored Editor enums, show flags, RHI fields or UI metadata.

Shaders follow the same readability and responsibility rules. Put attributes on their own line before the governed declaration/control statement. Separate input checks/output from shared mathematics and feature policy.

Validate domains at construction, publication or dispatch. Avoid repeated `IsValid/IsSupported/Try*`, range/finite checks, sentinels and validity plumbing inside BSDF, lighting, sampling, traversal and accumulation. Keep mathematical support, sidedness, visibility, random outcomes and required terminal branches, plus checks for values that can actually change after input validation. Fix missing guarantees at the input boundary.

Document shader differences required by syntax, ABI, waves, resources or measured GPU behavior. Shader-style changes need representative DXIL/SPIR-V compilation, reflection/layout and affected cooked-package checks. Scripts/build files follow their own language tools and local conventions.

## Includes and enforcement

Include direct dependencies and forward-declare when valid. Keep PCH assumptions, private headers and native backend headers private. Public out-of-line functions use the module export macro.

Use `Windows.h` or the project wrapper instead of SDK implementation headers. Preserve include groups/order; detailed ordering remains open. Remove unused includes and update CMake, source groups, exports, package rules and callers when moving files.

Check every added/modified source, header and shader after the responsibility review:

```powershell
CMake\CodeStyle.bat -Mode Check -Path "<repo-relative-file>"
git diff --check
```

Repeat per file. Use `-Staged` only when it covers the whole change; it checks working-tree content, so inspect the actual index for partial staging. `-ManifestPath` records file selection.

Record the tool, coverage, mechanical result and manual layout review. Repeat affected checks after edits and identify unchecked files. Documentation changes use documentation checks.

Review ownership, public surface, integration points, repeated switches, dependencies and removal of the old path. Formatting/build success is not a design review. External examples and proposed rules do not override these conventions; update the guide and executable checks together when accepting a change.
