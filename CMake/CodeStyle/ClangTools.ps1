Set-StrictMode -Version Latest

function Resolve-ClangFormat
{
    param(
        [string] $ConfiguredPath,
        [string] $RequiredVersion
    )

    $candidates = [System.Collections.Generic.List[string]]::new()
    if (-not [string]::IsNullOrWhiteSpace($ConfiguredPath))
    {
        $candidates.Add($ConfiguredPath)
    }

    $environmentVariable = 'SPARKLE_CLANG_FORMAT'
    $environmentPath = [Environment]::GetEnvironmentVariable($environmentVariable)
    if (-not [string]::IsNullOrWhiteSpace($environmentPath))
    {
        $candidates.Add($environmentPath)
    }

    $pathCommand = Get-Command clang-format -CommandType Application -ErrorAction SilentlyContinue
    if ($null -ne $pathCommand)
    {
        $candidates.Add($pathCommand.Source)
    }

    foreach ($visualStudioVersion in @("18", "17"))
    {
        foreach ($visualStudioEdition in @("Community", "Professional", "Enterprise", "BuildTools"))
        {
            $candidates.Add(
                "C:\Program Files\Microsoft Visual Studio\$visualStudioVersion\$visualStudioEdition\VC\Tools\Llvm\x64\bin\clang-format.exe")
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
                throw "Expected clang-format $RequiredVersion, got '$version'."
            }

            return $executable
        }
    }

    throw "clang-format $RequiredVersion was not found. Pass its configured path or set $environmentVariable."
}
