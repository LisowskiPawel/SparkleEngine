#include "SparkleLauncher/MaintenanceOperations.h"

#include "SparkleLauncher/BuildProfileCatalog.h"
#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "SparkleLauncher/CookOperations.h"
#include "ToolOutputPaths.h"

#include <algorithm>
#include <initializer_list>
#include <utility>

namespace SparkleLauncher
{
	template <typename Scope> static bool HasScope(const std::vector<Scope>& scopes, Scope scope)
	{
		return std::ranges::find(scopes, scope) != scopes.end();
	}

	static bool IncludesBuildScope(const ActionCleanTargetRequest& request, BuildWorkspaceScope scope)
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
		if (request.OperationId != "launcher.build.self" && !IncludesBuildScope(request, BuildWorkspaceScope::Launcher))
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
		if (request.OperationId != "cook.tools.prepare" && !IncludesBuildScope(request, BuildWorkspaceScope::CookTools))
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
		if (request.OperationId == "workspace.build.editor" || IncludesBuildScope(request, BuildWorkspaceScope::Editor))
		{
			AppendProjectCleanPaths(request, outputs, request.EditorProfile, "Editor target outputs.", paths);
		}
		if (request.OperationId == "workspace.build.game" || IncludesBuildScope(request, BuildWorkspaceScope::Game))
		{
			AppendProjectCleanPaths(request, outputs, request.GameProfile, "Game target outputs.", paths);
		}
		AppendCookToolCleanPaths(request, outputs, paths);
	}

	static void AppendCookedScopeCleanPaths(const std::filesystem::path& content, CookWorkspaceScope scope, std::vector<MaintenanceCleanPathSpec>& paths)
	{
		switch (scope)
		{
			case CookWorkspaceScope::Shaders:
				AppendCleanPath(paths, "Cooked shaders", content / "Shaders", "Generated shader outputs.");
				return;
			case CookWorkspaceScope::Textures:
				AppendCleanPath(paths, "Cooked textures", content / "Textures", "Generated texture outputs.");
				return;
			case CookWorkspaceScope::SceneAssets:
				AppendCleanPath(paths, "Cooked scene manifests", content / "SceneManifests", "Generated scene manifests.");
				AppendCleanPath(paths, "Cooked meshes", content / "Meshes", "Generated mesh outputs.");
				AppendCleanPath(paths, "Cooked materials", content / "Materials", "Generated material outputs.");
				AppendCleanPath(paths, "Cooked skeletons", content / "Skeletons", "Generated skeleton outputs.");
				AppendCleanPath(paths, "Cooked animations", content / "Animations", "Generated animation outputs.");
				AppendCleanPath(paths, "Cooked scene registry", content / "SceneAssetRegistry.sreg", "Generated scene asset registry.");
				return;
		}
	}

	static void AppendCookedCleanPaths(const ActionCleanTargetRequest& request, const SparkleTools::ToolOutputPaths& outputs, std::vector<MaintenanceCleanPathSpec>& paths)
	{
		const auto definition = FindCookOperationDefinition(request.OperationId);
		if (!definition)
		{
			return;
		}

		const auto content = outputs.CookedProjectDirectory(request.ContentId);
		switch (definition->Kind)
		{
			case CookOperationKind::CookWorkspace:
				for (const auto scope : {CookWorkspaceScope::Shaders, CookWorkspaceScope::Textures, CookWorkspaceScope::SceneAssets})
				{
					if (HasScope(request.CookScopes, scope))
					{
						AppendCookedScopeCleanPaths(content, scope, paths);
					}
				}
				return;
			case CookOperationKind::CookShaders:
				AppendCookedScopeCleanPaths(content, CookWorkspaceScope::Shaders, paths);
				return;
			case CookOperationKind::BuildTextures:
				AppendCookedScopeCleanPaths(content, CookWorkspaceScope::Textures, paths);
				return;
			case CookOperationKind::BuildSceneAssets:
				AppendCookedScopeCleanPaths(content, CookWorkspaceScope::SceneAssets, paths);
				return;
			case CookOperationKind::CookAllAssets:
				AppendCleanPath(paths, "Cooked content", content, "All generated cooked content.");
				return;
		}
	}

	bool SupportsActionSpecificClean(std::string_view operationId)
	{
		return operationId == "launcher.build.self" || operationId.starts_with("workspace.build") || operationId == "cook.tools.prepare" || operationId == "cook.workspace" || operationId == "cook.all"
		    || operationId == "cook.shaders" || operationId == "cook.textures" || operationId == "cook.assets";
	}

	std::vector<MaintenanceCleanPathSpec> BuildActionCleanPaths(const ActionCleanTargetRequest& request)
	{
		const bool bSelectedProjectBuild = IncludesBuildScope(request, BuildWorkspaceScope::Editor) || IncludesBuildScope(request, BuildWorkspaceScope::Game);
		const bool bDirectProjectBuild = request.OperationId == "workspace.build.editor" || request.OperationId == "workspace.build.game";
		const bool bBuildNeedsContent = bSelectedProjectBuild || bDirectProjectBuild;
		const bool bNeedsContent = bBuildNeedsContent || FindCookOperationDefinition(request.OperationId).has_value();

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
