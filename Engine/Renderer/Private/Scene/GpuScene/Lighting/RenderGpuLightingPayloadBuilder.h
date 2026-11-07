#pragma once

#include "Scene/GpuScene/RenderGpuScenePayloads.h"

struct PreparedRenderScene;

void BuildRenderGpuLightingPayloads(const PreparedRenderScene& preparedScene, RenderGpuLightingPayloads& payloads);
