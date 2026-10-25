#pragma once

#include <cstdint>

struct RenderRayTracingInstance final
{
	std::uint32_t PrimitiveIndex = 0u;
	std::uint32_t GpuSceneSlot = 0u;
};
