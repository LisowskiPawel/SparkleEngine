#include "PCH.h"

#include "Shaders/GlobalShaderMap.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/BinarySpanReader.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "Core/Public/Hash/HashUtils.h"

#include <algorithm>
#include <cstddef>
#include <format>
#include <string>

ShaderFeatureFlags operator|(ShaderFeatureFlags lhs, ShaderFeatureFlags rhs) noexcept
{
	return static_cast<ShaderFeatureFlags>(static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs));
}

ShaderFeatureFlags& operator|=(ShaderFeatureFlags& lhs, ShaderFeatureFlags rhs) noexcept
{
	lhs = lhs | rhs;
	return lhs;
}

bool HasShaderFeature(ShaderFeatureFlags value, ShaderFeatureFlags flag) noexcept
{
	return (static_cast<std::uint32_t>(value) & static_cast<std::uint32_t>(flag)) == static_cast<std::uint32_t>(flag);
}

static bool ContainsRange(std::size_t size, std::uint32_t offset, std::uint32_t count) noexcept
{
	return offset <= size && count <= size - offset;
}

static bool ContainsString(std::span<const std::uint8_t> table, ShaderMapStringRef ref) noexcept
{
	return ContainsRange(table.size(), ref.OffsetInBytes, ref.SizeInBytes);
}

static bool ContainsCode(std::span<const std::uint8_t> blob, ShaderCodeBlobRef ref) noexcept
{
	return ref.IsValid() && ContainsRange(blob.size(), ref.OffsetInBytes, ref.SizeInBytes);
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

	for (const CookedShaderResourceBindingRecord& resource : resources.subspan(reflection.ResourceBindingOffset, reflection.ResourceBindingCount))
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

	for (const CookedShaderConstantBufferRecord& buffer : constantBuffers.subspan(reflection.ConstantBufferOffset, reflection.ConstantBufferCount))
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

	for (const CookedShaderInputElementRecord& input : inputs.subspan(reflection.InputElementOffset, reflection.InputElementCount))
	{
		if (!ContainsRange(strings.size(), input.SemanticOffsetInBytes, input.SemanticSizeInBytes) || input.SemanticSizeInBytes == 0)
		{
			throw Diagnostics::Error("Global shader map contains an invalid reflected input element.");
		}
	}

	for (const CookedShaderSpecializationConstantRecord& value : specializationConstants.subspan(reflection.SpecializationConstantOffset, reflection.SpecializationConstantCount))
	{
		if (!ContainsRange(strings.size(), value.NameOffsetInBytes, value.NameSizeInBytes) || value.NameSizeInBytes == 0)
		{
			throw Diagnostics::Error("Global shader map contains an invalid reflected specialization constant.");
		}
	}
}

CookedShaderLibrary CookedShaderLibrary::Open(const std::filesystem::path& path)
{
	std::vector<std::uint8_t> fileBytes;
	std::string error;
	if (!Files::TryReadAllBytes(path, fileBytes, error))
	{
		throw Diagnostics::Error(error);
	}

	CookedShaderLibrary library;
	Files::BinarySpanReader reader(fileBytes);
	if (!reader.ReadValue(library.m_header, error) || library.m_header.Magic != kCookedShaderLibraryMagic)
	{
		throw Diagnostics::Error(std::format("Invalid cooked shader library header in '{}'.", path.string()));
	}

	const bool bPublicationIdentified = library.m_header.PublicationHash != 0;
	const bool bRecordsRead = bPublicationIdentified && reader.ReadArray(library.m_header.RecordCount, library.m_records, error);
	const bool bCodeRead = bRecordsRead && reader.ReadArray(library.m_header.CodeBlobSizeInBytes, library.m_codeBlob, error);

	if (!bCodeRead)
	{
		if (error.empty())
		{
			throw Diagnostics::Error("Cooked shader library has no publication identity.");
		}
		throw Diagnostics::Error(error);
	}
	if (reader.GetRemainingByteCount() != 0)
	{
		throw Diagnostics::Error(std::format("Cooked shader library '{}' contains trailing bytes.", path.string()));
	}

	ShaderCodeHash previousHash = 0;
	for (const CookedShaderCodeRecord& record : library.m_records)
	{
		if (record.CodeHash == 0 || (previousHash != 0 && record.CodeHash <= previousHash))
		{
			throw Diagnostics::Error("Cooked shader library index is not strictly ordered by unique code hash.");
		}
		if (!ContainsCode(library.m_codeBlob, record.Code))
		{
			throw Diagnostics::Error("Cooked shader library contains a truncated code record.");
		}

		const std::uint8_t* code = library.m_codeBlob.data() + record.Code.OffsetInBytes;
		if (Hash::Fnv1a64(code, record.Code.SizeInBytes) != record.CodeHash)
		{
			throw Diagnostics::Error(std::format("Cooked shader library code hash {} does not match its bytes.", Formatting::FormatHexUInt64(record.CodeHash)));
		}
		previousHash = record.CodeHash;
	}
	return library;
}

const CookedShaderCodeRecord* CookedShaderLibrary::Find(ShaderCodeHash codeHash) const noexcept
{
	const auto found = std::lower_bound(m_records.begin(), m_records.end(), codeHash, [](const CookedShaderCodeRecord& record, ShaderCodeHash value) { return record.CodeHash < value; });
	return found != m_records.end() && found->CodeHash == codeHash ? &*found : nullptr;
}

ShaderBytecode CookedShaderLibrary::GetBytecode(const CookedShaderCodeRecord& record) const noexcept
{
	if (!ContainsCode(m_codeBlob, record.Code))
	{
		return {};
	}
	return ShaderBytecode{m_codeBlob.data() + record.Code.OffsetInBytes, record.Code.SizeInBytes};
}

GlobalShaderMap GlobalShaderMap::Open(const std::filesystem::path& path, const CookedShaderLibrary& library)
{
	std::vector<std::uint8_t> fileBytes;
	std::string error;
	if (!Files::TryReadAllBytes(path, fileBytes, error))
	{
		throw Diagnostics::Error(error);
	}

	GlobalShaderMap map;
	Files::BinarySpanReader reader(fileBytes);
	if (!reader.ReadValue(map.m_header, error) || map.m_header.Magic != kGlobalShaderMapMagic)
	{
		throw Diagnostics::Error(std::format("Invalid global shader map header in '{}'.", path.string()));
	}
	if (map.m_header.PublicationHash == 0 || map.m_header.PublicationHash != library.GetPublicationHash())
	{
		throw Diagnostics::Error("Global shader map and cooked shader library do not belong to the same publication.");
	}

	const bool bEntriesRead = reader.ReadArray(map.m_header.EntryCount, map.m_entries, error);
	const bool bBindingsRead = bEntriesRead && reader.ReadArray(map.m_header.BindingRecordCount, map.m_bindingRecords, error);
	const bool bReflectionRead = bBindingsRead && reader.ReadArray(map.m_header.ReflectionRecordCount, map.m_reflectionRecords, error);
	const bool bResourcesRead = bReflectionRead && reader.ReadArray(map.m_header.ResourceBindingRecordCount, map.m_resourceBindings, error);
	const bool bConstantBuffersRead = bResourcesRead && reader.ReadArray(map.m_header.ConstantBufferRecordCount, map.m_constantBuffers, error);
	const bool bConstantMembersRead = bConstantBuffersRead && reader.ReadArray(map.m_header.ConstantBufferMemberRecordCount, map.m_constantBufferMembers, error);
	const bool bInputElementsRead = bConstantMembersRead && reader.ReadArray(map.m_header.InputElementRecordCount, map.m_inputElements, error);
	const bool bPushConstantsRead = bInputElementsRead && reader.ReadArray(map.m_header.PushConstantRangeRecordCount, map.m_pushConstantRanges, error);
	const bool bSpecializationConstantsRead = bPushConstantsRead && reader.ReadArray(map.m_header.SpecializationConstantRecordCount, map.m_specializationConstants, error);
	const bool bStringsRead = bSpecializationConstantsRead && reader.ReadArray(map.m_header.StringTableSizeInBytes, map.m_stringTable, error);

	if (!bStringsRead)
	{
		throw Diagnostics::Error(error);
	}
	if (reader.GetRemainingByteCount() != 0)
	{
		throw Diagnostics::Error(std::format("Global shader map '{}' contains trailing bytes.", path.string()));
	}

	ShaderTypeId previousType = 0;
	ShaderTarget previousTarget = kDefaultShaderTarget;
	bool bHasPrevious = false;
	for (const GlobalShaderMapEntry& entry : map.m_entries)
	{
		const bool bOrderedShaderType = !bHasPrevious || previousType < entry.ShaderType;
		const bool bOrderedTarget = previousType == entry.ShaderType && static_cast<std::uint16_t>(previousTarget) < static_cast<std::uint16_t>(entry.Target);
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
		if (entry.BinaryFormat != GetShaderBinaryFormat(entry.Target) || library.Find(entry.CodeHash) == nullptr)
		{
			throw Diagnostics::Error("Global shader map entry references missing or incompatible shader code.");
		}

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

		const bool bNamesValidEntryPoint = entry.ShaderName.IsValid() && entry.EntryPoint.IsValid();
		const bool bNamesValidBackendName = bNamesValidEntryPoint && (entry.BackendName.IsValid());
		const bool bNamesValid = bNamesValidBackendName && (entry.CodegenTarget.IsValid());
		const bool bShaderNameInRange = bNamesValid && ContainsString(map.m_stringTable, entry.ShaderName);
		const bool bEntryPointInRange = bShaderNameInRange && ContainsString(map.m_stringTable, entry.EntryPoint);
		const bool bBackendNameInRange = bEntryPointInRange && ContainsString(map.m_stringTable, entry.BackendName);
		const bool bCodegenTargetInRange = bBackendNameInRange && ContainsString(map.m_stringTable, entry.CodegenTarget);
		const bool bBindingsInRange = bCodegenTargetInRange && ContainsRange(map.m_bindingRecords.size(), entry.BindingRecordOffset, entry.BindingRecordCount);
		const bool bReflectionInRange = bBindingsInRange && entry.ReflectionRecordIndex < map.m_reflectionRecords.size();

		if (!bReflectionInRange)
		{
			throw Diagnostics::Error("Global shader map entry contains an out-of-range metadata reference.");
		}

		const ShaderTypeId resolvedShaderType = BuildShaderTypeId(map.ResolveString(entry.ShaderName));
		const bool bShaderTypeMatches = resolvedShaderType == entry.ShaderType;
		const bool bCodegenTargetMatches = bShaderTypeMatches && map.ResolveString(entry.CodegenTarget) == GetShaderTargetName(entry.Target);

		if (!bCodegenTargetMatches)
		{
			throw Diagnostics::Error("Global shader map entry contains inconsistent identity or target provenance.");
		}
		ValidateReflection(
		    map.m_reflectionRecords[entry.ReflectionRecordIndex],
		    map.m_resourceBindings,
		    map.m_constantBuffers,
		    map.m_constantBufferMembers,
		    map.m_inputElements,
		    map.m_pushConstantRanges,
		    map.m_specializationConstants,
		    map.m_stringTable);
		for (const ShaderMapBindingRecord& binding : std::span<const ShaderMapBindingRecord>(map.m_bindingRecords).subspan(entry.BindingRecordOffset, entry.BindingRecordCount))
		{
			if (!ContainsString(map.m_stringTable, binding.Name) || !binding.Name.IsValid() || binding.ArrayCount == 0)
			{
				throw Diagnostics::Error("Global shader map entry contains an invalid parameter binding.");
			}
		}
		previousType = entry.ShaderType;
		previousTarget = entry.Target;
		bHasPrevious = true;
	}
	return map;
}

const GlobalShaderMapEntry* GlobalShaderMap::Find(ShaderTypeId shaderType, ShaderTarget target) const noexcept
{
	const auto found = std::lower_bound(
	    m_entries.begin(),
	    m_entries.end(),
	    std::pair{shaderType, target},
	    [](const GlobalShaderMapEntry& entry, const std::pair<ShaderTypeId, ShaderTarget>& key)
	    { return entry.ShaderType < key.first || (entry.ShaderType == key.first && entry.Target < key.second); });
	return found != m_entries.end() && found->ShaderType == shaderType && found->Target == target ? &*found : nullptr;
}

std::string_view GlobalShaderMap::ResolveString(ShaderMapStringRef ref) const noexcept
{
	if (!ContainsString(m_stringTable, ref))
	{
		return {};
	}
	return {reinterpret_cast<const char*>(m_stringTable.data() + ref.OffsetInBytes), ref.SizeInBytes};
}

std::span<const ShaderMapBindingRecord> GlobalShaderMap::GetBindings(const GlobalShaderMapEntry& entry) const noexcept
{
	if (!ContainsRange(m_bindingRecords.size(), entry.BindingRecordOffset, entry.BindingRecordCount))
	{
		return {};
	}
	return std::span<const ShaderMapBindingRecord>(m_bindingRecords).subspan(entry.BindingRecordOffset, entry.BindingRecordCount);
}

const CookedShaderReflectionRecord& GlobalShaderMap::GetReflection(const GlobalShaderMapEntry& entry) const noexcept
{
	return m_reflectionRecords[entry.ReflectionRecordIndex];
}
