[CmdletBinding(DefaultParameterSetName = 'Package')]
param(
    [Parameter(Mandatory, ParameterSetName = 'Package')][string] $PackageRoot,
    [Parameter(Mandatory, ParameterSetName = 'Package')][string] $ArchivePath,
    [Parameter(Mandatory, ParameterSetName = 'Package')][string] $ArchiveSha256,
    [Parameter(Mandatory, ParameterSetName = 'Package')][ValidateSet('Launcher','Editor','Game')][string] $Component,
    [Parameter(Mandatory, ParameterSetName = 'Package')][ValidateSet('Startup','Crash','HangTimeout','GpuDevice')][string] $Incident,
    [string] $LogPath = '',
    [string] $NativePdbPath = '',
    [string] $OutputDirectory = '',
    [string] $PreviewSha256 = '',
    [Parameter(Mandatory, ParameterSetName = 'GpuCrash')][string] $GpuCrashManifestPath,
    [Parameter(ParameterSetName = 'GpuCrash')][switch] $IncludeGpuDump,
    [Parameter(ParameterSetName = 'GpuCrash')][switch] $IncludeNvdbg,
    [Parameter(ParameterSetName = 'GpuCrash')][switch] $IncludeSource,
    [Parameter(ParameterSetName = 'GpuCrash')][switch] $IncludeSymbols,
    [switch] $Consent
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'IncidentIdentity.ps1')

try
{
    if ($PSCmdlet.ParameterSetName -eq 'GpuCrash')
    {
        . (Join-Path $PSScriptRoot '../Diagnostics/GpuCrashIncident.ps1')
        $proposal = Get-GpuCrashExport $GpuCrashManifestPath $IncludeGpuDump.IsPresent $IncludeNvdbg.IsPresent $IncludeSource.IsPresent $IncludeSymbols.IsPresent
        $json = $proposal.Record | ConvertTo-Json -Depth 12
        $hash = [Security.Cryptography.SHA256]::Create()
        try { $digest = [BitConverter]::ToString($hash.ComputeHash([Text.Encoding]::UTF8.GetBytes($json))).Replace('-','').ToLowerInvariant() }
        finally { $hash.Dispose() }
        if (!$Consent)
        {
            [ordered]@{ PreviewSha256 = $digest; Warning = 'Raw dumps, NVIDIA debug data and full source/symbols may contain private content and paths. Binary redaction is not provided.'
                Record = $proposal.Record } | ConvertTo-Json -Depth 14
            exit 0
        }
        if ($PreviewSha256 -cne $digest) { throw 'Preview or selected categories changed; preview again.' }
        $output = [IO.Path]::GetFullPath($OutputDirectory).TrimEnd('\','/')
        $incidentRoot = [IO.Path]::GetDirectoryName([IO.Path]::GetFullPath($GpuCrashManifestPath))
        if ($output -eq $incidentRoot -or $output.StartsWith($incidentRoot + '\', [StringComparison]::OrdinalIgnoreCase)) { throw 'Export outside the incident.' }
        Write-GpuCrashExport $output $json $proposal.Copies
        exit 0
    }
    $manifest = Get-IncidentPackage $PackageRoot $ArchivePath $ArchiveSha256
    $product = @($manifest.Products | Where-Object Component -ceq $Component)
    if ($product.Count -ne 1) { throw 'Product identity is missing or ambiguous.' }
    $executable = @($manifest.Files | Where-Object Path -ceq $product[0].Path)
    if ($executable.Count -ne 1) { throw 'Product executable is not admitted.' }
    $findings = [ordered]@{ ErrorLines = 0; FatalLines = 0; DeviceRemovedLines = 0; VulkanDeviceLostLines = 0; DredLines = 0 }
    $logStatus = 'UNAVAILABLE'
    if ($LogPath -ne '')
    {
        [void] (Get-IncidentFile $LogPath 10485760)
        $stream = [IO.File]::Open($LogPath, [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::Read)
        $reader = [IO.StreamReader]::new($stream, [Text.UTF8Encoding]::new($false, $true), $true)
        try
        {
            while ($null -ne ($line = $reader.ReadLine()))
            {
                if ($line.Length -gt 65536) { throw 'Diagnostic line exceeds its character limit.' }
                if ($line -match '\[error\]') { $findings.ErrorLines++ }
                if ($line -match '\[critical\]|verification failed|fatal') { $findings.FatalLines++ }
                if ($line -match 'DXGI_ERROR_DEVICE_(REMOVED|HUNG|RESET)|0x887A000[567]') { $findings.DeviceRemovedLines++ }
                if ($line -match 'VK_ERROR_DEVICE_LOST') { $findings.VulkanDeviceLostLines++ }
                if ($line -match '\bDRED\b') { $findings.DredLines++ }
            }
        }
        finally { $reader.Dispose() }
        $logStatus = if ((Get-Item -LiteralPath $LogPath).Length -eq 0) { 'EMPTY' } else { 'PROJECTED' }
    }
    $symbols = [ordered]@{ Status = 'UNAVAILABLE'; BuildId = $null; PdbSha256 = $null }
    if ($NativePdbPath -ne '')
    {
        $peId = Get-NativeSymbolIdentity (Join-Path $PackageRoot $product[0].Path)
        $pdbId = Get-NativeSymbolIdentity $NativePdbPath
        if ($peId.Guid -cne $pdbId.Guid -or $peId.Age -ne $pdbId.Age) { throw 'Native symbols belong to another build.' }
        $symbols.Status = 'MATCHED'; $symbols.BuildId = $peId
        $symbols.PdbSha256 = (Get-FileHash -LiteralPath $NativePdbPath).Hash.ToLowerInvariant()
    }
    $record = [ordered]@{
        Version = $manifest.Version; Revision = $manifest.Revision; SourceInputsSha256 = $manifest.SourceInputsSha256
        ArchiveSha256 = $ArchiveSha256; Component = $Component; Configuration = $product[0].Configuration
        ExecutableSha256 = $executable[0].Sha256; Incident = $Incident; Observation = 'UserReported'
        Log = $logStatus; Findings = $findings; NativeSymbols = $symbols
        ShaderSymbols = 'UNAVAILABLE'; Dump = 'NOT_COLLECTED'; Capture = 'NOT_COLLECTED'
        CrashCollection = 'UNAVAILABLE'; HangCollection = 'UNAVAILABLE'; AutomaticUpload = $false
    }
    $json = $record | ConvertTo-Json -Depth 8
    $hash = [Security.Cryptography.SHA256]::Create()
    try { $digest = [BitConverter]::ToString($hash.ComputeHash([Text.Encoding]::UTF8.GetBytes($json))).Replace('-','').ToLowerInvariant() }
    finally { $hash.Dispose() }
    if (!$Consent)
    {
        [ordered]@{ PreviewSha256 = $digest; Record = $record } | ConvertTo-Json -Depth 10
        exit 0
    }
    if ($PreviewSha256 -cne $digest) { throw 'Preview changed or consent has no matching preview digest; preview again.' }
    $output = [IO.Path]::GetFullPath($OutputDirectory).TrimEnd('\','/')
    $root = [IO.Path]::GetFullPath($PackageRoot).TrimEnd('\','/')
    if ($output -eq $root -or $output.StartsWith($root + '\', [StringComparison]::OrdinalIgnoreCase)) { throw 'Export outside the immutable package.' }
    # A second check detects changes made while the proposal was collected.
    [void] (Get-IncidentPackage $PackageRoot $ArchivePath $ArchiveSha256)
    Write-IncidentRecord $OutputDirectory $json 'sparkle-diagnostic'
}
catch
{
    # Do not echo filesystem exceptions: they may disclose private input paths.
    [Console]::Error.WriteLine('Diagnostic export rejected. Check identities, file bounds, selected categories and preview consent. Originals were not changed.')
    exit 1
}
