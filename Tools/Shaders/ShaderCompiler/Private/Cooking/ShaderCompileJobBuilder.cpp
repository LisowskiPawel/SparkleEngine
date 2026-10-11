#include "PCH.h"

#include "Cooking/ShaderCompileJobBuilder.h"

#include "Backend/IShaderBackend.h"
#include "Backend/ShaderBackendPool.h"
#include "Compiler/ShaderCompileProfile.h"
#include "Compiler/ShaderSourceMountTable.h"
#include "Compiler/ShaderSourcePreprocessor.h"
#include "Cooking/Identity/IncludeClosureHasher.h"
#include "Cooking/Identity/ShaderCompileRequestHasher.h"
#include "Cooking/ShaderCookSettings.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Hash/HashUtils.h"

#include <algorithm>
#include <format>
#include <iterator>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace ShaderDescriptorBindings
{
	auto GetIdentity(const PassParameterDesc* parameter) noexcept
	{
		return std::tie(parameter->Name, parameter->Kind, parameter->ResourceDomain, parameter->Access);
	}
}

ShaderCompileJobBuilder::ShaderCompileJobBuilder(const ShaderContractCatalog& catalog)
{
	for (const ShaderContract& shader : catalog)
	{
		for (const PassParameterDesc& parameter : shader.parameterLayout.GetParameters())
		{
			if (parameter.Kind != ShaderParameterSemanticKind::RenderTarget && parameter.Kind != ShaderParameterSemanticKind::DepthTarget)
			{
				m_descriptorBindings.push_back(&parameter);
			}
		}
	}
	std::ranges::sort(m_descriptorBindings, {}, ShaderDescriptorBindings::GetIdentity);
	const auto duplicates = std::ranges::unique(m_descriptorBindings, {}, ShaderDescriptorBindings::GetIdentity);
	m_descriptorBindings.erase(duplicates.begin(), duplicates.end());
}

ShaderCompileInputHash ShaderCompileJobBuilder::BuildInputHash(
    std::uint64_t sourceContentHash,
    std::uint64_t dependencyClosureHash,
    std::uint64_t requestHash,
    std::string_view backendName,
    std::uint64_t backendVersion)
{
	std::uint64_t hash = Hash::kFnv64OffsetBasis;
	hash = Hash::ContinueFnv1a64Value(hash, sourceContentHash);
	hash = Hash::ContinueFnv1a64Value(hash, dependencyClosureHash);
	hash = Hash::ContinueFnv1a64Value(hash, requestHash);
	hash = Hash::ContinueFnv1a64(hash, backendName.data(), backendName.size());
	hash = Hash::ContinueFnv1a64Value(hash, backendVersion);
	return Hash::FinalizeFnv1a64(hash);
}

ShaderCompileJob ShaderCompileJobBuilder::Build(const ShaderCookSettings& settings, const ShaderCookDesc& shader, ShaderTarget target, ShaderBackendPool& backendPool) const
{
	ShaderCompileRequest request = BuildRequest(settings, shader, target);
	IShaderBackend& backend = backendPool.ResolveAndAcquire(request.VirtualSourcePath, request.Target, settings.backendName);
	const IncludeClosureHash before = IncludeClosureHasher::Compute(request);
	request.SourceCode = ShaderSourcePreprocessor::Load(request.VirtualSourcePath, request);
	const IncludeClosureHash closure = IncludeClosureHasher::Compute(request);
	if (before.sourceHash != closure.sourceHash || before.includeClosureHash != closure.includeClosureHash || before.virtualDependencies != closure.virtualDependencies)
	{
		throw Diagnostics::Error(std::format("Shader source closure changed while constructing compile job for '{}'.", shader.shaderTypeName));
	}
	const std::uint64_t sourceHash = Hash::Fnv1a64(request.SourceCode);
	const std::uint64_t requestHash = ShaderCompileRequestHasher::Compute(request);
	const std::string backendName(backend.GetBackendName());
	const std::uint64_t backendVersion = backend.GetBackendVersion();
	const std::string targetProfile = ShaderCompileProfile::BuildTargetProfile(request);

	return ShaderCompileJob{
	    .Request = std::move(request),
	    .BackendName = backendName,
	    .TargetProfile = targetProfile,
	    .BackendVersion = backendVersion,
	    .SourceContentHash = sourceHash,
	    .DependencyClosureHash = closure.includeClosureHash,
	    .RequestHash = requestHash,
	    .InputHash = BuildInputHash(sourceHash, closure.includeClosureHash, requestHash, backendName, backendVersion),
	    .VirtualDependencies = closure.virtualDependencies};
}

ShaderCompileRequest ShaderCompileJobBuilder::BuildRequest(const ShaderCookSettings& settings, const ShaderCookDesc& shader, ShaderTarget target) const
{
	ShaderCompileRequest request(GetSourceMounts());
	request.ShaderType = shader.shaderTypeId;
	request.ShaderTypeName = shader.shaderTypeName;
	request.VirtualSourcePath = request.SourceMounts.get().CanonicalizeVirtualPath(shader.sourcePath);
	request.EntryPoint = shader.entryPoint;
	request.Stage = shader.stage;
	request.Target = target;
	request.UnitKind = IsRayTracingShaderStage(shader.stage) ? ShaderCompileUnitKind::Library : ShaderCompileUnitKind::EntryPoint;
	request.RequiredFeatures = BuildRequiredFeatures(shader.features);
	if (IsRayTracingShaderStage(shader.stage))
	{
		request.RequiredFeatures |= ShaderCompileFeatureFlags::RayTracingPipeline;
	}
	request.ParameterStruct = shader.parameterStruct;
	request.CaptureDebugArtifacts = !settings.debugArtifactDirectory.empty();
	request.EnableDebugInfo = settings.enableDebugInfo;
	request.EnableOptimizations = settings.enableOptimizations;
	request.TreatWarningsAsErrors = settings.treatWarningsAsErrors;
	request.StripDebugInfo = settings.stripDebugInfo;
	AppendDescriptorBindingRemaps(shader, request);
	return request;
}

void ShaderCompileJobBuilder::AppendDescriptorBindingRemaps(const ShaderCookDesc& shader, ShaderCompileRequest& request) const
{
	if (shader.parameterLayout.GetParameters().empty())
	{
		for (std::size_t index = 0; index < m_descriptorBindings.size(); ++index)
		{
			const PassParameterDesc& binding = *m_descriptorBindings[index];
			const bool duplicateBefore = index > 0 && m_descriptorBindings[index - 1]->Name == binding.Name;
			const bool duplicateAfter = index + 1 < m_descriptorBindings.size() && m_descriptorBindings[index + 1]->Name == binding.Name;
			if (!duplicateBefore && !duplicateAfter)
			{
				request.DescriptorBindingRemaps.push_back(ShaderDescriptorBindingRemap{.Name = binding.Name, .Set = 0, .Binding = static_cast<std::uint32_t>(index)});
			}
		}
		return;
	}
	for (const PassParameterDesc& parameter : shader.parameterLayout.GetParameters())
	{
		if (parameter.Kind == ShaderParameterSemanticKind::RenderTarget || parameter.Kind == ShaderParameterSemanticKind::DepthTarget)
		{
			continue;
		}
		const auto identity = ShaderDescriptorBindings::GetIdentity(&parameter);
		const auto found = std::ranges::lower_bound(m_descriptorBindings, identity, {}, ShaderDescriptorBindings::GetIdentity);
		if (found == m_descriptorBindings.end() || ShaderDescriptorBindings::GetIdentity(*found) != identity)
		{
			throw Diagnostics::Error(std::format("Shader parameter '{}' is missing from the global binding assignment.", parameter.Name));
		}

		request.DescriptorBindingRemaps.push_back(
		    ShaderDescriptorBindingRemap{.Name = parameter.Name, .Set = 0, .Binding = static_cast<std::uint32_t>(std::distance(m_descriptorBindings.begin(), found))});
	}
}

ShaderCompileFeatureFlags ShaderCompileJobBuilder::BuildRequiredFeatures(ShaderFeatureFlags features) noexcept
{
	ShaderCompileFeatureFlags result = ShaderCompileFeatureFlags::None;
	if (HasShaderFeature(features, ShaderFeatureFlags::UsesInlineRayQuery))
	{
		result |= ShaderCompileFeatureFlags::InlineRayQuery;
	}
	if (HasShaderFeature(features, ShaderFeatureFlags::UsesAccelerationStructure))
	{
		result |= ShaderCompileFeatureFlags::AccelerationStructure;
	}
	if (HasShaderFeature(features, ShaderFeatureFlags::UsesDescriptorIndexing))
	{
		result |= ShaderCompileFeatureFlags::DescriptorIndexing;
	}
	return result;
}

const ShaderSourceMountTable& ShaderCompileJobBuilder::GetSourceMounts()
{
	static const ShaderSourceMountTable mounts(Filesystem::GetShaderPath(PathRoot::Engine), Filesystem::GetShaderPath(PathRoot::Project));
	return mounts;
}
