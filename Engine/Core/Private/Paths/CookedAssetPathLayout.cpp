#include "PCH.h"

#include "FileSystemPathState.h"

#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathUtils.h"

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

}
