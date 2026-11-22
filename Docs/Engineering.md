# Developer guide

[Documentation](README.md) | [Code style](CodeStyle.md)

This guide covers setup, module boundaries, development and validation. For the current work and release limits, see the [roadmap](Roadmap.md).

## Build and run

### Requirements

The recorded Windows toolchain is:

| Tool | Version |
| --- | --- |
| Visual Studio Community / MSVC | 2026 18.7.0 / 14.51.36231 |
| Windows SDK | 10.0.26100.0 |
| CMake / Git for Windows | 4.3.3 / 2.54.0 |
| Qt | 6.11.1, MSVC x64 |
| Vulkan SDK | 1.4.350.0, with Slang 2026.8 and DXC 1.9.0.5347 |

Other combinations need separate testing. These versions describe the recorded setup, not the latest available releases.

The project minimum is CMake 3.30; the [Visual Studio 18 2026 generator](https://cmake.org/cmake/help/latest/generator/Visual%20Studio%2018%202026.html) below requires 4.2+. Launcher uses four build jobs; direct builds select `--parallel`. Private PCHs do not replace explicit includes.

### Build Launcher

Use a short checkout path. Run these commands from the repository root with your Qt path. Stop if a command fails.

```powershell
cmake -S Tools/Launcher -B build/private/tools/SparkleLauncher -G "Visual Studio 18 2026" -A x64 -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/msvc2022_64
cmake --build build/private/tools/SparkleLauncher --config DevelopmentEditor --target SparkleLauncher --parallel 4
& ./artifacts/dev/Tools/SparkleLauncher/Binaries/DevelopmentEditor/SparkleLauncher.exe
```

Launcher selects content and builds, cooks and runs Editor or Game. Select scenes before cooking. Empty requires no example download.

### Command-line example

With only Empty selected in `Projects/Showcase/Levels.catalog`, prepare and run an Empty Game. Stop if any command fails:

```powershell
$launcher = './artifacts/dev/Tools/SparkleLauncher/Binaries/DevelopmentEditor/SparkleLauncher.exe'
$profiles = '--editor-profile', 'ShippingEditor', '--game-profile', 'ShippingGame'
& $launcher @profiles --run workspace.sync-code
& $launcher @profiles --run workspace.build.game
& $launcher @profiles --run cook.tools.prepare
& $launcher @profiles --run cook.all
& $launcher @profiles --level Empty --product game --graphics-api d3d12 --run levels.run
```

`--level` selects sync/run, not cook scope. Only selected assets download. Offline use requires completed preparation.

Build and dependency files are under `build/`; binaries and cooked content are under `artifacts/dev/`. [SparkleArtifactContract.cmake](../CMake/SparkleArtifactContract.cmake) defines the paths. Source and extracted products use the same workspace layout.

The root [CMakeLists.txt](../CMakeLists.txt) composes the project. Shared policy lives under `CMake/`; [EngineModule.cmake](../Engine/CMake/EngineModule.cmake) owns module linkage/exports. Modules declare sources, dependencies and SDK/deployment rules. Runtime inventories follow linked targets, including private transitive dependencies; owners explicitly stage dynamic plugins/data. Keep one artifact layout and dependency inventory.

Cook cleanup affects the current project's selected output categories. Scenes And Meshes includes dependent scene assets and the registry; textures and shaders have separate scopes. Clean Cook All and Cooked Outputs remove the project's entire cooked tree.

## Making a change

1. Check `git status --short` and `git rev-parse HEAD`. Preserve unrelated changes.
2. Read the affected code, build configuration and callers. Search for existing equivalents with `rg` before adding or renaming a concept.
3. Explain the intended result, user, owner, important risks and useful check in the issue or change description. Choose numerical thresholds before measuring.
4. Work in reviewable batches. Read complete affected functions, fix review findings and remove replaced code as the change lands.
5. Recheck the final diff, run applicable checks and `git diff --check`, and record commands, results, limitations and the next step. Update affected build files; apply the [documentation rules](#writing-documentation) to guide changes.

Reuse earlier results when their inputs still match. Changed inputs need the affected checks again; an unrelated revision does not require restarting the work.

## Module boundaries

A system owns its algorithms, state, validation, cancellation and lifetime. Callers use focused interfaces; Application and other composition code connect implementations. Core behavior must not depend on an Editor panel or Launcher operation.

Put stable contracts in `Public` and implementation in `Private`. New public APIs need two real module consumers or one necessary host/module boundary. Judge existing foundation contracts by purpose and semantics, including when they have no current callers.

Core is the base dependency. GameFramework does not depend on Renderer or RHI; RHI does not depend on Renderer. Native types remain inside the backend or an explicit interop interface. Dependencies follow actual public use, not convenient transitive includes.

Use the [dependency workflow](ThirdPartyMigration.md#ownership-and-dependency-workflow) for external source separation, pinning, sync and distribution metadata.

For replacements, name the mechanism being retired and the policy retained. Assess the complete producer-to-consumer change, including callers and adapters. Moving code earns no removal credit; report non-native, generated, dependency and build/runtime costs separately. Keep capability additions distinct from simplification.

Orchestration shows what happens and in what order. Keep cohesive algorithms/state machines together and split independently changing responsibilities. Avoid god classes, numbered fragments, forwarding-only collaborators, helper bags and service locators. Assess public surface, repetition and dependencies as well as size.

Keep feature state and mechanism in one predictable location. New caches, queues, layers, flags or APIs need a current user and a reason simpler code is insufficient. Prefer direct code to speculative extensibility.

### File placement

Group module implementation by capability, lifetime or backend. [External precedents](References.md#code-style) inform placement; Sparkle's contracts and dependencies decide it.

| Location | Contents |
| --- | --- |
| `<module>/CMakeLists.txt` | Target composition, source membership and dependencies |
| `<module>/Public/` | Contracts used across module or host boundaries; organize larger surfaces by capability |
| `<module>/Private/` | Implementation, private headers, PCH and entry points, grouped by capability or backend where useful |
| `<module>/CMake/` | Target-specific SDK discovery, source selection, asset generation and deployment |
| `<module>/Assets/` | Authored assets consumed by that module |

Keep headers beside their implementation and includes relative to the owning public or private root. Add folders for responsibilities, not file counts; small cohesive surfaces may stay flat. Avoid generic `Features`, `Helpers` or `Utils` levels. Preserve shared invariants and lifetimes when splitting code.

The source owners are:

| Module or source root | Responsibility and placement |
| --- | --- |
| `Engine/Core` | Product-independent files, paths, processes, input values, math, logging and time |
| `Engine/Tasks` | Task graphs, execution and scope lifetime; callers own scheduling policy |
| `Engine/Platform` | Native window/input; Core owns shared input values |
| `Engine/RHI` | GPU contracts, validation and D3D12/Vulkan backends |
| `Engine/Renderer` | Scene/view preparation, GPU scene, frame graphs, passes and image providers |
| `Engine/GameFramework` | Levels, asset loading, animation, world state and immutable render extraction |
| `Engine/Editor` | Inspection, editing and viewport presentation |
| `Engine/Application` | Runtime and Editor host composition |
| `Engine/Assets` | Shader passes, shared shader mathematics/material rules and default textures |
| `Tools/Import/SourceImporters` | Offline source-format adapters and imported scene contracts |
| `Tools/Cooking` | Asset discovery/dispatch and asset-family processing/publication |
| `Tools/Shaders/ShaderCompiler` | Compiler backends, source resolution, verification and cooked shader publication |
| `Tools/Support/ToolSupport` | Shared catalogs, paths and console/progress contracts |
| `Tools/Launcher/SparkleLauncher` | Workspace/build/cook/run operations, shell, GUI and scene selection |
| `Projects/Showcase` | Thin product entry points, project assets/levels and offline conversion |

## Data and lifetime

Borrow or move ownership before copying. Extra storage needs a reason, authority, lifetime and invalidation rule. Publish complete immutable results; derived Editor/Renderer data must not mutate world state.

Choose layout from access patterns and data size. Check bounds/arithmetic and define CPU/GPU representations explicitly. Prefer values, `unique_ptr` for sole heap ownership and `shared_ptr` for shared lifetime.

Spans and callback captures do not extend lifetime. Finish cancelled work before destroying its inputs. GPU resources remain alive until every queue using them completes.

Validate immutable inputs at admission and express guaranteed presence through references or construction. Check changing identity, capacity, thread ownership and GPU retirement where consumed. Report the operation, resource and native error there; avoid duplicate caller checks/logs.

Reject invalid/unsupported input before partial scheduling or publication where possible. Dummy values, clears, copies and swallowed errors cannot substitute for required work.

Editor navigation changes the viewport session, not authored cameras or undo history. Typed commands change world state; only explicit Save All writes the selected authored level. Failed edits, saves and replacement loads preserve the accepted state.

### Compatibility

There is no shipped compatibility contract, supported persisted user data or public plugin ABI. Change owned representations in place, update producers/consumers, remove old paths and regenerate disposable output. Do not add internal schema/content/ABI versions, migrations, aliases, legacy switches or dual paths.

External versions, build provenance and stale-handle/lifetime counters are valid. Persisted user data or installed external consumers need a product decision. Feature removal updates scope, selectors, dependencies and preservation checks.

## Verification

[Code style](CodeStyle.md) owns source conventions and formatter commands. Start with the diff and static checks; review behavior and responsibility, then build the smallest affected target. Use focused runtime, failure, numerical or native-API checks; broaden to full builds, cooks and clean-machine tests when needed.

In the configured root workspace, run `code_style_check` or `ShaderCompilerCliValidation` with `cmake --build build --config <Profile> --target <Target>`. The standalone Launcher lacks these targets. Shader validation requires `SPARKLE_ENABLE_SHADER_COMPILER=ON` and cooks DXIL/SPIR-V into Shared's cooked shader directory.

Keep executable checks with the invariant they test. Review ownership and dependency direction with the affected code; avoid a separate source-text policy engine for historical filenames or implementation conventions.

Export changes need a real shared-library caller. Source/header moves need regenerated build membership. Check worker-count differences, cancellation, stale generations, bounds and delayed GPU completion where relevant.

Record source/local changes, product/input identity, configuration, environment, command, expected result/tolerance and observation. State missing coverage and exclusions. Follow Launcher handoffs to the responsive child application.

Permanent test-only additions need explicit project approval; remove local probes before submission. Preserve existing tests unless removal is agreed. Keep original results in ignored output and review generated code.

### Script ownership

Each script needs a workflow, caller/documented command and useful result. Prefer existing build, compiler, formatter and GPU tools; retain project-specific style rules. Remove unused workflows with their helpers. [External examples](References.md#pipeline-automation) inform these choices.

Entry points validate inputs and compose subject-owned capabilities; keep process settlement and rollback with their operation. Shared capabilities live under their subject in `CMake/`. Moves update callers/package paths and remove replaced scripts. Check syntax and affected success/failure paths.

### Performance and images

Fix inputs, settings, warm-up, repeats, image domain and reference uncertainty before measuring. Report frame-time distributions, memory, overhead and whether cost improves, stays bounded or remains unmeasured. Remove unnecessary work before parallelizing.

Keep runs/machines separate. Choose absolute/relative thresholds and statistics accounting for correlated frames. An interval crossing a threshold is inconclusive. Worst-frame comparisons need equal sample counts or an explicit model; pooled percentiles cannot replace stable runs and correct images.

Separate native captures from optimized timing; match frames, shaders and symbols. Distinguish CPU time, GPU intervals and presented latency, and logical bytes, allocator storage, device budgets and high-water memory.

## Build and delivery

Run loads level definitions and cooked dependencies without requiring raw assets or source-selection flags. Game presents the scene; Editor provides inspection and editing.

Prepare the release base with a complete build and cook. [ReleaseProducts.cmake](../CMake/Packaging/ReleaseProducts.cmake) declares `release_package`; [Package.ps1](../CMake/Packaging/Package.ps1) requires canonical `build/`, reviewed `SPARKLE_RELEASE_INPUTS`, matching source/product hashes and the final `--shader-symbols` export. It publishes an **unsigned internal** archive with a path, SHA-256, manifest, SBOM and provenance. Staging rejects changed qualified bytes. Signing and public-release qualification remain open in [Roadmap](Roadmap.md#before-the-first-release).

The combined package requires **both ShippingEditor and ShippingGame inventories**. Multi-config builds produce both; a single Shipping configuration supplies only its own inventory.

Matching source can import the prepared base with [SyncProducts.ps1](../CMake/Packaging/SyncProducts.ps1), then rebuild/cook into the same outputs. Installer/updater, old-store migration and a freshness service are outside scope.

Scene selection does not grant redistribution rights. Preserve export restrictions on unqualified Classic/Modern Sponza derivatives and previews. Local acquisition, public archives and repository history are separate distribution cases. Keep [Qt notices](../CMake/Notices/Qt/README.md), [asset recipes](../Projects/Showcase/Assets/Meshes) and [thumbnail provenance](../Projects/Showcase/Launcher/Thumbnails/README.md).

Test the final package's file/dependency list, standard-user launch, relocation, writable paths, source rebuild, scene output, symbols, failure/recovery and signing/download identity. Check rights, notices and provenance for those actual files. Review GPU dumps, debug data, source and symbols before sharing incident files.

Cookers stage complete validated output before publication. Core owns containment, locks, incomplete markers and in-process rollback. Readers retain guards through their final filesystem access, including worker reads; decoding may continue from owned bytes afterward. Interrupted publication or failed rollback requires cleanup and recook, with source/settings outside disposable output. Persisted journal replay and power-loss recovery are unsupported.

## Writing documentation

Keep Engineering, Goals, References, Rendering and Roadmap at the scope and level of detail of the [November 8, 2026 guide set](https://github.com/LisowskiPawel/SparkleEngine/tree/dcd5b4d027543aeb11a133ac40da64cc74ce5a77/Docs). This is an editorial reference; behavior and decisions stay current.

Add material only for a recurring reader question about purpose, scope, a lasting decision, a required workflow or a product/comparison limit. Cite primary sources for those claims. Search the [existing owner](README.md#guides), including [dependency decisions](ThirdPartyMigration.md), before writing; revise its section and link from other guides.

Keep minimum runnable setup commands and important current limits. Put API/wire detail and per-file inventories with their code owner. Keep stage checklists and individual run results out of main guides. Use one concise `Docs/Implementation/<Feature>.md` for unfinished delivery and acceptance/failure checks; keep raw evidence in ignored output. After each verified stage, remove its heading, checklist, prompt and progress text in the same run; partial stages retain only missing work. Completion is marked by removal, with receipts kept outside the active plan. Preserve stage numbers, repair links and delete the plan when its last required item closes, retaining lasting decisions and unresolved product limits in their existing owners.

Review every addition for necessity, ownership, duplication and level of detail against the original guide set. New sections or files need independent recurring use; moving clutter into a catalogue or hierarchy does not qualify. Remove repetition and obsolete text in the same edit. Preserve numerical requirements, archived role wording and provenance; check links/anchors, UTF-8, whitespace and `git diff --check`.
