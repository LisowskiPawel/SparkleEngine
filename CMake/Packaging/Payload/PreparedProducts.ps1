Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot '../../Artifacts/NativeSymbols.ps1')

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
    foreach ($product in $Products)
    {
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
            if (@($product.MSVCRuntime).Count -eq 0) { throw 'Missing compiler-selected redistributables.' }
            foreach ($runtime in $product.MSVCRuntime)
            {
                $publisher = Get-LocalArtifactFile $runtime ([long]::MaxValue)
                $path = Join-Path $binary.DirectoryName $publisher.Name
                if ((Get-FileHash -LiteralPath $path).Hash -cne (Get-FileHash -LiteralPath $publisher.FullName).Hash -or
                    (Get-AuthenticodeSignature -LiteralPath $path).Status -ne 'Valid')
                {
                    throw 'App-local runtime differs from the compiler-selected signed redistributable.'
                }
            }
            $symbols = Get-LocalArtifactFile $product.Symbols ([long]::MaxValue)
            $binaryId = Get-NativeSymbolIdentity $binary.FullName
            $symbolId = Get-NativeSymbolIdentity $symbols.FullName
            if ($binaryId.Guid -cne $symbolId.Guid -or $binaryId.Age -ne $symbolId.Age)
            {
                throw 'PE/PDB build identity mismatch.'
            }
            @{ Component = $product.Component
                Binary = @{ Source = $binary.FullName.Substring($RepositoryRoot.Length + 1).Replace('\','/')
                    Size = $binary.Length; Sha256 = (Get-FileHash -LiteralPath $binary.FullName).Hash.ToLowerInvariant() } }
        }
        catch { throw "Prepared product '$($product.Component)' failed verification: $($_.Exception.Message)" }
    }
}
