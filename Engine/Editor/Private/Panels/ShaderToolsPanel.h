#pragma once

#include "Shaders/RegisteredShaderListModel.h"
#include "Shaders/ShaderExportInspection.h"
#include "Core/Public/Console/ConsoleCommandRegistry.h"

#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

class ShaderToolsPanel final
{
public:
	using CommandHandler = std::function<void()>;

	void SetOpen(bool open) noexcept { m_isOpen = open; }

	bool IsOpen() const noexcept { return m_isOpen; }

	void SetGenerationProvider(RegisteredShaderListModel::GenerationProvider provider);
	void SetReloadHandler(CommandHandler handler);
	void SetCompileHandler(std::function<ConsoleCommandResult(bool)> handler);
	void SetCompileStatus(bool busy, std::string_view message);
	ConsoleCommandResult RequestCompile(bool generateSymbols);
	void BuildUI(bool disableInteraction);

private:
	static void DrawExportText(const char* childId, const std::string& text) noexcept;

	void EnsureRows();
	void DrawToolbar(bool disableInteraction);
	void DrawTable(bool disableInteraction);
	void DrawSelectedShaderExport();
	const RegisteredShaderRow* GetSelectedRow() const noexcept;
	bool MatchesFilter(const RegisteredShaderRow& row) const noexcept;

	RegisteredShaderListModel m_model;
	CommandHandler m_reloadHandler;
	std::function<ConsoleCommandResult(bool)> m_compileHandler;
	std::string m_compileStatus;
	ShaderExportInspection m_exportInspection;
	std::array<char, 160> m_filterBuffer{};
	std::uint64_t m_selectedShaderTypeId = 0;
	bool m_hasRows = false;
	bool m_isOpen = false;
	bool m_compiling = false;
	bool m_generateSymbols = false;
};
