[CmdletBinding()]
param(
    [ValidateSet("Check", "Format")]
    [string] $Mode = "Check",
    [ValidateSet("All", "Cpp", "Shaders")]
    [string] $SourceFamily = "All",
    [switch] $Staged,
    [string] $Path = "",
    [string] $ManifestPath = "",
    [string] $ClangFormatPath = "",
    [string] $ClangTidyPath = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot 'CodeStyle/SourceSelection.ps1')
. (Join-Path $PSScriptRoot 'CodeStyle/ClangTools.ps1')
. (Join-Path $PSScriptRoot 'CodeStyle/SourceFormatting.ps1')
. (Join-Path $PSScriptRoot 'CodeStyle/SourcePolicy.ps1')

$repositoryRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$requiredClangFormatVersion = '22.1.3'
$selection = @{ RepositoryRoot = $repositoryRoot; SourceFamily = $SourceFamily; Staged = $Staged; Path = $Path }
$sourceEntries = @(Get-SourceEntries @selection)
$ownedSourcePaths = @($sourceEntries | Where-Object Selected | ForEach-Object Path)
Write-SourceManifest -Entries $sourceEntries @selection -Mode $Mode -ManifestPath $ManifestPath

if (-not [string]::IsNullOrWhiteSpace($Path))
{
    if ($ownedSourcePaths.Count -ne 1)
    {
        throw "-Path must identify an existing owned source file matching -SourceFamily."
    }
}

if ($ownedSourcePaths.Count -eq 0)
{
    if ($Staged)
    {
        Write-Host "No staged owned source files match the selected source family."
        exit 0
    }

    throw "The owned-source manifest is empty."
}

$clangFormat = Resolve-ClangTool -Name clang-format -ConfiguredPath $ClangFormatPath -RequiredVersion $requiredClangFormatVersion
$clangTidy = $null
if ($Mode -eq 'Check' -and $SourceFamily -ne 'Shaders')
{
    $clangTidy = Resolve-ClangTool -Name clang-tidy -ConfiguredPath $ClangTidyPath -RequiredVersion $requiredClangFormatVersion
    Push-Location $repositoryRoot
    try
    {
        & $clangTidy --verify-config
        if ($LASTEXITCODE -ne 0) { throw ".clang-tidy is invalid for clang-tidy $requiredClangFormatVersion." }
    }
    finally { Pop-Location }
}

$formatFailed = Invoke-SourceFormatting -RepositoryRoot $repositoryRoot -Mode $Mode -Paths $ownedSourcePaths -ClangFormat $clangFormat
$policyViolations = @(Get-SourcePolicyViolations -RepositoryRoot $repositoryRoot -Paths $ownedSourcePaths)

foreach ($violation in $policyViolations)
{
    [Console]::Error.WriteLine($violation)
}

if ($formatFailed -or $policyViolations.Count -gt 0)
{
    throw "Code-style $($Mode.ToLowerInvariant()) failed: formatterFailure=$formatFailed; policyViolations=$($policyViolations.Count)."
}

$finalSourcePaths = @(Get-SourceEntries @selection | Where-Object Selected | ForEach-Object Path)
if (@(Compare-Object -ReferenceObject $ownedSourcePaths -DifferenceObject $finalSourcePaths).Count -ne 0)
{
    throw "The selected source manifest changed during execution. Rerun against the current file set."
}

$semanticPolicySummary = if ($null -ne $clangTidy) { "; .clang-tidy configuration verified" } else { "" }
Write-Host "Code-style $($Mode.ToLowerInvariant()) passed for $($ownedSourcePaths.Count) owned $($SourceFamily.ToLowerInvariant()) files with clang-format $requiredClangFormatVersion$semanticPolicySummary."
