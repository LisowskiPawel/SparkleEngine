#include "SparkleLauncher/MaintenanceOperations.h"

#include "SparkleLauncher/BuildProfileCatalog.h"
#include "ToolOutputPaths.h"

#include <algorithm>
#include <utility>

namespace SparkleLauncher
{
	static bool HasScope(const std::vector<std::string>& scopes, std::string_view scope)
	{
		return std::ranges::find(scopes, scope) != scopes.end();
	}

	static bool IncludesBuildScope(const ActionCleanTargetRequest& request, std::string_view scope)
	{
		return request.OperationId == "workspace.build" && HasScope(request.BuildScopes, scope);
	}

	static void AppendCleanPath(std::vector<MaintenanceCleanPathSpec>& paths, std::string displayName, std::filesystem::path path, std::string detail)
	{
		paths.push_back({std::move(displayName), std::move(path), std::move(detail)});
	}

	static void AppendTargetFiles(
	    std::vector<MaintenanceCleanPathSpec>& paths,
	    const SparkleTools::ToolTargetOutputPaths& outputs,
	    const std::string& target,
	    const std::string& detail,
	    const std::filesystem::path& preservedPath = {})
	{
		const auto executable = outputs.BinaryDirectory / (target + ".exe");
		if (preservedPath.empty() || executable != preservedPath)
		{
			AppendCleanPath(paths, target + " executable", executable, detail);
		}
		AppendCleanPath(paths, target + " program database", outputs.SymbolDirectory / (target + ".pdb"), detail);
		AppendCleanPath(paths, target + " import library", outputs.LibraryDirectory / (target + ".lib"), detail);
		AppendCleanPath(paths, target + " compile database", outputs.SymbolDirectory / "obj" / (target + ".pdb"), detail);
	}

	static void AppendLauncherCleanPaths(const ActionCleanTargetRequest& request, const SparkleTools::ToolOutputPaths& outputs, std::vector<MaintenanceCleanPathSpec>& paths)
	{
		if (request.OperationId != "launcher.build.self" && !IncludesBuildScope(request, "launcher"))
		{
			return;
		}
		const auto launcher = outputs.LauncherTargetOutputs(request.EditorProfile);
		AppendTargetFiles(paths, launcher, "SparkleLauncher", "Launcher direct build outputs. The currently running launcher executable is preserved until restart.", request.RunningLauncherPath);
		AppendCleanPath(paths, "SparkleLauncherCore library", launcher.LibraryDirectory / "SparkleLauncherCore.lib", "Launcher support library built for the selected editor profile.");

		AppendCleanPath(
		    paths,
		    "SparkleLauncherCore program database",
		    launcher.SymbolDirectory / "obj" / "SparkleLauncherCore.pdb",
		    "Launcher support library debug symbols built for the selected editor profile.");
	}

	static void AppendProjectCleanPaths(
	    const ActionCleanTargetRequest& request,
	    const SparkleTools::ToolOutputPaths& outputs,
	    const std::string& profileName,
	    const std::string& detail,
	    std::vector<MaintenanceCleanPathSpec>& paths)
	{
		const auto profile = FindBuildProfile(profileName);
		if (!profile)
		{
			return;
		}
		const std::string role = profile->Target == BuildProfileTarget::Game ? "game" : "editor";
		AppendTargetFiles(paths, outputs.ProjectTargetOutputs(request.ContentId, role, profileName), BuildProjectTargetName(request.ContentId, *profile), detail);
	}

	static void AppendCookToolCleanPaths(const ActionCleanTargetRequest& request, const SparkleTools::ToolOutputPaths& outputs, std::vector<MaintenanceCleanPathSpec>& paths)
	{
		if (request.OperationId != "cook.tools.prepare" && !IncludesBuildScope(request, "cook-tools"))
		{
			return;
		}
#if SPARKLE_ENABLE_CONTENT_PIPELINE
		AppendTargetFiles(paths, outputs.ToolTargetOutputs("AssetCooker", request.EditorProfile), "AssetCooker", "AssetCooker executable outputs.");
		AppendTargetFiles(paths, outputs.ToolTargetOutputs("TextureCooker", request.EditorProfile), "TextureCooker", "TextureCooker executable outputs.");
#endif
#if SPARKLE_ENABLE_SHADER_COMPILER
		AppendTargetFiles(paths, outputs.ToolTargetOutputs("ShaderCompiler", request.EditorProfile), "ShaderCompiler", "ShaderCompiler executable outputs.");
#endif
	}

	static void AppendBuildCleanPaths(const ActionCleanTargetRequest& request, const SparkleTools::ToolOutputPaths& outputs, std::vector<MaintenanceCleanPathSpec>& paths)
	{
		AppendLauncherCleanPaths(request, outputs, paths);
		if (request.OperationId == "workspace.build.editor" || IncludesBuildScope(request, "editor"))
		{
			AppendProjectCleanPaths(request, outputs, request.EditorProfile, "Editor target outputs.", paths);
		}
		if (request.OperationId == "workspace.build.game" || IncludesBuildScope(request, "game"))
		{
			AppendProjectCleanPaths(request, outputs, request.GameProfile, "Game target outputs.", paths);
		}
		AppendCookToolCleanPaths(request, outputs, paths);
	}

	static void AppendCookedCleanPaths(const ActionCleanTargetRequest& request, const SparkleTools::ToolOutputPaths& outputs, std::vector<MaintenanceCleanPathSpec>& paths)
	{
		const auto content = outputs.CookedProjectDirectory(request.ContentId);
		if (request.OperationId == "cook.workspace")
		{
			if (HasScope(request.CookScopes, "textures") || HasScope(request.CookScopes, "assets"))
			{
				AppendCleanPath(paths, "Cooked content", content, "Generated texture, scene, mesh, and material outputs selected by Cook Workspace.");
			}
			if (HasScope(request.CookScopes, "shaders"))
			{
				AppendCleanPath(paths, "Cooked shaders", content / "Shaders", "Generated shader outputs selected by Cook Workspace.");
			}
		}
		else if (request.OperationId == "cook.all")
		{
			AppendCleanPath(paths, "Cooked content", content, "All generated cooked content.");
			AppendCleanPath(paths, "Cooked shaders", content / "Shaders", "Generated shader outputs refreshed by cook operations.");
		}
		else if (request.OperationId == "cook.shaders")
		{
			AppendCleanPath(paths, "Cooked shaders", content / "Shaders", "Generated shader outputs refreshed by shader cooking.");
		}
		else if (request.OperationId == "cook.textures" || request.OperationId == "cook.assets")
		{
			AppendCleanPath(paths, "Cooked content", content, request.OperationId == "cook.textures" ? "Cooked texture outputs." : "Cooked mesh and material outputs.");
		}
	}

	bool SupportsActionSpecificClean(std::string_view operationId)
	{
		return operationId == "launcher.build.self" || operationId.starts_with("workspace.build") || operationId == "cook.tools.prepare" || operationId == "cook.workspace" || operationId == "cook.all"
		    || operationId == "cook.shaders" || operationId == "cook.textures" || operationId == "cook.assets";
	}

	std::vector<MaintenanceCleanPathSpec> BuildActionCleanPaths(const ActionCleanTargetRequest& request)
	{
		const bool bNeedsContentBuildScope = IncludesBuildScope(request, "editor") || IncludesBuildScope(request, "game");
		const bool bNeedsContentOperationId = bNeedsContentBuildScope || (request.OperationId == "workspace.build.editor");
		const bool bNeedsContentOperationId2 = bNeedsContentOperationId || (request.OperationId == "workspace.build.game");
		const bool bNeedsContent = bNeedsContentOperationId2 || (request.OperationId.starts_with("cook."));

		if (bNeedsContent && request.ContentId.empty())
		{
			return {};
		}
		const auto outputs = SparkleTools::ResolveToolOutputPaths(request.WorkspaceRoot);
		std::vector<MaintenanceCleanPathSpec> paths;
		AppendBuildCleanPaths(request, outputs, paths);
		AppendCookedCleanPaths(request, outputs, paths);
		return paths;
	}
}
