#include "PCH.h"

#include "Shaders/GlobalShaderMap.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/BinarySpanReader.h"
#include "Core/Public/Files/FileUtils.h"

#include <format>
#include <string>

GlobalShaderMap GlobalShaderMap::Open(const std::filesystem::path& path, const CookedShaderLibrary& library)
{
	std::vector<std::uint8_t> fileBytes;
	std::string error;
	if (!Files::TryReadAllBytes(path, fileBytes, error))
	{
		throw Diagnostics::Error(error);
	}

	GlobalShaderMap map;
	map.ReadRecords(fileBytes, path, library);
	map.ValidateEntries(library);
	return map;
}

void GlobalShaderMap::ReadRecords(std::span<const std::uint8_t> fileBytes, const std::filesystem::path& path, const CookedShaderLibrary& library)
{
	std::string error;
	Files::BinarySpanReader reader(fileBytes);
	if (!reader.ReadValue(m_header, error) || m_header.Magic != kGlobalShaderMapMagic)
	{
		throw Diagnostics::Error(std::format("Invalid global shader map header in '{}'.", path.string()));
	}
	if (m_header.PublicationHash == 0 || m_header.PublicationHash != library.GetPublicationHash())
	{
		throw Diagnostics::Error("Global shader map and cooked shader library do not belong to the same publication.");
	}

	const bool bEntriesRead = reader.ReadArray(m_header.EntryCount, m_entries, error);
	const bool bBindingsRead = bEntriesRead && reader.ReadArray(m_header.BindingRecordCount, m_bindingRecords, error);
	const bool bReflectionRead = bBindingsRead && reader.ReadArray(m_header.ReflectionRecordCount, m_reflectionRecords, error);
	const bool bResourcesRead = bReflectionRead && reader.ReadArray(m_header.ResourceBindingRecordCount, m_resourceBindings, error);
	const bool bConstantBuffersRead = bResourcesRead && reader.ReadArray(m_header.ConstantBufferRecordCount, m_constantBuffers, error);
	const bool bConstantMembersRead = bConstantBuffersRead && reader.ReadArray(m_header.ConstantBufferMemberRecordCount, m_constantBufferMembers, error);
	const bool bInputElementsRead = bConstantMembersRead && reader.ReadArray(m_header.InputElementRecordCount, m_inputElements, error);
	const bool bPushConstantsRead = bInputElementsRead && reader.ReadArray(m_header.PushConstantRangeRecordCount, m_pushConstantRanges, error);
	const bool bSpecializationConstantsRead = bPushConstantsRead && reader.ReadArray(m_header.SpecializationConstantRecordCount, m_specializationConstants, error);
	const bool bStringsRead = bSpecializationConstantsRead && reader.ReadArray(m_header.StringTableSizeInBytes, m_stringTable, error);

	if (!bStringsRead)
	{
		throw Diagnostics::Error(error);
	}
	if (reader.GetRemainingByteCount() != 0)
	{
		throw Diagnostics::Error(std::format("Global shader map '{}' contains trailing bytes.", path.string()));
	}
}
