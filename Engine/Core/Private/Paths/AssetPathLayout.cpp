#include "PCH.h"

#include "AssetPathState.h"

#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Paths/PackagePaths.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Paths/WorkspaceOutputPaths.h"

#include <array>
#include <system_error>

namespace Filesystem::Private
{
	void RebuildTypedPaths(AssetPathState& state)
	{
		const auto buildPaths = [](const std::filesystem::path& root, auto& paths)
		{
			paths.fill({});
			if (root.empty())
			{
				return;
			}

			for (std::size_t index = 0; index < AssetPathState::AssetTypeCount; ++index)
			{
				const std::string_view subdirectory = GetAssetSubdirectory(static_cast<AssetType>(index));
				paths[index] = subdirectory.empty() ? root : root / subdirectory;
			}
		};

		buildPaths(state.projectAssetsPath, state.projectTypedPaths);
		buildPaths(state.engineAssetsPath, state.engineTypedPaths);
	}

	void RebuildCookedPaths(AssetPathState& state)
	{
		const std::filesystem::path projectName = state.projectPath.empty() ? std::filesystem::path("Shared") : state.projectPath.filename();
		const WorkspaceOutputPaths workspaceOutputs = ResolveWorkspaceOutputPaths(state.workspacePath);

		if (state.installedProduct)
		{
			const auto contentRoot = ResolvePackageContentRoot(state.workspacePath);
			SPARKLE_DEFINE_LOG_CATEGORY_STATIC(logger, "Core");
			if (!contentRoot)
			{
				Diagnostics::Fatal(logger, __FILE__, __LINE__, "Invalid ContentStore in " + (state.workspacePath / "manifests/sparkle-package-manifest.json").string());
			}
			state.cookedAssetRootPath = Paths::Normalize(*contentRoot / "Cooked");
		}
		else
		{
			state.cookedAssetRootPath = Paths::Normalize(workspaceOutputs.CookedProjectDirectory(projectName.string()));
		}

		state.cookedShaderRootPath = Paths::Normalize(state.cookedAssetRootPath / "Shaders");
		state.globalShaderMapPath = Filesystem::BuildGlobalShaderMapPath(state.cookedShaderRootPath);
		state.cookedShaderLibraryPath = Filesystem::BuildCookedShaderLibraryPath(state.cookedShaderRootPath);
		state.shaderRecookSignalPath = Filesystem::BuildShaderRecookSignalPath(state.cookedShaderRootPath);
		state.cookedTextureRootPath = Paths::Normalize(state.cookedAssetRootPath / "Textures");
		state.cookedSceneManifestRootPath = Paths::Normalize(state.cookedAssetRootPath / "SceneManifests");
		state.cookedMeshRootPath = Paths::Normalize(state.cookedAssetRootPath / "Meshes");
		state.cookedMaterialRootPath = Paths::Normalize(state.cookedAssetRootPath / "Materials");
		state.cookedSkeletonRootPath = Paths::Normalize(state.cookedAssetRootPath / "Skeletons");
		state.cookedAnimationRootPath = Paths::Normalize(state.cookedAssetRootPath / "Animations");
		state.sceneAssetRegistryPath = Paths::Normalize(state.cookedAssetRootPath / "SceneAssetRegistry.sreg");

		state.shaderSymbolsOutputPath = state.installedProduct ? Paths::Normalize(state.productUserStatePaths.CacheRoot / "ShaderSymbols" / projectName)
		                                                       : Paths::Normalize(workspaceOutputs.SymbolsRoot / "shaders" / projectName);
	}

	void MaterializeOutputDirectories(const AssetPathState& state)
	{
		const std::array outputDirectories = {
		    &state.cookedAssetRootPath,
		    &state.cookedShaderRootPath,
		    &state.cookedTextureRootPath,
		    &state.cookedSceneManifestRootPath,
		    &state.cookedMeshRootPath,
		    &state.cookedMaterialRootPath,
		    &state.cookedSkeletonRootPath,
		    &state.cookedAnimationRootPath,
		    &state.shaderSymbolsOutputPath,
		};

		for (const std::filesystem::path* outputDirectory : outputDirectories)
		{
			if (outputDirectory->empty())
			{
				continue;
			}
			std::error_code errorCode;
			std::filesystem::create_directories(*outputDirectory, errorCode);
		}
	}

}
