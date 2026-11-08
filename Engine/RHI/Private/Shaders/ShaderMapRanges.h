#pragma once

#include "Shaders/ShaderMap.h"

#include <cstddef>
#include <cstdint>
#include <span>

namespace ShaderMapRanges
{
	bool ContainsRange(std::size_t size, std::uint32_t offset, std::uint32_t count) noexcept;
	bool ContainsString(std::span<const std::uint8_t> table, ShaderMapStringRef ref) noexcept;
	bool ContainsCode(std::span<const std::uint8_t> blob, ShaderCodeBlobRef ref) noexcept;
}
