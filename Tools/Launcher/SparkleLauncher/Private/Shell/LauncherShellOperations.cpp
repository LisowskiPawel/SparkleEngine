#include "LauncherShellOperations.h"

#include "LauncherShellArguments.h"
#include "LauncherShellModel.h"

#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "SparkleLauncher/CookOperations.h"
#include "SparkleLauncher/LevelOperations.h"
#include "SparkleLauncher/LevelRunOperations.h"
#include "SparkleLauncher/MaintenanceOperations.h"
#include "SparkleLauncher/OperationModel.h"
#include "SparkleLauncher/ProcessRunner.h"

#include <ostream>
#include <string>
#include <string_view>

namespace SparkleLauncher
{
	BuildWorkspaceOperationRequest BuildWorkspaceShellRequest(const LauncherShellModel& model)
	{
		BuildWorkspaceOperationRequest request;
		request.RepositoryRoot = model.Repository.RootPath;
		request.ContentId = model.ContentId;
		request.EditorProfile = model.EditorProfile;
		request.GameProfile = model.GameProfile;
		request.PreferredIde = model.WorkspaceIdePreference;
		request.Compiler = model.WorkspaceCompilerPreference;
		request.ForceConfigure = false;
		return request;
	}

	CookOperationRequest BuildCookShellRequest(const LauncherShellModel& model, const LauncherShellArguments& arguments)
	{
		CookOperationRequest request;
		request.RepositoryRoot = model.Repository.RootPath;
		request.ContentId = model.ContentId;
		request.GameProfile = model.GameProfile;
		request.Mode = arguments.RequestedCookMode;
		request.ForceRecookConfirmed = arguments.ForceRecookConfirmed;
		request.SelectedScopes = {CookWorkspaceScope::Shaders, CookWorkspaceScope::Textures, CookWorkspaceScope::SceneAssets};
		return request;
	}

	LevelOperationRequest BuildLevelShellRequest(const LauncherShellModel& model, const LauncherShellArguments& arguments)
	{
		LevelOperationRequest request;
		request.RepositoryRoot = model.Repository.RootPath;
		request.ContentId = model.ContentId;
		if (!arguments.LevelId.empty())
		{
			request.RequestedLevelIds = {arguments.LevelId};
		}
		return request;
	}

	LevelRunOperationRequest BuildLevelRunShellRequest(const LauncherShellModel& model, const LauncherShellArguments& arguments)
	{
		LevelRunOperationRequest request;
		request.RepositoryRoot = model.Repository.RootPath;
		request.ContentId = model.ContentId;
		request.LevelId = arguments.LevelId;
		request.RunMode = arguments.Product;
		request.ProductProfile = arguments.Product == LevelRunMode::Editor ? model.EditorProfile : model.GameProfile;
		request.GraphicsApi = arguments.GraphicsApi;
		return request;
	}

	MaintenanceOperationRequest BuildMaintenanceShellRequest(const LauncherShellModel& model, const LauncherShellArguments& arguments)
	{
		MaintenanceOperationRequest request;
		request.RepositoryRoot = model.Repository.RootPath;
		request.ContentId = model.ContentId;
		request.EditorProfile = model.EditorProfile;
		request.RequestedCleanScope = arguments.RequestedCleanScope;
		request.DestructiveActionConfirmed = arguments.CleanConfirmed;
		return request;
	}

	void AppendBuildPlanDryRun(LauncherShellModel& model, const BuildWorkspaceOperationPlan& plan)
	{
		model.JobOutput.push_back(plan.Operation.DisplayName + " [" + std::string(plan.CanRun ? "Ready" : "Blocked") + "]");
		model.JobOutput.push_back("Editor profile: " + model.EditorProfile);
		model.JobOutput.push_back("Game profile: " + model.GameProfile);
		model.JobOutput.push_back("Workspace IDE: " + DisplayName(model.WorkspaceIdePreference));
		model.JobOutput.push_back("Workspace compiler: " + DisplayName(model.WorkspaceCompilerPreference));
		model.JobOutput.push_back("Latest log: " + plan.Operation.LogPath.string());
		for (const std::string& message : plan.ReadinessMessages)
		{
			model.JobOutput.push_back("Readiness: " + message);
		}
		for (const std::string& effect : plan.PlannedEffects)
		{
			model.JobOutput.push_back("Effect: " + effect);
		}
		model.JobOutput.push_back(plan.Operation.DryRunText);
		RecordLauncherShellActivity(model, plan.Operation.DisplayName + " dry-run planned");
	}

	void AppendCookPlanDryRun(LauncherShellModel& model, const CookOperationPlan& plan)
	{
		model.JobOutput.push_back(plan.Operation.DisplayName + " [" + std::string(plan.CanRun ? "Ready" : "Blocked") + "]");
		model.JobOutput.push_back("Game profile: " + model.GameProfile);
		model.JobOutput.push_back("Cook mode: " + ToString(plan.Request.Mode));
		model.JobOutput.push_back("Cooked output: " + plan.CookedOutputDirectory.string());
		model.JobOutput.push_back("Latest log: " + plan.Operation.LogPath.string());
		if (plan.Operation.RequiresConfirmation)
		{
			model.JobOutput.push_back("Confirmation required: " + ToString(plan.Operation.DestructiveScope));
		}
		for (const std::string& message : plan.ReadinessMessages)
		{
			model.JobOutput.push_back("Readiness: " + message);
		}
		for (const std::string& effect : plan.PlannedEffects)
		{
			model.JobOutput.push_back("Effect: " + effect);
		}
		model.JobOutput.push_back(plan.Operation.DryRunText);
		RecordLauncherShellActivity(model, plan.Operation.DisplayName + " dry-run planned");
	}

	void AppendLevelPlanDryRun(LauncherShellModel& model, const LevelOperationPlan& plan)
	{
		model.JobOutput.push_back(plan.Operation.DisplayName + " [" + std::string(plan.CanRun ? "Ready" : "Blocked") + "]");
		model.JobOutput.push_back("Content: " + model.ContentId);
		for (const std::string& levelId : plan.Request.RequestedLevelIds)
		{
			model.JobOutput.push_back("Requested level: " + levelId);
		}
		model.JobOutput.push_back("Latest log: " + plan.Operation.LogPath.string());
		for (const std::string& message : plan.ReadinessMessages)
		{
			model.JobOutput.push_back("Readiness: " + message);
		}
		for (const std::string& effect : plan.PlannedEffects)
		{
			model.JobOutput.push_back("Effect: " + effect);
		}
		model.JobOutput.push_back(plan.Operation.DryRunText);
		RecordLauncherShellActivity(model, plan.Operation.DisplayName + " dry-run planned");
	}

	void AppendLevelRunPlanDryRun(LauncherShellModel& model, const LevelRunOperationPlan& plan)
	{
		model.JobOutput.push_back(plan.Operation.DisplayName + " [" + std::string(plan.CanRun ? "Ready" : "Blocked") + "]");
		for (const std::string& message : plan.ReadinessMessages)
		{
			model.JobOutput.push_back("Readiness: " + message);
		}
		for (const std::string& effect : plan.PlannedEffects)
		{
			model.JobOutput.push_back("Effect: " + effect);
		}
		model.JobOutput.push_back(plan.Operation.DryRunText);
		RecordLauncherShellActivity(model, plan.Operation.DisplayName + " dry-run planned");
	}

	void AppendMaintenancePlanDryRun(LauncherShellModel& model, const MaintenanceOperationPlan& plan)
	{
		model.JobOutput.push_back(plan.Operation.DisplayName + " [" + std::string(plan.CanRun ? "Ready" : "Blocked") + "]");
		model.JobOutput.push_back("Editor profile: " + model.EditorProfile);
		model.JobOutput.push_back("Clean scope: " + ToString(plan.Request.RequestedCleanScope));
		model.JobOutput.push_back("Latest log: " + plan.Operation.LogPath.string());
		if (plan.Operation.RequiresConfirmation)
		{
			model.JobOutput.push_back("Confirmation required: " + ToString(plan.Operation.DestructiveScope));
		}
		for (const MaintenanceCleanTarget& target : plan.CleanTargets)
		{
			model.JobOutput.push_back("Clean target: " + target.Path.string() + " | " + target.Detail);
		}
		for (const std::string& message : plan.ReadinessMessages)
		{
			model.JobOutput.push_back("Readiness: " + message);
		}
		for (const std::string& effect : plan.PlannedEffects)
		{
			model.JobOutput.push_back("Effect: " + effect);
		}
		model.JobOutput.push_back(plan.Operation.DryRunText);
		RecordLauncherShellActivity(model, plan.Operation.DisplayName + " dry-run planned");
	}

	void ApplyLauncherShellDryRun(LauncherShellModel& model, const LauncherShellArguments& arguments)
	{
		const LauncherShellOperationRow* operation = FindLauncherShellOperation(model, arguments.DryRunOperationId);
		if (operation == nullptr)
		{
			model.JobOutput.push_back("Unknown dry-run operation: " + arguments.DryRunOperationId);
			RecordLauncherShellActivity(model, "Dry-run failed: unknown operation " + arguments.DryRunOperationId);
			return;
		}

		if (FindBuildWorkspaceOperationDefinition(operation->Id).has_value())
		{
			AppendBuildPlanDryRun(model, PlanBuildWorkspaceOperation(operation->Id, BuildWorkspaceShellRequest(model)));
			return;
		}

		if (FindCookOperationDefinition(operation->Id).has_value())
		{
			AppendCookPlanDryRun(model, PlanCookOperation(operation->Id, BuildCookShellRequest(model, arguments)));
			return;
		}

		if (FindLevelOperationDefinition(operation->Id).has_value())
		{
			AppendLevelPlanDryRun(model, PlanLevelOperation(operation->Id, BuildLevelShellRequest(model, arguments)));
			return;
		}

		if (FindLevelRunOperationDefinition(operation->Id).has_value())
		{
			AppendLevelRunPlanDryRun(model, PlanLevelRunOperation(operation->Id, BuildLevelRunShellRequest(model, arguments)));
			return;
		}

		if (FindMaintenanceOperationDefinition(operation->Id).has_value())
		{
			AppendMaintenancePlanDryRun(model, PlanMaintenanceOperation(operation->Id, BuildMaintenanceShellRequest(model, arguments)));
			return;
		}

		OperationRecord record = MakeOperationRecord(operation->Id, operation->DisplayName);
		record.DryRunText = "Dry-run only: " + operation->NextEffect;
		model.JobOutput.push_back(record.DisplayName + " [Preview]");
		model.JobOutput.push_back(record.DryRunText);
		RecordLauncherShellActivity(model, record.DisplayName + " dry-run");
	}

	int ReportLauncherShellOperationResult(const OperationRecord& operation, std::ostream& output, std::ostream& error)
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
		NativeProcessRunner processRunner;
		if (FindBuildWorkspaceOperationDefinition(arguments.RunOperationId).has_value())
		{
			const OperationRecord operation = RunBuildWorkspaceOperationPlan(
			    PlanBuildWorkspaceOperation(arguments.RunOperationId, BuildWorkspaceShellRequest(model)),
			    processRunner,
			    [&output](std::string_view text) { output << text; });

			return ReportLauncherShellOperationResult(operation, output, error);
		}

		if (FindCookOperationDefinition(arguments.RunOperationId).has_value())
		{
			const OperationRecord operation = RunCookOperationPlan(
			    PlanCookOperation(arguments.RunOperationId, BuildCookShellRequest(model, arguments)),
			    processRunner,
			    [&output](std::string_view text) { output << text; });

			return ReportLauncherShellOperationResult(operation, output, error);
		}

		if (FindLevelOperationDefinition(arguments.RunOperationId).has_value())
		{
			const OperationRecord operation = RunLevelOperationPlan(
			    PlanLevelOperation(arguments.RunOperationId, BuildLevelShellRequest(model, arguments)),
			    processRunner,
			    [&output](std::string_view text) { output << text; });

			return ReportLauncherShellOperationResult(operation, output, error);
		}

		if (FindLevelRunOperationDefinition(arguments.RunOperationId).has_value())
		{
			const OperationRecord operation = RunLevelRunOperationPlan(
			    PlanLevelRunOperation(arguments.RunOperationId, BuildLevelRunShellRequest(model, arguments)),
			    processRunner,
			    [&output](std::string_view text) { output << text; });

			return ReportLauncherShellOperationResult(operation, output, error);
		}

		if (FindMaintenanceOperationDefinition(arguments.RunOperationId).has_value())
		{
			const OperationRecord operation = RunMaintenanceOperationPlan(PlanMaintenanceOperation(arguments.RunOperationId, BuildMaintenanceShellRequest(model, arguments)), processRunner);
			return ReportLauncherShellOperationResult(operation, output, error);
		}

		error << "Result: Failed\n";
		error << "Problem type: Planning\n";
		error << "What failed: Unknown --run operation: " << arguments.RunOperationId << "\n";
		error << "Next action: Run SparkleLauncher --help, choose a registered operation id, then retry.\n";
		return 1;
	}
}
