#pragma once

#include "Scene/GpuScene/RenderGpuScenePayloads.h"

class GpuMeshCache;
struct PreparedRenderScene;

void BuildRenderGpuRayTracingPayloads(const PreparedRenderScene& preparedScene, const GpuMeshCache& meshes, RenderGpuRayTracingPayloads& payloads);
