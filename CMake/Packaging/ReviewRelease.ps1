[CmdletBinding()]
param(
    [Parameter(Mandatory)][string] $PackageRoot,
    [Parameter(Mandatory)][string] $ArchivePath,
    [Parameter(Mandatory)][string] $ArchiveSha256,
    [Parameter(Mandatory)][string] $LedgerPath,
    [Parameter(Mandatory)][ValidatePattern('^[\p{L}\p{Nd}][\p{L}\p{Nd} ._-]{0,127}$')][string] $Reviewer,
    [Parameter(Mandatory)][ValidateSet('Approve','Replace','Withdraw')][string] $Decision,
    [Parameter(Mandatory)][string] $OutputDirectory,
    [string] $ReplacementRoot = '',
    [string] $ReplacementArchive = '',
    [string] $ReplacementSha256 = '',
    [string] $ReplacementLedgerPath = '',
    [ValidateSet('PreserveSharedSettings','ManualResetAfterBackup')][string] $StatePolicy = 'PreserveSharedSettings'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'IncidentIdentity.ps1')

function Read-ReleaseDefects
{
    param([string] $Path, [string] $Sha256)
    [void] (Get-IncidentFile $Path 1048576)
    $ledger = Get-Content -LiteralPath $Path -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($ledger.ArchiveSha256 -cne $Sha256) { throw 'Known issues belong to another archive.' }
    $ids = [Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
    $blocking = @()
    $defects = @(foreach ($defect in $ledger.Defects)
    {
        if ($defect.Id -cnotmatch '^ISSUE-[0-9]{1,8}$' -or !$ids.Add($defect.Id) -or
            $defect.Severity -notin @('S0','S1','S2','S3') -or $defect.Disposition -notin @('Unresolved','Resolved'))
        {
            throw 'Known-issue classification is invalid.'
        }
        if ($defect.Disposition -eq 'Resolved')
        {
            [void] (Get-IncidentFile $defect.EvidencePath 1048576)
            $receipt = Get-Content -LiteralPath $defect.EvidencePath -Raw -Encoding UTF8 | ConvertFrom-Json
            if ($receipt.Status -cne 'PASS' -or $receipt.ArchiveSha256 -cne $Sha256 -or $receipt.DefectId -cne $defect.Id)
            {
                throw 'Resolved defect has no matching candidate check receipt.'
            }
            $evidence = (Get-FileHash -LiteralPath $defect.EvidencePath).Hash.ToLowerInvariant()
        }
        else
        {
            $evidence = $null
            if ($defect.Severity -in @('S0','S1')) { $blocking += $defect.Id }
        }
        [ordered]@{ Id = $defect.Id; Severity = $defect.Severity; Disposition = $defect.Disposition; EvidenceSha256 = $evidence }
    })
    return @{ Defects = $defects; Blocking = $blocking; LedgerSha256 = (Get-FileHash -LiteralPath $Path).Hash.ToLowerInvariant() }
}

try
{
    $manifest = Get-IncidentPackage $PackageRoot $ArchivePath $ArchiveSha256
    $review = Read-ReleaseDefects $LedgerPath $ArchiveSha256
    $replacement = $null
    $nextReview = $null
    if ($Decision -eq 'Replace')
    {
        $next = Get-IncidentPackage $ReplacementRoot $ReplacementArchive $ReplacementSha256
        $nextReview = Read-ReleaseDefects $ReplacementLedgerPath $ReplacementSha256
        if ($next.Version -ceq $manifest.Version -or $ReplacementSha256 -ceq $ArchiveSha256) { throw 'Replacement reuses immutable identity.' }
        foreach ($defect in $review.Defects)
        {
            $retained = @($nextReview.Defects | Where-Object Id -ceq $defect.Id)
            if ($retained.Count -ne 1 -or $retained[0].Severity -cne $defect.Severity)
            {
                throw 'Replacement review drops or reclassifies a known defect; resolve it with candidate evidence.'
            }
        }
        foreach ($component in @('Launcher','Editor','Game'))
        {
            $old = @($manifest.Products | Where-Object Component -ceq $component)
            $new = @($next.Products | Where-Object Component -ceq $component)
            if ($old.Count -ne 1 -or $new.Count -ne 1 -or $old[0].Configuration -cne $new[0].Configuration)
            {
                throw 'Replacement product scope/configuration differs; require a new applicability review.'
            }
        }
        $replacement = @{ Version = $next.Version; ArchiveSha256 = $ReplacementSha256; Revision = $next.Revision; SourceInputsSha256 = $next.SourceInputsSha256 }
    }
    $blocking = @(if ($Decision -eq 'Replace') { $nextReview.Blocking } else { $review.Blocking })
    $status = if ($blocking.Count -gt 0 -and $Decision -ne 'Withdraw') { 'BLOCKED' } else { 'PASS' }
    $record = [ordered]@{
        Reviewer = $Reviewer; Version = $manifest.Version; Revision = $manifest.Revision
        SourceInputsSha256 = $manifest.SourceInputsSha256; ArchiveSha256 = $ArchiveSha256
        LedgerSha256 = $review.LedgerSha256
        ReviewedUtc = [DateTime]::UtcNow.ToString('o'); Decision = $Decision; Status = $status
        Defects = $review.Defects; BlockingDefects = $blocking; Replacement = $replacement; ReplacementReview = $nextReview; StatePolicy = $StatePolicy
        InvalidatedReports = $(if ($Decision -in @('Replace','Withdraw')) { @('FCR-PROD-01','FCR-PROD-02','FCR-PROD-03','FCR-PROD-04','FCR-PROD-05','FCR-PROD-06','REL-03','REL-07','REL-08','REL-09','REL-11') } else { @() })
        Publication = 'NOT_PERFORMED'; Rollback = 'Re-extract previously verified archive; shared settings need a separate backup'
    }
    foreach ($rootPath in @($PackageRoot, $ReplacementRoot) | Where-Object { $_ -ne '' })
    {
        $root = [IO.Path]::GetFullPath($rootPath).TrimEnd('\','/')
        $output = [IO.Path]::GetFullPath($OutputDirectory).TrimEnd('\','/')
        if ($output -eq $root -or $output.StartsWith($root + '\', [StringComparison]::OrdinalIgnoreCase)) { throw 'Review output must be outside product bytes.' }
    }
    [void] (Get-IncidentPackage $PackageRoot $ArchivePath $ArchiveSha256)
    if ($Decision -eq 'Replace') { [void] (Get-IncidentPackage $ReplacementRoot $ReplacementArchive $ReplacementSha256) }
    Write-IncidentRecord $OutputDirectory ($record | ConvertTo-Json -Depth 10) 'sparkle-release-review'
    if ($status -eq 'BLOCKED') { exit 1 }
}
catch
{
    [Console]::Error.WriteLine('Release review rejected. Check exact archive identities, known-issue receipts, replacement version/scope and output location. No product, user state or public release was changed.')
    exit 1
}
