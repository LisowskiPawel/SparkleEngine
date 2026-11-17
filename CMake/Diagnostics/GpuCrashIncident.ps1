Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot '../Artifacts/NativeSymbols.ps1')

function Get-GpuCrashIncident
{
    param([string] $ManifestPath, [string] $ExpectedSessionId = '', [uint32] $ExpectedChildPid = 0)
    $file = Get-LocalArtifactFile $ManifestPath 33554432
    if ($file.Name -cne 'incident.json') { throw 'Choose the exact incident manifest.' }
    $incident = Get-Content -LiteralPath $file.FullName -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($incident.incidentId -cnotmatch '^[a-f0-9]{8}-[a-f0-9]{4}-4[a-f0-9]{3}-[89ab][a-f0-9]{3}-[a-f0-9]{12}$' -or
        $incident.childPid -le 0 -or [string] $incident.nativeFailure -cnotmatch '^-?[0-9]{1,20}$' -or $incident.state -cnotin @('Preparing','Ready','Collecting','Collected','Partial','Timed out','Unavailable') -or
        $incident.sourceMapping -cne 'unqualified' -or $incident.executableSha256 -cnotmatch '^[a-f0-9]{64}$')
    {
        throw 'Incomplete GPU crash session identity.'
    }
    [void] (Get-LocalArtifactFile (Join-Path $file.DirectoryName 'owner.txt') 36)
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
        $payload = Get-LocalArtifactFile (Join-Path $file.DirectoryName $entry.path) 1073741824
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
        [void] (Get-LocalArtifactFile $candidate.($pair[0]) $(if ($pair[0] -eq 'cpuPdbPath') { [long]::MaxValue } else { 33554432 }))
        if ((Get-FileHash -LiteralPath $candidate.($pair[0])).Hash.ToLowerInvariant() -cne $candidate.($pair[1])) { throw 'Candidate input changed.' }
    }
    $product = Get-Content -LiteralPath $candidate.productReceiptPath -Raw -Encoding UTF8 | ConvertFrom-Json
    $bundle = Get-Content -LiteralPath $candidate.shaderExportPath -Raw -Encoding UTF8 | ConvertFrom-Json
    $root = [IO.Path]::GetDirectoryName($candidate.shaderExportPath)
    if ($bundle.scope -cne 'registered-catalog' -or @($bundle.files).Count -eq 0 -or @($bundle.files).Count -gt 65536) { throw 'Shader export membership is incomplete.' }
    $names = @{}
    foreach ($entry in $bundle.files)
    {
        Assert-ArtifactPath $entry.path
        if ($names.ContainsKey($entry.path) -or $entry.sha256 -cnotmatch '^[a-f0-9]{64}$') { throw 'Invalid shader receipt.' }
        $names[$entry.path] = $true
        $file = Get-LocalArtifactFile (Join-Path $root $entry.path) 1073741824
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
