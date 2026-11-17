Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot '../../Artifacts/NativeSymbols.ps1')
. (Join-Path $PSScriptRoot 'PackageDependencies.ps1')

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

function Get-PreparedProductInputs
{
    param([string] $RepositoryRoot, $Products)
    $artifactRoot = Join-Path $RepositoryRoot 'artifacts'
    $facts = @(foreach ($product in $products)
    {
        $fact = [ordered]@{
            Component = $product.Component; Target = $product.Target; Configuration = $product.Configuration
            Path = $product.Path; Binary = $null; NativeSymbols = $null
            Imports = @(); RuntimeFiles = @(); SupportFiles = @(); CompilerRuntime = @(); MissingAppLocalImports = @()
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
            $binary = Get-LocalArtifactFile $product.Source ([long]::MaxValue)
            $source = $binary.FullName.Substring($RepositoryRoot.Length + 1).Replace('\','/')
            $fact.Binary = @{ Source = $source; Size = $binary.Length; Sha256 = (Get-FileHash -LiteralPath $binary.FullName).Hash.ToLowerInvariant() }
            $fact.Imports = @(Get-PackagePeImports $binary.FullName)
            $fact.RuntimeFiles = @(foreach ($file in Get-ChildItem -LiteralPath $binary.DirectoryName -Filter *.dll -File -Recurse | Sort-Object FullName)
            {
                [void] (Get-LocalArtifactFile $file.FullName ([long]::MaxValue))
                @{ Path = $file.FullName.Substring($binary.DirectoryName.Length + 1).Replace('\','/')
                    Size = $file.Length; Sha256 = (Get-FileHash -LiteralPath $file.FullName).Hash.ToLowerInvariant()
                    Signature = (Get-AuthenticodeSignature -LiteralPath $file.FullName).Status.ToString()
                    Imports = @(Get-PackagePeImports $file.FullName) }
            })
            $fact.SupportFiles = @(foreach ($support in $product.RuntimeSupport)
            {
                Assert-ArtifactPath $support
                $file = Get-LocalArtifactFile (Join-Path $binary.DirectoryName $support) ([long]::MaxValue)
                @{ Path = $support; Size = $file.Length; Sha256 = (Get-FileHash -LiteralPath $file.FullName).Hash.ToLowerInvariant() }
            })
            $fact.MissingAppLocalImports = @($fact.Imports | Where-Object {
                $_ -match '^(vcruntime|msvcp|concrt|Qt[56]|dxcompiler|dxil|slang|sl\.|nvngx|NvLowLatencyVk|Sparkle)' -and
                !(Test-Path -LiteralPath (Join-Path $binary.DirectoryName $_) -PathType Leaf)
            })
            $fact.CompilerRuntime = @(foreach ($runtime in $product.MSVCRuntime)
            {
                $publisher = Get-LocalArtifactFile $runtime ([long]::MaxValue)
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
            $symbols = Get-LocalArtifactFile $product.Symbols ([long]::MaxValue)
            $binaryId = Get-NativeSymbolIdentity $binary.FullName
            $symbolId = Get-NativeSymbolIdentity $symbols.FullName
            if ($binaryId.Guid -cne $symbolId.Guid -or $binaryId.Age -ne $symbolId.Age)
            {
                throw 'PE/PDB build identity mismatch.'
            }
            $fact.NativeSymbols = @{ Source = $symbols.FullName.Substring($RepositoryRoot.Length + 1).Replace('\','/')
                Size = $symbols.Length; Sha256 = (Get-FileHash -LiteralPath $symbols.FullName).Hash.ToLowerInvariant()
                Guid = $binaryId.Guid; Age = $binaryId.Age }
        }
        catch { throw "Prepared product '$($product.Component)' failed verification: $($_.Exception.Message)" }
        $fact
    })
    return $facts
}
