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
    Configurations = @('ShippingEditor', 'ShippingGame'); ReleaseInputs = $null; BuildCache = $null
}
$phase = 'Admission'
$lock = $null

function Invoke-ReleaseCommand
{
    param([string] $Executable, [string[]] $Arguments, [string] $LogName)
    $log = Join-Path $transactionRoot $LogName
    & $Executable @Arguments *> $log
    $code = $LASTEXITCODE
    $report.Steps += @{ Name = $phase; Command = @($Executable) + $Arguments; ExitCode = $code; Log = $LogName; Status = $(if ($code -eq 0) { 'PASS' } else { 'BLOCKED' }) }
    if ($code -ne 0) { throw "$phase failed with exit $code; see '$log'." }
}

function Get-ReleaseSourceInputs
{
    $encoding = [Console]::OutputEncoding
    try
    {
        [Console]::OutputEncoding = [Text.UTF8Encoding]::new($false)
        $paths = @(& git -c core.quotepath=false -C $repositoryRoot ls-files --cached --others --exclude-standard)
    }
    finally { [Console]::OutputEncoding = $encoding }
    if ($LASTEXITCODE -ne 0) { throw 'Cannot inventory source inputs.' }
    $paths = [string[]] @($paths | Select-Object -Unique)
    [Array]::Sort($paths, [StringComparer]::Ordinal)
    $sourceInputs = @(foreach ($path in $paths)
    {
        $source = Join-Path $repositoryRoot $path
        if ($path.StartsWith('"')) { throw 'Cannot bind an escaped Git source path; source inventory is unavailable.' }
        [ordered]@{ Path = $path; Sha256 = $(if (Test-Path -LiteralPath $source -PathType Leaf) { (Get-FileHash -LiteralPath $source).Hash.ToLowerInvariant() } else { $null }) }
    })
    return ConvertTo-Json -InputObject $sourceInputs -Depth 5 -Compress
}

function Get-ReleaseSourceHash
{
    param([string] $Json)
    $hash = [Security.Cryptography.SHA256]::Create()
    try { return [BitConverter]::ToString($hash.ComputeHash([Text.Encoding]::UTF8.GetBytes($Json))).Replace('-','').ToLowerInvariant() }
    finally { $hash.Dispose() }
}

function Assert-ReleaseCandidateUnchanged
{
    $currentRevision = & git -C $repositoryRoot rev-parse HEAD
    if ($LASTEXITCODE -ne 0 -or $currentRevision.Trim() -cne $report.Revision -or
        (Get-ReleaseSourceHash (Get-ReleaseSourceInputs)) -cne $report.SourceInputsSha256)
    {
        throw 'Source revision or bytes changed during packaging; regenerate candidate admissions.'
    }
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
    $sourceJson = Get-ReleaseSourceInputs
    $report.SourceInputsSha256 = Get-ReleaseSourceHash $sourceJson
    [IO.File]::WriteAllText((Join-Path $transactionRoot 'source-inputs.json'), $sourceJson, [Text.UTF8Encoding]::new($false))
    $cache = Join-Path $BuildDirectory 'CMakeCache.txt'
    if (Test-Path -LiteralPath $cache -PathType Leaf)
    {
        $report.BuildCache = @{ Size = (Get-Item -LiteralPath $cache).Length; Sha256 = (Get-FileHash -LiteralPath $cache).Hash.ToLowerInvariant() }
    }

    if ([string]::IsNullOrWhiteSpace($InputsPath) -or !(Test-Path -LiteralPath $InputsPath -PathType Leaf))
    {
        throw 'Missing reviewed release inputs. Rights, exact helper/DLL closure and symbol correlation must be admitted before build/cook/stage. See Docs/Architecture/Modules/BuildAndPackaging/PackagingAndInstallation.md#reviewed-package-inputs.'
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
    foreach ($name in @('Rights','DependencyClosure','SymbolCorrelation'))
    {
        $admission = $inputs.Admissions.$name
        $receiptPath = Assert-PackageFile $repositoryRoot $admission.Report $admission.Size $admission.Sha256
        $receipt = Get-Content -LiteralPath $receiptPath -Raw -Encoding UTF8 | ConvertFrom-Json
        if ($receipt.Status -cne 'PASS' -or $receipt.Gate -cne $name -or $receipt.Revision -cne $report.Revision -or
            $receipt.SourceInputsSha256 -cne $report.SourceInputsSha256) { throw "Unresolved or stale $name admission." }
    }
    $products = @()
    foreach ($configuration in @('ShippingEditor','ShippingGame'))
    {
        $declaration = Get-Content -LiteralPath (Join-Path $BuildDirectory "private/release-products/$configuration.json") -Raw | ConvertFrom-Json
        if ($declaration.Version -cne $version) { throw 'Configured release identity is stale.' }
        $products += @($declaration.Products | Where-Object Configuration -eq $configuration)
    }
    $manifest = [ordered]@{
        Version = $version; Revision = $report.Revision; SourceInputsSha256 = $report.SourceInputsSha256
        Signing = 'UnsignedInternal'; ProjectPath = 'content/Showcase'; EnginePath = 'engine'; ContentStore = 'Content/Showcase'
        Products = @($products | Select-Object Component, Target, Configuration, Path)
        SystemImports = @($inputs.SystemImports)
        Files = @($inputs.Files | Select-Object Path, Size, Sha256, Component, Role, Permission, Owner, Origin, License, Notice, Signature)
    }
    Assert-PackageManifest $manifest
    Assert-PackageLayout $manifest
    foreach ($product in $products)
    {
        $member = @($inputs.Files | Where-Object Path -eq $product.Path)
        if ($member.Count -ne 1 -or $member[0].Component -cne $product.Component -or
            [IO.Path]::GetFullPath((Join-Path $repositoryRoot $member[0].Source)) -ne $product.Source -or
            $product.Source -eq '') { throw "Missing/incompatible product admission: '$($product.Component)'." }
    }
    foreach ($path in @('tools/CMake/bin/cmake.exe','manifests/sparkle-sbom.spdx.json','manifests/sparkle-provenance.json',
        'tools/Diagnostics/ExportDiagnostics.ps1','tools/Diagnostics/IncidentIdentity.ps1','tools/Diagnostics/PackageVerification.ps1',
        'docs/SUPPORT.md','docs/SECURITY.md','docs/KNOWN_ISSUES.md'))
    {
        if (@($inputs.Files | Where-Object Path -eq $path).Count -ne 1) { throw "Missing required input: '$path'." }
    }
    foreach ($file in $inputs.Files)
    {
        [void] (Assert-PackageFile $repositoryRoot $file.Source $file.Size $file.Sha256)
    }
    foreach ($binding in @(
        @('tools/Diagnostics/ExportDiagnostics.ps1','CMake/Packaging/ExportDiagnostics.ps1'),
        @('tools/Diagnostics/IncidentIdentity.ps1','CMake/Packaging/IncidentIdentity.ps1'),
        @('tools/Diagnostics/PackageVerification.ps1','CMake/Packaging/PackageVerification.ps1'),
        @('docs/SUPPORT.md','SUPPORT.md'), @('docs/SECURITY.md','SECURITY.md'), @('docs/KNOWN_ISSUES.md','KNOWN_ISSUES.md')))
    {
        $file = @($inputs.Files | Where-Object Path -ceq $binding[0])[0]
        if ($file.Source -cne $binding[1] -or $file.Component -cne 'Shared' -or
            $file.Role -cne $(if ($binding[0].EndsWith('.ps1')) { 'Helper' } else { 'Metadata' }))
        {
            throw 'Diagnostic helper/public policy admission differs from its source owner.'
        }
    }
    $report.Steps += @{ Name = 'Admission'; Status = 'PASS' }

    $phase = 'Build'
    $cmake = (Get-Command cmake -ErrorAction Stop).Source
    $cmakeVersion = & $cmake --version
    if ($LASTEXITCODE -ne 0 -or $cmakeVersion[0] -cne 'cmake version 4.3.3') { throw 'Release build requires the qualified CMake 4.3.3.' }
    foreach ($configuration in @('ShippingEditor','ShippingGame'))
    {
        $targets = @($products | Where-Object Configuration -eq $configuration | Select-Object -ExpandProperty Target)
        Invoke-ReleaseCommand $cmake (@('--build',$BuildDirectory,'--config',$configuration,'--target') + $targets) "build-$configuration.log"
    }
    $phase = 'Cook'
    $cooker = ($products | Where-Object Component -eq 'AssetCooker').Source
    foreach ($configuration in @('ShippingEditor','ShippingGame'))
    {
        Invoke-ReleaseCommand $cooker @('cook-project','Showcase',$configuration,'--tool-profile','ShippingEditor','--root',$repositoryRoot) "cook-$configuration.log"
    }
    $phase = 'Stage'
    Assert-ReleaseCandidateUnchanged
    $stage = Join-Path $transactionRoot 'stage'
    [void] (New-Item -ItemType Directory -Path $stage)
    foreach ($file in $inputs.Files)
    {
        $source = Assert-PackageFile $repositoryRoot $file.Source $file.Size $file.Sha256
        $destination = Join-Path $stage $file.Path
        [void] (New-Item -ItemType Directory -Force -Path (Split-Path $destination))
        [IO.File]::Copy($source, $destination, $false)
        [IO.File]::SetAttributes($destination, [IO.FileAttributes]::ReadOnly)
    }
    [void] (New-Item -ItemType Directory -Force -Path (Join-Path $stage 'manifests'))
    $manifestPath = Join-Path $stage 'manifests/sparkle-package-manifest.json'
    $manifest | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath $manifestPath -Encoding UTF8
    [IO.File]::SetAttributes($manifestPath, [IO.FileAttributes]::ReadOnly)
    $phase = 'Verify'
    Test-PackageStage $stage $manifest
    Test-PackageDependencyClosure $stage $manifest
    Test-PackageMetadata $stage $manifest
    $report.Steps += @{ Name = 'Stage/Verify'; Status = 'PASS' }

    $phase = 'Package'
    $partial = Join-Path $transactionRoot 'archive.partial'
    $zip = [IO.Compression.ZipFile]::Open($partial, [IO.Compression.ZipArchiveMode]::Create)
    try
    {
        $paths = [string[]] (@($manifest.Files.Path) + @('manifests/sparkle-package-manifest.json'))
        [Array]::Sort($paths, [StringComparer]::Ordinal)
        foreach ($path in $paths)
        {
            $entry = $zip.CreateEntry($path, [IO.Compression.CompressionLevel]::Optimal)
            $entry.LastWriteTime = [DateTimeOffset]::new(1980,1,1,0,0,0,[TimeSpan]::Zero)
            $entry.ExternalAttributes = 1
            $reader = [IO.File]::OpenRead((Join-Path $stage $path))
            $writer = $entry.Open()
            try { $reader.CopyTo($writer) }
            finally { $reader.Dispose(); $writer.Dispose() }
        }
    }
    finally { $zip.Dispose() }
    [IO.File]::SetAttributes($partial, [IO.FileAttributes]::ReadOnly)
    Test-PackageArchive $partial $stage $manifest
    Test-PackageStage $stage $manifest
    Test-PackageDependencyClosure $stage $manifest
    Test-PackageMetadata $stage $manifest
    Assert-ReleaseCandidateUnchanged
    $archiveHash = (Get-FileHash -LiteralPath $partial).Hash.ToLowerInvariant()
    $archive = Join-Path $outputRoot "SparkleEngine-v$version-windows-x64-unsigned-internal-$archiveHash.zip"
    [IO.File]::Move($partial, $archive)
    $report.Archive = @{ Path = [IO.Path]::GetFileName($archive); Sha256 = $archiveHash; Size = (Get-Item -LiteralPath $archive).Length }
    $report.Steps += @{ Name = 'Package/ArchiveVerify'; Status = 'PASS' }
    $report.Status = 'PASS'
}
catch
{
    $report.Steps += @{ Name = $phase; Status = 'BLOCKED'; Reason = $_.Exception.Message }
    foreach ($skipped in @('Build','Cook','Stage','Verify','Package'))
    {
        if ($skipped -ne $phase -and !@($report.Steps | Where-Object Name -like "$skipped*").Count)
        {
            $report.Steps += @{ Name = $skipped; Status = 'SKIPPED'; Reason = "Predecessor $phase did not pass." }
        }
    }
    Write-Error $_ -ErrorAction Continue
}
finally
{
    # Failed transactions are retained for diagnosis under .partial; no ZIP is published from them.
    if (Test-Path -LiteralPath $transactionRoot -PathType Container)
    {
        $report | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath (Join-Path $transactionRoot 'report.json') -Encoding UTF8
        Write-Output "Release report: $transactionRoot/report.json"
    }
    if ($null -ne $lock) { $lock.Dispose() }
}
if ($report.Status -ne 'PASS') { exit 1 }
