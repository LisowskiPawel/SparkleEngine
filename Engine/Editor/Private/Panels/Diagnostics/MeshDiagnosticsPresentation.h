#pragma once

#include "Renderer/Public/Meshes/MeshDiagnostics.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace MeshDiagnosticsPresentation
{
	std::string FormatRuntimeId(std::uintptr_t id);
	std::string FormatAssetId(std::uint64_t id);
	std::string FormatMeshDisplayName(const MeshDiagnosticsRow& row);
	std::string FormatMeshSourcePath(const MeshDiagnosticsRow& row);
	std::optional<std::string> FindAuthoredSourcePath(const MeshDiagnosticsRow& row);
	std::string FormatMemorySummary(const MeshDiagnosticsRow& row);
	std::string FormatMaterial(const MeshDiagnosticsRow& row);
	std::string FormatBoundsExtent(const MeshDiagnosticsBounds& bounds);
	std::string FormatBoundsPoint(const std::array<float, 3>& point);
	std::size_t CountResidentMeshes(const MeshDiagnosticsSnapshot& snapshot) noexcept;
	std::uint64_t SumCpuBytes(const MeshDiagnosticsSnapshot& snapshot) noexcept;
	std::uint64_t SumGpuBytes(const MeshDiagnosticsSnapshot& snapshot) noexcept;
}
