Set-StrictMode -Version Latest

function ConvertTo-CanonicalShaderText
{
    param([string] $Text)

    $canonicalText = $Text.Replace("`r`n", "`n").Replace("`r", "`n")
    $canonicalText = [regex]::Replace(
        $canonicalText,
        '^(?<Indent>[\t ]*)}\s*//\s*namespace(?:\s+\S+)?\s*$',
        '${Indent}}',
        [Text.RegularExpressions.RegexOptions]::Multiline)
    $canonicalText = [regex]::Replace(
        $canonicalText,
        '^[\t ]*(?<Attribute>\[(?:shader|numthreads)[^\]\r\n]*\])[\t ]+(?<Code>\S[^\r\n]*)$',
        '${Attribute}' + "`n" + '${Code}',
        [Text.RegularExpressions.RegexOptions]::Multiline)
    $canonicalText = [regex]::Replace(
        $canonicalText,
        '^(?<Indent>[\t ]*)(?<Attribute>\[[^\]\r\n]+\])[\t ]+(?<Code>\S[^\r\n]*)$',
        '${Indent}${Attribute}' + "`n" + '${Indent}${Code}',
        [Text.RegularExpressions.RegexOptions]::Multiline)

    $lines = $canonicalText.Split([string[]] @("`n"), [StringSplitOptions]::None)
    for ($lineIndex = 0; $lineIndex -lt $lines.Count; ++$lineIndex)
    {
        if ($lines[$lineIndex] -notmatch '^\[(?:shader|numthreads)')
        {
            continue
        }

        $signatureLineIndex = $lineIndex + 1
        if ($signatureLineIndex -ge $lines.Count)
        {
            continue
        }

        $lines[$signatureLineIndex] = $lines[$signatureLineIndex].TrimStart()
        for (++$signatureLineIndex; $signatureLineIndex -lt $lines.Count -and $lines[$signatureLineIndex].Trim() -ne "{"; ++$signatureLineIndex)
        {
            $lines[$signatureLineIndex] = "`t" + $lines[$signatureLineIndex].TrimStart()
        }
    }

    $canonicalText = $lines -join "`n"

    return $canonicalText
}

function Test-ClangFormatChanges
{
    param([string] $RepositoryRoot, [string[]] $Paths, [string] $ReplacementXml)

    $documents = @([regex]::Split($ReplacementXml, '(?=<\?xml)') | Where-Object { -not [string]::IsNullOrWhiteSpace($_) })
    if ($documents.Count -ne $Paths.Count)
    {
        throw "clang-format returned an unexpected replacement-document count."
    }

    $changed = $false
    for ($fileIndex = 0; $fileIndex -lt $Paths.Count; ++$fileIndex)
    {
        $relativePath = $Paths[$fileIndex]
        $absolutePath = Join-Path $repositoryRoot $relativePath
        $bytes = [IO.File]::ReadAllBytes($absolutePath)
        $document = [xml] $documents[$fileIndex]
        if ($document.replacements.incomplete_format -eq "true")
        {
            throw "${relativePath}: clang-format could not complete formatting."
        }

        foreach ($replacement in $document.SelectNodes('/replacements/replacement'))
        {
            $offset = [int] $replacement.offset
            $length = [int] $replacement.length
            $existing = [Text.Encoding]::UTF8.GetString($bytes, $offset, $length)
            if ($existing -ceq $replacement.InnerText)
            {
                continue
            }

            [Console]::Error.WriteLine("${relativePath}: code should be clang-formatted")
            $changed = $true
            break
        }
    }

    return $changed
}

function Test-ControlFlowBraces
{
    param([string] $RepositoryRoot, [string[]] $Paths, [string] $ReplacementXml)

    $documents = @([regex]::Split($ReplacementXml, '(?=<\?xml)') | Where-Object { -not [string]::IsNullOrWhiteSpace($_) })
    if ($documents.Count -ne $Paths.Count)
    {
        throw "clang-format returned an unexpected brace-check document count."
    }

    $missingBraces = $false
    for ($fileIndex = 0; $fileIndex -lt $Paths.Count; ++$fileIndex)
    {
        $relativePath = $Paths[$fileIndex]
        $document = [xml] $documents[$fileIndex]
        if ($document.replacements.incomplete_format -eq "true")
        {
            throw "${relativePath}: clang-format could not complete the brace check."
        }

        foreach ($replacement in $document.SelectNodes('/replacements/replacement'))
        {
            if ($replacement.InnerText -notmatch '[{}]')
            {
                continue
            }

            $bytes = [IO.File]::ReadAllBytes((Join-Path $repositoryRoot $relativePath))
            $offset = [int] $replacement.offset
            $existing = [Text.Encoding]::UTF8.GetString($bytes, $offset, [int] $replacement.length)
            if ([regex]::Replace($existing, '\s', '') -ceq [regex]::Replace($replacement.InnerText, '\s', ''))
            {
                continue
            }

            $lineNumber = [Text.Encoding]::UTF8.GetString($bytes, 0, $offset).Split("`n").Length
            [Console]::Error.WriteLine("${relativePath}:${lineNumber}: control-flow bodies require explicit braces; repair the source before formatting")
            $missingBraces = $true
            break
        }
    }

    return $missingBraces
}

function Invoke-CppFormatting
{
    param([string] $RepositoryRoot, [string] $Mode, [string[]] $Paths, [string] $ClangFormat)

    $formatFailed = $false
    $braceCheckStylePath = $null
    try
    {
        if ($Mode -eq "Check" -and $Paths.Count -ne 0)
        {
            $resolvedStyle = @(& $clangFormat --style=file --dump-config)
            if ($LASTEXITCODE -ne 0)
            {
                throw "clang-format could not resolve the repository style for the brace check."
            }

            $braceCheckStyle = [regex]::Replace(($resolvedStyle -join "`n"), '(?m)^InsertBraces:[^\r\n]*', 'InsertBraces: true')
            $braceCheckStylePath = [IO.Path]::GetTempFileName()
            [IO.File]::WriteAllText($braceCheckStylePath, $braceCheckStyle, [Text.UTF8Encoding]::new($false))
        }

        for ($offset = 0; $offset -lt $Paths.Count; $offset += 64)
        {
            $lastIndex = [Math]::Min($offset + 63, $Paths.Count - 1)
            $batch = $Paths[$offset..$lastIndex]
            Write-Host "C/C++ formatter: $($lastIndex + 1)/$($Paths.Count) files."
            $arguments = if ($Mode -eq "Check")
            {
                @("--output-replacements-xml", "--Werror", "--style=file") + $batch
            }
            else
            {
                @("-i", "--Werror", "--style=file") + $batch
            }

            if ($Mode -eq "Check")
            {
                $replacementLines = @(& $clangFormat $arguments)
                if ($LASTEXITCODE -ne 0)
                {
                    $formatFailed = $true
                }
                elseif (Test-ClangFormatChanges -RepositoryRoot $RepositoryRoot -Paths $batch -ReplacementXml ($replacementLines -join "`n"))
                {
                    $formatFailed = $true
                }

                $braceLines = @(& $clangFormat --output-replacements-xml --Werror "--style=file:$braceCheckStylePath" $batch)
                if ($LASTEXITCODE -ne 0)
                {
                    $formatFailed = $true
                }
                elseif (Test-ControlFlowBraces -RepositoryRoot $RepositoryRoot -Paths $batch -ReplacementXml ($braceLines -join "`n"))
                {
                    $formatFailed = $true
                }
            }
            else
            {
                & $clangFormat $arguments
                if ($LASTEXITCODE -ne 0)
                {
                    $formatFailed = $true
                }
            }
        }
    }
    finally
    {
        if ($null -ne $braceCheckStylePath)
        {
            Remove-Item -LiteralPath $braceCheckStylePath -Force
        }
    }

    return $formatFailed
}

function Invoke-ParagraphFormatting
{
    param([string] $RepositoryRoot, [string] $Mode, [string[]] $Paths)

    $formatFailed = $false
    $utf8WithoutBom = [Text.UTF8Encoding]::new($false)
    foreach ($relativePath in $Paths)
    {
        $absolutePath = Join-Path $repositoryRoot ($relativePath -replace "/", [IO.Path]::DirectorySeparatorChar)
        $currentText = [IO.File]::ReadAllText($absolutePath)
        $canonicalText = [CodeStyleParagraphSpacing]::Format($currentText)
        if ($currentText.Replace("`r`n", "`n").Replace("`r", "`n") -ceq $canonicalText)
        {
            continue
        }

        if ($Mode -eq "Format")
        {
            [IO.File]::WriteAllText($absolutePath, $canonicalText, $utf8WithoutBom)
        }
        else
        {
            [Console]::Error.WriteLine("${relativePath}: multiline statements require blank paragraph boundaries")
            $formatFailed = $true
        }
    }

    return $formatFailed
}

function Invoke-ShaderFormatting
{
    param([string] $RepositoryRoot, [string] $Mode, [string[]] $Paths, [string] $ClangFormat)

    $formatFailed = $false
    $utf8WithoutBom = [Text.UTF8Encoding]::new($false)
    $shaderIndex = 0
    foreach ($relativePath in $Paths)
    {
        ++$shaderIndex
        if ($shaderIndex % 32 -eq 0 -or $shaderIndex -eq $Paths.Count)
        {
            Write-Host "Shader formatter: $shaderIndex/$($Paths.Count) files."
        }

        $absolutePath = Join-Path $repositoryRoot ($relativePath -replace "/", [IO.Path]::DirectorySeparatorChar)
        $formattedLines = @(& $clangFormat --Werror --style=file $absolutePath)
        if ($LASTEXITCODE -ne 0)
        {
            $formatFailed = $true
            continue
        }

        $formattedText = ($formattedLines -join "`n") + "`n"
        $canonicalText = ConvertTo-CanonicalShaderText -Text $formattedText
        $canonicalText = [CodeStyleParagraphSpacing]::Format($canonicalText)
        $currentText = ([IO.File]::ReadAllText($absolutePath)).Replace("`r`n", "`n").Replace("`r", "`n")
        if ($currentText -ceq $canonicalText)
        {
            continue
        }

        if ($Mode -eq "Format")
        {
            [IO.File]::WriteAllText($absolutePath, $canonicalText, $utf8WithoutBom)
        }
        else
        {
            [Console]::Error.WriteLine("${relativePath}: code should be formatted with the canonical shader policy")
            $formatFailed = $true
        }
    }

    return $formatFailed
}

function Invoke-SourceFormatting
{
    param([string] $RepositoryRoot, [string] $Mode, [string[]] $Paths, [string] $ClangFormat)

    $cppPaths = @($Paths | Where-Object { [IO.Path]::GetExtension($_) -notin @('.hlsl', '.hlsli') })
    $shaderPaths = @($Paths | Where-Object { [IO.Path]::GetExtension($_) -in @('.hlsl', '.hlsli') })
    Add-Type -Path (Join-Path $PSScriptRoot 'CodeStyleParagraphSpacing.cs')

    Push-Location $RepositoryRoot
    try
    {
        $cppFailed = Invoke-CppFormatting $RepositoryRoot $Mode $cppPaths $ClangFormat
        $paragraphFailed = Invoke-ParagraphFormatting $RepositoryRoot $Mode $cppPaths
        $shaderFailed = Invoke-ShaderFormatting $RepositoryRoot $Mode $shaderPaths $ClangFormat
        return $cppFailed -or $paragraphFailed -or $shaderFailed
    }
    finally { Pop-Location }
}
