# SparkleEngine

A Windows graphics learning project with **Sparkle Launcher, Sparkle Editor and Sparkle Game**. Development is public in this repository; the first packaged release is still being qualified.

## Start here

- [Engine at a glance](Docs/Architecture/EngineAtAGlance.md): features and current limits.
- [Release scope](Docs/Acceptance/ReleaseScope/README.md): approved products, maps and experience.
- [Release evidence](Docs/Acceptance/FirstRelease.md): what has passed and what remains blocked.
- [Contributing](CONTRIBUTING.md): changes, review and verification.
- [Documentation](Docs/README.md): architecture and engineering owners.

## Build and launch

Use a short Windows checkout path, such as `C:/src/SparkleEngine`. The recorded baseline uses Windows 11, Visual Studio 2026 C++ tools, CMake 4.3.3, Qt 6.11.1 MSVC x64 and Vulkan SDK 1.4.350.0. These are a tested environment snapshot, not proof that every other tool version works. See [exact prerequisites and baseline commands](Docs/Acceptance/ReleaseScope/Evidence.md#fr-01-closure-work-iter-rel-01-07).

Clone [SparkleEngine](https://github.com/stoleckipawel/SparkleEngine) and run these commands from its root in PowerShell, adjusting the Qt path. This standalone Launcher entry acquires only its own source dependencies; the engine workspace and Vulkan/shader tools are handled separately through Launcher:

```powershell
cmake -S Tools/Launcher -B build/private/tools/SparkleLauncher -G "Visual Studio 18 2026" -A x64 -DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/msvc2022_64
cmake --build build/private/tools/SparkleLauncher --config DevelopmentEditor --target SparkleLauncher --parallel 4
& ./artifacts/dev/launcher/DevelopmentEditor/SparkleLauncher.exe
```

Use Launcher to build, select content, cook and start Editor or Game. Initial content selection is Empty; publisher assets download only when selected. Acquisition needs internet access; offline use follows successful sync/cook. [Launcher](Docs/Architecture/Modules/Tools/Launcher/README.md) owns the workflow. A source build is not an accepted consumer package.

## License and third-party content

Sparkle-owned code is under the [MIT license](LICENSE.txt), including permission for commercial use. The project's educational purpose adds no noncommercial restriction. Launcher uses dynamically linked Qt; its [license and source instructions](CMake/Notices/Qt/README.md) describe library replacement and corresponding source. Dependencies, SDK binaries, fonts and sample assets retain their own licenses; the root license does not relicense them. See [release inputs and redistribution obligations](Docs/Architecture/Modules/BuildAndPackaging/ReleaseInputs.md). Public Git history and source archives are distinct distribution surfaces; archive exclusions do not remove historical files.

The project offers no user support service, response deadline or guaranteed fix. Public issues and pull requests are voluntary collaboration, not a support contract. Share only diagnostics you have reviewed for private data.
