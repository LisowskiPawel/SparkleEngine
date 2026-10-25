# SparkleEngine

A Windows graphics learning project with **Sparkle Launcher, Sparkle Editor and Sparkle Game**. The first packaged release is still being qualified.

## Start here

- [Engine at a glance](Docs/Architecture/EngineAtAGlance.md): features and current limits.
- [Release scope](Docs/Acceptance/FirstRelease.md#approved-release-scope) and [evidence](Docs/Acceptance/FirstRelease.md): planned release and remaining blockers.
- [Contributing](CONTRIBUTING.md): changes, review and verification.
- [Using Sparkle](SUPPORT.md): controls, recovery and local diagnostics.
- [Known issues](KNOWN_ISSUES.md) and [security](SECURITY.md).
- [Documentation](Docs/README.md): architecture and engineering owners.

## Build and launch

Use a short checkout path, such as `C:/src/SparkleEngine`. The [source prerequisites](Docs/Acceptance/FirstRelease.md#frozen-platform-and-toolchain-prerequisites) name Windows 11, Visual Studio 2026 C++ tools, CMake 4.3.3, Qt 6.11.1 MSVC x64 and Vulkan SDK 1.4.350.0; other versions are unqualified.

Clone [SparkleEngine](https://github.com/stoleckipawel/SparkleEngine). From its root, run in PowerShell with your Qt path:

```powershell
cmake -S Tools/Launcher -B build/private/tools/SparkleLauncher -G "Visual Studio 18 2026" -A x64 -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/msvc2022_64
cmake --build build/private/tools/SparkleLauncher --config DevelopmentEditor --target SparkleLauncher --parallel 4
& ./artifacts/dev/Tools/SparkleLauncher/Binaries/DevelopmentEditor/SparkleLauncher.exe
```

This builds Launcher. Open it to select content and build/cook/start Editor or Game, or use the same operations directly for the initial Empty Game:

```powershell
$launcher = './artifacts/dev/Tools/SparkleLauncher/Binaries/DevelopmentEditor/SparkleLauncher.exe'
$profiles = '--editor-profile', 'ShippingEditor', '--game-profile', 'ShippingGame'
& $launcher @profiles --run workspace.sync-code
& $launcher @profiles --run workspace.build.game
& $launcher @profiles --run cook.tools.prepare
& $launcher @profiles --run cook.all
& $launcher @profiles --level Empty --product game --graphics-api d3d12 --run levels.run
```

Stop at the first nonzero exit. Build/dependency state stays in `build/`; binaries and cooked content go to `artifacts/dev/`. Select maps in Launcher before cooking them; `--level` selects sync/run, not cook scope. Only selected assets download; offline use requires completed sync/cook. See [Launcher workflows](Docs/Architecture/Modules/Tools/Launcher/LauncherCapabilities.md) and [source budgets/cleanup](Docs/Acceptance/FirstRelease.md#frozen-paths-mutation-boundary-and-budgets). The `0.y.z` source API is unstable; no binary SDK or ABI compatibility is offered.

Check the configured workspace with `powershell -NoProfile -ExecutionPolicy Bypass -File CMake/RunBaselineChecks.ps1 -Configuration ShippingEditor`. Results stay under `artifacts/validation/baseline/`.

With an archive for your source revision, use the [precompiled-binary route](Docs/Architecture/Modules/BuildAndPackaging/PackagingAndInstallation.md#work-with-precompiled-binaries). It writes the same binary locations as local compilation. No qualified downloadable archive is available yet.

## License and third-party content

Sparkle-owned code uses the [MIT license](LICENSE.txt), including commercial use. Libraries, SDKs and assets keep their own licenses. See [Qt license/source instructions](CMake/Notices/Qt/README.md) and [redistribution requirements](Docs/Architecture/Modules/BuildAndPackaging/PackagingAndInstallation.md#reviewed-package-inputs), including unresolved asset rights in repository history.

No user support service or guaranteed fix is offered. Issues and pull requests are voluntary collaboration.
