#include "Operations/LauncherOperationExecution.h"

#include <type_traits>
#include <utility>

namespace SparkleLauncher
{
	LauncherOperationPlan PlanLauncherOperation(std::string_view operationId, const LauncherOperationInputs& inputs)
	{
		return std::visit(
		    [operationId](const auto& request) -> LauncherOperationPlan
		    {
			    using Request = std::remove_cvref_t<decltype(request)>;
			    if constexpr (std::is_same_v<Request, BuildWorkspaceOperationRequest>)
			    {
				    return PlanBuildWorkspaceOperation(operationId, request);
			    }
			    else if constexpr (std::is_same_v<Request, LevelOperationRequest>)
			    {
				    return PlanLevelOperation(operationId, request);
			    }
			    else if constexpr (std::is_same_v<Request, LevelRunOperationRequest>)
			    {
				    return PlanLevelRunOperation(operationId, request);
			    }
			    else if constexpr (std::is_same_v<Request, CookOperationRequest>)
			    {
				    return PlanCookOperation(operationId, request);
			    }
			    else
			    {
				    return PlanMaintenanceOperation(operationId, request);
			    }
		    },
		    inputs);
	}

	OperationRecord RunLauncherOperationPlan(LauncherOperationPlan plan, IProcessRunner& processRunner, const Process::ChildProcessOutputCallback& outputCallback)
	{
		return std::visit(
		    [&processRunner, &outputCallback](auto&& typedPlan) -> OperationRecord
		    {
			    using Plan = std::remove_cvref_t<decltype(typedPlan)>;
			    if constexpr (std::is_same_v<Plan, BuildWorkspaceOperationPlan>)
			    {
				    return RunBuildWorkspaceOperationPlan(std::move(typedPlan), processRunner, outputCallback);
			    }
			    else if constexpr (std::is_same_v<Plan, LevelOperationPlan>)
			    {
				    return RunLevelOperationPlan(std::move(typedPlan), processRunner, outputCallback);
			    }
			    else if constexpr (std::is_same_v<Plan, LevelRunOperationPlan>)
			    {
				    return RunLevelRunOperationPlan(std::move(typedPlan), processRunner, outputCallback);
			    }
			    else if constexpr (std::is_same_v<Plan, CookOperationPlan>)
			    {
				    return RunCookOperationPlan(std::move(typedPlan), processRunner, outputCallback);
			    }
			    else
			    {
				    return RunMaintenanceOperationPlan(std::move(typedPlan), outputCallback);
			    }
		    },
		    std::move(plan));
	}
}
