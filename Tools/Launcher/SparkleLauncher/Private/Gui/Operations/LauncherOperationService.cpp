#include "Gui/Operations/LauncherOperationService.h"
#include "Operations/LauncherOperationExecution.h"
#include "GpuCrash/GpuCrashLaunch.h"
#include <TaskScheduler_c.h>
#include "Core/Public/Diagnostics/Verify.h"
#include "Core/Public/Diagnostics/Logger.h"
#include <thread>
#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <mutex>
#include <new>
#include <optional>
#include <variant>
#include <stop_token>
#include <stdexcept>
#include <utility>

namespace SparkleLauncher
{
	SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_operationLogger, "Launcher.Operations");

	struct LauncherOperationService::Implementation final
	{
		static constexpr std::size_t MaximumRuns = 16;

		struct Run final
		{
			Implementation* Owner;
			std::string Id;
			LauncherOperationRequest Request;
			OperationRecord Record;
			OutputCallback Output;
			CompletionCallback Completion;
			std::optional<GpuCrashIncident> Incident;
			std::function<void(std::string)> Observation;
			std::stop_source Stop;
			enkiPinnedTask* Work = nullptr;
			enkiCompletionAction* Cleanup = nullptr;

			~Run()
			{
				if (Cleanup)
				{
					enkiDeleteCompletionAction(Owner->Scheduler, Cleanup);
				}
				if (Work)
				{
					enkiDeletePinnedTask(Owner->Scheduler, Work);
				}
			}
		};

		explicit Implementation(ProcessRunnerFactory factory) :
		    Factory(std::move(factory))
		{
			Scheduler = enkiNewTaskScheduler();
			if (!Scheduler)
			{
				throw std::bad_alloc();
			}
			try
			{
				auto config = enkiGetTaskSchedulerConfig(Scheduler);
				config.numTaskThreadsToCreate = 2;
				config.numExternalTaskThreads = 0;
				char error[256]{};
				if (!enkiTryInitTaskSchedulerWithConfig(Scheduler, config, error, sizeof(error)))
				{
					throw std::runtime_error(error);
				}
			}
			catch (...)
			{
				enkiDeleteTaskScheduler(Scheduler);
				throw;
			}
		}

		~Implementation()
		{
			if (std::this_thread::get_id() != CreatorThread)
			{
				Diagnostics::Fatal(g_operationLogger, __FILE__, __LINE__, "Launcher operation service must shut down on its initializing UI thread, after callback admission closes.");
			}

			std::array<std::optional<std::stop_source>, MaximumRuns> stops;
			std::size_t count = 0;
			{
				std::lock_guard lock(Mutex);
				bClosing = true;
				for (const auto& run : Runs)
				{
					stops[count++].emplace(run->Stop);
				}
			}
			for (auto& stop : stops)
			{
				if (stop)
				{
					stop->request_stop();
				}
			}
			// Joining native workers also covers post actions already marked complete.
			enkiWaitforAllAndShutdown(Scheduler);
			enkiDeleteTaskScheduler(Scheduler);
		}

		static void Execute(void* argument) noexcept
		{
			auto& run = *static_cast<Run*>(argument);
			try
			{
				if (run.Stop.stop_requested())
				{
					return;
				}
				if (run.Incident)
				{
					run.Observation(ReadGpuCrashCollectionState(*run.Incident));
					return;
				}
				auto runner = run.Owner->Factory();
				if (!runner)
				{
					throw std::runtime_error("No process runner is available for this Launcher operation.");
				}
				const auto operationId = run.Request.OperationId.toStdString();
				run.Record = ExecuteLauncherOperation(operationId, run.Request.Inputs, *runner, run.Stop.get_token(), run.Output);
			}
			catch (const std::exception& error)
			{
				if (run.Incident)
				{
					return;
				}
				try
				{
					SetOperationFailure(run.Record, OperationProblemKind::Internal, error.what(), "Resolve the reported error, then retry.");
				}
				catch (...)
				{ /* The pre-admission failure record remains valid. */
				}
				MarkOperationFinished(run.Record, OperationStatus::Failed, std::nullopt);
			}
			catch (...)
			{
				if (run.Incident)
				{
					return;
				}
				try
				{
					SetOperationFailure(run.Record, OperationProblemKind::Internal, "Unhandled Launcher operation exception.", "Restart Sparkle Launcher before retrying.");
				}
				catch (...)
				{ /* The pre-admission failure record remains valid. */
				}
				MarkOperationFinished(run.Record, OperationStatus::Failed, std::nullopt);
			}
		}

		static void Complete(void* argument, std::uint32_t) noexcept
		{
			auto& run = *static_cast<Run*>(argument);
			if (run.Incident)
			{
				return;
			}
			try
			{
				if (run.Record.Status == OperationStatus::Pending)
				{
					const bool bCanceled = run.Stop.stop_requested();
					run.Record.Failure->Kind = bCanceled ? OperationProblemKind::Cancellation : OperationProblemKind::Internal;
					MarkOperationFinished(run.Record, bCanceled ? OperationStatus::Canceled : OperationStatus::Failed, std::nullopt);
				}
				{
					std::lock_guard lock(run.Owner->Mutex);
					run.Owner->bBusy = false;
				}
				run.Completion(std::move(run.Record));
			}
			catch (...)
			{
				std::cerr << "Launcher completion callback failed.\n";
			}
		}

		static void Release(void* argument, std::uint32_t) noexcept
		{
			auto* run = static_cast<Run*>(argument);
			std::unique_ptr<Run> retired;
			{
				std::lock_guard lock(run->Owner->Mutex);
				auto& runs = run->Owner->Runs;
				const auto found = std::find_if(runs.begin(), runs.end(), [run](const auto& item) { return item.get() == run; });
				retired = std::move(*found);
				runs.erase(found);
			}
			// Upstream permits post-action deletion. Captures die outside the mutex.
		}

		void Prepare(Run& run)
		{
			run.Work = enkiCreatePinnedTask(Scheduler, Execute, static_cast<std::uint32_t>(Runs.size() % 2) + 1);
			if (!run.Work)
			{
				throw std::bad_alloc();
			}
			enkiSetArgsPinnedTask(run.Work, &run);
			run.Cleanup = enkiCreateCompletionAction(Scheduler, Complete, Release);
			if (!run.Cleanup)
			{
				throw std::bad_alloc();
			}
			enkiSetParamsCompletionAction(run.Cleanup, {&run, &run, enkiGetCompletableFromPinnedTask(run.Work)});
		}

		const std::thread::id CreatorThread = std::this_thread::get_id();
		ProcessRunnerFactory Factory;
		enkiTaskScheduler* Scheduler = nullptr;
		std::mutex Mutex;
		std::list<std::unique_ptr<Run>> Runs;
		bool bBusy = false;
		bool bClosing = false;
	};

	LauncherOperationService::LauncherOperationService(ProcessRunnerFactory factory) :
	    m_implementation(std::make_unique<Implementation>(std::move(factory)))
	{
	}

	LauncherOperationService::~LauncherOperationService() = default;

	void LauncherOperationService::Launch(LauncherOperationRequest request, std::string title, OutputCallback output, CompletionCallback completion)
	{
		auto& owner = *m_implementation;
		const auto id = request.RunId.isEmpty() ? request.OperationId.toStdString() : request.RunId.toStdString();
		if (id.empty())
		{
			throw std::logic_error("Launcher operation run identity is empty.");
		}
		auto run = std::make_unique<Implementation::Run>();
		run->Owner = &owner;
		run->Id = id;
		run->Record = MakeOperationRecord(request.OperationId.toStdString(), std::move(title));
		SetOperationFailure(run->Record, OperationProblemKind::Internal, "Launcher operation was interrupted before producing a result.", "Retry the operation when ready.");
		run->Request = std::move(request);
		run->Output = std::move(output);
		run->Completion = std::move(completion);
		std::unique_lock lock(owner.Mutex);
		const bool bDuplicate = std::any_of(owner.Runs.begin(), owner.Runs.end(), [&id](const auto& active) { return active->Id == id; });
		const bool bUnavailable = owner.bClosing || owner.bBusy;
		const bool bCapacityReached = bDuplicate || owner.Runs.size() == Implementation::MaximumRuns;
		if (bUnavailable || bCapacityReached)
		{
			lock.unlock();
			SetOperationFailure(run->Record, OperationProblemKind::Prerequisite, "Another Launcher operation is running.", "Wait for the running operation or cancel it, then retry.");
			MarkOperationFinished(run->Record, OperationStatus::Skipped, std::nullopt);
			run->Completion(std::move(run->Record));
			return;
		}
		owner.Prepare(*run);
		auto* work = run->Work;
		owner.Runs.push_back(std::move(run));
		owner.bBusy = true;
		lock.unlock();
		enkiAddPinnedTask(owner.Scheduler, work);
	}

	bool LauncherOperationService::Cancel(std::string_view id) noexcept
	{
		auto& owner = *m_implementation;
		std::stop_source stop{std::nostopstate};
		{
			std::lock_guard lock(owner.Mutex);
			for (const auto& run : owner.Runs)
			{
				if (run->Id == id)
				{
					stop = run->Stop;
					break;
				}
			}
		}
		if (!stop.stop_possible())
		{
			return false;
		}
		stop.request_stop();
		return true;
	}

	bool LauncherOperationService::ObserveGpuCrash(GpuCrashIncident incident, std::function<void(std::string)> completion)
	{
		auto& owner = *m_implementation;
		auto run = std::make_unique<Implementation::Run>();
		run->Owner = &owner;
		run->Incident = std::move(incident);
		run->Observation = std::move(completion);
		std::unique_lock lock(owner.Mutex);
		if (owner.bClosing || owner.Runs.size() == Implementation::MaximumRuns)
		{
			return false;
		}
		owner.Prepare(*run);
		auto* work = run->Work;
		owner.Runs.push_back(std::move(run));
		lock.unlock();
		enkiAddPinnedTask(owner.Scheduler, work);
		return true;
	}
}
