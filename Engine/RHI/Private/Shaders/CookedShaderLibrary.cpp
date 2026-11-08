#include "PCH.h"

#include "Shaders/GlobalShaderMap.h"
#include "Shaders/ShaderMapRanges.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/BinarySpanReader.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "Core/Public/Hash/HashUtils.h"

#include <algorithm>
#include <format>
#include <string>

using ShaderMapRanges::ContainsCode;

CookedShaderLibrary CookedShaderLibrary::Open(const std::filesystem::path& path)
{
	std::vector<std::uint8_t> fileBytes;
	std::string error;
	if (!Files::TryReadAllBytes(path, fileBytes, error))
	{
		throw Diagnostics::Error(error);
	}

	CookedShaderLibrary library;
	library.ReadRecords(fileBytes, path);
	library.ValidateCodeRecords();
	return library;
}

void CookedShaderLibrary::ReadRecords(std::span<const std::uint8_t> fileBytes, const std::filesystem::path& path)
{
	std::string error;
	Files::BinarySpanReader reader(fileBytes);
	if (!reader.ReadValue(m_header, error) || m_header.Magic != kCookedShaderLibraryMagic)
	{
		throw Diagnostics::Error(std::format("Invalid cooked shader library header in '{}'.", path.string()));
	}

	const bool bPublicationIdentified = m_header.PublicationHash != 0;
	const bool bRecordsRead = bPublicationIdentified && reader.ReadArray(m_header.RecordCount, m_records, error);
	const bool bCodeRead = bRecordsRead && reader.ReadArray(m_header.CodeBlobSizeInBytes, m_codeBlob, error);

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
}

void CookedShaderLibrary::ValidateCodeRecords() const
{
	ShaderCodeHash previousHash = 0;
	for (const CookedShaderCodeRecord& record : m_records)
	{
		if (record.CodeHash == 0 || (previousHash != 0 && record.CodeHash <= previousHash))
		{
			throw Diagnostics::Error("Cooked shader library index is not strictly ordered by unique code hash.");
		}
		if (!ContainsCode(m_codeBlob, record.Code))
		{
			throw Diagnostics::Error("Cooked shader library contains a truncated code record.");
		}

		const std::uint8_t* code = m_codeBlob.data() + record.Code.OffsetInBytes;
		if (Hash::Fnv1a64(code, record.Code.SizeInBytes) != record.CodeHash)
		{
			throw Diagnostics::Error(std::format("Cooked shader library code hash {} does not match its bytes.", Formatting::FormatHexUInt64(record.CodeHash)));
		}
		previousHash = record.CodeHash;
	}
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
