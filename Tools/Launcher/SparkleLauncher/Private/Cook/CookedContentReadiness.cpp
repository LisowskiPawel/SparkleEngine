#include "CookedContentReadiness.h"

#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Json/JsonReader.h"
#include "Core/Public/Paths/WorkspaceOutputPaths.h"
#include "Core/Public/Paths/ProductPaths.h"

#include <array>
#include <cstdint>
#include <string>
#include <system_error>
#include <vector>

namespace SparkleLauncher
{
	static bool DirectoryHasRegularFiles(const std::filesystem::path& directory)
	{
		std::error_code errorCode;
		if (!std::filesystem::is_directory(directory, errorCode))
		{
			return false;
		}

		std::filesystem::recursive_directory_iterator iterator(directory, std::filesystem::directory_options::skip_permission_denied, errorCode);
		const std::filesystem::recursive_directory_iterator end;
		while (iterator != end)
		{
			const std::filesystem::directory_entry& entry = *iterator;
			if (entry.is_regular_file(errorCode))
			{
				return true;
			}
			errorCode.clear();
			iterator.increment(errorCode);
			errorCode.clear();
		}
		return false;
	}

	static bool CookedAssetScopeHasFiles(const std::filesystem::path& cooked, const std::filesystem::path& shared, std::string_view relativeDirectory)
	{
		return (!cooked.empty() && DirectoryHasRegularFiles(cooked / relativeDirectory)) || (!shared.empty() && DirectoryHasRegularFiles(shared / relativeDirectory));
	}

	static bool ShaderPublicationMatchesSignal(const std::filesystem::path& cookedShaderDirectory)
	{
		const std::filesystem::path mapPath = Filesystem::BuildGlobalShaderMapPath(cookedShaderDirectory);
		const std::filesystem::path libraryPath = Filesystem::BuildCookedShaderLibraryPath(cookedShaderDirectory);
		std::string signal;
		std::string fileError;
		if (!Files::TryReadAllText(Filesystem::BuildShaderRecookSignalPath(cookedShaderDirectory), signal, fileError))
		{
			return false;
		}

		std::string schema;
		std::string status;
		std::string expectedMapHashText;
		std::string expectedLibraryHashText;
		if (!Json::TryReadStringProperty(signal, "schema", schema) || schema != "sparkle.shaderRecookResult" || !Json::TryReadStringProperty(signal, "status", status) || status != "succeeded"
		    || !Json::TryReadStringProperty(signal, "globalShaderMapHash", expectedMapHashText) || !Json::TryReadStringProperty(signal, "cookedShaderLibraryHash", expectedLibraryHashText))
		{
			return false;
		}

		std::uint64_t expectedMapHash = 0;
		std::uint64_t expectedLibraryHash = 0;
		if (!Json::TryParseHexUInt64(expectedMapHashText, expectedMapHash) || !Json::TryParseHexUInt64(expectedLibraryHashText, expectedLibraryHash))
		{
			return false;
		}

		std::vector<std::uint8_t> mapBytes;
		std::vector<std::uint8_t> libraryBytes;

		return Files::TryReadAllBytes(mapPath, mapBytes, fileError) && Files::TryReadAllBytes(libraryPath, libraryBytes, fileError)
		    && Hash::Fnv1a64(mapBytes.data(), mapBytes.size()) == expectedMapHash && Hash::Fnv1a64(libraryBytes.data(), libraryBytes.size()) == expectedLibraryHash;
	}

	static CookedShaderPublicationState InspectCookedShaderPublication(const std::filesystem::path& cookedShaderDirectory)
	{
		const std::array requiredArtifacts = {
		    Filesystem::BuildGlobalShaderMapPath(cookedShaderDirectory),
		    Filesystem::BuildCookedShaderLibraryPath(cookedShaderDirectory),
		    Filesystem::BuildShaderDependencyManifestPath(cookedShaderDirectory),
		    Filesystem::BuildShaderRecookSignalPath(cookedShaderDirectory)};

		for (const std::filesystem::path& artifactPath : requiredArtifacts)
		{
			std::error_code errorCode;
			if (!std::filesystem::is_regular_file(artifactPath, errorCode) || errorCode || std::filesystem::file_size(artifactPath, errorCode) == 0 || errorCode)
			{
				return CookedShaderPublicationState::Missing;
			}
		}
		if (!ShaderPublicationMatchesSignal(cookedShaderDirectory))
		{
			return CookedShaderPublicationState::Invalid;
		}

		return CookedShaderPublicationState::Ready;
	}

	CookedContentReadiness InspectCookedContentReadiness(const std::filesystem::path& repositoryRoot, std::string_view projectId)
	{
		const auto cooked = Filesystem::ResolveProductCookedRoot(repositoryRoot, projectId);
		const auto shared = Filesystem::IsSourceWorkspace(repositoryRoot) ? Filesystem::ResolveWorkspaceOutputPaths(repositoryRoot).SharedCookedProjectDirectory() : std::filesystem::path{};

		return CookedContentReadiness{
		    .MeshesReady = CookedAssetScopeHasFiles(cooked, shared, "Meshes"),
		    .TexturesReady = CookedAssetScopeHasFiles(cooked, shared, "Textures"),
		    .Shaders = cooked.empty() ? CookedShaderPublicationState::Missing : InspectCookedShaderPublication(cooked / "Shaders")};
	}
}
