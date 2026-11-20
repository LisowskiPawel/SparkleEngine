#include "../../../PCH.h"
#include "Passes/PostProcessing/Exposure/ExposureMeteringPasses.h"

#include "Core/Public/Math/MathUtils.h"
#include "Frame/Graph/RenderFrameGraphResources.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "Passes/PostProcessing/Exposure/ExposureHistogramPassDefinitions.h"
#include "Passes/PostProcessing/Exposure/ExposureMomentPassDefinitions.h"

#include <algorithm>

ExposureMomentTexture AddExposureHistogramPasses(FrameGraphBuilder& builder, RenderViewportExtent sceneExtent, const RenderFrameGraphResources& resources)
{
	const auto histogram = CreateExposureHistogramBuffer(builder);
	const auto moments = CreateExposureMomentTexture(builder, "ExposureHistogramMoments", 0u, 1u, 1u);
	AddExposureHistogramClearPass(builder, histogram);
	AddExposureHistogramBuildPass(builder, resources.Transient.Scene.SceneColor, sceneExtent, histogram);
	AddExposureHistogramResolvePass(builder, histogram, moments);
	return moments;
}

ExposureMomentTexture AddExposureDownsamplePasses(FrameGraphBuilder& builder, RenderViewportExtent sceneExtent, const RenderFrameGraphResources& resources)
{
	const std::uint32_t initialHalfWidth = MathUtils::DivideRoundUp(sceneExtent.Width, 2u);
	const std::uint32_t initialHalfHeight = MathUtils::DivideRoundUp(sceneExtent.Height, 2u);
	const std::uint32_t width = (std::max) (initialHalfWidth, 1u);
	const std::uint32_t height = (std::max) (initialHalfHeight, 1u);
	ExposureMomentTexture current = CreateExposureMomentTexture(builder, "ExposureDownsampleMoments", 0u, width, height);
	AddExposureSceneDownsamplePass(builder, resources.Transient.Scene.SceneColor, current);

	std::uint32_t level = 1u;
	while (current.Width > 1u || current.Height > 1u)
	{
		const std::uint32_t halfWidth = MathUtils::DivideRoundUp(current.Width, 2u);
		const std::uint32_t halfHeight = MathUtils::DivideRoundUp(current.Height, 2u);
		const std::uint32_t nextWidth = (std::max) (halfWidth, 1u);
		const std::uint32_t nextHeight = (std::max) (halfHeight, 1u);
		ExposureMomentTexture next = CreateExposureMomentTexture(builder, "ExposureDownsampleMoments", level, nextWidth, nextHeight);
		AddExposureTextureDownsamplePass(builder, current, next);
		current = next;
		++level;
	}

	return current;
}
