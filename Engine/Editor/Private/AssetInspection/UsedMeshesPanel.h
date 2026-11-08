#pragma once

#include "AssetInspection/MeshDiagnosticMetadataCatalog.h"
#include "Renderer/Public/Meshes/MeshDiagnostics.h"
#include "Renderer/Public/Diagnostics/MeshPreviewGeometry.h"

#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

class UsedMeshesPanel final
{
public:
	using DiagnosticsProvider = std::function<MeshDiagnosticsSnapshot()>;

	using PreviewGeometryProvider = std::function<MeshPreviewGeometry(std::uintptr_t)>;

	void SetOpen(bool bOpen) noexcept;

	void SetDiagnosticsProvider(DiagnosticsProvider provider);
	void SetPreviewGeometryProvider(PreviewGeometryProvider provider);
	void BuildUI(bool bDisableInteraction);

private:
	void RefreshSnapshot();
	void RefreshInspection();
	void RefreshPreviewGeometry(const MeshDiagnosticsRow& row);
	void DrawToolbar();
	void DrawMeshTable(bool bDisableInteraction);
	void DrawSelectedMeshInspector(bool bDisableInteraction);
	void DrawPreviewControls(bool bDisableInteraction);
	void DrawSelectedMeshDetails(const MeshDiagnosticsRow& row) const;
	const MeshDiagnosticsRow* GetSelectedRow() const noexcept;
	bool MatchesFilter(const MeshDiagnosticsRow& row) const;

	DiagnosticsProvider m_diagnosticsProvider;
	PreviewGeometryProvider m_previewGeometryProvider;
	MeshDiagnosticsSnapshot m_snapshot;
	MeshDiagnosticMetadataCatalog m_metadata;
	MeshPreviewGeometry m_previewGeometry;
	std::array<char, 160> m_filterBuffer{};
	std::uintptr_t m_selectedMeshRuntimeId = 0;
	std::uintptr_t m_previewGeometryMeshRuntimeId = 0;
	float m_previewYaw = 0.65f;
	float m_previewPitch = -0.35f;
	int m_previewModeIndex = 2;
	bool m_bIsOpen = false;
};
