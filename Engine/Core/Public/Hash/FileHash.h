#pragma once

#include "HashUtils.h"

#include <cstdint>
#include <filesystem>
#include <string>

namespace Hash
{
	SPARKLE_CORE_API bool TryFnv1a64File(const std::filesystem::path& path, std::uint64_t& outHash, std::string& outErrorMessage);
	SPARKLE_CORE_API bool TrySha256File(const std::filesystem::path& path, Sha256Digest& outHash, std::string& outErrorMessage);
}
