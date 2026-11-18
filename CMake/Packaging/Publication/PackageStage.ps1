Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot '../Payload/PackageManifest.ps1')

function New-PackageStage
{
    param([string] $RepositoryRoot, [string] $Stage, $Files, $Manifest)
    [void] (New-Item -ItemType Directory -Path $Stage)
    foreach ($file in $Files)
    {
        Assert-ArtifactPath $file.Source
        $source = (Get-LocalArtifactFile (Join-Path $RepositoryRoot $file.Source) ([long]::MaxValue)).FullName
        $destination = Join-Path $Stage $file.Path
        [void] (New-Item -ItemType Directory -Force -Path (Split-Path $destination))
        [IO.File]::Copy($source, $destination, $false)
        [IO.File]::SetAttributes($destination, [IO.FileAttributes]::ReadOnly)
    }
    [void] (New-Item -ItemType Directory -Force -Path (Join-Path $Stage 'manifests'))
    $manifestPath = Join-Path $Stage 'manifests/sparkle-package-manifest.json'
    $Manifest | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath $manifestPath -Encoding UTF8
    [IO.File]::SetAttributes($manifestPath, [IO.FileAttributes]::ReadOnly)
}

function Test-PackageStage
{
    param([string] $Stage, $Manifest)
    Assert-PackageManifest $Manifest
    Assert-PackageLayout $Manifest
    $expected = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    [void] $expected.Add('manifests/sparkle-package-manifest.json')
    foreach ($file in $Manifest.Files)
    {
        [void] $expected.Add($file.Path)
        $path = Assert-ArtifactFile $Stage $file.Path $file.Size $file.Sha256
        if ($file.Path -match '\.(exe|dll)$')
        {
            if ($file.Signature -eq 'VendorSigned' -and (Get-AuthenticodeSignature -LiteralPath $path).Status -ne 'Valid')
            {
                throw "Invalid vendor signature: '$($file.Path)'."
            }
            if ($file.Signature -eq 'NotApplicable') { throw "Missing PE signature policy: '$($file.Path)'." }
        }
    }
    # Walk directories explicitly so a reparse point is rejected before descent.
    $pending = [Collections.Generic.Stack[string]]::new()
    $pending.Push($Stage)
    while ($pending.Count -gt 0)
    {
        $directory = Get-Item -LiteralPath $pending.Pop() -Force
        if (($directory.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw 'Reparse stage directory.' }
        foreach ($item in Get-ChildItem -LiteralPath $directory.FullName -Force)
        {
            if (($item.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw 'Reparse stage member.' }
            if ($item.PSIsContainer) { $pending.Push($item.FullName); continue }
            $relative = $item.FullName.Substring($Stage.Length + 1).Replace('\','/')
            if (!$expected.Remove($relative)) { throw "Extra stage member: '$relative'." }
        }
    }
    if ($expected.Count -ne 0) { throw 'Missing stage members.' }
    $stored = Get-Content -LiteralPath (Join-Path $Stage 'manifests/sparkle-package-manifest.json') -Raw -Encoding UTF8 | ConvertFrom-Json
    if (($stored | ConvertTo-Json -Depth 20 -Compress) -cne ($Manifest | ConvertTo-Json -Depth 20 -Compress))
    {
        throw 'Stage manifest differs from verified inputs.'
    }
}
