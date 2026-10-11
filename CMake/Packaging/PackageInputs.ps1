Set-StrictMode -Version Latest

function Get-PackageProducts
{
    param([string] $BuildDirectory, [string] $Version)
    $products = @()
    foreach ($configuration in @('ShippingEditor','ShippingGame'))
    {
        $declaration = Get-Content -LiteralPath (Join-Path $BuildDirectory "private/release-products/$configuration.json") -Raw -Encoding UTF8 | ConvertFrom-Json
        if ($declaration.Version -cne $Version) { throw 'Configured release identity is stale.' }
        if (@($declaration.Products | Where-Object Configuration -cne $configuration).Count)
        {
            throw "Configured products contain a different profile: '$configuration'."
        }
        $products += @($declaration.Products)
    }
    return $products
}

function New-PackageManifest
{
    param([string] $RepositoryRoot, [string] $Version, [string] $Revision, [string] $SourceInputsSha256, $Products, $Inputs)
    $manifest = [ordered]@{
        Version = $Version; Revision = $Revision; SourceInputsSha256 = $SourceInputsSha256
        Signing = 'UnsignedInternal'; ProjectPath = 'Projects/Showcase'; EnginePath = 'Engine'; ContentStore = 'Content/Showcase'
        Products = @($Products | Select-Object Component, Target, Configuration, Path)
        SystemImports = @($Inputs.SystemImports)
        Files = @($Inputs.Files | Select-Object Path, Size, Sha256, Component, Role, Permission, Owner, Origin, License, Notice, Signature)
    }
    Assert-PackageManifest $manifest
    Assert-PackageLayout $manifest
    foreach ($product in $Products)
    {
        $member = @($Inputs.Files | Where-Object Path -eq $product.Path)
        if ([string]::IsNullOrWhiteSpace($product.Source) -or $member.Count -ne 1 -or
            $member[0].Component -cne $product.Component -or
            [IO.Path]::GetFullPath((Join-Path $RepositoryRoot $member[0].Source)) -ne [IO.Path]::GetFullPath($product.Source))
        {
            throw "Missing/incompatible product admission: '$($product.Component)'."
        }
    }
    foreach ($path in @('Tools/CMake/bin/cmake.exe','manifests/sparkle-sbom.spdx.json','manifests/sparkle-provenance.json'))
    {
        if (@($Inputs.Files | Where-Object Path -eq $path).Count -ne 1) { throw "Missing required input: '$path'." }
    }
    foreach ($file in $Inputs.Files)
    {
        [void] (Assert-PackageFile $RepositoryRoot $file.Source $file.Size $file.Sha256)
    }
    foreach ($binding in @(
        @('Tools/Diagnostics/ExportDiagnostics.ps1','CMake/Packaging/ExportDiagnostics.ps1'),
        @('Tools/Diagnostics/IncidentIdentity.ps1','CMake/Packaging/IncidentIdentity.ps1'),
        @('Tools/Diagnostics/PackageVerification.ps1','CMake/Packaging/PackageVerification.ps1'),
        @('docs/SUPPORT.md','SUPPORT.md'), @('docs/SECURITY.md','SECURITY.md'), @('docs/KNOWN_ISSUES.md','KNOWN_ISSUES.md')))
    {
        $file = @($Inputs.Files | Where-Object Path -ceq $binding[0])
        if ($file.Count -ne 1 -or $file[0].Source -cne $binding[1] -or $file[0].Component -cne 'Shared' -or
            $file[0].Role -cne $(if ($binding[0].EndsWith('.ps1')) { 'Helper' } else { 'Metadata' }))
        {
            throw 'Diagnostic helper/public policy admission differs from its source owner.'
        }
    }
    foreach ($name in @('SyncAssetPack.cmake','SyncAssetPackFiles.cmake','SyncAssetPackArchive.cmake'))
    {
        $file = @($Inputs.Files | Where-Object Path -ceq "Tools/Content/$name")
        if ($file.Count -ne 1 -or $file[0].Source -cne "Tools/Launcher/SparkleLauncher/Scripts/$name" -or
            $file[0].Component -cne 'Shared' -or $file[0].Role -cne 'Helper')
        {
            throw "Content acquisition helper differs from its source owner: '$name'."
        }
    }
    return $manifest
}

function Get-PackageInputAudit
{
    param([string] $RepositoryRoot, [string] $BuildDirectory, [string] $Version)
    . (Join-Path $PSScriptRoot 'IncidentIdentity.ps1')
    . (Join-Path $PSScriptRoot 'PackageDependencies.ps1')
    $artifactRoot = Join-Path $RepositoryRoot 'artifacts'
    $products = @(Get-PackageProducts $BuildDirectory $Version)
    $facts = @(foreach ($product in $products)
    {
        $fact = [ordered]@{
            Component = $product.Component; Target = $product.Target; Configuration = $product.Configuration
            Path = $product.Path; Status = 'BLOCKED'; Binary = $null; NativeSymbols = $null
            Imports = @(); RuntimeFiles = @(); CompilerRuntime = @(); MissingAppLocalImports = @(); Error = $null
        }
        try
        {
            foreach ($path in @($product.Source, $product.Symbols))
            {
                if ([string]::IsNullOrWhiteSpace($path) -or
                    ![IO.Path]::GetFullPath($path).StartsWith($artifactRoot + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase))
                {
                    throw 'Configured product/symbol input is absent or outside the repository-owned artifact tree.'
                }
            }
            $binary = Get-IncidentFile $product.Source ([long]::MaxValue)
            $source = $binary.FullName.Substring($RepositoryRoot.Length + 1).Replace('\','/')
            $fact.Binary = @{ Source = $source; Size = $binary.Length; Sha256 = (Get-FileHash -LiteralPath $binary.FullName).Hash.ToLowerInvariant() }
            $fact.Imports = @(Get-PackagePeImports $binary.FullName)
            $fact.RuntimeFiles = @(foreach ($file in Get-ChildItem -LiteralPath $binary.DirectoryName -Filter *.dll -File -Recurse | Sort-Object FullName)
            {
                [void] (Get-IncidentFile $file.FullName ([long]::MaxValue))
                @{ Path = $file.FullName.Substring($binary.DirectoryName.Length + 1).Replace('\','/')
                    Size = $file.Length; Sha256 = (Get-FileHash -LiteralPath $file.FullName).Hash.ToLowerInvariant()
                    Signature = (Get-AuthenticodeSignature -LiteralPath $file.FullName).Status.ToString()
                    Imports = @(Get-PackagePeImports $file.FullName) }
            })
            $fact.MissingAppLocalImports = @($fact.Imports | Where-Object {
                $_ -match '^(vcruntime|msvcp|concrt|Qt[56]|dxcompiler|dxil|slang|sl\.|nvngx|Sparkle)' -and
                !(Test-Path -LiteralPath (Join-Path $binary.DirectoryName $_) -PathType Leaf)
            })
            $fact.CompilerRuntime = @(foreach ($runtime in $product.MSVCRuntime)
            {
                $publisher = Get-IncidentFile $runtime ([long]::MaxValue)
                $path = Join-Path $binary.DirectoryName $publisher.Name
                $hash = (Get-FileHash -LiteralPath $publisher.FullName).Hash.ToLowerInvariant()
                $matches = (Test-Path -LiteralPath $path -PathType Leaf) -and
                    (Get-FileHash -LiteralPath $path).Hash.ToLowerInvariant() -ceq $hash
                @{ Name = $publisher.Name; Size = $publisher.Length; Sha256 = $hash
                    MatchesCompilerRedistributable = $matches
                    Signature = $(if ($matches) { (Get-AuthenticodeSignature -LiteralPath $path).Status.ToString() } else { 'Unavailable' }) }
            })
            if ($fact.MissingAppLocalImports.Count -ne 0 -or !@($fact.CompilerRuntime).Count -or
                @($fact.CompilerRuntime | Where-Object { !$_.MatchesCompilerRedistributable -or $_.Signature -cne 'Valid' }).Count)
            {
                throw 'App-local compiler/runtime bytes are missing or do not match the compiler-selected signed redistributables.'
            }
            $symbols = Get-IncidentFile $product.Symbols ([long]::MaxValue)
            $binaryId = Get-NativeSymbolIdentity $binary.FullName
            $symbolId = Get-NativeSymbolIdentity $symbols.FullName
            if ($binaryId.Guid -cne $symbolId.Guid -or $binaryId.Age -ne $symbolId.Age)
            {
                throw 'PE/PDB build identity mismatch.'
            }
            $fact.NativeSymbols = @{ Source = $symbols.FullName.Substring($RepositoryRoot.Length + 1).Replace('\','/')
                Size = $symbols.Length; Sha256 = (Get-FileHash -LiteralPath $symbols.FullName).Hash.ToLowerInvariant()
                Guid = $binaryId.Guid; Age = $binaryId.Age }
            $fact.Status = 'PASS'
        }
        catch { $fact.Error = $_.Exception.Message }
        $fact
    })
    return [ordered]@{
        Status = 'BLOCKED'; Products = $facts
        Limit = 'Discovery facts only: not a reviewed allowlist, legal admission, full dependency closure, shader-symbol or current-source build proof.'
        Remaining = @('Rights owner receipt and component notices/terms/source obligations',
            'Reviewed complete file allowlist and ordinary/delay/dynamic dependency closure',
            'Native and shader symbol/source correlation receipt', 'Installed content-store journey and exact-archive standard-user rehearsal')
    }
}
