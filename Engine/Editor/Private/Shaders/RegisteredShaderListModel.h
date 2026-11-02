#pragma once

#include "Renderer/Public/Diagnostics/ShaderDiagnostics.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

struct RegisteredShaderRow final
{
	std::string ShaderName;
	std::uint64_t ShaderTypeId = 0;
	std::string SourcePath;
	std::string EntryPoint;
	std::string Stage;
	std::size_t ParameterCount = 0;
	std::uint64_t RuntimeGeneration = 0;
	std::filesystem::path ExportDirectory;
};

class RegisteredShaderListModel final
{
public:
	using DiagnosticsProvider = std::function<ShaderDiagnosticsSnapshot()>;

	void SetDiagnosticsProvider(DiagnosticsProvider provider);
	void Refresh();

	const std::vector<RegisteredShaderRow>& GetRows() const noexcept { return m_rows; }

private:
	DiagnosticsProvider m_diagnosticsProvider;
	std::vector<RegisteredShaderRow> m_rows;
};
