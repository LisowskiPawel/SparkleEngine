#include "SparkleLauncher/BuildWorkspaceOperations.h"

#include "Build/NativeBuildOutputReset.h"
#include "Build/BuildWorkspaceFailure.h"
#include "Operations/LauncherOperationProgress.h"
#include "ToolOutputPaths.h"
#include "SparkleLauncher/SourceDependencyState.h"

#include <algorithm>
#include <optional>
#include <sstream>
#include <system_error>
#include <vector>

namespace SparkleLauncher
{
	static std::optional<std::string> ValidateEnabledSourceDependenciesAfterConfigure(const BuildWorkspaceOperationPlan& plan)
	{
		if (!plan.Request.SourceDependencyId.empty())
		{
			return std::nullopt;
		}
		if (plan.Kind != BuildWorkspaceOperationKind::SyncCode && plan.Kind != BuildWorkspaceOperationKind::GenerateBuildFiles)
		{
			return std::nullopt;
		}

		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(plan.Request.WorkspaceRoot);
		const SourceDependencyInventoryStatus status = InspectSourceDependencyCache(workspaceOutputs.DependencyCacheRoot);
		if (status.bAllEnabledDependenciesReady)
		{
			return std::nullopt;
		}

		std::ostringstream summary;
		summary << "Enabled source dependency cache is still incomplete after configure.";
		if (!status.ReadinessMessages.empty())
		{
			summary << ' ' << status.ReadinessMessages.front();
			if (status.ReadinessMessages.size() > 1)
			{
				summary << " +" << (status.ReadinessMessages.size() - 1) << " more issue(s).";
			}
		}
		return summary.str();
	}

	static std::optional<std::string> ValidateRequestedSourceDependencyAfterConfigure(const BuildWorkspaceOperationPlan& plan)
	{
		if (plan.Request.SourceDependencyId.empty())
		{
			return std::nullopt;
		}

		const SourceDependencyEntry* dependency = FindSourceDependency(plan.Request.SourceDependencyId);
		if (dependency == nullptr)
		{
			return "Unknown source dependency: " + plan.Request.SourceDependencyId + ".";
		}

		const SparkleTools::ToolOutputPaths workspaceOutputs = SparkleTools::ResolveToolOutputPaths(plan.Request.WorkspaceRoot);
		const SourceDependencyValidation validation = InspectSourceDependencyAvailability(*dependency, workspaceOutputs.DependencyCacheRoot);
		if (validation.bReady)
		{
			return std::nullopt;
		}

		std::ostringstream summary;
		summary << dependency->Label << " source cache is incomplete after sync.";
		if (!validation.MissingRelativePaths.empty())
		{
			summary << " Missing: " << validation.MissingRelativePaths.front() << ".";
		}
		return summary.str();
	}

	static bool PrepareBuildWorkspaceStep(
	    const BuildWorkspaceOperationPlan& plan,
	    const BuildWorkspaceOperationStep& step,
	    const Process::ChildProcessOutputCallback& outputCallback,
	    OperationRecord& operation)
	{
		if (step.Id == "configure")
		{
			std::error_code errorCode;
			std::filesystem::create_directories(step.Request.WorkingDirectory, errorCode);
			if (errorCode)
			{
				SetOperationFailure(
				    operation,
				    OperationProblemKind::Filesystem,
				    "Failed to prepare the configure working directory: " + step.Request.WorkingDirectory.string() + ": " + errorCode.message(),
				    "Verify that the build directory is writable and not locked, then retry Generate Build Files.");

				MarkOperationFinished(operation, OperationStatus::Failed, std::nullopt);
				return false;
			}
			if (plan.Request.SourceDependencyId.empty() && RequiresNativeBuildOutputReset(plan.Freshness.State))
			{
				if (outputCallback)
				{
					outputCallback(
					    "The selected toolchain is incompatible with existing native outputs. Resetting generated build products while "
					    "preserving downloaded sources and cooked content.\n");
				}

				std::string cleanupError;
				if (!ResetNativeBuildOutputs(plan.Request.WorkspaceRoot, plan.Freshness.BuildDirectory, cleanupError))
				{
					SetOperationFailure(operation, OperationProblemKind::Filesystem, std::move(cleanupError), "Close processes using generated build outputs, verify path permissions, then retry.");
					MarkOperationFinished(operation, OperationStatus::Failed, std::nullopt);
					return false;
				}
			}
		}

		return true;
	}

	static bool RunBuildWorkspaceStep(
	    const BuildWorkspaceOperationPlan& plan,
	    const BuildWorkspaceOperationStep& step,
	    const Process::ChildProcessOutputCallback& outputCallback,
	    OperationRecord& operation,
	    std::stop_token cancellation)
	{
		Process::ChildProcessRequest request = step.Request;
		request.Cancellation = cancellation;
		if (!PrepareBuildWorkspaceStep(plan, step, outputCallback, operation))
		{
			return false;
		}

		AppendProcessOutputCallback(request, outputCallback);
		operation.LogPath = request.LogPath;

		const Process::ChildProcessResult result = RunLauncherProcess(request);
		if (!result.Succeeded())
		{
			if (result.bCancelled)
			{
				const auto summary = result.FailureReason.empty() ? step.DisplayName + " was canceled." : result.FailureReason;
				SetOperationFailure(operation, OperationProblemKind::Cancellation, summary, "Run the operation again when ready.");
			}
			else
			{
				const auto summary = MakeBuildWorkspaceFailureSummary(step, result);
				const auto recoveryAction = BuildWorkspaceRecoveryAction(step);
				SetProcessOperationFailure(operation, result.StartFailure, summary, recoveryAction);
			}
			MarkOperationFinished(operation, result.bCancelled ? OperationStatus::Canceled : OperationStatus::Failed, result.ExitCode);
			return false;
		}
		return true;
	}

	static bool ValidateBuildWorkspaceStep(const BuildWorkspaceOperationPlan& plan, const BuildWorkspaceOperationStep& step, OperationRecord& operation)
	{
		const std::optional<std::string> requestedDependencyFailure = ValidateRequestedSourceDependencyAfterConfigure(plan);
		if (requestedDependencyFailure)
		{
			SetOperationFailure(
			    operation,
			    OperationProblemKind::OutputValidation,
			    *requestedDependencyFailure,
			    "Correct the first dependency sync error in the named log, then retry this dependency Sync.");

			MarkOperationFinished(operation, OperationStatus::Failed, 0);
			return false;
		}

		if (step.bUpdatesBuildFilesFreshness)
		{
			const std::optional<std::string> enabledDependencyFailure = ValidateEnabledSourceDependenciesAfterConfigure(plan);
			if (enabledDependencyFailure)
			{
				SetOperationFailure(
				    operation,
				    OperationProblemKind::OutputValidation,
				    *enabledDependencyFailure,
				    "Correct the first dependency configure error in the named log, then retry Sync Code.");

				MarkOperationFinished(operation, OperationStatus::Failed, 0);
				return false;
			}

			std::string errorMessage;
			if (!UpdateBuildFilesFreshnessStamp(plan.Request.WorkspaceRoot, plan.Toolchain, errorMessage))
			{
				SetOperationFailure(operation, OperationProblemKind::Filesystem, std::move(errorMessage), "Verify write permission for the launcher state directory, then retry Generate Build Files.");
				MarkOperationFinished(operation, OperationStatus::Failed, std::nullopt);
				return false;
			}
		}
		return true;
	}

	OperationRecord RunLauncherOperationPlan(BuildWorkspaceOperationPlan plan, const Process::ChildProcessOutputCallback& outputCallback, std::stop_token cancellation)
	{
		OperationRecord operation = plan.Operation;
		MarkOperationStarted(operation, operation.LogPath);
		if (!plan.bCanRun)
		{
			SetOperationFailure(
			    operation,
			    OperationProblemKind::Prerequisite,
			    plan.ReadinessMessages.empty() ? "Operation is not ready to run." : plan.ReadinessMessages.back(),
			    "Open Sync, resolve the reported prerequisite, then retry this workflow.");

			MarkOperationFinished(operation, OperationStatus::Failed, std::nullopt);
			return operation;
		}

		for (std::size_t stepIndex = 0; stepIndex < plan.Steps.size(); ++stepIndex)
		{
			const BuildWorkspaceOperationStep& step = plan.Steps[stepIndex];
			ReportOperationProgress(outputCallback, step.DisplayName, stepIndex, plan.Steps.size());
			const bool bStepExecuted = RunBuildWorkspaceStep(plan, step, outputCallback, operation, cancellation);
			const bool bStepValidated = bStepExecuted && ValidateBuildWorkspaceStep(plan, step, operation);

			if (!bStepValidated)
			{
				return operation;
			}
			ReportOperationProgress(outputCallback, step.DisplayName, stepIndex + 1, plan.Steps.size());
		}
		MarkOperationFinished(operation, OperationStatus::Succeeded, 0);
		return operation;
	}
}
