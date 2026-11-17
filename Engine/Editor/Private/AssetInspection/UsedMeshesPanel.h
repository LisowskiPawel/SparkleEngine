#pragma once

#include "AssetInspection/MeshDiagnosticMetadataCatalog.h"
#include "AssetInspection/MeshPreview.h"
#include "Renderer/Public/Meshes/MeshDiagnostics.h"

#include <array>
#include <cstdint>
#include <functional>

class UsedMeshesPanel final
{
public:
	using DiagnosticsProvider = std::function<MeshDiagnosticsSnapshot()>;

	void SetOpen(bool bOpen) noexcept;

	void SetDiagnosticsProvider(DiagnosticsProvider provider);
	void SetPreviewGeometryProvider(MeshPreview::GeometryProvider provider);
	void BuildUI(bool bDisableInteraction);

private:
	void RefreshSnapshot();
	void RefreshInspection();
	void DrawToolbar();
	void DrawMeshTable(bool bDisableInteraction);
	void DrawSelectedMeshInspector(bool bDisableInteraction);
	void DrawSelectedMeshDetails(const MeshDiagnosticsRow& row) const;
	const MeshDiagnosticsRow* GetSelectedRow() const noexcept;
	bool MatchesFilter(const MeshDiagnosticsRow& row) const;

	DiagnosticsProvider m_diagnosticsProvider;
	MeshDiagnosticsSnapshot m_snapshot;
	MeshDiagnosticMetadataCatalog m_metadata;
	MeshPreview m_preview;
	std::array<char, 160> m_filterBuffer{};
	std::uintptr_t m_selectedMeshRuntimeId = 0;
	bool m_bIsOpen = false;
};
