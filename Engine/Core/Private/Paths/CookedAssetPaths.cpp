#include "PCH.h"

#include "Core/Public/Paths/CookedAssetPaths.h"

#include "FileSystemPathState.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Formatting/HexFormat.h"

namespace Filesystem
{
	void RebuildCookedAssetPaths(FileSystemPathState& state)
	{
		const std::filesystem::path projectName = state.projectPath.empty() ? std::filesystem::path("Shared") : state.projectPath.filename();
		const auto artifactRoot = ResolveArtifactRootPath(state.workspacePath);

		state.cookedAssetRootPath = Paths::Normalize(artifactRoot / "dev" / "Projects" / projectName / "Cooked");

		state.cookedShaderRootPath = Paths::Normalize(state.cookedAssetRootPath / "Shaders");
		state.globalShaderMapPath = Filesystem::BuildGlobalShaderMapPath(state.cookedShaderRootPath);
		state.cookedShaderLibraryPath = Filesystem::BuildCookedShaderLibraryPath(state.cookedShaderRootPath);
		state.cookedTextureRootPath = Paths::Normalize(state.cookedAssetRootPath / "Textures");
		state.cookedSceneManifestRootPath = Paths::Normalize(state.cookedAssetRootPath / "SceneManifests");
		state.cookedMeshRootPath = Paths::Normalize(state.cookedAssetRootPath / "Meshes");
		state.cookedMaterialRootPath = Paths::Normalize(state.cookedAssetRootPath / "Materials");
		state.cookedSkeletonRootPath = Paths::Normalize(state.cookedAssetRootPath / "Skeletons");
		state.cookedAnimationRootPath = Paths::Normalize(state.cookedAssetRootPath / "Animations");
		state.sceneAssetRegistryPath = Paths::Normalize(state.cookedAssetRootPath / "SceneAssetRegistry.sreg");
	}

	const std::filesystem::path& GetCookedAssetRootPath()
	{
		return GetFileSystemPathState().cookedAssetRootPath;
	}

	const std::filesystem::path& GetCookedShaderRootPath()
	{
		return GetFileSystemPathState().cookedShaderRootPath;
	}

	const std::filesystem::path& GetGlobalShaderMapPath()
	{
		return GetFileSystemPathState().globalShaderMapPath;
	}

	const std::filesystem::path& GetCookedShaderLibraryPath()
	{
		return GetFileSystemPathState().cookedShaderLibraryPath;
	}

	const std::filesystem::path& GetCookedTextureRootPath()
	{
		return GetFileSystemPathState().cookedTextureRootPath;
	}

	const std::filesystem::path& GetCookedSceneManifestRootPath()
	{
		return GetFileSystemPathState().cookedSceneManifestRootPath;
	}

	const std::filesystem::path& GetCookedMeshRootPath()
	{
		return GetFileSystemPathState().cookedMeshRootPath;
	}

	const std::filesystem::path& GetCookedMaterialRootPath()
	{
		return GetFileSystemPathState().cookedMaterialRootPath;
	}

	const std::filesystem::path& GetCookedSkeletonRootPath()
	{
		return GetFileSystemPathState().cookedSkeletonRootPath;
	}

	const std::filesystem::path& GetCookedAnimationRootPath()
	{
		return GetFileSystemPathState().cookedAnimationRootPath;
	}

	const std::filesystem::path& GetSceneAssetRegistryPath()
	{
		return GetFileSystemPathState().sceneAssetRegistryPath;
	}

	std::filesystem::path BuildGlobalShaderMapPath(const std::filesystem::path& cookedShaderRootPath)
	{
		return Paths::Normalize(cookedShaderRootPath / "GlobalShaderMap.smap");
	}

	std::filesystem::path BuildCookedShaderLibraryPath(const std::filesystem::path& cookedShaderRootPath)
	{
		return Paths::Normalize(cookedShaderRootPath / "CookedShaderLibrary.slib");
	}
}

namespace Paths
{
	std::filesystem::path CookedSceneManifest(std::string_view sceneAssetId)
	{
		std::filesystem::path relativeScenePath{std::string(sceneAssetId)};
		relativeScenePath.replace_extension(".sscn");
		return Filesystem::GetCookedSceneManifestRootPath() / relativeScenePath;
	}

	std::filesystem::path CookedSceneManifestRelative(const std::filesystem::path& relativeManifestPath)
	{
		return Filesystem::GetCookedSceneManifestRootPath() / relativeManifestPath;
	}

	std::filesystem::path CookedMeshAsset(std::uint64_t meshAssetId)
	{
		return Filesystem::GetCookedMeshRootPath() / (Formatting::FormatHexUInt64(meshAssetId) + ".smsh");
	}

	std::filesystem::path CookedMaterialAsset(std::uint64_t materialAssetId)
	{
		return Filesystem::GetCookedMaterialRootPath() / (Formatting::FormatHexUInt64(materialAssetId) + ".smat");
	}

	std::filesystem::path CookedSkeletonAsset(std::uint64_t skeletonAssetId)
	{
		return Filesystem::GetCookedSkeletonRootPath() / (Formatting::FormatHexUInt64(skeletonAssetId) + ".sskel");
	}

	std::filesystem::path CookedAnimationAsset(std::uint64_t animationAssetId)
	{
		return Filesystem::GetCookedAnimationRootPath() / (Formatting::FormatHexUInt64(animationAssetId) + ".sanim");
	}
}
