#include "Gui/Operations/LauncherOperationExecution.h"

#include "Tasks/Public/TaskExecutionContext.h"

#include <type_traits>
#include <utility>

namespace SparkleLauncher
{
	class TaskContextProcessRunner final : public IProcessRunner
	{
	public:
		explicit TaskContextProcessRunner(IProcessRunner& runner, TaskExecutionContext& context) :
		    m_runner(runner),
		    m_context(context)
		{
		}

		Process::ChildProcessResult Run(const Process::ChildProcessRequest& request) override
		{
			Process::ChildProcessRequest scopedRequest = request;
			scopedRequest.Cancellation = m_context.GetCancellationToken();
			return m_runner.Run(scopedRequest);
		}

	private:
		IProcessRunner& m_runner;
		TaskExecutionContext& m_context;
	};

	using LauncherOperationPlan = std::variant<BuildWorkspaceOperationPlan, LevelOperationPlan, LevelRunOperationPlan, CookOperationPlan, MaintenanceOperationPlan>;

	static LauncherOperationPlan PlanLauncherOperation(const LauncherOperationRequest& request)
	{
		const std::string operationId = request.OperationId.toStdString();

		return std::visit(
		    [&operationId](const auto& inputs) -> LauncherOperationPlan
		    {
			    using Request = std::remove_cvref_t<decltype(inputs)>;
			    if constexpr (std::is_same_v<Request, BuildWorkspaceOperationRequest>)
			    {
				    return PlanBuildWorkspaceOperation(operationId, inputs);
			    }
			    else if constexpr (std::is_same_v<Request, LevelOperationRequest>)
			    {
				    return PlanLevelOperation(operationId, inputs);
			    }
			    else if constexpr (std::is_same_v<Request, LevelRunOperationRequest>)
			    {
				    return PlanLevelRunOperation(operationId, inputs);
			    }
			    else if constexpr (std::is_same_v<Request, CookOperationRequest>)
			    {
				    return PlanCookOperation(operationId, inputs);
			    }
			    else
			    {
				    return PlanMaintenanceOperation(operationId, inputs);
			    }
		    },
		    request.Inputs);
	}

	OperationRecord ExecuteLauncherOperation(
	    const LauncherOperationRequest& request,
	    IProcessRunner& processRunner,
	    TaskExecutionContext& context,
	    const Process::ChildProcessOutputCallback& outputCallback)
	{
		TaskContextProcessRunner taskProcessRunner(processRunner, context);
		LauncherOperationPlan plan = PlanLauncherOperation(request);

		return std::visit(
		    [&taskProcessRunner, &outputCallback](auto&& typedPlan) -> OperationRecord
		    {
			    using Plan = std::remove_cvref_t<decltype(typedPlan)>;
			    if (!typedPlan.bCanRun)
			    {
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
			    }

			    if constexpr (std::is_same_v<Plan, BuildWorkspaceOperationPlan>)
			    {
				    return RunBuildWorkspaceOperationPlan(std::move(typedPlan), taskProcessRunner, outputCallback);
			    }
			    else if constexpr (std::is_same_v<Plan, LevelOperationPlan>)
			    {
				    return RunLevelOperationPlan(std::move(typedPlan), taskProcessRunner, outputCallback);
			    }
			    else if constexpr (std::is_same_v<Plan, LevelRunOperationPlan>)
			    {
				    return RunLevelRunOperationPlan(std::move(typedPlan), taskProcessRunner, outputCallback);
			    }
			    else if constexpr (std::is_same_v<Plan, CookOperationPlan>)
			    {
				    return RunCookOperationPlan(std::move(typedPlan), taskProcessRunner, outputCallback);
			    }
			    else
			    {
				    return RunMaintenanceOperationPlan(std::move(typedPlan));
			    }
		    },
		    std::move(plan));
	}
}
