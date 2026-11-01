# Using Sparkle

[SparkleEngine](https://github.com/stoleckipawel/SparkleEngine#readme)

No user support service, response deadline or guaranteed fix is offered. Check [known issues](KNOWN_ISSUES.md) and [security](SECURITY.md).

**Approved public workflow target:** a fresh precompiled/cooked release base plus optional source modification/rebuild through Launcher. Current source uses one binary/cooked output layout; a public archive is not yet qualified. Earlier internal archives retain their recorded behavior. [Delivery contract and remaining changes](https://github.com/stoleckipawel/SparkleEngine/blob/master/Docs/Architecture/Modules/BuildAndPackaging/PackagingAndInstallation.md#prebuilt-base-and-source-rebuild). Source-only build commands remain in the [README](https://github.com/stoleckipawel/SparkleEngine#build-and-launch).

## Start, recover and exit

For the target release, extract the ready-to-run consumer bundle and open `LaunchSparkle.cmd` (the executable is `artifacts/dev/Tools/SparkleLauncher/Binaries/ShippingEditor/SparkleLauncher.exe`). Choose a supplied scene and Run Game, or open Editor for inspection. Empty remains the initial selection. Ordinary Run is offline; publisher supplies the cooked content, runtime dependencies and defaults. No source acquisition, SDK installation, manual content assembly, build, cook or old content store is required.

The approved Game target has no ImGui menu, HUD, help or settings overlay. Hold right mouse to look; W/A/S/D or arrows move; E/Space and Q/C move up/down. Release mouse-look before changing focus; close the window or use Alt+F4 to exit. Choose scenes/API in Launcher; inspect/reset settings and cameras in Editor. Save Editor edits explicitly. Current source removes the Game UI; earlier internal archives retain their recorded menu behavior. New-archive runtime qualification remains open.

For startup failure, follow Launcher's error and next action. Check prerequisites, backend and selected content before retrying. After a crash, device error or hang, preserve the local log. If normal exit fails, close the process through Windows.

Both source and extracted products use root-keyed state under `%LOCALAPPDATA%/SparkleEngine/Development/<workspace-key>/Showcase/`; there is no release-version state lookup or migration. Earlier internal archives retain their recorded paths. Exit all products and back up `Settings/EngineRendering.ini` before deleting that file to reset settings. Clear only the selected product's diagnostic files. Removing an extraction preserves per-user settings/authored data; old generated content must not be copied or migrated into a new release.

## Modify source and rebuild

Obtain the source tag matching the release. Optionally initialize its normal `artifacts/dev` binary and cooked outputs from that release's base, then open Launcher for the source workspace. Edit code, shaders or assets and use the existing Build/Cook/Run actions. Rebuilt output replaces that starting base in the same paths. A source-only start remains supported; source rebuilds need the documented public prerequisites. Ordinary prebuilt Run does not.

`SyncProducts.ps1` imports matching binaries and the complete cooked base into canonical source-workspace outputs. Local Build/Cook overwrites those files; Run retains no origin marker or per-file version database. Source acquisition, Build and Cook are explicit optional Launcher actions. Editor can reload existing shaders after Launcher Cook; it does not invoke a compiler or watch source. Supplied level documents can run without acquisition/cook-selection metadata; missing cooked dependencies fail at their loader. No automatic update feed is advertised.

## Preview and export local diagnostics

Use Launcher's actionable failure and local product/operation logs first. Keep the actual build identity and identify missing/unavailable evidence honestly. Diagnostic inspection must remain usable when the ZIP is gone or the installation is incomplete; it is not full package requalification.

Review any files before voluntarily sharing them. Raw logs, dumps, captures, source and symbols may contain private paths/content. Nothing uploads automatically. Optional symbols must match before being called correlated; unavailable collection is stated explicitly. No diagnostic helper or support account is required for ordinary Run.

The generic archive-dependent exporter is removed. The source-only GPU crash tools retain the separate [qualified launch/inspection and consent contract](https://github.com/stoleckipawel/SparkleEngine/blob/master/Docs/Architecture/CrossModule/PerformanceDiagnostics/GpuCrashDebugging/GpuCrashDebuggingDesign.md#launch-and-inspection-route). Ordinary packages do not promise automatic CPU crash/hang collection or device recovery.

## Retention and replacement

Core logs rotate at 5 MiB with two backups per name, with up to one message of overshoot. Launcher logs, captures and exports remain until manually removed. Close products before clearing files.

Each published version has fresh build/cooked outputs and a new immutable archive identity. Extract it separately and use its own supplied content; use its own generated files without importing old content state. A new release leaves old files alone. Back up authored source and settings before deliberate cleanup; ordinary settings Save/Reset and manual re-extraction are sufficient recovery.

Rollback means running the previously verified extraction or re-extracting its archive, with the documented settings backup/reset policy. There is no generated-content migration, automatic updater, local-file version tracker or compatibility guarantee. Stop using an archive identified as withdrawn.
