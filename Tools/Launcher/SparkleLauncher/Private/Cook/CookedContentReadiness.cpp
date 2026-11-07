#include "Cook/CookedContentReadiness.h"

#include "Core/Public/Paths/FileSystemPaths.h"
#include "ToolOutputPaths.h"

#include <array>
#include <string>
#include <system_error>

namespace SparkleLauncher
{
	static bool DirectoryHasRegularFiles(const std::filesystem::path& directory)
	{
		std::error_code errorCode;
		if (std::filesystem::exists(directory / ".publication-incomplete", errorCode) || errorCode || !std::filesystem::is_directory(directory, errorCode))
		{
			return false;
		}

		std::filesystem::recursive_directory_iterator iterator(directory, std::filesystem::directory_options::skip_permission_denied, errorCode);
		const std::filesystem::recursive_directory_iterator end;
		while (iterator != end)
		{
			const std::filesystem::directory_entry& entry = *iterator;
			if (entry.is_regular_file(errorCode) && !entry.path().filename().string().starts_with("."))
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

		return !cooked.empty() && !std::filesystem::exists(cooked / ".publication-incomplete", errorCode) && !errorCode && std::filesystem::is_regular_file(cooked / ".publication-lock", errorCode)
		    && !errorCode && std::filesystem::is_regular_file(cooked / "SceneAssetRegistry.sreg", errorCode) && !errorCode && DirectoryHasRegularFiles(cooked / "SceneManifests")
		    && DirectoryHasRegularFiles(cooked / "Meshes");
	}

	static CookedShaderPublicationState InspectCookedShaderPublication(const std::filesystem::path& cookedShaderDirectory)
	{
		std::error_code errorCode;
		if (std::filesystem::exists(cookedShaderDirectory / ".publication-incomplete", errorCode) || errorCode)
		{
			return CookedShaderPublicationState::Invalid;
		}

		const std::array requiredArtifacts = {
		    Filesystem::BuildGlobalShaderMapPath(cookedShaderDirectory),
		    Filesystem::BuildCookedShaderLibraryPath(cookedShaderDirectory),
		    SparkleTools::ShaderDependencyManifestPath(cookedShaderDirectory)};

		for (const auto& artifact : requiredArtifacts)
		{
			if (!std::filesystem::is_regular_file(artifact, errorCode) || errorCode || std::filesystem::file_size(artifact, errorCode) == 0 || errorCode)
			{
				return CookedShaderPublicationState::Missing;
			}
		}
		return CookedShaderPublicationState::Ready;
	}

	CookedContentReadiness InspectCookedContentReadiness(const std::filesystem::path& workspaceRoot, std::string_view projectId, std::string_view variant)
	{
		const auto outputs = variant.empty() ? SparkleTools::ResolveToolOutputPaths(workspaceRoot) : SparkleTools::ResolveToolOutputPaths(workspaceRoot, variant);
		const auto cooked = outputs.CookedProjectDirectory(projectId);
		const auto shared = outputs.SharedCookedProjectDirectory();

		return CookedContentReadiness{
		    .MeshesReady = CookedSceneScopeIsReady(cooked) || CookedSceneScopeIsReady(shared),
		    .TexturesReady = CookedAssetScopeHasFiles(cooked, shared, "Textures"),
		    .Shaders = cooked.empty() ? CookedShaderPublicationState::Missing : InspectCookedShaderPublication(cooked / "Shaders")};
	}
}
