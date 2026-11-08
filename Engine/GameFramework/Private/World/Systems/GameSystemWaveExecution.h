#pragma once

#include <span>

class TaskExecutor;

namespace ECS
{
	struct GameSystemWave;
	struct CompiledGameSystem;
	struct GameSystemGraphError;
	class GameWorldSystemExecution;

	bool ExecuteGameSystemWave(GameSystemWave& wave, std::span<const CompiledGameSystem> declarations, TaskExecutor& executor, GameWorldSystemExecution& systems, GameSystemGraphError& error);
}
