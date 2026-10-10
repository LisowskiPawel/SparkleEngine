[CmdletBinding()]
param([string] $Configuration = "ShippingEditor")

$ErrorActionPreference = "Stop"
$repositoryRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot "..")).Path

& git -C $repositoryRoot rev-parse HEAD
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& git -C $repositoryRoot status --short
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# These targets own the checks and their success/failure semantics.
foreach ($gate in @("architecture_boundary_check", "code_style_check", "ShaderCompilerCliValidation"))
{
    Write-Host "Baseline owner check: $gate ($Configuration)"
    & cmake --build (Join-Path $repositoryRoot "build") --config $Configuration --target $gate
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}
