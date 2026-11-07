#include "Maintenance/MaintenanceOperationProcessRequests.h"

#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "Workspace/LauncherStatePaths.h"
#include "ToolOutputPaths.h"

#include <algorithm>
#include <utility>

namespace SparkleLauncher
{
	static void AddCleanStep(
	    std::vector<MaintenanceOperationProcessStep>& steps,
	    std::string id,
	    std::string displayName,
	    std::filesystem::path path,
	    MaintenanceCleanBehavior behavior,
	    std::filesystem::path preservedPath = {},
	    std::string previewDetail = {})
	{
		MaintenanceOperationProcessStep step;
		step.Id = std::move(id);
		step.DisplayName = std::move(displayName);
		step.DestructivePath = std::move(path);
		step.PreservedPath = std::move(preservedPath);
		step.PreviewDetail = std::move(previewDetail);
		step.CleanBehavior = behavior;
		step.bDeletesGeneratedOutput = true;
		steps.push_back(std::move(step));
	}

	static void AddContentGeneratedCleanSteps(std::vector<MaintenanceOperationProcessStep>& steps, const MaintenanceOperationPlan& plan, bool bIncludeBuild, bool bIncludeLogs, bool bIncludeState)
	{
		const std::filesystem::path contentPath = plan.Request.WorkspaceRoot / "Projects" / plan.Request.ContentId;
		if (bIncludeBuild)
		{
			AddCleanStep(steps, "clean-content-build", "Clean content build tree", contentPath / "build", MaintenanceCleanBehavior::RemovePath);
		}
		if (bIncludeLogs)
		{
			AddCleanStep(steps, "clean-content-logs", "Clean content logs", contentPath / "logs", MaintenanceCleanBehavior::RemovePath);
		}
		if (bIncludeState)
		{
			AddCleanStep(steps, "clean-content-imgui", "Clean content ImGui state", contentPath / "imgui.ini", MaintenanceCleanBehavior::RemovePath);
		}
	}

	std::vector<CleanScope> ResolveRequestedCleanScopes(const MaintenanceOperationRequest& request)
	{
		std::vector<CleanScope> scopes = request.RequestedCleanScopes;
		if (scopes.empty())
		{
			scopes.push_back(request.RequestedCleanScope);
		}

		std::vector<CleanScope> uniqueScopes;
		for (const CleanScope scope : scopes)
		{
			if (std::find(uniqueScopes.begin(), uniqueScopes.end(), scope) == uniqueScopes.end())
			{
				uniqueScopes.push_back(scope);
			}
		}
		if (std::find(uniqueScopes.begin(), uniqueScopes.end(), CleanScope::PristineGeneratedWorkspace) != uniqueScopes.end())
		{
			return {CleanScope::PristineGeneratedWorkspace};
		}
		if (std::find(uniqueScopes.begin(), uniqueScopes.end(), CleanScope::ArtifactOutputs) != uniqueScopes.end())
		{
			std::erase(uniqueScopes, CleanScope::CookedOutputs);
		}
		return uniqueScopes;
	}

	static void AddCleanStepsForScope(
	    std::vector<MaintenanceOperationProcessStep>& steps,
	    const MaintenanceOperationPlan& plan,
	    CleanScope scope,
	    const Filesystem::ProductUserStatePaths& productUserState,
	    const SparkleTools::ToolOutputPaths& outputs,
	    const LauncherStatePaths& launcherState)
	{
		switch (scope)
		{
			case CleanScope::CookedOutputs:
				AddCleanStep(steps, "clean-cooked", "Clean cooked content", outputs.CookedProjectDirectory(plan.Request.ContentId), MaintenanceCleanBehavior::RemovePath);
				return;
			case CleanScope::BuildTree:
				AddCleanStep(
				    steps,
				    "clean-build-tree",
				    "Clean build tree except dependency cache",
				    outputs.BuildRoot,
				    MaintenanceCleanBehavior::RemoveDirectoryContentsPreservingPath,
				    outputs.DependencyCacheRoot);

				AddCleanStep(steps, "clean-root-generated", "Clean root CMake and Visual Studio generated files", plan.Request.WorkspaceRoot, MaintenanceCleanBehavior::RemoveRootGeneratedFiles);
				AddContentGeneratedCleanSteps(steps, plan, true, false, false);
				return;
			case CleanScope::ArtifactOutputs:
				AddCleanStep(steps, "clean-artifacts", "Clean generated artifacts", outputs.ArtifactRoot, MaintenanceCleanBehavior::RemovePath);
				return;
			case CleanScope::WorkspaceState:
				AddCleanStep(steps, "clean-dot-vs", "Clean Visual Studio workspace state", plan.Request.WorkspaceRoot / ".vs", MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-dot-vscode", "Clean VS Code workspace state", plan.Request.WorkspaceRoot / ".vscode", MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-dot-idea", "Clean Rider workspace state", plan.Request.WorkspaceRoot / ".idea", MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-root-imgui", "Clean root ImGui state", plan.Request.WorkspaceRoot / "imgui.ini", MaintenanceCleanBehavior::RemovePath);
				AddContentGeneratedCleanSteps(steps, plan, false, false, true);
				AddCleanStep(steps, "clean-product-settings", "Clean product user settings", productUserState.SettingsRoot, MaintenanceCleanBehavior::RemovePath);
				return;
			case CleanScope::ThirdPartyDependencyCache:
				AddCleanStep(steps, "clean-dependency-cache", "Clean source dependency cache", outputs.DependencyCacheRoot, MaintenanceCleanBehavior::RemovePath);
				return;
			case CleanScope::Logs:
				AddCleanStep(steps, "clean-root-logs", "Clean legacy repository logs", plan.Request.WorkspaceRoot / "logs", MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-product-logs", "Clean product logs", productUserState.LogsRoot, MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-launcher-logs", "Clean launcher logs", launcherState.LogsRoot, MaintenanceCleanBehavior::RemovePath);
				AddContentGeneratedCleanSteps(steps, plan, false, true, false);
				return;
			case CleanScope::PristineGeneratedWorkspace:
				AddCleanStep(steps, "clean-build", "Clean build tree", outputs.BuildRoot, MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-artifacts", "Clean development artifacts", outputs.ArtifactRoot, MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-dot-vs", "Clean Visual Studio workspace state", plan.Request.WorkspaceRoot / ".vs", MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-dot-vscode", "Clean VS Code workspace state", plan.Request.WorkspaceRoot / ".vscode", MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-dot-idea", "Clean Rider workspace state", plan.Request.WorkspaceRoot / ".idea", MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-logs", "Clean legacy repository logs", plan.Request.WorkspaceRoot / "logs", MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-product-state", "Clean product user state", productUserState.Root, MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-launcher-state", "Clean launcher state", launcherState.Root, MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-root-imgui", "Clean root ImGui state", plan.Request.WorkspaceRoot / "imgui.ini", MaintenanceCleanBehavior::RemovePath);
				AddCleanStep(steps, "clean-root-generated", "Clean root CMake and Visual Studio generated files", plan.Request.WorkspaceRoot, MaintenanceCleanBehavior::RemoveRootGeneratedFiles);
				AddContentGeneratedCleanSteps(steps, plan, true, true, true);
				return;
		}
	}

	static void AppendCleanSteps(std::vector<MaintenanceOperationProcessStep>& steps, const MaintenanceOperationPlan& plan)
	{
		if (!plan.Request.RequestedCleanTargets.empty())
		{
			for (const MaintenanceCleanPathSpec& target : plan.Request.RequestedCleanTargets)
			{
				AddCleanStep(steps, "clean-explicit-target", "Clean " + target.DisplayName, target.Path, MaintenanceCleanBehavior::RemovePath, {}, target.Detail);
			}
			return;
		}

		const Filesystem::ProductUserStatePaths productUserState = Filesystem::ResolveDevelopmentProductUserStatePaths(plan.Request.WorkspaceRoot, plan.Request.ContentId);
		const SparkleTools::ToolOutputPaths outputs = SparkleTools::ResolveToolOutputPaths(plan.Request.WorkspaceRoot);
		const LauncherStatePaths launcherState = ResolveLauncherStatePaths(plan.Request.WorkspaceRoot);
		for (const CleanScope scope : ResolveRequestedCleanScopes(plan.Request))
		{
			AddCleanStepsForScope(steps, plan, scope, productUserState, outputs, launcherState);
		}
	}

	std::vector<MaintenanceOperationProcessStep> BuildMaintenanceCleanSteps(const MaintenanceOperationPlan& plan)
	{
		std::vector<MaintenanceOperationProcessStep> steps;
		AppendCleanSteps(steps, plan);
		return steps;
	}

	std::vector<MaintenanceOperationProcessStep> BuildMaintenanceProcessStepsForPlan(const MaintenanceOperationPlan& plan)
	{
		std::vector<MaintenanceOperationProcessStep> steps;
		if (!plan.bCanRun)
		{
			return steps;
		}

		switch (plan.Kind)
		{
			case MaintenanceOperationKind::CleanWorkspace:
				return BuildMaintenanceCleanSteps(plan);
		}

		return steps;
	}
}
