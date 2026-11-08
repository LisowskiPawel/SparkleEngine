#include "PCH.h"
#include "Shaders/ShaderExportInspection.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileSetPublication.h"

#include <fstream>
#include <sstream>
#include <system_error>
#include <utility>

static bool IsShaderExportDirectory(const std::filesystem::path& directory, const Files::FileSetReadScope& publication)
{
	const auto request = directory / "compile-request.json";
	const auto manifest = directory.parent_path() / "shader-export.json";
	const bool bRequestPublished = publication.Contains(request);
	const bool bManifestPublished = publication.Contains(manifest);
	if (!bRequestPublished || !bManifestPublished)
	{
		return false;
	}
	std::error_code error;
	const bool bHasRequest = std::filesystem::is_regular_file(request, error) && !error;
	if (!bHasRequest)
	{
		return false;
	}
	return std::filesystem::is_regular_file(manifest, error) && !error;
}

static std::string ReadTextFileOrMessage(const std::filesystem::path& path, const Files::FileSetReadScope& publication)
{
	if (!publication.Contains(path))
	{
		return "Artifact is outside the accepted publication: " + path.generic_string();
	}
	std::ifstream input(path, std::ios::binary);
	if (!input.is_open())
	{
		return "Artifact not found: " + path.generic_string();
	}

	std::ostringstream contents;
	contents << input.rdbuf();
	return contents.str();
}

static ShaderExportInspection::Texts ReadShaderExportTexts(const std::filesystem::path& directory, const Files::FileSetReadScope& publication)
{
	return {
	    .Source = ReadTextFileOrMessage(directory / "preprocessed-source.hlsl", publication),
	    .Reflection = ReadTextFileOrMessage(directory / "reflection.json", publication),
	    .Disassembly = ReadTextFileOrMessage(directory / "disassembly.txt", publication),
	    .ParameterMatch = ReadTextFileOrMessage(directory / "parameter-struct-match.json", publication),
	    .CompileRequest = ReadTextFileOrMessage(directory / "compile-request.json", publication)};
}

void ShaderExportInspection::InspectExport(const std::filesystem::path& directory)
{
	if (m_directory == directory)
	{
		return;
	}

	Invalidate();
	if (directory.empty())
	{
		return;
	}

	try
	{
		const Files::FileSetReadScope publication(directory.parent_path().parent_path());
		if (!IsShaderExportDirectory(directory, publication))
		{
			m_directory = directory;
			return;
		}
		Texts texts = ReadShaderExportTexts(directory, publication);
		m_directory = directory;
		m_texts = std::move(texts);
	}
	catch (const Diagnostics::Error&)
	{
		// Busy or interrupted publications are not snapshots; retry on the next draw.
	}
}

void ShaderExportInspection::Invalidate() noexcept
{
	m_directory.clear();
	m_texts.reset();
}
