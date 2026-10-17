Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot '../Packaging/IncidentIdentity.ps1')

function Get-GpuCrashIncident
{
    param([string] $ManifestPath, [string] $ExpectedSessionId = '', [uint32] $ExpectedChildPid = 0)
    $file = Get-IncidentFile $ManifestPath 33554432
    if ($file.Name -cne 'incident.json') { throw 'Choose the exact incident manifest.' }
    $incident = Get-Content -LiteralPath $file.FullName -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($incident.incidentId -cnotmatch '^[a-f0-9]{8}-[a-f0-9]{4}-4[a-f0-9]{3}-[89ab][a-f0-9]{3}-[a-f0-9]{12}$' -or
        $incident.childPid -le 0 -or [string] $incident.nativeFailure -cnotmatch '^-?[0-9]{1,20}$' -or $incident.state -cnotin @('Preparing','Ready','Collecting','Collected','Partial','Timed out','Unavailable') -or
        $incident.sourceMapping -cne 'unqualified' -or $incident.executableSha256 -cnotmatch '^[a-f0-9]{64}$')
    {
        throw 'Incomplete GPU crash session identity.'
    }
    [void] (Get-IncidentFile (Join-Path $file.DirectoryName 'owner.txt') 36)
    $owner = [IO.File]::ReadAllText((Join-Path $file.DirectoryName 'owner.txt'))
    if ($owner -cne $incident.incidentId -or ($ExpectedSessionId -ne '' -and $ExpectedSessionId -cne $incident.incidentId) -or
        ($ExpectedChildPid -ne 0 -and $ExpectedChildPid -ne $incident.childPid)) { throw 'Stale or other-child incident.' }
    if (@($incident.files).Count -gt 65535) { throw 'Incident file count exceeds its limit.' }
    $names = @{}
    $total = [long] $file.Length
    foreach ($entry in $incident.files)
    {
        if ($entry.path -cnotmatch '^(crash\.nv-gpudmp|shader-[a-f0-9]+-[a-f0-9]+\.(nvdbg|json)|native-[a-f0-9]+-[a-f0-9]+\.json)$' -or
            $names.ContainsKey($entry.path) -or $entry.sha256 -cnotmatch '^[a-f0-9]{64}$') { throw 'Invalid or duplicate incident membership.' }
        $names[$entry.path] = $true
        $payload = Get-IncidentFile (Join-Path $file.DirectoryName $entry.path) 1073741824
        $total += $payload.Length
        if ($payload.Length -ne $entry.bytes -or (Get-FileHash -LiteralPath $payload.FullName).Hash.ToLowerInvariant() -cne $entry.sha256 -or $total -gt 1073741824)
        {
            throw 'Incomplete or changed incident payload.'
        }
    }
    return $incident
}

function Get-GpuCrashBundle
{
    param($Incident)
    $candidate = $Incident.candidate
    if ($null -eq $candidate) { throw 'GPU crash inputs were not qualified.' }
    foreach ($pair in @(@('shaderExportPath','shaderExportSha256'), @('productReceiptPath','productReceiptSha256'), @('cpuPdbPath','cpuPdbSha256')))
    {
        [void] (Get-IncidentFile $candidate.($pair[0]) $(if ($pair[0] -eq 'cpuPdbPath') { [long]::MaxValue } else { 33554432 }))
        if ((Get-FileHash -LiteralPath $candidate.($pair[0])).Hash.ToLowerInvariant() -cne $candidate.($pair[1])) { throw 'Candidate input changed.' }
    }
    $product = Get-Content -LiteralPath $candidate.productReceiptPath -Raw -Encoding UTF8 | ConvertFrom-Json
    $bundle = Get-Content -LiteralPath $candidate.shaderExportPath -Raw -Encoding UTF8 | ConvertFrom-Json
    $root = [IO.Path]::GetDirectoryName($candidate.shaderExportPath)
    if ($bundle.scope -cne 'registered-catalog' -or @($bundle.files).Count -eq 0 -or @($bundle.files).Count -gt 65536) { throw 'Shader export membership is incomplete.' }
    $names = @{}
    foreach ($entry in $bundle.files)
    {
        Assert-PackagePath $entry.path
        if ($names.ContainsKey($entry.path) -or $entry.sha256 -cnotmatch '^[a-f0-9]{64}$') { throw 'Invalid shader receipt.' }
        $names[$entry.path] = $true
        $file = Get-IncidentFile (Join-Path $root $entry.path) 1073741824
        if ($file.Length -ne $entry.bytes -or (Get-FileHash -LiteralPath $file.FullName).Hash.ToLowerInvariant() -cne $entry.sha256) { throw 'Shader bundle changed.' }
    }
    if (!$names.ContainsKey('product-receipt.json')) { throw 'Compiler source identity is absent.' }
    $compiler = Get-Content -LiteralPath (Join-Path $root 'product-receipt.json') -Raw -Encoding UTF8 | ConvertFrom-Json
    foreach ($key in @('revision','sourceInputsSha256','dirtyPatchSha256'))
    {
        if ($candidate.$key -cne $product.$key -or $candidate.$key -cne $compiler.$key) { throw 'Source candidate mismatch.' }
    }
    if ($product.executableSha256 -cne $Incident.executableSha256 -or $candidate.cpuPdbGuid -cne $product.cpuPdbGuid -or
        $candidate.cpuPdbAge -ne $product.cpuPdbAge -or $candidate.globalShaderMapSha256 -cne $bundle.globalShaderMapSha256 -or
        $candidate.cookedShaderLibrarySha256 -cne $bundle.cookedShaderLibrarySha256) { throw 'Product/cook/symbol identity mismatch.' }
    $nativeId = Get-NativeSymbolIdentity $candidate.cpuPdbPath
    if ($nativeId.Guid -cne $candidate.cpuPdbGuid -or $nativeId.Age -ne $candidate.cpuPdbAge) { throw 'CPU symbol identity mismatch.' }
    return $bundle
}

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
            $pdb = Get-IncidentFile $incident.candidate.cpuPdbPath 1073741824
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
        Assert-PackagePath $file.Path
        [void] (Get-IncidentFile $file.Source 1073741824)
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
