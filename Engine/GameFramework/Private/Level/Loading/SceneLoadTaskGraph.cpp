#include "PCH.h"

#include "Level/Loading/SceneLoadTaskGraph.h"

#include "Level/Loading/SceneLoadWorkState.h"
#include "Tasks/Public/TaskExecutionContext.h"

#include <format>

namespace Assets
{
	static constexpr std::size_t MaximumSceneAssetsInFlight = 2;

	static std::vector<TaskNodeHandle> AddAssetTasks(TaskGraphBuilder& graph, const std::shared_ptr<SceneLoadWorkState>& state)
	{
		std::vector<TaskNodeHandle> decodedNodes;
		decodedNodes.reserve(state->Assets.size());
		for (std::size_t index = 0; index < state->Assets.size(); ++index)
		{
			const TaskNodeHandle read = graph.Add(
			    TaskDesc{TaskName(std::format("Read scene asset {}", index)), TaskLane::BlockingIo},
			    [state, index](TaskExecutionContext& context) { return state->ReadAsset(index, context); });

			const TaskNodeHandle decode = graph.Add(
			    TaskDesc{TaskName(std::format("Decode scene asset {}", index)), TaskLane::Background},
			    [state, index](TaskExecutionContext& context) { return state->DecodeAsset(index, context); });

			graph.DependsOn(decode, read);
			if (index >= MaximumSceneAssetsInFlight)
			{
				graph.DependsOn(read, decodedNodes[index - MaximumSceneAssetsInFlight]);
			}
			decodedNodes.push_back(decode);
		}

		return decodedNodes;
	}

	CompiledTaskGraph BuildSceneLoadTaskGraph(const std::shared_ptr<SceneLoadWorkState>& state)
	{
		const std::size_t assetCount = state->Assets.size();
		const std::size_t throttlingEdgeCount = assetCount > MaximumSceneAssetsInFlight ? assetCount - MaximumSceneAssetsInFlight : 0;
		TaskGraphBuilder graph(
		    TaskGraphLimits{.MaximumTasks = static_cast<std::uint32_t>(assetCount * 2u + 2u), .MaximumEdges = static_cast<std::uint32_t>(assetCount * 2u + throttlingEdgeCount + 1u)});
		const auto decodedNodes = AddAssetTasks(graph, state);

		const TaskNodeHandle decoded = graph.WhenAll(TaskDesc{TaskName("Join decoded scene assets"), TaskLane::Background}, decodedNodes);

		graph.ContinueWith(decoded, TaskDesc{TaskName("Finalize scene load package"), TaskLane::Background}, [state](TaskExecutionContext& context) { return state->FinalizePackage(context); });

		return graph.Compile();
	}
}
