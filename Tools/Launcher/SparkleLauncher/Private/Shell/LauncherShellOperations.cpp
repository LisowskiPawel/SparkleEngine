#include "Shell/LauncherShellOperations.h"

#include "Startup/LauncherArguments.h"
#include "Shell/LauncherShellModel.h"

#include "Operations/LauncherOperationExecution.h"

#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace SparkleLauncher
{
	static std::optional<LauncherOperationInputs> BuildLauncherShellInputs(std::string_view operationId, const LauncherShellModel& model, const LauncherShellArguments& arguments)
	{
		if (FindBuildWorkspaceOperationDefinition(operationId) != nullptr)
		{
			BuildWorkspaceOperationRequest request;
			request.WorkspaceRoot = model.WorkspaceRoot;
			request.ContentId = model.ContentId;
			request.EditorProfile = model.EditorProfile;
			request.GameProfile = model.GameProfile;
			request.PreferredIde = model.WorkspaceIdePreference;
			request.Compiler = model.WorkspaceCompilerPreference;
			request.bForceConfigure = false;
			return request;
		}
		if (FindCookOperationDefinition(operationId) != nullptr)
		{
			CookOperationRequest request;
			request.WorkspaceRoot = model.WorkspaceRoot;
			request.ContentId = model.ContentId;
			request.GameProfile = model.GameProfile;
			request.Mode = arguments.RequestedCookMode;
			request.bShaderEnableDebugInfo = arguments.bShaderSymbols;
			request.bForceRecookConfirmed = arguments.bForceRecookConfirmed;
			request.SelectedScopes = {CookWorkspaceScope::Shaders, CookWorkspaceScope::Textures, CookWorkspaceScope::SceneAssets};
			return request;
		}
		if (FindLevelOperationDefinition(operationId) != nullptr)
		{
			LevelOperationRequest request;
			request.WorkspaceRoot = model.WorkspaceRoot;
			request.ContentId = model.ContentId;
			if (!arguments.LevelId.empty())
			{
				request.RequestedLevelIds = {arguments.LevelId};
			}
			return request;
		}
		if (FindLevelRunOperationDefinition(operationId) != nullptr)
		{
			LevelRunOperationRequest request;
			request.WorkspaceRoot = model.WorkspaceRoot;
			request.ContentId = model.ContentId;
			request.LevelId = arguments.LevelId;
			request.RunMode = arguments.Product;
			request.ProductProfile = arguments.Product == LevelRunMode::Editor ? model.EditorProfile : model.GameProfile;
			request.GraphicsApi = arguments.GraphicsApi;
			request.GpuCrashMode = arguments.GpuCrashMode;
			request.ShaderExportPath = arguments.ShaderExportPath;
			return request;
		}
		if (FindMaintenanceOperationDefinition(operationId) != nullptr)
		{
			MaintenanceOperationRequest request;
			request.WorkspaceRoot = model.WorkspaceRoot;
			request.ContentId = model.ContentId;
			request.EditorProfile = model.EditorProfile;
			request.RequestedCleanScope = arguments.RequestedCleanScope;
			request.bDestructiveActionConfirmed = arguments.bCleanConfirmed;
			return request;
		}
		return std::nullopt;
	}

	static void AppendLauncherShellDryRun(LauncherShellModel& model, const LauncherOperationPlan& plan)
	{
		std::visit(
		    [&model](const auto& typedPlan)
		    {
			    using Plan = std::remove_cvref_t<decltype(typedPlan)>;
			    model.JobOutput.push_back(typedPlan.Operation.DisplayName + " [" + std::string(typedPlan.bCanRun ? "Ready" : "Blocked") + "]");
			    if constexpr (std::is_same_v<Plan, BuildWorkspaceOperationPlan>)
			    {
				    model.JobOutput.push_back("Editor profile: " + model.EditorProfile);
				    model.JobOutput.push_back("Game profile: " + model.GameProfile);
				    model.JobOutput.push_back("Workspace IDE: " + DisplayName(model.WorkspaceIdePreference));
				    model.JobOutput.push_back("Workspace compiler: " + DisplayName(model.WorkspaceCompilerPreference));
			    }
			    else if constexpr (std::is_same_v<Plan, CookOperationPlan>)
			    {
				    model.JobOutput.push_back("Game profile: " + model.GameProfile);
				    model.JobOutput.push_back("Cook mode: " + ToString(typedPlan.Request.Mode));
				    model.JobOutput.push_back("Cooked output: " + typedPlan.CookedOutputDirectory.string());
				    if (typedPlan.Operation.bRequiresConfirmation)
				    {
					    model.JobOutput.push_back("Confirmation required: " + ToString(typedPlan.Operation.DestructiveScope));
				    }
			    }
			    else if constexpr (std::is_same_v<Plan, LevelOperationPlan>)
			    {
				    model.JobOutput.push_back("Content: " + model.ContentId);
				    for (const std::string& levelId : typedPlan.Request.RequestedLevelIds)
				    {
					    model.JobOutput.push_back("Requested level: " + levelId);
				    }
			    }
			    else if constexpr (std::is_same_v<Plan, MaintenanceOperationPlan>)
			    {
				    model.JobOutput.push_back("Editor profile: " + model.EditorProfile);
				    model.JobOutput.push_back("Clean scope: " + ToString(typedPlan.Request.RequestedCleanScope));
				    model.JobOutput.push_back("Latest log: " + typedPlan.Operation.LogPath.string());
				    if (typedPlan.Operation.bRequiresConfirmation)
				    {
					    model.JobOutput.push_back("Confirmation required: " + ToString(typedPlan.Operation.DestructiveScope));
				    }
				    for (const MaintenanceCleanTarget& target : typedPlan.CleanTargets)
				    {
					    model.JobOutput.push_back("Clean target: " + target.Path.string() + " | " + target.Detail);
				    }
			    }

			    for (const std::string& message : typedPlan.ReadinessMessages)
			    {
				    model.JobOutput.push_back("Readiness: " + message);
			    }
			    for (const std::string& effect : typedPlan.PlannedEffects)
			    {
				    model.JobOutput.push_back("Effect: " + effect);
			    }
			    model.JobOutput.push_back(typedPlan.Operation.DryRunText);
			    RecordLauncherShellActivity(model, typedPlan.Operation.DisplayName + " dry-run planned");
		    },
		    plan);
	}

	bool ApplyLauncherShellDryRun(LauncherShellModel& model, const LauncherShellArguments& arguments)
	{
		const LauncherShellOperationRow* operation = FindLauncherShellOperation(model, arguments.DryRunOperationId);
		if (operation == nullptr)
		{
			model.JobOutput.push_back("Unknown dry-run operation: " + arguments.DryRunOperationId);
			RecordLauncherShellActivity(model, "Dry-run failed: unknown operation " + arguments.DryRunOperationId);
			return false;
		}

		const auto inputs = BuildLauncherShellInputs(operation->Id, model, arguments);
		if (!inputs.has_value())
		{
			model.JobOutput.push_back("No dry-run planner for operation: " + operation->Id);
			RecordLauncherShellActivity(model, "Dry-run failed: no planner for " + operation->Id);
			return false;
		}

		const LauncherOperationPlan plan = PlanLauncherOperation(operation->Id, *inputs);
		AppendLauncherShellDryRun(model, plan);
		return true;
	}

	static int ReportLauncherShellOperationResult(const OperationRecord& operation, std::ostream& output, std::ostream& error)
	{
		output << "Result: " << ToString(operation.Status) << "\nOperation: " << operation.DisplayName << '\n';
		if (operation.Failure.has_value())
		{
			error << "Problem type: " << ToString(operation.Failure->Kind) << '\n';
			error << "What failed: " << operation.Failure->Summary << '\n';
			error << "Next action: " << operation.Failure->ExpectedAction << '\n';
		}
		if (operation.ExitCode.has_value())
		{
			(operation.Failure.has_value() ? error : output) << "Exit code: " << *operation.ExitCode << '\n';
		}
		if (!operation.LogPath.empty())
		{
			(operation.Failure.has_value() ? error : output) << "Log: " << operation.LogPath.string() << '\n';
		}
		return operation.Status == OperationStatus::Succeeded ? 0 : 1;
	}

	int RunLauncherShellOperation(LauncherShellModel& model, const LauncherShellArguments& arguments, std::ostream& output, std::ostream& error)
	{
		const auto inputs = BuildLauncherShellInputs(arguments.RunOperationId, model, arguments);
		if (!inputs.has_value())
		{
			error << "Result: Failed\n";
			error << "Problem type: Planning\n";
			error << "What failed: Unknown --run operation: " << arguments.RunOperationId << "\n";
			error << "Next action: Run SparkleLauncher --help, choose a registered operation id, then retry.\n";
			return 1;
		}

		const auto writeOutput = [&output](std::string_view text)
		{
			output << text << std::flush;
		};

		LauncherOperationPlan plan = PlanLauncherOperation(arguments.RunOperationId, *inputs);
		const OperationRecord operation = RunLauncherOperationPlan(std::move(plan), writeOutput);
		return ReportLauncherShellOperationResult(operation, output, error);
	}
}
