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
        if ($file.Path.StartsWith('game/') -and $file.Path -match '(?i)(Qt6|SparkleEditor|AssetCooker|TextureCooker|ShaderCompiler)')
        {
            throw "Editor/authoring dependency in Game component: '$($file.Path)'."
        }
    }
    foreach ($file in $Manifest.Files)
    {
        if (!$paths.Contains($file.Notice)) { throw "Notice is not admitted for '$($file.Path)'." }
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
