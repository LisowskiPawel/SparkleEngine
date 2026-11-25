#pragma once

#include "Renderer/Public/Meshes/MeshDiagnostics.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

struct MeshDiagnosticMetadata;

namespace MeshDiagnosticsPresentation
{
	struct Summary final
	{
		std::size_t ResidentCount = 0;
		std::uint64_t CpuBytes = 0;
		std::uint64_t GpuBytes = 0;
	};

	Summary BuildSummary(const MeshDiagnosticsSnapshot& snapshot) noexcept;
	std::string FormatMeshDisplayName(const MeshDiagnosticsRow& row, const MeshDiagnosticMetadata* metadata);
	std::string FormatMeshSourcePath(const MeshDiagnosticsRow& row, const MeshDiagnosticMetadata* metadata);
	std::string FormatMemorySummary(const MeshDiagnosticsRow& row);
	std::string FormatMaterial(const MeshDiagnosticsRow& row);
	std::string FormatBoundsExtent(const MeshDiagnosticsBounds& bounds);
	std::string FormatBoundsPoint(const std::array<float, 3>& point);
}
