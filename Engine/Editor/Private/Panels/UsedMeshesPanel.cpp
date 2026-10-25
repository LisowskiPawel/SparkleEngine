#include "PCH.h"

#include "Panels/UsedMeshesPanel.h"

#include "Panels/Diagnostics/MeshDiagnosticsPresentation.h"
#include "Panels/Diagnostics/MeshPreview.h"
#include "Panels/Diagnostics/PanelDiagnosticsFormatting.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Util/UiUtil.h"

#include <algorithm>
#include <string>
#include <utility>

#include <imgui.h>

void UsedMeshesPanel::SetDiagnosticsProvider(DiagnosticsProvider provider)
{
	m_diagnosticsProvider = std::move(provider);
}

void UsedMeshesPanel::SetPreviewGeometryProvider(PreviewGeometryProvider provider)
{
	m_previewGeometryProvider = std::move(provider);
	m_previewGeometry = {};
	m_previewGeometryMeshRuntimeId = 0;
}

void UsedMeshesPanel::BuildUI(bool disableInteraction)
{
	if (!m_isOpen)
	{
		return;
	}

	RefreshSnapshot();
	ImGui::SetNextWindowSize(ImVec2(1280.0f, 760.0f), ImGuiCond_FirstUseEver);
	const std::string windowTitle = UiUtil::MakeIconLabel(UiUtil::EditorIcon::StaticMesh, "Mesh Tools") + "##Mesh Tools";
	if (!ImGui::Begin(windowTitle.c_str(), &m_isOpen))
	{
		ImGui::End();
		return;
	}

	DrawToolbar();
	ImGui::Separator();

	const ImGuiTableFlags layoutFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings;
	if (ImGui::BeginTable("##MeshInspectorLayout", 2, layoutFlags, ImGui::GetContentRegionAvail()))
	{
		ImGui::TableSetupColumn("Meshes", ImGuiTableColumnFlags_WidthStretch, 0.62f);
		ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthStretch, 0.38f);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::BeginChild("##MeshTablePane", ImVec2(0.0f, 0.0f), ImGuiChildFlags_None);
		DrawMeshTable(disableInteraction);
		ImGui::EndChild();
		ImGui::TableSetColumnIndex(1);
		ImGui::BeginChild("##MeshPreviewPane", ImVec2(0.0f, 0.0f), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		DrawSelectedMeshInspector(disableInteraction);
		ImGui::EndChild();
		ImGui::EndTable();
	}

	ImGui::End();
}

void UsedMeshesPanel::RefreshSnapshot()
{
	if (m_diagnosticsProvider)
	{
		m_snapshot = m_diagnosticsProvider();
	}
}

void UsedMeshesPanel::RefreshPreviewGeometry(const MeshDiagnosticsRow& row)
{
	if (m_previewGeometryMeshRuntimeId == row.MeshRuntimeId)
	{
		return;
	}

	m_previewGeometryMeshRuntimeId = row.MeshRuntimeId;
	m_previewGeometry = m_previewGeometryProvider ? m_previewGeometryProvider(row.MeshRuntimeId) : MeshPreviewGeometry{};
}

void UsedMeshesPanel::DrawToolbar()
{
	ImGui::SetNextItemWidth(300.0f);
	const std::string filterHint = UiUtil::MakeIconLabel(UiUtil::EditorIcon::Search, "Filter mesh/source/id/material");
	ImGui::InputTextWithHint("##UsedMeshesFilter", filterHint.c_str(), m_filterBuffer.data(), m_filterBuffer.size());
	ImGui::SameLine();

	ImGui::TextDisabled(
	    "%zu mesh(es), %zu resident, instances %u, groups %u (authored %u, shared %u), batches %u (authored %u, preserved %u, auto %u, "
	    "single %u), saved %u",
	    m_snapshot.Rows.size(),
	    MeshDiagnosticsPresentation::CountResidentMeshes(m_snapshot),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.RenderableInstanceCount),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.RuntimeInstanceGroupCount),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.RuntimeAuthoredGroupCount),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.RuntimeSharedMeshReferenceGroupCount),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.MeshBatchCount),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.AuthoredBatchCount),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.PreservedGroupBatchCount),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.AutoBatchCount),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.SingleInstanceBatchCount),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.EstimatedGBufferDrawCallsSaved));

	ImGui::TextDisabled(
	    "instances/batch %u..%u, rejected %u (gpu %u, material %u, group %u, incompatible %u), GPU %s, CPU %s estimated",
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.MinInstancesPerBatch),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.MaxInstancesPerBatch),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.RejectedCandidateCount),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.RejectedMissingGpuMeshCount),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.RejectedInvalidMaterialCount),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.RejectedInvalidInstanceGroupCount),
	    static_cast<unsigned int>(m_snapshot.GeometryInstancing.RejectedIncompatibleGroupCount),
	    PanelDiagnosticsFormatting::FormatByteSize(MeshDiagnosticsPresentation::SumGpuBytes(m_snapshot)).c_str(),
	    PanelDiagnosticsFormatting::FormatByteSize(MeshDiagnosticsPresentation::SumCpuBytes(m_snapshot)).c_str());
}

void UsedMeshesPanel::DrawMeshTable(bool disableInteraction)
{
	const ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
	if (!ImGui::BeginTable("##UsedMeshesTable", 9, tableFlags, ImVec2(0.0f, 0.0f)))
	{
		return;
	}

	ImGui::TableSetupScrollFreeze(0, 1);
	ImGui::TableSetupColumn("Mesh");
	ImGui::TableSetupColumn("Source");
	ImGui::TableSetupColumn("Uploaded");
	ImGui::TableSetupColumn("Vertices");
	ImGui::TableSetupColumn("Indices");
	ImGui::TableSetupColumn("Triangles");
	ImGui::TableSetupColumn("Memory");
	ImGui::TableSetupColumn("Material");
	ImGui::TableSetupColumn("Bounds");
	ImGui::TableHeadersRow();

	ImGui::BeginDisabled(disableInteraction);
	for (const MeshDiagnosticsRow& row : m_snapshot.Rows)
	{
		if (!MatchesFilter(row))
		{
			continue;
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		const bool selected = row.MeshRuntimeId == m_selectedMeshRuntimeId;
		const std::string displayName = MeshDiagnosticsPresentation::FormatMeshDisplayName(row);
		if (ImGui::Selectable(displayName.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns))
		{
			m_selectedMeshRuntimeId = row.MeshRuntimeId;
			m_previewGeometryMeshRuntimeId = 0;
			m_previewGeometry = {};
		}
		ImGui::TableNextColumn();
		const std::string sourcePath = MeshDiagnosticsPresentation::FormatMeshSourcePath(row);
		ImGui::TextUnformatted(sourcePath.c_str());
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(row.GpuResident ? "yes" : "no");
		ImGui::TableNextColumn();
		ImGui::Text("%u", static_cast<unsigned int>(row.VertexCount));
		ImGui::TableNextColumn();
		ImGui::Text("%u", static_cast<unsigned int>(row.IndexCount));
		ImGui::TableNextColumn();
		ImGui::Text("%u", static_cast<unsigned int>(row.TriangleCount));
		ImGui::TableNextColumn();
		const std::string memory = MeshDiagnosticsPresentation::FormatMemorySummary(row);
		ImGui::TextUnformatted(memory.c_str());
		ImGui::TableNextColumn();
		const std::string material = MeshDiagnosticsPresentation::FormatMaterial(row);
		ImGui::TextUnformatted(material.c_str());
		ImGui::TableNextColumn();
		const std::string bounds = MeshDiagnosticsPresentation::FormatBoundsExtent(row.Bounds);
		ImGui::TextUnformatted(bounds.c_str());
	}
	ImGui::EndDisabled();
	ImGui::EndTable();
}

void UsedMeshesPanel::DrawSelectedMeshInspector(bool disableInteraction)
{
	const MeshDiagnosticsRow* selectedRow = GetSelectedRow();
	if (selectedRow == nullptr)
	{
		ImGui::TextDisabled("Select a mesh to inspect memory, geometry, residency, bounds, and preview.");
		return;
	}

	RefreshPreviewGeometry(*selectedRow);
	const std::string displayName = MeshDiagnosticsPresentation::FormatMeshDisplayName(*selectedRow);
	const std::string sourcePath = MeshDiagnosticsPresentation::FormatMeshSourcePath(*selectedRow);
	ImGui::TextUnformatted(displayName.c_str());
	PanelDiagnosticsFormatting::DrawWrappedDisabledText(sourcePath);
	ImGui::Separator();
	DrawPreviewControls(disableInteraction);
	MeshPreview::Draw(m_previewGeometry, selectedRow->GpuResident, m_previewYaw, m_previewPitch, m_previewModeIndex);
	ImGui::SeparatorText("Runtime Properties");
	ImGui::BeginChild("##MeshDetailsPane", ImVec2(0.0f, 0.0f), ImGuiChildFlags_None);
	DrawSelectedMeshDetails(*selectedRow);
	ImGui::EndChild();
}

void UsedMeshesPanel::DrawPreviewControls(bool disableInteraction)
{
	ImGui::BeginDisabled(disableInteraction);
	ImGui::SetNextItemWidth(150.0f);
	ImGui::Combo("Mode", &m_previewModeIndex, MeshPreview::Modes.data(), static_cast<int>(MeshPreview::Modes.size()));
	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);
	ImGui::SliderFloat("Yaw", &m_previewYaw, -MeshPreview::Pi, MeshPreview::Pi, "%.2f");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);
	ImGui::SliderFloat("Pitch", &m_previewPitch, -1.45f, 1.45f, "%.2f");
	ImGui::SameLine();
	if (UiUtil::DrawEditorIconButton(UiUtil::EditorIcon::Reset, "##ResetMeshPreview", "Reset preview view"))
	{
		m_previewYaw = 0.65f;
		m_previewPitch = -0.35f;
	}
	ImGui::EndDisabled();
}

void UsedMeshesPanel::DrawSelectedMeshDetails(const MeshDiagnosticsRow& row) const
{
	const std::string meshRuntimeId = MeshDiagnosticsPresentation::FormatRuntimeId(row.MeshRuntimeId);
	const std::string gpuRuntimeId = row.GpuMeshRuntimeId != 0 ? MeshDiagnosticsPresentation::FormatRuntimeId(row.GpuMeshRuntimeId) : std::string("none");
	const std::string meshAssetId = row.MeshAssetId != 0 ? MeshDiagnosticsPresentation::FormatAssetId(row.MeshAssetId) : std::string("none");
	const std::string vertices = std::to_string(row.VertexCount);
	const std::string indices = std::to_string(row.IndexCount);
	const std::string triangles = std::to_string(row.TriangleCount);
	const std::string cpuBytes = PanelDiagnosticsFormatting::FormatByteSize(row.EstimatedCpuByteSize);
	const std::string gpuBytes = PanelDiagnosticsFormatting::FormatByteSize(row.EstimatedGpuByteSize);
	const std::string instances = std::to_string(row.InstanceCount);
	const std::string visibleInstances = std::to_string(row.VisibleInstanceCount);
	const std::string vertexStride = std::to_string(row.VertexStrideBytes);
	const std::string indexStride = std::to_string(row.IndexStrideBytes);
	const std::string material = MeshDiagnosticsPresentation::FormatMaterial(row);
	const std::string boundsMin = row.Bounds.IsValid ? MeshDiagnosticsPresentation::FormatBoundsPoint(row.Bounds.Min) : std::string("unknown");
	const std::string boundsMax = row.Bounds.IsValid ? MeshDiagnosticsPresentation::FormatBoundsPoint(row.Bounds.Max) : std::string("unknown");
	const std::string boundsExtent = MeshDiagnosticsPresentation::FormatBoundsExtent(row.Bounds);

	if (const std::optional<std::string> sourcePath = MeshDiagnosticsPresentation::FindAuthoredSourcePath(row))
	{
		UiUtil::DrawKeyValueRow("Source", sourcePath->c_str());
	}
	UiUtil::DrawKeyValueRow("CPU Loaded", row.CpuLoaded ? "yes" : "no");
	UiUtil::DrawKeyValueRow("GPU Resident", row.GpuResident ? "yes" : "no");
	UiUtil::DrawKeyValueRow("Vertices", vertices.c_str());
	UiUtil::DrawKeyValueRow("Indices", indices.c_str());
	UiUtil::DrawKeyValueRow("Triangles", triangles.c_str());
	UiUtil::DrawKeyValueRow("CPU Memory", cpuBytes.c_str());
	UiUtil::DrawKeyValueRow("GPU Memory", gpuBytes.c_str());
	UiUtil::DrawKeyValueRow("Vertex Stride", vertexStride.c_str());
	UiUtil::DrawKeyValueRow("Index Stride", indexStride.c_str());
	UiUtil::DrawKeyValueRow("Instances", instances.c_str());
	UiUtil::DrawKeyValueRow("Visible", visibleInstances.c_str());
	UiUtil::DrawKeyValueRow("Material", material.c_str());
	UiUtil::DrawKeyValueRow("Bounds Min", boundsMin.c_str());
	UiUtil::DrawKeyValueRow("Bounds Max", boundsMax.c_str());
	UiUtil::DrawKeyValueRow("Bounds Extent", boundsExtent.c_str());
	UiUtil::DrawKeyValueRow("Mesh Asset ID", meshAssetId.c_str());
	UiUtil::DrawKeyValueRow("Mesh ID", meshRuntimeId.c_str());
	UiUtil::DrawKeyValueRow("GPU Mesh ID", gpuRuntimeId.c_str());
}

const MeshDiagnosticsRow* UsedMeshesPanel::GetSelectedRow() const noexcept
{
	for (const MeshDiagnosticsRow& row : m_snapshot.Rows)
	{
		if (row.MeshRuntimeId == m_selectedMeshRuntimeId)
		{
			return &row;
		}
	}
	return nullptr;
}

bool UsedMeshesPanel::MatchesFilter(const MeshDiagnosticsRow& row) const
{
	const std::string_view filter(m_filterBuffer.data());
	if (filter.empty())
	{
		return true;
	}

	const std::string displayName = MeshDiagnosticsPresentation::FormatMeshDisplayName(row);
	const std::string meshRuntimeId = MeshDiagnosticsPresentation::FormatRuntimeId(row.MeshRuntimeId);
	const std::string gpuRuntimeId = row.GpuMeshRuntimeId != 0 ? MeshDiagnosticsPresentation::FormatRuntimeId(row.GpuMeshRuntimeId) : std::string("none");
	const std::string meshAssetId = row.MeshAssetId != 0 ? MeshDiagnosticsPresentation::FormatAssetId(row.MeshAssetId) : std::string("none");
	const std::string sourcePath = MeshDiagnosticsPresentation::FormatMeshSourcePath(row);
	const std::string material = MeshDiagnosticsPresentation::FormatMaterial(row);
	const std::string memory = MeshDiagnosticsPresentation::FormatMemorySummary(row);

	return Strings::ContainsIgnoreCase(displayName, filter) || Strings::ContainsIgnoreCase(sourcePath, filter) || Strings::ContainsIgnoreCase(meshAssetId, filter)
	    || Strings::ContainsIgnoreCase(meshRuntimeId, filter) || Strings::ContainsIgnoreCase(gpuRuntimeId, filter) || Strings::ContainsIgnoreCase(material, filter)
	    || Strings::ContainsIgnoreCase(memory, filter);
}
