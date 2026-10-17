Set-StrictMode -Version Latest

function Get-ReleaseSourceInputs
{
    param([Parameter(Mandatory)][string] $SourceRoot)
    $encoding = [Console]::OutputEncoding
    try
    {
        [Console]::OutputEncoding = [Text.UTF8Encoding]::new($false)
        $paths = @(& git -c core.quotepath=false -C $SourceRoot ls-files --cached --others --exclude-standard)
    }
    finally { [Console]::OutputEncoding = $encoding }
    if ($LASTEXITCODE -ne 0) { throw 'Cannot inventory source inputs.' }
    $paths = [string[]] @($paths | Select-Object -Unique)
    [Array]::Sort($paths, [StringComparer]::Ordinal)
    $sourceInputs = @(foreach ($path in $paths)
    {
        $source = Join-Path $SourceRoot $path
        if ($path.StartsWith('"')) { throw 'Cannot bind an escaped Git source path; source inventory is unavailable.' }
        [ordered]@{ Path = $path; Sha256 = $(if (Test-Path -LiteralPath $source -PathType Leaf) { (Get-FileHash -LiteralPath $source).Hash.ToLowerInvariant() } else { $null }) }
    })
    return ConvertTo-Json -InputObject $sourceInputs -Depth 5 -Compress
}

function Get-ReleaseSourceHash
{
    param([string] $Json)
    $hash = [Security.Cryptography.SHA256]::Create()
    try { return [BitConverter]::ToString($hash.ComputeHash([Text.Encoding]::UTF8.GetBytes($Json))).Replace('-','').ToLowerInvariant() }
    finally { $hash.Dispose() }
}

