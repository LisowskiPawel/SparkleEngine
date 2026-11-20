#include "PCH.h"
#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerDisplay.h"

#include "Core/Public/Math/MathUtils.h"
#include "Frame/Graph/RenderFrameGraphResources.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerResources.h"
#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerShader.h"
#include "Passes/Lighting/ReferencePathTracer/ReferencePathTracerWork.h"

void AddReferencePathTracerDisplayPass(
    FrameGraphBuilder& builder,
    RenderViewportExtent extent,
    const ReferencePathTracerGraphResources& graphResources,
    const ReferencePathTracerWork& work,
    const RenderFrameGraphResources& resources)
{
	auto& parameters = builder.AllocParameters<ReferencePathTracerDisplayCS>();
	parameters->WorkingMean = builder.CreateSRV(graphResources.WorkingMean);
	parameters->WorkingM2 = builder.CreateSRV(graphResources.WorkingM2);
	parameters->CommittedMean = builder.CreateUAV(graphResources.CommittedMean);
	parameters->CommittedM2 = builder.CreateUAV(graphResources.CommittedM2);
	parameters->SceneColor = builder.CreateUAV(resources.Transient.Scene.SceneColor);

	parameters->PriorSampleCount = work.PriorSampleCount;
	parameters->WorkFlags = work.WorkFlags;

	const std::uint32_t groupCountX = MathUtils::DivideRoundUp(extent.Width, 8u);
	const std::uint32_t groupCountY = MathUtils::DivideRoundUp(extent.Height, 8u);

	builder.Dispatch<ReferencePathTracerDisplayCS>("ReferencePathTracer.CommittedDisplay", parameters, ComputeDispatchDesc{groupCountX, groupCountY, 1u});
}
