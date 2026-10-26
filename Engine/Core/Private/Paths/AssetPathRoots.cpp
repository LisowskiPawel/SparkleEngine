#include "PCH.h"

#include "AssetPathState.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Paths/PackagePaths.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Paths/UserStatePaths.h"

#include <optional>
#include <string>

namespace Filesystem::Private
{
	static void DiscoverWorkspaceAssetRoots(AssetPathState& state)
	{
		if (const auto engineRoot = Filesystem::DiscoverEngineRoot())
		{
			state.enginePath = *engineRoot;
			state.engineAssetsPath = state.enginePath / "Assets";
		}
		if (const auto projectRoot = Filesystem::DiscoverProjectRoot())
		{
			state.projectPath = *projectRoot;
			state.projectAssetsPath = state.projectPath / "Assets";
		}
	}

	static void DiscoverInstalledAssetRoots(AssetPathState& state)
	{
		std::string errorMessage;
		if (!ValidatePackageFiles(state.workspacePath, errorMessage))
		{
			throw Diagnostics::Error(errorMessage);
		}
		if (const auto engineRoot = DiscoverPackageEngineRoot(state.workspacePath))
		{
			state.enginePath = *engineRoot;
			state.engineAssetsPath = state.enginePath / "Assets";
		}
		if (const auto projectRoot = ResolvePackageSourceRoot(state.workspacePath))
		{
			state.projectPath = *projectRoot;
			state.projectAssetsPath = state.projectPath / "Assets";
		}
	}

	void DiscoverAssetPathRoots(AssetPathState& state)
	{
		state.workingDirectory = std::filesystem::current_path();
		state.executableDirectory = Filesystem::GetExecutableDirectory();

		const std::optional<std::filesystem::path> packageRoot = DiscoverPackageRoot();
		state.installedProduct = packageRoot.has_value();
		state.workspacePath = state.installedProduct ? Paths::Normalize(*packageRoot) : Filesystem::ResolveWorkspaceRootPath();
		state.productUserStatePaths = ResolveCurrentProductUserStatePaths();
		if (state.installedProduct)
		{
			DiscoverInstalledAssetRoots(state);
		}
		else
		{
			DiscoverWorkspaceAssetRoots(state);
		}

		state.projectPath = Paths::Normalize(state.projectPath);
		state.projectAssetsPath = Paths::Normalize(state.projectAssetsPath);
		state.enginePath = Paths::Normalize(state.enginePath);
		state.engineAssetsPath = Paths::Normalize(state.engineAssetsPath);
	}

}
