#include "PCH.h"

#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Strings/StringUtils.h"

#include <format>
#include <fstream>
#include <limits>

namespace Files
{
	bool TryReadAllBytes(const std::filesystem::path& path, std::vector<std::uint8_t>& outBytes, std::string& outErrorMessage)
	{
		std::ifstream input(path, std::ios::binary | std::ios::ate);
		if (!input)
		{
			const std::string displayPath = Strings::ToNarrow(path.wstring());
			outErrorMessage = std::format("Failed to open '{}'", displayPath);
			return false;
		}

		const std::ifstream::pos_type fileSize = input.tellg();
		if (fileSize <= 0)
		{
			const std::string displayPath = Strings::ToNarrow(path.wstring());
			outErrorMessage = std::format("'{}' is empty", displayPath);
			return false;
		}
		if (static_cast<std::uint64_t>(fileSize) > (std::numeric_limits<std::size_t>::max)())
		{
			const std::string displayPath = Strings::ToNarrow(path.wstring());
			outErrorMessage = std::format("'{}' is too large to load", displayPath);
			return false;
		}

		outBytes.resize(static_cast<std::size_t>(fileSize));
		input.seekg(0, std::ios::beg);
		if (!input.read(reinterpret_cast<char*>(outBytes.data()), fileSize))
		{
			const std::string displayPath = Strings::ToNarrow(path.wstring());
			outErrorMessage = std::format("Failed to read '{}'", displayPath);
			outBytes.clear();
			return false;
		}

		outErrorMessage.clear();
		return true;
	}

	bool TryReadAllText(const std::filesystem::path& path, std::string& outText, std::string& outErrorMessage, std::size_t maximumBytes)
	{
		std::ifstream input(path, std::ios::binary | std::ios::ate);
		if (!input)
		{
			const std::string displayPath = Strings::ToNarrow(path.wstring());
			outErrorMessage = std::format("Failed to open '{}'", displayPath);
			return false;
		}

		const std::ifstream::pos_type fileSize = input.tellg();
		if (fileSize == std::ifstream::pos_type(-1) || static_cast<std::uint64_t>(fileSize) > maximumBytes)
		{
			const std::string displayPath = Strings::ToNarrow(path.wstring());
			outErrorMessage = std::format("Failed to determine a supported size for '{}'", displayPath);
			return false;
		}

		outText.resize(static_cast<std::size_t>(fileSize));
		input.seekg(0, std::ios::beg);
		if (!outText.empty() && !input.read(outText.data(), static_cast<std::streamsize>(outText.size())))
		{
			outText.clear();
			const std::string displayPath = Strings::ToNarrow(path.wstring());
			outErrorMessage = std::format("Failed to read '{}'", displayPath);
			return false;
		}
		outErrorMessage.clear();
		return true;
	}
}
