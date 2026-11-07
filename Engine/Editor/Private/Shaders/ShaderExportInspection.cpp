#include "PCH.h"
#include "Shaders/ShaderExportInspection.h"

#include <fstream>
#include <sstream>
#include <system_error>

static bool IsShaderExportDirectory(const std::filesystem::path& directory)
{
	if (directory.empty())
	{
		return false;
	}
	std::error_code error;
	return std::filesystem::is_regular_file(directory / "compile-request.json", error) && !error && std::filesystem::is_regular_file(directory.parent_path() / "shader-export.json", error) && !error;
}

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

void ShaderExportInspection::InspectExport(const std::filesystem::path& directory)
{
	if (m_directory == directory)
	{
		return;
	}

	m_directory = directory;
	m_texts.reset();
	if (!IsShaderExportDirectory(directory))
	{
		return;
	}

	Texts& texts = m_texts.emplace();
	texts.Source = ReadTextFileOrMessage(m_directory / "preprocessed-source.hlsl");
	texts.Reflection = ReadTextFileOrMessage(m_directory / "reflection.json");
	texts.Disassembly = ReadTextFileOrMessage(m_directory / "disassembly.txt");
	texts.ParameterMatch = ReadTextFileOrMessage(m_directory / "parameter-struct-match.json");
	texts.CompileRequest = ReadTextFileOrMessage(m_directory / "compile-request.json");
}

void ShaderExportInspection::Invalidate() noexcept
{
	m_directory.clear();
	m_texts.reset();
}
