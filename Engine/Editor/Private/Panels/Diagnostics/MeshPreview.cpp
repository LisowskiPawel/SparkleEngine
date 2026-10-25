#include "PCH.h"
#include "Panels/Diagnostics/MeshPreview.h"

#include <imgui.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

const std::array<const char*, 3> MeshPreview::Modes = {"Wireframe", "Solid", "Wire + Solid"};
const float MeshPreview::Pi = 3.1415926535f;

static constexpr std::size_t MaxPreviewTriangles = 20000;

struct PreviewBounds final
{
	MeshPreviewVertex Min;
	MeshPreviewVertex Max;
	bool IsValid = false;
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
	if (geometry.Vertices.empty())
	{
		return bounds;
	}

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
	bounds.IsValid = true;
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

static void DrawSolidTriangles(ImDrawList& drawList, const MeshPreviewGeometry& geometry, const PreviewProjection& projection, std::size_t triangleCount, bool gpuResident, bool drawWire)
{
	const ImDrawListFlags previousFlags = drawList.Flags;
	if (!drawWire)
	{
		drawList.Flags &= ~ImDrawListFlags_AntiAliasedFill;
	}

	const ImU32 color = gpuResident ? (drawWire ? IM_COL32(89, 142, 199, 84) : IM_COL32(89, 142, 199, 230)) : (drawWire ? IM_COL32(128, 128, 128, 72) : IM_COL32(128, 128, 128, 220));
	ForEachProjectedTriangle(geometry, projection, triangleCount, [&](const std::array<ImVec2, 3>& triangle) { drawList.AddTriangleFilled(triangle[0], triangle[1], triangle[2], color); });
	drawList.Flags = previousFlags;
}

static void DrawWireTriangles(ImDrawList& drawList, const MeshPreviewGeometry& geometry, const PreviewProjection& projection, std::size_t triangleCount, bool gpuResident)
{
	const ImU32 color = gpuResident ? IM_COL32(176, 212, 255, 220) : IM_COL32(184, 184, 184, 210);

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

void MeshPreview::Draw(const MeshPreviewGeometry& geometry, bool gpuResident, float yaw, float pitch, int modeIndex)
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
	if (!geometry.IsValid())
	{
		drawList.AddText(ImVec2(canvasMin.x + 12.0f, canvasMin.y + 12.0f), IM_COL32(160, 166, 176, 255), "Preview geometry is unavailable.");
		ImGui::EndChild();
		return;
	}

	const PreviewBounds bounds = ComputePreviewBounds(geometry);
	if (!bounds.IsValid || canvasSize.x <= 1.0f || canvasSize.y <= 1.0f)
	{
		ImGui::EndChild();
		return;
	}

	const PreviewProjection projection = BuildProjection(geometry, bounds, canvasMin, canvasSize, yaw, pitch);
	const bool drawSolid = modeIndex == 1 || modeIndex == 2;
	const bool drawWire = modeIndex == 0 || modeIndex == 2;
	const std::size_t triangleCount = geometry.Indices.size() / 3;
	const std::size_t previewTriangleCount = (std::min) (triangleCount, MaxPreviewTriangles);
	if (drawSolid)
	{
		DrawSolidTriangles(drawList, geometry, projection, previewTriangleCount, gpuResident, drawWire);
	}
	if (drawWire)
	{
		DrawWireTriangles(drawList, geometry, projection, previewTriangleCount, gpuResident);
	}
	if (triangleCount > previewTriangleCount)
	{
		drawList.AddText(ImVec2(canvasMin.x + 12.0f, canvasMin.y + 12.0f), IM_COL32(192, 196, 204, 255), "Preview truncated for large mesh.");
	}
	ImGui::EndChild();
}
