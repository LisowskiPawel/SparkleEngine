#pragma once

#include <filesystem>

namespace Filesystem
{
	struct FileSystemPathState final
	{
		std::filesystem::path workspacePath;
		std::filesystem::path cookedAssetRootPath;
		std::filesystem::path cookedShaderRootPath;
		std::filesystem::path globalShaderMapPath;
		std::filesystem::path cookedShaderLibraryPath;
		std::filesystem::path cookedTextureRootPath;
		std::filesystem::path cookedSceneManifestRootPath;
		std::filesystem::path cookedMeshRootPath;
		std::filesystem::path cookedMaterialRootPath;
		std::filesystem::path cookedSkeletonRootPath;
		std::filesystem::path cookedAnimationRootPath;
		std::filesystem::path sceneAssetRegistryPath;
		std::filesystem::path projectPath;
		std::filesystem::path enginePath;
		std::filesystem::path workingDirectory;
		std::filesystem::path executableDirectory;
	};

	FileSystemPathState& GetFileSystemPathState();
	void RebuildProjectPaths(FileSystemPathState& state);
	void DiscoverFileSystemPathRoots(FileSystemPathState& state);
	void RebuildCookedAssetPaths(FileSystemPathState& state);
	void ValidateConfiguredPaths(const FileSystemPathState& state);
}
