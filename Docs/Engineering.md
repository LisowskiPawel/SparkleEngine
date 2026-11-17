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

Source builds require CMake 3.27 or newer. Launcher requests four parallel build jobs; direct builds can choose the count with `--parallel`. Private PCHs speed compilation but do not replace explicit includes.

### Build Launcher

Use a short checkout path. Run these commands from the repository root with your Qt path. Stop if a command fails.

```powershell
cmake -S Tools/Launcher -B build/private/tools/SparkleLauncher -G "Visual Studio 18 2026" -A x64 -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/msvc2022_64
cmake --build build/private/tools/SparkleLauncher --config DevelopmentEditor --target SparkleLauncher --parallel 4
& ./artifacts/dev/Tools/SparkleLauncher/Binaries/DevelopmentEditor/SparkleLauncher.exe
```

Launcher selects content and builds, cooks and runs Editor or Game. Select scenes before cooking. Empty requires no example download.

### Command-line example

To prepare and run an Empty Game:

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

The root [CMakeLists.txt](../CMakeLists.txt) composes dependencies, Engine, Tools and projects. Shared build policy lives under `CMake/`; target-specific backend, SDK and deployment rules stay beside their module. [EngineModule.cmake](../Engine/CMake/EngineModule.cmake) owns shared linkage/export rules; each module declares its sources and dependencies.

Linked targets own runtime dependencies. Staging and release inventories derive DLL membership from that graph, including private transitive dependencies and imported SDK targets. Dynamically loaded plugins and data files use their owner's deployment rules. Keep one artifact layout and dependency inventory.

Cleaning a cook action removes its selected outputs; scene cleanup includes its dependent assets and registry. Clean Cook All and the Cooked Outputs scope remove all cooked content.

## Making a change

1. Check `git status --short` and `git rev-parse HEAD`. Preserve unrelated changes.
2. Read the affected code, build configuration and callers. Search for existing equivalents with `rg` before adding or renaming a concept.
3. Explain the intended result, user, owner, important risks and useful check in the issue or change description. Choose numerical thresholds before measuring.
4. Work in reviewable batches. Read complete affected functions, fix review findings and remove replaced code as the change lands.
5. Recheck the final diff, run applicable checks and `git diff --check`, and record commands, results, limitations and the next step. Update affected build files and docs.

Reuse earlier results when their inputs still match. Changed inputs need the affected checks again; an unrelated revision does not require restarting the work.

## Module boundaries

A system owns its algorithms, state, validation, cancellation and lifetime. Callers use focused interfaces; Application and other composition code connect implementations. Core behavior must not depend on an Editor panel or Launcher operation.

Put stable cross-module contracts in `Public` and implementation in `Private`. New public APIs need two real module consumers or one necessary host/module boundary. Review existing foundation contracts by purpose and semantics before removal; a lack of current callers alone does not make them obsolete.

Core is the base dependency. GameFramework does not depend on Renderer or RHI; RHI does not depend on Renderer. Native types remain inside the backend or an explicit interop interface. Dependencies follow actual public use, not convenient transitive includes.

For external libraries, use the existing [dependency workflow](ThirdPartyMigration.md#ownership-and-dependency-workflow) for source separation, pinning, Launcher sync and distribution metadata.

Orchestration should show what happens and in what order. Keep cohesive algorithms/state machines with their owner and split independently changing responsibilities. Avoid god classes, numbered fragments, forwarding-only collaborators, helper bags and service locators. Review public surface, repeated logic, callers, dependency direction and removal of the old path; fewer lines or files alone do not establish an improvement.

Keep a feature's state and mechanism in one predictable location. Changes elsewhere need an integration reason and a check. A new cache, queue, layer, flag or API needs a current user and a reason simpler code is insufficient. Prefer direct code or a closed enum switch to speculative extensibility.

### File placement

Use the module as the ownership boundary, then group implementation by capability, lifetime or backend. This follows the interface/implementation separation in the [Epic module guide](References.md#code-style) and the backend separation in the referenced NVRHI build. Sparkle's contracts and dependencies determine the actual layout.

| Location | Contents |
| --- | --- |
| `<module>/CMakeLists.txt` | Target composition, source membership and dependencies |
| `<module>/Public/` | Contracts used across module or host boundaries; organize larger surfaces by capability |
| `<module>/Private/` | Implementation, private headers, PCH and executable entry points |
| `<module>/Private/<capability>/` | A cohesive algorithm, state machine or workflow and its supporting types |
| `<module>/Private/<backend>/` | Native implementation of the module's contracts, with capability folders where useful |
| `<module>/CMake/` | Target-specific SDK discovery, source selection, asset generation and deployment |
| `<module>/Assets/` | Authored assets consumed by that module |

Keep headers beside the implementation they describe; public contracts remain in `Public`. Use includes relative to the owning public or private root instead of reaching through `../Private`. Small cohesive surfaces may stay flat. Add a folder when it identifies a responsibility; avoid generic `Features`, `Helpers` or `Utils` levels and folders created only to hold one class. A large folder alone does not justify splitting a shared invariant or lifetime.

The source owners are:

| Module or source root | Responsibility and placement |
| --- | --- |
| `Engine/Core` | Product-independent files, paths, processes, input values, math, logging and time |
| `Engine/Tasks` | Graph construction, execution, scheduling, scope lifetime and profiling; product scheduling policy stays with callers |
| `Engine/Platform` | Native window/input implementation; Core owns the shared input values |
| `Engine/RHI` | Device/resource/command contracts, common validation and separate D3D12/Vulkan implementations |
| `Engine/Renderer` | Scene/view preparation, GPU scene state, frame graphs, passes and image providers; vendor adapters remain separate from neutral orchestration |
| `Engine/GameFramework` | Authored levels, cooked asset loading, animation and world state; ECS, editing, publication and render extraction stay private |
| `Engine/Editor` | Inspection, editing controls and workspace/viewport presentation |
| `Engine/Application` | Runtime host composition and startup, with separate Editor host workflows |
| `Engine/Assets` | Shared shader source and default textures; passes own entry points, subjects own shared mathematics/material rules |
| `Tools/Import/SourceImporters` | Source-format adapters and their imported scene contracts; source decoding stays outside runtime modules |
| `Tools/Cooking` | AssetCooker discovers/plans/dispatches; Mesh, Material, Scene and Texture cookers own construction, processing and publication for their asset families |
| `Tools/Shaders/ShaderCompiler` | Compiler backends, source resolution, verification and cooked shader publication |
| `Tools/Support/ToolSupport` | Shared catalog parsing, source/output paths and console/progress contracts; readers use the wire format, producers use the synchronized writer or a borrowed callback |
| `Tools/Launcher/SparkleLauncher` | Workspace/build/cook/run operations, shell, GUI and selection edits to `Levels.catalog`; operation policy stays outside Engine |
| `Projects/Showcase` | Thin Editor/Game entry points in `Private`, project assets, level definitions and project-specific offline conversion |

## Data and lifetime

Borrow with references, views or handles, or move ownership, before copying. Extra storage needs a reason, an authoritative source, a lifetime and an invalidation rule. Publish complete immutable results; do not modify world state through derived Editor or Renderer data.

Choose layout from actual access patterns and data size. Check bounds and arithmetic sizes, and define CPU/GPU representations explicitly. Use values for immutable data, `unique_ptr` for sole heap ownership and `shared_ptr` for genuinely shared lifetime.

Spans and callback captures do not extend lifetime. Finish cancelled work before destroying its inputs. GPU resources remain alive until every queue using them completes.

Required work needs an implementation. Reject invalid or unsupported input before partially scheduling or publishing a result where possible. A dummy value, clear, copy or swallowed error cannot substitute for the requested operation.

Editor navigation changes the viewport session, not authored cameras or undo history. Typed commands change world state; only explicit Save All writes the selected authored level. Failed edits, saves and replacement loads preserve the accepted state.

### Compatibility

The project has no shipped compatibility contract, supported persisted user data or public plugin ABI. Change owned representations in place, update all producers/consumers, remove the old path and regenerate disposable output. Do not add internal schema/content/ABI versions, migrations, deprecated aliases, legacy switches or dual paths.

External versions, build provenance and counters for stale-handle/lifetime checks are valid. Support for persisted user data or installed external consumers needs a product decision. Removing a feature also updates its advertised scope, selectors, dependencies and preservation checks.

## Verification

[Code style](CodeStyle.md) covers source conventions and formatter commands. Review responsibility and behavior as well as formatting.

Start with the diff and static checks, then build the smallest affected target. Use focused runtime, failure, numerical or native-API checks for the behavior in question. Run broad builds, cooks and clean-machine tests when the change requires that coverage.

Run the existing check owners directly: `code_style_check` and `ShaderCompilerCliValidation` through `cmake --build build --config <Profile> --target <Target>`. Keep the command, source identity, logs and result with the work being validated.

CMake owns build composition, SDK integration, shader tooling and deployment. Review module dependency direction and public/private boundaries with the affected code; keep executable checks close to the invariant they test. RHI configuration checks backend source membership, while shader CLI validation exercises actual compiler products. Avoid a separate source-text policy engine for historical filenames or implementation conventions.

Export changes need a real shared-library caller. Source/header moves need regenerated build membership. Check worker-count differences, cancellation, stale generations, bounds and delayed GPU completion where relevant.

Record the revision/local changes, configuration, executable and input identity, hardware/driver/tools, command, expected result/tolerance and observation. Explain missing checks and exclusions. Follow Launcher process handoffs through to the responsive child application.

Permanent test-only additions require explicit project approval. Keep temporary probes local and remove them before submission. Preserve existing tests unless their removal is agreed. Store shareable original results in ignored output and review generated code as carefully as other contributions.

### Script ownership

Keep scripts with a named workflow, a build/CI caller or documented command, and a useful result. Prefer existing CMake, compiler, formatter and GPU tools over another runner, parser or reporting layer. Maintain project-specific style checks with their rules; remove unused workflows with their helpers and controls. [External examples](References.md#pipeline-automation) provide precedent; local use and maintenance cost decide ownership.

PowerShell entry points validate arguments and compose operations with explicit inputs. Dot-source capabilities from `$PSScriptRoot`; keep subprocess settlement and rollback with their owning operation. Shared script capabilities live under their subject in `CMake/`, following the same ownership rules as modules. Batch/command files stay small entry points. Moves update callers and package paths and remove replaced scripts. Check syntax and affected success/failure paths with local fixtures.

Qualification fixes the bytes admitted to later steps: staging must reject changed products rather than accept new hashes. One transaction publishes complete private output. GPU incident files stay local until their dump, debug, source and symbol contents have been reviewed for sharing.

### Performance and images

Fix the input, settings, warm-up, repeats, image domain and reference uncertainty before measuring. Report frame-time distributions, memory and measurement overhead. State whether cost improves, stays bounded, is unaffected or remains unmeasured. Remove unnecessary work before parallelizing.

Keep runs and machines separate. Choose absolute and relative effect thresholds and a statistical method that accounts for correlated frames. An interval crossing the threshold is inconclusive. Worst-frame comparisons need equal sample counts or an explicit model; pooled percentiles and a p-value cannot replace stable runs and correct images.

Inspect native captures separately from optimized timing runs. Match the frame, shaders and symbols. CPU time, GPU queue intervals and presented latency are different quantities, as are logical resource bytes, allocator storage, device budgets and high-water memory.

## Build and delivery

Run loads level definitions and cooked dependencies without requiring raw assets or source-selection flags. Game presents the scene; Editor provides inspection and editing.

Prepare the release base with a complete build and cook. [ReleaseProducts.cmake](../CMake/Packaging/ReleaseProducts.cmake) defines staging, verification and archive membership. Matching source can import that base with [SyncProducts.ps1](../CMake/Packaging/SyncProducts.ps1), then rebuild/cook into the same outputs. Installer/updater, old-store migration and a freshness service are outside scope.

Multi-config generators produce both Shipping inventories. Single-config generators produce the selected Shipping inventory and expose packaging only for a Shipping build.

Scene selection does not grant redistribution rights. Preserve export restrictions on unqualified Classic/Modern Sponza derivatives and previews. Local acquisition, public archives and repository history are separate distribution cases. Keep [Qt notices](../CMake/Notices/Qt/README.md), [asset recipes](../Projects/Showcase/Assets/Meshes) and [thumbnail provenance](../Projects/Showcase/Launcher/Thumbnails/README.md).

Test the final package's file/dependency list, standard-user launch, relocation, writable paths, source rebuild, scene output, symbols, failure/recovery and signing/download identity. Check rights, notices and provenance for those actual files. [Roadmap](Roadmap.md) records remaining work and earlier exclusions.

Cookers publish complete validated output. Core provides containment, locks, incomplete markers and in-process rollback; readers hold their scope through lookup, decode and worker completion. Interrupted publication or failed rollback makes disposable output unusable: reject it, clean and recook. This does not provide persisted journal replay or power-loss recovery. Source and settings stay outside the disposable output.

## Writing documentation

Write for people planning work or learning the engine. Start with purpose, then explain the behavior, targets, important choices and limits. Use descriptive headings, short paragraphs and tables for comparisons.

Keep goals, scope, priorities, lasting decisions, coding rules, numerical limits and primary references in the main guides. Each subject has one owner, as listed in [the index](README.md#guides). Update that owner when lasting behavior or guidance changes; link from other guides. Dependency evaluations cover selected work and concrete removal opportunities. Keep speculative catalogues, stage status, revision stamps, temporary baselines and delivery checklists out.

Use one concise `Docs/Implementation/<Feature>.md` per active feature for its plan, copyable prompts, acceptance criteria, failure checks and current progress. Update that same file at each stage; keep raw logs and comparison artifacts in ignored output.

At feature closure, fold only lasting guidance into the appropriate existing guide, delete the implementation file and remove the folder when empty. Do not archive plans or copy their progress history into the main guides.

Link to the owning guide instead of repeating it. Preserve archived role wording and its provenance. Remove requirement codes and obsolete procedures. Check links/anchors, navigation, UTF-8, whitespace and `git diff --check`.
