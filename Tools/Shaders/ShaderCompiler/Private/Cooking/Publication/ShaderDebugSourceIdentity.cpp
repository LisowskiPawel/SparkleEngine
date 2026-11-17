#include "PCH.h"
#include "Cooking/Publication/ShaderDebugArtifactWriter.h"

#include "Compiler/ShaderSourceMountTable.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Json/JsonInputAdmission.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Strings/StringUtils.h"

#include <nlohmann/json.hpp>
#include <algorithm>
#include <array>
#include <optional>

void ShaderDebugArtifactWriter::RecordSourceCandidate()
{
	const auto path = Filesystem::GetExecutableDirectory() / "gpu-crash-product.json";
	std::string error;
	std::string receipt;
	if (!Files::TryReadAllText(path, receipt, error, 32u * 1024u * 1024u))
	{
		throw Diagnostics::Error("Diagnostic shader export requires this compiler's complete source receipt.");
	}

	try
	{
		Json::InputAdmission admission(receipt, 32u * 1024u * 1024u);
		const std::array<std::string_view, 8> fields{"product", "profile", "variant", "revision", "sourceInputsSha256", "dirtyPatchSha256", "executableSha256", "sourceFiles"};
		std::map<std::string, std::optional<std::string>> sourceFiles;
		std::size_t fileCount = 0;
		bool bSourceFiles = false;
		bool bIdentityField = false;
		bool bRecordField = false;

		auto document = nlohmann::json::parse(
		    receipt.begin(),
		    receipt.end(),
		    [&](int depth, nlohmann::json::parse_event_t event, nlohmann::json& value)
		    {
			    using Event = nlohmann::json::parse_event_t;
			    const bool bContainerStart = event == Event::object_start || event == Event::array_start;
			    const bool bIdentityContainer = depth == 1 && bIdentityField;
			    const bool bInRecord = depth == 3 && bSourceFiles;
			    const bool bRecordContainer = bInRecord && bRecordField;
			    if (bContainerStart && (bIdentityContainer || bRecordContainer))
			    {
				    throw Diagnostics::Error("Source receipt identity and file fields require scalar values.");
			    }
			    if (bSourceFiles && depth == 1 && event == Event::object_start)
			    {
				    throw Diagnostics::Error("Source receipt membership must be an array.");
			    }
			    if (event == Event::object_start)
			    {
				    admission.BeginObject(depth);
				    if (bSourceFiles && depth == 2 && ++fileCount > 65'536)
				    {
					    throw Diagnostics::Error("Source receipt exceeds its membership limit.");
				    }
			    }
			    if (event == Event::array_start || event == Event::value)
			    {
				    admission.CheckDepth(depth);
				    if (bSourceFiles && depth == 2)
				    {
					    throw Diagnostics::Error("Source receipt member must be an object.");
				    }
			    }
			    if (event == Event::key)
			    {
				    const auto& key = value.get_ref<const std::string&>();
				    if (depth == 1)
				    {
					    bSourceFiles = key == "sourceFiles";
					    const bool bConsumed = std::find(fields.begin(), fields.end(), key) != fields.end();
					    bIdentityField = bConsumed && !bSourceFiles;
					    if (!bConsumed)
					    {
						    return false;
					    }
				    }
				    else
				    {
					    const bool bFileKey = depth == 3 && bSourceFiles;
					    const bool bConsumed = key == "Path" || key == "Sha256";
					    bRecordField = bFileKey && bConsumed;
					    if (!bRecordField)
					    {
						    return false;
					    }
				    }
				    admission.ReadKey(depth, key);
			    }
			    if (event == Event::object_end)
			    {
				    admission.EndObject(depth);
				    if (bSourceFiles && depth == 2)
				    {
					    const auto& name = value.at("Path").get_ref<const std::string&>();
					    const auto& hash = value.at("Sha256");
					    std::optional<std::string> expected;
					    if (!hash.is_null())
					    {
						    expected = hash.get<std::string>();
					    }
					    if (!sourceFiles.emplace(name, std::move(expected)).second)
					    {
						    throw Diagnostics::Error("Source receipt repeats a path.");
					    }
					    return false;
				    }
			    }
			    return true;
		    });

		// Finish complete syntax/schema/count admission before touching member files.
		if (!document.is_object() || !document.at("sourceFiles").is_array() || fileCount == 0)
		{
			throw Diagnostics::Error("Compiler source receipt has no bounded membership array.");
		}
		for (const auto key : fields)
		{
			if (key == "sourceFiles")
			{
				continue;
			}
			const auto& value = document.at(std::string(key));
			if (!value.is_string() || value.get_ref<const std::string&>().empty())
			{
				throw Diagnostics::Error("Compiler source identity is incomplete.");
			}
		}
		document.erase("sourceFiles");
		if (document.at("executableSha256").get_ref<const std::string&>() != HashFile(Filesystem::GetExecutablePath()))
		{
			throw Diagnostics::Error("Compiler source receipt does not identify this executable.");
		}

		const auto& root = Filesystem::GetWorkspaceRootPath();
		while (!sourceFiles.empty())
		{
			auto file = sourceFiles.extract(sourceFiles.begin());
			const auto& relative = file.key();
			const auto& expected = file.mapped();
			const std::filesystem::path relativePath = Strings::ToWide(std::string_view{relative});
			const auto source = root / relativePath;
			const bool bInvalidRoot = relativePath.has_root_path() || relative.empty();
			const bool bInvalidName = relative.find(':') != std::string::npos || relative.find('\0') != std::string::npos;
			const bool bInvalidPath = bInvalidRoot || bInvalidName;
			if (bInvalidPath || !Paths::IsUnderRoot(source, root))
			{
				throw Diagnostics::Error("Source receipt escapes the workspace.");
			}
			if (!expected.has_value())
			{
				if (std::filesystem::exists(source))
				{
					throw Diagnostics::Error("A deleted build input has reappeared; rebuild the diagnostic candidate.");
				}
				continue;
			}
			if (HashFile(source) != *expected || !m_sourceFileHashes.emplace(Paths::MakePathKey(source), std::move(*file.mapped())).second)
			{
				throw Diagnostics::Error("Source changed since the diagnostic compiler build; rebuild the candidate.");
			}
		}
		WriteText("product-receipt.json", document.dump());
	}
	catch (const nlohmann::json::exception&)
	{
		throw Diagnostics::Error("Compiler source receipt has invalid JSON, member types or numeric range.");
	}
}

void ShaderDebugArtifactWriter::VerifyCapturedSource(const ShaderCompileRequest& request, const ShaderSourceFile& source) const
{
	const auto physicalPath = request.SourceMounts.get().ResolvePhysicalPath(source.VirtualPath);
	const auto input = m_sourceFileHashes.find(Paths::MakePathKey(physicalPath));
	const auto bytes = std::as_bytes(std::span(source.Contents));
	if (input == m_sourceFileHashes.end() || input->second != HashBytes(bytes))
	{
		throw Diagnostics::Error("Captured shader source does not belong to the diagnostic source candidate.");
	}
}
