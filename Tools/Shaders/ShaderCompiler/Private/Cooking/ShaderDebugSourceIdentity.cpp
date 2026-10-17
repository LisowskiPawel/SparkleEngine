#include "PCH.h"
#include "Cooking/ShaderDebugArtifactWriter.h"

#include "Compiler/ShaderSourceMountTable.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Json/JsonReader.h"
#include "Core/Public/Json/JsonWriter.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Strings/StringUtils.h"

void ShaderDebugArtifactWriter::RecordSourceCandidate()
{
	const auto path = Filesystem::GetExecutableDirectory() / "gpu-crash-product.json";
	std::string error;
	std::string receipt;
	std::string executableHash;
	std::vector<std::string_view> files;
	if (!Files::TryReadAllText(path, receipt, error, 32u * 1024u * 1024u) || !Json::TryReadStringProperty(receipt, "executableSha256", executableHash)
	    || executableHash != HashFile(Filesystem::GetExecutablePath()) || !Json::TryReadArrayProperty(receipt, "sourceFiles", files) || files.empty() || files.size() > 65'536)
	{
		throw Diagnostics::Error("Diagnostic shader export requires this compiler's complete source receipt.");
	}
	const auto& root = Filesystem::GetWorkspaceRootPath();
	for (const auto file : files)
	{
		std::string relative;
		std::string hash;
		if (!Json::TryReadStringProperty(file, "Path", relative))
		{
			throw Diagnostics::Error("Source receipt membership is malformed.");
		}
		const std::filesystem::path relativePath = Strings::ToWide(std::string_view{relative});
		const auto source = root / relativePath;
		if (relativePath.is_absolute() || relativePath.has_root_path() || relative.find(':') != std::string::npos || !Paths::IsUnderRoot(source, root))
		{
			throw Diagnostics::Error("Source receipt escapes the workspace.");
		}
		if (!Json::TryReadStringProperty(file, "Sha256", hash))
		{
			const auto hashOffset = Json::FindPropertyValue(file, "Sha256");
			if (hashOffset == std::string_view::npos || file.substr(hashOffset, 4) != "null")
			{
				throw Diagnostics::Error("Source receipt has no hash or explicit deletion.");
			}
			if (std::filesystem::exists(source))
			{
				throw Diagnostics::Error("A deleted build input has reappeared; rebuild the diagnostic candidate.");
			}
			continue;
		}
		if (HashFile(source) != hash || !m_sourceFileHashes.emplace(Paths::MakePathKey(source), hash).second)
		{
			throw Diagnostics::Error("Source changed since the diagnostic compiler build; rebuild the candidate.");
		}
	}
	Json::ObjectWriter identity;
	for (const auto key : {"product", "profile", "variant", "revision", "sourceInputsSha256", "dirtyPatchSha256", "executableSha256"})
	{
		std::string value;
		if (!Json::TryReadStringProperty(receipt, key, value) || value.empty())
		{
			throw Diagnostics::Error("Compiler source identity is incomplete.");
		}
		identity.WriteString(key, value);
	}
	WriteText("product-receipt.json", identity.Finish());
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
