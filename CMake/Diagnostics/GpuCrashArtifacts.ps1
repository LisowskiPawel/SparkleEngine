[CmdletBinding()]
param(
    [Parameter(Mandatory)][ValidateSet('Prepare','Publish')][string] $Action,
    [Parameter(Mandatory)][string] $RepositoryRoot,
    [Parameter(Mandatory)][string] $SnapshotPath,
    [Parameter(Mandatory)][string] $Profile,
    [string] $ExecutablePath,
    [string] $ReceiptPath,
    [string] $PdbPath,
    [string] $SymbolRoot,
    [string] $RuntimeFiles = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot '../SourceIdentity.ps1')
. (Join-Path $PSScriptRoot '../Packaging/IncidentIdentity.ps1')

function Get-SourceCandidate
{
    $revision = & git -C $RepositoryRoot rev-parse HEAD
    if ($LASTEXITCODE -ne 0 -or $revision -cnotmatch '^[a-f0-9]{40}$') { throw 'Source revision is unavailable.' }
    $source = Get-ReleaseSourceInputs -SourceRoot $RepositoryRoot
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
    # Untracked bytes are independently covered by sourceInputsSha256.
    return [ordered]@{ revision = $revision; sourceInputsSha256 = Get-ReleaseSourceHash $source
        dirtyPatchSha256 = Get-ReleaseSourceHash $patch; sourceFiles = [object[]]($source | ConvertFrom-Json) }
}

function Write-Receipt
{
    param([string] $Path, $Record)
    $parent = [IO.Path]::GetDirectoryName([IO.Path]::GetFullPath($Path))
    [void] [IO.Directory]::CreateDirectory($parent)
    $temporary = $Path + '.' + [Guid]::NewGuid().ToString('N') + '.pending'
    try
    {
        [IO.File]::WriteAllText($temporary, ($Record | ConvertTo-Json -Depth 12 -Compress), [Text.UTF8Encoding]::new($false))
        if ([IO.File]::Exists($Path)) { [IO.File]::Replace($temporary, $Path, [NullString]::Value) }
        else { [IO.File]::Move($temporary, $Path) }
    }
    finally { if ([IO.File]::Exists($temporary)) { [IO.File]::Delete($temporary) } }
}

try
{
    $sourceRoot = [IO.Path]::GetFullPath($RepositoryRoot).TrimEnd('\','/')
    if (!(Test-Path -LiteralPath (Join-Path $sourceRoot '.sparkle') -PathType Leaf)) { throw 'Choose the source workspace root.' }
    $artifactPrefix = (Join-Path $sourceRoot 'artifacts/gpu-crash/dev').Replace('/', '\') + '\'
    $snapshotPrefix = (Join-Path $sourceRoot 'build/variants/gpu-crash/private/gpu-crash').Replace('/', '\') + '\'
    if (![IO.Path]::GetFullPath($SnapshotPath).StartsWith($snapshotPrefix, [StringComparison]::OrdinalIgnoreCase)) { throw 'Snapshot escapes the diagnostic build owner.' }
    $output = if ($Action -eq 'Prepare') { $ReceiptPath } else { $ExecutablePath }
    if (![IO.Path]::GetFullPath($output).StartsWith($artifactPrefix, [StringComparison]::OrdinalIgnoreCase)) { throw 'Product output escapes the diagnostic artifact owner.' }
    if ($Action -eq 'Prepare')
    {
        $oldReceipt = [IO.Path]::GetFullPath($ReceiptPath)
        if ([IO.Path]::GetFileName($oldReceipt) -cne 'gpu-crash-product.json') { throw 'Unexpected diagnostic receipt output.' }
        if ([IO.File]::Exists($oldReceipt))
        {
            [void] (Get-IncidentFile $oldReceipt 33554432)
            [IO.File]::Delete($oldReceipt)
        }
        $candidate = Get-SourceCandidate
        Write-Receipt $SnapshotPath $candidate
        exit 0
    }
    $candidate = Get-SourceCandidate
    $before = Get-Content -LiteralPath $SnapshotPath -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($before.revision -cne $candidate.revision -or $before.sourceInputsSha256 -cne $candidate.sourceInputsSha256 -or $before.dirtyPatchSha256 -cne $candidate.dirtyPatchSha256)
    {
        throw 'Source changed during the build; rebuild before using diagnostic receipts.'
    }
    $binary = Get-IncidentFile $ExecutablePath ([long]::MaxValue)
    $symbols = Get-IncidentFile $PdbPath ([long]::MaxValue)
    $peId = Get-NativeSymbolIdentity $binary.FullName
    $pdbId = Get-NativeSymbolIdentity $symbols.FullName
    if ($peId.Guid -cne $pdbId.Guid -or $peId.Age -ne $pdbId.Age) { throw 'PE/PDB identity mismatch.' }
    $runtime = @(foreach ($path in $RuntimeFiles.Split('|', [StringSplitOptions]::RemoveEmptyEntries))
    {
        $file = Get-IncidentFile $path ([long]::MaxValue)
        [ordered]@{ path = $file.Name; sha256 = (Get-FileHash -LiteralPath $file.FullName).Hash.ToLowerInvariant(); bytes = $file.Length }
    })
    $executableHash = (Get-FileHash -LiteralPath $binary.FullName).Hash.ToLowerInvariant()
    $pdbHash = (Get-FileHash -LiteralPath $symbols.FullName).Hash.ToLowerInvariant()
    $expectedSymbols = Join-Path $symbols.DirectoryName 'retained'
    $symbolPrefix = (Join-Path $sourceRoot 'artifacts/gpu-crash/symbols').Replace('/','\') + '\'
    if (![IO.Path]::GetFullPath($SymbolRoot).StartsWith($symbolPrefix, [StringComparison]::OrdinalIgnoreCase)) { throw 'Symbol retention escapes the diagnostic namespace.' }
    if ([IO.Path]::GetFullPath($SymbolRoot).TrimEnd('\','/') -ine $expectedSymbols.Replace('/','\')) { throw 'Symbol retention escapes the artifact owner.' }
    $runtimeJson = ConvertTo-Json -InputObject $runtime -Depth 6 -Compress
    $symbolId = Get-ReleaseSourceHash ($candidate.sourceInputsSha256 + "`n" + $executableHash + "`n" + $pdbHash + "`n" + $Profile + "`n" + $runtimeJson)
    $retained = Join-Path $SymbolRoot $symbolId
    [void] [IO.Directory]::CreateDirectory($retained)
    $retainedPdb = Join-Path $retained $symbols.Name
    if (![IO.File]::Exists($retainedPdb)) { [IO.File]::Copy($symbols.FullName, $retainedPdb, $false) }
    [void] (Get-IncidentFile $retainedPdb ([long]::MaxValue))
    if ((Get-FileHash -LiteralPath $retainedPdb).Hash.ToLowerInvariant() -cne $pdbHash) { throw 'Retained CPU symbols changed.' }
    $retainedReceipt = Join-Path $retained 'gpu-crash-product.json'
    $receipt = [ordered]@{
        product = $binary.BaseName; profile = $Profile; variant = 'gpu-crash'
        revision = $candidate.revision; sourceInputsSha256 = $candidate.sourceInputsSha256; dirtyPatchSha256 = $candidate.dirtyPatchSha256
        executableSha256 = $executableHash
        cpuPdbGuid = $pdbId.Guid; cpuPdbAge = $pdbId.Age
        cpuPdbPath = $retainedPdb; cpuPdbSha256 = $pdbHash; retainedReceiptPath = $retainedReceipt
        runtimeFiles = $runtime; sourceFiles = $candidate.sourceFiles
    }
    if ([IO.File]::Exists($retainedReceipt))
    {
        [void] (Get-IncidentFile $retainedReceipt 33554432)
        $expected = $receipt | ConvertTo-Json -Depth 12 -Compress
        if ([IO.File]::ReadAllText($retainedReceipt) -cne $expected) { throw 'Retained product receipt changed.' }
    }
    else { Write-Receipt $retainedReceipt $receipt }
    Write-Receipt (Join-Path $binary.DirectoryName 'gpu-crash-product.json') $receipt
}
catch
{
    [Console]::Error.WriteLine('GPU crash product receipt failed: ' + $_.Exception.Message)
    exit 1
}
