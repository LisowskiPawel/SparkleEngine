#include "PCH.h"

#include "Panels/Diagnostics/MeshDiagnosticsPresentation.h"

#include "Panels/Diagnostics/PanelDiagnosticsFormatting.h"
#include "Panels/Diagnostics/MeshDiagnosticMetadataCatalog.h"

#include <format>
#include <optional>
#include <string>

namespace MeshDiagnosticsPresentation
{
	std::string FormatRuntimeId(std::uintptr_t id)
	{
		return std::format("0x{:016X}", static_cast<std::uint64_t>(id));
	}

	std::string FormatAssetId(std::uint64_t id)
	{
		return std::format("0x{:016X}", id);
	}

	std::string FormatMeshDisplayName(const MeshDiagnosticsRow& row)
	{
		if (const std::optional<MeshDiagnosticMetadata> metadata = FindMeshDiagnosticMetadata(row))
		{
			if (!metadata->DisplayName.empty())
			{
				return metadata->DisplayName;
			}
		}

		return std::format("Mesh {} verts / {} tris", row.VertexCount, row.TriangleCount);
	}

	std::string FormatMeshSourcePath(const MeshDiagnosticsRow& row)
	{
		if (const std::optional<MeshDiagnosticMetadata> metadata = FindMeshDiagnosticMetadata(row))
		{
			if (!metadata->SourcePath.empty())
			{
				return metadata->SourcePath;
			}
		}
		return row.MeshAssetId != 0 ? FormatAssetId(row.MeshAssetId) : std::string("unknown");
	}

	std::optional<std::string> FindAuthoredSourcePath(const MeshDiagnosticsRow& row)
	{
		const std::optional<MeshDiagnosticMetadata> metadata = FindMeshDiagnosticMetadata(row);
		return metadata.has_value() && !metadata->SourcePath.empty() ? std::optional<std::string>(metadata->SourcePath) : std::nullopt;
	}

	std::string FormatMemorySummary(const MeshDiagnosticsRow& row)
	{
		return std::format("GPU {} / CPU {}", PanelDiagnosticsFormatting::FormatByteSize(row.EstimatedGpuByteSize), PanelDiagnosticsFormatting::FormatByteSize(row.EstimatedCpuByteSize));
	}

	std::string FormatMaterial(const MeshDiagnosticsRow& row)
	{
		return row.HasMaterial ? std::format("slot {}", row.FirstMaterialSlot) : std::string("none");
	}

	std::string FormatBoundsExtent(const MeshDiagnosticsBounds& bounds)
	{
		if (!bounds.IsValid)
		{
			return "unknown";
		}

		return std::format("{:.2f} x {:.2f} x {:.2f}", bounds.Max[0] - bounds.Min[0], bounds.Max[1] - bounds.Min[1], bounds.Max[2] - bounds.Min[2]);
	}

	std::string FormatBoundsPoint(const std::array<float, 3>& point)
	{
		return std::format("{:.3f}, {:.3f}, {:.3f}", point[0], point[1], point[2]);
	}

	std::size_t CountResidentMeshes(const MeshDiagnosticsSnapshot& snapshot) noexcept
	{
		std::size_t residentCount = 0;
		for (const MeshDiagnosticsRow& row : snapshot.Rows)
		{
			if (row.GpuResident)
			{
				++residentCount;
			}
		}
		return residentCount;
	}

	std::uint64_t SumCpuBytes(const MeshDiagnosticsSnapshot& snapshot) noexcept
	{
		std::uint64_t totalBytes = 0;
		for (const MeshDiagnosticsRow& row : snapshot.Rows)
		{
			totalBytes += row.EstimatedCpuByteSize;
		}
		return totalBytes;
	}

	std::uint64_t SumGpuBytes(const MeshDiagnosticsSnapshot& snapshot) noexcept
	{
		std::uint64_t totalBytes = 0;
		for (const MeshDiagnosticsRow& row : snapshot.Rows)
		{
			totalBytes += row.EstimatedGpuByteSize;
		}
		return totalBytes;
	}

}
