#include "PCH.h"

#include "Core/Public/Files/BinaryBufferWriter.h"

namespace Files
{
	BinaryBufferWriter::BinaryBufferWriter(std::vector<std::uint8_t>& bytes) noexcept :
	    m_bytes(bytes)
	{
	}

	void BinaryBufferWriter::WriteBytes(std::span<const std::uint8_t> values)
	{
		m_bytes.insert(m_bytes.end(), values.begin(), values.end());
	}

}
