#include "../../../PCH.h"
#include "Passes/Lighting/Sky/Sky.h"

#include "Frame/RenderFrame.h"
#include "Core/Public/Math/MathUtils.h"
#include "Frame/Graph/RenderFrameGraphResources.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"
#include "Passes/Lighting/Sky/SkyShader.h"
#include "ShaderData/SceneShaderParameters.h"

ConsoleVariable<bool> CVarSkyEnabled("r.Sky.Enabled", true, "Render the sky and evaluate its environment illumination.");

void AddSkyPass(FrameGraphBuilder& builder, const RenderFrame& frame, RenderViewportExtent sceneExtent, const RenderFrameGraphResources& resources)
{
	if (!CVarSkyEnabled.Get())
	{
		return;
	}
	auto& parameters = builder.AllocParameters<SkyCS>();
	parameters->SceneColor = builder.CreateUAV(resources.Transient.Scene.SceneColor);
	parameters->SceneDepth = builder.CreateSRV(resources.Transient.Scene.SceneDepth);
	BindSceneShaderParameters(builder, frame, parameters, resources);

	builder.Dispatch<SkyCS>(parameters, ComputeDispatchDesc{MathUtils::DivideRoundUp(sceneExtent.Width, 8u), MathUtils::DivideRoundUp(sceneExtent.Height, 8u), 1u});
}
