#pragma once

#include "Shaders/RegisteredShaderListModel.h"
#include "Shaders/ShaderArtifactInspection.h"

#include <array>
#include <functional>
#include <string>

class UsedShadersPanel final
{
public:
	using RecookHandler = std::function<void(std::string)>;

	using CommandHandler = std::function<void()>;

	void SetOpen(bool open) noexcept { m_isOpen = open; }

	bool IsOpen() const noexcept { return m_isOpen; }

	void SetGenerationProvider(RegisteredShaderListModel::GenerationProvider provider);
	void SetReloadHandler(CommandHandler handler);
	void SetRecookAllHandler(CommandHandler handler);
	void SetRecookHandler(RecookHandler handler);
	void BuildUI(bool disableInteraction);

private:
	static void DrawTextArtifact(const char* childId, const std::string& text) noexcept;

	void EnsureRows();
	void DrawToolbar(bool disableInteraction);
	void DrawTable(bool disableInteraction);
	void DrawSelectedShaderArtifacts();
	const RegisteredShaderRow* GetSelectedRow() const noexcept;
	bool MatchesFilter(const RegisteredShaderRow& row) const noexcept;

	RegisteredShaderListModel m_model;
	CommandHandler m_reloadHandler;
	CommandHandler m_recookAllHandler;
	RecookHandler m_recookHandler;
	ShaderArtifactInspection m_artifactInspection;
	std::array<char, 160> m_filterBuffer{};
	std::string m_selectedShaderId;
	bool m_hasRows = false;
	bool m_isOpen = false;
};
