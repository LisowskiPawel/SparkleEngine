#pragma once

#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "SparkleLauncher/CookOperations.h"
#include "SparkleLauncher/LevelOperations.h"
#include "SparkleLauncher/LevelRunOperations.h"
#include "SparkleLauncher/MaintenanceOperations.h"
#include "SparkleLauncher/OperationModel.h"
#include "SparkleLauncher/LauncherProcess.h"

#include <string_view>
#include <optional>
#include <stop_token>
#include <variant>

namespace SparkleLauncher
{
	template <typename Visitor> void VisitLauncherOperationDefinitions(Visitor&& visit)
	{
		const auto visitDefinitions = [&visit](const auto& definitions)
		{
			for (const auto& definition : definitions)
			{
				visit(definition);
			}
		};

		visitDefinitions(GetBuildWorkspaceOperationDefinitions());
		visitDefinitions(GetCookOperationDefinitions());
		visitDefinitions(GetLevelOperationDefinitions());
		visitDefinitions(GetLevelRunOperationDefinitions());
		visitDefinitions(GetMaintenanceOperationDefinitions());
	}

	using LauncherOperationInputs = std::variant<BuildWorkspaceOperationRequest, LevelOperationRequest, LevelRunOperationRequest, CookOperationRequest, MaintenanceOperationRequest>;
	using LauncherOperationPlan = std::variant<BuildWorkspaceOperationPlan, LevelOperationPlan, LevelRunOperationPlan, CookOperationPlan, MaintenanceOperationPlan>;

	std::optional<std::string_view> FindLauncherOperationDisplayName(std::string_view operationId);

	OperationRecord ExecuteLauncherOperation(
	    std::string_view operationId,
	    const LauncherOperationInputs& inputs,
	    std::stop_token cancellation,
	    const Process::ChildProcessOutputCallback& outputCallback);

	LauncherOperationPlan PlanLauncherOperation(std::string_view operationId, const LauncherOperationInputs& inputs);
	OperationRecord RunLauncherOperationPlan(LauncherOperationPlan plan, const Process::ChildProcessOutputCallback& outputCallback, std::stop_token cancellation = {});
}
