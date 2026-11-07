#include "AssetCookerService.h"
#include "Core/Public/Strings/StringUtils.h"

#include "AssetCookerTypes.h"
#include "../Diagnostics/AssetCookerDiagnostics.h"
#include "../Planning/ProjectCookPlanner.h"
#include "../Dispatch/AssetCookerDispatcher.h"
#include "../Planning/ProjectCookPlan.h"

#include <filesystem>
#include <optional>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

AssetCookerService::AssetCookerService(const char* workspaceRoot, const char* projectName, const char* configuration, const char* toolProfile)
{
	if (HasText(workspaceRoot))
	{
		m_configuredWorkspaceRoot = Strings::ToWide(std::string_view(workspaceRoot));
	}
	if (HasText(projectName))
	{
		m_configuredProjectName = projectName;
	}
	if (HasText(configuration))
	{
		m_configuredConfiguration = configuration;
	}
	if (HasText(toolProfile))
	{
		m_configuredToolProfile = toolProfile;
	}
}

AssetCookerServiceResult AssetCookerService::Cook(const char* projectName, const char* configuration, AssetCookerCategory category)
{
	AssetCookerDiagnostics diagnostics;
	std::filesystem::path workspaceRoot;
	if (!ResolveWorkspaceRoot(diagnostics, workspaceRoot))
	{
		return Finish(false, diagnostics);
	}

	const std::string resolvedConfiguration = ResolveConfiguration(configuration);
	const std::optional<std::string_view> expectedToolProfile = ProjectCookPlanner::ResolveToolProfile(resolvedConfiguration);
	if (!expectedToolProfile.has_value())
	{
		diagnostics.AddError(AssetCookerCategory::All, "Unsupported configuration '" + resolvedConfiguration + "'.");
		return Finish(false, diagnostics);
	}
	const std::string resolvedToolProfile = ResolveToolProfile(*expectedToolProfile);
	if (resolvedToolProfile != *expectedToolProfile)
	{
		diagnostics.AddError(AssetCookerCategory::All, "Tool profile '" + resolvedToolProfile + "' does not match game profile '" + resolvedConfiguration + "'.");
		return Finish(false, diagnostics);
	}

	const std::string resolvedProjectName = ResolveProjectName(projectName);
	std::vector<std::string> projects;
	if (!ResolveProjects(workspaceRoot, resolvedProjectName, diagnostics, projects))
	{
		return Finish(false, diagnostics);
	}

	std::vector<AssetCookerOutputRecord> outputs;
	const bool bSucceeded = CookProjects(workspaceRoot, resolvedToolProfile, category, projects, diagnostics, outputs);
	return Finish(bSucceeded, diagnostics, std::move(outputs));
}

bool AssetCookerService::ResolveProjects(const std::filesystem::path& workspaceRoot, std::string_view projectName, AssetCookerDiagnostics& diagnostics, std::vector<std::string>& outProjects) const
{
	if (!IsAllProjects(projectName))
	{
		outProjects.emplace_back(projectName);
		return true;
	}

	outProjects = ProjectCookPlanner::DiscoverProjects(workspaceRoot, diagnostics);
	return !outProjects.empty();
}

bool AssetCookerService::CookProjects(
    const std::filesystem::path& workspaceRoot,
    std::string_view toolProfile,
    AssetCookerCategory category,
    const std::vector<std::string>& projects,
    AssetCookerDiagnostics& diagnostics,
    std::vector<AssetCookerOutputRecord>& outOutputs) const
{
	for (const std::string& projectName : projects)
	{
		if (!CookProject(workspaceRoot, projectName, toolProfile, category, diagnostics, outOutputs))
		{
			return false;
		}
	}

	return true;
}

bool AssetCookerService::CookProject(
    const std::filesystem::path& workspaceRoot,
    std::string_view projectName,
    std::string_view toolProfile,
    AssetCookerCategory category,
    AssetCookerDiagnostics& diagnostics,
    std::vector<AssetCookerOutputRecord>& outOutputs) const
{
	AssetCookerProjectCookPlan plan;
	if (!ProjectCookPlanner::BuildProjectCookPlan(workspaceRoot, projectName, toolProfile, category, plan, diagnostics))
	{
		return false;
	}

	return DispatchAssetCookPlan(plan, diagnostics, outOutputs);
}

bool AssetCookerService::HasText(const char* text) noexcept
{
	return text != nullptr && text[0] != '\0';
}

bool AssetCookerService::IsAllProjects(std::string_view projectName) noexcept
{
	return projectName.empty() || projectName == "ALL" || projectName == "All" || projectName == "all";
}

AssetCookerServiceResult AssetCookerService::Finish(bool bSucceeded, AssetCookerDiagnostics& diagnostics, std::vector<AssetCookerOutputRecord> outputs)
{
	AssetCookerServiceResult result;
	result.exitCode = bSucceeded ? 0 : 1;
	result.diagnostics = diagnostics.ReleaseRecords();
	result.outputs = std::move(outputs);
	return result;
}

bool AssetCookerService::ResolveWorkspaceRoot(AssetCookerDiagnostics& diagnostics, std::filesystem::path& outWorkspaceRoot) const
{
	if (!m_configuredWorkspaceRoot.empty())
	{
		outWorkspaceRoot = std::filesystem::absolute(m_configuredWorkspaceRoot).lexically_normal();
		return true;
	}

	std::error_code currentPathError;
	const std::filesystem::path currentPath = std::filesystem::current_path(currentPathError);
	if (currentPathError || !ProjectCookPlanner::TryFindWorkspaceRoot(currentPath, outWorkspaceRoot))
	{
		diagnostics.AddError(AssetCookerCategory::All, "Failed to resolve workspace root. Pass --root or set workspaceRoot.");
		return false;
	}

	return true;
}

std::string AssetCookerService::ResolveProjectName(const char* requestProjectName) const
{
	if (HasText(requestProjectName))
	{
		return requestProjectName;
	}
	return m_configuredProjectName;
}

std::string AssetCookerService::ResolveConfiguration(const char* requestConfiguration) const
{
	if (HasText(requestConfiguration))
	{
		return requestConfiguration;
	}
	if (!m_configuredConfiguration.empty())
	{
		return m_configuredConfiguration;
	}
	return "DevelopmentGame";
}

std::string AssetCookerService::ResolveToolProfile(std::string_view defaultToolProfile) const
{
	if (!m_configuredToolProfile.empty())
	{
		return m_configuredToolProfile;
	}
	return std::string(defaultToolProfile);
}
