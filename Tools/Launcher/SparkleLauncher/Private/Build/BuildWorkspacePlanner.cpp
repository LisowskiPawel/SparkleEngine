#include "SparkleLauncher/BuildWorkspaceOperations.h"

#include "Workspace/LauncherProductPaths.h"
#include "Build/NativeBuildOutputReset.h"
#include "Build/BuildWorkspaceProcessRequests.h"
#include "Build/Toolchains/HostToolInstaller.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"
#include "SparkleLauncher/ArtifactNaming.h"
#include "Workspace/LauncherStatePaths.h"
#include "ToolOutputPaths.h"

#include <algorithm>
#include <optional>
#include <sstream>
#include <utility>

namespace SparkleLauncher
{
	static void AddReadiness(BuildWorkspaceOperationPlan& plan, std::string message)
	{
		plan.ReadinessMessages.push_back(std::move(message));
	}

	static void AddPlannedEffect(BuildWorkspaceOperationPlan& plan, std::string message)
	{
		plan.PlannedEffects.push_back(std::move(message));
	}

	static bool RequireCurrentWorkspace(BuildWorkspaceOperationPlan& plan)
	{
		if (plan.Freshness.bCurrent)
		{
			return true;
		}

		AddReadiness(plan, "Generated build files are not current. Run Generate Build Files first.");
		return false;
	}

	static bool RequireConfigurePrerequisites(BuildWorkspaceOperationPlan& plan)
	{
		if (plan.Toolchain.bConfigurePrerequisitesAvailable)
		{
			return true;
		}

		AddReadiness(plan, "Enabled workspace configure prerequisites are incomplete.");
		for (const ToolchainItemStatus& item : plan.Toolchain.Items)
		{
			const bool bUnresolvedRequiredToolId = item.Id == "shader-compiler-sdk" || item.Id == "vulkan-sdk";
			const bool bUnresolvedRequiredToolState = bUnresolvedRequiredToolId && (item.State != ToolchainItemState::Found);
			const bool bUnresolvedRequiredTool = bUnresolvedRequiredToolState && (!item.Detail.empty());
			
			if (bUnresolvedRequiredTool)
			{
				AddReadiness(plan, item.Detail);
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

		AddReadiness(plan, "Enabled repository dependencies are incomplete. Run Sync Code before local build workflows.");
		return false;
	}

	static bool AdmitHostToolInstallation(BuildWorkspaceOperationPlan& plan)
	{
		const BuildWorkspaceOperationRequest& request = plan.Request;
		const HostToolInstallerDefinition* installer = FindHostToolInstaller(request.HostToolId);
		if (installer == nullptr)
		{
			AddReadiness(plan, "No launcher installer is registered for host tool '" + request.HostToolId + "'.");
			return false;
		}

		const auto status = std::find_if(plan.Toolchain.Items.begin(), plan.Toolchain.Items.end(), [&request](const ToolchainItemStatus& item) { return item.Id == request.HostToolId; });
		if (status != plan.Toolchain.Items.end() && status->State == ToolchainItemState::Found)
		{
			AddReadiness(plan, installer->DisplayName + " is already installed.");
			return false;
		}
		if (!CanInstallHostTool(request.HostToolId, plan.Toolchain))
		{
			AddReadiness(plan, "The registered launcher installer for " + installer->DisplayName + " is unavailable on this machine.");
			return false;
		}

		AddPlannedEffect(plan, installer->InstallEffect);
		return true;
	}

	static bool AdmitWorkspaceOperation(BuildWorkspaceOperationPlan& plan)
	{
		if (plan.Kind == BuildWorkspaceOperationKind::InstallHostTool)
		{
			return AdmitHostToolInstallation(plan);
		}
		if (!plan.Toolchain.bRequiredToolsAvailable)
		{
			AddReadiness(plan, "Required host prerequisites are missing or unsupported.");
			return false;
		}

		switch (plan.Kind)
		{
			case BuildWorkspaceOperationKind::SyncCode:
				if (!RequireConfigurePrerequisites(plan))
				{
					return false;
				}
				AddPlannedEffect(plan, "Validate pinned source dependencies and acquire missing caches.");
				return true;
			case BuildWorkspaceOperationKind::GenerateBuildFiles:
				if (!RequireConfigurePrerequisites(plan))
				{
					return false;
				}
				if (HasIncompleteEnabledSourceDependencies(plan))
				{
					AddPlannedEffect(plan, "Repair incomplete enabled source dependency caches while refreshing generated workspace files.");
				}
				if (RequiresNativeBuildOutputReset(plan.Freshness.State))
				{
					AddPlannedEffect(plan, "Reset compiler-produced outputs that are incompatible with the selected toolchain while preserving source caches and cooked content.");
				}
				return true;
			case BuildWorkspaceOperationKind::BuildWorkspace:
				if (plan.Request.SelectedScopes.empty())
				{
					AddReadiness(plan, "Select at least one workspace build scope.");
					return false;
				}
				if (!RequireSyncedSourceDependencies(plan) || !RequireConfigurePrerequisites(plan))
				{
					return false;
				}
				if (BuildWorkspaceOperationRequiresConfigureStep(plan) && RequiresNativeBuildOutputReset(plan.Freshness.State))
				{
					AddPlannedEffect(plan, "Reset compiler-produced outputs that are incompatible with the selected toolchain while preserving source caches and cooked content.");
				}
				AddPlannedEffect(plan, "Build the selected workspace products in dependency order using incremental CMake builds.");
				return true;
			case BuildWorkspaceOperationKind::CompileLauncher:
				AddPlannedEffect(plan, "Configure the Launcher-owned product graph, then rebuild its executable and deployed support files.");
				return true;
			case BuildWorkspaceOperationKind::CompileEditor:
			case BuildWorkspaceOperationKind::CompileGame:
			case BuildWorkspaceOperationKind::BuildCookTools:
				return RequireSyncedSourceDependencies(plan) && RequireConfigurePrerequisites(plan) && RequireCurrentWorkspace(plan);
			case BuildWorkspaceOperationKind::InstallHostTool:
				return false;
		}
		return false;
	}

	const std::vector<BuildWorkspaceOperationDefinition>& GetBuildWorkspaceOperationDefinitions()
	{
		static const std::vector<BuildWorkspaceOperationDefinition> definitions = {
		    {BuildWorkspaceOperationKind::SyncCode,
		        "workspace.sync-code",
		        "Sync",
		        std::string(ArtifactNaming::kActionSyncSourceDependencies),
		        "Download enabled repository dependencies and refresh workspace configure state; registered host-tool actions remain "
		        "available "
		        "on the same page."},
		    {BuildWorkspaceOperationKind::GenerateBuildFiles,
		        "workspace.generate-build-files",
		        "Build",
		        std::string(ArtifactNaming::kActionGenerateBuildFiles),
		        "Refresh generated CMake and IDE build files for the selected generator, platform, toolset, and Qt kit."},
		    {BuildWorkspaceOperationKind::BuildWorkspace,
		        "workspace.build",
		        "Build",
		        "Build Workspace",
		        "Build the selected editor, game, cooking-tool, and launcher scopes; refresh generated workspace files when needed."},
		    {BuildWorkspaceOperationKind::CompileLauncher, "launcher.build.self", "Build", "Build Launcher", "Optional local rebuild of Sparkle Launcher for development or customization.", true},
		    {BuildWorkspaceOperationKind::CompileEditor, "workspace.build.editor", "Build", "Build Editor", "Optional local rebuild of the editor target."},
		    {BuildWorkspaceOperationKind::CompileGame, "workspace.build.game", "Build", "Build Game", "Optional local rebuild of the game target."},
		    {BuildWorkspaceOperationKind::BuildCookTools, "cook.tools.prepare", "Build", "Build Cooking Tools", "Optional local build of tools required by recook workflows."},
		    {BuildWorkspaceOperationKind::InstallHostTool,
		        "workspace.install-host-tool",
		        "Setup",
		        "Install Host Tool",
		        "Install a registered host tool through its launcher-owned installation provider."},
		};

		return definitions;
	}

	std::optional<BuildWorkspaceOperationDefinition> FindBuildWorkspaceOperationDefinition(std::string_view operationId)
	{
		const std::vector<BuildWorkspaceOperationDefinition>& definitions = GetBuildWorkspaceOperationDefinitions();
		const auto found = std::find_if(definitions.begin(), definitions.end(), [operationId](const BuildWorkspaceOperationDefinition& definition) { return definition.Id == operationId; });
		return found == definitions.end() ? std::nullopt : std::optional<BuildWorkspaceOperationDefinition>(*found);
	}

	static void RecordBuildInputs(BuildWorkspaceOperationPlan& plan)
	{
		const BuildWorkspaceOperationRequest& request = plan.Request;
		plan.Operation.Inputs.push_back({"content", request.ContentId});
		plan.Operation.Inputs.push_back({"editorProfile", request.EditorProfile});
		plan.Operation.Inputs.push_back({"gameProfile", request.GameProfile});
		plan.Operation.Inputs.push_back({"workspaceIde", WorkspaceIdeCommandLineValue(request.PreferredIde)});
		plan.Operation.Inputs.push_back({"workspaceCompiler", WorkspaceCompilerCommandLineValue(request.Compiler)});
		if (plan.Kind == BuildWorkspaceOperationKind::BuildWorkspace && !request.SelectedScopes.empty())
		{
			std::vector<std::string> scopeIds;
			scopeIds.reserve(request.SelectedScopes.size());
			for (const BuildWorkspaceScope scope : request.SelectedScopes)
			{
				scopeIds.push_back(BuildWorkspaceScopeId(scope));
			}
			std::vector<std::string_view> scopeViews;
			scopeViews.reserve(scopeIds.size());
			for (const std::string& scopeId : scopeIds)
			{
				scopeViews.push_back(scopeId);
			}
			plan.Operation.Inputs.push_back({"buildScopes", Strings::Join(scopeViews, ",")});
		}
		if (!request.SourceDependencyId.empty())
		{
			plan.Operation.Inputs.push_back({"sourceDependency", request.SourceDependencyId});
		}
		if (!request.HostToolId.empty())
		{
			plan.Operation.Inputs.push_back({"hostTool", request.HostToolId});
		}
	}

	static void InspectWorkspacePrerequisites(BuildWorkspaceOperationPlan& plan)
	{
		const BuildWorkspaceOperationRequest& request = plan.Request;
		plan.Toolchain = DetectBuildToolchain(request.WorkspaceRoot, request.PreferredIde, request.Compiler);
		plan.Freshness = CheckBuildFilesFreshness(request.WorkspaceRoot, plan.Toolchain);
		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(request.WorkspaceRoot);
		plan.SourceDependencies = InspectSourceDependencyCache(workspaceOutputs.DependencyCacheRoot);

		AddReadiness(plan, plan.Toolchain.bRequiredToolsAvailable ? "Required toolchain is available." : "Required toolchain is incomplete.");
		AddReadiness(plan, plan.Freshness.Summary);
		if (!plan.SourceDependencies.bAllEnabledDependenciesReady)
		{
			for (const std::string& message : plan.SourceDependencies.ReadinessMessages)
			{
				AddReadiness(plan, message);
			}
		}
	}

	static void BuildExecutablePreview(BuildWorkspaceOperationPlan& plan)
	{
		try
		{
			const std::vector<BuildWorkspaceProcessStep> processSteps = BuildProcessStepsForPlan(plan);
			for (const BuildWorkspaceProcessStep& processStep : processSteps)
			{
				BuildWorkspaceOperationStep step;
				step.Id = processStep.Id;
				step.DisplayName = processStep.DisplayName;
				step.DisplayCommandLine = BuildDisplayCommandLine(processStep.Request.ExecutablePath, processStep.Request.Arguments);
				step.LogPath = processStep.Request.LogPath;
				step.bUpdatesBuildFilesFreshness = processStep.bUpdatesBuildFilesFreshness;
				plan.Steps.push_back(std::move(step));
				if (plan.Kind != BuildWorkspaceOperationKind::InstallHostTool)
				{
					AddPlannedEffect(plan, processStep.DisplayName + ".");
				}
			}
		}
		catch (const Diagnostics::Error& error)
		{
			plan.bCanRun = false;
			plan.Steps.clear();
			AddReadiness(plan, std::string("Operation planning failed: ") + error.what());
		}
	}

	static void DescribeBuildDryRun(BuildWorkspaceOperationPlan& plan, std::string_view displayName)
	{
		std::ostringstream dryRun;
		dryRun << "Dry-run plan for " << displayName << ":";
		for (const BuildWorkspaceOperationStep& step : plan.Steps)
		{
			dryRun << "\n  " << step.DisplayName << ": " << step.DisplayCommandLine;
		}
		if (plan.Steps.empty())
		{
			dryRun << "\n  No command step required.";
		}
		plan.Operation.DryRunText = dryRun.str();
	}

	BuildWorkspaceOperationPlan PlanBuildWorkspaceOperation(std::string_view operationId, const BuildWorkspaceOperationRequest& request)
	{
		BuildWorkspaceOperationPlan plan;
		const std::optional<BuildWorkspaceOperationDefinition> definition = FindBuildWorkspaceOperationDefinition(operationId);
		if (!definition.has_value())
		{
			plan.Operation = MakeOperationRecord(std::string(operationId), "Unknown build operation");
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, "Unknown build/workspace operation id.", "Choose a registered Build or Sync operation, then retry.");
			AddReadiness(plan, plan.Operation.Failure->Summary);
			return plan;
		}

		plan.Kind = definition->Kind;
		plan.Operation = MakeOperationRecord(definition->Id, definition->DisplayName);
		plan.Request = request;
		plan.Operation.LogPath = ResolveLauncherOperationLogPath(request.WorkspaceRoot, definition->Id, "Latest.txt");
		RecordBuildInputs(plan);
		if (!HasBuildSources(request.WorkspaceRoot))
		{
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, "Source workspace operations are unavailable in a portable package.", "Use a source checkout for this operation.");
			AddReadiness(plan, plan.Operation.Failure->Summary);
			return plan;
		}

		InspectWorkspacePrerequisites(plan);
		plan.bCanRun = AdmitWorkspaceOperation(plan);
		if (plan.bCanRun)
		{
			BuildExecutablePreview(plan);
		}
		DescribeBuildDryRun(plan, definition->DisplayName);
		return plan;
	}
}
