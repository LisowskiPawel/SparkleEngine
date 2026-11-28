#include "SparkleLauncher/BuildWorkspaceOperations.h"

#include "Workspace/LauncherProductPaths.h"
#include "Build/BuildWorkspaceProcessRequests.h"
#include "Core/Public/Diagnostics/Error.h"
#include "ToolOutputPaths.h"

#include <algorithm>

namespace SparkleLauncher
{
	static bool RequireCurrentWorkspace(BuildWorkspaceOperationPlan& plan)
	{
		if (plan.Freshness.bCurrent)
		{
			return true;
		}

		plan.ReadinessMessages.push_back("Generated build files are not current. Run Generate Build Files first.");
		return false;
	}

	static bool RequireConfigurePrerequisites(BuildWorkspaceOperationPlan& plan)
	{
		if (plan.Toolchain.bConfigurePrerequisitesAvailable)
		{
			return true;
		}

		plan.ReadinessMessages.push_back("Enabled workspace configure prerequisites are incomplete.");
		for (const ToolchainItemStatus& item : plan.Toolchain.Items)
		{
			const bool bUnresolvedRequiredToolId = item.Id == "shader-compiler-sdk" || item.Id == "vulkan-sdk";
			const bool bUnresolvedRequiredToolState = bUnresolvedRequiredToolId && (item.State != ToolchainItemState::Found);
			const bool bUnresolvedRequiredTool = bUnresolvedRequiredToolState && (!item.Detail.empty());

			if (bUnresolvedRequiredTool)
			{
				plan.ReadinessMessages.push_back(item.Detail);
			}
		}
		return false;
	}

	static bool RequireSyncedSourceDependencies(BuildWorkspaceOperationPlan& plan)
	{
		if (!HasIncompleteEnabledSourceDependencies(plan))
		{
			return true;
		}

		plan.ReadinessMessages.push_back("Enabled repository dependencies are incomplete. Run Sync Code before local build workflows.");
		return false;
	}

	static bool AdmitWorkspaceOperation(BuildWorkspaceOperationPlan& plan)
	{
		if (!plan.Toolchain.bRequiredToolsAvailable)
		{
			plan.ReadinessMessages.push_back("Required host prerequisites are missing or unsupported.");
			return false;
		}

		switch (plan.Kind)
		{
			case BuildWorkspaceOperationKind::SyncCode:
				if (plan.Request.SourceDependencyId.empty() && !RequireConfigurePrerequisites(plan))
				{
					return false;
				}
				return true;
			case BuildWorkspaceOperationKind::GenerateBuildFiles:
				return RequireConfigurePrerequisites(plan);
			case BuildWorkspaceOperationKind::BuildWorkspace:
				if (plan.Request.SelectedScopes.empty())
				{
					plan.ReadinessMessages.push_back("Select at least one workspace build scope.");
					return false;
				}
				if (!RequireSyncedSourceDependencies(plan) || !RequireConfigurePrerequisites(plan))
				{
					return false;
				}
				return true;
			case BuildWorkspaceOperationKind::CompileEditor:
			case BuildWorkspaceOperationKind::CompileGame:
			case BuildWorkspaceOperationKind::BuildCookTools:
				return RequireSyncedSourceDependencies(plan) && RequireConfigurePrerequisites(plan) && RequireCurrentWorkspace(plan);
		}
		return false;
	}

	const std::vector<BuildWorkspaceOperationDefinition>& GetBuildWorkspaceOperationDefinitions()
	{
		static const std::vector<BuildWorkspaceOperationDefinition> definitions = {
		    {BuildWorkspaceOperationKind::SyncCode, "workspace.sync-code", "Sync Code", "Download enabled repository dependencies and refresh workspace configure state."},
		    {BuildWorkspaceOperationKind::GenerateBuildFiles,
		        "workspace.generate-build-files",
		        "Generate Build Files",
		        "Refresh generated CMake and IDE build files for the selected generator, platform, toolset, and Qt kit."},
		    {BuildWorkspaceOperationKind::BuildWorkspace,
		        "workspace.build",
		        "Build Workspace",
		        "Build the selected editor, game and cooking-tool scopes; refresh generated workspace files when needed."},
		    {BuildWorkspaceOperationKind::CompileEditor, "workspace.build.editor", "Build Editor", "Optional local rebuild of the editor target."},
		    {BuildWorkspaceOperationKind::CompileGame, "workspace.build.game", "Build Game", "Optional local rebuild of the game target."},
		    {BuildWorkspaceOperationKind::BuildCookTools, "cook.tools.prepare", "Build Cooking Tools", "Optional local build of tools required by recook workflows."},
		};

		return definitions;
	}

	const BuildWorkspaceOperationDefinition* FindBuildWorkspaceOperationDefinition(std::string_view operationId)
	{
		const std::vector<BuildWorkspaceOperationDefinition>& definitions = GetBuildWorkspaceOperationDefinitions();
		const auto found = std::find_if(definitions.begin(), definitions.end(), [operationId](const BuildWorkspaceOperationDefinition& definition) { return definition.Id == operationId; });
		return found == definitions.end() ? nullptr : &*found;
	}

	static void InspectWorkspacePrerequisites(BuildWorkspaceOperationPlan& plan)
	{
		const BuildWorkspaceOperationRequest& request = plan.Request;
		plan.Toolchain = DetectBuildToolchain(request.PreferredIde, request.Compiler);
		plan.Freshness = CheckBuildFilesFreshness(request.WorkspaceRoot, plan.Toolchain);
		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(request.WorkspaceRoot);
		plan.SourceDependencies = InspectSourceDependencyCache(workspaceOutputs.DependencyCacheRoot);

		plan.ReadinessMessages.push_back(plan.Toolchain.bRequiredToolsAvailable ? "Required toolchain is available." : "Required toolchain is incomplete.");
		plan.ReadinessMessages.push_back(plan.Freshness.Summary);
		if (!plan.SourceDependencies.bAllEnabledDependenciesReady)
		{
			for (const std::string& message : plan.SourceDependencies.ReadinessMessages)
			{
				plan.ReadinessMessages.push_back(message);
			}
		}
	}

	static void PrepareBuildSteps(BuildWorkspaceOperationPlan& plan)
	{
		try
		{
			plan.Steps = BuildProcessStepsForPlan(plan);
		}
		catch (const Diagnostics::Error& error)
		{
			plan.bCanRun = false;
			plan.Steps.clear();
			plan.ReadinessMessages.push_back(std::string("Operation planning failed: ") + error.what());
		}
	}

	BuildWorkspaceOperationPlan PlanLauncherOperation(std::string_view operationId, const BuildWorkspaceOperationRequest& request)
	{
		BuildWorkspaceOperationPlan plan;
		const BuildWorkspaceOperationDefinition* definition = FindBuildWorkspaceOperationDefinition(operationId);
		if (definition == nullptr)
		{
			plan.Operation = MakeOperationRecord(std::string(operationId), "Unknown build operation");
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, "Unknown build/workspace operation id.", "Choose a registered Build or Sync operation, then retry.");
			plan.ReadinessMessages.push_back(plan.Operation.Failure->Summary);
			return plan;
		}

		plan.Kind = definition->Kind;
		plan.Operation = MakeOperationRecord(definition->Id, definition->DisplayName);
		plan.Request = request;
		if (!HasBuildSources(request.WorkspaceRoot))
		{
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, "Source workspace operations are unavailable in a portable package.", "Use a source checkout for this operation.");
			plan.ReadinessMessages.push_back(plan.Operation.Failure->Summary);
			return plan;
		}

		InspectWorkspacePrerequisites(plan);
		plan.bCanRun = AdmitWorkspaceOperation(plan);
		if (plan.bCanRun)
		{
			PrepareBuildSteps(plan);
		}
		return plan;
	}
}
