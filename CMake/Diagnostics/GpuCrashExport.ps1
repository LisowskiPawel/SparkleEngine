Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot 'GpuCrashIncident.ps1')

function Get-GpuCrashExport
{
    param([string] $ManifestPath, [bool] $IncludeGpuDump, [bool] $IncludeNvdbg, [bool] $IncludeSource, [bool] $IncludeSymbols)
    $incident = Get-GpuCrashIncident $ManifestPath
    $record = [ordered]@{
        IncidentId = $incident.incidentId; State = $incident.state; ExecutableSha256 = $incident.executableSha256
        SourceMapping = $incident.sourceMapping; NativeFailure = $incident.nativeFailure; Candidate = $null
        Privacy = $(if ($IncludeGpuDump -or $IncludeNvdbg -or $IncludeSource -or $IncludeSymbols) { 'ExplicitPrivatePayloads' } else { 'SanitizedMetadata' }); AutomaticUpload = $false
        Categories = [ordered]@{ RawDump = $IncludeGpuDump; NvidiaDebugInfo = $IncludeNvdbg; Source = $IncludeSource; Symbols = $IncludeSymbols }
        Files = @()
    }
    if ($null -ne $incident.candidate)
    {
        $record.Candidate = [ordered]@{}
        foreach ($name in @('product','profile','graphicsApi','revision','sourceInputsSha256','dirtyPatchSha256','productReceiptSha256','shaderExportSha256',
                            'globalShaderMapSha256','cookedShaderLibrarySha256','cpuPdbGuid','cpuPdbAge','cpuPdbSha256','aftermathRuntimeSha256'))
        {
            $value = $incident.candidate.$name
            if ([string] $value -cnotmatch '^[A-Za-z0-9.\-]{1,128}$') { throw 'Unsafe shareable identity.' }
            $record.Candidate[$name] = $value
        }
    }
    if ($IncludeSymbols -and !$IncludeSource) { throw 'Full GPU symbols may embed source: select both Source and Symbols explicitly.' }
    $copies = @()
    $root = [IO.Path]::GetDirectoryName([IO.Path]::GetFullPath($ManifestPath))
    foreach ($file in $incident.files)
    {
        if (($IncludeGpuDump -and $file.path -ceq 'crash.nv-gpudmp') -or ($IncludeNvdbg -and $file.path.EndsWith('.nvdbg')))
        {
            $copies += [pscustomobject]@{ Source = Join-Path $root $file.path; Path = $file.path; Sha256 = $file.sha256; Bytes = $file.bytes }
        }
    }
    if ($IncludeSource -or $IncludeSymbols)
    {
        $bundle = Get-GpuCrashBundle $incident
        $bundleRoot = [IO.Path]::GetDirectoryName($incident.candidate.shaderExportPath)
        foreach ($file in $bundle.files)
        {
            $source = $file.path.StartsWith('sources/') -or $file.path.EndsWith('/preprocessed-source.hlsl') -or $file.path.EndsWith('/compiler-input.hlsl')
            $symbols = [IO.Path]::GetExtension($file.path) -cin @('.pdb','.dxil','.spv','.bin')
            if (($IncludeSource -and $source) -or ($IncludeSymbols -and $symbols))
            {
                $copies += [pscustomobject]@{ Source = Join-Path $bundleRoot $file.path; Path = 'bundle/' + $file.path; Sha256 = $file.sha256; Bytes = $file.bytes }
            }
        }
        if ($IncludeSymbols)
        {
            $pdb = Get-LocalArtifactFile $incident.candidate.cpuPdbPath 1073741824
            $copies += [pscustomobject]@{ Source = $pdb.FullName; Path = 'symbols/' + $incident.candidate.product + '.pdb'; Sha256 = $incident.candidate.cpuPdbSha256; Bytes = $pdb.Length }
        }
    }
    if ($copies.Count -ne 0 -and ($copies | Measure-Object -Property Bytes -Sum).Sum -gt 1073741824) { throw 'Export exceeds its 1 GiB limit.' }
    if ($copies.Count -gt 65535) { throw 'Export file count exceeds its limit.' }
    $record.Files = @($copies | Select-Object Path, Sha256, Bytes)
    return @{ Record = $record; Copies = $copies }
}

function Write-GpuCrashExport
{
    param([string] $OutputDirectory, [string] $Json, $Copies)
    $output = Get-Item -LiteralPath $OutputDirectory -Force
    if (!$output.PSIsContainer -or $output.FullName.StartsWith('\\')) { throw 'Choose an existing local export directory.' }
    $current = $output
    while ($null -ne $current)
    {
        if (($current.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw 'Reparse export path.' }
        $current = $current.Parent
    }
    $destination = Join-Path $output.FullName ('sparkle-diagnostic-' + [Guid]::NewGuid().ToString('N'))
    $temporary = $destination + '.partial'
    [void] [IO.Directory]::CreateDirectory($temporary)
    foreach ($file in $Copies)
    {
        Assert-ArtifactPath $file.Path
        [void] (Get-LocalArtifactFile $file.Source 1073741824)
        $path = Join-Path $temporary $file.Path
        [void] [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($path))
        $input = [IO.File]::Open($file.Source, [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::Read)
        try
        {
            $stream = [IO.File]::Open($path, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::None)
            try { $input.CopyTo($stream); $stream.Flush($true) }
            finally { $stream.Dispose() }
        }
        finally { $input.Dispose() }
        if ((Get-Item -LiteralPath $path).Length -ne $file.Bytes -or (Get-FileHash -LiteralPath $path).Hash.ToLowerInvariant() -cne $file.Sha256)
        {
            throw 'Export input changed while copying; only a partial directory was retained.'
        }
    }
    [void] (Write-IncidentRecord $temporary $Json 'incident')
    [IO.Directory]::Move($temporary, $destination)
    return $destination
}

function Write-IncidentRecord
{
    param([string] $OutputDirectory, [string] $Json, [string] $Prefix)
    if ([IO.Path]::GetFullPath($OutputDirectory).StartsWith('\\')) { throw 'Choose a local export directory.' }
    $directory = Get-Item -LiteralPath $OutputDirectory -Force
    if (!$directory.PSIsContainer) { throw 'Choose an existing local export directory.' }
    $current = $directory
    while ($null -ne $current)
    {
        if (($current.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw 'Reparse export directory.' }
        $current = $current.Parent
    }
    $path = Join-Path $directory.FullName ($Prefix + '-' + [Guid]::NewGuid().ToString('N') + '.json')
    $partial = $path + '.partial'
    $stream = [IO.File]::Open($partial, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::None)
    try
    {
        $bytes = [Text.UTF8Encoding]::new($false).GetBytes($Json)
        $stream.Write($bytes, 0, $bytes.Length)
        $stream.Flush($true)
    }
    finally { $stream.Dispose() }
    [IO.File]::Move($partial, $path)
    return $path
}
