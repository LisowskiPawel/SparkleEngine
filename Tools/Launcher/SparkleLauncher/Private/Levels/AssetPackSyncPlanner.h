#pragma once

#include <string>
#include <string_view>
#include <span>
#include <vector>

struct ProjectLevelCatalog;

namespace SparkleLauncher
{
	std::string FindAssetPackAcquisitionBlocker(const ProjectLevelCatalog& catalog, std::string_view packId);
	std::vector<std::string> BuildAssetPackSyncPlan(const ProjectLevelCatalog& catalog, std::span<const std::string> requestedLevelIds = {}, bool bRequireGameSupport = false);
}
