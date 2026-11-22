Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot '../../Artifacts/ArtifactIdentity.ps1')

function Test-PackageMetadata
{
    param([string] $Stage, $Manifest)
    $sbom = Get-Content -LiteralPath (Join-Path $Stage 'manifests/sparkle-sbom.spdx.json') -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($sbom.spdxVersion -cne 'SPDX-2.3' -or $sbom.dataLicense -cne 'CC0-1.0' -or $sbom.SPDXID -cne 'SPDXRef-DOCUMENT' -or
        $sbom.documentNamespace -notmatch '^https://[^ ]+$' -or !@($sbom.creationInfo.creators).Count -or !@($sbom.packages).Count)
    {
        throw 'Incomplete SPDX 2.3 document identity.'
    }
    $inventory = @{}
    foreach ($file in $sbom.files)
    {
        if (!$file.fileName.StartsWith('./')) { throw 'SPDX file names must be package relative.' }
        $path = $file.fileName.Substring(2)
        Assert-ArtifactPath $path
        if ($inventory.ContainsKey($path)) { throw 'Duplicate SPDX file.' }
        $inventory[$path] = $file
    }
    foreach ($file in $Manifest.Files | Where-Object Path -notin @('manifests/sparkle-sbom.spdx.json','manifests/sparkle-provenance.json'))
    {
        if (!$inventory.ContainsKey($file.Path)) { throw "Missing SPDX file: '$($file.Path)'." }
        $entry = $inventory[$file.Path]
        $checksum = @($entry.checksums | Where-Object algorithm -ceq 'SHA256')
        if ($checksum.Count -ne 1 -or $checksum[0].checksumValue -cne $file.Sha256 -or $entry.licenseConcluded -cne $file.License)
        {
            throw "SPDX file identity differs from manifest: '$($file.Path)'."
        }
        $inventory.Remove($file.Path)
    }
    if ($inventory.Count -ne 0) { throw 'Extra SPDX file; metadata self-digests belong only in the package manifest.' }
    foreach ($component in $sbom.packages)
    {
        if ([string]::IsNullOrWhiteSpace($component.name) -or [string]::IsNullOrWhiteSpace($component.versionInfo) -or
            $component.licenseDeclared -match '^(|NONE|NOASSERTION)$') { throw 'Unqualified SPDX software component.' }
    }
    $licenseDefinitions = @{}
    if ($sbom.PSObject.Properties.Name -contains 'hasExtractedLicensingInfos')
    {
        foreach ($definition in $sbom.hasExtractedLicensingInfos)
        {
            if ($licenseDefinitions.ContainsKey($definition.licenseId) -or
                [string]::IsNullOrWhiteSpace($definition.extractedText) -or $definition.extractedText -in @('NONE','NOASSERTION'))
            {
                throw "Missing/duplicate SPDX license definition: '$($definition.licenseId)'."
            }
            $licenseDefinitions[$definition.licenseId] = $definition.extractedText
        }
    }
    $licenses = @($sbom.packages | ForEach-Object licenseDeclared) + @($sbom.files | ForEach-Object licenseConcluded)
    foreach ($license in $licenses)
    {
        foreach ($reference in [regex]::Matches($license, '(?<![A-Za-z0-9.-])LicenseRef-[A-Za-z0-9.-]+'))
        {
            if (!$licenseDefinitions.ContainsKey($reference.Value))
            {
                throw "Unresolved SPDX license reference '$($reference.Value)' in '$license'."
            }
        }
    }
    foreach ($product in $Manifest.Products)
    {
        foreach ($required in $product.SourcePackages)
        {
            $matches = @($sbom.packages | Where-Object { $_.name -ceq $required.name -and $_.versionInfo -ceq $required.versionInfo })
            if ($matches.Count -ne 1) { throw "Missing/duplicate SPDX dependency: '$($required.name)' version '$($required.versionInfo)'." }
            foreach ($field in @('versionInfo','licenseDeclared','downloadLocation'))
            {
                if ($matches[0].$field -cne $required.$field)
                {
                    throw "SPDX dependency differs from configured source: '$($required.name).$field'."
                }
            }
        }
    }
    $provenance = Get-Content -LiteralPath (Join-Path $Stage 'manifests/sparkle-provenance.json') -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($provenance.Revision -cne $Manifest.Revision -or $provenance.SourceInputsSha256 -cne $Manifest.SourceInputsSha256 -or
        $provenance.Version -cne $Manifest.Version) { throw 'Provenance belongs to a different source candidate.' }
    foreach ($product in $Manifest.Products)
    {
        $entry = @($provenance.Products | Where-Object Component -ceq $product.Component)
        if ($entry.Count -ne 1 -or $entry[0].Target -cne $product.Target -or $entry[0].Configuration -cne $product.Configuration -or $entry[0].Path -cne $product.Path)
        {
            throw "Wrong provenance product: '$($product.Component)'."
        }
    }
    if (@($provenance.Products).Count -ne @($Manifest.Products).Count) { throw 'Extra provenance product.' }
}
