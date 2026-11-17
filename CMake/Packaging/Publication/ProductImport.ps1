Set-StrictMode -Version Latest

function Get-ProductReplacements
{
    param([string] $RepositoryRoot, [string] $Transaction, [string] $Stage, $Manifest, [string[]] $Products)

    $directories = @(foreach ($target in @($Products | Select-Object -Unique))
    {
        $product = @($manifest.Products | Where-Object Target -ceq $target)
        if ($product.Count -ne 1) { throw "Missing/duplicate product: '$target'." }
        $profile = if ($target -eq 'ShowcaseGame') { 'ShippingGame' } else { 'ShippingEditor' }
        $directory = switch ($target)
        {
            { $_ -in @('ShowcaseEditor','ShowcaseGame') } { "artifacts/dev/Projects/Showcase/Binaries/$profile" }
            default { "artifacts/dev/Tools/$target/Binaries/$profile" }
        }
        if ($product[0].Configuration -cne $profile -or $product[0].Path -cne "$directory/$target.exe")
        {
            throw "Product sync product differs from the canonical build layout: '$target'."
        }
        $directory
    })
    $directories += 'artifacts/dev/Projects/Showcase/Cooked'
    if (@($manifest.Files | Where-Object { $_.Path.StartsWith('artifacts/dev/Projects/Shared/Cooked/', [StringComparison]::Ordinal) }).Count)
    {
        $directories += 'artifacts/dev/Projects/Shared/Cooked'
    }
    foreach ($directory in @($directories | Select-Object -Unique))
    {
        $destination = Join-Path $RepositoryRoot $directory
        # Reject links before replacing an owned binary directory.
        $current = [IO.Path]::GetFullPath($destination)
        while ($current)
        {
            if ((Test-Path -LiteralPath $current) -and ((Get-Item -LiteralPath $current -Force).Attributes -band [IO.FileAttributes]::ReparsePoint))
            {
                throw 'Product sync output path contains a reparse point.'
            }
            $current = [IO.Path]::GetDirectoryName($current)
        }
        $incoming = Join-Path $stage $directory
        if (!(Test-Path -LiteralPath $incoming -PathType Container)) { throw "Missing product output directory: '$directory'." }
        foreach ($file in Get-ChildItem -LiteralPath $incoming -Recurse -File -Force) { $file.IsReadOnly = $false }
        [pscustomobject]@{ Destination = $destination; Incoming = $incoming; Previous = (Join-Path $transaction "previous/$directory"); MovedPrevious = $false; Published = $false }
    }
}

function Publish-ProductReplacements
{
    param($Replacements)

    foreach ($replacement in $replacements)
    {
        [void] (New-Item -ItemType Directory -Force -Path (Split-Path $replacement.Destination), (Split-Path $replacement.Previous))
        if (Test-Path -LiteralPath $replacement.Destination)
        {
            [IO.Directory]::Move($replacement.Destination, $replacement.Previous)
            $replacement.MovedPrevious = $true
        }
        [IO.Directory]::Move($replacement.Incoming, $replacement.Destination)
        $replacement.Published = $true
    }
}

function Undo-ProductReplacements
{
    param($Replacements)

    for ($index = $replacements.Count - 1; $index -ge 0; --$index)
    {
        $replacement = $replacements[$index]
        if ($replacement.Published) { [IO.Directory]::Move($replacement.Destination, $replacement.Incoming) }
        if ($replacement.MovedPrevious) { [IO.Directory]::Move($replacement.Previous, $replacement.Destination) }
    }
}
