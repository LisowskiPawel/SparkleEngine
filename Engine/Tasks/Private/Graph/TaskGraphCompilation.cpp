#include "TaskGraphBuilderState.h"

#include <functional>
#include <queue>
#include <vector>

class TaskGraphBuilder::State::Compilation final
{
public:
	explicit Compilation(const State& builder) noexcept :
	    m_builder(builder)
	{
	}

	std::shared_ptr<TaskGraphStorage> Build() const
	{
		auto graph = std::make_shared<TaskGraphStorage>();
		CopyBuilderState(*graph);
		if (graph->Error)
		{
			return graph;
		}

		const std::size_t taskCount = graph->Nodes.size();
		std::vector<std::vector<std::uint32_t>> adjacency(taskCount * 2u);
		if (!BuildDependencyRelations(*graph, adjacency))
		{
			return graph;
		}

		RejectCycles(*graph, adjacency);
		return graph;
	}

private:
	static std::uint32_t BodyEvent(std::uint32_t taskIndex) noexcept { return taskIndex * 2u; }

	static std::uint32_t CompletionEvent(std::uint32_t taskIndex) noexcept { return BodyEvent(taskIndex) + 1u; }

	static bool HasCycle(const std::vector<std::vector<std::uint32_t>>& adjacency)
	{
		std::vector<std::uint32_t> incoming(adjacency.size(), 0);
		for (const auto& successors : adjacency)
		{
			for (const std::uint32_t successor : successors)
			{
				++incoming[successor];
			}
		}

		std::priority_queue<std::uint32_t, std::vector<std::uint32_t>, std::greater<>> ready;
		for (std::uint32_t index = 0; index < incoming.size(); ++index)
		{
			if (incoming[index] == 0)
			{
				ready.push(index);
			}
		}

		std::uint32_t visited = 0;
		while (!ready.empty())
		{
			const std::uint32_t index = ready.top();
			ready.pop();
			++visited;

			for (const std::uint32_t successor : adjacency[index])
			{
				if (--incoming[successor] == 0)
				{
					ready.push(successor);
				}
			}
		}

		return visited != adjacency.size();
	}

	void CopyBuilderState(TaskGraphStorage& graph) const
	{
		graph.Error = m_builder.Error;
		graph.BuilderIdentity = m_builder.BuilderIdentity;
		graph.BuilderGeneration = m_builder.BuilderGeneration;
		graph.EdgeCount = m_builder.EdgeCount;
		graph.Nodes = m_builder.Nodes;
	}

	bool BuildDependencyRelations(TaskGraphStorage& graph, std::vector<std::vector<std::uint32_t>>& adjacency) const
	{
		for (std::uint32_t taskIndex = 0; taskIndex < graph.Nodes.size(); ++taskIndex)
		{
			const TaskGraphNode& node = graph.Nodes[taskIndex];
			const std::uint32_t bodyEvent = BodyEvent(taskIndex);
			const std::uint32_t completionEvent = CompletionEvent(taskIndex);
			// Body completion precedes logical completion, which also waits for nested children.
			adjacency[bodyEvent].push_back(completionEvent);
			for (const std::uint32_t prerequisiteIndex : node.Prerequisites)
			{
				if (!ValidatePrerequisiteLane(graph, taskIndex, prerequisiteIndex))
				{
					return false;
				}
				const std::uint32_t prerequisiteCompletionEvent = CompletionEvent(prerequisiteIndex);
				adjacency[prerequisiteCompletionEvent].push_back(bodyEvent);
			}

			if (!ValidateNestedLane(graph, taskIndex))
			{
				return false;
			}
			if (node.Parent.has_value())
			{
				const std::uint32_t parentBodyEvent = BodyEvent(*node.Parent);
				const std::uint32_t parentCompletionEvent = CompletionEvent(*node.Parent);
				adjacency[parentBodyEvent].push_back(bodyEvent);
				adjacency[completionEvent].push_back(parentCompletionEvent);
			}
		}
		return true;
	}

	bool ValidatePrerequisiteLane(TaskGraphStorage& graph, std::uint32_t taskIndex, std::uint32_t prerequisiteIndex) const
	{
		const TaskGraphNode& node = graph.Nodes[taskIndex];
		const TaskGraphNode& prerequisite = graph.Nodes[prerequisiteIndex];
		if (node.Desc.Lane != TaskLane::FrameCritical || prerequisite.Desc.Lane == TaskLane::FrameCritical)
		{
			return true;
		}

		graph.Error = State::CreateError(TaskGraphErrorCode::InvalidLaneDependency, "A FrameCritical task cannot depend on Background or BlockingIo work.");
		return false;
	}

	bool ValidateNestedLane(TaskGraphStorage& graph, std::uint32_t taskIndex) const
	{
		const TaskGraphNode& node = graph.Nodes[taskIndex];
		if (!node.Parent.has_value())
		{
			return true;
		}

		const TaskGraphNode& parent = graph.Nodes[*node.Parent];
		const bool bFrameCriticalTask = node.Desc.Lane == TaskLane::FrameCritical;
		const bool bFrameCriticalParent = parent.Desc.Lane == TaskLane::FrameCritical;
		if (bFrameCriticalTask == bFrameCriticalParent)
		{
			return true;
		}

		graph.Error = State::CreateError(TaskGraphErrorCode::InvalidLaneDependency, "FrameCritical parent and nested task completion must remain in the FrameCritical lane.");
		return false;
	}

	void RejectCycles(TaskGraphStorage& graph, const std::vector<std::vector<std::uint32_t>>& adjacency) const
	{
		if (!HasCycle(adjacency))
		{
			return;
		}

		graph.Error = State::CreateError(TaskGraphErrorCode::Cycle, "Task graph contains a dependency or nested-completion cycle.");
	}

	const State& m_builder;
};

std::shared_ptr<TaskGraphStorage> TaskGraphBuilder::State::Compile() const
{
	return Compilation(*this).Build();
}
