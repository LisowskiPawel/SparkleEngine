#include "PCH.h"
#include "Level/Loading/SceneLoadExecutor.h"

#include "Assets/SceneAssetCatalog.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Core/Public/Paths/CookedAssetPaths.h"
#include "Level/Loading/SceneLoadWorkState.h"
#include <TaskScheduler_c.h>

#include <algorithm>
#include <array>
#include <exception>
#include <new>
#include <stop_token>
#include <utility>
#include <vector>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_sceneLoadLogger, "GameFramework.SceneLoad");

namespace Assets
{
	static constexpr std::size_t MaximumSceneAssetsInFlight = 2;
	struct SceneLoadRun;

	struct SceneAssetNativeWork final
	{
		SceneLoadRun* Owner = nullptr;
		std::size_t Index = 0;
		enkiPinnedTask* Read = nullptr;
		enkiPinnedTask* Decode = nullptr;
		enkiCompletionAction* ReadComplete = nullptr;
		enkiCompletionAction* Advance = nullptr;
	};

	struct SceneLoadRun final
	{
		SceneLoadRun(enkiTaskScheduler* io, enkiTaskScheduler* cpu, std::stop_token parentStop, std::unique_ptr<SceneLoadWorkState> work);
		~SceneLoadRun() noexcept;
		void Prepare();
		void Submit();
		bool IsSettled() const noexcept;
		void Join() noexcept;
		bool IsCancelled() const noexcept;
		void ReadAsset(std::size_t index) noexcept;
		void DecodeAsset(std::size_t index) noexcept;
		void Finalize() noexcept;
		static void Read(void* argument) noexcept;
		static void Decode(void* argument) noexcept;
		static void Handoff(void* argument, std::uint32_t) noexcept;
		static void Advance(void* argument, std::uint32_t) noexcept;
		static void Finish(void* argument) noexcept;
		static void AdmitFinalizer(std::uint32_t, std::uint32_t, std::uint32_t, void* argument) noexcept;

		enkiTaskScheduler* Io;
		enkiTaskScheduler* Cpu;
		std::stop_token ParentStop;
		std::stop_source Stop;
		std::unique_ptr<SceneLoadWorkState> Work;
		std::vector<SceneAssetNativeWork> Items;
		std::array<std::exception_ptr, MaximumSceneAssetsInFlight> Failures;
		std::exception_ptr FinalFailure;
		enkiPinnedTask* Finalizer = nullptr;
		enkiTaskSet* FinalizeGate = nullptr;
		std::array<enkiDependency*, MaximumSceneAssetsInFlight> FinalizeDependencies{};
		bool bSubmitted = false;
	};

	SceneLoadRun::SceneLoadRun(enkiTaskScheduler* io, enkiTaskScheduler* cpu, std::stop_token parentStop, std::unique_ptr<SceneLoadWorkState> work) :
	    Io(io),
	    Cpu(cpu),
	    ParentStop(parentStop),
	    Work(std::move(work)),
	    Items(Work->Assets.size())
	{
	}

	SceneLoadRun::~SceneLoadRun() noexcept
	{
		if (bSubmitted && !IsSettled())
		{
			Diagnostics::Fatal(g_sceneLoadLogger, __FILE__, __LINE__, "Accepted scene work must be joined before destroying its native argument storage.");
		}
		for (auto* dependency : FinalizeDependencies)
		{
			if (dependency)
			{
				enkiDeleteDependency(Cpu, dependency);
			}
		}
		if (FinalizeGate)
		{
			enkiDeleteTaskSet(Cpu, FinalizeGate);
		}
		for (auto& item : Items)
		{
			if (item.Advance)
			{
				enkiDeleteCompletionAction(Cpu, item.Advance);
			}
			if (item.ReadComplete)
			{
				enkiDeleteCompletionAction(Io, item.ReadComplete);
			}
		}
		if (Finalizer)
		{
			enkiDeletePinnedTask(Cpu, Finalizer);
		}
		for (auto& item : Items)
		{
			if (item.Decode)
			{
				enkiDeletePinnedTask(Cpu, item.Decode);
			}
			if (item.Read)
			{
				enkiDeletePinnedTask(Io, item.Read);
			}
		}
	}

	bool SceneLoadRun::IsCancelled() const noexcept
	{
		return Stop.stop_requested() || ParentStop.stop_requested();
	}

	void SceneLoadRun::Prepare()
	{
		if (!Io && !Cpu)
		{
			return;
		}
		if (!Io || !Cpu)
		{
			throw Diagnostics::Error("Scene loading requires both native I/O and CPU resources, or neither.");
		}
		const auto ioConfig = enkiGetTaskSchedulerConfig(Io);
		const auto cpuConfig = enkiGetTaskSchedulerConfig(Cpu);
		if (ioConfig.numTaskThreadsToCreate == 0 || cpuConfig.numTaskThreadsToCreate == 0)
		{
			throw Diagnostics::Error("Scene loading requires physical native I/O and CPU workers.");
		}
		Finalizer = enkiCreatePinnedTask(Cpu, Finish, cpuConfig.numExternalTaskThreads + 1);
		if (!Finalizer)
		{
			throw std::bad_alloc();
		}
		enkiSetArgsPinnedTask(Finalizer, this);
		for (std::size_t index = 0; index < Items.size(); ++index)
		{
			auto& item = Items[index];
			item.Owner = this;
			item.Index = index;
			const auto ioWorker = ioConfig.numExternalTaskThreads + 1 + static_cast<std::uint32_t>(index % ioConfig.numTaskThreadsToCreate);
			const auto cpuWorker = cpuConfig.numExternalTaskThreads + 1 + static_cast<std::uint32_t>(index % cpuConfig.numTaskThreadsToCreate);
			item.Read = enkiCreatePinnedTask(Io, Read, ioWorker);
			item.Decode = enkiCreatePinnedTask(Cpu, Decode, cpuWorker);
			item.ReadComplete = enkiCreateCompletionAction(Io, Handoff, nullptr);
			if (!item.Read || !item.Decode || !item.ReadComplete)
			{
				throw std::bad_alloc();
			}
			enkiSetArgsPinnedTask(item.Read, &item);
			enkiSetArgsPinnedTask(item.Decode, &item);
			enkiSetParamsCompletionAction(item.ReadComplete, {&item, nullptr, enkiGetCompletableFromPinnedTask(item.Read)});
			if (index + MaximumSceneAssetsInFlight < Items.size())
			{
				item.Advance = enkiCreateCompletionAction(Cpu, Advance, nullptr);
				if (!item.Advance)
				{
					throw std::bad_alloc();
				}
				enkiSetParamsCompletionAction(item.Advance, {&item, nullptr, enkiGetCompletableFromPinnedTask(item.Decode)});
			}
		}
		if (Items.empty())
		{
			return;
		}
		FinalizeGate = enkiCreateTaskSet(Cpu, AdmitFinalizer);
		if (!FinalizeGate)
		{
			throw std::bad_alloc();
		}
		enkiSetParamsTaskSet(FinalizeGate, {this, 1, 1, 0});
		const auto terminalCount = (std::min) (Items.size(), MaximumSceneAssetsInFlight);
		for (std::size_t terminal = 0; terminal < terminalCount; ++terminal)
		{
			auto& dependency = FinalizeDependencies[terminal];
			dependency = enkiCreateDependency(Cpu);
			if (!dependency)
			{
				throw std::bad_alloc();
			}
			auto* previous = Items[Items.size() - 1 - terminal].Decode;
			enkiSetDependency(dependency, enkiGetCompletableFromPinnedTask(previous), enkiGetCompletableFromTaskSet(FinalizeGate));
		}
	}

	void SceneLoadRun::Submit()
	{
		bSubmitted = true;
		if (!Io)
		{
			for (std::size_t index = 0; index < Items.size(); ++index)
			{
				ReadAsset(index);
				DecodeAsset(index);
			}
			Finalize();
			return;
		}
		if (Items.empty())
		{
			enkiAddPinnedTask(Cpu, Finalizer);
			return;
		}
		for (std::size_t index = 0; index < (std::min) (Items.size(), MaximumSceneAssetsInFlight); ++index)
		{
			enkiAddPinnedTask(Io, Items[index].Read);
		}
	}

	void SceneLoadRun::ReadAsset(std::size_t index) noexcept
	{
		const auto slot = index % MaximumSceneAssetsInFlight;
		if (IsCancelled() || Failures[slot])
		{
			return;
		}
		try
		{
			Work->ReadAsset(index);
		}
		catch (...)
		{
			Failures[slot] = std::current_exception();
		}
	}

	void SceneLoadRun::DecodeAsset(std::size_t index) noexcept
	{
		const auto slot = index % MaximumSceneAssetsInFlight;
		if (!IsCancelled() && !Failures[slot])
		{
			try
			{
				Work->DecodeAsset(index);
			}
			catch (...)
			{
				Failures[slot] = std::current_exception();
			}
		}
		// A skipped or failed body still closes this admission's mapped inputs.
		Work->Assets[index].Files.Reset();
		Work->Assets[index].Manifest = {};
	}

	void SceneLoadRun::Finalize() noexcept
	{
		auto stage = LevelLoadOperationStage::Ready;
		const bool bFailed = std::any_of(Failures.begin(), Failures.end(), [](const auto& failure) { return bool(failure); });
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
				Work->FinalizePackage();
			}
			catch (...)
			{
				FinalFailure = std::current_exception();
				stage = LevelLoadOperationStage::Failed;
			}
		}
		Work->Stage.store(stage, std::memory_order_release);
	}

	void SceneLoadRun::Read(void* argument) noexcept
	{
		auto& item = *static_cast<SceneAssetNativeWork*>(argument);
		item.Owner->ReadAsset(item.Index);
	}

	void SceneLoadRun::Decode(void* argument) noexcept
	{
		auto& item = *static_cast<SceneAssetNativeWork*>(argument);
		item.Owner->DecodeAsset(item.Index);
	}

	void SceneLoadRun::Handoff(void* argument, std::uint32_t) noexcept
	{
		auto& item = *static_cast<SceneAssetNativeWork*>(argument);
		enkiAddPinnedTask(item.Owner->Cpu, item.Decode);
	}

	void SceneLoadRun::Advance(void* argument, std::uint32_t) noexcept
	{
		auto& item = *static_cast<SceneAssetNativeWork*>(argument);
		auto& owner = *item.Owner;
		enkiAddPinnedTask(owner.Io, owner.Items[item.Index + MaximumSceneAssetsInFlight].Read);
	}

	void SceneLoadRun::Finish(void* argument) noexcept
	{
		static_cast<SceneLoadRun*>(argument)->Finalize();
	}

	void SceneLoadRun::AdmitFinalizer(std::uint32_t, std::uint32_t, std::uint32_t, void* argument) noexcept
	{
		auto& run = *static_cast<SceneLoadRun*>(argument);
		enkiAddPinnedTask(run.Cpu, run.Finalizer);
	}

	bool SceneLoadRun::IsSettled() const noexcept
	{
		const auto stage = Work->Stage.load(std::memory_order_acquire);
		const bool bTerminal = stage == LevelLoadOperationStage::Ready || stage == LevelLoadOperationStage::Failed;
		if (!bTerminal && stage != LevelLoadOperationStage::Cancelled)
		{
			return false;
		}
		if (!Io)
		{
			return true;
		}
		if (!enkiIsPinnedTaskComplete(Cpu, Finalizer))
		{
			return false;
		}
		if (FinalizeGate && !enkiIsTaskSetComplete(Cpu, FinalizeGate))
		{
			return false;
		}
		for (const auto& item : Items)
		{
			if (!enkiIsCompletableComplete(Io, enkiGetCompletableFromCompletionAction(item.ReadComplete)))
			{
				return false;
			}
			if (item.Advance && !enkiIsCompletableComplete(Cpu, enkiGetCompletableFromCompletionAction(item.Advance)))
			{
				return false;
			}
		}
		return true;
	}

	void SceneLoadRun::Join() noexcept
	{
		if (!bSubmitted || !Io)
		{
			return;
		}
		// Wait in admission order: each handoff has returned before a wait on its
		// target. Native initial-complete state is never mistaken for submitted work.
		for (const auto& item : Items)
		{
			enkiWaitForPinnedTask(Io, item.Read);
			enkiWaitForCompletable(Io, enkiGetCompletableFromCompletionAction(item.ReadComplete));
			enkiWaitForPinnedTask(Cpu, item.Decode);
			if (item.Advance)
			{
				enkiWaitForCompletable(Cpu, enkiGetCompletableFromCompletionAction(item.Advance));
			}
		}
		if (FinalizeGate)
		{
			enkiWaitForTaskSet(Cpu, FinalizeGate);
		}
		enkiWaitForPinnedTask(Cpu, Finalizer);
	}

	struct SceneLoadExecutor::ControlState final
	{
		enkiTaskScheduler* Io;
		enkiTaskScheduler* Cpu;
		std::stop_token ParentStop;
		std::unique_ptr<SceneLoadRun> Current;
		std::shared_ptr<const SceneAssetCatalog> Catalog;
		std::uint64_t NextCatalogGeneration = 1;
		std::uint64_t RequestId = 0;
	};

	SceneLoadExecutor::SceneLoadExecutor(enkiTaskScheduler* io, enkiTaskScheduler* cpu, std::stop_token parentStop) :
	    m_control(std::make_unique<ControlState>(io, cpu, parentStop))
	{
	}

	SceneLoadExecutor::~SceneLoadExecutor()
	{
		Cancel();
		if (m_control->Current)
		{
			m_control->Current->Join();
		}
	}

	void SceneLoadExecutor::Start(std::uint64_t requestId, std::uint64_t worldGeneration, std::uint64_t documentGeneration, LevelDesc level)
	{
		if (m_control->Current)
		{
			throw Diagnostics::Error("A scene load was started before the previous request settled and was consumed.");
		}
		auto work = std::make_unique<SceneLoadWorkState>(Filesystem::GetCookedAssetRootPath());
		m_control->Catalog = LoadSceneAssetCatalog(m_control->NextCatalogGeneration++);
		work->PreparePackage(requestId, worldGeneration, documentGeneration, std::move(level), *m_control->Catalog);
		auto run = std::make_unique<SceneLoadRun>(m_control->Io, m_control->Cpu, m_control->ParentStop, std::move(work));
		run->Prepare();
		m_control->RequestId = requestId;
		m_control->Current = std::move(run);
		m_control->Current->Submit();
	}

	void SceneLoadExecutor::Cancel() noexcept
	{
		if (m_control->Current)
		{
			m_control->Current->Stop.request_stop();
		}
	}

	std::optional<SceneLoadCompletion> SceneLoadExecutor::ConsumeSettled()
	{
		auto& run = m_control->Current;
		if (!run || !run->IsSettled())
		{
			return std::nullopt;
		}
		SceneLoadCompletion completion;
		completion.RequestId = m_control->RequestId;
		completion.Stage = run->Work->Stage.load(std::memory_order_acquire);
		if (completion.Stage == LevelLoadOperationStage::Ready)
		{
			completion.Package = std::move(run->Work->Package);
		}
		else if (completion.Stage == LevelLoadOperationStage::Cancelled)
		{
			completion.Diagnostic = "Scene load cancelled.";
		}
		else
		{
			auto failure = run->FinalFailure;
			if (!failure)
			{
				for (const auto& item : run->Failures)
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
				completion.Diagnostic = error.what();
			}
			catch (...)
			{
				completion.Diagnostic = "Unknown scene load failure.";
			}
		}
		run.reset();
		return completion;
	}

	LevelLoadOperationProgress SceneLoadExecutor::GetProgress() const noexcept
	{
		LevelLoadOperationProgress progress;
		progress.RequestId = m_control->RequestId;
		if (m_control->Current)
		{
			const auto& work = *m_control->Current->Work;
			progress.Stage = work.Stage.load(std::memory_order_acquire);
			progress.CompletedAssets = work.CompletedDecodes.load(std::memory_order_relaxed);
			progress.TotalAssets = static_cast<std::uint32_t>(work.Assets.size());
		}
		return progress;
	}

	std::uint64_t SceneLoadExecutor::GetCatalogGeneration() const noexcept
	{
		return m_control->Catalog ? m_control->Catalog->GetGeneration() : 0;
	}
}
