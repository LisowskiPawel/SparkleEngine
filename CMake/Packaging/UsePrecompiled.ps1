[CmdletBinding()]
param(
    [Parameter(Mandatory)][string] $Archive,
    [Parameter(Mandatory)][ValidatePattern('^[0-9a-f]{64}$')][string] $Sha256,
    [ValidateSet('SparkleLauncher','ShowcaseEditor','ShowcaseGame','AssetCooker','TextureCooker','ShaderCompiler')]
    [string[]] $Products = @('SparkleLauncher','ShowcaseEditor','ShowcaseGame','AssetCooker','TextureCooker','ShaderCompiler')
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.IO.Compression.FileSystem
Add-Type -AssemblyName System.IO.Compression
. (Join-Path $PSScriptRoot 'PackageVerification.ps1')
. (Join-Path $PSScriptRoot 'PackageDependencies.ps1')

$root = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '../..')).Path
if (!(Test-Path -LiteralPath (Join-Path $root '.sparkle') -PathType Leaf)) { throw 'Precompiled import requires a source checkout.' }
$Archive = (Resolve-Path -LiteralPath $Archive).Path
if ($Products.Count -eq 0) { throw 'Select at least one precompiled product.' }
if ((Get-FileHash -LiteralPath $Archive).Hash.ToLowerInvariant() -cne $Sha256) { throw 'Precompiled archive hash mismatch.' }
$revision = & git -C $root rev-parse HEAD
if ($LASTEXITCODE -ne 0) { throw 'Cannot identify the source checkout revision.' }
$output = Join-Path $root 'artifacts/dev'
$transaction = Join-Path $root ('build/private/precompiled/' + [Guid]::NewGuid().ToString('N'))
$stage = Join-Path $transaction 'archive'
$replacements = @()
$lock = $null
$preserveTransaction = $false
try
{
    [void] (New-Item -ItemType Directory -Force -Path $transaction)
    $lockPath = Join-Path $root 'build/private/precompiled/import.lock'
    $lock = [IO.File]::Open($lockPath, [IO.FileMode]::OpenOrCreate, [IO.FileAccess]::ReadWrite, [IO.FileShare]::None)
    $zip = [IO.Compression.ZipFile]::OpenRead($Archive)
    try
    {
        $paths = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
        foreach ($entry in $zip.Entries)
        {
            Assert-PackagePath $entry.FullName
            if (!$paths.Add($entry.FullName)) { throw 'Duplicate precompiled archive member.' }
        }
        [IO.Compression.ZipFile]::ExtractToDirectory($Archive, $stage)
    }
    finally { $zip.Dispose() }
    $manifest = Get-Content -LiteralPath (Join-Path $stage 'manifests/sparkle-package-manifest.json') -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($manifest.Revision -cne $revision.Trim()) { throw 'Precompiled archive belongs to a different source revision.' }
    Test-PackageArchive $Archive $stage $manifest
    # Extraction does not preserve Windows file attributes. Restore the declared
    # immutable stage attributes before invoking the existing stage verifier.
    foreach ($file in Get-ChildItem -LiteralPath $stage -Recurse -File -Force) { $file.IsReadOnly = $true }
    Test-PackageStage $stage $manifest
    Test-PackageMetadata $stage $manifest
    Test-PackageDependencyClosure $stage $manifest
    Assert-PackageLayout $manifest

    foreach ($target in @($Products | Select-Object -Unique))
    {
        $product = @($manifest.Products | Where-Object Target -ceq $target)
        if ($product.Count -ne 1) { throw "Missing/duplicate precompiled product: '$target'." }
        $profile = if ($target -eq 'ShowcaseGame') { 'ShippingGame' } else { 'ShippingEditor' }
        $directory = switch ($target)
        {
            { $_ -in @('ShowcaseEditor','ShowcaseGame') } { "Projects/Showcase/Binaries/$profile" }
            default { "Tools/$target/Binaries/$profile" }
        }
        if ($product[0].Configuration -cne $profile -or $product[0].Path -cne "$directory/$target.exe")
        {
            throw "Precompiled product differs from the canonical build layout: '$target'."
        }
        $destination = Join-Path $output $directory
        # Reject links before replacing an owned binary directory.
        $current = [IO.Path]::GetFullPath($destination)
        while ($current)
        {
            if ((Test-Path -LiteralPath $current) -and ((Get-Item -LiteralPath $current -Force).Attributes -band [IO.FileAttributes]::ReparsePoint))
            {
                throw 'Precompiled output path contains a reparse point.'
            }
            $current = [IO.Path]::GetDirectoryName($current)
        }
        $incoming = Join-Path $stage $directory
        foreach ($file in Get-ChildItem -LiteralPath $incoming -Recurse -File -Force) { $file.IsReadOnly = $false }
        $replacements += @{ Destination = $destination; Incoming = $incoming; Previous = (Join-Path $transaction "previous/$target"); MovedPrevious = $false; Published = $false }
    }
    # Prepare and verify every component before touching existing outputs.
    if ((Get-FileHash -LiteralPath $Archive).Hash.ToLowerInvariant() -cne $Sha256) { throw 'Precompiled archive changed during import.' }
    $currentRevision = & git -C $root rev-parse HEAD
    if ($LASTEXITCODE -ne 0 -or $currentRevision.Trim() -cne $revision.Trim()) { throw 'Source revision changed during import.' }
    foreach ($replacement in $replacements)
    {
        [void] (New-Item -ItemType Directory -Force -Path (Split-Path $replacement.Destination), (Split-Path $replacement.Previous))
        if (Test-Path -LiteralPath $replacement.Destination)
        {
            [IO.Directory]::Move($replacement.Destination, $replacement.Previous)
            $replacement.MovedPrevious = $true
        }
        [IO.Directory]::Move($replacement.Incoming, $replacement.Destination)
        $replacement.Published = $true
    }
    Write-Output "Imported $($replacements.Count) precompiled products for $($revision.Trim()) into artifacts/dev. Local builds replace these same files."
}
catch
{
    try
    {
        for ($index = $replacements.Count - 1; $index -ge 0; --$index)
        {
            $replacement = $replacements[$index]
            if ($replacement.Published) { [IO.Directory]::Move($replacement.Destination, $replacement.Incoming) }
            if ($replacement.MovedPrevious) { [IO.Directory]::Move($replacement.Previous, $replacement.Destination) }
        }
    }
    catch
    {
        $preserveTransaction = $true
        throw "Precompiled rollback could not finish. Previous outputs remain at '$transaction/previous'. $($_.Exception.Message)"
    }
    throw
}
finally
{
    if ($lock) { $lock.Dispose() }
    if (!$preserveTransaction -and (Test-Path -LiteralPath $transaction)) { Remove-Item -LiteralPath $transaction -Recurse -Force }
}
