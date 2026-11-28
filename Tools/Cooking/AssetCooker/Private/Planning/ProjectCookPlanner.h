#pragma once

#include "Api/AssetCookerTypes.h"
#include "../Diagnostics/AssetCookerDiagnostics.h"
#include "ProjectCookPlan.h"

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class ProjectCookPlanner final
{
public:
	static bool TryFindWorkspaceRoot(const std::filesystem::path& startPath, std::filesystem::path& outWorkspaceRoot);
	static std::optional<std::string_view> ResolveToolProfile(std::string_view configuration) noexcept;
	static std::vector<std::string> DiscoverProjects(const std::filesystem::path& workspaceRoot, AssetCookerDiagnostics& diagnostics);
	static bool BuildProjectCookPlan(
	    const std::filesystem::path& workspaceRoot,
	    std::string_view projectName,
	    std::string_view toolProfile,
	    AssetCookerCategory category,
	    AssetCookerProjectCookPlan& outPlan,
	    AssetCookerDiagnostics& diagnostics);

private:
	static bool PathExists(const std::filesystem::path& path);
	static bool CategoryNeedsScenes(AssetCookerCategory category) noexcept;
	static void AddPlanSteps(AssetCookerCategory category, std::vector<AssetCookerPlanStep>& outSteps);
	static bool CollectSceneEntries(const std::filesystem::path& projectRoot, std::vector<AssetCookerSceneEntry>& outEntries, AssetCookerDiagnostics& diagnostics);
};
