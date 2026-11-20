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
#include <utility>

static nlohmann::json ParseSourceCandidateReceipt(std::string_view receipt, std::map<std::wstring, std::optional<std::string>>& sourceFiles)
{
	Json::InputAdmission admission(receipt, 32u * 1024u * 1024u);
	const std::array<std::string_view, 8> fields{"product", "profile", "variant", "revision", "sourceInputsSha256", "dirtyPatchSha256", "executableSha256", "sourceFiles"};
	std::size_t fileCount = 0;
	bool bSourceFiles = false;
	bool bScalarField = false;

	const auto admitValue = [&](int depth, nlohmann::json::parse_event_t event, nlohmann::json& value)
	{
		using Event = nlohmann::json::parse_event_t;
		admission.ReadEvent(depth, event);
		const bool bContainerStart = event == Event::object_start || event == Event::array_start;
		const bool bRecordDepth = bSourceFiles && depth == 3;
		const bool bScalarDepth = depth == 1 || bRecordDepth;
		const bool bScalarValue = bScalarDepth && bScalarField;
		if (bContainerStart && bScalarValue)
		{
			throw Diagnostics::Error("Source receipt identity and file fields require scalar values.");
		}
		if (bSourceFiles && depth == 1 && event == Event::object_start)
		{
			throw Diagnostics::Error("Source receipt membership must be an array.");
		}
		const bool bEntryStart = bContainerStart || event == Event::value;
		if (bSourceFiles && depth == 2 && bEntryStart)
		{
			if (event != Event::object_start)
			{
				throw Diagnostics::Error("Source receipt member must be an object.");
			}
			if (++fileCount > 65'536)
			{
				throw Diagnostics::Error("Source receipt exceeds its membership limit.");
			}
		}
		if (event != Event::key)
		{
			return true;
		}

		const auto& key = value.get_ref<const std::string&>();
		if (depth == 1)
		{
			bSourceFiles = key == "sourceFiles";
			const bool bConsumed = std::find(fields.begin(), fields.end(), key) != fields.end();
			bScalarField = bConsumed && !bSourceFiles;
			return bConsumed && admission.ReadKey(depth, key);
		}
		const bool bFileKey = key == "Path" || key == "Sha256";
		bScalarField = bRecordDepth && bFileKey;
		return bScalarField && admission.ReadKey(depth, key);
	};

	auto document = nlohmann::json::parse(receipt.begin(), receipt.end(), admitValue);
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
		const auto* value = document.at(std::string(key)).get_ptr<const std::string*>();
		if (value == nullptr || value->empty())
		{
			throw Diagnostics::Error("Compiler source identity is incomplete.");
		}
	}

	const auto& root = Filesystem::GetWorkspaceRootPath();
	for (const auto& file : document.at("sourceFiles"))
	{
		const auto& relative = file.at("Path").get_ref<const std::string&>();
		const std::filesystem::path relativePath = Strings::ToWide(relative);
		const auto source = root / relativePath;
		const bool bInvalidRoot = relativePath.has_root_path() || relative.empty();
		const bool bInvalidName = relative.find(':') != std::string::npos || relative.find('\0') != std::string::npos;
		const bool bInvalidPath = bInvalidRoot || bInvalidName;
		if (bInvalidPath || !Paths::IsUnderRoot(source, root))
		{
			throw Diagnostics::Error("Source receipt escapes the workspace.");
		}
		const auto& hash = file.at("Sha256");
		std::optional<std::string> expected;
		if (!hash.is_null())
		{
			expected = hash.get<std::string>();
		}
		if (!sourceFiles.emplace(Paths::MakePathKey(source), std::move(expected)).second)
		{
			throw Diagnostics::Error("Source receipt repeats a path.");
		}
	}
	return document;
}

void ShaderDebugArtifactWriter::VerifySourceCandidateFiles(const nlohmann::json& sourceFiles) const
{
	const auto& root = Filesystem::GetWorkspaceRootPath();
	for (const auto& file : sourceFiles)
	{
		const auto source = root / Strings::ToWide(file.at("Path").get_ref<const std::string&>());
		const auto& expected = m_sourceFileHashes.at(Paths::MakePathKey(source));
		if (!expected.has_value())
		{
			if (std::filesystem::exists(source))
			{
				throw Diagnostics::Error("A deleted build input has reappeared; rebuild the diagnostic candidate.");
			}
		}
		else if (HashFile(source) != *expected)
		{
			throw Diagnostics::Error("Source changed since the diagnostic compiler build; rebuild the candidate.");
		}
	}
}

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
		auto identity = ParseSourceCandidateReceipt(receipt, m_sourceFileHashes);
		const auto& expectedExecutableHash = identity.at("executableSha256").get_ref<const std::string&>();
		const auto executablePath = Filesystem::GetExecutablePath();
		const auto executableHash = HashFile(executablePath);
		if (expectedExecutableHash != executableHash)
		{
			throw Diagnostics::Error("Compiler source receipt does not identify this executable.");
		}

		VerifySourceCandidateFiles(identity.at("sourceFiles"));
		identity.erase("sourceFiles");

		WriteText("product-receipt.json", identity.dump());
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
