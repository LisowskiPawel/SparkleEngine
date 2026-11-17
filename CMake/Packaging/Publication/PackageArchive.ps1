Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot '../../Artifacts/ArtifactIdentity.ps1')

function Write-PackageArchive
{
    param([string] $ArchivePath, [string] $Stage, $Manifest)
    $zip = [IO.Compression.ZipFile]::Open($ArchivePath, [IO.Compression.ZipArchiveMode]::Create)
    try
    {
        $paths = [string[]] (@($Manifest.Files.Path) + @('manifests/sparkle-package-manifest.json'))
        [Array]::Sort($paths, [StringComparer]::Ordinal)
        foreach ($path in $paths)
        {
            $entry = $zip.CreateEntry($path, [IO.Compression.CompressionLevel]::Optimal)
            $entry.LastWriteTime = [DateTimeOffset]::new(1980,1,1,0,0,0,[TimeSpan]::Zero)
            $entry.ExternalAttributes = 1
            $reader = [IO.File]::OpenRead((Join-Path $Stage $path))
            try
            {
                $writer = $entry.Open()
                try { $reader.CopyTo($writer) }
                finally { $writer.Dispose() }
            }
            finally { $reader.Dispose() }
        }
    }
    finally { $zip.Dispose() }
    [IO.File]::SetAttributes($ArchivePath, [IO.FileAttributes]::ReadOnly)
}

function Test-PackageArchive
{
    param([string] $ArchivePath, [string] $Stage, $Manifest)
    $expected = [Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
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
            Assert-ArtifactPath $entry.FullName
            if (!$expected.ContainsKey($entry.FullName)) { throw "Extra/duplicate archive member: '$($entry.FullName)'." }
            $file = $expected[$entry.FullName]
            if ($entry.Length -ne $file.Size) { throw "Archive size mismatch: '$($entry.FullName)'." }
            if (($entry.ExternalAttributes -band 1) -eq 0) { throw "Missing read-only archive attribute: '$($entry.FullName)'." }
            $stream = $entry.Open()
            $hash = [Security.Cryptography.SHA256]::Create()
            try { $digest = [BitConverter]::ToString($hash.ComputeHash($stream)).Replace('-','').ToLowerInvariant() }
            finally { $stream.Dispose(); $hash.Dispose() }
            if ($digest -cne $file.Sha256) { throw "Archive hash mismatch: '$($entry.FullName)'." }
            [void] $expected.Remove($entry.FullName)
        }
        if ($expected.Count -ne 0) { throw 'Missing archive members.' }
    }
    finally { $zip.Dispose() }
}
