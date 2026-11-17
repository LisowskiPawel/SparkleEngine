Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot 'Artifacts/ArtifactIdentity.ps1')

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

function Get-SourcePatch
{
    param([string] $RepositoryRoot)

    $start = [Diagnostics.ProcessStartInfo]::new()
    $start.FileName = (Get-Command git -CommandType Application | Select-Object -First 1).Source
    $start.WorkingDirectory = $RepositoryRoot
    $start.Arguments = 'diff --no-ext-diff --no-textconv --binary HEAD'
    $start.UseShellExecute = $false; $start.CreateNoWindow = $true; $start.RedirectStandardOutput = $true
    $start.StandardOutputEncoding = [Text.UTF8Encoding]::new($false)
    $process = [Diagnostics.Process]::Start($start)
    try
    {
        $patch = $process.StandardOutput.ReadToEnd()
        $process.WaitForExit()
        if ($process.ExitCode -ne 0) { throw 'Cannot bind the dirty Git patch.' }
    }
    finally { $process.Dispose() }

    return $patch
}

function Get-SourceCandidate
{
    param([string] $RepositoryRoot)

    $revision = & git -C $RepositoryRoot rev-parse HEAD
    if ($LASTEXITCODE -ne 0 -or $revision -cnotmatch '^[a-f0-9]{40}$') { throw 'Source revision is unavailable.' }
    $source = Get-ReleaseSourceInputs -SourceRoot $RepositoryRoot
    $patch = Get-SourcePatch -RepositoryRoot $RepositoryRoot
    # Untracked bytes are independently covered by sourceInputsSha256.
    return [ordered]@{ revision = $revision; sourceInputsSha256 = Get-TextSha256 $source
        dirtyPatchSha256 = Get-TextSha256 $patch; sourceFiles = [object[]]($source | ConvertFrom-Json) }
}
