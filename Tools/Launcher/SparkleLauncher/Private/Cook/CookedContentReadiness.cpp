#include "CookedContentReadiness.h"

#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Json/JsonReader.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "Core/Public/Paths/PackagePaths.h"
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

	static bool CookedSceneScopeIsReady(const std::filesystem::path& cooked)
	{
		std::error_code errorCode;

		return !cooked.empty() && std::filesystem::is_regular_file(cooked / "SceneAssetRegistry.sreg", errorCode) && !errorCode && DirectoryHasRegularFiles(cooked / "SceneManifests")
		    && DirectoryHasRegularFiles(cooked / "Meshes");
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
		if (!Formatting::TryParseHexUInt64(expectedMapHashText, expectedMapHash) || !Formatting::TryParseHexUInt64(expectedLibraryHashText, expectedLibraryHash))
		{
			return false;
		}

		std::vector<std::uint8_t> mapBytes;
		std::vector<std::uint8_t> libraryBytes;

		return Files::TryReadAllBytes(mapPath, mapBytes, fileError) && Files::TryReadAllBytes(libraryPath, libraryBytes, fileError)
		    && Hash::Fnv1a64(mapBytes.data(), mapBytes.size()) == expectedMapHash && Hash::Fnv1a64(libraryBytes.data(), libraryBytes.size()) == expectedLibraryHash;
	}

	static bool ShaderPublicationMatchesPackageSeed(const std::filesystem::path& packageRoot, const std::filesystem::path& cookedShaderDirectory)
	{
		const auto engine = Filesystem::DiscoverPackageEngineRoot(packageRoot);
		std::string document;
		if (!engine || !Filesystem::TryReadPackageManifest(packageRoot, document))
		{
			return false;
		}
		for (const auto& path :
		    {Filesystem::BuildGlobalShaderMapPath(cookedShaderDirectory),
		        Filesystem::BuildCookedShaderLibraryPath(cookedShaderDirectory),
		        Filesystem::BuildShaderDependencyManifestPath(cookedShaderDirectory)})
		{
			const auto relative = (*engine / "Cooked/Shaders" / path.filename()).lexically_relative(packageRoot).generic_string();
			const auto seed = Filesystem::ResolvePackageFilePath(packageRoot, relative, document);
			Hash::Sha256Digest seedHash;
			Hash::Sha256Digest cookedHash;
			std::string error;
			if (!seed || !Hash::TrySha256File(*seed, seedHash, error) || !Hash::TrySha256File(path, cookedHash, error) || seedHash != cookedHash)
			{
				return false;
			}
		}
		return true;
	}

	static CookedShaderPublicationState InspectCookedShaderPublication(const std::filesystem::path& repositoryRoot, const std::filesystem::path& cookedShaderDirectory)
	{
		const std::array requiredArtifacts = {
		    Filesystem::BuildGlobalShaderMapPath(cookedShaderDirectory),
		    Filesystem::BuildCookedShaderLibraryPath(cookedShaderDirectory),
		    Filesystem::BuildShaderDependencyManifestPath(cookedShaderDirectory)};

		for (const std::filesystem::path& artifactPath : requiredArtifacts)
		{
			std::error_code errorCode;
			if (!std::filesystem::is_regular_file(artifactPath, errorCode) || errorCode || std::filesystem::file_size(artifactPath, errorCode) == 0 || errorCode)
			{
				return CookedShaderPublicationState::Missing;
			}
		}
		std::error_code errorCode;
		if (!std::filesystem::exists(Filesystem::BuildShaderRecookSignalPath(cookedShaderDirectory), errorCode))
		{
			if (errorCode || Filesystem::IsSourceWorkspace(repositoryRoot))
			{
				return CookedShaderPublicationState::Missing;
			}
			return ShaderPublicationMatchesPackageSeed(repositoryRoot, cookedShaderDirectory) ? CookedShaderPublicationState::Ready : CookedShaderPublicationState::Invalid;
		}
		if (errorCode || !ShaderPublicationMatchesSignal(cookedShaderDirectory))
		{
			return CookedShaderPublicationState::Invalid;
		}

		return CookedShaderPublicationState::Ready;
	}

	CookedContentReadiness InspectCookedContentReadiness(const std::filesystem::path& repositoryRoot, std::string_view projectId, std::string_view variant)
	{
		const auto outputs = Filesystem::ResolveWorkspaceOutputPaths(repositoryRoot, variant);
		const auto cooked = variant.empty() ? Filesystem::ResolveProductCookedRoot(repositoryRoot, projectId) : outputs.CookedProjectDirectory(projectId);
		const auto shared = Filesystem::IsSourceWorkspace(repositoryRoot) ? outputs.SharedCookedProjectDirectory() : std::filesystem::path{};

		return CookedContentReadiness{
		    .MeshesReady = CookedSceneScopeIsReady(cooked) || CookedSceneScopeIsReady(shared),
		    .TexturesReady = CookedAssetScopeHasFiles(cooked, shared, "Textures"),
		    .Shaders = cooked.empty() ? CookedShaderPublicationState::Missing : InspectCookedShaderPublication(repositoryRoot, cooked / "Shaders")};
	}
}
