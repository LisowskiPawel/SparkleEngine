#include "PCH.h"
#include "Cooking/Compilation/ShaderCompileJob.h"

#include <tuple>
#include <algorithm>

bool ShaderCompileJob::HasSameCompilerInput(const ShaderCompileJob& other) const noexcept
{
	const auto sourceIdentity = std::tie(SourceContentHash, DependencyClosureHash, RequestHash, VirtualDependencies);
	const auto otherSourceIdentity = std::tie(other.SourceContentHash, other.DependencyClosureHash, other.RequestHash, other.VirtualDependencies);
	if (sourceIdentity != otherSourceIdentity)
	{
		return false;
	}

	const auto backendIdentity = std::tie(BackendName, BackendVersion, TargetProfile);
	const auto otherBackendIdentity = std::tie(other.BackendName, other.BackendVersion, other.TargetProfile);
	if (backendIdentity != otherBackendIdentity)
	{
		return false;
	}

	const auto programIdentity = std::tie(Request.VirtualSourcePath, Request.SourceCode, Request.EntryPoint, Request.Stage, Request.Target, Request.UnitKind, Request.RequiredFeatures);

	const auto otherProgramIdentity = std::tie(
	    other.Request.VirtualSourcePath,
	    other.Request.SourceCode,
	    other.Request.EntryPoint,
	    other.Request.Stage,
	    other.Request.Target,
	    other.Request.UnitKind,
	    other.Request.RequiredFeatures);

	if (programIdentity != otherProgramIdentity)
	{
		return false;
	}

	const auto options = std::tie(Request.bEnableDebugInfo, Request.bEnableOptimizations, Request.bTreatWarningsAsErrors, Request.bStripDebugInfo, Request.bCaptureDebugArtifacts, Request.Defines);

	const auto otherOptions = std::tie(
	    other.Request.bEnableDebugInfo,
	    other.Request.bEnableOptimizations,
	    other.Request.bTreatWarningsAsErrors,
	    other.Request.bStripDebugInfo,
	    other.Request.bCaptureDebugArtifacts,
	    other.Request.Defines);

	if (options != otherOptions)
	{
		return false;
	}

	const bool bCapturedSourcesMatch = std::ranges::equal(
	    Request.SourceFiles,
	    other.Request.SourceFiles,
	    [](const ShaderSourceFile& source, const ShaderSourceFile& candidate) { return source.VirtualPath == candidate.VirtualPath && source.Contents == candidate.Contents; });
	return bCapturedSourcesMatch && Request.DescriptorBindingRemaps == other.Request.DescriptorBindingRemaps;
}
