#pragma once

#include "Core/Public/CoreAPI.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace Files
{
	SPARKLE_CORE_API bool TryReadAllBytes(const std::filesystem::path& path, std::vector<std::uint8_t>& outBytes, std::string& outErrorMessage);
	SPARKLE_CORE_API bool TryReadAllText(const std::filesystem::path& path, std::string& outText, std::string& outErrorMessage, std::size_t maximumBytes = (std::numeric_limits<std::size_t>::max)());
	SPARKLE_CORE_API bool TryWriteAllBytes(const std::filesystem::path& path, std::span<const std::byte> bytes, std::string& outErrorMessage);
	SPARKLE_CORE_API bool TryWriteAllBytes(const std::filesystem::path& path, const std::vector<std::uint8_t>& bytes, std::string& outErrorMessage);
	SPARKLE_CORE_API bool TryWriteNewFileDurable(const std::filesystem::path& path, std::span<const std::byte> bytes, std::string& outErrorMessage);
	SPARKLE_CORE_API bool TryWriteAllBytesAtomic(const std::filesystem::path& path, std::span<const std::byte> bytes, std::string& outErrorMessage);
	SPARKLE_CORE_API bool TryWriteAllText(const std::filesystem::path& path, std::string_view text, std::string& outErrorMessage);
	SPARKLE_CORE_API bool TryWriteAllTextAtomic(const std::filesystem::path& path, std::string_view text, std::string& outErrorMessage);
	SPARKLE_CORE_API bool TryOpenBinaryOutput(const std::filesystem::path& path, std::ofstream& output, std::string& outErrorMessage);
	SPARKLE_CORE_API bool TryOpenTextOutput(const std::filesystem::path& path, std::ofstream& output, std::string& outErrorMessage);
	SPARKLE_CORE_API std::filesystem::path BuildTemporaryPath(const std::filesystem::path& path, std::string_view suffix = ".tmp");
	SPARKLE_CORE_API bool TryFinalizeTemporaryFile(const std::filesystem::path& temporaryPath, const std::filesystem::path& finalPath, std::string& outErrorMessage);
	SPARKLE_CORE_API bool TryFinalizeTemporaryFileIfMissing(const std::filesystem::path& temporaryPath, const std::filesystem::path& finalPath, std::string& outErrorMessage);
	SPARKLE_CORE_API void CleanupTemporaryFile(const std::filesystem::path& temporaryPath, std::ofstream* output = nullptr) noexcept;
	SPARKLE_CORE_API bool TryCloseOutput(std::ofstream& output, const std::filesystem::path& path, std::string& outErrorMessage);
}
