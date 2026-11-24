#include "CookProjectContent.h"

#include "../Diagnostics/AssetCookerDiagnostics.h"
#include "../Planning/ProjectCookPlanner.h"
#include "../Dispatch/AssetCookerDispatcher.h"
#include "../Planning/ProjectCookPlan.h"

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

static bool ResolveWorkspaceRoot(const std::filesystem::path& requestedRoot, AssetCookerDiagnostics& diagnostics, std::filesystem::path& outWorkspaceRoot)
{
	if (!requestedRoot.empty())
	{
		outWorkspaceRoot = std::filesystem::absolute(requestedRoot).lexically_normal();
		return true;
	}

	std::error_code currentPathError;
	const std::filesystem::path currentPath = std::filesystem::current_path(currentPathError);
	if (currentPathError || !ProjectCookPlanner::TryFindWorkspaceRoot(currentPath, outWorkspaceRoot))
	{
		diagnostics.AddError("Failed to resolve workspace root. Pass --root or set workspaceRoot.");
		return false;
	}

	return true;
}

static bool ResolveProjects(const std::filesystem::path& workspaceRoot, std::string_view projectName, AssetCookerDiagnostics& diagnostics, std::vector<std::string>& outProjects)
{
	const bool bAllProjects = projectName.empty() || projectName == "ALL";
	const bool bAllAlias = projectName == "All" || projectName == "all";
	if (!bAllProjects && !bAllAlias)
	{
		outProjects.emplace_back(projectName);
		return true;
	}

	outProjects = ProjectCookPlanner::DiscoverProjects(workspaceRoot, diagnostics);
	return !outProjects.empty();
}

static AssetCookerResult Finish(bool bSucceeded, AssetCookerDiagnostics& diagnostics, std::size_t productCount = 0)
{
	AssetCookerResult result;
	result.exitCode = bSucceeded ? 0 : 1;
	result.diagnostics = diagnostics.ReleaseRecords();
	result.productCount = productCount;
	return result;
}

AssetCookerResult CookProjectContent(const AssetCookerRequest& request)
{
	AssetCookerDiagnostics diagnostics;
	std::filesystem::path workspaceRoot;
	if (!ResolveWorkspaceRoot(request.workspaceRoot, diagnostics, workspaceRoot))
	{
		return Finish(false, diagnostics);
	}

	const std::optional<std::string_view> expectedToolProfile = ProjectCookPlanner::ResolveToolProfile(request.configuration);
	if (!expectedToolProfile.has_value())
	{
		diagnostics.AddError("Unsupported configuration '" + request.configuration + "'.");
		return Finish(false, diagnostics);
	}
	const std::string_view toolProfile = request.toolProfile.empty() ? *expectedToolProfile : std::string_view(request.toolProfile);
	if (toolProfile != *expectedToolProfile)
	{
		diagnostics.AddError("Tool profile '" + std::string(toolProfile) + "' does not match game profile '" + request.configuration + "'.");
		return Finish(false, diagnostics);
	}

	std::vector<std::string> projects;
	if (!ResolveProjects(workspaceRoot, request.projectName, diagnostics, projects))
	{
		return Finish(false, diagnostics);
	}

	std::size_t productCount = 0;
	for (const std::string& projectName : projects)
	{
		AssetCookerProjectCookPlan plan;
		if (!ProjectCookPlanner::BuildProjectCookPlan(workspaceRoot, projectName, toolProfile, request.category, plan, diagnostics))
		{
			return Finish(false, diagnostics, productCount);
		}

		if (!DispatchAssetCookPlan(plan, diagnostics, productCount))
		{
			return Finish(false, diagnostics, productCount);
		}
	}

	return Finish(true, diagnostics, productCount);
}
