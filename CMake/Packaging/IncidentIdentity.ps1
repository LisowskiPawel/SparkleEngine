Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot 'PackageVerification.ps1')
Add-Type -AssemblyName System.IO.Compression.FileSystem
Add-Type -AssemblyName System.IO.Compression

function Get-IncidentFile
{
    param([string] $Path, [long] $MaximumBytes)
    if ([IO.Path]::GetFullPath($Path).StartsWith('\\')) { throw 'Choose a local file, not a network/device path.' }
    $item = Get-Item -LiteralPath $Path -Force -ErrorAction Stop
    if ($item.PSIsContainer -or $item.Length -gt $MaximumBytes) { throw 'Diagnostic input is absent, not a file, or exceeds its byte limit.' }
    $current = $item
    while ($null -ne $current)
    {
        if (($current.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw 'Diagnostic input/output uses a reparse point.' }
        $current = if ($current -is [IO.DirectoryInfo]) { $current.Parent } else { $current.Directory }
    }
    return $item
}

function Get-IncidentPackage
{
    param([string] $PackageRoot, [string] $ArchivePath, [string] $ArchiveSha256)
    if ($ArchiveSha256 -cnotmatch '^[a-f0-9]{64}$') { throw 'An exact lowercase archive SHA-256 is required.' }
    [void] (Get-IncidentFile $ArchivePath ([long]::MaxValue))
    if ((Get-FileHash -LiteralPath $ArchivePath).Hash.ToLowerInvariant() -cne $ArchiveSha256) { throw 'Archive identity mismatch.' }
    if ([IO.Path]::GetFullPath($PackageRoot).StartsWith('\\')) { throw 'Choose a local package.' }
    $PackageRoot = (Get-Item -LiteralPath $PackageRoot).FullName.TrimEnd('\','/')
    $path = Join-Path $PackageRoot 'manifests/sparkle-package-manifest.json'
    [void] (Get-IncidentFile $path 16777216)
    $manifest = Get-Content -LiteralPath $path -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($manifest.Version -cnotmatch '^[0-9]+\.[0-9]+\.[0-9]+$' -or $manifest.Revision -cnotmatch '^[a-f0-9]{40}$' -or
        $manifest.SourceInputsSha256 -cnotmatch '^[a-f0-9]{64}$') { throw 'Incomplete candidate identity.' }
    foreach ($product in $manifest.Products)
    {
        Assert-PackagePath $product.Path
        if ($product.Target -cnotmatch '^[A-Za-z][A-Za-z0-9]*$' -or
            $product.Configuration -cnotmatch '^Shipping(Editor|Game)$') { throw 'Private or unsupported product identity.' }
    }
    Assert-PackageLayout $manifest
    Test-PackageStage $PackageRoot $manifest
    Test-PackageArchive $ArchivePath $PackageRoot $manifest
    return $manifest
}

function Get-NativeSymbolIdentity
{
    param([string] $Path)
    [void] (Get-IncidentFile $Path ([long]::MaxValue))
    if ($null -eq ('SparkleSymbolIdentity' -as [type]))
    {
        Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class SparkleSymbolIdentity
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct FileIndex
    {
        public uint Size;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 261)] public string File;
        [MarshalAs(UnmanagedType.Bool)] public bool Stripped;
        public uint Timestamp;
        public uint ImageSize;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 261)] public string DebugFile;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 261)] public string PdbFile;
        public Guid Guid;
        public uint Signature;
        public uint Age;
    }
    [DllImport("dbghelp.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    [DefaultDllImportSearchPaths(DllImportSearchPath.System32)]
    [return: MarshalAs(UnmanagedType.Bool)]
    public static extern bool SymSrvGetFileIndexInfoW(string file, ref FileIndex info, uint flags);
}
'@
    }
    $info = New-Object SparkleSymbolIdentity+FileIndex
    $info.Size = [Runtime.InteropServices.Marshal]::SizeOf($info)
    if (![SparkleSymbolIdentity]::SymSrvGetFileIndexInfoW($Path, [ref] $info, 0) -or $info.Guid -eq [Guid]::Empty)
    {
        throw 'Native build ID is unavailable; retain a matching PE/PDB pair.'
    }
    # Embedded file names and source paths deliberately do not cross this boundary.
    return [ordered]@{ Guid = $info.Guid.ToString('D'); Age = $info.Age }
}

function Write-IncidentRecord
{
    param([string] $OutputDirectory, [string] $Json, [string] $Prefix)
    if ([IO.Path]::GetFullPath($OutputDirectory).StartsWith('\\')) { throw 'Choose a local export directory.' }
    $directory = Get-Item -LiteralPath $OutputDirectory -Force
    if (!$directory.PSIsContainer) { throw 'Choose an existing local export directory.' }
    $current = $directory
    while ($null -ne $current)
    {
        if (($current.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw 'Reparse export directory.' }
        $current = $current.Parent
    }
    $path = Join-Path $directory.FullName ($Prefix + '-' + [Guid]::NewGuid().ToString('N') + '.json')
    $partial = $path + '.partial'
    $stream = [IO.File]::Open($partial, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::None)
    try
    {
        $bytes = [Text.UTF8Encoding]::new($false).GetBytes($Json)
        $stream.Write($bytes, 0, $bytes.Length)
        $stream.Flush($true)
    }
    finally { $stream.Dispose() }
    [IO.File]::Move($partial, $path)
    return $path
}
