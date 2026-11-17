Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot 'ArtifactIdentity.ps1')

function Get-ShaderExport
{
    param([string] $Path)
    $file = Get-LocalArtifactFile $Path 33554432
    $export = Get-Content -LiteralPath $file.FullName -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($export.scope -cne 'registered-catalog' -or !@($export.jobs).Count -or
        !@($export.files).Count -or @($export.files).Count -gt 65536)
    {
        throw 'Shader export membership is incomplete.'
    }
    $paths = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    foreach ($entry in $export.files)
    {
        if (!$paths.Add($entry.path) -or $entry.bytes -lt 0 -or $entry.bytes -gt 1073741824 -or
            $entry.sha256 -cnotmatch '^[a-f0-9]{64}$') { throw 'Invalid or duplicate shader export member.' }
        [void] (Assert-ArtifactFile $file.DirectoryName $entry.path $entry.bytes $entry.sha256)
    }
    foreach ($job in $export.jobs)
    {
        if (!$paths.Contains($job)) { throw 'Shader job is absent from its export receipt.' }
    }
    return $export
}
