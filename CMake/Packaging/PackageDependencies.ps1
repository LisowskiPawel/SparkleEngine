Set-StrictMode -Version Latest

function Get-PackagePeImports
{
    param([string] $Path)
    $ErrorActionPreference = 'Stop'
    $reader = [IO.BinaryReader]::new([IO.File]::OpenRead($Path))
    try
    {
        $length = $reader.BaseStream.Length
        $read32 = {
            param([long] $Offset)
            if ($Offset -lt 0 -or $Offset -gt $length - 4) { throw "Truncated PE: '$Path'." }
            $reader.BaseStream.Position = $Offset
            return $reader.ReadUInt32()
        }
        if ($length -lt 64 -or $reader.ReadUInt16() -ne 0x5a4d) { throw "Not a PE: '$Path'." }
        $pe = & $read32 0x3c
        if ((& $read32 $pe) -ne 0x4550) { throw "Invalid PE signature: '$Path'." }
        $reader.BaseStream.Position = $pe + 4
        if ($reader.ReadUInt16() -ne 0x8664) { throw "Not a Windows x64 PE: '$Path'." }
        $sectionCount = $reader.ReadUInt16()
        $reader.BaseStream.Position = $pe + 20
        $optionalSize = $reader.ReadUInt16()
        $optional = $pe + 24
        $reader.BaseStream.Position = $optional
        if ($sectionCount -lt 1 -or $sectionCount -gt 96 -or $optionalSize -lt 112 -or $reader.ReadUInt16() -ne 0x20b)
        {
            throw "Invalid PE32+ header: '$Path'."
        }
        $headersSize = & $read32 ($optional + 60)
        $directoryCount = & $read32 ($optional + 108)
        if ($directoryCount -gt 16 -or $optionalSize -lt 112 + 8 * $directoryCount) { throw 'Invalid PE data directories.' }
        $sections = @(for ($index = 0; $index -lt $sectionCount; ++$index)
        {
            $section = $optional + $optionalSize + 40 * $index
            if ($section -gt $length - 40) { throw 'Truncated PE section table.' }
            @{ Rva = (& $read32 ($section + 12)); Size = (& $read32 ($section + 16)); Offset = (& $read32 ($section + 20)) }
        })
        $resolveRva = {
            param([long] $Rva, [long] $Size)
            if ($Rva -lt $headersSize -and $Rva + $Size -le $headersSize -and $Rva + $Size -le $length) { return $Rva }
            foreach ($section in $sections)
            {
                $delta = $Rva - [long] $section.Rva
                $offset = [long] $section.Offset + $delta
                if ($delta -ge 0 -and $delta + $Size -le $section.Size -and $offset + $Size -le $length) { return $offset }
            }
            throw "Unmapped import RVA in '$Path'."
        }
        $imports = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
        foreach ($directory in @(@{ Index = 1; Width = 20; NameIndex = 3 }, @{ Index = 13; Width = 32; NameIndex = 1 }))
        {
            if ($directory.Index -ge $directoryCount) { continue }
            $entry = $optional + 112 + 8 * $directory.Index
            $rva = & $read32 $entry
            $size = & $read32 ($entry + 4)
            if ($rva -eq 0 -and $size -eq 0) { continue }
            if ($rva -eq 0 -or $size -lt $directory.Width -or $size -gt 16MB) { throw 'Invalid PE import directory.' }
            $terminated = $false
            for ($position = 0; $position + $directory.Width -le $size; $position += $directory.Width)
            {
                $offset = & $resolveRva ($rva + $position) $directory.Width
                $fields = @(for ($field = 0; $field -lt $directory.Width / 4; ++$field) { & $read32 ($offset + 4 * $field) })
                if (!@($fields | Where-Object { $_ -ne 0 }).Count) { $terminated = $true; break }
                if ($directory.Index -eq 13 -and $fields[0] -ne 1) { throw 'Unsupported non-RVA delay import descriptor.' }
                $nameRva = $fields[$directory.NameIndex]
                $name = [Text.StringBuilder]::new()
                $ended = $false
                for ($character = 0; $character -lt 260; ++$character)
                {
                    $reader.BaseStream.Position = & $resolveRva ($nameRva + $character) 1
                    $value = $reader.ReadByte()
                    if ($value -eq 0) { $ended = $true; break }
                    [void] $name.Append([char] $value)
                }
                $dll = $name.ToString()
                if (!$ended -or $dll -notmatch '^[a-zA-Z0-9_.-]+\.dll$') { throw "Invalid import name in '$Path'." }
                [void] $imports.Add($dll)
            }
            if (!$terminated) { throw "Unterminated import directory in '$Path'." }
        }
        return @($imports | Sort-Object)
    }
    finally { $reader.Dispose() }
}

function Test-PackageDependencyClosure
{
    param([string] $Stage, $Manifest)
    $ErrorActionPreference = 'Stop'
    $system = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    foreach ($dependency in $Manifest.SystemImports)
    {
        if ($dependency.Name -notmatch '^[a-zA-Z0-9_.-]+\.dll$' -or !$system.Add($dependency.Name) -or
            $dependency.Owner -notin @('Windows','GraphicsDriver') -or [string]::IsNullOrWhiteSpace($dependency.Requirement) -or
            $dependency.Name -match '^(Qt[56]|vcruntime|msvcp|concrt|dxcompiler|dxil|slang|sl\.|nvngx|Sparkle)')
        {
            throw 'Unqualified or duplicate external runtime dependency.'
        }
    }
    $paths = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    foreach ($file in $Manifest.Files) { [void] $paths.Add($file.Path) }
    foreach ($file in $Manifest.Files | Where-Object Path -match '\.(exe|dll)$')
    {
        $product = @($Manifest.Products | Where-Object Component -eq $file.Component)
        if ($product.Count -gt 1) { throw "Ambiguous component loader root: '$($file.Component)'." }
        $directory = $file.Path.Substring(0, $file.Path.LastIndexOf('/') + 1)
        $productDirectory = $(if ($product.Count -eq 1) { $product[0].Path.Substring(0, $product[0].Path.LastIndexOf('/') + 1) } else { $directory })
        foreach ($dll in @(Get-PackagePeImports (Join-Path $Stage $file.Path)))
        {
            if (($file.Component -eq 'Game' -or $file.Path -match '^Projects/[^/]+/Binaries/ShippingGame/') -and $dll -match '(Qt[56]|Editor|AssetCooker|TextureCooker|ShaderCompiler)')
            {
                throw "Editor/authoring import '$dll' in '$($file.Path)'."
            }
            if (!$paths.Contains($directory + $dll) -and !$paths.Contains($productDirectory + $dll) -and !$system.Contains($dll))
            {
                throw "Unresolved app-local import '$dll' in '$($file.Path)'."
            }
        }
    }
}
