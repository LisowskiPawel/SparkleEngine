#pragma once

#include "Renderer/Public/Resources/Textures/TextureDiagnostics.h"

#include <array>
#include <functional>
#include <string>

class UsedTexturesPanel final
{
public:
	using DiagnosticsProvider = std::function<TextureDiagnosticsSnapshot()>;

	void SetOpen(bool bOpen) noexcept { m_bIsOpen = bOpen; }

	void SetDiagnosticsProvider(DiagnosticsProvider provider);
	void BuildUI(bool bDisableInteraction);

private:
	void RefreshSnapshot();
	void DrawToolbar();
	void DrawTextureTable(bool bDisableInteraction);
	void DrawSelectedTextureInspector(bool bDisableInteraction);
	void DrawPreview(const TextureDiagnosticsRow& row) const;
	void DrawSelectedTextureDetails(const TextureDiagnosticsRow& row) const;
	const TextureDiagnosticsRow* GetSelectedRow() const noexcept;
	bool MatchesFilter(const TextureDiagnosticsRow& row) const noexcept;

	DiagnosticsProvider m_diagnosticsProvider;
	TextureDiagnosticsSnapshot m_snapshot;
	std::array<char, 160> m_filterBuffer{};
	std::string m_selectedKey;
	bool m_bIsOpen = false;
};
