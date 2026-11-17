Set-StrictMode -Version Latest

function Test-TopLevelComma
{
    param([string] $Text)

    $angleDepth = 0
    $parenthesisDepth = 0
    $bracketDepth = 0
    foreach ($character in $Text.ToCharArray())
    {
        switch ($character)
        {
            "<" { ++$angleDepth; continue }
            ">" { if ($angleDepth -gt 0) { --$angleDepth }; continue }
            "(" { ++$parenthesisDepth; continue }
            ")" { if ($parenthesisDepth -gt 0) { --$parenthesisDepth }; continue }
            "[" { ++$bracketDepth; continue }
            "]" { if ($bracketDepth -gt 0) { --$bracketDepth }; continue }
            ","
            {
                if ($angleDepth -eq 0 -and $parenthesisDepth -eq 0 -and $bracketDepth -eq 0)
                {
                    return $true
                }
            }
        }
    }

    return $false
}

function Get-SourcePolicyViolations
{
    param([string] $RepositoryRoot, [string[]] $Paths)

    Add-Type -Path (Join-Path $PSScriptRoot "CodeStyleDecisions.cs")
    $policyViolations = [System.Collections.Generic.List[string]]::new()
    $multipleInheritancePattern = [regex]::new(
        "\b(?:class|struct)\s+[^:;{}]+\s*:\s*(?<Bases>[^;{}]+)\{",
        [Text.RegularExpressions.RegexOptions]::Multiline -bor [Text.RegularExpressions.RegexOptions]::Singleline)

    foreach ($relativePath in $Paths)
    {
        $absolutePath = Join-Path $repositoryRoot ($relativePath -replace "/", [IO.Path]::DirectorySeparatorChar)
        $sourceText = [IO.File]::ReadAllText($absolutePath)
        foreach ($lineNumber in [CodeStyleDecisions]::FindIfViolations($sourceText))
        {
            $policyViolations.Add("${relativePath}:${lineNumber}: long call-bearing if conditions must resolve named local results")
        }
        foreach ($lineNumber in [CodeStyleDecisions]::FindBooleanBudgetViolations($sourceText))
        {
            $policyViolations.Add("${relativePath}:${lineNumber}: boolean decisions allow at most two atomic checks; use small named expressions")
        }
        foreach ($lineNumber in [CodeStyleDecisions]::FindBooleanNameViolations($sourceText))
        {
            $policyViolations.Add("${relativePath}:${lineNumber}: boolean variables require bUpperCamelCase, after any existing scope prefix")
        }
        $lines = $sourceText.Replace("`r`n", "`n").Replace("`r", "`n").Split("`n")
        for ($lineIndex = 0; $lineIndex -lt $lines.Count; ++$lineIndex)
        {
            $line = $lines[$lineIndex]
            if ($line -match "^\s*}\s*//\s*namespace(?:\s+\S+)?\s*$")
            {
                $policyViolations.Add("${relativePath}:$($lineIndex + 1): namespace-end comments are prohibited")
            }
            if ([IO.Path]::GetExtension($relativePath) -in @(".hlsl", ".hlsli") -and $line -match "^\s*\[[^\]]+\]\s+\S")
            {
                $policyViolations.Add("${relativePath}:$($lineIndex + 1): shader attributes must occupy their own line")
            }
        }

        if ([IO.Path]::GetExtension($relativePath) -notin @(".hlsl", ".hlsli"))
        {
            $searchableText = [regex]::Replace($sourceText, "/\*.*?\*/", " ", [Text.RegularExpressions.RegexOptions]::Singleline)
            $searchableText = [regex]::Replace($searchableText, "//.*$", "", [Text.RegularExpressions.RegexOptions]::Multiline)
            foreach ($match in [regex]::Matches($searchableText, "\bnamespace\s*\{"))
            {
                $lineNumber = [regex]::Matches($searchableText.Substring(0, $match.Index), "\n").Count + 1
                $policyViolations.Add("${relativePath}:${lineNumber}: anonymous namespaces are prohibited")
            }
            foreach ($match in $multipleInheritancePattern.Matches($searchableText))
            {
                if (Test-TopLevelComma $match.Groups["Bases"].Value)
                {
                    $lineNumber = [regex]::Matches($searchableText.Substring(0, $match.Index), "\n").Count + 1
                    $policyViolations.Add("${relativePath}:${lineNumber}: owned classes and structs may have at most one direct base")
                }
            }
        }
    }

    return $policyViolations
}
