#include "Operations/LauncherOperationExecution.h"

#include <type_traits>
#include <utility>

namespace SparkleLauncher
{
	std::optional<std::string_view> FindLauncherOperationDisplayName(std::string_view operationId)
	{
		std::optional<std::string_view> name;
		VisitLauncherOperationDefinitions(
		    [&](const auto& definition)
		    {
			    if (!name && definition.Id == operationId)
			    {
				    name = definition.DisplayName;
			    }
		    });
		return name;
	}

	LauncherOperationPlan PlanLauncherOperation(std::string_view operationId, const LauncherOperationInputs& inputs)
	{
		return std::visit([operationId](const auto& request) -> LauncherOperationPlan { return PlanLauncherOperation(operationId, request); }, inputs);
	}

	OperationRecord RunLauncherOperationPlan(LauncherOperationPlan plan, const Process::ChildProcessOutputCallback& outputCallback, std::stop_token cancellation)
	{
		return std::visit(
		    [&outputCallback, cancellation](auto&& typedPlan) -> OperationRecord
		    {
			    using Plan = std::remove_cvref_t<decltype(typedPlan)>;
			    if constexpr (std::is_same_v<Plan, MaintenanceOperationPlan>)
			    {
				    return RunLauncherOperationPlan(std::move(typedPlan), outputCallback);
			    }
			    else
			    {
				    return RunLauncherOperationPlan(std::move(typedPlan), outputCallback, cancellation);
			    }
		    },
		    std::move(plan));
	}

	OperationRecord ExecuteLauncherOperation(
	    std::string_view operationId,
	    const LauncherOperationInputs& inputs,
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

		return RunLauncherOperationPlan(std::move(plan), outputCallback, cancellation);
	}

}
