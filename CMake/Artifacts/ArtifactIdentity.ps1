Set-StrictMode -Version Latest

function Assert-ArtifactPath
{
    param([string] $Path)
    if ([string]::IsNullOrWhiteSpace($Path) -or $Path -match '[\\:\x00-\x1f]' -or
        $Path.StartsWith('/') -or $Path.EndsWith('/') -or $Path -match '(^|/)\.\.?(/|$)')
    {
        throw "Invalid artifact-relative path: '$Path'."
    }
    foreach ($part in $Path.Split('/'))
    {
        if ($part -eq '' -or $part -match '[<>"|?*]' -or $part -match '[. ]$' -or
            $part -match '^(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(\.|$)')
        {
            throw "Invalid Windows artifact path: '$Path'."
        }
    }
}

function Assert-ArtifactFile
{
    param([string] $Root, [string] $Path, [long] $Size, [string] $Sha256)
    Assert-ArtifactPath $Path
    $file = Get-LocalArtifactFile (Join-Path $Root $Path) ([long]::MaxValue)
    if ($Sha256 -notmatch '^[0-9a-f]{64}$' -or $file.Length -ne $Size -or
        (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant() -cne $Sha256)
    {
        throw "Artifact input size/hash mismatch: '$Path'."
    }
    return $file.FullName
}

function Resolve-LocalArtifactPath
{
    param([string] $Path)
    $resolved = [IO.Path]::GetFullPath($Path)
    if ($resolved.StartsWith('\\')) { throw 'Choose a local artifact path, not a network/device path.' }
    # Missing tail components are allowed for new publication destinations.
    $current = $resolved
    while ($current)
    {
        if ((Test-Path -LiteralPath $current) -and
            ((Get-Item -LiteralPath $current -Force -ErrorAction Stop).Attributes -band [IO.FileAttributes]::ReparsePoint))
        {
            throw 'Artifact input/output uses a reparse point.'
        }
        $current = [IO.Path]::GetDirectoryName($current)
    }
    return $resolved
}

function Get-LocalArtifactFile
{
    param([string] $Path, [long] $MaximumBytes)
    $resolved = Resolve-LocalArtifactPath $Path
    $item = Get-Item -LiteralPath $resolved -Force -ErrorAction Stop
    if ($item.PSIsContainer -or $item.Length -gt $MaximumBytes) { throw 'Artifact input is absent, not a file, or exceeds its byte limit.' }
    return $item
}

function Get-TextSha256
{
    param([string] $Text)
    $hash = [Security.Cryptography.SHA256]::Create()
    try { return [BitConverter]::ToString($hash.ComputeHash([Text.Encoding]::UTF8.GetBytes($Text))).Replace('-','').ToLowerInvariant() }
    finally { $hash.Dispose() }
}
