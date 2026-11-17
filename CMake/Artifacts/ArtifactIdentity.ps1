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
    $current = Get-Item -LiteralPath $Root -Force
    foreach ($part in $Path.Split('/'))
    {
        if (($current.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0)
        {
            throw "Reparse point is not an artifact input: '$($current.FullName)'."
        }
        $current = Get-Item -LiteralPath (Join-Path $current.FullName $part) -Force
    }
    if ($current.PSIsContainer -or ($current.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0)
    {
        throw "Expected a regular artifact file: '$Path'."
    }
    if ($Sha256 -notmatch '^[0-9a-f]{64}$' -or $current.Length -ne $Size -or
        (Get-FileHash -LiteralPath $current.FullName -Algorithm SHA256).Hash.ToLowerInvariant() -cne $Sha256)
    {
        throw "Artifact input size/hash mismatch: '$Path'."
    }
    return $current.FullName
}

function Get-LocalArtifactFile
{
    param([string] $Path, [long] $MaximumBytes)
    if ([IO.Path]::GetFullPath($Path).StartsWith('\\')) { throw 'Choose a local file, not a network/device path.' }
    $item = Get-Item -LiteralPath $Path -Force -ErrorAction Stop
    if ($item.PSIsContainer -or $item.Length -gt $MaximumBytes) { throw 'Artifact input is absent, not a file, or exceeds its byte limit.' }
    $current = $item
    while ($null -ne $current)
    {
        if (($current.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw 'Artifact input/output uses a reparse point.' }
        $current = if ($current -is [IO.DirectoryInfo]) { $current.Parent } else { $current.Directory }
    }
    return $item
}

function Get-TextSha256
{
    param([string] $Text)
    $hash = [Security.Cryptography.SHA256]::Create()
    try { return [BitConverter]::ToString($hash.ComputeHash([Text.Encoding]::UTF8.GetBytes($Text))).Replace('-','').ToLowerInvariant() }
    finally { $hash.Dispose() }
}
