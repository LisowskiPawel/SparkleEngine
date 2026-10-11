#include "../../PCH.h"
#include "Passes/Lighting/RealTimePathTracerPasses.h"

#include "Frame/RenderFrame.h"
#include "Frame/Graph/RenderFrameGraphSettings.h"
#include "Passes/GBuffer/GBufferPasses.h"
#include "Passes/Lighting/LightingComposite.h"
#include "Passes/Lighting/RealTimePathTracerProducts.h"
#include "Passes/Lighting/Restir/RestirLightingPasses.h"
#include "Passes/Lighting/Sky/Sky.h"
#include "Frame/Graph/RenderFrameGraphResources.h"

void AddRealTimePathTracerPasses(
    FrameGraphBuilder& builder,
    const RenderFrame& frame,
    const RenderFrameGraphSettings& settings,
    RenderRayTracingScene& rayTracingScene,
    GpuMeshCache& gpuMeshCache,
    RenderFrameGraphResources& resources)
{
	AddGBufferPasses(builder, frame, settings.RenderExtent, gpuMeshCache, rayTracingScene, resources);

	AddRestirLightingPasses(builder, frame, settings.RenderExtent, rayTracingScene, resources);
	AddLightingCompositePasses(builder, settings.RenderExtent, resources);
	AddSkyPass(builder, frame, settings.RenderExtent, resources);

	PublishRealTimePathTracerProducts(resources);
}
