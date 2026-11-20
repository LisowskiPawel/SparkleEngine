#include "PCH.h"

#include "Shaders/ShaderExportDiscovery.h"
#include "Renderer/Public/Diagnostics/ShaderDiagnostics.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Json/JsonInputAdmission.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Core/Public/Formatting/HexFormat.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <string_view>
#include <system_error>

static nlohmann::json ReadExportDocument(std::string_view text, std::initializer_list<std::string_view> fields)
{
	Json::InputAdmission admission(text, 32u * 1024u * 1024u);
	bool bJobs = false;
	std::size_t jobCount = 0;

	const auto admitValue = [&](int depth, nlohmann::json::parse_event_t event, nlohmann::json& value)
	{
		using Event = nlohmann::json::parse_event_t;
		admission.ReadEvent(depth, event);
		const bool bContainerStart = event == Event::object_start || event == Event::array_start;
		const bool bEntryStart = bContainerStart || event == Event::value;
		if (bJobs && depth == 2 && bEntryStart)
		{
			if (event != Event::value || !value.is_string())
			{
				throw Diagnostics::Error("Shader export jobs require path strings.");
			}
			if (++jobCount > 65'536)
			{
				throw Diagnostics::Error("Shader export exceeds its job limit.");
			}
		}
		if (event == Event::key)
		{
			const auto& key = value.get_ref<const std::string&>();
			const bool bConsumed = depth == 1 && std::find(fields.begin(), fields.end(), key) != fields.end();
			if (depth == 1)
			{
				bJobs = bConsumed && key == "jobs";
			}
			return bConsumed && admission.ReadKey(depth, key);
		}
		const bool bNestedObject = event == Event::object_end && depth > 0;
		const bool bNestedArray = event == Event::array_end && depth > 1;
		return !bNestedObject && !bNestedArray;
	};

	auto document = nlohmann::json::parse(text.begin(), text.end(), admitValue);

	if (!document.is_object())
	{
		throw Diagnostics::Error("Shader export requires an object root.");
	}
	return document;
}

static bool ReadExportIdentity(const nlohmann::json& document, std::string_view key, std::uint64_t& value)
{
	const auto& hex = document.at(std::string(key)).get_ref<const std::string&>();
	const bool bUnpadded = hex == Strings::TrimAsciiWhitespace(hex);
	return bUnpadded && Formatting::TryParseHexUInt64(hex, value);
}

static bool ReadExportShader(std::string_view text, ShaderTarget target, ShaderDiagnosticsEntry& shader)
{
	try
	{
		const auto document = ReadExportDocument(text, {"shaderTypeId", "compileInputHash", "target", "backend"});
		const auto& exportTarget = document.at("target").get_ref<const std::string&>();
		shader.Backend = document.at("backend").get<std::string>();
		const bool bShaderTypeRead = ReadExportIdentity(document, "shaderTypeId", shader.ShaderType);
		const bool bCompileInputRead = bShaderTypeRead && ReadExportIdentity(document, "compileInputHash", shader.CompileInputHash);
		return bCompileInputRead && exportTarget == GetShaderTargetName(target);
	}
	catch (const std::exception&)
	{
		return false;
	}
}

static bool ResolveExportRequest(const std::filesystem::path& cookDirectory, const std::string& name, std::filesystem::path& request)
{
	const std::filesystem::path relative(Strings::ToWide(name));
	const bool bInvalidRoot = relative.has_root_path() || relative.filename() != "compile-request.json";
	const bool bInvalidName = name.find(':') != std::string::npos || name.find('\0') != std::string::npos;
	const bool bTraversal = std::any_of(relative.begin(), relative.end(), [](const auto& component) { return component == ".."; });
	const bool bInvalidPath = bInvalidRoot || bInvalidName;
	if (bInvalidPath || bTraversal)
	{
		return false;
	}

	request = cookDirectory / relative;
	return true;
}

static void SelectNewestShaderExport(
    const std::filesystem::path& request,
    const ShaderDiagnosticsEntry& shader,
    const ShaderDiagnosticsSnapshot& diagnostics,
    std::vector<std::filesystem::path>& selected)
{
	for (std::size_t index = 0; index < diagnostics.Entries.size(); ++index)
	{
		const ShaderDiagnosticsEntry& active = diagnostics.Entries[index];
		const bool bSameCompile = active.ShaderType == shader.ShaderType && active.CompileInputHash == shader.CompileInputHash;
		if (!bSameCompile || active.Backend != shader.Backend)
		{
			continue;
		}

		const auto directory = request.parent_path();
		if (selected[index].empty() || directory.native() > selected[index].native())
		{
			selected[index] = directory;
		}
	}
}

static void InspectPublishedShaderRequest(
    const std::filesystem::path& request,
    const Files::FileSetReadScope& publication,
    const ShaderDiagnosticsSnapshot& diagnostics,
    std::vector<std::filesystem::path>& selected)
{
	std::string text;
	std::string failure;
	ShaderDiagnosticsEntry shader;
	const bool bRequestPublished = publication.Contains(request);
	const bool bRequestRead = bRequestPublished && Files::TryReadAllText(request, text, failure, 32u * 1024u * 1024u);
	const bool bShaderDecoded = bRequestRead && ReadExportShader(text, diagnostics.Target, shader);

	if (!bShaderDecoded)
	{
		return;
	}

	SelectNewestShaderExport(request, shader, diagnostics, selected);
}

static void InspectPublishedCook(
    const std::filesystem::path& cookDirectory,
    const Files::FileSetReadScope& publication,
    const ShaderDiagnosticsSnapshot& diagnostics,
    std::vector<std::filesystem::path>& selected)
{
	std::string manifest;
	std::string failure;
	if (!Files::TryReadAllText(cookDirectory / "shader-export.json", manifest, failure, 32u * 1024u * 1024u))
	{
		return;
	}
	try
	{
		const auto document = ReadExportDocument(manifest, {"jobs"});
		const auto& jobs = document.at("jobs");
		if (!jobs.is_array())
		{
			return;
		}
		for (const auto& job : jobs)
		{
			std::filesystem::path request;
			const auto& requestName = job.get_ref<const std::string&>();
			if (ResolveExportRequest(cookDirectory, requestName, request))
			{
				InspectPublishedShaderRequest(request, publication, diagnostics, selected);
			}
		}
	}
	catch (const std::exception&)
	{
		return;
	}
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
			const bool bDirectory = cook->is_directory(error) && !error;
			if (!bDirectory)
			{
				error.clear();
				continue;
			}
			const std::filesystem::path& directory = cook->path();
			if (!publication.Contains(directory))
			{
				continue;
			}
			InspectPublishedCook(directory, publication, diagnostics, selected);
		}
	}
	catch (const Diagnostics::Error&)
	{
		return std::vector<std::filesystem::path>(diagnostics.Entries.size());
	}
	return selected;
}

std::vector<std::filesystem::path> FindShaderExportDirectories(const ShaderDiagnosticsSnapshot& diagnostics)
{
	if (diagnostics.Entries.empty())
	{
		return {};
	}

	const std::filesystem::path& workspaceRoot = Filesystem::GetWorkspaceRootPath();
	const auto artifactRoot = Filesystem::ResolveArtifactRootPath(workspaceRoot);
	const auto projectName = Filesystem::GetProjectPath().filename();
	const auto exportRoot = artifactRoot / "symbols" / "shaders" / projectName;
	return FindMatchingShaderExports(exportRoot, diagnostics);
}
