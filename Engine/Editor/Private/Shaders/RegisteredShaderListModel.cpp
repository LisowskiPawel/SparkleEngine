#include "PCH.h"

#include "Shaders/RegisteredShaderListModel.h"
#include "Shaders/ShaderExportInspection.h"

#include "RHI/Public/Shaders/Authoring/GlobalShader.h"
#include "RHI/Public/Shaders/ShaderStage.h"

#include <utility>

void RegisteredShaderListModel::SetGenerationProvider(GenerationProvider provider)
{
	m_generationProvider = std::move(provider);
}

void RegisteredShaderListModel::Refresh()
{
	m_rows.clear();
	const std::uint64_t generation = m_generationProvider ? m_generationProvider() : 0;
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
		row.RuntimeGeneration = generation;
		row.ExportDirectory = ShaderExportInspection::FindExportDirectory(row.ShaderName);
		m_rows.push_back(std::move(row));
	}
}
