#include "PCH.h"

#include "Core/Public/Paths/ProductFilePaths.h"

#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "Paths/LogPathPolicy.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

namespace Paths
{
	std::filesystem::path LogFile(std::string_view configuredFile, bool bEnsureParentExists)
	{
		const Private::LogParentDirectoryPolicy parentDirectoryPolicy = bEnsureParentExists ? Private::LogParentDirectoryPolicy::EnsureExists : Private::LogParentDirectoryPolicy::Preserve;
		return Private::ResolveBootstrapLogFile(configuredFile, parentDirectoryPolicy);
	}

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
