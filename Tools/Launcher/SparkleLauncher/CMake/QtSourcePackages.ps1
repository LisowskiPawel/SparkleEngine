param([string] $QtRoot, [string] $Version, [string] $SourceList, [string] $OutputPath)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$packages = @{}
$dependencies = @{}
$owners = @{}
$publisherInputs = @()
foreach ($module in @('qtbase', 'qtsvg', 'qttranslations'))
{
    $path = Join-Path $QtRoot "sbom/$module-$Version.spdx.json"
    if (!(Test-Path -LiteralPath $path))
    {
        if ($module -eq 'qttranslations') { continue }
        throw "The selected Qt kit needs its matching publisher SBOM: '$path'."
    }
    $document = Get-Content -LiteralPath $path -Raw -Encoding UTF8 | ConvertFrom-Json
    $publisherInputs += @{ File = $path; Sha256 = (Get-FileHash -LiteralPath $path).Hash.ToLowerInvariant() }
    $files = @{}
    foreach ($file in $document.files) { $files[$file.SPDXID] = $file.fileName.Substring(2) }
    foreach ($package in $document.packages) { $packages[$package.SPDXID] = $package }
    foreach ($relationship in $document.relationships)
    {
        $source = $relationship.spdxElementId
        $target = $relationship.relatedSpdxElement -replace '^DocumentRef-[^:]+:', ''
        if ($relationship.relationshipType -ceq 'CONTAINS' -and $files.ContainsKey($target))
        {
            $owners[$files[$target]] = $source
        }
        if ($relationship.relationshipType -ceq 'DEPENDS_ON')
        {
            if (!$dependencies.ContainsKey($source)) { $dependencies[$source] = [Collections.Generic.List[string]]::new() }
            $dependencies[$source].Add($target)
        }
    }
}

$rootPrefix = [IO.Path]::GetFullPath($QtRoot).TrimEnd('\', '/') + [IO.Path]::DirectorySeparatorChar
$pending = [Collections.Generic.Stack[string]]::new()
$selected = [Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
$result = [Collections.Generic.List[object]]::new()
foreach ($source in (Get-Content -LiteralPath $SourceList -Raw).Split(';'))
{
    $absolute = [IO.Path]::GetFullPath($source)
    $relative = if ($absolute.StartsWith($rootPrefix, [StringComparison]::OrdinalIgnoreCase)) { $absolute.Substring($rootPrefix.Length).Replace('\', '/') } else { '' }
    if ($owners.ContainsKey($relative)) { $pending.Push($owners[$relative]); continue }
    if ($relative.StartsWith('translations/'))
    {
        # Qt ships aggregate translation catalogs outside individual module SBOMs.
        [void] $selected.Add('Sparkle-QtTranslations')
        continue
    }
    $name = [IO.Path]::GetFileName($absolute)
    $hash = (Get-FileHash -LiteralPath $absolute).Hash.ToLowerInvariant()
    switch -Regex ($name)
    {
        '^opengl32sw[.]dll$' { $component = 'Qt deployed Mesa'; $license = 'LicenseRef-Qt-Mesa-Notices'; $location = 'https://www.qt.io' }
        '^D3Dcompiler_47[.]dll$' { $component = 'Microsoft D3DCompiler'; $license = 'LicenseRef-Microsoft-DirectX-Redistributable'; $location = 'https://learn.microsoft.com/windows/win32/direct3dhlsl/dx-graphics-hlsl-dxcompiler' }
        '^dxcompiler[.]dll$' { $component = 'Microsoft DXC runtime'; $license = 'LicenseRef-DXC-ThirdPartyNotices'; $location = 'https://github.com/microsoft/DirectXShaderCompiler' }
        '^dxil[.]dll$' { $component = 'Microsoft DXIL runtime'; $license = 'LicenseRef-Microsoft-DirectX-Redistributable'; $location = 'https://github.com/microsoft/DirectXShaderCompiler' }
        default { throw "Qt deploys an unattributed SDK file '$source'. Add its actual publisher identity and terms before admitting it." }
    }
    $result.Add(@{ name = $component; versionInfo = "sha256:$hash"; licenseDeclared = $license; downloadLocation = $location })
}
while ($pending.Count)
{
    $id = $pending.Pop()
    if (!$selected.Add($id)) { continue }
    if (!$packages.ContainsKey($id)) { throw "Unresolved Qt publisher dependency '$id'." }
    $package = $packages[$id]
    # System/compiler wrappers are build inputs, not distributed libraries.
    if ($id -match '-system-3rdparty-') { continue }
    $license = $package.licenseDeclared
    if ($license -in @('', 'NONE', 'NOASSERTION')) { $license = $package.licenseConcluded }
    if ($license -in @('', 'NONE', 'NOASSERTION')) { throw "Unqualified publisher license for Qt '$($package.name)'." }
    $versionInfo = if ($package.PSObject.Properties.Name -contains 'versionInfo') { $package.versionInfo } else { $Version }
    $location = if ($package.downloadLocation -notin @('', 'NONE', 'NOASSERTION')) { $package.downloadLocation } else { 'https://code.qt.io/qt/' }
    $result.Add(@{ name = "Qt $($package.name)"; versionInfo = $versionInfo; licenseDeclared = $license; downloadLocation = $location })
    if ($dependencies.ContainsKey($id)) { foreach ($dependency in $dependencies[$id]) { $pending.Push($dependency) } }
}
if ($selected.Contains('Sparkle-QtTranslations'))
{
    $result.Add(@{ name = 'Qt translations'; versionInfo = $Version; licenseDeclared = 'LicenseRef-Qt-Translation-Notices'; downloadLocation = 'https://code.qt.io/qt/qttranslations.git' })
}
$json = ConvertTo-Json -InputObject @($result | ForEach-Object { [pscustomobject] $_ } | Sort-Object -Property name) -Depth 8
[IO.File]::WriteAllText($OutputPath, $json, [Text.UTF8Encoding]::new($false))
$receipt = @{ PublisherInputs = $publisherInputs; SelectedIds = @($selected); Sources = (Get-Content -LiteralPath $SourceList -Raw).Split(';') }
[IO.File]::WriteAllText($OutputPath + '.inputs.json', ($receipt | ConvertTo-Json -Depth 8), [Text.UTF8Encoding]::new($false))
