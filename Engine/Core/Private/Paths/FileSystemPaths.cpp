#include "PCH.h"

#include "Core/Public/Paths/FileSystemPaths.h"

#include "FileSystemPathState.h"
#include "Core/Public/Paths/PathUtils.h"

namespace Filesystem
{
	const std::filesystem::path& GetWorkingDirectory()
	{
		return Private::GetFileSystemPathState().workingDirectory;
	}

	const std::filesystem::path& GetWorkspaceRootPath()
	{
		return Private::GetFileSystemPathState().workspacePath;
	}

	const ProductUserStatePaths& GetProductUserStatePaths()
	{
		return Private::GetFileSystemPathState().productUserStatePaths;
	}

	const std::filesystem::path& GetProjectPath()
	{
		return Private::GetFileSystemPathState().projectPath;
	}

	const std::filesystem::path& GetEnginePath()
	{
		return Private::GetFileSystemPathState().enginePath;
	}

	void ConfigureProjectRoot(const std::filesystem::path& projectRoot)
	{
		Private::FileSystemPathState& state = Private::GetFileSystemPathState();
		state.projectPath = Paths::Normalize(projectRoot);
		state.projectAssetsPath = state.projectPath.empty() ? std::filesystem::path{} : Paths::Normalize(state.projectPath / "Assets");
		Private::RebuildProjectPaths(state);
	}

	const std::filesystem::path& GetCookedAssetRootPath()
	{
		return Private::GetFileSystemPathState().cookedAssetRootPath;
	}

	const std::filesystem::path& GetCookedShaderRootPath()
	{
		return Private::GetFileSystemPathState().cookedShaderRootPath;
	}

	const std::filesystem::path& GetGlobalShaderMapPath()
	{
		return Private::GetFileSystemPathState().globalShaderMapPath;
	}

	const std::filesystem::path& GetCookedShaderLibraryPath()
	{
		return Private::GetFileSystemPathState().cookedShaderLibraryPath;
	}

	const std::filesystem::path& GetCookedTextureRootPath()
	{
		return Private::GetFileSystemPathState().cookedTextureRootPath;
	}

	const std::filesystem::path& GetCookedSceneManifestRootPath()
	{
		return Private::GetFileSystemPathState().cookedSceneManifestRootPath;
	}

	const std::filesystem::path& GetCookedMeshRootPath()
	{
		return Private::GetFileSystemPathState().cookedMeshRootPath;
	}

	const std::filesystem::path& GetCookedMaterialRootPath()
	{
		return Private::GetFileSystemPathState().cookedMaterialRootPath;
	}

	const std::filesystem::path& GetCookedSkeletonRootPath()
	{
		return Private::GetFileSystemPathState().cookedSkeletonRootPath;
	}

	const std::filesystem::path& GetCookedAnimationRootPath()
	{
		return Private::GetFileSystemPathState().cookedAnimationRootPath;
	}

	const std::filesystem::path& GetSceneAssetRegistryPath()
	{
		return Private::GetFileSystemPathState().sceneAssetRegistryPath;
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
