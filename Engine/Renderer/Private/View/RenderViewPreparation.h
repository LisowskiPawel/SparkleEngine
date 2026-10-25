#pragma once

#include "View/RenderViewVisibility.h"
#include "Tasks/Public/TaskGraph.h"
#include "Tasks/Public/TaskTypes.h"
#include "View/MeshInstanceBatchBuilder.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class TaskExecutionContext;
class TaskExecutor;
struct PreparedRenderScene;
class RenderViewState;
struct RenderView;

class RenderViewPreparation final
{
public:
	explicit RenderViewPreparation(TaskExecutor& taskExecutor) noexcept;
	void Prepare(const PreparedRenderScene& preparedScene, RenderView& view, RenderViewState& viewState);

private:
	void BeginPreparation(const PreparedRenderScene& preparedScene, RenderView& view);
	bool ExecuteVisibility();
	void BuildMeshBatches(const PreparedRenderScene& preparedScene, RenderView& view);
	void ReleaseInputViews() noexcept;
	void EnsureGraph(std::size_t primitiveCount);

	TaskExecutor* m_taskExecutor = nullptr;
	RenderViewVisibilityRun m_run;
	MeshInstanceBatchBuilder m_batchBuilder;
	MeshInstanceBatchBuildResult m_batchResult;
	CompiledTaskGraph m_graph;
	std::uint32_t m_capacity = 0u;
};
