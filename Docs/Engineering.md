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

## Making a change

1. Check `git status --short` and `git rev-parse HEAD`. Preserve unrelated changes.
2. Read the affected code, build configuration and callers. Search for existing equivalents with `rg` before adding or renaming a concept.
3. Explain the intended result, user, owner, important risks and useful check in the issue or change description. Choose numerical thresholds before measuring.
4. Work in reviewable batches. Read complete affected functions, fix review findings and remove replaced code as the change lands.
5. Recheck the final diff, run applicable checks and `git diff --check`, and record commands, results, limitations and the next step. Update affected build files and docs.

Reuse earlier results when their inputs still match. Changed inputs need the affected checks again; an unrelated revision does not require restarting the work.

## Module boundaries

A system owns its algorithms, state, validation, cancellation and lifetime. Callers use focused interfaces; Application and other composition code connect implementations. Core behavior must not depend on an Editor panel or Launcher operation.

Put stable cross-module requests, results, handles and interfaces in `Public`; keep implementation in `Private`. A public API needs two real module consumers or one necessary host/module boundary.

Core is the base dependency. GameFramework does not depend on Renderer or RHI; RHI does not depend on Renderer. Native types remain inside the backend or an explicit interop interface. Dependencies follow actual public use, not convenient transitive includes.

Orchestration should show what happens and in what order. Put substantial algorithms and state in their owning capability. Keep cohesive algorithms/state machines together and split independently changing responsibilities. Avoid god classes, numbered fragments, forwarding-only collaborators, helper bags and service locators.

Keep a feature's state and mechanism in one predictable location. Changes elsewhere need an integration reason and a check. A new cache, queue, layer, flag or API needs a current user and a reason simpler code is insufficient. Prefer direct code or a closed enum switch to speculative extensibility.

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

Renderer/RHI boundary changes require:

```powershell
cmake -P CMake/ArchitectureBoundaryCheck.cmake
```

Export changes need a real shared-library caller. Source/header moves need regenerated build membership. Check worker-count differences, cancellation, stale generations, bounds and delayed GPU completion where relevant.

Record the revision/local changes, configuration, executable and input identity, hardware/driver/tools, command, expected result/tolerance and observation. Explain missing checks and exclusions. Follow Launcher process handoffs through to the responsive child application.

Permanent test-only additions require explicit project approval. Keep temporary probes local and remove them before submission. Preserve existing tests unless their removal is agreed. Store shareable original results in ignored output and review generated code as carefully as other contributions.

### Performance and images

Fix the input, settings, warm-up, repeats, image domain and reference uncertainty before measuring. Report frame-time distributions, memory and measurement overhead. State whether cost improves, stays bounded, is unaffected or remains unmeasured. Remove unnecessary work before parallelizing.

Keep runs and machines separate. Choose absolute and relative effect thresholds and a statistical method that accounts for correlated frames. An interval crossing the threshold is inconclusive. Worst-frame comparisons need equal sample counts or an explicit model; pooled percentiles and a p-value cannot replace stable runs and correct images.

Inspect native captures separately from optimized timing runs. Match the frame, shaders and symbols. CPU time, GPU queue intervals and presented latency are different quantities, as are logical resource bytes, allocator storage, device budgets and high-water memory.

## Build and delivery

Run consumes level definitions and cooked dependencies without requiring raw assets or source-selection flags. Game remains a rendering application; Editor retains its UI.

Publisher Build/full Cook prepares the complete base. [ReleaseProducts.cmake](../CMake/Packaging/ReleaseProducts.cmake) defines staging, verification and archive membership. Matching source can import that base with [SyncProducts.ps1](../CMake/Packaging/SyncProducts.ps1), then rebuild/cook into the same outputs. Installer/updater, old-store migration and a freshness service are outside scope.

Scene selection does not grant redistribution rights. Preserve export restrictions on unqualified Classic/Modern Sponza derivatives and previews. Local acquisition, public archives and repository history are separate distribution cases. Keep [Qt notices](../CMake/Notices/Qt/README.md), [asset recipes](../Projects/Showcase/Assets/Meshes) and [thumbnail provenance](../Projects/Showcase/Launcher/Thumbnails/README.md).

Test the final package's file/dependency list, standard-user launch, relocation, writable paths, source rebuild, scene output, symbols, failure/recovery and signing/download identity. Check rights, notices and provenance for those actual files. [Roadmap](Roadmap.md) records remaining work and earlier exclusions.

Cookers publish complete validated output. Core provides containment, locks, incomplete markers and in-process rollback; readers hold their scope through lookup, decode and worker completion. Interrupted publication or failed rollback makes disposable output unusable: reject it, clean and recook. This does not provide persisted journal replay or power-loss recovery. Source and settings stay outside the disposable output.

## Writing documentation

Use the existing guides. Explain purpose, current behavior, important choices, limits and the next useful action. Keep related information together and link deeper sources.

Preserve goals, decisions, numerical limits, open questions and primary references when editing. Remove repeated instructions, copied prompts, source inventories and obsolete procedures. Add a file only for a subject readers need independently. Check links/anchors, navigation, UTF-8, whitespace and `git diff --check`.
