[CmdletBinding()]
param(
    [string] $BuildDirectory = '',
    [AllowEmptyString()][string] $InputsPath = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.IO.Compression.FileSystem
Add-Type -AssemblyName System.IO.Compression
. (Join-Path $PSScriptRoot 'PackageVerification.ps1')
. (Join-Path $PSScriptRoot 'PackageDependencies.ps1')
. (Join-Path $PSScriptRoot 'PackageInputs.ps1')
. (Join-Path $PSScriptRoot 'PackageOutputs.ps1')

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

. (Join-Path $PSScriptRoot '../SourceIdentity.ps1')

function Assert-ReleaseCandidateUnchanged
{
    $currentRevision = & git -C $repositoryRoot rev-parse HEAD
    if ($LASTEXITCODE -ne 0 -or $currentRevision.Trim() -cne $report.Revision -or
        (Get-ReleaseSourceHash (Get-ReleaseSourceInputs -SourceRoot $repositoryRoot)) -cne $report.SourceInputsSha256)
    {
        throw 'Source revision or bytes changed during packaging; prepare and verify the changed outputs before retrying.'
    }
}

function New-PackageStage
{
    param([string] $RepositoryRoot, [string] $Stage, $Files, $Manifest)
    [void] (New-Item -ItemType Directory -Path $Stage)
    foreach ($file in $Files)
    {
        $source = Assert-PackageFile $RepositoryRoot $file.Source $file.Size $file.Sha256
        $destination = Join-Path $Stage $file.Path
        [void] (New-Item -ItemType Directory -Force -Path (Split-Path $destination))
        [IO.File]::Copy($source, $destination, $false)
        [IO.File]::SetAttributes($destination, [IO.FileAttributes]::ReadOnly)
    }
    [void] (New-Item -ItemType Directory -Force -Path (Join-Path $Stage 'manifests'))
    $manifestPath = Join-Path $Stage 'manifests/sparkle-package-manifest.json'
    $Manifest | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath $manifestPath -Encoding UTF8
    [IO.File]::SetAttributes($manifestPath, [IO.FileAttributes]::ReadOnly)
}

function Write-PackageArchive
{
    param([string] $ArchivePath, [string] $Stage, $Manifest)
    $zip = [IO.Compression.ZipFile]::Open($ArchivePath, [IO.Compression.ZipArchiveMode]::Create)
    try
    {
        $paths = [string[]] (@($Manifest.Files.Path) + @('manifests/sparkle-package-manifest.json'))
        [Array]::Sort($paths, [StringComparer]::Ordinal)
        foreach ($path in $paths)
        {
            $entry = $zip.CreateEntry($path, [IO.Compression.CompressionLevel]::Optimal)
            $entry.LastWriteTime = [DateTimeOffset]::new(1980,1,1,0,0,0,[TimeSpan]::Zero)
            $entry.ExternalAttributes = 1
            $reader = [IO.File]::OpenRead((Join-Path $Stage $path))
            try
            {
                $writer = $entry.Open()
                try { $reader.CopyTo($writer) }
                finally { $writer.Dispose() }
            }
            finally { $reader.Dispose() }
        }
    }
    finally { $zip.Dispose() }
    [IO.File]::SetAttributes($ArchivePath, [IO.FileAttributes]::ReadOnly)
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
    $report.SourceInputsSha256 = Get-ReleaseSourceHash $sourceJson
    [IO.File]::WriteAllText((Join-Path $transactionRoot 'source-inputs.json'), $sourceJson, [Text.UTF8Encoding]::new($false))
    if ([string]::IsNullOrWhiteSpace($InputsPath) -or !(Test-Path -LiteralPath $InputsPath -PathType Leaf))
    {
        throw 'Missing reviewed release inputs. Prepare fresh products/content through Launcher, then supply their allowlist and final ShaderExport path. See Docs/Architecture/Modules/BuildAndPackaging/PackagingAndInstallation.md#reviewed-package-inputs.'
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
    Assert-ReleaseCandidateUnchanged
    $report.Outputs = Freeze-PackageOutputs $repositoryRoot $BuildDirectory $version $transactionRoot $products $inputs
    $manifest = New-PackageManifest $repositoryRoot $version $report.Revision $report.SourceInputsSha256 $products $inputs
    $report.PayloadSha256 = Get-ReleaseSourceHash ($manifest | ConvertTo-Json -Depth 20 -Compress)
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
    Assert-ReleaseCandidateUnchanged
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
