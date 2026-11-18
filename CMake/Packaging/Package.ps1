[CmdletBinding()]
param(
    [string] $BuildDirectory = '',
    [AllowEmptyString()][string] $InputsPath = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot '../SourceIdentity.ps1')
. (Join-Path $PSScriptRoot 'Payload/PackageInputs.ps1')
. (Join-Path $PSScriptRoot 'Payload/PackageOutputs.ps1')
. (Join-Path $PSScriptRoot 'Payload/PackageDependencies.ps1')
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
$transactionRoot = Join-Path $outputRoot ('.partial-' + [Guid]::NewGuid().ToString('N'))
$lock = $null

function Assert-ReleaseCandidateUnchanged
{
    param([string] $RepositoryRoot, [string] $Revision, [string] $SourceInputsSha256)

    $currentRevision = & git -C $RepositoryRoot rev-parse HEAD
    if ($LASTEXITCODE -ne 0 -or $currentRevision.Trim() -cne $Revision -or
        (Get-TextSha256 (Get-ReleaseSourceInputs -SourceRoot $RepositoryRoot)) -cne $SourceInputsSha256)
    {
        throw 'Source revision or bytes changed during packaging; prepare the changed outputs before retrying.'
    }
}

try
{
    if ($BuildDirectory -ne (Join-Path $repositoryRoot 'build')) { throw 'Release packaging requires the canonical build directory.' }
    $outputRoot = Resolve-LocalArtifactPath $outputRoot
    [void] [IO.Directory]::CreateDirectory($outputRoot)
    $lock = [IO.File]::Open((Join-Path $outputRoot 'package.lock'), [IO.FileMode]::OpenOrCreate, [IO.FileAccess]::ReadWrite, [IO.FileShare]::None)
    [void] (New-Item -ItemType Directory -Path $transactionRoot)
    $revision = & git -C $repositoryRoot rev-parse HEAD
    if ($LASTEXITCODE -ne 0) { throw 'Cannot identify source revision.' }
    $revision = $revision.Trim()
    $sourceInputsSha256 = Get-TextSha256 (Get-ReleaseSourceInputs -SourceRoot $repositoryRoot)
    if ([string]::IsNullOrWhiteSpace($InputsPath))
    {
        throw 'Missing reviewed release inputs. Prepare products/content through Launcher, then supply their inventory and final ShaderExport path. See Docs/Engineering.md#build-and-delivery.'
    }
    $inputs = Get-Content -LiteralPath $InputsPath -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($inputs.Revision -cne $revision -or $inputs.SourceInputsSha256 -cne $sourceInputsSha256)
    {
        throw 'Release inputs do not match the current source candidate.'
    }

    $products = @(Get-PackageProducts $BuildDirectory $version)
    Assert-PackageOutputs $repositoryRoot $products $inputs
    $manifest = New-PackageManifest $repositoryRoot $version $revision $sourceInputsSha256 $products $inputs
    $stage = Join-Path $transactionRoot 'stage'
    New-PackageStage $repositoryRoot $stage $inputs.Files $manifest
    Test-PackageStage $stage $manifest
    Test-PackageDependencyClosure $stage $manifest
    Test-PackageMetadata $stage $manifest

    $partial = Join-Path $transactionRoot 'archive.partial'
    Write-PackageArchive $partial $stage $manifest
    Test-PackageArchive $partial $stage $manifest
    Assert-ReleaseCandidateUnchanged $repositoryRoot $revision $sourceInputsSha256
    $archiveHash = (Get-FileHash -LiteralPath $partial).Hash.ToLowerInvariant()
    $name = "SparkleEngine-v$version-windows-x64-unsigned-internal.zip"
    [IO.File]::Move($partial, (Join-Path $transactionRoot $name))
    $destination = Join-Path $outputRoot $archiveHash
    [IO.Directory]::Move($transactionRoot, $destination)
    Write-Output "Package: $(Join-Path $destination $name)"
    Write-Output "SHA256: $archiveHash"
}
catch
{
    [Console]::Error.WriteLine("Packaging failed: $($_.Exception.Message) Transaction: $transactionRoot")
    exit 1
}
finally
{
    if ($null -ne $lock) { $lock.Dispose() }
}
