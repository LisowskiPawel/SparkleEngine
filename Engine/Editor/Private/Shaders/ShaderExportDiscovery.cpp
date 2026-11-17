#include "PCH.h"

#include "Shaders/ShaderExportDiscovery.h"
#include "Renderer/Public/Diagnostics/ShaderDiagnostics.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Json/JsonInputAdmission.h"

#include <nlohmann/json.hpp>
#include "Core/Public/Diagnostics/Error.h"

#include <algorithm>
#include <charconv>
#include <string_view>
#include <system_error>

static nlohmann::json ReadExportDocument(std::string_view text, std::initializer_list<std::string_view> fields)
{
	Json::InputAdmission admission(text, 32u * 1024u * 1024u);
	bool bJobs = false;
	std::size_t jobCount = 0;
	auto document = nlohmann::json::parse(
	    text.begin(),
	    text.end(),
	    [&](int depth, nlohmann::json::parse_event_t event, nlohmann::json& value)
	    {
		    using Event = nlohmann::json::parse_event_t;
		    const bool bContainerStart = event == Event::object_start || event == Event::array_start;
		    const bool bEntryStart = bContainerStart || event == Event::value;
		    if (bJobs && depth == 2 && bEntryStart && ++jobCount > 65'536)
		    {
			    throw Diagnostics::Error("Shader export exceeds its job limit.");
		    }
		    if (event == Event::object_start)
		    {
			    admission.BeginObject(depth);
		    }
		    if (event == Event::array_start || event == Event::value)
		    {
			    admission.CheckDepth(depth);
		    }
		    if (event == Event::key)
		    {
			    const auto& key = value.get_ref<const std::string&>();
			    if (depth == 1)
			    {
				    bJobs = key == "jobs";
			    }
			    const bool bConsumed = depth == 1 && std::find(fields.begin(), fields.end(), key) != fields.end();
			    if (!bConsumed)
			    {
				    return false;
			    }
			    admission.ReadKey(depth, key);
		    }
		    if (event == Event::object_end)
		    {
			    admission.EndObject(depth);
			    if (depth > 0)
			    {
				    return false;
			    }
		    }
		    if (event == Event::array_end && depth > 1)
		    {
			    return false;
		    }
		    return true;
	    });
	if (!document.is_object())
	{
		throw Diagnostics::Error("Shader export requires an object root.");
	}
	return document;
}

static bool ReadExportIdentity(const nlohmann::json& document, std::string_view key, std::uint64_t& value)
{
	const auto& hex = document.at(std::string(key)).get_ref<const std::string&>();
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
	try
	{
		const auto document = ReadExportDocument(text, {"shaderTypeId", "compileInputHash", "target", "backend"});
		const auto& exportTarget = document.at("target").get_ref<const std::string&>();
		shader.Backend = document.at("backend").get<std::string>();
		const bool bIdentity = ReadExportIdentity(document, "shaderTypeId", shader.ShaderType) && ReadExportIdentity(document, "compileInputHash", shader.CompileInputHash);
		return bIdentity && exportTarget == GetShaderTargetName(target);
	}
	catch (const std::exception&)
	{
		return false;
	}
}

static bool ResolveExportRequest(const std::filesystem::path& cookDirectory, const std::string& name, std::filesystem::path& request)
{
	const std::filesystem::path relative(name);
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
		const bool bSameTarget = bSameCompile && active.Backend == shader.Backend;
		const bool bFirstExport = bSameTarget && selected[index].empty();
		const bool bCompareAge = bSameTarget && !bFirstExport;
		const bool bNewerExport = bCompareAge && request.parent_path().native() > selected[index].native();
		const bool bSelectExport = bFirstExport || bNewerExport;

		if (bSelectExport)
		{
			selected[index] = request.parent_path();
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
		if (!jobs.is_array() || jobs.size() > 65'536)
		{
			return;
		}
		for (const auto& job : jobs)
		{
			std::filesystem::path request;
			const bool bString = job.is_string();
			const bool bResolved = bString && ResolveExportRequest(cookDirectory, job.get_ref<const std::string&>(), request);
			if (bResolved)
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
			if (!cook->is_directory(error) || error || !publication.Contains(cook->path()))
			{
				error.clear();
				continue;
			}
			InspectPublishedCook(cook->path(), publication, diagnostics, selected);
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

	const auto root = Filesystem::ResolveArtifactRootPath(Filesystem::GetWorkspaceRootPath()) / "symbols" / "shaders" / Filesystem::GetProjectPath().filename();
	return FindMatchingShaderExports(root, diagnostics);
}
