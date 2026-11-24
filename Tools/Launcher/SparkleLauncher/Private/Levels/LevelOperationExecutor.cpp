#include "SparkleLauncher/LevelOperations.h"

#include "Levels/LevelSelectionCommit.h"
#include "Workspace/LauncherProductPaths.h"
#include "Operations/LauncherOperationProgress.h"

namespace SparkleLauncher
{
	static bool AcquireLevelPacks(const std::vector<LevelOperationStep>& steps, const Process::ChildProcessOutputCallback& outputCallback, OperationRecord& operation, std::stop_token cancellation)
	{
		for (std::size_t stepIndex = 0; stepIndex < steps.size(); ++stepIndex)
		{
			const LevelOperationStep& step = steps[stepIndex];
			ReportOperationProgress(outputCallback, step.DisplayName, stepIndex, steps.size());
			Process::ChildProcessRequest request = step.Request;
			request.Cancellation = cancellation;
			AppendProcessOutputCallback(request, outputCallback);
			operation.LogPath = request.LogPath;

			const Process::ChildProcessResult result = RunLauncherProcess(request);
			if (!result.Succeeded())
			{
				if (result.bCancelled)
				{
					const auto summary = result.FailureReason.empty() ? step.DisplayName + " was canceled." : result.FailureReason;
					SetOperationFailure(operation, OperationProblemKind::Cancellation, summary, "Run Sync again when ready.");
				}
				else
				{
					const std::string summary = result.FailureReason.empty() ? "Asset pack acquisition failed." : result.FailureReason;
					SetProcessOperationFailure(operation, result.StartFailure, summary, "Correct the first asset acquisition error in the named log, then retry Sync.");
				}
				MarkOperationFinished(operation, result.bCancelled ? OperationStatus::Canceled : OperationStatus::Failed, result.ExitCode);
				return false;
			}
			ReportOperationProgress(outputCallback, step.DisplayName, stepIndex + 1, steps.size());
		}

		return true;
	}

	OperationRecord RunLevelOperationPlan(LevelOperationPlan plan, const Process::ChildProcessOutputCallback& outputCallback, std::stop_token cancellation)
	{
		OperationRecord operation = plan.Operation;
		MarkOperationStarted(operation, operation.LogPath);
		if (!plan.bCanRun)
		{
			SetOperationFailure(
			    operation,
			    OperationProblemKind::Prerequisite,
			    plan.ReadinessMessages.empty() ? "Level operation is not ready to run." : plan.ReadinessMessages.back(),
			    "Resolve the reported source or content prerequisite, then retry Sync.");

			MarkOperationFinished(operation, OperationStatus::Failed, std::nullopt);
			return operation;
		}

		if (!AcquireLevelPacks(plan.Steps, outputCallback, operation, cancellation))
		{
			return operation;
		}

		if (plan.Request.bSelectAvailableLevels)
		{
			const auto projectRoot = ResolveProjectRoot(plan.Request.WorkspaceRoot, plan.Request.ContentId);
			const bool bSelectionCommitted = CommitLevelSelection(projectRoot, plan.Request.RequestedLevelIds, true, operation);
			if (!bSelectionCommitted)
			{
				return operation;
			}
		}

		MarkOperationFinished(operation, OperationStatus::Succeeded, 0);
		return operation;
	}
}
