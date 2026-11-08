#include "PCH.h"

#include "Shaders/ShaderMapRanges.h"

namespace ShaderMapRanges
{
	bool ContainsRange(std::size_t size, std::uint32_t offset, std::uint32_t count) noexcept
	{
		return offset <= size && count <= size - offset;
	}

	bool ContainsString(std::span<const std::uint8_t> table, ShaderMapStringRef ref) noexcept
	{
		return ContainsRange(table.size(), ref.OffsetInBytes, ref.SizeInBytes);
	}

	bool ContainsCode(std::span<const std::uint8_t> blob, ShaderCodeBlobRef ref) noexcept
	{
		return ref.IsValid() && ContainsRange(blob.size(), ref.OffsetInBytes, ref.SizeInBytes);
	}
}
