[CmdletBinding()]
param(
    [Parameter(Mandatory)][string] $Archive,
    [Parameter(Mandatory)][ValidatePattern('^[0-9a-f]{64}$')][string] $Sha256,
    [ValidateSet('SparkleLauncher','ShowcaseEditor','ShowcaseGame','AssetCooker','TextureCooker','ShaderCompiler')]
    [string[]] $Products = @('SparkleLauncher','ShowcaseEditor','ShowcaseGame')
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.IO.Compression.FileSystem
Add-Type -AssemblyName System.IO.Compression
. (Join-Path $PSScriptRoot 'Payload/PackageMetadata.ps1')
. (Join-Path $PSScriptRoot 'Payload/PackageDependencies.ps1')
. (Join-Path $PSScriptRoot 'Publication/PackageStage.ps1')
. (Join-Path $PSScriptRoot 'Publication/PackageArchive.ps1')
. (Join-Path $PSScriptRoot 'Publication/ProductImport.ps1')

$root = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '../..')).Path
if (!(Test-Path -LiteralPath (Join-Path $root 'CMakeLists.txt') -PathType Leaf)) { throw 'Product sync requires build sources.' }
$Archive = (Resolve-Path -LiteralPath $Archive).Path
if ($Products.Count -eq 0) { throw 'Select at least one product to sync.' }
if ((Get-FileHash -LiteralPath $Archive).Hash.ToLowerInvariant() -cne $Sha256) { throw 'Product sync archive hash mismatch.' }
$revision = & git -C $root rev-parse HEAD
if ($LASTEXITCODE -ne 0) { throw 'Cannot identify the source checkout revision.' }
$transactionRoot = [IO.Path]::GetFullPath((Join-Path $root 'build/private/product-sync'))
$transaction = Join-Path $transactionRoot ([Guid]::NewGuid().ToString('N'))
$stage = Join-Path $transaction 'archive'
$replacements = @()
$lock = $null
$preserveTransaction = $false
try
{
    [void] (New-Item -ItemType Directory -Force -Path $transaction)
    $lockPath = Join-Path $transactionRoot 'import.lock'
    $lock = [IO.File]::Open($lockPath, [IO.FileMode]::OpenOrCreate, [IO.FileAccess]::ReadWrite, [IO.FileShare]::None)
    $zip = [IO.Compression.ZipFile]::OpenRead($Archive)
    try
    {
        $paths = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
        foreach ($entry in $zip.Entries)
        {
            Assert-ArtifactPath $entry.FullName
            if (!$paths.Add($entry.FullName)) { throw 'Duplicate archive member.' }
        }
        [IO.Compression.ZipFile]::ExtractToDirectory($Archive, $stage)
    }
    finally { $zip.Dispose() }
    $manifest = Get-Content -LiteralPath (Join-Path $stage 'manifests/sparkle-package-manifest.json') -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($manifest.Revision -cne $revision.Trim()) { throw 'Product sync archive belongs to a different source revision.' }
    Test-PackageArchive $Archive $stage $manifest
    # Extraction does not preserve Windows file attributes. Restore the declared
    # immutable stage attributes before invoking the existing stage verifier.
    foreach ($file in Get-ChildItem -LiteralPath $stage -Recurse -File -Force) { $file.IsReadOnly = $true }
    Test-PackageStage $stage $manifest
    Test-PackageMetadata $stage $manifest
    Test-PackageDependencyClosure $stage $manifest

    $replacements = @(Get-ProductReplacements $root $transaction $stage $manifest $Products)
    # Prepare and verify every component before touching existing outputs.
    if ((Get-FileHash -LiteralPath $Archive).Hash.ToLowerInvariant() -cne $Sha256) { throw 'Product sync archive changed during import.' }
    $currentRevision = & git -C $root rev-parse HEAD
    if ($LASTEXITCODE -ne 0 -or $currentRevision.Trim() -cne $revision.Trim()) { throw 'Source revision changed during import.' }
    Publish-ProductReplacements $replacements
    Write-Output "Imported the selected binaries and cooked base for $($revision.Trim()) into artifacts/dev. Local builds and cooks replace these same files."
}
catch
{
    try
    {
        Undo-ProductReplacements $replacements
    }
    catch
    {
        $preserveTransaction = $true
        throw "Product sync rollback could not finish. Previous outputs remain at '$transaction/previous'. $($_.Exception.Message)"
    }
    throw
}
finally
{
    if ($lock) { $lock.Dispose() }
    if (!$preserveTransaction -and (Test-Path -LiteralPath $transaction))
    {
        $resolvedTransaction = (Get-Item -LiteralPath $transaction -Force).FullName
        if (!$resolvedTransaction.StartsWith($transactionRoot + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase))
        {
            throw 'Product sync cleanup escapes its transaction root.'
        }
        Remove-Item -LiteralPath $resolvedTransaction -Recurse -Force
    }
}
