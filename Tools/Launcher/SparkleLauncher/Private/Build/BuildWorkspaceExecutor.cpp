#include "SparkleLauncher/BuildWorkspaceOperations.h"

#include "NativeBuildOutputReset.h"
#include "BuildWorkspaceProcessRequests.h"
#include "BuildWorkspaceFailure.h"
#include "LauncherOperationProgress.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Paths/WorkspaceOutputPaths.h"
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

		const Filesystem::WorkspaceOutputPaths workspaceOutputs = Filesystem::ResolveWorkspaceOutputPaths(plan.RepositoryRoot);
		const SourceDependencyInventoryStatus status = InspectSourceDependencyCache(workspaceOutputs.DependencyCacheRoot);
		if (status.AllEnabledDependenciesReady)
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

		const Filesystem::WorkspaceOutputPaths workspaceOutputs = Filesystem::ResolveWorkspaceOutputPaths(plan.RepositoryRoot);
		const SourceDependencyValidation validation = ValidateSourceDependency(*dependency, workspaceOutputs.DependencyCacheRoot);
		if (validation.Ready)
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

	static std::optional<std::string> ValidateRequestedHostToolAfterInstall(const BuildWorkspaceOperationPlan& plan)
	{
		if (plan.Kind != BuildWorkspaceOperationKind::InstallHostTool)
		{
			return std::nullopt;
		}

		const BuildToolchainStatus refreshedToolchain = DetectBuildToolchain(plan.RepositoryRoot, plan.Request.PreferredIde, plan.Request.Compiler);

		const auto installedTool = std::find_if(
		    refreshedToolchain.Items.begin(),
		    refreshedToolchain.Items.end(),
		    [&plan](const ToolchainItemStatus& item) { return item.Id == plan.Request.HostToolId; });

		if (installedTool != refreshedToolchain.Items.end() && installedTool->State == ToolchainItemState::Found)
		{
			return std::nullopt;
		}

		const std::string displayName = installedTool == refreshedToolchain.Items.end() ? plan.Request.HostToolId : installedTool->DisplayName;
		return "The installer completed, but " + displayName + " is still unavailable. The required compiler or toolchain component was not installed.";
	}

	static bool MatchesPlannedStep(const BuildWorkspaceOperationStep& planned, const BuildWorkspaceProcessStep& executable)
	{
		return planned.Id == executable.Id && planned.DisplayName == executable.DisplayName
		    && planned.DisplayCommandLine == BuildDisplayCommandLine(executable.Request.ExecutablePath, executable.Request.Arguments) && planned.LogPath == executable.Request.LogPath
		    && planned.UpdatesBuildFilesFreshness == executable.UpdatesBuildFilesFreshness;
	}

	bool BuildWorkspaceExecutionPlanMatches(const BuildWorkspaceOperationPlan& plan, const std::vector<BuildWorkspaceProcessStep>& processSteps)
	{
		return plan.Steps.size() == processSteps.size()
		    && std::equal(
		        plan.Steps.begin(),
		        plan.Steps.end(),
		        processSteps.begin(),
		        [](const BuildWorkspaceOperationStep& planned, const BuildWorkspaceProcessStep& executable) { return MatchesPlannedStep(planned, executable); });
	}

	static bool PrepareBuildWorkspaceExecution(const BuildWorkspaceOperationPlan& plan, OperationRecord& operation, std::vector<BuildWorkspaceProcessStep>& processSteps)
	{
		if (!plan.CanRun)
		{
			SetOperationFailure(
			    operation,
			    OperationProblemKind::Prerequisite,
			    plan.ReadinessMessages.empty() ? "Operation is not ready to run." : plan.ReadinessMessages.back(),
			    "Open Sync, resolve the reported prerequisite, then retry this workflow.");

			MarkOperationFinished(operation, OperationStatus::Failed, std::nullopt);
			return false;
		}

		try
		{
			processSteps = BuildProcessStepsForPlan(plan);
		}
		catch (const Diagnostics::Error& error)
		{
			SetOperationFailure(operation, OperationProblemKind::Planning, std::string("Operation planning failed: ") + error.what(), "Refresh the workflow, review its preview, then retry.");
			MarkOperationFinished(operation, OperationStatus::Failed, std::nullopt);
			return false;
		}
		if (!BuildWorkspaceExecutionPlanMatches(plan, processSteps))
		{
			SetOperationFailure(operation, OperationProblemKind::Planning, "Operation inputs changed after planning.", "Refresh the workflow, review its updated preview, then retry.");
			MarkOperationFinished(operation, OperationStatus::Failed, std::nullopt);
			return false;
		}

		return true;
	}

	static bool PrepareBuildWorkspaceStep(
	    const BuildWorkspaceOperationPlan& plan,
	    const BuildWorkspaceProcessStep& step,
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
				if (!ResetNativeBuildOutputs(plan.RepositoryRoot, plan.Freshness.BuildDirectory, cleanupError))
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
	    const BuildWorkspaceProcessStep& step,
	    IProcessRunner& processRunner,
	    const Process::ChildProcessOutputCallback& outputCallback,
	    OperationRecord& operation)
	{
		Process::ChildProcessRequest request = step.Request;
		if (!PrepareBuildWorkspaceStep(plan, step, outputCallback, operation))
		{
			return false;
		}

		AppendProcessOutputCallback(request, outputCallback);

		const Process::ChildProcessResult result = processRunner.Run(request);
		if (!result.Succeeded())
		{
			if (result.Cancelled)
			{
				SetOperationFailure(operation, OperationProblemKind::Cancellation, step.DisplayName + " was canceled.", "Run the operation again when ready.");
			}
			else
			{
				SetProcessOperationFailure(operation, result.StartFailure, MakeBuildWorkspaceFailureSummary(step, result), BuildWorkspaceRecoveryAction(step));
			}
			MarkOperationFinished(operation, result.Cancelled ? OperationStatus::Canceled : OperationStatus::Failed, result.ExitCode);
			return false;
		}
		return true;
	}

	static bool ValidateBuildWorkspaceStep(const BuildWorkspaceOperationPlan& plan, const BuildWorkspaceProcessStep& step, OperationRecord& operation)
	{
		if (const std::optional<std::string> hostToolValidationFailure = ValidateRequestedHostToolAfterInstall(plan))
		{
			SetOperationFailure(
			    operation,
			    OperationProblemKind::OutputValidation,
			    *hostToolValidationFailure,
			    "Review the installer result, ensure the requested compiler and Visual Studio toolset are selected, then retry "
			    "Install.");

			MarkOperationFinished(operation, OperationStatus::Failed, 0);
			return false;
		}

		if (const std::optional<std::string> dependencyValidationFailure = ValidateRequestedSourceDependencyAfterConfigure(plan))
		{
			SetOperationFailure(
			    operation,
			    OperationProblemKind::OutputValidation,
			    *dependencyValidationFailure,
			    "Correct the first dependency sync error in the named log, then retry this dependency Sync.");

			MarkOperationFinished(operation, OperationStatus::Failed, 0);
			return false;
		}

		if (step.UpdatesBuildFilesFreshness)
		{
			if (const std::optional<std::string> dependencyValidationFailure = ValidateEnabledSourceDependenciesAfterConfigure(plan))
			{
				SetOperationFailure(
				    operation,
				    OperationProblemKind::OutputValidation,
				    *dependencyValidationFailure,
				    "Correct the first dependency configure error in the named log, then retry Sync Code.");

				MarkOperationFinished(operation, OperationStatus::Failed, 0);
				return false;
			}

			std::string errorMessage;
			if (!UpdateBuildFilesFreshnessStamp(plan.RepositoryRoot, plan.Toolchain, errorMessage))
			{
				SetOperationFailure(operation, OperationProblemKind::Filesystem, std::move(errorMessage), "Verify write permission for the launcher state directory, then retry Generate Build Files.");
				MarkOperationFinished(operation, OperationStatus::Failed, std::nullopt);
				return false;
			}
		}
		return true;
	}

	OperationRecord RunBuildWorkspaceOperationPlan(BuildWorkspaceOperationPlan plan, IProcessRunner& processRunner, const Process::ChildProcessOutputCallback& outputCallback)
	{
		OperationRecord operation = plan.Operation;
		MarkOperationStarted(operation, operation.LogPath);
		std::vector<BuildWorkspaceProcessStep> processSteps;
		if (!PrepareBuildWorkspaceExecution(plan, operation, processSteps))
		{
			return operation;
		}

		for (std::size_t stepIndex = 0; stepIndex < processSteps.size(); ++stepIndex)
		{
			const BuildWorkspaceProcessStep& step = processSteps[stepIndex];
			ReportOperationProgress(outputCallback, step.DisplayName, stepIndex, processSteps.size());
			if (!RunBuildWorkspaceStep(plan, step, processRunner, outputCallback, operation) || !ValidateBuildWorkspaceStep(plan, step, operation))
			{
				return operation;
			}
			ReportOperationProgress(outputCallback, step.DisplayName, stepIndex + 1, processSteps.size());
		}
		MarkOperationFinished(operation, OperationStatus::Succeeded, 0);
		return operation;
	}
}
