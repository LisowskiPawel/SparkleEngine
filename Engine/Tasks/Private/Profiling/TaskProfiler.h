#pragma once

#include "TaskTypes.h"

#include <chrono>
#include <cstdint>

namespace TaskProfiler
{
	using TimePoint = std::chrono::steady_clock::time_point;

	void RecordDependency(std::uint64_t generation, std::uint32_t prerequisite, std::uint32_t dependent) noexcept;
	TimePoint Begin(const TaskDesc& desc, std::uint64_t generation, std::uint32_t taskIndex, std::uint32_t laneWorkerIndex) noexcept;
	void End(const TaskDesc& desc, std::uint64_t generation, std::uint32_t taskIndex, std::uint32_t laneWorkerIndex, const TaskResult& result, TimePoint start) noexcept;
}
