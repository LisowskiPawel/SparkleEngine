#include "TaskGraph.h"

#include "TaskExecutionContext.h"
#include "TaskGraphBuilderState.h"
#include "TaskGraphStorage.h"

#include <algorithm>
#include <limits>
#include <utility>

static void ReserveRelationSlot(std::vector<std::uint32_t>& relation)
{
	if (relation.size() < relation.capacity())
	{
		return;
	}

	const std::size_t capacity = std::max<std::size_t>(1, relation.capacity() * 2);
	relation.reserve(capacity);
}

TaskNodeHandle::TaskNodeHandle() noexcept = default;

TaskNodeHandle::TaskNodeHandle(std::uint64_t builderIdentity, std::uint32_t builderGeneration, std::uint32_t index) noexcept :
    m_builderIdentity(builderIdentity),
    m_builderGeneration(builderGeneration),
    m_indexPlusOne(index + 1u)
{
}

bool TaskNodeHandle::operator==(const TaskNodeHandle&) const noexcept = default;

CompiledTaskGraph::CompiledTaskGraph() noexcept = default;

bool TaskGraphAccess::Decode(TaskNodeHandle handle, std::uint64_t builderIdentity, std::uint32_t builderGeneration, std::uint32_t taskCount, std::uint32_t& outIndex) noexcept
{
	if (!handle || handle.m_builderIdentity != builderIdentity || handle.m_builderGeneration != builderGeneration)
	{
		return false;
	}
	outIndex = handle.m_indexPlusOne - 1u;
	return outIndex < taskCount;
}

void TaskGraphAccess::RecordError(TaskGraphBuilder& builder, TaskGraphErrorCode code, std::string message)
{
	builder.m_state->RecordError(code, std::move(message));
}

CompiledTaskGraph::CompiledTaskGraph(std::shared_ptr<const TaskGraphStorage> data) noexcept :
    m_data(std::move(data))
{
}

bool CompiledTaskGraph::IsValid() const noexcept
{
	return m_data != nullptr && !m_data->Error;
}

const TaskGraphError& CompiledTaskGraph::GetError() const noexcept
{
	static const TaskGraphError invalidGraphError{.Code = TaskGraphErrorCode::InvalidHandle, .Message = "No compiled task graph is present."};
	return m_data != nullptr ? m_data->Error : invalidGraphError;
}

std::uint32_t CompiledTaskGraph::GetTaskCount() const noexcept
{
	return m_data != nullptr ? static_cast<std::uint32_t>(m_data->Nodes.size()) : 0;
}

std::uint32_t CompiledTaskGraph::GetEdgeCount() const noexcept
{
	return m_data != nullptr ? m_data->EdgeCount : 0;
}

TaskGraphBuilder::TaskGraphBuilder(TaskGraphLimits limits) :
    m_state(std::make_unique<State>(limits))
{
}

TaskGraphBuilder::~TaskGraphBuilder() = default;

TaskNodeHandle TaskGraphBuilder::Add(TaskDesc desc, TaskFunction function)
{
	if (m_state->Error)
	{
		return {};
	}
	if (!State::IsValidTaskLane(desc.Lane))
	{
		m_state->RecordError(TaskGraphErrorCode::InvalidTaskLane, "Task lane is not recognized.");
		return {};
	}
	if (desc.CompletionPolicy != TaskCompletionPolicy::Normal && desc.CompletionPolicy != TaskCompletionPolicy::Cleanup)
	{
		m_state->RecordError(TaskGraphErrorCode::InvalidCompletionPolicy, "Task completion policy is not recognized.");
		return {};
	}
	if (m_state->Nodes.size() >= m_state->Limits.MaximumTasks)
	{
		m_state->RecordError(TaskGraphErrorCode::TaskCapacityExceeded, "Task graph exceeded its configured task capacity.");
		return {};
	}

	const std::uint32_t index = static_cast<std::uint32_t>(m_state->Nodes.size());
	m_state->Nodes.push_back(TaskGraphNode{.Desc = std::move(desc), .Function = std::move(function)});
	return TaskNodeHandle(m_state->BuilderIdentity, m_state->BuilderGeneration, index);
}

TaskNodeHandle TaskGraphBuilder::AddNested(TaskNodeHandle parent, TaskDesc desc, TaskFunction function)
{
	if (m_state->Error)
	{
		return {};
	}
	std::uint32_t parentIndex = 0;
	if (!m_state->ResolveHandle(parent, parentIndex))
	{
		return {};
	}
	if (m_state->EdgeCount >= m_state->Limits.MaximumEdges)
	{
		m_state->RecordError(TaskGraphErrorCode::EdgeCapacityExceeded, "Task graph exceeded its configured edge capacity.");
		return {};
	}

	// Reserve the parent relation before Add can publish the child node.
	auto& children = m_state->Nodes[parentIndex].NestedChildren;
	ReserveRelationSlot(children);

	TaskNodeHandle child = Add(std::move(desc), std::move(function));
	if (!child)
	{
		return {};
	}
	const std::uint32_t childIndex = child.m_indexPlusOne - 1u;
	m_state->Nodes[childIndex].Parent = parentIndex;
	m_state->Nodes[parentIndex].NestedChildren.push_back(childIndex);
	++m_state->EdgeCount;
	return child;
}

bool TaskGraphBuilder::DependsOn(TaskNodeHandle task, TaskNodeHandle prerequisite)
{
	if (m_state->Error)
	{
		return false;
	}
	std::uint32_t taskIndex = 0;
	std::uint32_t prerequisiteIndex = 0;
	const bool bTaskValid = m_state->ResolveHandle(task, taskIndex);
	const bool bHandlesValid = bTaskValid && m_state->ResolveHandle(prerequisite, prerequisiteIndex);
	if (!bHandlesValid)
	{
		return false;
	}
	if (m_state->EdgeCount >= m_state->Limits.MaximumEdges)
	{
		m_state->RecordError(TaskGraphErrorCode::EdgeCapacityExceeded, "Task graph exceeded its configured edge capacity.");
		return false;
	}

	auto& prerequisites = m_state->Nodes[taskIndex].Prerequisites;
	// Both directions commit without allocation after either adjacency changes.
	auto& dependents = m_state->Nodes[prerequisiteIndex].Dependents;
	ReserveRelationSlot(prerequisites);
	ReserveRelationSlot(dependents);

	prerequisites.push_back(prerequisiteIndex);
	dependents.push_back(taskIndex);
	++m_state->EdgeCount;
	return true;
}

TaskNodeHandle TaskGraphBuilder::WhenAll(TaskDesc desc, std::span<const TaskNodeHandle> prerequisites)
{
	TaskNodeHandle barrier = Add(std::move(desc), [](TaskExecutionContext&) { return TaskResult::Success(); });
	for (const TaskNodeHandle prerequisite : prerequisites)
	{
		DependsOn(barrier, prerequisite);
	}
	return barrier;
}

TaskNodeHandle TaskGraphBuilder::WhenAll(std::string name, std::span<const TaskNodeHandle> prerequisites)
{
	return WhenAll(TaskDesc{.Name = std::move(name)}, prerequisites);
}

TaskNodeHandle TaskGraphBuilder::ContinueWith(TaskNodeHandle prerequisite, TaskDesc desc, TaskFunction function)
{
	TaskNodeHandle continuation = Add(std::move(desc), std::move(function));
	DependsOn(continuation, prerequisite);
	return continuation;
}

CompiledTaskGraph TaskGraphBuilder::Compile() const
{
	return CompiledTaskGraph(m_state->Compile());
}

void TaskGraphBuilder::Reset() noexcept
{
	if (m_state->Error.Code == TaskGraphErrorCode::InvalidLimits || m_state->Error.Code == TaskGraphErrorCode::GenerationExhausted)
	{
		return;
	}
	m_state->Nodes.clear();
	m_state->EdgeCount = 0;
	m_state->Error = {};
	if (m_state->BuilderGeneration == std::numeric_limits<std::uint32_t>::max())
	{
		m_state->RecordError(TaskGraphErrorCode::GenerationExhausted, "Task graph builder generation is exhausted and cannot be reused.");
		return;
	}
	++m_state->BuilderGeneration;
}

const TaskGraphError& TaskGraphBuilder::GetError() const noexcept
{
	return m_state->Error;
}
