#pragma once

#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "SparkleLauncher/CookOperations.h"
#include "SparkleLauncher/LevelOperations.h"
#include "SparkleLauncher/LevelRunOperations.h"
#include "SparkleLauncher/MaintenanceOperations.h"
#include "SparkleLauncher/OperationModel.h"
#include "SparkleLauncher/ProcessRunner.h"

#include <string_view>
#include <stop_token>
#include <variant>

namespace SparkleLauncher
{
	using LauncherOperationInputs = std::variant<BuildWorkspaceOperationRequest, LevelOperationRequest, LevelRunOperationRequest, CookOperationRequest, MaintenanceOperationRequest>;
	using LauncherOperationPlan = std::variant<BuildWorkspaceOperationPlan, LevelOperationPlan, LevelRunOperationPlan, CookOperationPlan, MaintenanceOperationPlan>;

	OperationRecord ExecuteLauncherOperation(
	    std::string_view operationId,
	    const LauncherOperationInputs& inputs,
	    IProcessRunner& processRunner,
	    std::stop_token cancellation,
	    const Process::ChildProcessOutputCallback& outputCallback);

	LauncherOperationPlan PlanLauncherOperation(std::string_view operationId, const LauncherOperationInputs& inputs);
	OperationRecord RunLauncherOperationPlan(LauncherOperationPlan plan, IProcessRunner& processRunner, const Process::ChildProcessOutputCallback& outputCallback, std::stop_token cancellation = {});
}
