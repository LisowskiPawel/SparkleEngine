#include "../PCH.h"

#include "RenderPassRuntimeCache.h"

#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "RHI/Public/Device/RenderDeviceServices.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"
#include "RHI/Public/Shaders/ShaderParameterLayoutBuilder.h"

#include <algorithm>
#include <array>
#include <format>
#include <unordered_set>

RenderPassRuntimeStorage::Holder::~Holder() noexcept = default;

RenderPassRuntimeCache::RenderPassRuntimeCache(RenderDeviceServices& deviceServices) :
    m_deviceServices(&deviceServices),
    m_renderHardwareInterface(&deviceServices.GetRenderHardwareInterface()),
    m_activeGeneration(OpenGeneration(1))
{
}

RenderPassRuntimeCache::~RenderPassRuntimeCache() noexcept = default;

std::uint64_t RenderPassRuntimeCache::GetShaderGeneration() const noexcept
{
	return m_activeGeneration != nullptr ? m_activeGeneration->Generation : 0;
}

std::uint64_t RenderPassRuntimeCache::GetShaderPublicationHash() const noexcept
{
	return m_activeGeneration != nullptr ? m_activeGeneration->Map.GetPublicationHash() : 0;
}

ShaderDiagnosticsSnapshot RenderPassRuntimeCache::CaptureShaderDiagnostics() const
{
	ShaderDiagnosticsSnapshot snapshot;
	if (!m_activeGeneration)
	{
		return snapshot;
	}
	snapshot.Generation = m_activeGeneration->Generation;
	snapshot.Target = m_activeGeneration->Target;
	for (const GlobalShaderMapEntry& entry : m_activeGeneration->Map.GetEntries())
	{
		if (entry.Target == snapshot.Target)
		{
			snapshot.Entries.push_back({entry.ShaderType, entry.CompileInputHash, std::string(m_activeGeneration->Map.ResolveString(entry.BackendName))});
		}
	}
	return snapshot;
}

void RenderPassRuntimeCache::ReloadShaders()
{
	PollRetiredGenerations();
	if (m_activeGeneration == nullptr || m_renderHardwareInterface == nullptr)
	{
		HandleRuntimeCreationFailure("Shader runtime generation has no active renderer owner.");
	}

	const std::uint64_t replacementGeneration = m_activeGeneration->Generation + 1;
	if (replacementGeneration == 0)
	{
		HandleRuntimeCreationFailure("Shader runtime generation identity exhausted.");
	}

	try
	{
		auto replacement = CreateReplacementGeneration(replacementGeneration);
		ActivateGeneration(std::move(replacement));
	}
	catch (const Diagnostics::Error& error)
	{
		throw Diagnostics::Error(std::string("Shader runtime replacement validation failed; active generation remains unchanged. ") + error.what());
	}
}

std::unique_ptr<RenderPassRuntimeStorage::ShaderGeneration> RenderPassRuntimeCache::CreateReplacementGeneration(std::uint64_t generation) const
{
	auto replacement = OpenGeneration(generation);
	for (const auto& [storageType, activeHolder] : m_activeGeneration->RuntimeStorageByType)
	{
		if (activeHolder == nullptr)
		{
			HandleRuntimeCreationFailure("Shader runtime cache contains an empty shader holder.");
		}
		replacement->RuntimeStorageByType.emplace(storageType, activeHolder->CreateReplacement(*m_renderHardwareInterface, *replacement));
	}
	return replacement;
}

void RenderPassRuntimeCache::ActivateGeneration(std::unique_ptr<RenderPassRuntimeStorage::ShaderGeneration> replacement)
{
	m_retiredGenerations.push_back(RetiredShaderRuntimeGeneration{.LastUse = CaptureLastSubmittedState(), .Runtime = std::move(m_activeGeneration)});
	m_activeGeneration = std::move(replacement);
}

std::unique_ptr<RenderPassRuntimeStorage::ShaderGeneration> RenderPassRuntimeCache::OpenGeneration(std::uint64_t generation) const
{
	const Files::FileSetReadScope publication(Filesystem::GetCookedShaderRootPath());
	auto result = std::make_unique<RenderPassRuntimeStorage::ShaderGeneration>();
	result->Generation = generation;
	result->Target = GetRuntimeShaderTarget(m_renderHardwareInterface->GetCapabilities().RuntimeShaderBinaryFormat);
	result->Library = CookedShaderLibrary::Open(Filesystem::GetCookedShaderLibraryPath());
	result->Map = GlobalShaderMap::Open(Filesystem::GetGlobalShaderMapPath(), result->Library);
	ValidateGenerationContracts(*result);
	return result;
}

void RenderPassRuntimeCache::ValidateGenerationContracts(const RenderPassRuntimeStorage::ShaderGeneration& generation)
{
	std::unordered_set<ShaderTypeId> registeredShaderTypes;
	for (const ShaderRegistrationDesc& registration : GlobalShaderRegistry::GetRegistrations())
	{
		if (registration.TypeId == 0 || !registeredShaderTypes.insert(registration.TypeId).second)
		{
			throw Diagnostics::Error("Renderer shader registration catalog contains an invalid or duplicate shader type.");
		}

		const GlobalShaderMapEntry* const entry = generation.Map.Find(registration.TypeId, generation.Target);
		if (entry == nullptr)
		{
			throw Diagnostics::Error(std::format("Shader '{}' is missing from the active-target global shader map.", registration.ShaderName));
		}

		const PassParameterLayout parameterLayout = BuildShaderParameterLayout(registration);
		const std::uint64_t expectedParameterSignature = BuildShaderParameterSignature(parameterLayout);
		const bool bShaderNameMatches = generation.Map.ResolveString(entry->ShaderName) == registration.ShaderName;
		const bool bEntryPointMatches = bShaderNameMatches && generation.Map.ResolveString(entry->EntryPoint) == registration.EntryPoint;
		const bool bShaderMetadataMatchesStage = bEntryPointMatches && entry->Stage == registration.Stage;
		const bool bShaderMetadataMatches = bShaderMetadataMatchesStage && (entry->Features == registration.Features);

		const bool bRayContractMatchesRayTracing = bShaderMetadataMatches && entry->RayPayloadSizeInBytes == registration.RayTracing.PayloadSizeInBytes;
		const bool bRayContractMatchesRayTracing2 = bRayContractMatchesRayTracing && (entry->RayAttributeSizeInBytes == registration.RayTracing.AttributeSizeInBytes);
		const bool bRayTracingContractMatches = bRayContractMatchesRayTracing2 && (entry->MinimumRayRecursionDepth == registration.RayTracing.MinimumRecursionDepth);

		const bool bLocalRecordMatchesRayTracing = bRayTracingContractMatches && entry->LocalRecordSizeInBytes == registration.RayTracing.LocalRecordSizeInBytes;
		const bool bLocalRecordMatches = bLocalRecordMatchesRayTracing && (entry->LocalRecordSignature == registration.RayTracing.LocalRecordSignature);

		const bool bParameterSignatureMatches = bLocalRecordMatches && entry->ParameterSignature == expectedParameterSignature;

		if (!bParameterSignatureMatches)
		{
			throw Diagnostics::Error(
			    std::format(
			        "Shader '{}' map entry does not match its registered contract (parameter signature map={:016X}, runtime={:016X}).",
			        registration.ShaderName,
			        entry->ParameterSignature,
			        expectedParameterSignature));
		}
	}
}

void RenderPassRuntimeCache::PollRetiredGenerations() noexcept
{
	m_retiredGenerations.erase(
	    std::remove_if(m_retiredGenerations.begin(), m_retiredGenerations.end(), [this](const RetiredShaderRuntimeGeneration& generation) noexcept { return IsComplete(generation.LastUse); }),
	    m_retiredGenerations.end());
}

RhiSubmissionState RenderPassRuntimeCache::CaptureLastSubmittedState() const noexcept
{
	RhiSubmissionState state;
	if (m_deviceServices == nullptr)
	{
		return state;
	}

	for (std::size_t queueIndex = 0; queueIndex < RhiQueueTypeCount; ++queueIndex)
	{
		state.MarkUsed(m_deviceServices->GetLastSubmittedToken(static_cast<ERhiQueueType>(queueIndex)));
	}
	return state;
}

bool RenderPassRuntimeCache::IsComplete(const RhiSubmissionState& state) const noexcept
{
	if (m_deviceServices == nullptr)
	{
		return true;
	}

	std::array<RhiSubmissionToken, RhiQueueTypeCount> tokens{};
	const std::size_t tokenCount = state.CopyTokens(tokens);
	for (std::size_t tokenIndex = 0; tokenIndex < tokenCount; ++tokenIndex)
	{
		if (!m_deviceServices->IsSubmissionComplete(tokens[tokenIndex]))
		{
			return false;
		}
	}
	return true;
}

std::unique_ptr<RayTracingShaderTable> RenderPassRuntimeCache::CreateRayTracingShaderTable(
    const RayTracingPipelineRuntime& runtime,
    const RayTracingPipelineComposition& composition,
    const RayTracingShaderTablePlan* plan) const noexcept
{
	try
	{
		return plan != nullptr ? runtime.CreateShaderTable(*m_renderHardwareInterface, composition, *plan) : runtime.CreateShaderTable(*m_renderHardwareInterface, composition);
	}
	catch (const Diagnostics::Error& error)
	{
		HandleRuntimeCreationFailure(error.what());
	}
}

[[noreturn]] void RenderPassRuntimeCache::HandleRuntimeCreationFailure(std::string_view errorMessage) const
{
	SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogRenderer, "Renderer");
	Diagnostics::Fatal(LogRenderer, __FILE__, __LINE__, errorMessage);
}
