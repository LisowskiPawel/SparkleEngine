Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot 'ArtifactIdentity.ps1')

function Get-NativeSymbolIdentity
{
    param([string] $Path)
    [void] (Get-LocalArtifactFile $Path ([long]::MaxValue))
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
