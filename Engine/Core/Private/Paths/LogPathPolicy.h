#pragma once

#include <cstdint>
#include <filesystem>
#include <string_view>

namespace Paths
{
	enum class LogParentDirectoryPolicy : std::uint8_t
	{
		Preserve,
		EnsureExists
	};

	std::filesystem::path ResolveBootstrapLogFile(std::string_view configuredFile, LogParentDirectoryPolicy parentDirectoryPolicy);
}
