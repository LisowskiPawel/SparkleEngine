Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot '../../Artifacts/ArtifactIdentity.ps1')

function Assert-PackageManifest
{
    param($Manifest)
    $paths = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    foreach ($file in $Manifest.Files)
    {
        Assert-ArtifactPath $file.Path
        if (!$paths.Add($file.Path) -or $file.Path -eq 'manifests/sparkle-package-manifest.json')
        {
            throw "Duplicate or reserved manifest path: '$($file.Path)'."
        }
        if ($file.Path.StartsWith('Tools/Diagnostics/', [StringComparison]::OrdinalIgnoreCase) -or
            $file.Path -match '(^|/)recook\.signal$' -or
            $file.Path -match '(^|/)\.publication-incomplete$' -or
            $file.Path -match '\.(pdb|ilk|obj|lib|exp|cpp|cxx|h|hpp)$' -or
            $file.Path -match '(^|/)(build|_deps|\.git|Cache|Logs|Settings)(/|$)' -or
            $file.Path -match '(^|/)(Sponza|ModernSponza)/.*\.(glb|gltf|bin|dds|png|jpg|ktx2)$')
        {
            throw "Unadmitted source/debug/user-state/content payload: '$($file.Path)'."
        }
        if ($file.Component -notin @('Launcher','Editor','Game','AssetCooker','TextureCooker','ShaderCompiler','CMake','Engine','Shared') -or
            $file.Role -notin @('Runtime','Helper','EngineContent','Recipe','Defaults','Notice','Metadata') -or
            $file.Permission -ne 'ReadOnly' -or $file.License -match '^(|NOASSERTION|NONE)$' -or
            $file.Signature -notin @('UnsignedBuild','VendorSigned','NotApplicable') -or
            [string]::IsNullOrWhiteSpace($file.Owner) -or [string]::IsNullOrWhiteSpace($file.Origin) -or
            $file.Origin -match '[\\:]' -or $file.Origin.StartsWith('/'))
        {
            throw "Incomplete or private-path payload declaration: '$($file.Path)'."
        }
        if (($file.Component -eq 'Game' -or $file.Path -match '^artifacts/dev/Projects/[^/]+/Binaries/ShippingGame/') -and $file.Path -match '(?i)(Qt6|SparkleEditor|AssetCooker|TextureCooker|ShaderCompiler)')
        {
            throw "Editor/authoring dependency in Game component: '$($file.Path)'."
        }
        if ($file.Path -match '\.(exe|dll)$' -and $file.Role -notin @('Runtime','Helper'))
        {
            throw "PE payload has a non-executable role: '$($file.Path)'."
        }
    }
    foreach ($file in $Manifest.Files)
    {
        if (!$paths.Contains($file.Notice)) { throw "Notice is not admitted for '$($file.Path)'." }
    }
}

function Assert-PackageLayout
{
    param($Manifest)
    foreach ($path in @('LaunchSparkle.cmd', '.sparkle', 'Engine/.sparkle-engine', 'Projects/Showcase/.sparkle-project', 'Projects/Showcase/Levels.catalog', 'Config/DefaultEngine.ini'))
    {
        if (@($Manifest.Files | Where-Object Path -ceq $path).Count -ne 1) { throw "Missing package recipe input: '$path'." }
    }
    foreach ($path in @('artifacts/dev/Projects/Showcase/Cooked/.publication-lock', 'artifacts/dev/Projects/Showcase/Cooked/Shaders/GlobalShaderMap.smap',
                        'artifacts/dev/Projects/Showcase/Cooked/Shaders/CookedShaderLibrary.slib', 'artifacts/dev/Projects/Showcase/Cooked/Shaders/ShaderDependencies.sdep'))
    {
        if (@($Manifest.Files | Where-Object Path -ceq $path).Count -ne 1) { throw "Missing installed content input: '$path'." }
    }
    if (!@($Manifest.Files | Where-Object { $_.Path.StartsWith('Engine/Assets/') -and $_.Role -eq 'EngineContent' }).Count)
    {
        throw 'Missing immutable engine assets.'
    }
    foreach ($file in $Manifest.Files)
    {
        if ($file.Path -match '^Projects/Showcase/(Cache|Downloads)(/|$)')
        {
            throw "Writable selected-content output inside immutable package: '$($file.Path)'."
        }
    }
}
