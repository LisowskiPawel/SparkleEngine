#pragma once

#include <filesystem>
#include <span>
#include <string>

namespace SparkleLauncher
{
	bool WriteLevelSelection(const std::filesystem::path& projectRoot, std::span<const std::string> levelIds, bool bSelected, std::string& outErrorMessage);
}
