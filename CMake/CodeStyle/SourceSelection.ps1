Set-StrictMode -Version Latest

function Get-SelectedSourcePaths
{
    param([string] $RepositoryRoot, [string] $SourceFamily, [switch] $Staged, [string] $Path)

    if ($Staged -and -not [string]::IsNullOrWhiteSpace($Path))
    {
        throw "Use either -Staged or -Path, not both."
    }

    if (-not [string]::IsNullOrWhiteSpace($Path))
    {
        $absolutePath = [IO.Path]::GetFullPath((Join-Path $RepositoryRoot $Path))
        $rootPrefix = $RepositoryRoot + [IO.Path]::DirectorySeparatorChar
        if (-not $absolutePath.StartsWith($rootPrefix, [StringComparison]::OrdinalIgnoreCase))
        {
            throw "-Path must identify an owned source file inside the repository."
        }

        $selectedPaths = @($absolutePath.Substring($rootPrefix.Length).Replace('\', '/'))
    }
    elseif ($Staged)
    {
        $selectedPaths = @(& git -C $RepositoryRoot -c core.quotepath=false diff --cached --name-only --diff-filter=ACMR -- CMake Engine Tools Projects)
        if ($LASTEXITCODE -ne 0) { throw "Failed to enumerate staged source files." }
    }
    else
    {
        $selectedPaths = @(& git -C $RepositoryRoot -c core.quotepath=false ls-files --cached --others --exclude-standard -- CMake Engine Tools Projects)
        if ($LASTEXITCODE -ne 0) { throw "Failed to enumerate repository source files." }
    }

    foreach ($relativePath in @($selectedPaths | Sort-Object -Unique))
    {
        $extension = [IO.Path]::GetExtension($relativePath)
        if ($extension -notin @('.c', '.cc', '.cpp', '.cxx', '.h', '.hh', '.hpp', '.hxx', '.hlsl', '.hlsli') -or
            $relativePath -notmatch '^(CMake|Engine|Tools|Projects)/')
        {
            continue
        }

        $isShader = $extension -in @('.hlsl', '.hlsli')
        if (($SourceFamily -eq 'Shaders' -and -not $isShader) -or ($SourceFamily -eq 'Cpp' -and $isShader))
        {
            continue
        }

        if (Test-Path -LiteralPath (Join-Path $RepositoryRoot $relativePath) -PathType Leaf)
        {
            $relativePath
        }
    }
}
