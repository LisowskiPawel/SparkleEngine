Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot 'PreparedProducts.ps1')
. (Join-Path $PSScriptRoot '../../Artifacts/ShaderExport.ps1')

function Get-PackageShaderExport
{
    param([string] $RepositoryRoot, [string] $ShaderExport)
    Assert-ArtifactPath $ShaderExport
    $root = Join-Path $RepositoryRoot 'artifacts/symbols/shaders/Showcase'
    $file = Get-LocalArtifactFile (Join-Path $RepositoryRoot $ShaderExport) 16777216
    if (!$file.FullName.StartsWith($root + '\', [StringComparison]::OrdinalIgnoreCase))
    {
        throw 'Shader export escapes the configured symbol owner.'
    }
    $export = Get-ShaderExport $file.FullName
    $cooked = Join-Path $RepositoryRoot 'artifacts/dev/Projects/Showcase/Cooked/Shaders'
    $cookedFiles = @(foreach ($binding in @(@('GlobalShaderMap.smap','globalShaderMapSha256'), @('CookedShaderLibrary.slib','cookedShaderLibrarySha256')))
    {
        $product = Get-LocalArtifactFile (Join-Path $cooked $binding[0]) ([long]::MaxValue)
        if ((Get-FileHash -LiteralPath $product.FullName).Hash.ToLowerInvariant() -cne $export.($binding[1]))
        {
            throw 'Shader export does not match the final cooked products.'
        }
        @{ Source = $product.FullName.Substring($RepositoryRoot.Length + 1).Replace('\','/')
            Size = $product.Length; Sha256 = $export.($binding[1]) }
    })
    return @{ CompilerSha256 = $export.shaderCompilerExecutableSha256; CookedFiles = $cookedFiles }
}

function Assert-PackageOutputs
{
    param([string] $RepositoryRoot, $Products, $Inputs)
    $prepared = @(Get-PreparedProductInputs $RepositoryRoot $Products)
    $shader = Get-PackageShaderExport $RepositoryRoot $Inputs.ShaderExport
    $compiler = @($prepared | Where-Object { $_.Component -ceq 'ShaderCompiler' })
    if ($compiler.Count -ne 1 -or $compiler[0].Binary.Sha256 -cne $shader.CompilerSha256)
    {
        throw 'Shader export belongs to a different compiler executable.'
    }

    foreach ($file in $Inputs.Files)
    {
        [void] (Assert-ArtifactFile $RepositoryRoot $file.Source $file.Size $file.Sha256)
    }
}
