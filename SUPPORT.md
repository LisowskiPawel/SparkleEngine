# Using Sparkle

[SparkleEngine](https://github.com/stoleckipawel/SparkleEngine#readme)

No user support service, response deadline or guaranteed fix is offered. Check [known issues](KNOWN_ISSUES.md) and [security](SECURITY.md).

The portable release is not ready. For now, follow the source-build instructions in the [repository README](https://github.com/stoleckipawel/SparkleEngine#build-and-launch). Package instructions below apply once a verified archive is available.

## Start, recover and exit

Start `Tools/SparkleLauncher/Binaries/ShippingEditor/SparkleLauncher.exe`, select Editor or Game, then select content. Empty is the initial selection. Downloads require internet access; offline use requires completed sync/cook.

In Game, Esc releases the mouse and opens the menu; F1 opens help. Hold right mouse to look; W/A/S/D or arrows move; E/Space and Q/C move up/down. Use the menu to select a level, reload the camera or Quit. Alt+F4 exits. Save Editor changes explicitly before exiting.

For startup failure, follow Launcher's error and next action. Check prerequisites, backend and selected content before retrying. After a crash, device error or hang, preserve the local log. If normal exit fails, close the process through Windows.

Packaged state is under `%LOCALAPPDATA%/SparkleEngine/Showcase/v0.1/`; source products use repository-specific development state. Exit all products and back up `Settings/EngineRendering.ini` before deleting that file to reset settings. Clear only the selected product's diagnostic files. Deleting the package directory preserves user state and selected content.

## Preview and export local diagnostics

Use Windows PowerShell 5.1, the retained ZIP, its independently recorded SHA-256 and an unchanged read-only extraction. The package helper is `Tools/Diagnostics/ExportDiagnostics.ps1`; its source is `CMake/Packaging/ExportDiagnostics.ps1`.

```powershell
$arguments = @{
    PackageRoot = 'C:/Sparkle'
    ArchivePath = 'C:/Downloads/Sparkle.zip'
    ArchiveSha256 = '<released archive SHA-256>'
    Component = 'Game'
    Incident = 'GpuDevice' # or Startup, Crash, HangTimeout
    LogPath = '<selected product log>' # optional
}
$preview = & C:/Sparkle/Tools/Diagnostics/ExportDiagnostics.ps1 @arguments | ConvertFrom-Json
$preview.Record | ConvertTo-Json -Depth 8
# Inspect the preview before exporting:
& C:/Sparkle/Tools/Diagnostics/ExportDiagnostics.ps1 @arguments -Consent `
    -PreviewSha256 $preview.PreviewSha256 -OutputDirectory 'C:/Users/Public/Documents'
```

Preview writes nothing. Export requires the matching preview digest and an existing directory outside the package. Changed previews require renewed consent. Nothing is uploaded.

The JSON contains product/build identity, incident category, error counts and diagnostic availability. It excludes log text, private paths, settings, content, dumps and captures; originals stay untouched. Logs over 10 MiB or with lines over 64 Ki characters are rejected. Missing/empty data is identified explicitly. Incident categories are user observations, not proof of collection.

Optional `-NativePdbPath` checks matching native symbols; mismatches reject export. Ordinary packages have no automatic crash/hang collection or qualified shader-symbol correlation. Review any files you voluntarily share; raw dumps/captures may contain private data.

The opt-in source-only GPU crash variant has its own [qualified candidate and launch/inspection route](https://github.com/stoleckipawel/SparkleEngine/blob/master/Docs/Architecture/CrossModule/PerformanceDiagnostics/GpuCrashDebugging/GpuCrashDebuggingDesign.md#launch-and-inspection-route). Ordinary packages do not include that variant. Its source export helper accepts `-GpuCrashManifestPath <incident.json>`; preview contains sanitized metadata. Raw dump, NVIDIA debug data, source and symbols each require category consent and the matching preview digest. Full symbols also require Source consent because they can embed source. Nothing is uploaded.

## Retention and replacement

Core logs rotate at 5 MiB with two backups per name, with up to one message of overshoot. Launcher logs, captures and exports remain until manually removed. Close products before clearing files.

Replacements use a new version and archive hash. Extract separately; keep the previous verified ZIP and back up settings. Side-by-side versions share `v0.1` settings. To roll back, re-extract the old ZIP and restore the settings backup if needed. No automatic updater or compatibility guarantee is provided. Stop using an archive identified as withdrawn.
