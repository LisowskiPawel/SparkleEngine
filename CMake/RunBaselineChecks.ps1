[CmdletBinding()]
param(
    [ValidatePattern('^[A-Za-z0-9]+$')][string] $Configuration = "ShippingEditor",
    [ValidateRange(1, 86400)][int] $TimeoutSeconds = 1800,
    [string] $CancellationSignal = "",
    [ValidateSet("architecture_boundary_check", "code_style_check", "ShaderCompilerCliValidation")]
    [string[]] $SkipGate = @(),
    [string] $OutputDirectory = ""
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$repositoryRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot "..")).Path
$revision = & git -C $repositoryRoot rev-parse HEAD
if ($LASTEXITCODE -ne 0) { throw "Cannot identify the source revision." }
$worktree = @(& git -C $repositoryRoot status --short)
if ($LASTEXITCODE -ne 0) { throw "Cannot identify the worktree state." }
if ($OutputDirectory -eq "")
{
    $OutputDirectory = Join-Path $repositoryRoot ("artifacts/validation/baseline/" + [Guid]::NewGuid().ToString('N'))
}
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
if (Test-Path -LiteralPath $OutputDirectory) { throw "Choose a fresh baseline output directory." }
[void] (New-Item -ItemType Directory -Path $OutputDirectory)
$patchPath = Join-Path $OutputDirectory 'worktree.patch'
$patch = @(& git -C $repositoryRoot diff --binary HEAD)
if ($LASTEXITCODE -ne 0) { throw "Cannot retain the worktree patch." }
[IO.File]::WriteAllText($patchPath, ($patch -join "`n"), [Text.UTF8Encoding]::new($false))
$build = Join-Path $repositoryRoot 'build'
$cmake = Get-Command cmake -CommandType Application -ErrorAction SilentlyContinue
$checks = @("architecture_boundary_check", "code_style_check", "ShaderCompilerCliValidation") | ForEach-Object {
    [ordered]@{ Target = $_; Status = 'SKIPPED'; ExitCode = $null; Reason = 'Not reached'; Seconds = 0; Command = "cmake --build `"$build`" --config $Configuration --target $_" }
}
$cancelled = $false
try
{
    # Owner targets retain their checks and exit codes; this entry only runs them.
    foreach ($check in $checks)
    {
        if ($check.Target -in $SkipGate) { $check.Reason = 'Explicitly skipped'; continue }
        if ($cancelled) { $check.Reason = 'Cancelled before invocation'; continue }
        if ($CancellationSignal -ne '' -and (Test-Path -LiteralPath $CancellationSignal))
        {
            $check.Status = 'CANCELLED'; $check.Reason = 'Cancellation requested'; $cancelled = $true; continue
        }
        if ($null -eq $cmake) { $check.Status = 'UNAVAILABLE'; $check.Reason = 'CMake is not installed'; continue }
        if (!(Test-Path -LiteralPath (Join-Path $build 'CMakeCache.txt') -PathType Leaf))
        {
            $check.Status = 'MISSING_ARTIFACT'; $check.Reason = 'Configure the canonical workspace first'; continue
        }
        $process = $null
        $timer = [Diagnostics.Stopwatch]::StartNew()
        $check.Status = 'CANCELLED'; $check.Reason = 'Invocation interrupted'
        try
        {
            Write-Host "Baseline owner check: $($check.Target) ($Configuration)"
            $process = Start-Process -FilePath $cmake.Source -ArgumentList "--build `"$build`" --config $Configuration --target $($check.Target)" -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $OutputDirectory "$($check.Target).log") -RedirectStandardError (Join-Path $OutputDirectory "$($check.Target).error.log")
            [void] $process.Handle
            while (!$process.WaitForExit(100))
            {
                if ($CancellationSignal -ne '' -and (Test-Path -LiteralPath $CancellationSignal))
                {
                    $check.Reason = 'Cancellation requested'; $cancelled = $true; break
                }
                if ($timer.Elapsed.TotalSeconds -ge $TimeoutSeconds)
                {
                    $check.Status = 'TIMED_OUT'; $check.Reason = 'Per-check time limit reached'; break
                }
            }
            if ($process.HasExited)
            {
                $check.ExitCode = $process.ExitCode
                $check.Status = if ($process.ExitCode -eq 0) { 'PASS' } else { 'FAILED' }
                $check.Reason = 'Owner exit code'
                if ($check.Status -eq 'PASS' -and $check.Target -eq 'ShaderCompilerCliValidation')
                {
                    foreach ($artifact in @('GlobalShaderMap.smap', 'CookedShaderLibrary.slib'))
                    {
                        if (!(Test-Path -LiteralPath (Join-Path $repositoryRoot "artifacts/dev/Projects/Shared/Cooked/Shaders/$artifact") -PathType Leaf))
                        {
                            $check.Status = 'MISSING_ARTIFACT'; $check.Reason = "Missing owner output: $artifact"
                        }
                    }
                }
            }
        }
        finally
        {
            if ($null -ne $process)
            {
                if (!$process.HasExited)
                {
                    & "$env:SystemRoot/System32/taskkill.exe" /PID $process.Id /T /F | Out-Null
                    $process.WaitForExit()
                    $check.ExitCode = $process.ExitCode
                }
                $process.Dispose()
            }
            $check.Seconds = [Math]::Round($timer.Elapsed.TotalSeconds, 3)
        }
    }
}
finally
{
    [ordered]@{
        Revision = $revision.Trim(); Worktree = $worktree; Configuration = $Configuration
        WorktreePatchSha256 = (Get-FileHash -LiteralPath $patchPath).Hash.ToLowerInvariant()
        CMake = if ($null -eq $cmake) { $null } else { $cmake.Source }
        Checks = @($checks)
    } | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $OutputDirectory 'report.json') -Encoding UTF8
    Write-Host "Baseline results: $OutputDirectory/report.json"
}
if (@($checks | Where-Object { $_.Status -ne 'PASS' }).Count -ne 0) { exit 1 }
exit 0
