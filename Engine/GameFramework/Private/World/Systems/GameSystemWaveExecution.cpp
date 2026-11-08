#include "PCH.h"
#include "World/Systems/GameSystemWaveExecution.h"
#include "World/Systems/CompiledGameSystemGraphData.h"
#include "World/Systems/GameWorldSystems.h"
#include "Tasks/Public/TaskExecutionContext.h"
#include "Tasks/Public/TaskExecutor.h"

#include <string>

namespace ECS
{
	static bool RefreshItemCounts(GameSystemWave& wave, std::span<const CompiledGameSystem> declarations, GameWorldSystemExecution& systems)
	{
		bool bChanged = !wave.Tasks.IsValid();
		for (std::size_t offset = 0; offset < wave.Systems.size(); ++offset)
		{
			const auto count = declarations[wave.Systems[offset]].GetItemCount(systems);
			bChanged |= count != wave.ItemCounts[offset];
			wave.ItemCounts[offset] = count;
		}
		return bChanged;
	}

	static TaskResult ExecuteSystemRange(const CompiledGameSystem& system, std::uint32_t begin, std::uint32_t end, TaskExecutionContext& context)
	{
		if (context.IsCancellationRequested())
		{
			return TaskResult::Cancelled("Game-system execution cancelled at the owner boundary.");
		}
		auto* execution = context.TryGet<GameWorldSystemExecution>();
		return execution != nullptr && system.ExecuteRange(*execution, begin, end) ? TaskResult::Success() : TaskResult::Failure("Game-system range rejected its declared access or target range.");
	}

	static CompiledTaskGraph BuildWaveTasks(const GameSystemWave& wave, std::span<const CompiledGameSystem> declarations)
	{
		TaskGraphBuilder tasks;
		for (std::size_t offset = 0; offset < wave.Systems.size(); ++offset)
		{
			const CompiledGameSystem& system = declarations[wave.Systems[offset]];
			const auto count = wave.ItemCounts[offset];
			if (count == 0)
			{
				continue;
			}

			ParallelFor(
			    tasks,
			    TaskDesc{TaskName(system.Name), TaskLane::FrameCritical},
			    count,
			    system.RangePolicy,
			    [&system](std::uint32_t begin, std::uint32_t end, TaskExecutionContext& context) { return ExecuteSystemRange(system, begin, end, context); });
		}
		return tasks.Compile();
	}

	bool ExecuteGameSystemWave(GameSystemWave& wave, std::span<const CompiledGameSystem> declarations, TaskExecutor& executor, GameWorldSystemExecution& systems, GameSystemGraphError& error)
	{
		if (RefreshItemCounts(wave, declarations, systems))
		{
			wave.Tasks = BuildWaveTasks(wave, declarations);
			if (!wave.Tasks)
			{
				error = {GameSystemGraphErrorCode::TaskGraphRejected, wave.Tasks.GetError().Message};
				return false;
			}
		}
		if (wave.Tasks.GetTaskCount() == 0)
		{
			return true;
		}

		TaskExecutionContext context(systems);
		TaskExecution result = executor.Submit(wave.Tasks, context);
		if (!result.IsValid() || result.GetStatus() != TaskExecutionStatus::Succeeded)
		{
			error = {GameSystemGraphErrorCode::ExecutionFailed, result.IsValid() ? std::string(result.GetResult().GetMessage()) : "SparkleTasks rejected the game-system execution."};
			return false;
		}
		return true;
	}
}
