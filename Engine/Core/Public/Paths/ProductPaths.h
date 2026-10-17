#pragma once

#include "Core/Public/CoreAPI.h"

#include <filesystem>
#include <string_view>

namespace Filesystem
{
	SPARKLE_CORE_API bool IsSourceWorkspace(const std::filesystem::path& root);
	SPARKLE_CORE_API std::filesystem::path ResolveProductToolPath(const std::filesystem::path& root, std::string_view target, std::string_view profile);
	SPARKLE_CORE_API std::filesystem::path ResolveProjectProductPath(
	    const std::filesystem::path& root,
	    std::string_view project,
	    std::string_view role,
	    std::string_view profile,
	    std::string_view variant = {});
	SPARKLE_CORE_API std::filesystem::path ResolveProductProjectRoot(const std::filesystem::path& root, std::string_view project);
	SPARKLE_CORE_API std::filesystem::path ResolveProductCookedRoot(const std::filesystem::path& root, std::string_view project);
	SPARKLE_CORE_API std::filesystem::path ResolveProductToolScratch(const std::filesystem::path& root, std::string_view tool, std::string_view profile);
}
