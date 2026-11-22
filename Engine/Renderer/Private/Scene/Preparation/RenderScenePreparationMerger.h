#pragma once

struct RenderScenePreparationRun;

namespace RenderScenePreparationMerger
{
	void Merge(RenderScenePreparationRun& run);
	void BuildRayTracingPlan(RenderScenePreparationRun& run);
	void PublishSceneOutputs(RenderScenePreparationRun& run);

}
