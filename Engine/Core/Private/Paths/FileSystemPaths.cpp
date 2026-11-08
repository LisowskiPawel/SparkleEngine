#include "PCH.h"

#include "Core/Public/Paths/FileSystemPaths.h"

#include "FileSystemPathState.h"
#include "Core/Public/Paths/PathUtils.h"

namespace Filesystem
{
	const std::filesystem::path& GetWorkingDirectory()
	{
		return GetFileSystemPathState().workingDirectory;
	}

	const std::filesystem::path& GetWorkspaceRootPath()
	{
		return GetFileSystemPathState().workspacePath;
	}

	const ProductUserStatePaths& GetProductUserStatePaths()
	{
		return GetFileSystemPathState().productUserStatePaths;
	}

	const std::filesystem::path& GetProjectPath()
	{
		return GetFileSystemPathState().projectPath;
	}

	const std::filesystem::path& GetEnginePath()
	{
		return GetFileSystemPathState().enginePath;
	}

	void ConfigureProjectRoot(const std::filesystem::path& projectRoot)
	{
		FileSystemPathState& state = GetFileSystemPathState();
		state.projectPath = Paths::Normalize(projectRoot);
		state.projectAssetsPath = state.projectPath.empty() ? std::filesystem::path{} : Paths::Normalize(state.projectPath / "Assets");
		RebuildProjectPaths(state);
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
