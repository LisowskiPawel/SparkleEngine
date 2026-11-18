#include "SparkleLauncher/MaintenanceOperations.h"

#include "Core/Public/Diagnostics/Logger.h"
#include "Maintenance/MaintenanceCleanExecution.h"
#include "Maintenance/MaintenanceCleanSteps.h"
#include "Levels/LevelSelectionCommit.h"
#include "Operations/LauncherOperationProgress.h"
#include "Workspace/LauncherProductPaths.h"

#include <optional>
#include <system_error>

namespace SparkleLauncher
{

	static std::string MakeCleanFailureSummary(const MaintenanceCleanStep& step, const std::string& errorMessage)
	{
		std::string summary = errorMessage.empty() ? "Clean blocked by locked files or permissions." : errorMessage;
		summary += " Scope: " + step.DestructivePath.string();
		return summary;
	}

	OperationRecord RunMaintenanceOperationPlan(MaintenanceOperationPlan plan, const Process::ChildProcessOutputCallback& outputCallback)
	{
		OperationRecord operation = plan.Operation;
		MarkOperationStarted(operation, operation.LogPath);

		if (!plan.bCanRun)
		{
			SetOperationFailure(
			    operation,
			    OperationProblemKind::Prerequisite,
			    plan.ReadinessMessages.empty() ? "Clean operation is not ready to run." : plan.ReadinessMessages.front(),
			    "Adjust the selected clean scope or resolve the reported path prerequisite, then retry.");

			MarkOperationFinished(operation, OperationStatus::Failed, std::nullopt);
			return operation;
		}

		std::vector<std::filesystem::path> preservedPaths = plan.Request.PreservedPaths;
		{
			const std::optional<std::filesystem::path> activeLogFilePath = Logging::GetActiveLogFilePath();

			if (activeLogFilePath.has_value())
			{
				preservedPaths.push_back(*activeLogFilePath);
			}
		}
		const std::vector<MaintenanceCleanStep> steps = BuildMaintenanceCleanSteps(plan);
		for (std::size_t stepIndex = 0; stepIndex < steps.size(); ++stepIndex)
		{
			const MaintenanceCleanStep& step = steps[stepIndex];
			ReportOperationProgress(outputCallback, step.DisplayName, stepIndex, steps.size());
			std::string errorMessage;
			if (!RunCleanStep(step, preservedPaths, errorMessage))
			{
				SetOperationFailure(
				    operation,
				    OperationProblemKind::Filesystem,
				    MakeCleanFailureSummary(step, errorMessage),
				    "Close processes using this scope, verify path permissions, then retry Clean.");

				MarkOperationFinished(operation, OperationStatus::Failed, std::nullopt);
				return operation;
			}
			ReportOperationProgress(outputCallback, step.DisplayName, stepIndex + 1, steps.size());
		}

		const bool bSelectionCommitted = !plan.Request.bCleanLevelContent || ClearLevelSelection(ResolveProjectRoot(plan.Request.WorkspaceRoot, plan.Request.ContentId), operation);
		if (!bSelectionCommitted)
		{
			return operation;
		}

		MarkOperationFinished(operation, OperationStatus::Succeeded, 0);
		return operation;
	}
}
