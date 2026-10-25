#include "PCH.h"
#include "Shaders/ShaderArtifactInspection.h"

#include "Shaders/RegisteredShaderListModel.h"

#include <fstream>
#include <sstream>

static std::string ReadTextFileOrMessage(const std::filesystem::path& path)
{
	std::ifstream input(path, std::ios::binary);
	if (!input.is_open())
	{
		return "Artifact not found: " + path.generic_string();
	}

	std::ostringstream contents;
	contents << input.rdbuf();
	return contents.str();
}

void ShaderArtifactInspection::Select(const RegisteredShaderRow* selectedRow)
{
	if (selectedRow == nullptr)
	{
		m_loadedShaderId.clear();
		m_directory.clear();
		m_texts = {};
		return;
	}

	if (m_loadedShaderId == selectedRow->ShaderId)
	{
		return;
	}

	m_loadedShaderId = selectedRow->ShaderId;
	m_directory = RegisteredShaderListModel::HasDebugArtifacts(selectedRow->ArtifactDirectory) ? selectedRow->ArtifactDirectory : std::filesystem::path{};
	if (m_directory.empty())
	{
		m_texts = {};
		return;
	}

	m_texts.Source = ReadTextFileOrMessage(m_directory / "preprocessed-source.hlsl");
	m_texts.Reflection = ReadTextFileOrMessage(m_directory / "reflection.json");
	m_texts.Disassembly = ReadTextFileOrMessage(m_directory / "disassembly.txt");
	m_texts.ParameterMatch = ReadTextFileOrMessage(m_directory / "parameter-struct-match.json");
	m_texts.CompileRequest = ReadTextFileOrMessage(m_directory / "compile-request.json");
}

void ShaderArtifactInspection::Invalidate() noexcept
{
	m_loadedShaderId.clear();
}
