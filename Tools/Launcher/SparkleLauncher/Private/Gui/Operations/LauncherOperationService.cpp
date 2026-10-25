#include "LauncherOperationService.h"

#include "LauncherOperationExecution.h"
#include "GpuCrashLaunch.h"
#include "Tasks/Public/TaskExecutor.h"
#include "Tasks/Public/TaskGraph.h"
#include "Tasks/Public/TaskScope.h"

#include <atomic>
#include <exception>
#include <map>
#include <stdexcept>
#include <utility>

namespace SparkleLauncher
{
	struct LauncherOperationService::Implementation final
	{
		explicit Implementation(ProcessRunnerFactory processRunnerFactory) :
		    m_processRunnerFactory(std::move(processRunnerFactory))
		{
		}

		~Implementation()
		{
			m_rootScope.Cancel();
			m_executor.Shutdown(TaskExecutorShutdownMode::Cancel);
			for (const auto& operation : m_operationScopes)
			{
				operation.second->JoinFor(std::chrono::milliseconds::zero());
			}
			m_rootScope.JoinFor(std::chrono::milliseconds::zero());
		}

		void ReapSettledOperations()
		{
			for (auto operation = m_operationScopes.begin(); operation != m_operationScopes.end();)
			{
				// JoinFor closes the per-run scope. A scope does not become settled merely
				// because its only execution finished; the owner must close it explicitly.
				if (!operation->second->JoinFor(std::chrono::milliseconds::zero()))
				{
					++operation;
					continue;
				}

				operation = m_operationScopes.erase(operation);
			}
		}

		ProcessRunnerFactory m_processRunnerFactory;
		std::atomic<bool> m_operationRunning{false};
		TaskExecutor m_executor{TaskExecutorConfig{.FrameCriticalWorkerCount = 1, .BackgroundWorkerCount = 1, .BlockingIoWorkerCount = 2, .MaximumActiveExecutions = 16}};
		TaskScope m_rootScope{TaskScopeDesc{TaskScopeKind::Application, "Launcher operations"}};
		std::map<std::string, std::unique_ptr<TaskScope>, std::less<>> m_operationScopes;
	};

	LauncherOperationService::LauncherOperationService(ProcessRunnerFactory processRunnerFactory) :
	    m_implementation(std::make_unique<Implementation>(std::move(processRunnerFactory)))
	{
	}

	LauncherOperationService::~LauncherOperationService() = default;

	void LauncherOperationService::Launch(LauncherOperationCategory category, LauncherOperationRequest request, std::string title, OutputCallback outputCallback, CompletionCallback completionCallback)
	{
		m_implementation->ReapSettledOperations();
		const std::string runId = request.RunId.isEmpty() ? request.OperationId.toStdString() : request.RunId.toStdString();
		if (runId.empty())
		{
			throw std::logic_error("Launcher operation run identity is empty.");
		}
		if (m_implementation->m_operationRunning.load(std::memory_order_acquire) || m_implementation->m_operationScopes.contains(runId))
		{
			OperationRecord record = MakeOperationRecord(request.OperationId.toStdString(), title);

			SetOperationFailure(
			    record,
			    OperationProblemKind::Prerequisite,
			    "Another Launcher operation is running.",
			    "Wait for the running operation or select it in Activity and cancel it, then retry.");

			MarkOperationFinished(record, OperationStatus::Skipped, std::nullopt);
			completionCallback(std::move(record));
			return;
		}

		auto record = std::make_shared<OperationRecord>(MakeOperationRecord(request.OperationId.toStdString(), std::move(title)));
		auto operationScope = std::make_unique<TaskScope>(TaskScopeDesc{TaskScopeKind::ToolInvocation, "Launcher operation " + runId}, &m_implementation->m_rootScope);
		TaskScope& scope = *operationScope;
		m_implementation->m_operationScopes.emplace(runId, std::move(operationScope));
		m_implementation->m_operationRunning.store(true, std::memory_order_release);
		try
		{
			TaskGraphBuilder graph;

			const TaskNodeHandle execute = graph.Add(
			    TaskDesc{TaskName("Launcher operation"), TaskLane::BlockingIo},
			    [category, request = std::move(request), processRunnerFactory = m_implementation->m_processRunnerFactory, outputCallback = std::move(outputCallback), record](
			        TaskExecutionContext& context)
			    {
				    try
				    {
					    std::unique_ptr<IProcessRunner> processRunner = processRunnerFactory();
					    if (!processRunner)
					    {
						    throw std::runtime_error("No process runner is available for this Launcher operation.");
					    }
					    *record = ExecuteLauncherOperation(category, request.OperationId.toStdString(), request, *processRunner, context, outputCallback);
					    return TaskResult::Success();
				    }
				    catch (const std::exception& exception)
				    {
					    SetOperationFailure(*record, OperationProblemKind::Internal, exception.what(), "Resolve the reported error, then retry.");
				    }
				    catch (...)
				    {
					    SetOperationFailure(*record, OperationProblemKind::Internal, "Unhandled Launcher operation exception.", "Restart Sparkle Launcher before retrying.");
				    }
				    MarkOperationFinished(*record, OperationStatus::Failed, std::nullopt);
				    return TaskResult::Failure(record->Failure->Summary);
			    });

			// Cleanup runs even when cancellation prevents the operation body from starting.
			graph.ContinueWith(
			    execute,
			    TaskDesc{TaskName("Complete Launcher operation"), TaskLane::BlockingIo, TaskCompletionPolicy::Cleanup},
			    [record, completionCallback, implementation = m_implementation.get()](TaskExecutionContext& context)
			    {
				    if (record->Status == OperationStatus::Pending)
				    {
					    const bool canceled = context.IsCancellationRequested();

					    SetOperationFailure(
					        *record,
					        canceled ? OperationProblemKind::Cancellation : OperationProblemKind::Internal,
					        canceled ? "Launcher operation was canceled before starting." : "Launcher operation did not produce a result.",
					        "Retry the operation when ready.");

					    MarkOperationFinished(*record, canceled ? OperationStatus::Canceled : OperationStatus::Failed, std::nullopt);
				    }
				    // Mutating work has settled; scope bookkeeping can finish after the next workflow starts.
				    implementation->m_operationRunning.store(false, std::memory_order_release);
				    completionCallback(std::move(*record));
				    return TaskResult::Success();
			    });

			const TaskExecution execution = m_implementation->m_executor.Launch(scope, graph.Compile());
			if (execution.GetStatus() == TaskExecutionStatus::Rejected)
			{
				SetOperationFailure(*record, OperationProblemKind::Internal, std::string(execution.GetResult().GetMessage()), "Restart Sparkle Launcher before retrying.");
				MarkOperationFinished(*record, OperationStatus::Failed, std::nullopt);
				scope.JoinFor(std::chrono::milliseconds::zero());
				m_implementation->m_operationScopes.erase(runId);
				m_implementation->m_operationRunning.store(false, std::memory_order_release);
				completionCallback(std::move(*record));
			}
		}
		catch (...)
		{
			const auto operation = m_implementation->m_operationScopes.find(runId);
			if (operation != m_implementation->m_operationScopes.end())
			{
				m_implementation->m_operationRunning.store(false, std::memory_order_release);
				operation->second->JoinFor(std::chrono::milliseconds::zero());
				m_implementation->m_operationScopes.erase(operation);
			}
			throw;
		}
	}

	bool LauncherOperationService::ObserveGpuCrash(GpuCrashIncident incident, std::function<void(std::string)> completion)
	{
		const auto execution = m_implementation->m_executor.Launch(
		    m_implementation->m_rootScope,
		    TaskDesc{TaskName("Read GPU crash session"), TaskLane::BlockingIo},
		    [incident = std::move(incident), completion = std::move(completion)](TaskExecutionContext&)
		    {
			    std::string state = ReadGpuCrashCollectionState(incident);
			    completion(std::move(state));
			    return TaskResult::Success();
		    });

		return execution.IsValid() && execution.GetStatus() != TaskExecutionStatus::Rejected;
	}

	bool LauncherOperationService::Cancel(std::string_view runId) noexcept
	{
		const auto operation = m_implementation->m_operationScopes.find(runId);
		if (operation == m_implementation->m_operationScopes.end() || operation->second->IsSettled())
		{
			return false;
		}

		operation->second->Cancel();
		return true;
	}
}
