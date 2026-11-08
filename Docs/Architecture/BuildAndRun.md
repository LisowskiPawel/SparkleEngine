# Build, Launcher and delivery

[Docs](../README.md) ? [Subjects](README.md) ? [Quick start](../../README.md#build-and-launch)

Launcher is the common front door for scene selection, sync, Configure/Build/Cook/Run and actionable progress/failure. The target prebuilt journey is extract one bundle, open `LaunchSparkle.cmd`, choose a supplied scene and Run. Public distribution is still being qualified.

## Source workflow

The documented toolchain is Windows 11, Visual Studio 2026 C++ tools, CMake 4.3.3, Qt 6.11.1 MSVC x64 and Vulkan SDK 1.4.350.0. These are the existing qualification inputs; other versions need their own checks. Use a short checkout path and the exact Launcher build/CLI commands in the [root quick start](../../README.md#build-and-launch).

- Source/dependency/build state lives in `build/`; binaries and cooked products live in `artifacts/dev/`. [Artifact configuration](../../CMake/SparkleArtifactContract.cmake) owns the exact grammar.
- Select maps before cooking. `--level` selects sync/run, not cook scope. Empty needs no downloaded example content. Offline use needs completed sync/cook.
- Run consumes ordinary level definitions, executable and cooked dependencies; it does not require raw assets, acquisition manifests or source-selection flags.
- Sync/Cook owns preparation prerequisites. Missing input fails before download/publication. Readiness preflight does not prove actual output.
- Preserve Launcher selection/previews, profile/API/product choice, progress, cancellation, logs and recovery. Reuse its existing operation/settings owners; no general workflow platform is selected.

[Launcher source](../../Tools/Launcher), [build profiles](../../CMake/SparkleBuildProfiles.cmake) and [level catalog](../../Projects/Showcase/Levels.catalog) are the implementation references.

## Prebuilt and source rebuild

Publisher prepares fresh binaries and the complete legally admitted cooked base through existing build/cook owners. Source adopters obtain the matching tag and may import its base with [SyncProducts.ps1](../../CMake/Packaging/SyncProducts.ps1), then edit and rebuild/cook into the same canonical outputs. Source-only preparation remains supported.

There is one runtime layout and no old-store migration, per-file freshness service or installed-mode path branch. Rebuilt output replaces imported output. Game retains rendering, camera input and native close; Editor retains its separate UI and supported tools.

## Release inputs

[ReleaseProducts.cmake](../../CMake/Packaging/ReleaseProducts.cmake) owns the stage/verify/archive operations and their declared membership. Build -> full Cook produces inputs; freeze -> stage -> verify -> ZIP produces a candidate. Do not infer missing membership by scanning unrelated development output.

Before publication, bind actual payload bytes to dependency pins, licenses/notices, source obligations, provenance/SBOM, symbols, signatures and content rights. [Qt notices](../../CMake/Notices/Qt/README.md), [source asset recipes](../../Projects/Showcase/Assets/Meshes) and [thumbnail provenance](../../Projects/Showcase/Launcher/Thumbnails/README.md) retain legal ownership.

**Release-supported is not redistribution-cleared.** Classic and Modern Sponza source/cooked derivatives and previews have explicit public-export restrictions pending rights review. Acquisition for local use and a public archive are separate surfaces; repository history also needs its own rights decision. Preserve existing exclusions rather than admitting content through a catalog flag.

## Delivery work

The recorded unsigned `a0f34c8b...` package retains known scene-publication interruption defect `S1` / `ISSUE-3`. Changed source/publication code needs affected checks and a repaired immutable archive; earlier assembly or source results cannot qualify new bytes.

Under FR-06, prove that exact archive's membership/imports, relocation, standard-user launch, writable-state boundaries, no-source/tool-free consumer path, optional matching-source rebuild, Game/Editor separation, symbols, failures and recovery. Observe actual scene/camera/material/presented output on the selected map/API matrix. Activation alone cannot pass a blank or wrong frame. Reuse applicable original receipts; do not infer fresh-Windows or broader hardware proof from internal laptop runs.

FR-07 closes deferred Windows certificate/signing/timestamping, signed-byte verification, exact public rights and downloaded-byte verification. All required performance, native and feature proof must still apply to those final bytes.

## Release limits

No qualified public download is claimed. Package removal was explicitly excluded from the earlier candidate; it has no inferred pass. There is no installer/updater, account service, remote build/upload or ongoing support service. Finite closeout follows the [roadmap](../Roadmap.md#release-work).

When Windows application control blocks a development tool, identify the actual failing executable and typed start error; a nested cooker failure may need its parent log. Use the publisher trust/signing route. Launcher cannot create signing trust.
