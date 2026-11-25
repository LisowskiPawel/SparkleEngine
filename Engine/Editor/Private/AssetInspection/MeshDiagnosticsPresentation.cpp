#include "PCH.h"

#include "AssetInspection/MeshDiagnosticsPresentation.h"

#include "AssetInspection/PanelDiagnosticsFormatting.h"
#include "AssetInspection/MeshDiagnosticMetadataCatalog.h"
#include "Core/Public/Formatting/HexFormat.h"

#include <format>
#include <string>

namespace MeshDiagnosticsPresentation
{
	Summary BuildSummary(const MeshDiagnosticsSnapshot& snapshot) noexcept
	{
		Summary summary;
		for (const MeshDiagnosticsRow& row : snapshot.Rows)
		{
			summary.GpuBytes += row.EstimatedGpuByteSize;
			summary.CpuBytes += row.EstimatedCpuByteSize;
			summary.ResidentCount += row.bGpuResident ? 1u : 0u;
		}
		return summary;
	}

	std::string FormatMeshDisplayName(const MeshDiagnosticsRow& row, const MeshDiagnosticMetadata* metadata)
	{
		if (metadata != nullptr && !metadata->DisplayName.empty())
		{
			return metadata->DisplayName;
		}
		return std::format("Mesh {} verts / {} tris", row.VertexCount, row.TriangleCount);
	}

	std::string FormatMeshSourcePath(const MeshDiagnosticsRow& row, const MeshDiagnosticMetadata* metadata)
	{
		if (metadata != nullptr && !metadata->SourcePath.empty())
		{
			return metadata->SourcePath;
		}
		return row.MeshAssetId != 0 ? Formatting::FormatPrefixedHexUInt64(row.MeshAssetId) : std::string("unknown");
	}

	std::string FormatMemorySummary(const MeshDiagnosticsRow& row)
	{
		const std::string gpuMemory = PanelDiagnosticsFormatting::FormatByteSize(row.EstimatedGpuByteSize);
		const std::string cpuMemory = PanelDiagnosticsFormatting::FormatByteSize(row.EstimatedCpuByteSize);
		return std::format("GPU {} / CPU {}", gpuMemory, cpuMemory);
	}

	std::string FormatMaterial(const MeshDiagnosticsRow& row)
	{
		return row.bHasMaterial ? std::format("slot {}", row.FirstMaterialSlot) : std::string("none");
	}

	std::string FormatBoundsExtent(const MeshDiagnosticsBounds& bounds)
	{
		if (!bounds.bIsValid)
		{
			return "unknown";
		}

		return std::format("{:.2f} x {:.2f} x {:.2f}", bounds.Max[0] - bounds.Min[0], bounds.Max[1] - bounds.Min[1], bounds.Max[2] - bounds.Min[2]);
	}

	std::string FormatBoundsPoint(const std::array<float, 3>& point)
	{
		return std::format("{:.3f}, {:.3f}, {:.3f}", point[0], point[1], point[2]);
	}
}
