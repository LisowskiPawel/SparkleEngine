Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot 'PackageManifest.ps1')

function New-PackageManifest
{
    param([string] $RepositoryRoot, [string] $Version, [string] $Revision, [string] $SourceInputsSha256, $Products, $Inputs)
    $packagedProducts = @($Products | Where-Object { $Inputs.Files.Path -ccontains $_.Path })
    foreach ($component in @('Launcher','Editor','Game'))
    {
        if (@($packagedProducts | Where-Object Component -ceq $component).Count -ne 1) { throw "Missing/duplicate release product: '$component'." }
    }
    $manifest = [ordered]@{
        Version = $Version; Revision = $Revision; SourceInputsSha256 = $SourceInputsSha256
        Signing = 'UnsignedInternal'
        Products = @($packagedProducts | Select-Object Component, Target, Configuration, Path, SourcePackages)
        SystemImports = @($Inputs.SystemImports)
        Files = @($Inputs.Files | Select-Object Path, Size, Sha256, Component, Role, Permission, Owner, Origin, License, Notice, Signature)
    }
    Assert-PackageManifest $manifest
    Assert-PackageLayout $manifest
    Assert-PackageProductInputs $RepositoryRoot $packagedProducts $Inputs
    foreach ($path in @('notices/ThirdPartyNotices.txt',
                        'manifests/sparkle-sbom.spdx.json','manifests/sparkle-provenance.json'))
    {
        if (@($Inputs.Files | Where-Object Path -eq $path).Count -ne 1) { throw "Missing required input: '$path'." }
    }
    $entry = @($Inputs.Files | Where-Object Path -ceq 'LaunchSparkle.cmd')
    if ($entry.Count -ne 1 -or $entry[0].Source -cne 'CMake/Packaging/LaunchSparkle.cmd')
    {
        throw 'Missing canonical Launcher entry script.'
    }
    Assert-PackageRecipeInputs $RepositoryRoot $Inputs
    return $manifest
}

function Assert-PackageProductInputs
{
    param([string] $RepositoryRoot, $PackagedProducts, $Inputs)

    foreach ($product in $packagedProducts)
    {
        $member = @($Inputs.Files | Where-Object Path -eq $product.Path)
        if ([string]::IsNullOrWhiteSpace($product.Source) -or $member.Count -ne 1 -or
            $member[0].Component -cne $product.Component -or
            [IO.Path]::GetFullPath((Join-Path $RepositoryRoot $member[0].Source)) -ne [IO.Path]::GetFullPath($product.Source))
        {
            throw "Missing/incompatible product admission: '$($product.Component)'."
        }
        $directory = $product.Path.Substring(0, $product.Path.LastIndexOf('/') + 1)
        foreach ($support in $product.RuntimeSupport)
        {
            Assert-ArtifactPath $support
            $member = @($Inputs.Files | Where-Object Path -ceq ($directory + $support))
            if ($member.Count -ne 1 -or $member[0].Component -cne $product.Component -or $member[0].Role -cne 'Helper')
            {
                throw "Missing/incompatible runtime support: '$($directory + $support)'."
            }
        }
    }
}

function Assert-PackageRecipeInputs
{
    param([string] $RepositoryRoot, $Inputs)

    foreach ($file in $Inputs.Files | Where-Object Path -like '*.gltf')
    {
        $gltf = Get-Content -LiteralPath (Join-Path $RepositoryRoot $file.Source) -Raw -Encoding UTF8 | ConvertFrom-Json
        $references = @($gltf.PSObject.Properties | Where-Object Name -in @('buffers','images') | ForEach-Object Value)
        foreach ($reference in $references)
        {
            $uri = $reference.PSObject.Properties['uri']
            if ($null -eq $uri -or $uri.Value.StartsWith('data:')) { continue }
            $directory = [IO.Path]::GetDirectoryName((Join-Path $RepositoryRoot $file.Path))
            $path = [IO.Path]::GetFullPath((Join-Path $directory ([Uri]::UnescapeDataString($uri.Value))))
            if (!$path.StartsWith($RepositoryRoot.TrimEnd('\','/') + '\', [StringComparison]::OrdinalIgnoreCase))
            {
                throw "glTF reference escapes the package: '$($file.Path)'."
            }
            $relative = $path.Substring($RepositoryRoot.TrimEnd('\','/').Length + 1).Replace('\','/')
            if (@($Inputs.Files | Where-Object { $_.Path -eq $relative -and $_.Role -in @('Recipe','EngineContent') }).Count -ne 1)
            {
                throw "Missing declared glTF buffer/image: '$relative' referenced by '$($file.Path)'."
            }
        }
    }
    $defaults = @($Inputs.Files | Where-Object Path -ceq 'Config/DefaultEngine.ini')
    if ($defaults.Count -ne 1 -or $defaults[0].Source -cne 'Config/DefaultEngine.ini' -or
        $defaults[0].Component -cne 'Shared' -or $defaults[0].Role -cne 'Defaults')
    {
        throw 'Rendering defaults differ from their configuration owner.'
    }
}
