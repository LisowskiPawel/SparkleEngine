#include "PCH.h"

#include "Shaders/GlobalShaderMap.h"
#include "Shaders/ShaderMapRanges.h"
#include "Core/Public/Diagnostics/Error.h"

using ShaderMapRanges::ContainsRange;
using ShaderMapRanges::ContainsString;

static void ValidateReflectedResources(const CookedShaderReflectionRecord& reflection, std::span<const CookedShaderResourceBindingRecord> resources, std::span<const std::uint8_t> strings)
{
	for (const CookedShaderResourceBindingRecord& resource : resources)
	{
		const bool bValidConstantBufferBufferIndex = resource.ConstantBufferIndex == kCookedShaderReflectionInvalidIndex;

		const bool bValidConstantBufferBufferCount = resource.ConstantBufferIndex >= reflection.ConstantBufferOffset
		    && resource.ConstantBufferIndex < reflection.ConstantBufferOffset + reflection.ConstantBufferCount;

		const bool bValidConstantBuffer = bValidConstantBufferBufferIndex || bValidConstantBufferBufferCount;
		const bool bInvalidReflectedResourceInBytes = !ContainsRange(strings.size(), resource.NameOffsetInBytes, resource.NameSizeInBytes) || resource.NameSizeInBytes == 0;
		const bool bReflectedResourceArrayCount = bInvalidReflectedResourceInBytes || (resource.ArrayCount == 0);
		const bool bInvalidReflectedResource = bReflectedResourceArrayCount || (!bValidConstantBuffer);

		if (bInvalidReflectedResource)
		{
			throw Diagnostics::Error("Global shader map contains an invalid reflected resource binding.");
		}
	}
}

static void ValidateReflectedConstantBuffers(
    std::span<const CookedShaderConstantBufferRecord> constantBuffers,
    std::span<const CookedShaderConstantBufferMemberRecord> constantBufferMembers,
    std::span<const std::uint8_t> strings)
{
	for (const CookedShaderConstantBufferRecord& buffer : constantBuffers)
	{
		const bool bNameInRange = ContainsRange(strings.size(), buffer.NameOffsetInBytes, buffer.NameSizeInBytes) && buffer.NameSizeInBytes != 0;
		const bool bMembersInRange = bNameInRange && ContainsRange(constantBufferMembers.size(), buffer.MemberOffset, buffer.MemberCount);

		if (!bMembersInRange)
		{
			throw Diagnostics::Error("Global shader map contains an invalid reflected constant buffer.");
		}
		for (const CookedShaderConstantBufferMemberRecord& member : constantBufferMembers.subspan(buffer.MemberOffset, buffer.MemberCount))
		{
			const bool bInvalidReflectedMemberInBytes = !ContainsRange(strings.size(), member.NameOffsetInBytes, member.NameSizeInBytes) || member.NameSizeInBytes == 0;
			const bool bInvalidReflectedMemberArrayCount = bInvalidReflectedMemberInBytes || (member.ArrayCount == 0);
			const bool bOffsetInvalid = bInvalidReflectedMemberArrayCount || (member.OffsetInBytes > buffer.SizeInBytes);
			const bool bInvalidReflectedMember = bOffsetInvalid || (member.SizeInBytes > buffer.SizeInBytes - member.OffsetInBytes);

			if (bInvalidReflectedMember)
			{
				throw Diagnostics::Error("Global shader map contains an invalid reflected constant-buffer member.");
			}
		}
	}
}

static void ValidateReflectedInputs(std::span<const CookedShaderInputElementRecord> inputs, std::span<const std::uint8_t> strings)
{
	for (const CookedShaderInputElementRecord& input : inputs)
	{
		if (!ContainsRange(strings.size(), input.SemanticOffsetInBytes, input.SemanticSizeInBytes) || input.SemanticSizeInBytes == 0)
		{
			throw Diagnostics::Error("Global shader map contains an invalid reflected input element.");
		}
	}
}

static void ValidateReflectedSpecializationConstants(std::span<const CookedShaderSpecializationConstantRecord> specializationConstants, std::span<const std::uint8_t> strings)
{
	for (const CookedShaderSpecializationConstantRecord& value : specializationConstants)
	{
		if (!ContainsRange(strings.size(), value.NameOffsetInBytes, value.NameSizeInBytes) || value.NameSizeInBytes == 0)
		{
			throw Diagnostics::Error("Global shader map contains an invalid reflected specialization constant.");
		}
	}
}

static void ValidateReflection(
    const CookedShaderReflectionRecord& reflection,
    std::span<const CookedShaderResourceBindingRecord> resources,
    std::span<const CookedShaderConstantBufferRecord> constantBuffers,
    std::span<const CookedShaderConstantBufferMemberRecord> constantBufferMembers,
    std::span<const CookedShaderInputElementRecord> inputs,
    std::span<const CookedShaderPushConstantRangeRecord> pushConstants,
    std::span<const CookedShaderSpecializationConstantRecord> specializationConstants,
    std::span<const std::uint8_t> strings)
{
	const bool bResourcesInRange = ContainsRange(resources.size(), reflection.ResourceBindingOffset, reflection.ResourceBindingCount);
	const bool bConstantBuffersInRange = bResourcesInRange && ContainsRange(constantBuffers.size(), reflection.ConstantBufferOffset, reflection.ConstantBufferCount);
	const bool bInputsInRange = bConstantBuffersInRange && ContainsRange(inputs.size(), reflection.InputElementOffset, reflection.InputElementCount);
	const bool bPushConstantsInRange = bInputsInRange && ContainsRange(pushConstants.size(), reflection.PushConstantRangeOffset, reflection.PushConstantRangeCount);

	const bool bSpecializationConstantsInRange = bPushConstantsInRange
	    && ContainsRange(specializationConstants.size(), reflection.SpecializationConstantOffset, reflection.SpecializationConstantCount);

	if (!bSpecializationConstantsInRange)
	{
		throw Diagnostics::Error("Global shader map contains an out-of-range reflection record.");
	}

	ValidateReflectedResources(reflection, resources.subspan(reflection.ResourceBindingOffset, reflection.ResourceBindingCount), strings);
	ValidateReflectedConstantBuffers(constantBuffers.subspan(reflection.ConstantBufferOffset, reflection.ConstantBufferCount), constantBufferMembers, strings);
	ValidateReflectedInputs(inputs.subspan(reflection.InputElementOffset, reflection.InputElementCount), strings);
	ValidateReflectedSpecializationConstants(specializationConstants.subspan(reflection.SpecializationConstantOffset, reflection.SpecializationConstantCount), strings);
}

static void ValidateEntryKey(const GlobalShaderMapEntry& entry, const GlobalShaderMapEntry* previous)
{
	const bool bOrderedShaderType = previous == nullptr || previous->ShaderType < entry.ShaderType;
	const bool bSameShaderType = previous != nullptr && previous->ShaderType == entry.ShaderType;
	const bool bOrderedTarget = bSameShaderType && static_cast<std::uint16_t>(previous->Target) < static_cast<std::uint16_t>(entry.Target);
	const bool bOrdered = bOrderedShaderType || bOrderedTarget;
	const std::uint32_t featureBits = static_cast<std::uint32_t>(entry.Features);

	constexpr std::uint32_t knownFeatureBits = static_cast<std::uint32_t>(ShaderFeatureFlags::UsesInlineRayQuery) | static_cast<std::uint32_t>(ShaderFeatureFlags::UsesAccelerationStructure)
	    | static_cast<std::uint32_t>(ShaderFeatureFlags::UsesDescriptorIndexing) | static_cast<std::uint32_t>(ShaderFeatureFlags::UsesFloat64);

	const bool bInvalidShaderEntryCodeHash = entry.ShaderType == 0 || entry.CodeHash == 0;
	const bool bShaderEntryParameterSignature = bInvalidShaderEntryCodeHash || (entry.ParameterSignature == 0);
	const bool bInvalidShaderEntryInputHash = bShaderEntryParameterSignature || (entry.CompileInputHash == 0);
	const bool bInvalidShaderEntryBackendVersion = bInvalidShaderEntryInputHash || (entry.BackendVersion == 0);
	const bool bInvalidShaderEntryStage = bInvalidShaderEntryBackendVersion || (entry.Stage >= ShaderStage::Count);
	const bool bInvalidShaderEntryTarget = bInvalidShaderEntryStage || (!IsShaderTarget(entry.Target));
	const bool bInvalidShaderEntryFeatureBits = bInvalidShaderEntryTarget || ((featureBits & ~knownFeatureBits) != 0);
	const bool bInvalidShaderEntry = bInvalidShaderEntryFeatureBits || (!bOrdered);

	if (bInvalidShaderEntry)
	{
		throw Diagnostics::Error("Global shader map contains an invalid or duplicate logical key.");
	}
}

static void ValidateRayTracingContract(const GlobalShaderMapEntry& entry)
{
	const bool bHasLocalRecord = entry.LocalRecordSizeInBytes != 0 || entry.LocalRecordSignature != 0;
	const bool bHasTracingContractInBytes = entry.RayPayloadSizeInBytes != 0 || entry.RayAttributeSizeInBytes != 0;
	const bool bHasSharedRayTracingContract = bHasTracingContractInBytes || (entry.MinimumRayRecursionDepth != 0);
	const bool bLocalRecordMismatch = (entry.LocalRecordSizeInBytes == 0) != (entry.LocalRecordSignature == 0);
	const bool bRayGenerationIncompleteStage = !bLocalRecordMismatch && entry.Stage == ShaderStage::RayGeneration;
	const bool bRayGenerationIncompleteInBytes = entry.RayPayloadSizeInBytes == 0 || entry.RayAttributeSizeInBytes == 0;
	const bool bGenerationIncompleteDepth = bRayGenerationIncompleteInBytes || (entry.MinimumRayRecursionDepth == 0);
	const bool bRayGenerationIncomplete = bRayGenerationIncompleteStage && bGenerationIncompleteDepth;

	const bool bContractShapeValid = !bLocalRecordMismatch && !bRayGenerationIncomplete;
	const bool bSharedContractMisplacedStage = bContractShapeValid && (IsRayTracingShaderStage(entry.Stage));
	const bool bNonRayGenerationStage = bSharedContractMisplacedStage && (entry.Stage != ShaderStage::RayGeneration);
	const bool bSharedContractMisplaced = bNonRayGenerationStage && (bHasSharedRayTracingContract);

	const bool bContractMisplacedMisplaced = bContractShapeValid && (!bSharedContractMisplaced);
	const bool bRasterContractMisplacedStage = bContractMisplacedMisplaced && (!IsRayTracingShaderStage(entry.Stage));
	const bool bContractMisplacedLocalRecord = bHasSharedRayTracingContract || bHasLocalRecord;
	const bool bRasterContractMisplaced = bRasterContractMisplacedStage && bContractMisplacedLocalRecord;

	if (bLocalRecordMismatch || bRayGenerationIncomplete || bSharedContractMisplaced || bRasterContractMisplaced)
	{
		throw Diagnostics::Error("Global shader map entry contains an invalid ray-tracing contract.");
	}
}

void GlobalShaderMap::ValidateEntryMetadata(const GlobalShaderMapEntry& entry) const
{
	const bool bNamesValidEntryPoint = entry.ShaderName.IsValid() && entry.EntryPoint.IsValid();
	const bool bNamesValidBackendName = bNamesValidEntryPoint && (entry.BackendName.IsValid());
	const bool bNamesValid = bNamesValidBackendName && (entry.CodegenTarget.IsValid());
	const bool bShaderNameInRange = bNamesValid && ContainsString(m_stringTable, entry.ShaderName);
	const bool bEntryPointInRange = bShaderNameInRange && ContainsString(m_stringTable, entry.EntryPoint);
	const bool bBackendNameInRange = bEntryPointInRange && ContainsString(m_stringTable, entry.BackendName);
	const bool bCodegenTargetInRange = bBackendNameInRange && ContainsString(m_stringTable, entry.CodegenTarget);
	const bool bBindingsInRange = bCodegenTargetInRange && ContainsRange(m_bindingRecords.size(), entry.BindingRecordOffset, entry.BindingRecordCount);
	const bool bReflectionInRange = bBindingsInRange && entry.ReflectionRecordIndex < m_reflectionRecords.size();

	if (!bReflectionInRange)
	{
		throw Diagnostics::Error("Global shader map entry contains an out-of-range metadata reference.");
	}

	const ShaderTypeId resolvedShaderType = BuildShaderTypeId(ResolveString(entry.ShaderName));
	const bool bShaderTypeMatches = resolvedShaderType == entry.ShaderType;
	const bool bCodegenTargetMatches = bShaderTypeMatches && ResolveString(entry.CodegenTarget) == GetShaderTargetName(entry.Target);

	if (!bCodegenTargetMatches)
	{
		throw Diagnostics::Error("Global shader map entry contains inconsistent identity or target provenance.");
	}

	ValidateReflection(
	    m_reflectionRecords[entry.ReflectionRecordIndex],
	    m_resourceBindings,
	    m_constantBuffers,
	    m_constantBufferMembers,
	    m_inputElements,
	    m_pushConstantRanges,
	    m_specializationConstants,
	    m_stringTable);

	for (const ShaderMapBindingRecord& binding : std::span<const ShaderMapBindingRecord>(m_bindingRecords).subspan(entry.BindingRecordOffset, entry.BindingRecordCount))
	{
		if (!ContainsString(m_stringTable, binding.Name) || !binding.Name.IsValid() || binding.ArrayCount == 0)
		{
			throw Diagnostics::Error("Global shader map entry contains an invalid parameter binding.");
		}
	}
}

void GlobalShaderMap::ValidateEntries(const CookedShaderLibrary& library) const
{
	const GlobalShaderMapEntry* previous = nullptr;
	for (const GlobalShaderMapEntry& entry : m_entries)
	{
		ValidateEntryKey(entry, previous);
		if (entry.BinaryFormat != GetShaderBinaryFormat(entry.Target) || library.Find(entry.CodeHash) == nullptr)
		{
			throw Diagnostics::Error("Global shader map entry references missing or incompatible shader code.");
		}
		ValidateRayTracingContract(entry);
		ValidateEntryMetadata(entry);
		previous = &entry;
	}
}
