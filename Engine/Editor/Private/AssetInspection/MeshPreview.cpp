#include "PCH.h"
#include "AssetInspection/MeshPreview.h"
#include "Icons/EditorIcon.h"
#include "Widgets/UiIconWidgets.h"

#include <imgui.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numbers>
#include <utility>

static constexpr std::size_t MaxPreviewTriangles = 20000;
static constexpr std::array<const char*, 3> PreviewModes = {"Wireframe", "Solid", "Wire + Solid"};

struct PreviewBounds final
{
	MeshPreviewVertex Min;
	MeshPreviewVertex Max;
};

struct PreviewProjection final
{
	MeshPreviewVertex Center;
	ImVec2 CanvasCenter;
	float Scale;
	float Yaw;
	float Pitch;
};

static PreviewBounds ComputePreviewBounds(const MeshPreviewGeometry& geometry) noexcept
{
	PreviewBounds bounds;
	bounds.Min.X = bounds.Min.Y = bounds.Min.Z = (std::numeric_limits<float>::max)();
	bounds.Max.X = bounds.Max.Y = bounds.Max.Z = (std::numeric_limits<float>::lowest)();
	for (const MeshPreviewVertex& vertex : geometry.Vertices)
	{
		bounds.Min.X = (std::min) (bounds.Min.X, vertex.X);
		bounds.Min.Y = (std::min) (bounds.Min.Y, vertex.Y);
		bounds.Min.Z = (std::min) (bounds.Min.Z, vertex.Z);
		bounds.Max.X = (std::max) (bounds.Max.X, vertex.X);
		bounds.Max.Y = (std::max) (bounds.Max.Y, vertex.Y);
		bounds.Max.Z = (std::max) (bounds.Max.Z, vertex.Z);
	}
	return bounds;
}

static MeshPreviewVertex RotatePreviewVertex(const MeshPreviewVertex& vertex, const MeshPreviewVertex& center, float yaw, float pitch) noexcept
{
	const float x = vertex.X - center.X;
	const float y = vertex.Y - center.Y;
	const float z = vertex.Z - center.Z;

	const float cosYaw = std::cos(yaw);
	const float sinYaw = std::sin(yaw);
	const float yawX = (x * cosYaw) + (z * sinYaw);
	const float yawZ = (-x * sinYaw) + (z * cosYaw);

	const float cosPitch = std::cos(pitch);
	const float sinPitch = std::sin(pitch);
	return MeshPreviewVertex{yawX, (y * cosPitch) - (yawZ * sinPitch), (y * sinPitch) + (yawZ * cosPitch)};
}

static PreviewProjection BuildProjection(const MeshPreviewGeometry& geometry, const PreviewBounds& bounds, ImVec2 canvasMin, ImVec2 canvasSize, float yaw, float pitch) noexcept
{
	const MeshPreviewVertex center{(bounds.Min.X + bounds.Max.X) * 0.5f, (bounds.Min.Y + bounds.Max.Y) * 0.5f, (bounds.Min.Z + bounds.Max.Z) * 0.5f};
	float projectedMinX = (std::numeric_limits<float>::max)();
	float projectedMinY = (std::numeric_limits<float>::max)();
	float projectedMaxX = (std::numeric_limits<float>::lowest)();
	float projectedMaxY = (std::numeric_limits<float>::lowest)();
	for (const MeshPreviewVertex& vertex : geometry.Vertices)
	{
		const MeshPreviewVertex rotated = RotatePreviewVertex(vertex, center, yaw, pitch);
		projectedMinX = (std::min) (projectedMinX, rotated.X);
		projectedMinY = (std::min) (projectedMinY, rotated.Y);
		projectedMaxX = (std::max) (projectedMaxX, rotated.X);
		projectedMaxY = (std::max) (projectedMaxY, rotated.Y);
	}

	const float projectedWidth = (std::max) (projectedMaxX - projectedMinX, 0.001f);
	const float projectedHeight = (std::max) (projectedMaxY - projectedMinY, 0.001f);

	return PreviewProjection{
	    .Center = center,
	    .CanvasCenter = ImVec2(canvasMin.x + canvasSize.x * 0.5f, canvasMin.y + canvasSize.y * 0.5f),
	    .Scale = 0.86f * (std::min) (canvasSize.x / projectedWidth, canvasSize.y / projectedHeight),
	    .Yaw = yaw,
	    .Pitch = pitch};
}

static ImVec2 ProjectVertex(const MeshPreviewVertex& vertex, const PreviewProjection& projection) noexcept
{
	const MeshPreviewVertex rotated = RotatePreviewVertex(vertex, projection.Center, projection.Yaw, projection.Pitch);
	return ImVec2(projection.CanvasCenter.x + rotated.X * projection.Scale, projection.CanvasCenter.y - rotated.Y * projection.Scale);
}

template <typename Callback> static void ForEachProjectedTriangle(const MeshPreviewGeometry& geometry, const PreviewProjection& projection, std::size_t triangleCount, Callback callback)
{
	for (std::size_t triangleIndex = 0; triangleIndex < triangleCount; ++triangleIndex)
	{
		const std::uint32_t index0 = geometry.Indices[triangleIndex * 3 + 0];
		const std::uint32_t index1 = geometry.Indices[triangleIndex * 3 + 1];
		const std::uint32_t index2 = geometry.Indices[triangleIndex * 3 + 2];
		if (index0 >= geometry.Vertices.size() || index1 >= geometry.Vertices.size() || index2 >= geometry.Vertices.size())
		{
			continue;
		}

		callback(std::array{ProjectVertex(geometry.Vertices[index0], projection), ProjectVertex(geometry.Vertices[index1], projection), ProjectVertex(geometry.Vertices[index2], projection)});
	}
}

static void DrawSolidTriangles(ImDrawList& drawList, const MeshPreviewGeometry& geometry, const PreviewProjection& projection, std::size_t triangleCount, bool bGpuResident, bool bDrawWire)
{
	const ImDrawListFlags previousFlags = drawList.Flags;
	if (!bDrawWire)
	{
		drawList.Flags &= ~ImDrawListFlags_AntiAliasedFill;
	}

	const ImU32 color = bGpuResident ? (bDrawWire ? IM_COL32(89, 142, 199, 84) : IM_COL32(89, 142, 199, 230)) : (bDrawWire ? IM_COL32(128, 128, 128, 72) : IM_COL32(128, 128, 128, 220));
	ForEachProjectedTriangle(geometry, projection, triangleCount, [&](const std::array<ImVec2, 3>& triangle) { drawList.AddTriangleFilled(triangle[0], triangle[1], triangle[2], color); });
	drawList.Flags = previousFlags;
}

static void DrawWireTriangles(ImDrawList& drawList, const MeshPreviewGeometry& geometry, const PreviewProjection& projection, std::size_t triangleCount, bool bGpuResident)
{
	const ImU32 color = bGpuResident ? IM_COL32(176, 212, 255, 220) : IM_COL32(184, 184, 184, 210);

	ForEachProjectedTriangle(
	    geometry,
	    projection,
	    triangleCount,
	    [&](const std::array<ImVec2, 3>& triangle)
	    {
		    drawList.AddLine(triangle[0], triangle[1], color, 1.0f);
		    drawList.AddLine(triangle[1], triangle[2], color, 1.0f);
		    drawList.AddLine(triangle[2], triangle[0], color, 1.0f);
	    });
}

void MeshPreview::SetGeometryProvider(GeometryProvider provider)
{
	m_geometryProvider = std::move(provider);
	Invalidate();
}

void MeshPreview::Invalidate() noexcept
{
	m_geometry = {};
	m_meshRuntimeId = 0;
}

void MeshPreview::RefreshGeometry(std::uintptr_t meshRuntimeId)
{
	if (m_meshRuntimeId == meshRuntimeId)
	{
		return;
	}

	MeshPreviewGeometry geometry = m_geometryProvider ? m_geometryProvider(meshRuntimeId) : MeshPreviewGeometry{};
	m_geometry = std::move(geometry);
	m_meshRuntimeId = meshRuntimeId;
}

void MeshPreview::BuildUI(std::uintptr_t meshRuntimeId, bool bGpuResident, bool bDisableInteraction)
{
	RefreshGeometry(meshRuntimeId);
	DrawControls(bDisableInteraction);
	DrawGeometry(bGpuResident);
}

void MeshPreview::DrawControls(bool bDisableInteraction)
{
	ImGui::BeginDisabled(bDisableInteraction);
	ImGui::SetNextItemWidth(150.0f);
	int modeIndex = static_cast<int>(m_mode);
	if (ImGui::Combo("Mode", &modeIndex, PreviewModes.data(), static_cast<int>(PreviewModes.size())))
	{
		m_mode = static_cast<MeshPreviewMode>(modeIndex);
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);
	ImGui::SliderFloat("Yaw", &m_yawRadians, -std::numbers::pi_v<float>, std::numbers::pi_v<float>, "%.2f");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);
	ImGui::SliderFloat("Pitch", &m_pitchRadians, -1.45f, 1.45f, "%.2f");
	ImGui::SameLine();
	if (EditorWidgets::DrawEditorIconButton(EditorIcons::EditorIcon::Reset, "##ResetMeshPreview", "Reset preview view"))
	{
		m_yawRadians = 0.65f;
		m_pitchRadians = -0.35f;
	}
	ImGui::EndDisabled();
}

void MeshPreview::DrawGeometry(bool bGpuResident) const
{
	ImVec2 availableRegion = ImGui::GetContentRegionAvail();
	availableRegion.y = (std::max) (128.0f, availableRegion.y - 214.0f);
	ImGui::BeginChild("##MeshPreview", ImVec2(0.0f, availableRegion.y), ImGuiChildFlags_Borders);
	const ImVec2 canvasMin = ImGui::GetCursorScreenPos();
	const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
	ImGui::InvisibleButton("##MeshPreviewCanvas", canvasSize);
	ImDrawList& drawList = *ImGui::GetWindowDrawList();
	const ImVec2 canvasMax(canvasMin.x + canvasSize.x, canvasMin.y + canvasSize.y);
	drawList.AddRectFilled(canvasMin, canvasMax, IM_COL32(18, 20, 24, 255));
	drawList.AddRect(canvasMin, canvasMax, IM_COL32(68, 74, 84, 255));
	if (!m_geometry.IsValid())
	{
		drawList.AddText(ImVec2(canvasMin.x + 12.0f, canvasMin.y + 12.0f), IM_COL32(160, 166, 176, 255), "Preview geometry is unavailable.");
		ImGui::EndChild();
		return;
	}

	if (canvasSize.x <= 1.0f || canvasSize.y <= 1.0f)
	{
		ImGui::EndChild();
		return;
	}

	const PreviewBounds bounds = ComputePreviewBounds(m_geometry);
	const PreviewProjection projection = BuildProjection(m_geometry, bounds, canvasMin, canvasSize, m_yawRadians, m_pitchRadians);
	const bool bDrawSolid = m_mode != MeshPreviewMode::Wireframe;
	const bool bDrawWire = m_mode != MeshPreviewMode::Solid;
	const std::size_t triangleCount = m_geometry.Indices.size() / 3;
	const std::size_t previewTriangleCount = (std::min) (triangleCount, MaxPreviewTriangles);
	if (bDrawSolid)
	{
		DrawSolidTriangles(drawList, m_geometry, projection, previewTriangleCount, bGpuResident, bDrawWire);
	}
	if (bDrawWire)
	{
		DrawWireTriangles(drawList, m_geometry, projection, previewTriangleCount, bGpuResident);
	}
	if (triangleCount > previewTriangleCount)
	{
		drawList.AddText(ImVec2(canvasMin.x + 12.0f, canvasMin.y + 12.0f), IM_COL32(192, 196, 204, 255), "Preview truncated for large mesh.");
	}
	ImGui::EndChild();
}
