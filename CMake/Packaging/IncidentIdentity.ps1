Set-StrictMode -Version Latest

function Get-IncidentPreviewSha256
{
    param([string] $Json)
    $hash = [Security.Cryptography.SHA256]::Create()
    try { return [BitConverter]::ToString($hash.ComputeHash([Text.Encoding]::UTF8.GetBytes($Json))).Replace('-','').ToLowerInvariant() }
    finally { $hash.Dispose() }
}

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
