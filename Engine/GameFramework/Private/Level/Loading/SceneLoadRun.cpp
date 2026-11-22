#include "PCH.h"
#include "Level/Loading/SceneLoadRun.h"

#include "Level/Loading/SceneLoadExecutor.h"
#include "Level/Loading/SceneLoadWorkState.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Diagnostics/Verify.h"
#include <TaskScheduler_c.h>
#include <algorithm>
#include <new>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_sceneLoadLogger, "GameFramework.SceneLoad");

namespace Assets
{
	SceneLoadRun::SceneLoadRun(enkiTaskScheduler* io, enkiTaskScheduler* cpu, std::stop_token parentStop, std::unique_ptr<SceneLoadWorkState> work) :
	    m_io(io),
	    m_cpu(cpu),
	    m_parentStop(parentStop),
	    m_work(std::move(work)),
	    m_items(m_work->Assets.size())
	{
	}

	SceneLoadRun::~SceneLoadRun() noexcept
	{
		if (m_bSubmitted && !IsSettled())
		{
			Diagnostics::Fatal(g_sceneLoadLogger, __FILE__, __LINE__, "Accepted scene work must be joined before destroying its native argument storage.");
		}
		ReleaseFinalizationGate();
		ReleaseAssetWork();
	}

	void SceneLoadRun::ReleaseFinalizationGate() noexcept
	{
		for (auto* dependency : m_finalizeDependencies)
		{
			if (dependency)
			{
				enkiDeleteDependency(m_cpu, dependency);
			}
		}
		if (m_finalizeGate)
		{
			enkiDeleteTaskSet(m_cpu, m_finalizeGate);
		}
	}

	void SceneLoadRun::ReleaseAssetWork() noexcept
	{
		for (auto& item : m_items)
		{
			if (item.Advance)
			{
				enkiDeleteCompletionAction(m_cpu, item.Advance);
			}
			if (item.ReadComplete)
			{
				enkiDeleteCompletionAction(m_io, item.ReadComplete);
			}
		}
		if (m_finalizer)
		{
			enkiDeletePinnedTask(m_cpu, m_finalizer);
		}
		for (auto& item : m_items)
		{
			if (item.Decode)
			{
				enkiDeletePinnedTask(m_cpu, item.Decode);
			}
			if (item.Read)
			{
				enkiDeletePinnedTask(m_io, item.Read);
			}
		}
	}

	bool SceneLoadRun::IsCancelled() const noexcept
	{
		return m_stop.stop_requested() || m_parentStop.stop_requested();
	}

	void SceneLoadRun::Prepare()
	{
		if (!m_io && !m_cpu)
		{
			return;
		}
		if (!m_io || !m_cpu)
		{
			throw Diagnostics::Error("Scene loading requires both native I/O and CPU resources, or neither.");
		}
		const auto ioConfig = enkiGetTaskSchedulerConfig(m_io);
		const auto cpuConfig = enkiGetTaskSchedulerConfig(m_cpu);
		if (ioConfig.numTaskThreadsToCreate == 0 || cpuConfig.numTaskThreadsToCreate == 0)
		{
			throw Diagnostics::Error("Scene loading requires physical native I/O and CPU workers.");
		}
		PrepareFinalizer(cpuConfig.numExternalTaskThreads + 1);
		PrepareAssetWork(ioConfig, cpuConfig);
		PrepareFinalizationGate();
	}

	void SceneLoadRun::PrepareFinalizer(std::uint32_t worker)
	{
		m_finalizer = enkiCreatePinnedTask(m_cpu, Finish, worker);
		if (!m_finalizer)
		{
			throw std::bad_alloc();
		}
		enkiSetArgsPinnedTask(m_finalizer, this);
	}

	void SceneLoadRun::PrepareAssetWork(const enkiTaskSchedulerConfig& ioConfig, const enkiTaskSchedulerConfig& cpuConfig)
	{
		for (std::size_t index = 0; index < m_items.size(); ++index)
		{
			auto& item = m_items[index];
			item.Owner = this;
			item.Index = index;
			const auto ioWorker = ioConfig.numExternalTaskThreads + 1 + static_cast<std::uint32_t>(index % ioConfig.numTaskThreadsToCreate);
			const auto cpuWorker = cpuConfig.numExternalTaskThreads + 1 + static_cast<std::uint32_t>(index % cpuConfig.numTaskThreadsToCreate);
			item.Read = enkiCreatePinnedTask(m_io, Read, ioWorker);
			item.Decode = enkiCreatePinnedTask(m_cpu, Decode, cpuWorker);
			item.ReadComplete = enkiCreateCompletionAction(m_io, Handoff, nullptr);
			if (!item.Read || !item.Decode || !item.ReadComplete)
			{
				throw std::bad_alloc();
			}
			enkiSetArgsPinnedTask(item.Read, &item);
			enkiSetArgsPinnedTask(item.Decode, &item);
			enkiSetParamsCompletionAction(item.ReadComplete, {&item, nullptr, enkiGetCompletableFromPinnedTask(item.Read)});
			if (index + MaximumAssetsInFlight < m_items.size())
			{
				item.Advance = enkiCreateCompletionAction(m_cpu, Advance, nullptr);
				if (!item.Advance)
				{
					throw std::bad_alloc();
				}
				enkiSetParamsCompletionAction(item.Advance, {&item, nullptr, enkiGetCompletableFromPinnedTask(item.Decode)});
			}
		}
	}

	void SceneLoadRun::PrepareFinalizationGate()
	{
		if (m_items.empty())
		{
			return;
		}
		m_finalizeGate = enkiCreateTaskSet(m_cpu, AdmitFinalizer);
		if (!m_finalizeGate)
		{
			throw std::bad_alloc();
		}
		enkiSetParamsTaskSet(m_finalizeGate, {this, 1, 1, 0});
		const auto terminalCount = (std::min) (m_items.size(), MaximumAssetsInFlight);
		for (std::size_t terminal = 0; terminal < terminalCount; ++terminal)
		{
			auto& dependency = m_finalizeDependencies[terminal];
			dependency = enkiCreateDependency(m_cpu);
			if (!dependency)
			{
				throw std::bad_alloc();
			}
			auto* previous = m_items[m_items.size() - 1 - terminal].Decode;
			enkiSetDependency(dependency, enkiGetCompletableFromPinnedTask(previous), enkiGetCompletableFromTaskSet(m_finalizeGate));
		}
	}

	void SceneLoadRun::Submit()
	{
		m_bSubmitted = true;
		if (!m_io)
		{
			for (std::size_t index = 0; index < m_items.size(); ++index)
			{
				ReadAsset(index);
				DecodeAsset(index);
			}
			Finalize();
			return;
		}
		if (m_items.empty())
		{
			enkiAddPinnedTask(m_cpu, m_finalizer);
			return;
		}
		for (std::size_t index = 0; index < (std::min) (m_items.size(), MaximumAssetsInFlight); ++index)
		{
			enkiAddPinnedTask(m_io, m_items[index].Read);
		}
	}

	void SceneLoadRun::ReadAsset(std::size_t index) noexcept
	{
		const auto slot = index % MaximumAssetsInFlight;
		if (IsCancelled() || m_failures[slot])
		{
			return;
		}
		try
		{
			m_work->ReadAsset(index);
		}
		catch (...)
		{
			m_failures[slot] = std::current_exception();
		}
	}

	void SceneLoadRun::DecodeAsset(std::size_t index) noexcept
	{
		const auto slot = index % MaximumAssetsInFlight;
		if (!IsCancelled() && !m_failures[slot])
		{
			try
			{
				m_work->DecodeAsset(index);
			}
			catch (...)
			{
				m_failures[slot] = std::current_exception();
			}
		}
		// A skipped or failed body still closes this admission's mapped inputs.
		m_work->Assets[index].Files.Reset();
		m_work->Assets[index].Manifest = {};
	}

	void SceneLoadRun::Finalize() noexcept
	{
		auto stage = LevelLoadOperationStage::Ready;
		const bool bFailed = std::any_of(m_failures.begin(), m_failures.end(), [](const auto& failure) { return bool(failure); });
		if (bFailed)
		{
			stage = LevelLoadOperationStage::Failed;
		}
		else if (IsCancelled())
		{
			stage = LevelLoadOperationStage::Cancelled;
		}
		else
		{
			try
			{
				m_work->FinalizePackage();
			}
			catch (...)
			{
				m_finalFailure = std::current_exception();
				stage = LevelLoadOperationStage::Failed;
			}
		}
		m_work->Stage.store(stage, std::memory_order_release);
	}

	void SceneLoadRun::Read(void* argument) noexcept
	{
		auto& item = *static_cast<AssetWork*>(argument);
		item.Owner->ReadAsset(item.Index);
	}

	void SceneLoadRun::Decode(void* argument) noexcept
	{
		auto& item = *static_cast<AssetWork*>(argument);
		item.Owner->DecodeAsset(item.Index);
	}

	void SceneLoadRun::Handoff(void* argument, std::uint32_t) noexcept
	{
		auto& item = *static_cast<AssetWork*>(argument);
		enkiAddPinnedTask(item.Owner->m_cpu, item.Decode);
	}

	void SceneLoadRun::Advance(void* argument, std::uint32_t) noexcept
	{
		auto& item = *static_cast<AssetWork*>(argument);
		auto& owner = *item.Owner;
		enkiAddPinnedTask(owner.m_io, owner.m_items[item.Index + MaximumAssetsInFlight].Read);
	}

	void SceneLoadRun::Finish(void* argument) noexcept
	{
		static_cast<SceneLoadRun*>(argument)->Finalize();
	}

	void SceneLoadRun::AdmitFinalizer(std::uint32_t, std::uint32_t, std::uint32_t, void* argument) noexcept
	{
		auto& run = *static_cast<SceneLoadRun*>(argument);
		enkiAddPinnedTask(run.m_cpu, run.m_finalizer);
	}

	bool SceneLoadRun::IsSettled() const noexcept
	{
		const auto stage = m_work->Stage.load(std::memory_order_acquire);
		const bool bTerminal = stage == LevelLoadOperationStage::Ready || stage == LevelLoadOperationStage::Failed;
		if (!bTerminal && stage != LevelLoadOperationStage::Cancelled)
		{
			return false;
		}
		if (!m_io)
		{
			return true;
		}
		if (!enkiIsPinnedTaskComplete(m_cpu, m_finalizer))
		{
			return false;
		}
		if (m_finalizeGate && !enkiIsTaskSetComplete(m_cpu, m_finalizeGate))
		{
			return false;
		}
		for (const auto& item : m_items)
		{
			if (!enkiIsCompletableComplete(m_io, enkiGetCompletableFromCompletionAction(item.ReadComplete)))
			{
				return false;
			}
			if (item.Advance && !enkiIsCompletableComplete(m_cpu, enkiGetCompletableFromCompletionAction(item.Advance)))
			{
				return false;
			}
		}
		return true;
	}

	void SceneLoadRun::Join() noexcept
	{
		if (!m_bSubmitted || !m_io)
		{
			return;
		}
		// Wait in admission order: each handoff has returned before a wait on its
		// target. Native initial-complete state is never mistaken for submitted work.
		for (const auto& item : m_items)
		{
			enkiWaitForPinnedTask(m_io, item.Read);
			enkiWaitForCompletable(m_io, enkiGetCompletableFromCompletionAction(item.ReadComplete));
			enkiWaitForPinnedTask(m_cpu, item.Decode);
			if (item.Advance)
			{
				enkiWaitForCompletable(m_cpu, enkiGetCompletableFromCompletionAction(item.Advance));
			}
		}
		if (m_finalizeGate)
		{
			enkiWaitForTaskSet(m_cpu, m_finalizeGate);
		}
		enkiWaitForPinnedTask(m_cpu, m_finalizer);
	}

	void SceneLoadRun::Cancel() noexcept
	{
		m_stop.request_stop();
	}

	SceneLoadCompletion SceneLoadRun::TakeCompletion(std::uint64_t requestId)
	{
		SceneLoadCompletion completion;
		completion.RequestId = requestId;
		completion.Stage = m_work->Stage.load(std::memory_order_acquire);
		if (completion.Stage == LevelLoadOperationStage::Ready)
		{
			completion.Package = std::move(m_work->Package);
		}
		else if (completion.Stage == LevelLoadOperationStage::Cancelled)
		{
			completion.Diagnostic = "Scene load cancelled.";
		}
		else
		{
			completion.Diagnostic = DescribeFailure();
		}
		return completion;
	}

	std::string SceneLoadRun::DescribeFailure() const
	{
		auto failure = m_finalFailure;
		if (!failure)
		{
			for (const auto& item : m_failures)
			{
				if (item)
				{
					failure = item;
					break;
				}
			}
		}
		try
		{
			if (failure)
			{
				std::rethrow_exception(failure);
			}
			throw Diagnostics::Error("Scene load failed without a diagnostic.");
		}
		catch (const std::exception& error)
		{
			return error.what();
		}
		catch (...)
		{
			return "Unknown scene load failure.";
		}
	}

	LevelLoadOperationProgress SceneLoadRun::GetProgress(std::uint64_t requestId) const noexcept
	{
		LevelLoadOperationProgress progress;
		progress.RequestId = requestId;
		progress.Stage = m_work->Stage.load(std::memory_order_acquire);
		progress.CompletedAssets = m_work->CompletedDecodes.load(std::memory_order_relaxed);
		progress.TotalAssets = static_cast<std::uint32_t>(m_work->Assets.size());
		return progress;
	}
}
