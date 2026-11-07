#pragma once

#include "RayTracing/Effects/Shadows/RayTracedShadowCVars.h"
#include "Scene/GpuScene/RenderSceneGpuBindings.h"
#include "Scene/Preparation/PreparedRenderScene.h"

template <typename TParameterInstance> void BindRayTracedShadowParameters(const PreparedRenderScene& scene, TParameterInstance& parameters)
{
	const auto& rayTracing = scene.gpuBindings->RayTracing;
	const bool bHasInstances = rayTracing.InstanceCount != 0u;
	parameters->RayTracedDirectionalShadowsEnabled = bHasInstances ? 1u : 0u;
	parameters->RayTracedLocalLightShadowsEnabled = bHasInstances ? 1u : 0u;
	parameters->RayTracedShadowNormalBias = bHasInstances ? CVarRayTracedShadowNormalBias.Get() : 0.0f;
	parameters->RayTracedShadowMaxDistance = bHasInstances ? CVarRayTracedShadowMaxDistance.Get() : 0.0f;
}
