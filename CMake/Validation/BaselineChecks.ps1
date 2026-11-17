Set-StrictMode -Version Latest

function Invoke-BaselineCheck
{
    param(
        $Check,
        [string] $Build,
        [string] $Configuration,
        [string] $CMakePath,
        [string] $OutputDirectory,
        [int] $TimeoutSeconds,
        [string] $CancellationSignal
    )

    if ($CancellationSignal -ne '' -and (Test-Path -LiteralPath $CancellationSignal))
    {
        $check.Status = 'CANCELLED'; $check.Reason = 'Cancellation requested'; return
    }
    if ([string]::IsNullOrWhiteSpace($CMakePath)) { $check.Status = 'UNAVAILABLE'; $check.Reason = 'CMake is not installed'; return }
    if (!(Test-Path -LiteralPath (Join-Path $build 'CMakeCache.txt') -PathType Leaf))
    {
        $check.Status = 'MISSING_ARTIFACT'; $check.Reason = 'Configure the canonical workspace first'; return
    }
    $process = $null
    $timer = [Diagnostics.Stopwatch]::StartNew()
    $check.Status = 'FAILED'; $check.Reason = 'Owner invocation did not complete'
    try
    {
        Write-Host "Baseline owner check: $($check.Target) ($Configuration)"
        $process = Start-Process -FilePath $CMakePath -ArgumentList "--build `"$build`" --config $Configuration --target $($check.Target)" -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $OutputDirectory "$($check.Target).log") -RedirectStandardError (Join-Path $OutputDirectory "$($check.Target).error.log")
        [void] $process.Handle
        while ($true)
        {
            $completed = $process.WaitForExit(100)
            if ($completed) { break }
            if ($CancellationSignal -ne '' -and (Test-Path -LiteralPath $CancellationSignal))
            {
                $check.Status = 'CANCELLED'; $check.Reason = 'Cancellation requested'; break
            }
            if ($timer.Elapsed.TotalSeconds -ge $TimeoutSeconds)
            {
                $check.Status = 'TIMED_OUT'; $check.Reason = 'Per-check time limit reached'; break
            }
        }
        if ($completed)
        {
            $check.ExitCode = $process.ExitCode
            $check.Status = if ($process.ExitCode -eq 0) { 'PASS' } else { 'FAILED' }
            $check.Reason = 'Owner exit code'
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
