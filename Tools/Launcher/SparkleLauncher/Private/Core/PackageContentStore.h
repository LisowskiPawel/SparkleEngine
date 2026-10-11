#pragma once

#include <filesystem>
#include <string>

namespace SparkleLauncher
{
	bool InitializePackageContentStore(const std::filesystem::path& packageRoot, std::string& outErrorMessage);
}
