#include "PCH.h"

#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Strings/StringUtils.h"

#include <algorithm>
#include <format>
#include <fstream>
#include <limits>

namespace Files
{
	template <typename Buffer> static bool TryReadFile(const std::filesystem::path& path, Buffer& output, std::string& outErrorMessage, std::size_t maximumBytes, bool bAllowEmpty)
	{
		std::ifstream input(path, std::ios::binary | std::ios::ate);
		if (!input)
		{
			const std::string displayPath = Strings::ToNarrow(path.wstring());
			outErrorMessage = std::format("Failed to open '{}'", displayPath);
			return false;
		}

		const std::ifstream::pos_type fileSize = input.tellg();
		if (fileSize == 0 && !bAllowEmpty)
		{
			const std::string displayPath = Strings::ToNarrow(path.wstring());
			outErrorMessage = std::format("'{}' is empty", displayPath);
			return false;
		}
		const bool bInvalidSize = fileSize < 0 || static_cast<std::uint64_t>(fileSize) > maximumBytes;
		const auto storageLimit = std::min<std::uint64_t>(output.max_size(), static_cast<std::uint64_t>((std::numeric_limits<std::streamsize>::max)()));
		if (bInvalidSize || static_cast<std::uint64_t>(fileSize) > storageLimit)
		{
			const std::string displayPath = Strings::ToNarrow(path.wstring());
			outErrorMessage = std::format("'{}' is too large to load", displayPath);
			return false;
		}

		output.resize(static_cast<std::size_t>(fileSize));
		input.seekg(0, std::ios::beg);
		input.read(reinterpret_cast<char*>(output.data()), static_cast<std::streamsize>(fileSize));
		const bool bReadComplete = static_cast<bool>(input);
		const bool bAtEnd = bReadComplete && input.peek() == std::ifstream::traits_type::eof();
		if (!bAtEnd || input.bad())
		{
			const std::string displayPath = Strings::ToNarrow(path.wstring());
			outErrorMessage = std::format("Failed to read '{}'", displayPath);
			output.clear();
			return false;
		}

		outErrorMessage.clear();
		return true;
	}

	bool TryReadAllBytes(const std::filesystem::path& path, std::vector<std::uint8_t>& outBytes, std::string& outErrorMessage, std::size_t maximumBytes)
	{
		return TryReadFile(path, outBytes, outErrorMessage, maximumBytes, false);
	}

	bool TryReadAllText(const std::filesystem::path& path, std::string& outText, std::string& outErrorMessage, std::size_t maximumBytes)
	{
		return TryReadFile(path, outText, outErrorMessage, maximumBytes, true);
	}
}
