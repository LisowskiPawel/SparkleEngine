#include "PCH.h"
#include "Passes/GBuffer/LinearizeDeviceZ.h"

#include "Frame/RenderFrame.h"
#include "Core/Public/Math/MathUtils.h"
#include "Frame/Graph/RenderFrameGraphResources.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "Passes/GBuffer/SceneDepthShader.h"
#include "View/RenderView.h"

void AddLinearizeDeviceZPass(FrameGraphBuilder& builder, const RenderFrame& frame, RenderViewportExtent sceneExtent, const RenderFrameGraphResources& resources)
{
	auto& parameters = builder.AllocParameters<SceneDepthCS>();
	parameters->GBufferDeviceZ = builder.CreateSRV(resources.Transient.GBuffer.DeviceZ);
	parameters->SceneDepth = builder.CreateUAV(resources.Transient.Scene.SceneDepth);
	parameters->ViewCamera = frame.View.cameraUniform;

	const std::uint32_t groupCountX = MathUtils::DivideRoundUp(sceneExtent.Width, 8u);
	const std::uint32_t groupCountY = MathUtils::DivideRoundUp(sceneExtent.Height, 8u);

	builder.Dispatch<SceneDepthCS>(parameters, ComputeDispatchDesc{groupCountX, groupCountY, 1u}, EFrameGraphQueuePreference::AsyncCompute);
}
