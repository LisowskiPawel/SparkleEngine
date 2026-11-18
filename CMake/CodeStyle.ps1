[CmdletBinding()]
param(
    [ValidateSet("Check", "Format")]
    [string] $Mode = "Check",
    [ValidateSet("All", "Cpp", "Shaders")]
    [string] $SourceFamily = "All",
    [switch] $Staged,
    [string] $Path = "",
    [string] $ClangFormatPath = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot 'CodeStyle/SourceSelection.ps1')
. (Join-Path $PSScriptRoot 'CodeStyle/ClangTools.ps1')
. (Join-Path $PSScriptRoot 'CodeStyle/SourceFormatting.ps1')

$repositoryRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$requiredClangFormatVersion = '22.1.3'
$selection = @{ RepositoryRoot = $repositoryRoot; SourceFamily = $SourceFamily; Staged = $Staged; Path = $Path }
$ownedSourcePaths = @(Get-SelectedSourcePaths @selection)

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

    throw "No owned source files match the selection."
}

$clangFormat = Resolve-ClangFormat -ConfiguredPath $ClangFormatPath -RequiredVersion $requiredClangFormatVersion
$formatFailed = Invoke-SourceFormatting -RepositoryRoot $repositoryRoot -Mode $Mode -Paths $ownedSourcePaths -ClangFormat $clangFormat
if ($formatFailed)
{
    throw "Code-style $($Mode.ToLowerInvariant()) failed."
}

Write-Host "Code-style $($Mode.ToLowerInvariant()) passed for $($ownedSourcePaths.Count) files with clang-format $requiredClangFormatVersion."
