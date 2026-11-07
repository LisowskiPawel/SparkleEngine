#include "PCH.h"

#include "Shaders/RegisteredShaderListModel.h"
#include "Shaders/ShaderExportDiscovery.h"

#include "RHI/Public/Shaders/Authoring/GlobalShader.h"
#include "RHI/Public/Shaders/ShaderStage.h"

#include <utility>
#include <algorithm>

void RegisteredShaderListModel::SetDiagnosticsProvider(DiagnosticsProvider provider)
{
	m_diagnosticsProvider = std::move(provider);
}

void RegisteredShaderListModel::Refresh()
{
	m_rows.clear();
	const ShaderDiagnosticsSnapshot diagnostics = m_diagnosticsProvider ? m_diagnosticsProvider() : ShaderDiagnosticsSnapshot{};
	const std::vector<std::filesystem::path> exportDirectories = FindShaderExportDirectories(diagnostics);
	for (const ShaderRegistrationDesc& shader : GlobalShaderRegistry::GetRegistrations())
	{
		const ShaderParameterStructDescriptor parameters = shader.BuildParameterStructDescriptor != nullptr ? shader.BuildParameterStructDescriptor() : ShaderParameterStructDescriptor{};

		RegisteredShaderRow row;
		row.ShaderName = std::string(shader.ShaderName);
		row.ShaderTypeId = shader.TypeId;
		row.SourcePath = std::string(shader.SourcePath);
		row.EntryPoint = std::string(shader.EntryPoint);
		row.Stage = GetShaderStagePrefix(shader.Stage);
		row.ParameterCount = parameters.Fields.size();
		row.RuntimeGeneration = diagnostics.Generation;
		const auto active = std::ranges::find(diagnostics.Entries, shader.TypeId, &ShaderDiagnosticsEntry::ShaderType);
		if (active != diagnostics.Entries.end())
		{
			row.ExportDirectory = exportDirectories[static_cast<std::size_t>(active - diagnostics.Entries.begin())];
		}
		m_rows.push_back(std::move(row));
	}
}
