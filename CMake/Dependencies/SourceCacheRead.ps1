$ErrorActionPreference = 'Stop'
$reader = $null
$process = $null
try
{
    # PowerShell 5.1 binds -switch:value as two script arguments. Read the native
    # invocation instead so compiler switches and definitions keep their spelling.
    $invocation = [Environment]::GetCommandLineArgs()
    $fileOption = [Array]::IndexOf($invocation, '-File')
    if ($fileOption -lt 0 -or $invocation.Length -lt $fileOption + 4)
    {
        throw 'Invoke with powershell -File SourceCacheRead.ps1 <lock file> <executable> [arguments].'
    }
    $lockFile = $invocation[$fileOption + 2]
    $sharing = [IO.FileShare]::Read -bor [IO.FileShare]::Inheritable
    $reader = [IO.File]::Open($lockFile, [IO.FileMode]::Open, [IO.FileAccess]::Read, $sharing)
    $executable = $invocation[$fileOption + 3]
    $arguments = @($invocation | Select-Object -Skip ($fileOption + 4))
    # Preserve literal quotes in compiler definitions on Windows PowerShell 5.1.
    $quotedArguments = foreach ($argument in $arguments)
    {
        $escaped = [regex]::Replace($argument, '(\\*)"', '$1$1\"')
        $escaped = [regex]::Replace($escaped, '(\\+)$', '$1$1')
        '"' + $escaped + '"'
    }
    $start = [Diagnostics.ProcessStartInfo]::new()
    $start.FileName = $executable
    $start.Arguments = $quotedArguments -join ' '
    $start.UseShellExecute = $false
    $process = [Diagnostics.Process]::Start($start)
    $process.WaitForExit()
    $commandExit = $process.ExitCode
}
catch
{
    [Console]::Error.WriteLine("Source dependency cache command failed; retry after configure/Clean settles. $_")
    $commandExit = 1
}
finally
{
    if ($reader) { $reader.Dispose() }
    if ($process) { $process.Dispose() }
}
exit $commandExit
