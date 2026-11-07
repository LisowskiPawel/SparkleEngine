#pragma once

#include "Renderer/Public/Diagnostics/MeshPreviewGeometry.h"

#include <array>

namespace MeshPreview
{
	extern const std::array<const char*, 3> Modes;
	extern const float Pi;

	void Draw(const MeshPreviewGeometry& geometry, bool gpuResident, float yaw, float pitch, int modeIndex);
}
