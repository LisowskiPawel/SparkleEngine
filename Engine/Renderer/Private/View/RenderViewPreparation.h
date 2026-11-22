#pragma once

#include "View/RenderViewVisibility.h"
#include "View/MeshInstanceBatchBuilder.h"

#include <cstddef>
#include <cstdint>

struct enkiTaskScheduler;
struct PreparedRenderScene;
class RenderViewState;
struct RenderView;

class RenderViewPreparation final
{
public:
	explicit RenderViewPreparation(enkiTaskScheduler* scheduler) noexcept;
	void Prepare(const PreparedRenderScene& preparedScene, RenderView& view, RenderViewState& viewState);

private:
	void BeginPreparation(const PreparedRenderScene& preparedScene, RenderView& view);
	void ExecuteVisibility();
	void BuildMeshBatches(const PreparedRenderScene& preparedScene, RenderView& view);
	void ReleaseInputViews() noexcept;

	enkiTaskScheduler* m_scheduler = nullptr;
	RenderViewVisibilityRun m_run;
	MeshInstanceBatchBuilder m_batchBuilder;
	MeshInstanceBatchBuildResult m_batchResult;
};
