#pragma once

#include "Level/LevelLoadOperation.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <stop_token>
#include <string>
#include <vector>

struct enkiTaskScheduler;
struct enkiTaskSchedulerConfig;
struct enkiPinnedTask;
struct enkiCompletionAction;
struct enkiTaskSet;
struct enkiDependency;

namespace Assets
{
	struct SceneLoadCompletion;
	struct SceneLoadWorkState;

	// One admitted scene's native read/decode chains and physical lifetime.
	class SceneLoadRun final
	{
	public:
		SceneLoadRun(enkiTaskScheduler* io, enkiTaskScheduler* cpu, std::stop_token parentStop, std::unique_ptr<SceneLoadWorkState> work);
		~SceneLoadRun() noexcept;
		void Prepare();
		void Submit();
		void Cancel() noexcept;
		bool IsSettled() const noexcept;
		void Join() noexcept;
		SceneLoadCompletion TakeCompletion(std::uint64_t requestId);
		LevelLoadOperationProgress GetProgress(std::uint64_t requestId) const noexcept;

	private:
		static constexpr std::size_t MaximumAssetsInFlight = 2;

		struct AssetWork final
		{
			SceneLoadRun* Owner = nullptr;
			std::size_t Index = 0;
			enkiPinnedTask* Read = nullptr;
			enkiPinnedTask* Decode = nullptr;
			enkiCompletionAction* ReadComplete = nullptr;
			enkiCompletionAction* Advance = nullptr;
		};

		void PrepareFinalizer(std::uint32_t worker);
		void PrepareAssetWork(const enkiTaskSchedulerConfig& ioConfig, const enkiTaskSchedulerConfig& cpuConfig);
		void PrepareFinalizationGate();
		void ReleaseFinalizationGate() noexcept;
		void ReleaseAssetWork() noexcept;
		bool IsCancelled() const noexcept;
		void ReadAsset(std::size_t index) noexcept;
		void DecodeAsset(std::size_t index) noexcept;
		void Finalize() noexcept;
		std::string DescribeFailure() const;
		static void Read(void* argument) noexcept;
		static void Decode(void* argument) noexcept;
		static void Handoff(void* argument, std::uint32_t) noexcept;
		static void Advance(void* argument, std::uint32_t) noexcept;
		static void Finish(void* argument) noexcept;
		static void AdmitFinalizer(std::uint32_t, std::uint32_t, std::uint32_t, void* argument) noexcept;

		enkiTaskScheduler* m_io;
		enkiTaskScheduler* m_cpu;
		std::stop_token m_parentStop;
		std::stop_source m_stop;
		std::unique_ptr<SceneLoadWorkState> m_work;
		std::vector<AssetWork> m_items;
		std::array<std::exception_ptr, MaximumAssetsInFlight> m_failures;
		std::exception_ptr m_finalFailure;
		enkiPinnedTask* m_finalizer = nullptr;
		enkiTaskSet* m_finalizeGate = nullptr;
		std::array<enkiDependency*, MaximumAssetsInFlight> m_finalizeDependencies{};
		bool m_bSubmitted = false;
	};
}
