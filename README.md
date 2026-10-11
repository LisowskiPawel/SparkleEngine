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

This builds Launcher; use it to configure/build the engine, select content, cook and start Editor or Game. Start with Empty. Only selected assets download; offline use requires completed sync/cook. See [Launcher workflows](Docs/Architecture/Modules/Tools/Launcher/LauncherCapabilities.md).

With an archive for your source revision, use the [precompiled-binary route](Docs/Architecture/Modules/BuildAndPackaging/PackagingAndInstallation.md#work-with-precompiled-binaries). It writes the same binary locations as local compilation. No qualified downloadable archive is available yet.

## License and third-party content

Sparkle-owned code uses the [MIT license](LICENSE.txt), including commercial use. Libraries, SDKs and assets keep their own licenses. See [Qt license/source instructions](CMake/Notices/Qt/README.md) and [redistribution requirements](Docs/Architecture/Modules/BuildAndPackaging/PackagingAndInstallation.md#reviewed-package-inputs), including unresolved asset rights in repository history.

No user support service or guaranteed fix is offered. Issues and pull requests are voluntary collaboration.
