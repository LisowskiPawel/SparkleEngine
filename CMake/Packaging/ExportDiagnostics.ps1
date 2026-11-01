[CmdletBinding()]
param(
    [Parameter(Mandatory)][string] $GpuCrashManifestPath,
    [switch] $IncludeGpuDump,
    [switch] $IncludeNvdbg,
    [switch] $IncludeSource,
    [switch] $IncludeSymbols,
    [string] $OutputDirectory = '',
    [string] $PreviewSha256 = '',
    [switch] $Consent
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot '../Diagnostics/GpuCrashIncident.ps1')

try
{
    $proposal = Get-GpuCrashExport $GpuCrashManifestPath $IncludeGpuDump.IsPresent $IncludeNvdbg.IsPresent $IncludeSource.IsPresent $IncludeSymbols.IsPresent
    $json = $proposal.Record | ConvertTo-Json -Depth 12
    $digest = Get-IncidentPreviewSha256 $json
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
}
catch
{
    # Do not echo filesystem exceptions: they may disclose private input paths.
    [Console]::Error.WriteLine('Diagnostic export rejected. Check identities, file bounds, selected categories and preview consent. Originals were not changed.')
    exit 1
}
