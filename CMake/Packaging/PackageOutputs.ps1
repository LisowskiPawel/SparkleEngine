Set-StrictMode -Version Latest

function Get-PackageShaderExport
{
    param([string] $RepositoryRoot, [string] $CookLog)
    $markers = @(Get-Content -LiteralPath $CookLog | Where-Object { $_.StartsWith('SPARKLE_SHADER_EXPORT ') })
    if ($markers.Count -ne 1) { throw 'The final cook must return exactly one shader export receipt.' }
    $path = $markers[0].Substring('SPARKLE_SHADER_EXPORT '.Length) | ConvertFrom-Json
    $root = Join-Path $RepositoryRoot 'artifacts/symbols/shaders/Showcase'
    $file = Get-IncidentFile $path 16777216
    if (!$file.FullName.StartsWith($root + '\', [StringComparison]::OrdinalIgnoreCase))
    {
        throw 'Shader export escapes the configured symbol owner.'
    }
    $export = Get-Content -LiteralPath $file.FullName -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($export.scope -cne 'registered-catalog' -or !@($export.jobs).Count -or !@($export.files).Count)
    {
        throw 'The final cook did not export a complete shader catalog.'
    }
    $paths = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    foreach ($entry in $export.files)
    {
        if (!$paths.Add($entry.path)) { throw 'Duplicate shader export member.' }
        [void] (Assert-PackageFile $file.DirectoryName $entry.path $entry.bytes $entry.sha256)
    }
    foreach ($job in $export.jobs)
    {
        if (!$paths.Contains($job)) { throw 'Shader job is absent from its export receipt.' }
    }
    $cooked = Join-Path $RepositoryRoot 'artifacts/dev/Projects/Showcase/Cooked/Shaders'
    foreach ($binding in @(@('GlobalShaderMap.smap','globalShaderMapSha256'), @('CookedShaderLibrary.slib','cookedShaderLibrarySha256')))
    {
        if ((Get-FileHash -LiteralPath (Join-Path $cooked $binding[0])).Hash.ToLowerInvariant() -cne $export.($binding[1]))
        {
            throw 'Shader export does not match the final cooked products.'
        }
    }
    return @{ Path = $file.FullName.Substring($RepositoryRoot.Length + 1).Replace('\','/')
        Size = $file.Length; Sha256 = (Get-FileHash -LiteralPath $file.FullName).Hash.ToLowerInvariant()
        CompilerSha256 = $export.shaderCompilerExecutableSha256 }
}

function Update-PackageSbom
{
    param([string] $RepositoryRoot, [string] $TransactionRoot, $Inputs)
    $entry = @($Inputs.Files | Where-Object { $_.Path -ceq 'manifests/sparkle-sbom.spdx.json' })
    if ($entry.Count -ne 1) { throw 'Missing/duplicate reviewed SBOM.' }
    $sbom = Get-Content -LiteralPath (Join-Path $RepositoryRoot $entry[0].Source) -Raw -Encoding UTF8 | ConvertFrom-Json
    $files = @{}
    foreach ($file in $Inputs.Files) { $files[$file.Path] = $file }
    foreach ($file in $sbom.files)
    {
        if (!$file.fileName.StartsWith('./')) { throw 'SPDX file names must be package relative.' }
        $path = $file.fileName.Substring(2)
        if (!$files.ContainsKey($path)) { throw 'SBOM contains an unadmitted output.' }
        $checksums = @($file.checksums | Where-Object { $_.algorithm -ceq 'SHA256' })
        if ($checksums.Count -ne 1) { throw 'SBOM output has no unique SHA-256 checksum.' }
        $checksums[0].checksumValue = $files[$path].Sha256
    }
    $metadata = Join-Path $TransactionRoot 'metadata'
    [void] (New-Item -ItemType Directory -Path $metadata)
    $path = Join-Path $metadata 'sparkle-sbom.spdx.json'
    $sbom | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath $path -Encoding UTF8
    $entry[0].Source = $path.Substring($RepositoryRoot.Length + 1).Replace('\','/')
    $entry[0].Size = (Get-Item -LiteralPath $path).Length
    $entry[0].Sha256 = (Get-FileHash -LiteralPath $path).Hash.ToLowerInvariant()
}

function Update-PackageOutputs
{
    param([string] $RepositoryRoot, [string] $BuildDirectory, [string] $Version, [string] $TransactionRoot, $Products, $Inputs)
    . (Join-Path $PSScriptRoot 'IncidentIdentity.ps1')
    $audit = Get-PackageInputAudit $RepositoryRoot $BuildDirectory $Version
    if (@($audit.Products | Where-Object { $_.Status -cne 'PASS' }).Count)
    {
        throw 'Final product, runtime or PE/PDB audit failed.'
    }
    $shader = Get-PackageShaderExport $RepositoryRoot (Join-Path $TransactionRoot 'cook-ShippingGame.log')
    $compiler = @($audit.Products | Where-Object { $_.Component -ceq 'ShaderCompiler' })
    if ($compiler.Count -ne 1 -or $compiler[0].Binary.Sha256 -cne $shader.CompilerSha256)
    {
        throw 'Shader export belongs to a different compiler executable.'
    }

    $generated = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    foreach ($product in $Products) { [void] $generated.Add([IO.Path]::GetFullPath($product.Source)) }
    foreach ($file in $Inputs.Files)
    {
        $path = [IO.Path]::GetFullPath((Join-Path $RepositoryRoot $file.Source))
        $cooked = $file.Path.StartsWith('Engine/Cooked/', [StringComparison]::Ordinal) -and $file.Role -ceq 'EngineContent' -and
            $file.Source.StartsWith('artifacts/dev/Projects/Showcase/Cooked/', [StringComparison]::Ordinal)
        if ($generated.Contains($path) -or $cooked)
        {
            # Only products of this build/cook may acquire new identities. External inputs stay pinned.
            $output = Get-IncidentFile $path ([long]::MaxValue)
            $file.Size = $output.Length
            $file.Sha256 = (Get-FileHash -LiteralPath $output.FullName).Hash.ToLowerInvariant()
        }
        else { [void] (Assert-PackageFile $RepositoryRoot $file.Source $file.Size $file.Sha256) }
    }

    Update-PackageSbom $RepositoryRoot $TransactionRoot $Inputs
    return @{ Products = $audit.Products; ShaderExport = $shader }
}
