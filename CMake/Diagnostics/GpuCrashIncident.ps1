Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot '../Artifacts/NativeSymbols.ps1')
. (Join-Path $PSScriptRoot '../Artifacts/ShaderExport.ps1')

function ConvertFrom-GpuCrashUtf8
{
    param([byte[]] $Bytes)
    $ErrorActionPreference = 'Stop'
    if ($Bytes.Length -gt 33554432) { throw 'Incident exceeds its byte limit.' }
    $utf8 = [Text.UTF8Encoding]::new($false, $true)
    $stream = [IO.MemoryStream]::new($bytes, $false)
    $textReader = [IO.StreamReader]::new($stream, $utf8, $true)
    try
    {
        $text = $textReader.ReadToEnd()
        if ($textReader.CurrentEncoding.CodePage -ne 65001) { throw 'Incident requires UTF-8 encoding.' }
    }
    finally { $textReader.Dispose() }
    if ($text.IndexOf([char] 0) -ge 0) { throw 'Incident contains a raw NUL.' }
    return $text
}

function Assert-GpuCrashJsonMembers
{
    param([string] $Text, [string] $Kind)
    $ErrorActionPreference = 'Stop'
    $utf8 = [Text.UTF8Encoding]::new($false, $true)
    Add-Type -AssemblyName System.Runtime.Serialization
    $quotas = [Xml.XmlDictionaryReaderQuotas]::new()
    $quotas.MaxDepth = 66
    $quotas.MaxStringContentLength = 33554432
    $quotas.MaxArrayLength = 65536
    $members = @{
        root = @('incidentId','state','childPid','candidate','executableSha256','privacy','sourceMapping','nativeFailure','sdkFinished','files')
        candidate = @('product','profile','revision','sourceInputsSha256','dirtyPatchSha256','cpuPdbGuid','cpuPdbAge','cpuPdbSha256','graphicsApi','productReceiptSha256','shaderExportSha256','globalShaderMapSha256','cookedShaderLibrarySha256','aftermathRuntimeSha256','shaderExportPath','productReceiptPath','cpuPdbPath')
        file = @('path','sha256','bytes')
    }
    if ($Kind -ceq 'Product') { $members.root = @('product','profile','variant','revision','sourceInputsSha256','dirtyPatchSha256','executableSha256','cpuPdbGuid','cpuPdbAge','cpuPdbSha256') }
    elseif ($Kind -ceq 'Compiler') { $members.root = @('product','variant','revision','sourceInputsSha256','dirtyPatchSha256','executableSha256') }
    $isIncident = $Kind -ceq 'Incident'
    $seen = @{}
    foreach ($scope in $members.Keys) { $seen[$scope] = [Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal) }
    $required = [Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
    $rootField = ''
    $fileCount = 0
    $reader = [Runtime.Serialization.Json.JsonReaderWriterFactory]::CreateJsonReader($utf8.GetBytes($text), $quotas)
    try
    {
        while ($reader.Read())
        {
            if ($reader.NodeType -ne [Xml.XmlNodeType]::Element) { continue }
            $depth = $reader.Depth
            $type = $reader.GetAttribute('type')
            $name = if ($reader.NamespaceURI -ceq 'item') { $reader.GetAttribute('item') } else { $reader.LocalName }
            [void] $utf8.GetBytes($name)
            if ($depth -eq 0 -and $type -cne 'object') { throw 'Incident requires an object root.' }
            $scope = ''
            if ($depth -eq 1)
            {
                $scope = 'root'
                $rootField = $name
                if ($isIncident -and $name -ceq 'files')
                {
                    if ($type -cne 'array') { throw 'Incident requires a file array.' }
                    [void] $required.Add('files')
                }
            }
            elseif ($isIncident -and $rootField -ceq 'candidate' -and $depth -eq 2) { $scope = 'candidate' }
            elseif ($isIncident -and $rootField -ceq 'files' -and $depth -eq 2)
            {
                if ($type -cne 'object' -or ++$fileCount -gt 65535) { throw 'Incident exceeds its file bound or has a non-object member.' }
                $seen.file.Clear()
            }
            elseif ($isIncident -and $rootField -ceq 'files' -and $depth -eq 3) { $scope = 'file' }
            if ($scope -ne '' -and $members[$scope] -contains $name -and $members[$scope] -cnotcontains $name) { throw 'GPU crash receipt field names require exact casing.' }
            if ($scope -ne '' -and $members[$scope] -ccontains $name -and !$seen[$scope].Add($name)) { throw 'Incident repeats a consumed member.' }
            if ($type -cin @('object','array'))
            {
                if (($scope -ceq 'file' -and $name -ceq 'bytes') -or ($scope -ceq 'candidate' -and $name -ceq 'cpuPdbAge') -or
                    ($Kind -ceq 'Product' -and $scope -ceq 'root' -and $name -ceq 'cpuPdbAge'))
                {
                    throw 'Incident byte counts and CPU PDB age require unsigned integer tokens.'
                }
                continue
            }
            $value = $reader.ReadString()
            [void] $utf8.GetBytes($value)
            if ($isIncident -and $scope -ceq 'root' -and $name -ceq 'childPid')
            {
                $pidValue = [uint32] 0
                if ($type -cne 'number' -or ![uint32]::TryParse($value, [Globalization.NumberStyles]::None, [Globalization.CultureInfo]::InvariantCulture, [ref] $pidValue) -or
                    $pidValue -eq 0) { throw 'Incident childPid requires a positive uint32 integer token.' }
                [void] $required.Add('childPid')
            }
            elseif ($isIncident -and $scope -ceq 'root' -and $name -ceq 'nativeFailure')
            {
                $failure = [long] 0
                if ($type -cne 'number' -or $value.StartsWith('+') -or
                    ![long]::TryParse($value, [Globalization.NumberStyles]::AllowLeadingSign, [Globalization.CultureInfo]::InvariantCulture, [ref] $failure))
                {
                    throw 'Incident nativeFailure requires an int64 integer token.'
                }
                [void] $required.Add('nativeFailure')
            }
            elseif ($isIncident -and $scope -ceq 'root' -and $name -ceq 'sdkFinished')
            {
                if ($type -cne 'boolean') { throw 'Incident sdkFinished requires a boolean token.' }
                [void] $required.Add('sdkFinished')
            }
            elseif (($scope -ceq 'file' -and $name -ceq 'bytes') -or ($scope -ceq 'candidate' -and $name -ceq 'cpuPdbAge') -or
                    ($Kind -ceq 'Product' -and $scope -ceq 'root' -and $name -ceq 'cpuPdbAge'))
            {
                $unsigned = [uint64] 0
                if ($type -cne 'number' -or ![uint64]::TryParse($value, [Globalization.NumberStyles]::None, [Globalization.CultureInfo]::InvariantCulture, [ref] $unsigned))
                {
                    throw 'Incident byte counts and CPU PDB age require unsigned integer tokens.'
                }
                if ($Kind -ceq 'Product' -and $scope -ceq 'root') { [void] $required.Add('cpuPdbAge') }
            }
        }
    }
    finally { $reader.Dispose() }
    if ($isIncident -and $required.Count -ne 4) { throw 'Incident omits required typed collection fields.' }
    if ($Kind -ceq 'Product' -and !$required.Contains('cpuPdbAge')) { throw 'Product receipt omits its unsigned CPU PDB age.' }
}

function ConvertFrom-GpuCrashJson
{
    param([byte[]] $Bytes, [ValidateSet('Incident','Product','Compiler')][string] $Kind = 'Incident')
    $ErrorActionPreference = 'Stop'
    $text = ConvertFrom-GpuCrashUtf8 $Bytes
    Assert-GpuCrashJsonMembers $text $Kind
    # Windows JSON supplies strict grammar; the framework reader preserves token types and spelling.
    [void] [Windows.Data.Json.JsonObject,Windows.Data.Json,ContentType=WindowsRuntime]::Parse($text)
    return ($text | ConvertFrom-Json)
}

function Get-GpuCrashIncident
{
    param([string] $ManifestPath, [string] $ExpectedSessionId = '', [uint32] $ExpectedChildPid = 0)
    $file = Get-LocalArtifactFile $ManifestPath 33554432
    if ($file.Name -cne 'incident.json') { throw 'Choose the exact incident manifest.' }
    $incident = ConvertFrom-GpuCrashJson ([IO.File]::ReadAllBytes($file.FullName))

    if ($incident.incidentId -cnotmatch '^[a-f0-9]{8}-[a-f0-9]{4}-4[a-f0-9]{3}-[89ab][a-f0-9]{3}-[a-f0-9]{12}$' -or
        $incident.state -cnotin @('Preparing','Ready','Collecting','Collected','Partial','Timed out','Unavailable') -or
        $incident.sourceMapping -cne 'unqualified' -or $incident.executableSha256 -cnotmatch '^[a-f0-9]{64}$')
    {
        throw 'Incomplete GPU crash session identity.'
    }
    [void] (Get-LocalArtifactFile (Join-Path $file.DirectoryName 'owner.txt') 36)
    $owner = [IO.File]::ReadAllText((Join-Path $file.DirectoryName 'owner.txt'))
    if ($owner -cne $incident.incidentId -or ($ExpectedSessionId -ne '' -and $ExpectedSessionId -cne $incident.incidentId) -or
        ($ExpectedChildPid -ne 0 -and $ExpectedChildPid -ne $incident.childPid)) { throw 'Stale or other-child incident.' }
    $names = @{}
    $total = [long] $file.Length
    foreach ($entry in $incident.files)
    {
        if ($entry.path -cnotmatch '^(crash\.nv-gpudmp|shader-[a-f0-9]+-[a-f0-9]+\.(nvdbg|json)|native-[a-f0-9]+-[a-f0-9]+\.json)$' -or
            $names.ContainsKey($entry.path) -or $entry.sha256 -cnotmatch '^[a-f0-9]{64}$') { throw 'Invalid or duplicate incident membership.' }
        $names[$entry.path] = $true
        $payload = Get-LocalArtifactFile (Join-Path $file.DirectoryName $entry.path) 1073741824
        $total += $payload.Length
        if ($payload.Length -ne $entry.bytes -or (Get-FileHash -LiteralPath $payload.FullName).Hash.ToLowerInvariant() -cne $entry.sha256 -or $total -gt 1073741824)
        {
            throw 'Incomplete or changed incident payload.'
        }
    }
    return $incident
}

function Get-GpuCrashBundle
{
    param($Incident)
    $candidate = $Incident.candidate
    if ($null -eq $candidate) { throw 'GPU crash inputs were not qualified.' }
    foreach ($pair in @(@('shaderExportPath','shaderExportSha256'), @('productReceiptPath','productReceiptSha256'), @('cpuPdbPath','cpuPdbSha256')))
    {
        [void] (Get-LocalArtifactFile $candidate.($pair[0]) $(if ($pair[0] -eq 'cpuPdbPath') { [long]::MaxValue } else { 33554432 }))
        if ((Get-FileHash -LiteralPath $candidate.($pair[0])).Hash.ToLowerInvariant() -cne $candidate.($pair[1])) { throw 'Candidate input changed.' }
    }
    $product = ConvertFrom-GpuCrashJson ([IO.File]::ReadAllBytes($candidate.productReceiptPath)) -Kind Product
    $bundle = Get-ShaderExport $candidate.shaderExportPath
    $root = [IO.Path]::GetDirectoryName($candidate.shaderExportPath)
    if (@($bundle.files | Where-Object path -ceq 'product-receipt.json').Count -ne 1) { throw 'Compiler source identity is absent.' }
    $compilerFile = Get-LocalArtifactFile (Join-Path $root 'product-receipt.json') 33554432
    $compiler = ConvertFrom-GpuCrashJson ([IO.File]::ReadAllBytes($compilerFile.FullName)) -Kind Compiler
    foreach ($key in @('revision','sourceInputsSha256','dirtyPatchSha256'))
    {
        if ($candidate.$key -cne $product.$key -or $candidate.$key -cne $compiler.$key) { throw 'Source candidate mismatch.' }
    }
    if ($product.executableSha256 -cne $Incident.executableSha256 -or $candidate.cpuPdbGuid -cne $product.cpuPdbGuid -or
        $candidate.cpuPdbAge -ne $product.cpuPdbAge -or $candidate.globalShaderMapSha256 -cne $bundle.globalShaderMapSha256 -or
        $candidate.cookedShaderLibrarySha256 -cne $bundle.cookedShaderLibrarySha256) { throw 'Product/cook/symbol identity mismatch.' }
    if ($product.product -cne $candidate.product -or $product.profile -cne $candidate.profile -or $product.variant -cne 'gpu-crash' -or
        $candidate.profile -cnotin @('DevelopmentEditor','DevelopmentGame','ShippingEditor','ShippingGame') -or
        $candidate.graphicsApi -cnotin @('D3D12','Vulkan') -or $product.cpuPdbSha256 -cne $candidate.cpuPdbSha256 -or
        $compiler.product -cne 'ShaderCompiler' -or $compiler.variant -cne 'gpu-crash' -or
        $compiler.executableSha256 -cne $bundle.shaderCompilerExecutableSha256) { throw 'Candidate product/API/compiler receipt mismatch.' }
    $nativeId = Get-NativeSymbolIdentity $candidate.cpuPdbPath
    if ($nativeId.Guid -cne $candidate.cpuPdbGuid -or $nativeId.Age -ne $candidate.cpuPdbAge) { throw 'CPU symbol identity mismatch.' }
    return $bundle
}
