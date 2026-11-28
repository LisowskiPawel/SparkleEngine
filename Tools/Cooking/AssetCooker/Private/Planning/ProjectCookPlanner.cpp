#include "ProjectCookPlanner.h"
#include "ToolOutputPaths.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathUtils.h"
#include "KnightSceneSources.h"
#include "Level/LevelDocumentReader.h"
#include "ProjectLevelCatalog.h"
#include "SourceSceneImporter.h"

#include <algorithm>
#include <format>
#include <fstream>
#include <map>
#include <optional>
#include <system_error>
#include <utility>

bool ProjectCookPlanner::TryFindWorkspaceRoot(const std::filesystem::path& startPath, std::filesystem::path& outWorkspaceRoot)
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
	const bool bWorkspaceFound = workspaceRoot.has_value();
	const bool bEnginePresent = bWorkspaceFound && PathExists(*workspaceRoot / "Engine" / std::string(Filesystem::kEngineMarker));
	const bool bProjectsPresent = bEnginePresent && PathExists(*workspaceRoot / "Projects");
	const bool bToolsPresent = bProjectsPresent && PathExists(*workspaceRoot / "Tools");

	if (!bToolsPresent)
	{
		return false;
	}

	outWorkspaceRoot = *workspaceRoot;
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

std::vector<std::string> ProjectCookPlanner::DiscoverProjects(const std::filesystem::path& workspaceRoot, AssetCookerDiagnostics& diagnostics)
{
	std::vector<std::string> projects;
	const std::filesystem::path projectsRoot = workspaceRoot / "Projects";
	if (!PathExists(projectsRoot))
	{
		diagnostics.AddError("Projects directory was not found.", projectsRoot);
		return projects;
	}

	std::error_code iteratorError;
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(projectsRoot, iteratorError))
	{
		std::error_code statusError;
		const std::string projectName = entry.path().filename().string();
		const bool bProjectDirectory = entry.is_directory(statusError) && projectName != "TemplateProject";
		const bool bProjectMarkerPresent = bProjectDirectory && PathExists(entry.path() / std::string(Filesystem::kProjectMarker));

		if (bProjectMarkerPresent)
		{
			projects.push_back(projectName);
		}
	}

	std::sort(projects.begin(), projects.end());
	if (projects.empty())
	{
		diagnostics.AddError("No runnable projects were found under Projects.");
	}
	return projects;
}

bool ProjectCookPlanner::BuildProjectCookPlan(
    const std::filesystem::path& workspaceRoot,
    std::string_view projectName,
    std::string_view toolProfile,
    AssetCookerCategory category,
    AssetCookerProjectCookPlan& outPlan,
    AssetCookerDiagnostics& diagnostics)
{
	outPlan = {};
	outPlan.projectName = std::string(projectName);
	const auto outputs = SparkleTools::ResolveToolOutputPaths(workspaceRoot);
	outPlan.projectRoot = workspaceRoot / "Projects" / projectName;
	outPlan.shaderCompilerPath = outputs.ToolTargetOutputs("ShaderCompiler", toolProfile).BinaryDirectory / "ShaderCompiler.exe";
	outPlan.textureCookerPath = outputs.ToolTargetOutputs("TextureCooker", toolProfile).BinaryDirectory / "TextureCooker.exe";
	outPlan.temporaryRoot = outputs.ToolScratchDirectory("AssetCooker", toolProfile);
	AddPlanSteps(category, outPlan.steps);

	if (!PathExists(outPlan.projectRoot / std::string(Filesystem::kProjectMarker)))
	{
		diagnostics.AddError("Project marker was not found.", outPlan.projectRoot / std::string(Filesystem::kProjectMarker));
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

static std::string AdmitSceneSource(const std::filesystem::path& meshRoot, const std::filesystem::path& sourcePath, std::map<std::string, AssetCookerSceneEntry>& entries)
{
	const auto relativePath = Paths::TryMakeRelativeUnderRoot(sourcePath, meshRoot);
	if (!relativePath)
	{
		throw Diagnostics::Error(std::format("Scene source must remain under Assets/Meshes: '{}'.", sourcePath.string()));
	}
	if (!SupportsSourceScenePath(sourcePath))
	{
		throw Diagnostics::Error(std::format("Unsupported scene source extension: '{}'.", sourcePath.string()));
	}
	std::error_code error;
	if (!std::filesystem::is_regular_file(sourcePath, error))
	{
		throw Diagnostics::Error(std::format("Exact scene source is missing or unreadable: '{}'.", sourcePath.string()));
	}
	std::filesystem::path sceneIdPath = *relativePath;
	const std::string sceneId = sceneIdPath.replace_extension().generic_string();
	const auto [entry, bInserted] = entries.try_emplace(sceneId, AssetCookerSceneEntry{relativePath->generic_string(), sourcePath});
	if (!bInserted && entry->second.sourcePath != sourcePath)
	{
		throw Diagnostics::Error(std::format("Scene sources share cooked identity '{}': '{}' and '{}'.", sceneId, entry->second.sourcePath.string(), sourcePath.string()));
	}
	return sceneId;
}

static void ValidateSceneBindings(const ProjectLevelCatalogEntry& level, const std::vector<std::string>& expectedIds)
{
	std::ifstream input(level.sourcePath);
	if (!input.is_open())
	{
		throw Diagnostics::Error(std::format("Failed to open level file '{}'.", level.sourcePath.string()));
	}
	const LevelDesc description = LevelParsing::ReadLevelDescription(input, level.sourcePath);
	if (!std::ranges::is_permutation(description.sceneAssetIds, expectedIds, std::ranges::equal_to{}, &SceneAssetId::value))
	{
		throw Diagnostics::Error(std::format("Level '{}' scene bindings do not match the declared scene sources.", level.id));
	}
}

static void CollectLevelSceneEntries(const ProjectLevelCatalogEntry& level, const std::filesystem::path& meshRoot, bool bShowcase, std::map<std::string, AssetCookerSceneEntry>& entries)
{
	const bool bKnight = bShowcase && level.id == "ModernSponzaKnight";
	const bool bEmpty = level.id == "Empty";
	if (level.sceneSourcePath.empty() != (bKnight || bEmpty))
	{
		throw Diagnostics::Error(std::format("Level '{}' has an invalid SceneSource declaration.", level.id));
	}

	std::vector<std::string> expectedIds;
	if (bKnight)
	{
		expectedIds.reserve(Showcase::KnightSceneSources.size());
		for (const std::string_view source : Showcase::KnightSceneSources)
		{
			expectedIds.push_back(AdmitSceneSource(meshRoot, meshRoot / source, entries));
		}
	}
	else if (!bEmpty)
	{
		expectedIds.push_back(AdmitSceneSource(meshRoot, level.sceneSourcePath, entries));
	}
	ValidateSceneBindings(level, expectedIds);
}

bool ProjectCookPlanner::CollectSceneEntries(const std::filesystem::path& projectRoot, std::vector<AssetCookerSceneEntry>& outEntries, AssetCookerDiagnostics& diagnostics)
{
	const std::filesystem::path meshRoot = Paths::Normalize(projectRoot / "Assets" / "Meshes");
	const bool bShowcase = projectRoot.filename() == "Showcase";
	std::map<std::string, AssetCookerSceneEntry> entries;
	try
	{
		const ProjectLevelCatalog catalog = LoadProjectLevelCatalog(projectRoot);
		for (const ProjectLevelCatalogEntry& level : catalog.levels)
		{
			if (!level.bSelected)
			{
				continue;
			}
			CollectLevelSceneEntries(level, meshRoot, bShowcase, entries);
			if (!catalog.HasLevelSourceInputs(level))
			{
				throw Diagnostics::Error(std::format("Catalog level '{}' is unavailable: '{}'.", level.id, level.sourcePath.string()));
			}
		}
	}
	catch (const Diagnostics::Error& error)
	{
		diagnostics.AddError(error.what());
		return false;
	}

	outEntries.reserve(entries.size());
	for (auto& [sceneId, entry] : entries)
	{
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
