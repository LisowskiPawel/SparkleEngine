#include "Build/BuildWorkspaceProcessRequests.h"

#include "Build/CMakeWorkflowProcessRequests.h"
#include "Build/Toolchains/HostToolInstaller.h"
#include "Core/Public/Diagnostics/Error.h"

#include <string_view>
#include <utility>

namespace SparkleLauncher
{
	static Process::ChildProcessRequest MakeConfigureRequest(const BuildWorkspaceOperationPlan& plan)
	{
		if (plan.Kind == BuildWorkspaceOperationKind::CompileLauncher)
		{
			return MakeCMakeLauncherConfigureRequest(plan.Request.WorkspaceRoot, plan.Toolchain, plan.Operation.Id, "Configure.txt");
		}
		if (!plan.Request.SourceDependencyId.empty())
		{
			return MakeCMakeDependencySyncRequest(plan.Request.WorkspaceRoot, plan.Toolchain, plan.Operation.Id, plan.Request.SourceDependencyId, "SyncSourceDependency.txt");
		}
		return MakeCMakeConfigureRequest(plan.Request.WorkspaceRoot, plan.Toolchain, plan.Operation.Id, "Configure.txt");
	}

	static Process::ChildProcessRequest MakeBuildRequest(const BuildWorkspaceOperationPlan& plan, std::string_view profileName, const std::vector<std::string>& targets)
	{
		if (plan.Kind == BuildWorkspaceOperationKind::CompileLauncher)
		{
			return MakeCMakeLauncherBuildRequest(plan.Request.WorkspaceRoot, plan.Toolchain, plan.Operation.Id, profileName, "Build.txt");
		}
		return MakeCMakeBuildRequest(plan.Request.WorkspaceRoot, plan.Toolchain, plan.Operation.Id, profileName, targets, "Build.txt");
	}

	static std::vector<std::string> RequireBuildTargets(const BuildWorkspaceOperationPlan& plan, BuildWorkspaceScope scope)
	{
		std::vector<std::string> targets = ResolveWorkspaceBuildTargets(plan, scope);
		if (!targets.empty())
		{
			return targets;
		}
		switch (scope)
		{
			case BuildWorkspaceScope::Editor:
				throw Diagnostics::Error("No editor build target could be resolved.");
			case BuildWorkspaceScope::Game:
				throw Diagnostics::Error("No game build target could be resolved.");
			case BuildWorkspaceScope::CookTools:
				throw Diagnostics::Error(
				    plan.Kind == BuildWorkspaceOperationKind::BuildWorkspace ? "No cook-tool targets are enabled in this workspace configuration."
				                                                             : "No optional cook-tool targets are enabled in this workspace configuration.");

			case BuildWorkspaceScope::Launcher:
				throw Diagnostics::Error("No launcher build target could be resolved.");
		}
		throw Diagnostics::Error("Unknown workspace build scope.");
	}

	static void AddConfigureStep(std::vector<BuildWorkspaceProcessStep>& steps, const BuildWorkspaceOperationPlan& plan)
	{
		BuildWorkspaceProcessStep step;
		step.Id = "configure";
		step.DisplayName = "Generate build files";
		step.Request = MakeConfigureRequest(plan);
		step.bUpdatesBuildFilesFreshness = plan.Kind != BuildWorkspaceOperationKind::CompileLauncher && plan.Request.SourceDependencyId.empty();
		steps.push_back(std::move(step));
	}

	static void AddBuildStep(std::vector<BuildWorkspaceProcessStep>& steps, const BuildWorkspaceOperationPlan& plan, std::string_view profileName, const std::vector<std::string>& targets)
	{
		BuildWorkspaceProcessStep step;
		step.Id = "build";
		step.DisplayName = "Build";
		for (std::size_t index = 0; index < targets.size(); ++index)
		{
			step.DisplayName += index == 0 ? " " : ", ";
			step.DisplayName += targets[index];
		}
		step.DisplayName += " for " + std::string(profileName);
		step.Request = MakeBuildRequest(plan, profileName, targets);
		steps.push_back(std::move(step));
	}

	static void AddWorkspaceBuildSteps(std::vector<BuildWorkspaceProcessStep>& steps, const BuildWorkspaceOperationPlan& plan)
	{
		if (BuildWorkspaceOperationRequiresConfigureStep(plan))
		{
			AddConfigureStep(steps, plan);
		}
		std::vector<std::string> editorProfileTargets;
		for (BuildWorkspaceScope scope : {BuildWorkspaceScope::Launcher, BuildWorkspaceScope::Editor, BuildWorkspaceScope::CookTools})
		{
			if (HasSelectedBuildScope(plan.Request, scope))
			{
				const std::vector<std::string> targets = RequireBuildTargets(plan, scope);
				editorProfileTargets.insert(editorProfileTargets.end(), targets.begin(), targets.end());
			}
		}
		if (!editorProfileTargets.empty())
		{
			AddBuildStep(steps, plan, plan.Request.EditorProfile, editorProfileTargets);
		}
		if (HasSelectedBuildScope(plan.Request, BuildWorkspaceScope::Game))
		{
			AddBuildStep(steps, plan, plan.Request.GameProfile, RequireBuildTargets(plan, BuildWorkspaceScope::Game));
		}
	}

	static void AddHostToolInstallStep(std::vector<BuildWorkspaceProcessStep>& steps, const BuildWorkspaceOperationPlan& plan)
	{
		std::string errorMessage;
		std::optional<Process::ChildProcessRequest> request = BuildHostToolInstallRequest(plan.Request.HostToolId, plan.Toolchain, plan.Request.WorkspaceRoot, plan.Operation.Id, errorMessage);
		if (!request.has_value())
		{
			throw Diagnostics::Error(errorMessage);
		}

		BuildWorkspaceProcessStep step;
		step.Id = "install-host-tool";
		step.DisplayName = "Install host tool";
		step.Request = std::move(*request);
		steps.push_back(std::move(step));
	}

	std::vector<BuildWorkspaceProcessStep> BuildProcessStepsForPlan(const BuildWorkspaceOperationPlan& plan)
	{
		std::vector<BuildWorkspaceProcessStep> steps;
		if (plan.Kind == BuildWorkspaceOperationKind::InstallHostTool)
		{
			AddHostToolInstallStep(steps, plan);
			return steps;
		}
		if (!plan.Toolchain.bRequiredToolsAvailable)
		{
			return steps;
		}

		switch (plan.Kind)
		{
			case BuildWorkspaceOperationKind::SyncCode:
			case BuildWorkspaceOperationKind::GenerateBuildFiles:
				AddConfigureStep(steps, plan);
				return steps;
			case BuildWorkspaceOperationKind::BuildWorkspace:
				AddWorkspaceBuildSteps(steps, plan);
				return steps;
			case BuildWorkspaceOperationKind::CompileLauncher:
			{
				AddConfigureStep(steps, plan);
				const std::vector<std::string> targets = RequireBuildTargets(plan, BuildWorkspaceScope::Launcher);
				AddBuildStep(steps, plan, plan.Request.EditorProfile, targets);
				return steps;
			}
			case BuildWorkspaceOperationKind::CompileEditor:
			{
				const std::vector<std::string> targets = RequireBuildTargets(plan, BuildWorkspaceScope::Editor);
				AddBuildStep(steps, plan, plan.Request.EditorProfile, targets);
				return steps;
			}
			case BuildWorkspaceOperationKind::CompileGame:
			{
				const std::vector<std::string> targets = RequireBuildTargets(plan, BuildWorkspaceScope::Game);
				AddBuildStep(steps, plan, plan.Request.GameProfile, targets);
				return steps;
			}
			case BuildWorkspaceOperationKind::BuildCookTools:
			{
				AddBuildStep(steps, plan, plan.Request.EditorProfile, RequireBuildTargets(plan, BuildWorkspaceScope::CookTools));
				return steps;
			}
			case BuildWorkspaceOperationKind::InstallHostTool:
				return steps;
		}

		return steps;
	}
}
