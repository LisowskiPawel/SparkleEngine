#include "PCH.h"

#include "Shaders/ShaderExportDiscovery.h"
#include "Renderer/Public/Diagnostics/ShaderDiagnostics.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Json/JsonReader.h"
#include "Core/Public/Diagnostics/Error.h"

#include <algorithm>
#include <charconv>
#include <string_view>
#include <system_error>

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

static bool ResolveExportRequest(const std::filesystem::path& cookDirectory, std::string_view job, std::filesystem::path& request)
{
	std::string name;
	if (!Json::TryReadStringProperty("{\"path\":" + std::string(job) + "}", "path", name))
	{
		return false;
	}

	const std::filesystem::path relative(name);
	const bool bInvalidRequestPathfilename = relative.has_root_path() || relative.filename() != "compile-request.json";
	const bool invalidCompileRequestPath = bInvalidRequestPathfilename || (std::any_of(relative.begin(), relative.end(), [](const auto& component) { return component == ".."; }));
	
	if (invalidCompileRequestPath)
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
		const bool bNewerMatchingExportInputHash = active.ShaderType == shader.ShaderType && active.CompileInputHash == shader.CompileInputHash;
		const bool bNewerMatchingExportBackend = bNewerMatchingExportInputHash && (active.Backend == shader.Backend);
		const bool bNewerMatchingExportselected = bNewerMatchingExportBackend && (selected[index].empty());
		const bool bMatchingExportGuardparentpath = bNewerMatchingExportBackend && !bNewerMatchingExportselected;
		const bool bNewerMatchingExportparentpath = bMatchingExportGuardparentpath && (request.parent_path().native() > selected[index].native());
		const bool bNewerMatchingExportparentpath2 = bNewerMatchingExportselected || bNewerMatchingExportparentpath;
		const bool newerMatchingExport = bNewerMatchingExportBackend && bNewerMatchingExportparentpath2;
		
		if (newerMatchingExport)
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
	const bool requestPublished = publication.Contains(request);
	const bool requestRead = requestPublished && Files::TryReadAllText(request, text, failure);
	const bool shaderDecoded = requestRead && ReadExportShader(text, diagnostics.Target, shader);

	if (!shaderDecoded)
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
	std::vector<std::string_view> jobs;
	const bool manifestRead = Files::TryReadAllText(cookDirectory / "shader-export.json", manifest, failure);
	const bool jobsRead = manifestRead && Json::TryReadArrayProperty(manifest, "jobs", jobs);

	if (!jobsRead)
	{
		return;
	}
	for (const std::string_view job : jobs)
	{
		std::filesystem::path request;
		if (ResolveExportRequest(cookDirectory, job, request))
		{
			InspectPublishedShaderRequest(request, publication, diagnostics, selected);
		}
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
