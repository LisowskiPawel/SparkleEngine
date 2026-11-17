#pragma once

#include "Renderer/Public/Diagnostics/MeshPreviewGeometry.h"

#include <cstdint>
#include <functional>

enum class MeshPreviewMode : std::uint8_t
{
	Wireframe,
	Solid,
	WireAndSolid
};

class MeshPreview final
{
public:
	using GeometryProvider = std::function<MeshPreviewGeometry(std::uintptr_t)>;

	void SetGeometryProvider(GeometryProvider provider);
	void Invalidate() noexcept;
	void BuildUI(std::uintptr_t meshRuntimeId, bool bGpuResident, bool bDisableInteraction);

private:
	void RefreshGeometry(std::uintptr_t meshRuntimeId);
	void DrawControls(bool bDisableInteraction);
	void DrawGeometry(bool bGpuResident) const;

	GeometryProvider m_geometryProvider;
	MeshPreviewGeometry m_geometry;
	std::uintptr_t m_meshRuntimeId = 0;
	float m_yawRadians = 0.65f;
	float m_pitchRadians = -0.35f;
	MeshPreviewMode m_mode = MeshPreviewMode::WireAndSolid;
};
