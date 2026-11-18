Set-StrictMode -Version Latest

function ConvertTo-CanonicalShaderText
{
    param([string] $Text)

    $canonicalText = $Text.Replace("`r`n", "`n").Replace("`r", "`n")
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

function Invoke-SourceFormatting
{
    param([string] $RepositoryRoot, [string] $Mode, [string[]] $Paths, [string] $ClangFormat)

    $formatFailed = $false
    foreach ($relativePath in $Paths)
    {
        $absolutePath = Join-Path $RepositoryRoot $relativePath
        # Compare formatted text: clang-format 22 can report no-op replacements in --dry-run.
        $formattedLines = @(& $ClangFormat --Werror --style=file $absolutePath)
        if ($LASTEXITCODE -ne 0)
        {
            $formatFailed = $true
            continue
        }

        $canonicalText = ($formattedLines -join "`n") + "`n"
        if ([IO.Path]::GetExtension($relativePath) -in @('.hlsl', '.hlsli'))
        {
            # clang-format uses its C++ parser for HLSL; restore own-line shader attributes.
            $canonicalText = ConvertTo-CanonicalShaderText -Text $canonicalText
        }
        $currentText = ([IO.File]::ReadAllText($absolutePath)).Replace("`r`n", "`n").Replace("`r", "`n")
        if ($currentText -ceq $canonicalText) { continue }

        if ($Mode -eq 'Format')
        {
            [IO.File]::WriteAllText($absolutePath, $canonicalText, [Text.UTF8Encoding]::new($false))
        }
        else
        {
            [Console]::Error.WriteLine("${relativePath}: code should be formatted")
            $formatFailed = $true
        }
    }
    return $formatFailed
}
