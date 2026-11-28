#pragma once

#include "Meshes/GpuMeshHandle.h"
#include "Scene/Geometry/RenderMeshClassification.h"

#include <cstdint>

enum class RenderMaterialClassification : std::uint8_t
{
	Opaque,
	AlphaTested,
	Transparent,
	Rejected
};

struct MeshInstanceBatch final
{
	GpuMeshHandle Mesh;
	std::uint32_t materialSlot = 0;
	std::uint32_t firstInstance = 0;
	std::uint32_t instanceCount = 0;
	RenderMeshKind meshKind = RenderMeshKind::Static;
	RenderMaterialClassification materialClassification = RenderMaterialClassification::Rejected;
};
