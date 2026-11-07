#pragma once

#include "Shaders/RegisteredShaderListModel.h"
#include "Shaders/ShaderExportInspection.h"
#include "Core/Public/Console/ConsoleOutput.h"

#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

class ShaderToolsPanel final
{
public:
	using CommandHandler = std::function<void()>;

	void SetOpen(bool bOpen) noexcept { m_bIsOpen = bOpen; }

	bool IsOpen() const noexcept { return m_bIsOpen; }

	void SetDiagnosticsProvider(RegisteredShaderListModel::DiagnosticsProvider provider);
	void SetReloadHandler(CommandHandler handler);
	void SetCompileHandler(std::function<ConsoleCommandResult(bool)> handler);
	void SetCompileStatus(bool bBusy, std::string_view message);
	ConsoleCommandResult RequestCompile(bool bGenerateSymbols);
	void BuildUI(bool bDisableInteraction);

private:
	static void DrawExportText(const char* childId, const std::string& text) noexcept;

	void EnsureRows();
	void DrawToolbar(bool bDisableInteraction);
	void DrawTable(bool bDisableInteraction);
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
	bool m_bHasRows = false;
	bool m_bIsOpen = false;
	bool m_bCompiling = false;
	bool m_bGenerateSymbols = false;
};
