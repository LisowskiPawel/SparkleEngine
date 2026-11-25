#pragma once

#include "Level/Loading/SceneLoadCompletion.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <stop_token>

struct enkiTaskScheduler;

namespace Assets
{
	class SceneLoadExecutor final
	{
	public:
		SceneLoadExecutor(enkiTaskScheduler* io, enkiTaskScheduler* cpu, std::stop_token parentStop);
		~SceneLoadExecutor();

		SceneLoadExecutor(const SceneLoadExecutor&) = delete;
		SceneLoadExecutor& operator=(const SceneLoadExecutor&) = delete;

		void Start(std::uint64_t requestId, std::uint64_t worldGeneration, std::uint64_t documentGeneration, LevelDesc level);
		void Cancel() noexcept;
		std::optional<SceneLoadCompletion> ConsumeSettled();
		std::uint64_t GetCatalogGeneration() const noexcept;

	private:
		struct ControlState;
		std::unique_ptr<ControlState> m_control;
	};
}
