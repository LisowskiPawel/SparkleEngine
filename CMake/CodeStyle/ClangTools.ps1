Set-StrictMode -Version Latest

function Resolve-ClangTool
{
    param(
        [ValidateSet('clang-format', 'clang-tidy')][string] $Name,
        [string] $ConfiguredPath,
        [string] $RequiredVersion
    )

    $candidates = [System.Collections.Generic.List[string]]::new()
    if (-not [string]::IsNullOrWhiteSpace($ConfiguredPath))
    {
        $candidates.Add($ConfiguredPath)
    }

    $environmentVariable = "SPARKLE_" + $Name.Replace("-", "_").ToUpperInvariant()
    $environmentPath = [Environment]::GetEnvironmentVariable($environmentVariable)
    if (-not [string]::IsNullOrWhiteSpace($environmentPath))
    {
        $candidates.Add($environmentPath)
    }

    $pathCommand = Get-Command $Name -CommandType Application -ErrorAction SilentlyContinue
    if ($null -ne $pathCommand)
    {
        $candidates.Add($pathCommand.Source)
    }

    foreach ($visualStudioVersion in @("18", "17"))
    {
        foreach ($visualStudioEdition in @("Community", "Professional", "Enterprise", "BuildTools"))
        {
            $candidates.Add(
                "C:\Program Files\Microsoft Visual Studio\$visualStudioVersion\$visualStudioEdition\VC\Tools\Llvm\x64\bin\$Name.exe")
        }
    }

    foreach ($candidate in $candidates)
    {
        if (Test-Path -LiteralPath $candidate -PathType Leaf)
        {
            $executable = (Resolve-Path -LiteralPath $candidate).Path
            $version = (& $executable --version 2>&1 | Out-String).Trim()
            if ($LASTEXITCODE -ne 0 -or $version -notmatch "version $([regex]::Escape($RequiredVersion))(?:\s|$)")
            {
                throw "Expected $Name $RequiredVersion, got '$version'."
            }

            return $executable
        }
    }

    throw "$Name $RequiredVersion was not found. Pass its configured path or set $environmentVariable."
}
