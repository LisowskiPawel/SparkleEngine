#include "ProjectCookPlanner.h"
#include "Core/Public/Paths/ProductPaths.h"

#include "Core/Public/Paths/PackagePaths.h"
#include "Core/Public/Strings/StringUtils.h"

#include "../Discovery/CatalogedLevelSceneReader.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Projects/ProjectLevelCatalog.h"
#include "SourceSceneImporter.h"

#include <algorithm>
#include <array>
#include <optional>
#include <system_error>
#include <utility>

bool ProjectCookPlanner::TryFindRepositoryRoot(const std::filesystem::path& startPath, std::filesystem::path& outRepositoryRoot)
{
	std::error_code error;
	std::filesystem::path currentPath = Paths::Normalize(startPath.empty() ? std::filesystem::current_path(error) : startPath);
	if (error)
	{
		return false;
	}
	if (!std::filesystem::is_directory(currentPath))
	{
		currentPath = currentPath.parent_path();
	}

	const std::optional<std::filesystem::path> workspaceRoot = Filesystem::FindAncestorWithMarker(currentPath, Filesystem::kWorkspaceMarker);
	if (!workspaceRoot || !PathExists(*workspaceRoot / "Engine" / std::string(Filesystem::kEngineMarker)) || !PathExists(*workspaceRoot / "Projects") || !PathExists(*workspaceRoot / "Tools"))
	{
		return false;
	}

	outRepositoryRoot = *workspaceRoot;
	return true;
}

std::optional<std::string_view> ProjectCookPlanner::ResolveToolProfile(std::string_view configuration) noexcept
{
	if (configuration == "DebugEditor" || configuration == "DebugGame")
	{
		return "DebugEditor";
	}
	if (configuration == "DevelopmentEditor" || configuration == "DevelopmentGame")
	{
		return "DevelopmentEditor";
	}
	if (configuration == "ShippingEditor" || configuration == "ShippingGame")
	{
		return "ShippingEditor";
	}
	return std::nullopt;
}

std::vector<std::string> ProjectCookPlanner::DiscoverProjects(const std::filesystem::path& repositoryRoot, AssetCookerDiagnostics& diagnostics)
{
	std::vector<std::string> projects;
	if (!Filesystem::IsSourceWorkspace(repositoryRoot))
	{
		const auto source = Filesystem::ResolvePackageSourceRoot(repositoryRoot);
		if (source && PathExists(*source / std::string(Filesystem::kProjectMarker)))
		{
			return {Strings::ToNarrow(source->filename().wstring())};
		}
		diagnostics.AddError(AssetCookerCategory::All, "Package content store is not initialized. Start Launcher first.");
		return {};
	}
	const std::filesystem::path projectsRoot = repositoryRoot / "Projects";
	if (!PathExists(projectsRoot))
	{
		diagnostics.AddError(AssetCookerCategory::All, "Projects directory was not found.", projectsRoot);
		return projects;
	}

	std::error_code iteratorError;
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(projectsRoot, iteratorError))
	{
		std::error_code statusError;
		const std::string projectName = entry.path().filename().string();
		if (entry.is_directory(statusError) && projectName != "TemplateProject" && PathExists(entry.path() / std::string(Filesystem::kProjectMarker)))
		{
			projects.push_back(projectName);
		}
	}

	std::sort(projects.begin(), projects.end());
	if (projects.empty())
	{
		diagnostics.AddError(AssetCookerCategory::All, "No runnable projects were found under Projects.");
	}
	return projects;
}

bool ProjectCookPlanner::BuildProjectCookPlan(
    const std::filesystem::path& repositoryRoot,
    std::string_view projectName,
    std::string_view toolProfile,
    AssetCookerCategory category,
    AssetCookerProjectCookPlan& outPlan,
    AssetCookerDiagnostics& diagnostics)
{
	outPlan = {};
	outPlan.projectName = std::string(projectName);
	outPlan.projectRoot = Filesystem::ResolveProductProjectRoot(repositoryRoot, projectName);
	outPlan.cookedRoot = Filesystem::ResolveProductCookedRoot(repositoryRoot, projectName);
	outPlan.shaderCompilerPath = Filesystem::ResolveProductToolPath(repositoryRoot, "ShaderCompiler", toolProfile);
	outPlan.textureCookerPath = Filesystem::ResolveProductToolPath(repositoryRoot, "TextureCooker", toolProfile);
	outPlan.temporaryRoot = Filesystem::ResolveProductToolScratch(repositoryRoot, "AssetCooker", toolProfile);
	AddPlanSteps(category, outPlan.steps);
	if (!Filesystem::IsSourceWorkspace(repositoryRoot) && (outPlan.projectRoot.empty() || outPlan.cookedRoot.empty() || outPlan.shaderCompilerPath.empty() || outPlan.textureCookerPath.empty()))
	{
		diagnostics.AddError(AssetCookerCategory::All, "Invalid package project, profile or cook-helper manifest entry.");
		return false;
	}

	if (!PathExists(outPlan.projectRoot / std::string(Filesystem::kProjectMarker)))
	{
		diagnostics.AddError(AssetCookerCategory::All, "Project marker was not found.", outPlan.projectRoot / std::string(Filesystem::kProjectMarker));
		return false;
	}

	if (!CategoryNeedsScenes(category))
	{
		return true;
	}

	return CollectSceneEntries(outPlan.projectRoot, outPlan.sceneEntries, diagnostics);
}

bool ProjectCookPlanner::PathExists(const std::filesystem::path& path)
{
	std::error_code error;
	return std::filesystem::exists(path, error) && !error;
}

bool ProjectCookPlanner::CategoryNeedsScenes(AssetCookerCategory category) noexcept
{
	return category == AssetCookerCategory::All || category == AssetCookerCategory::Textures || category == AssetCookerCategory::SceneAssets;
}

bool ProjectCookPlanner::CollectSceneEntries(const std::filesystem::path& projectRoot, std::vector<AssetCookerSceneEntry>& outEntries, AssetCookerDiagnostics& diagnostics)
{
	std::vector<std::string> sceneIds;
	if (!CollectSceneIds(projectRoot, sceneIds, diagnostics))
	{
		return false;
	}

	for (const std::string& sceneId : sceneIds)
	{
		AssetCookerSceneEntry entry;
		if (!ResolveSceneEntry(projectRoot, sceneId, entry, diagnostics))
		{
			return false;
		}

		outEntries.push_back(std::move(entry));
	}

	return true;
}

void ProjectCookPlanner::AddPlanSteps(AssetCookerCategory category, std::vector<AssetCookerPlanStep>& outSteps)
{
	outSteps.clear();
	if (category == AssetCookerCategory::All)
	{
		outSteps = {AssetCookerPlanStep::Shaders, AssetCookerPlanStep::Textures, AssetCookerPlanStep::SceneAssets};
		return;
	}

	switch (category)
	{
		case AssetCookerCategory::Shaders:
			outSteps.push_back(AssetCookerPlanStep::Shaders);
			break;
		case AssetCookerCategory::Textures:
			outSteps.push_back(AssetCookerPlanStep::Textures);
			break;
		default:
			outSteps.push_back(AssetCookerPlanStep::SceneAssets);
			break;
	}
}

bool ProjectCookPlanner::CollectSceneIds(const std::filesystem::path& projectRoot, std::vector<std::string>& outSceneIds, AssetCookerDiagnostics& diagnostics)
{
	ProjectLevelCatalog catalog;
	try
	{
		catalog = ProjectLevelCatalogFile::Load(projectRoot);
	}
	catch (const Diagnostics::Error& error)
	{
		diagnostics.AddError(AssetCookerCategory::SceneAssets, error.what());
		return false;
	}

	outSceneIds.clear();
	for (const ProjectLevelCatalogEntry& level : catalog.levels)
	{
		if (!AppendLevelSceneIds(catalog, level, outSceneIds, diagnostics))
		{
			return false;
		}
	}

	std::sort(outSceneIds.begin(), outSceneIds.end());
	outSceneIds.erase(std::unique(outSceneIds.begin(), outSceneIds.end()), outSceneIds.end());
	return true;
}

bool ProjectCookPlanner::AppendLevelSceneIds(const ProjectLevelCatalog& catalog, const ProjectLevelCatalogEntry& level, std::vector<std::string>& outSceneIds, AssetCookerDiagnostics& diagnostics)
{
	if (!level.selected)
	{
		return true;
	}

	if (!catalog.IsLevelReady(level))
	{
		diagnostics.AddError(AssetCookerCategory::SceneAssets, "Catalog level is unavailable: " + level.id, level.sourcePath);
		return false;
	}

	std::string errorMessage;
	if (CatalogedLevelSceneReader::AppendSceneIds(level.sourcePath, outSceneIds, errorMessage))
	{
		return true;
	}

	diagnostics.AddError(AssetCookerCategory::SceneAssets, std::move(errorMessage), level.sourcePath);
	return false;
}

bool ProjectCookPlanner::ResolveSceneEntry(const std::filesystem::path& projectRoot, std::string_view sceneId, AssetCookerSceneEntry& outEntry, AssetCookerDiagnostics& diagnostics)
{
	const std::filesystem::path meshRoot = Paths::Normalize(projectRoot / "Assets" / "Meshes");
	const std::filesystem::path relativeBase = std::filesystem::path(sceneId).lexically_normal();

	if (!IsSceneIdSafe(relativeBase))
	{
		diagnostics.AddError(AssetCookerCategory::SceneAssets, "Catalog scene id must remain under the project mesh root: " + std::string(sceneId));
		return false;
	}

	std::filesystem::path sourcePath;
	if (!ResolveSceneSource(meshRoot, relativeBase, sceneId, sourcePath, diagnostics))
	{
		return false;
	}

	if (sourcePath.empty())
	{
		diagnostics.AddError(AssetCookerCategory::SceneAssets, "Catalog scene source was not found: " + std::string(sceneId), meshRoot / relativeBase);
		return false;
	}

	const std::optional<std::filesystem::path> relativePath = Paths::TryMakeRelativeUnderRoot(sourcePath, meshRoot);
	if (!relativePath)
	{
		diagnostics.AddError(AssetCookerCategory::SceneAssets, "Resolved scene source escaped the project mesh root.", sourcePath);
		return false;
	}

	outEntry.relativePath = relativePath->generic_string();
	outEntry.sourcePath = sourcePath;
	return true;
}

bool ProjectCookPlanner::IsSceneIdSafe(const std::filesystem::path& relativeScenePath) noexcept
{
	return !relativeScenePath.empty() && !relativeScenePath.is_absolute() && !relativeScenePath.generic_string().starts_with("..");
}

bool ProjectCookPlanner::ResolveSceneSource(
    const std::filesystem::path& meshRoot,
    const std::filesystem::path& relativeScenePath,
    std::string_view sceneId,
    std::filesystem::path& outSourcePath,
    AssetCookerDiagnostics& diagnostics)
{
	outSourcePath.clear();

	const std::filesystem::path exactCandidate = Paths::Normalize(meshRoot / relativeScenePath);
	if (SupportsSourceScenePath(relativeScenePath) && Paths::IsUnderRoot(exactCandidate, meshRoot) && PathExists(exactCandidate))
	{
		outSourcePath = exactCandidate;
		return true;
	}

	const std::array<std::wstring_view, 4> extensions = {L".gltf", L".glb", L".fbx", L".ply"};
	for (std::wstring_view extension : extensions)
	{
		std::filesystem::path candidate = exactCandidate;
		candidate.replace_extension(extension);
		if (!Paths::IsUnderRoot(candidate, meshRoot) || !PathExists(candidate))
		{
			continue;
		}
		if (!outSourcePath.empty())
		{
			diagnostics.AddError(AssetCookerCategory::SceneAssets, "Catalog scene id resolves to more than one source file: " + std::string(sceneId));
			return false;
		}
		outSourcePath = std::move(candidate);
	}

	return true;
}
