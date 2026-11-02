#include "PCH.h"
#include "Shaders/ShaderExportInspection.h"

#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Json/JsonReader.h"
#include "Core/Public/Diagnostics/Error.h"

#include <algorithm>
#include <charconv>

#include <fstream>
#include <sstream>
#include <string_view>
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

static bool ReadExportIdentity(std::string_view text, std::string_view key, std::uint64_t& value)
{
	std::string hex;
	if (!Json::TryReadStringProperty(text, key, hex))
	{
		return false;
	}
	std::string_view digits(hex);
	if (digits.starts_with("0x") || digits.starts_with("0X"))
	{
		digits.remove_prefix(2);
	}
	const auto parsed = std::from_chars(digits.data(), digits.data() + digits.size(), value, 16);
	return parsed.ec == std::errc{} && parsed.ptr == digits.data() + digits.size();
}

static bool ReadExportShader(std::string_view text, ShaderTarget target, ShaderDiagnosticsEntry& shader)
{
	std::string exportTarget;

	return ReadExportIdentity(text, "shaderTypeId", shader.ShaderType) && ReadExportIdentity(text, "compileInputHash", shader.CompileInputHash)
	    && Json::TryReadStringProperty(text, "target", exportTarget) && exportTarget == GetShaderTargetName(target) && Json::TryReadStringProperty(text, "backend", shader.Backend);
}

static std::vector<std::filesystem::path> FindMatchingShaderExports(const std::filesystem::path& root, const ShaderDiagnosticsSnapshot& diagnostics)
{
	std::vector<std::filesystem::path> selected(diagnostics.Entries.size());
	try
	{
		const Files::FileSetReadScope publication(root);
		std::error_code error;
		for (std::filesystem::directory_iterator cook(root, error), end; cook != end && !error; cook.increment(error))
		{
			if (!cook->is_directory(error) || error || !publication.Contains(cook->path()))
			{
				error.clear();
				continue;
			}
			std::string manifest;
			std::string failure;
			std::vector<std::string_view> jobs;
			if (!Files::TryReadAllText(cook->path() / "shader-export.json", manifest, failure) || !Json::TryReadArrayProperty(manifest, "jobs", jobs))
			{
				continue;
			}
			for (const std::string_view job : jobs)
			{
				std::string name;
				if (!Json::TryReadStringProperty("{\"path\":" + std::string(job) + "}", "path", name))
				{
					continue;
				}
				const std::filesystem::path relative(name);
				if (relative.has_root_path() || relative.filename() != "compile-request.json" || std::any_of(relative.begin(), relative.end(), [](const auto& component) { return component == ".."; }))
				{
					continue;
				}
				const auto request = cook->path() / relative;
				std::string text;
				ShaderDiagnosticsEntry shader;
				if (!publication.Contains(request) || !Files::TryReadAllText(request, text, failure) || !ReadExportShader(text, diagnostics.Target, shader))
				{
					continue;
				}
				for (std::size_t index = 0; index < diagnostics.Entries.size(); ++index)
				{
					const ShaderDiagnosticsEntry& active = diagnostics.Entries[index];
					if (active.ShaderType == shader.ShaderType && active.CompileInputHash == shader.CompileInputHash && active.Backend == shader.Backend
					    && (selected[index].empty() || request.parent_path().native() > selected[index].native()))
					{
						selected[index] = request.parent_path();
					}
				}
			}
		}
	}
	catch (const Diagnostics::Error&)
	{
		return std::vector<std::filesystem::path>(diagnostics.Entries.size());
	}
	return selected;
}

std::vector<std::filesystem::path> ShaderExportInspection::FindExportDirectories(const ShaderDiagnosticsSnapshot& diagnostics)
{
	if (diagnostics.Entries.empty())
	{
		return {};
	}
	const auto root = Filesystem::ResolveArtifactRootPath(Filesystem::GetWorkspaceRootPath()) / "symbols" / "shaders" / Filesystem::GetProjectPath().filename();
	return FindMatchingShaderExports(root, diagnostics);
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
