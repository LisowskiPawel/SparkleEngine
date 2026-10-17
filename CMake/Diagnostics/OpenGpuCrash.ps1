[CmdletBinding()]
param(
    [Parameter(Mandatory)][string] $ManifestPath,
    [Parameter(Mandatory)][string] $ExpectedSessionId,
    [Parameter(Mandatory)][uint32] $ExpectedChildPid,
    [Parameter(Mandatory)][string] $InspectorPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
[Console]::OutputEncoding = [Text.UTF8Encoding]::new($false)
. (Join-Path $PSScriptRoot 'GpuCrashIncident.ps1')
try
{
    $incident = Get-GpuCrashIncident $ManifestPath $ExpectedSessionId $ExpectedChildPid
    if ($incident.state -cnotin @('Collected','Partial','Timed out') -or @($incident.files | Where-Object path -ceq 'crash.nv-gpudmp').Count -ne 1)
    {
        throw 'No terminal incident with a verified dump is available.'
    }
    [void] (Get-GpuCrashBundle $incident)
    $tool = Get-IncidentFile $InspectorPath ([long]::MaxValue)
    if ($tool.Extension -ine '.exe') { throw 'Configure an inspector executable explicitly.' }
    $directory = [IO.Path]::GetDirectoryName([IO.Path]::GetFullPath($ManifestPath))
    Write-Output ('Open manually in the inspector: ' + (Join-Path $directory 'crash.nv-gpudmp'))
    Write-Output ('NVIDIA debug-info search path: ' + $directory)
    Write-Output ('Shader binaries/debug/source search root (include its job subdirectories): ' + [IO.Path]::GetDirectoryName($incident.candidate.shaderExportPath))
    Write-Output ('CPU symbols: ' + $incident.candidate.cpuPdbPath)
    Write-Output 'Use only these matched paths and isolated caches. Collection does not qualify source mapping.'
    Start-Process -FilePath $tool.FullName -WorkingDirectory $tool.DirectoryName -WindowStyle Normal
}
catch
{
    [Console]::Error.WriteLine('Inspector handoff rejected: receipt, identity, terminal dump or configured inspector is unavailable. Incident retained.')
    exit 1
}
