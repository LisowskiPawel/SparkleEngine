#pragma once

#include "Meshes/GpuMesh.h"
#include "Rendering/RenderSceneDelta.h"
#include "Rendering/RenderSceneDynamicData.h"

#include <cstdint>

struct RenderPrimitive final
{
	RenderObjectId Object;
	RenderObjectStaticData Static;
	RenderObjectDynamicData Dynamic;
	GpuMeshHandle GpuMesh;
	RenderObjectStaticData PendingStatic;
	GpuMeshHandle PendingGpuMesh;
	std::uint32_t GpuSceneSlot = 0;
	bool bGpuMeshResident = false;
	bool bHasPendingStatic = false;
};
