#include "Cook/CookOperationProcessRequests.h"
#include "Core/Public/Strings/StringUtils.h"

#include "Workspace/LauncherProductPaths.h"
#include "Build/CMakeWorkflowProcessRequests.h"
#include "Workspace/LauncherStatePaths.h"

#include <algorithm>
#include <utility>

namespace SparkleLauncher
{
	bool IncludesCookScope(const CookOperationPlan& plan, CookWorkspaceScope scope)
	{
		switch (plan.Kind)
		{
			case CookOperationKind::CookWorkspace:
				return std::find(plan.Request.SelectedScopes.begin(), plan.Request.SelectedScopes.end(), scope) != plan.Request.SelectedScopes.end();
			case CookOperationKind::CookShaders:
				return scope == CookWorkspaceScope::Shaders;
			case CookOperationKind::BuildTextures:
				return scope == CookWorkspaceScope::Textures;
			case CookOperationKind::BuildSceneAssets:
				return scope == CookWorkspaceScope::SceneAssets;
			case CookOperationKind::CookAllAssets:
				return true;
		}
		return false;
	}

	static void AppendCommonShaderCompilerArguments(const CookOperationPlan& plan, std::vector<std::string>& arguments)
	{
		if (!plan.Request.ShaderBackend.empty())
		{
			arguments.push_back("--backend");
			arguments.push_back(plan.Request.ShaderBackend);
		}
		if (plan.Request.ShaderEnableDebugInfo)
		{
			arguments.push_back("--debug-info");
		}
		if (!plan.Request.ShaderEnableOptimizations)
		{
			arguments.push_back("--disable-optimizations");
		}
		arguments.push_back("--warnings-as-errors");
		arguments.push_back(plan.Request.ShaderWarningsAsErrors ? "on" : "off");
		arguments.push_back("--strip-debug");
		arguments.push_back(plan.Request.ShaderStripDebugInfo ? "on" : "off");
	}

	static Process::ChildProcessRequest MakeAssetCookerRequest(const CookOperationPlan& plan, std::string_view command, std::string_view logFileName)
	{
		Process::ChildProcessRequest process;
		process.ExecutablePath = ResolveToolExecutable(plan.Request.WorkspaceRoot, "AssetCooker", plan.ToolProfile);
		process.WorkingDirectory = plan.Request.WorkspaceRoot;
		process.LogPath = ResolveLauncherOperationLogPath(plan.Request.WorkspaceRoot, plan.Operation.Id, logFileName);

		process.Arguments =
		    {std::string(command), plan.Request.ContentId, plan.Request.GameProfile, "--tool-profile", plan.ToolProfile, "--root", Strings::ToNarrow(plan.Request.WorkspaceRoot.wstring())};

		return process;
	}

	static Process::ChildProcessRequest MakeShaderCompilerCookAllRequest(const CookOperationPlan& plan)
	{
		Process::ChildProcessRequest process;
		process.ExecutablePath = ResolveToolExecutable(plan.Request.WorkspaceRoot, "ShaderCompiler", plan.ToolProfile);
		process.WorkingDirectory = ResolveProjectRoot(plan.Request.WorkspaceRoot, plan.Request.ContentId);
		process.LogPath = ResolveLauncherOperationLogPath(plan.Request.WorkspaceRoot, plan.Operation.Id, "CookShaders.txt");
		process.Arguments = {"cook"};
		AppendCommonShaderCompilerArguments(plan, process.Arguments);
		return process;
	}

	static void AddStep(std::vector<CookOperationProcessStep>& steps, std::string id, std::string displayName, Process::ChildProcessRequest request)
	{
		CookOperationProcessStep step;
		step.Id = std::move(id);
		step.DisplayName = std::move(displayName);
		step.Request = std::move(request);
		steps.push_back(std::move(step));
	}

	static void AddCleanStep(std::vector<CookOperationProcessStep>& steps, const CookOperationPlan& plan)
	{
		CookOperationProcessStep step;
		step.Id = "clean-cooked-output";
		step.DisplayName = "Clean cooked output scope";
		step.DestructivePath = plan.CookedOutputDirectory;
		step.HasProcessRequest = false;
		step.DeletesCookedOutputs = true;
		steps.push_back(std::move(step));
	}

	std::vector<CookOperationProcessStep> BuildCookProcessStepsForPlan(const CookOperationPlan& plan)
	{
		std::vector<CookOperationProcessStep> steps;
		if (plan.Request.Mode == CookMode::Force)
		{
			AddCleanStep(steps, plan);
		}

#if SPARKLE_ENABLE_SHADER_COMPILER
		if (IncludesCookScope(plan, CookWorkspaceScope::Shaders))
		{
			AddStep(steps, "cook-shaders", "Cook shaders", MakeShaderCompilerCookAllRequest(plan));
		}
#endif
#if SPARKLE_ENABLE_CONTENT_PIPELINE
		if (IncludesCookScope(plan, CookWorkspaceScope::Textures))
		{
			AddStep(steps, "cook-textures", "Cook textures", MakeAssetCookerRequest(plan, "cook-textures", "CookTextures.txt"));
		}
		if (IncludesCookScope(plan, CookWorkspaceScope::SceneAssets))
		{
			AddStep(steps, "cook-scene-assets", "Cook meshes", MakeAssetCookerRequest(plan, "cook-assets", "CookSceneAssets.txt"));
		}
#endif
		return steps;
	}
}
