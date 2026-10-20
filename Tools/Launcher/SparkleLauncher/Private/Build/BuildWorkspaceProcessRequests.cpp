#include "BuildWorkspaceProcessRequests.h"

#include "CMakeWorkflowProcessRequests.h"
#include "HostToolInstaller.h"
#include "Core/Public/Diagnostics/Error.h"

#include <string_view>
#include <utility>

namespace SparkleLauncher
{
	static ProcessRequest MakeConfigureRequest(const BuildWorkspaceOperationPlan& plan)
	{
		if (plan.Kind == BuildWorkspaceOperationKind::CompileLauncher)
		{
			return MakeCMakeLauncherConfigureRequest(plan.RepositoryRoot, plan.Toolchain, plan.Operation.Id, "Configure.txt");
		}
		if (!plan.Request.SourceDependencyId.empty())
		{
			return MakeCMakeDependencySyncRequest(plan.RepositoryRoot, plan.Toolchain, plan.Operation.Id, plan.Request.SourceDependencyId, "SyncSourceDependency.txt");
		}
		return MakeCMakeConfigureRequest(plan.RepositoryRoot, plan.Toolchain, plan.Operation.Id, "Configure.txt");
	}

	static ProcessRequest MakeBuildRequest(const BuildWorkspaceOperationPlan& plan, std::string_view profileName, const std::vector<std::string>& targets)
	{
		if (plan.Kind == BuildWorkspaceOperationKind::CompileLauncher)
		{
			return MakeCMakeLauncherBuildRequest(plan.RepositoryRoot, plan.Toolchain, plan.Operation.Id, profileName, "Build.txt");
		}
		return MakeCMakeBuildRequest(plan.RepositoryRoot, plan.Toolchain, plan.Operation.Id, profileName, targets, "Build.txt");
	}

	static void AppendTargets(std::vector<std::string>& destination, const std::vector<std::string>& targets)
	{
		destination.insert(destination.end(), targets.begin(), targets.end());
	}

	static void AddConfigureStep(std::vector<BuildWorkspaceProcessStep>& steps, const BuildWorkspaceOperationPlan& plan)
	{
		BuildWorkspaceProcessStep step;
		step.Id = "configure";
		step.DisplayName = "Generate build files";
		step.Request = MakeConfigureRequest(plan);
		step.UpdatesBuildFilesFreshness = plan.Kind != BuildWorkspaceOperationKind::CompileLauncher && plan.Request.SourceDependencyId.empty();
		steps.push_back(std::move(step));
	}

	static void AddBuildStep(std::vector<BuildWorkspaceProcessStep>& steps, const BuildWorkspaceOperationPlan& plan, std::string_view profileName, const std::vector<std::string>& targets)
	{
		BuildWorkspaceProcessStep step;
		step.Id = "build";
		step.DisplayName = "Build targets";
		step.Request = MakeBuildRequest(plan, profileName, targets);
		steps.push_back(std::move(step));
	}

	static void AddHostToolInstallStep(std::vector<BuildWorkspaceProcessStep>& steps, const BuildWorkspaceOperationPlan& plan)
	{
		std::string errorMessage;
		std::optional<ProcessRequest> request = BuildHostToolInstallRequest(plan.Request.HostToolId, plan.Toolchain, plan.RepositoryRoot, plan.Operation.Id, errorMessage);
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
		if (!plan.Toolchain.RequiredToolsAvailable)
		{
			return steps;
		}

		switch (plan.Kind)
		{
			case BuildWorkspaceOperationKind::SyncCode:
				if (BuildWorkspaceOperationRequiresConfigureStep(plan))
				{
					AddConfigureStep(steps, plan);
				}
				return steps;
			case BuildWorkspaceOperationKind::GenerateBuildFiles:
				AddConfigureStep(steps, plan);
				return steps;
			case BuildWorkspaceOperationKind::BuildWorkspace:
			{
				if (BuildWorkspaceOperationRequiresConfigureStep(plan))
				{
					AddConfigureStep(steps, plan);
				}

				std::vector<std::string> editorProfileTargets;
				if (HasSelectedBuildScope(plan.Request, BuildWorkspaceScope::Launcher))
				{
					AppendTargets(editorProfileTargets, ResolveWorkspaceBuildTargets(plan, BuildWorkspaceScope::Launcher));
				}
				if (HasSelectedBuildScope(plan.Request, BuildWorkspaceScope::Editor))
				{
					AppendTargets(editorProfileTargets, ResolveWorkspaceBuildTargets(plan, BuildWorkspaceScope::Editor));
				}
				if (HasSelectedBuildScope(plan.Request, BuildWorkspaceScope::CookTools))
				{
					AppendTargets(editorProfileTargets, ResolveWorkspaceBuildTargets(plan, BuildWorkspaceScope::CookTools));
				}
				if (!editorProfileTargets.empty())
				{
					AddBuildStep(steps, plan, plan.Request.EditorProfile, editorProfileTargets);
				}
				if (HasSelectedBuildScope(plan.Request, BuildWorkspaceScope::Game))
				{
					AddBuildStep(steps, plan, plan.Request.GameProfile, ResolveWorkspaceBuildTargets(plan, BuildWorkspaceScope::Game));
				}
				return steps;
			}
			case BuildWorkspaceOperationKind::CompileLauncher:
				AddConfigureStep(steps, plan);
				AddBuildStep(steps, plan, plan.Request.EditorProfile, ResolveWorkspaceBuildTargets(plan, BuildWorkspaceScope::Launcher));
				return steps;
			case BuildWorkspaceOperationKind::CompileEditor:
				AddBuildStep(steps, plan, plan.Request.EditorProfile, ResolveWorkspaceBuildTargets(plan, BuildWorkspaceScope::Editor));
				return steps;
			case BuildWorkspaceOperationKind::CompileGame:
				AddBuildStep(steps, plan, plan.Request.GameProfile, ResolveWorkspaceBuildTargets(plan, BuildWorkspaceScope::Game));
				return steps;
			case BuildWorkspaceOperationKind::BuildCookTools:
			{
				const std::vector<std::string> cookToolTargets = ResolveWorkspaceBuildTargets(plan, BuildWorkspaceScope::CookTools);
				if (!cookToolTargets.empty())
				{
					AddBuildStep(steps, plan, plan.Request.EditorProfile, cookToolTargets);
				}
				return steps;
			}
			case BuildWorkspaceOperationKind::InstallHostTool:
				return steps;
		}

		return steps;
	}
}
