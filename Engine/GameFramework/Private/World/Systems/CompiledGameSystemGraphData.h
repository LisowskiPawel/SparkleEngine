#pragma once

#include "World/Systems/GameSystemGraph.h"
#include "Tasks/Public/TaskGraph.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ECS
{
	struct CompiledGameSystem final
	{
		std::string Name;
		ParallelForPolicy RangePolicy;
		std::uint32_t (*GetItemCount)(GameWorldSystemExecution&) = nullptr;
		bool (*ExecuteRange)(GameWorldSystemExecution&, std::uint32_t begin, std::uint32_t end) = nullptr;
	};

	struct GameSystemWave final
	{
		std::vector<std::uint32_t> Systems;
		// This key describes the ranges captured by the single retained graph, not mutable world truth.
		std::vector<std::uint32_t> ItemCounts;
		CompiledTaskGraph Tasks;
	};

	struct CompiledGameSystemGraphData final
	{
		std::vector<CompiledGameSystem> Systems;
		std::vector<GameSystemWave> Waves;
		GameSystemGraphError Error;
	};
}
