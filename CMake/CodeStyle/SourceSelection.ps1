Set-StrictMode -Version Latest

function Get-SelectedSourcePaths
{
    param([string] $RepositoryRoot, [switch] $Staged, [string] $Path)

    if ($Staged -and -not [string]::IsNullOrWhiteSpace($Path))
    {
        throw "Use either -Staged or -Path, not both."
    }

    if (-not [string]::IsNullOrWhiteSpace($Path))
    {
        $absolutePath = [IO.Path]::GetFullPath((Join-Path $repositoryRoot $Path))
        $rootPrefix = $repositoryRoot + [IO.Path]::DirectorySeparatorChar
        if (-not $absolutePath.StartsWith($rootPrefix, [StringComparison]::OrdinalIgnoreCase))
        {
            throw "-Path must identify an owned source file inside the repository."
        }

        return $absolutePath.Substring($rootPrefix.Length).Replace('\', '/')
    }

    if ($Staged)
    {
        $selectedPaths = @(& git -C $repositoryRoot -c core.quotepath=false diff --cached --name-only --diff-filter=ACMR -- CMake Engine Tools Projects)
        if ($LASTEXITCODE -ne 0)
        {
            throw "Failed to enumerate staged source files."
        }

        return $selectedPaths
    }

    $selectedPaths = @(& git -C $repositoryRoot -c core.quotepath=false ls-files --cached --others --exclude-standard)
    if ($LASTEXITCODE -ne 0)
    {
        throw "Failed to enumerate tracked and untracked repository files."
    }

    return $selectedPaths
}

function Get-SourceExclusionReason
{
    param([string] $RepositoryRoot, [string] $SourceFamily, [string] $RelativePath)

    if ($RelativePath -notmatch '^(CMake|Engine|Tools|Projects)/')
    {
        return "Outside owned source roots"
    }

    if ($RelativePath.StartsWith("Engine/RHI/Private/D3D12/ThirdParty/", [StringComparison]::OrdinalIgnoreCase))
    {
        return "Third-party source"
    }

    $isShader = [IO.Path]::GetExtension($RelativePath) -in @(".hlsl", ".hlsli")
    if (($SourceFamily -eq "Shaders" -and -not $isShader) -or ($SourceFamily -eq "Cpp" -and $isShader))
    {
        return "Outside selected source family"
    }

    if (-not (Test-Path -LiteralPath (Join-Path $repositoryRoot $RelativePath) -PathType Leaf))
    {
        return "Missing working-tree file"
    }

    return ""
}

function Write-SourceManifest
{
    param(
        [object[]] $Entries,
        [string] $RepositoryRoot,
        [string] $Mode,
        [string] $SourceFamily,
        [switch] $Staged,
        [string] $Path,
        [string] $ManifestPath
    )

    $selectedCount = @($Entries | Where-Object { $_.Selected }).Count
    $excludedCount = $Entries.Count - $selectedCount
    Write-Host "Source coverage: $selectedCount selected; $excludedCount excluded source candidates."
    $shaderCount = @($Entries | Where-Object { $_.Selected -and [IO.Path]::GetExtension($_.Path) -in @(".hlsl", ".hlsli") }).Count
    Write-Host "Selected families: $($selectedCount - $shaderCount) C/C++ and headers; $shaderCount shaders."
    $scope = if ($Staged) { "Staged paths, working-tree contents" } elseif ($Path) { "Single working-tree file" } else { "Tracked files and nonignored untracked files" }
    Write-Host "Selection scope: $scope."
    foreach ($group in @($Entries | Where-Object { -not $_.Selected } | Group-Object Reason))
    {
        Write-Host "Excluded $($group.Count): $($group.Name)."
    }

    if ([string]::IsNullOrWhiteSpace($ManifestPath))
    {
        return
    }

    $absoluteManifestPath = [IO.Path]::GetFullPath((Join-Path $repositoryRoot $ManifestPath))
    if ([IO.Path]::GetExtension($absoluteManifestPath) -ne ".json")
    {
        throw "-ManifestPath must identify a JSON output file."
    }

    $manifest = [ordered] @{
        Repository = $repositoryRoot
        Mode = $Mode
        Scope = $scope
        SourceFamily = $SourceFamily
        SelectedCount = $selectedCount
        ExcludedCount = $excludedCount
        Files = $Entries
        Limits = @("Git-ignored untracked files are excluded", "Only declared C/C++ and HLSL extensions are candidates", "Selection manifest is not a successful check result", "Authored design rules require review")
    }

    [void] [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($absoluteManifestPath))
    [IO.File]::WriteAllText($absoluteManifestPath, ($manifest | ConvertTo-Json -Depth 5), [Text.UTF8Encoding]::new($false))
    Write-Host "Source manifest: $absoluteManifestPath"
}

function Get-SourceEntries
{
    param([string] $RepositoryRoot, [string] $SourceFamily, [switch] $Staged, [string] $Path)

    $selectedSourcePaths = @(Get-SelectedSourcePaths -RepositoryRoot $RepositoryRoot -Staged:$Staged -Path $Path)
    return @(
        foreach ($relativePath in @($selectedSourcePaths | Sort-Object -Unique))
        {
            if ([IO.Path]::GetExtension($relativePath) -notin @(".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx", ".hlsl", ".hlsli"))
            {
                continue
            }

            $reason = Get-SourceExclusionReason -RepositoryRoot $RepositoryRoot -SourceFamily $SourceFamily -RelativePath $relativePath
            [pscustomobject] @{
                Path = $relativePath
                Selected = [string]::IsNullOrEmpty($reason)
                Reason = $reason
            }
        }
    )
}
