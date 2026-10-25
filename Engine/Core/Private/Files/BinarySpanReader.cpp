#include "PCH.h"

#include "Core/Public/Files/BinarySpanReader.h"

namespace Files
{
	BinarySpanReader::BinarySpanReader(std::span<const std::uint8_t> bytes) noexcept :
	    m_bytes(bytes)
	{
	}

	bool BinarySpanReader::ReadBytes(std::size_t byteCount, std::span<const std::uint8_t>& outBytes, std::string& outErrorMessage)
	{
		if (!CanRead(byteCount))
		{
			outErrorMessage = "Unexpected end of binary byte payload";
			return false;
		}

		outBytes = m_bytes.subspan(m_offset, byteCount);
		m_offset += byteCount;
		return true;
	}

	bool BinarySpanReader::ReadStringWithUInt32Length(std::string& outValue, std::string& outErrorMessage)
	{
		std::uint32_t sizeInBytes = 0;
		if (!ReadValue(sizeInBytes, outErrorMessage))
		{
			return false;
		}

		return ReadString(sizeInBytes, outValue, outErrorMessage);
	}

	bool BinarySpanReader::ReadString(std::size_t byteCount, std::string& outValue, std::string& outErrorMessage)
	{
		if (!CanRead(byteCount))
		{
			outErrorMessage = "Unexpected end of binary byte payload";
			return false;
		}
		if (byteCount == 0)
		{
			outValue.clear();
		}
		else
		{
			outValue.assign(reinterpret_cast<const char*>(m_bytes.data() + m_offset), byteCount);
		}
		m_offset += byteCount;
		return true;
	}

	bool BinarySpanReader::CanRead(std::size_t byteCount) const noexcept
	{
		return byteCount <= m_bytes.size() - m_offset;
	}
}
