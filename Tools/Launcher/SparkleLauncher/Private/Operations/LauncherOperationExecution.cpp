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

	OperationRecord RunLauncherOperationPlan(LauncherOperationPlan plan, IProcessRunner& processRunner, const Process::ChildProcessOutputCallback& outputCallback, std::stop_token cancellation)
	{
		return std::visit(
		    [&processRunner, &outputCallback, cancellation](auto&& typedPlan) -> OperationRecord
		    {
			    using Plan = std::remove_cvref_t<decltype(typedPlan)>;
			    if constexpr (std::is_same_v<Plan, BuildWorkspaceOperationPlan>)
			    {
				    return RunBuildWorkspaceOperationPlan(std::move(typedPlan), processRunner, outputCallback, cancellation);
			    }
			    else if constexpr (std::is_same_v<Plan, LevelOperationPlan>)
			    {
				    return RunLevelOperationPlan(std::move(typedPlan), processRunner, outputCallback, cancellation);
			    }
			    else if constexpr (std::is_same_v<Plan, LevelRunOperationPlan>)
			    {
				    return RunLevelRunOperationPlan(std::move(typedPlan), processRunner, outputCallback, cancellation);
			    }
			    else if constexpr (std::is_same_v<Plan, CookOperationPlan>)
			    {
				    return RunCookOperationPlan(std::move(typedPlan), processRunner, outputCallback, cancellation);
			    }
			    else
			    {
				    return RunMaintenanceOperationPlan(std::move(typedPlan), outputCallback);
			    }
		    },
		    std::move(plan));
	}

	OperationRecord ExecuteLauncherOperation(
	    std::string_view operationId,
	    const LauncherOperationInputs& inputs,
	    IProcessRunner& processRunner,
	    std::stop_token cancellation,
	    const Process::ChildProcessOutputCallback& outputCallback)
	{
		LauncherOperationPlan plan = PlanLauncherOperation(operationId, inputs);

		auto blockedOperation = std::visit(
		    [](auto& typedPlan) -> std::optional<OperationRecord>
		    {
			    if (typedPlan.bCanRun)
			    {
				    return std::nullopt;
			    }

			    OperationRecord record = std::move(typedPlan.Operation);
			    if (!record.Failure.has_value())
			    {
				    SetOperationFailure(
				        record,
				        OperationProblemKind::Prerequisite,
				        typedPlan.ReadinessMessages.empty() ? "Operation readiness failed." : typedPlan.ReadinessMessages.back(),
				        "Resolve the reported prerequisite, refresh the workflow, then retry.");
			    }
			    MarkOperationFinished(record, OperationStatus::Skipped, std::nullopt);
			    return record;
		    },
		    plan);

		if (blockedOperation.has_value())
		{
			return std::move(*blockedOperation);
		}

		return RunLauncherOperationPlan(std::move(plan), processRunner, outputCallback, cancellation);
	}

}
