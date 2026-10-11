Set-StrictMode -Version Latest

function Assert-PackagePath
{
    param([string] $Path)
    if ([string]::IsNullOrWhiteSpace($Path) -or $Path -match '[\\:\x00-\x1f]' -or
        $Path.StartsWith('/') -or $Path.EndsWith('/') -or $Path -match '(^|/)\.\.?(/|$)')
    {
        throw "Invalid package-relative path: '$Path'."
    }
    foreach ($part in $Path.Split('/'))
    {
        if ($part -eq '' -or $part -match '[<>"|?*]' -or $part -match '[. ]$' -or
            $part -match '^(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(\.|$)')
        {
            throw "Invalid Windows package path: '$Path'."
        }
    }
}

function Assert-PackageFile
{
    param([string] $Root, [string] $Path, [long] $Size, [string] $Sha256)
    Assert-PackagePath $Path
    $current = Get-Item -LiteralPath $Root -Force
    foreach ($part in $Path.Split('/'))
    {
        if (($current.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0)
        {
            throw "Reparse point is not a package input: '$($current.FullName)'."
        }
        $current = Get-Item -LiteralPath (Join-Path $current.FullName $part) -Force
    }
    if ($current.PSIsContainer -or ($current.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0)
    {
        throw "Expected a regular package file: '$Path'."
    }
    if ($Sha256 -notmatch '^[0-9a-f]{64}$' -or $current.Length -ne $Size -or
        (Get-FileHash -LiteralPath $current.FullName -Algorithm SHA256).Hash.ToLowerInvariant() -cne $Sha256)
    {
        throw "Package input size/hash mismatch: '$Path'."
    }
    return $current.FullName
}

function Assert-PackageManifest
{
    param($Manifest)
    $paths = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    foreach ($file in $Manifest.Files)
    {
        Assert-PackagePath $file.Path
        if (!$paths.Add($file.Path) -or $file.Path -eq 'manifests/sparkle-package-manifest.json')
        {
            throw "Duplicate or reserved manifest path: '$($file.Path)'."
        }
        if ($file.Path -match '\.(pdb|ilk|obj|lib|exp|cpp|cxx|h|hpp)$' -or
            $file.Path -match '(^|/)(build|_deps|\.git|Cache|Logs|Settings)(/|$)' -or
            $file.Path -match '(^|/)(Sponza|ModernSponza)/.*\.(glb|gltf|bin|dds|png|jpg|ktx2)$')
        {
            throw "Unadmitted source/debug/user-state/content payload: '$($file.Path)'."
        }
        if ($file.Component -notin @('Launcher','Editor','Game','AssetCooker','TextureCooker','ShaderCompiler','CMake','Engine','Shared') -or
            $file.Role -notin @('Runtime','Helper','EngineContent','Recipe','Defaults','Notice','Metadata') -or
            $file.Permission -ne 'ReadOnly' -or $file.License -match '^(|NOASSERTION|NONE)$' -or
            $file.Signature -notin @('UnsignedFirstParty','VendorSigned','NotApplicable') -or
            [string]::IsNullOrWhiteSpace($file.Owner) -or [string]::IsNullOrWhiteSpace($file.Origin) -or
            $file.Origin -match '[\\:]' -or $file.Origin.StartsWith('/'))
        {
            throw "Incomplete or private-path payload declaration: '$($file.Path)'."
        }
        if ($file.Path.StartsWith('game/', [StringComparison]::OrdinalIgnoreCase) -and $file.Path -match '(?i)(Qt6|SparkleEditor|AssetCooker|TextureCooker|ShaderCompiler)')
        {
            throw "Editor/authoring dependency in Game component: '$($file.Path)'."
        }
        if ($file.Path -match '\.(exe|dll)$' -and $file.Role -notin @('Runtime','Helper'))
        {
            throw "PE payload has a non-executable role: '$($file.Path)'."
        }
    }
    foreach ($file in $Manifest.Files)
    {
        if (!$paths.Contains($file.Notice)) { throw "Notice is not admitted for '$($file.Path)'." }
    }
}

function Assert-PackageLayout
{
    param($Manifest)
    foreach ($path in @($Manifest.ProjectPath, $Manifest.EnginePath, $Manifest.ContentStore)) { Assert-PackagePath $path }
    if ($Manifest.ProjectPath -cne 'content/Showcase' -or $Manifest.EnginePath -cne 'engine' -or $Manifest.ContentStore -cne 'Content/Showcase')
    {
        throw 'Package layout differs from the frozen three-product content-store contract.'
    }
    foreach ($path in @('content/Showcase/.sparkle-project', 'content/Showcase/Levels.catalog'))
    {
        if (@($Manifest.Files | Where-Object Path -ceq $path).Count -ne 1) { throw "Missing package recipe input: '$path'." }
    }
    if (!@($Manifest.Files | Where-Object { $_.Path.StartsWith('engine/Assets/') -and $_.Role -eq 'EngineContent' }).Count)
    {
        throw 'Missing immutable engine assets.'
    }
    foreach ($file in $Manifest.Files)
    {
        if ($file.Path -match '^content/Showcase/(Cooked|Cache|Downloads)(/|$)')
        {
            throw "Writable selected-content output inside immutable package: '$($file.Path)'."
        }
    }
}

function Test-PackageStage
{
    param([string] $Stage, $Manifest)
    Assert-PackageManifest $Manifest
    $expected = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    [void] $expected.Add('manifests/sparkle-package-manifest.json')
    foreach ($file in $Manifest.Files)
    {
        [void] $expected.Add($file.Path)
        $path = Assert-PackageFile $Stage $file.Path $file.Size $file.Sha256
        if (((Get-Item -LiteralPath $path).Attributes -band [IO.FileAttributes]::ReadOnly) -eq 0)
        {
            throw "Writable stage member: '$($file.Path)'."
        }
        if ($file.Path -match '\.(exe|dll)$')
        {
            $reader = [IO.BinaryReader]::new([IO.File]::OpenRead($path))
            try
            {
                if ($reader.ReadUInt16() -ne 0x5a4d) { throw "Not a Windows PE: '$($file.Path)'." }
                $reader.BaseStream.Position = 0x3c
                $offset = $reader.ReadInt32()
                if ($offset -lt 0x40 -or $offset -gt ($reader.BaseStream.Length - 6)) { throw 'Invalid PE header.' }
                $reader.BaseStream.Position = $offset
                if ($reader.ReadUInt32() -ne 0x4550 -or $reader.ReadUInt16() -ne 0x8664) { throw "Not a Windows x64 PE: '$($file.Path)'." }
            }
            finally { $reader.Dispose() }
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
    if (((Get-Item -LiteralPath (Join-Path $Stage 'manifests/sparkle-package-manifest.json')).Attributes -band [IO.FileAttributes]::ReadOnly) -eq 0)
    {
        throw 'Writable stage manifest.'
    }
    if (($stored | ConvertTo-Json -Depth 20 -Compress) -cne ($Manifest | ConvertTo-Json -Depth 20 -Compress))
    {
        throw 'Stage manifest differs from verified inputs.'
    }
}

function Test-PackageArchive
{
    param([string] $ArchivePath, [string] $Stage, $Manifest)
    $expected = @{}
    foreach ($file in $Manifest.Files) { $expected[$file.Path] = $file }
    $manifestPath = Join-Path $Stage 'manifests/sparkle-package-manifest.json'
    $expected['manifests/sparkle-package-manifest.json'] = @{
        Size = (Get-Item -LiteralPath $manifestPath).Length
        Sha256 = (Get-FileHash -LiteralPath $manifestPath).Hash.ToLowerInvariant()
    }
    $zip = [IO.Compression.ZipFile]::OpenRead($ArchivePath)
    try
    {
        foreach ($entry in $zip.Entries)
        {
            Assert-PackagePath $entry.FullName
            if (!$expected.ContainsKey($entry.FullName)) { throw "Extra/duplicate archive member: '$($entry.FullName)'." }
            $file = $expected[$entry.FullName]
            if ($entry.Length -ne $file.Size) { throw "Archive size mismatch: '$($entry.FullName)'." }
            if (($entry.ExternalAttributes -band 1) -eq 0) { throw "Missing read-only archive attribute: '$($entry.FullName)'." }
            $stream = $entry.Open()
            $hash = [Security.Cryptography.SHA256]::Create()
            try { $digest = [BitConverter]::ToString($hash.ComputeHash($stream)).Replace('-','').ToLowerInvariant() }
            finally { $stream.Dispose(); $hash.Dispose() }
            if ($digest -cne $file.Sha256) { throw "Archive hash mismatch: '$($entry.FullName)'." }
            $expected.Remove($entry.FullName)
        }
        if ($expected.Count -ne 0) { throw 'Missing archive members.' }
    }
    finally { $zip.Dispose() }
}

function Test-PackageMetadata
{
    param([string] $Stage, $Manifest)
    $sbom = Get-Content -LiteralPath (Join-Path $Stage 'manifests/sparkle-sbom.spdx.json') -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($sbom.spdxVersion -cne 'SPDX-2.3' -or $sbom.dataLicense -cne 'CC0-1.0' -or $sbom.SPDXID -cne 'SPDXRef-DOCUMENT' -or
        $sbom.documentNamespace -notmatch '^https://[^ ]+$' -or !@($sbom.creationInfo.creators).Count -or !@($sbom.packages).Count)
    {
        throw 'Incomplete SPDX 2.3 document identity.'
    }
    $inventory = @{}
    foreach ($file in $sbom.files)
    {
        if (!$file.fileName.StartsWith('./')) { throw 'SPDX file names must be package relative.' }
        $path = $file.fileName.Substring(2)
        Assert-PackagePath $path
        if ($inventory.ContainsKey($path)) { throw 'Duplicate SPDX file.' }
        $inventory[$path] = $file
    }
    foreach ($file in $Manifest.Files | Where-Object Path -notin @('manifests/sparkle-sbom.spdx.json','manifests/sparkle-provenance.json'))
    {
        if (!$inventory.ContainsKey($file.Path)) { throw "Missing SPDX file: '$($file.Path)'." }
        $entry = $inventory[$file.Path]
        $checksum = @($entry.checksums | Where-Object algorithm -ceq 'SHA256')
        if ($checksum.Count -ne 1 -or $checksum[0].checksumValue -cne $file.Sha256 -or $entry.licenseConcluded -cne $file.License)
        {
            throw "SPDX file identity differs from manifest: '$($file.Path)'."
        }
        $inventory.Remove($file.Path)
    }
    if ($inventory.Count -ne 0) { throw 'Extra SPDX file; metadata self-digests belong only in the package manifest.' }
    foreach ($component in $sbom.packages)
    {
        if ([string]::IsNullOrWhiteSpace($component.name) -or [string]::IsNullOrWhiteSpace($component.versionInfo) -or
            $component.licenseDeclared -match '^(|NONE|NOASSERTION)$') { throw 'Unqualified SPDX software component.' }
    }
    $provenance = Get-Content -LiteralPath (Join-Path $Stage 'manifests/sparkle-provenance.json') -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($provenance.Revision -cne $Manifest.Revision -or $provenance.SourceInputsSha256 -cne $Manifest.SourceInputsSha256 -or
        $provenance.Version -cne $Manifest.Version) { throw 'Provenance belongs to a different source candidate.' }
    foreach ($product in $Manifest.Products)
    {
        $entry = @($provenance.Products | Where-Object Component -ceq $product.Component)
        if ($entry.Count -ne 1 -or $entry[0].Target -cne $product.Target -or $entry[0].Configuration -cne $product.Configuration -or $entry[0].Path -cne $product.Path)
        {
            throw "Wrong provenance product: '$($product.Component)'."
        }
    }
    if (@($provenance.Products).Count -ne @($Manifest.Products).Count) { throw 'Extra provenance product.' }
}
