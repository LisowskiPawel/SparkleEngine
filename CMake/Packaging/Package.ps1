[CmdletBinding()]
param(
    [string] $BuildDirectory = '',
    [AllowEmptyString()][string] $InputsPath = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.IO.Compression.FileSystem
Add-Type -AssemblyName System.IO.Compression
. (Join-Path $PSScriptRoot '../SourceIdentity.ps1')
. (Join-Path $PSScriptRoot 'Payload/PackageInputs.ps1')
. (Join-Path $PSScriptRoot 'Payload/PackageOutputs.ps1')
. (Join-Path $PSScriptRoot 'Payload/PackageMetadata.ps1')
. (Join-Path $PSScriptRoot 'Publication/PackageStage.ps1')
. (Join-Path $PSScriptRoot 'Publication/PackageArchive.ps1')

$repositoryRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '../..')).Path
if ($BuildDirectory -eq '') { $BuildDirectory = Join-Path $repositoryRoot 'build' }
$BuildDirectory = [IO.Path]::GetFullPath($BuildDirectory)
$versionOwner = Get-Content -LiteralPath (Join-Path $repositoryRoot 'CMake/SparkleProductIdentity.cmake') -Raw
if ($versionOwner -notmatch 'set\(SPARKLE_VERSION "([0-9.]+)"\)') { throw 'Release identity owner has no version.' }
$version = $Matches[1]
$outputRoot = Join-Path $repositoryRoot "artifacts/releases/$version"
$transaction = [Guid]::NewGuid().ToString('N')
$transactionRoot = Join-Path $outputRoot ".partial-$transaction"
$report = [ordered]@{
    Version = $version; Revision = ''; SourceInputsSha256 = ''; Signing = 'UnsignedInternal'; Status = 'BLOCKED'; Steps = @(); Archive = $null
    Environment = @{ OperatingSystem = [Environment]::OSVersion.VersionString; PowerShell = $PSVersionTable.PSVersion.ToString() }
    Configurations = @('ShippingEditor', 'ShippingGame'); ReleaseInputs = $null; PayloadSha256 = $null; Outputs = $null
}
$phase = 'Inputs'
$lock = $null
$reportPath = Join-Path $transactionRoot 'report.json'

function Assert-ReleaseCandidateUnchanged
{
    param([string] $RepositoryRoot, $Report)

    $currentRevision = & git -C $repositoryRoot rev-parse HEAD
    if ($LASTEXITCODE -ne 0 -or $currentRevision.Trim() -cne $report.Revision -or
        (Get-TextSha256 (Get-ReleaseSourceInputs -SourceRoot $repositoryRoot)) -cne $report.SourceInputsSha256)
    {
        throw 'Source revision or bytes changed during packaging; prepare and verify the changed outputs before retrying.'
    }
}

function Publish-Package
{
    param([string] $Partial, [string] $TransactionRoot, [string] $OutputRoot, [string] $Version, [string] $ArchiveHash, $Report)
    $name = "SparkleEngine-v$Version-windows-x64-unsigned-internal.zip"
    $destination = Join-Path $OutputRoot $ArchiveHash
    $Report.Archive = @{ Path = $name; Sha256 = $ArchiveHash; Size = (Get-Item -LiteralPath $Partial).Length }
    [IO.File]::Move($Partial, (Join-Path $TransactionRoot $Report.Archive.Path))
    $Report.Steps += @{ Name = 'Package'; Status = 'PASS' }
    $Report.Status = 'PASS'
    $Report | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath (Join-Path $TransactionRoot 'report.json') -Encoding UTF8
    [IO.Directory]::Move($TransactionRoot, $destination)
    return Join-Path $destination 'report.json'
}

try
{
    # This entry owns only the canonical workspace; it never deletes an earlier stage/archive.
    if ($BuildDirectory -ne (Join-Path $repositoryRoot 'build')) { throw 'Release packaging requires the canonical build directory.' }
    $current = Get-Item -LiteralPath $repositoryRoot -Force
    foreach ($part in @('artifacts','releases',$version))
    {
        if (($current.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw 'Reparse release output root.' }
        $path = Join-Path $current.FullName $part
        if (!(Test-Path -LiteralPath $path)) { [void] (New-Item -ItemType Directory -Path $path) }
        $current = Get-Item -LiteralPath $path -Force
    }
    if (($current.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw 'Reparse release output root.' }
    $lock = [IO.File]::Open((Join-Path $outputRoot 'package.lock'), [IO.FileMode]::OpenOrCreate, [IO.FileAccess]::ReadWrite, [IO.FileShare]::None)
    [void] (New-Item -ItemType Directory -Path $transactionRoot)
    $revision = & git -C $repositoryRoot rev-parse HEAD
    if ($LASTEXITCODE -ne 0) { throw 'Cannot identify source revision.' }
    $report.Revision = $revision.Trim()
    $sourceJson = Get-ReleaseSourceInputs -SourceRoot $repositoryRoot
    $report.SourceInputsSha256 = Get-TextSha256 $sourceJson
    [IO.File]::WriteAllText((Join-Path $transactionRoot 'source-inputs.json'), $sourceJson, [Text.UTF8Encoding]::new($false))
    if ([string]::IsNullOrWhiteSpace($InputsPath) -or !(Test-Path -LiteralPath $InputsPath -PathType Leaf))
    {
        throw 'Missing reviewed release inputs. Prepare fresh products/content through Launcher, then supply their allowlist and final ShaderExport path. See Docs/Engineering.md#build-and-delivery.'
    }
    $inputSnapshot = Join-Path $transactionRoot 'release-inputs.json'
    [IO.File]::Copy([IO.Path]::GetFullPath($InputsPath), $inputSnapshot, $false)
    [IO.File]::SetAttributes($inputSnapshot, [IO.FileAttributes]::ReadOnly)
    $report.ReleaseInputs = @{ Size = (Get-Item -LiteralPath $inputSnapshot).Length; Sha256 = (Get-FileHash -LiteralPath $inputSnapshot).Hash.ToLowerInvariant() }
    $inputs = Get-Content -LiteralPath $inputSnapshot -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($inputs.Revision -cne $report.Revision -or $inputs.SourceInputsSha256 -cne $report.SourceInputsSha256)
    {
        throw 'Release inputs do not match the exact current source candidate.'
    }
    $products = @(Get-PackageProducts $BuildDirectory $version)
    $report.Steps += @{ Name = 'Inputs'; Status = 'PASS' }
    $phase = 'Freeze'
    Assert-ReleaseCandidateUnchanged $repositoryRoot $report
    $report.Outputs = Freeze-PackageOutputs $repositoryRoot $transactionRoot $products $inputs
    $manifest = New-PackageManifest $repositoryRoot $version $report.Revision $report.SourceInputsSha256 $products $inputs
    $report.PayloadSha256 = Get-TextSha256 ($manifest | ConvertTo-Json -Depth 20 -Compress)
    $inputs | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath (Join-Path $transactionRoot 'frozen-inputs.json') -Encoding UTF8
    $report.Steps += @{ Name = 'Freeze'; Status = 'PASS' }
    $phase = 'Stage'
    $stage = Join-Path $transactionRoot 'stage'
    New-PackageStage $repositoryRoot $stage $inputs.Files $manifest
    $report.Steps += @{ Name = 'Stage'; Status = 'PASS' }
    $phase = 'Verify'
    Test-PackageStage $stage $manifest
    Test-PackageDependencyClosure $stage $manifest
    Test-PackageMetadata $stage $manifest
    $report.Steps += @{ Name = 'Verify'; Status = 'PASS' }

    $phase = 'Package'
    $partial = Join-Path $transactionRoot 'archive.partial'
    Write-PackageArchive $partial $stage $manifest
    Test-PackageArchive $partial $stage $manifest
    Assert-ReleaseCandidateUnchanged $repositoryRoot $report
    $archiveHash = (Get-FileHash -LiteralPath $partial).Hash.ToLowerInvariant()
    $reportPath = Publish-Package $partial $transactionRoot $outputRoot $version $archiveHash $report
}
catch
{
    $report.Status = 'BLOCKED'
    $report.Archive = $null
    $report.Steps = @($report.Steps | Where-Object { $_.Name -ne $phase -or $_.Status -ne 'PASS' })
    $report.Steps += @{ Name = $phase; Status = 'BLOCKED'; Reason = $_.Exception.Message }
    foreach ($skipped in @('Freeze','Stage','Verify','Package'))
    {
        if ($skipped -ne $phase -and !@($report.Steps | Where-Object Name -eq $skipped).Count)
        {
            $report.Steps += @{ Name = $skipped; Status = 'SKIPPED'; Reason = "Predecessor $phase did not pass." }
        }
    }
}
finally
{
    # Failed transactions are retained for diagnosis under .partial; no ZIP is published from them.
    try
    {
        if ($report.Status -ne 'PASS' -and (Test-Path -LiteralPath $transactionRoot -PathType Container))
        {
            $report | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath $reportPath -Encoding UTF8
        }
    }
    finally { if ($null -ne $lock) { $lock.Dispose() } }
}
Write-Output "Release report: $reportPath"
if ($report.Status -ne 'PASS')
{
    [Console]::Error.WriteLine(($report.Steps | Where-Object Status -eq 'BLOCKED' | Select-Object -Last 1).Reason)
    exit 1
}
