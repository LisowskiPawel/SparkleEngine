#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace SparkleLauncher
{
	struct OperationRecord;

	bool CommitLevelSelection(const std::filesystem::path& contentRoot, const std::vector<std::string>& levelIds, bool bSelected, OperationRecord& operation);
	bool ClearLevelSelection(const std::filesystem::path& contentRoot, OperationRecord& operation);
}
