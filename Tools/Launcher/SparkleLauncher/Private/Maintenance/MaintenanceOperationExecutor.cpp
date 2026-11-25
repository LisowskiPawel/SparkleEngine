#include "SparkleLauncher/MaintenanceOperations.h"

#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Dependencies/SourceDependencyCacheAccess.h"
#include "Maintenance/MaintenanceCleanExecution.h"
#include "Maintenance/MaintenanceCleanSteps.h"
#include "Levels/LevelSelectionCommit.h"
#include "Operations/LauncherOperationProgress.h"
#include "Workspace/LauncherProductPaths.h"

#include <optional>
#include <algorithm>
#include <system_error>

namespace SparkleLauncher
{

	static std::string MakeCleanFailureSummary(const MaintenanceCleanStep& step, const std::string& errorMessage)
	{
		std::string summary = errorMessage.empty() ? "Clean blocked by locked files or permissions." : errorMessage;
		summary += " Scope: " + step.DestructivePath.string();
		return summary;
	}

	OperationRecord RunLauncherOperationPlan(MaintenanceOperationPlan plan, const Process::ChildProcessOutputCallback& outputCallback)
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
		const auto cacheRoot = plan.Request.WorkspaceRoot / "build" / "_deps";
		const bool bTouchesSourceCache = std::any_of(
		    steps.begin(),
		    steps.end(),
		    [&cacheRoot](const MaintenanceCleanStep& step)
		    {
			    const bool bContainsCache = Paths::TryMakeRelativeUnderRoot(cacheRoot, step.DestructivePath).has_value();
			    const bool bInsideCache = Paths::TryMakeRelativeUnderRoot(step.DestructivePath, cacheRoot).has_value();
			    return bContainsCache || bInsideCache;
		    });
		std::optional<SourceDependencyCacheAccess> cacheAccess;
		if (bTouchesSourceCache)
		{
			cacheAccess.emplace(plan.Request.WorkspaceRoot, SourceDependencyCacheAccessMode::Write);
			if (!cacheAccess->IsAcquired())
			{
				SetOperationFailure(operation, OperationProblemKind::Filesystem, cacheAccess->GetErrorMessage(), "Wait for source-cache readers or writers to settle, then retry Clean.");
				MarkOperationFinished(operation, OperationStatus::Failed, std::nullopt);
				return operation;
			}
		}
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

		if (plan.Request.bCleanLevelContent)
		{
			const auto projectRoot = ResolveProjectRoot(plan.Request.WorkspaceRoot, plan.Request.ContentId);
			const bool bSelectionCommitted = ClearLevelSelection(projectRoot, operation);
			if (!bSelectionCommitted)
			{
				return operation;
			}
		}

		MarkOperationFinished(operation, OperationStatus::Succeeded, 0);
		return operation;
	}
}
