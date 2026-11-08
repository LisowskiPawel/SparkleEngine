#include "PCH.h"

#include "CookedSceneManifestWriter.h"
#include "CookedSceneBuild.h"
#include "BinaryAssetWriter.h"
#include "Assets/SceneAssetRegistry.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Files/FileUtils.h"
#include "Core/Public/Files/FileSetPublication.h"
#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathUtils.h"

#include <optional>

static void StageManifest(const CookedSceneBuild& build, std::vector<Files::FilePublication>& outPublication)
{
	const std::filesystem::path stagedPath = Files::BuildTemporaryPath(build.identity.manifestPath, ".cook-generation");

	Files::CleanupTemporaryFile(stagedPath);
	outPublication.push_back({stagedPath, build.identity.manifestPath});

	SparkleTools::WriteBinaryAssetFile(
	    stagedPath,
	    build.manifest.header,
	    build.manifest.meshAssetReferences,
	    build.manifest.materialAssetReferences,
	    build.manifest.instances,
	    build.manifest.instanceGroups,
	    build.manifest.cameras,
	    build.manifest.lights,
	    build.manifest.skeletonRefs,
	    build.manifest.animationReferences,
	    build.manifest.morphWeights,
	    build.manifest.materialVariants,
	    build.manifest.materialVariantMappings);
}

static std::filesystem::path ResolveManifestRelativePath(const CookedSceneBuild& build)
{
	const std::optional<std::filesystem::path> relativePath = Paths::TryMakeRelativeUnderRoot(build.identity.manifestPath, Filesystem::GetCookedSceneManifestRootPath());
	if (!relativePath)
	{
		throw Diagnostics::Error("Failed to derive a relative cooked scene manifest path for scene asset id '" + build.identity.assetId + "'.");
	}

	return *relativePath;
}

static void StageRegistry(std::span<const CookedSceneBuild* const> builds, std::vector<Files::FilePublication>& outPublication)
{
	Assets::SceneAssetRegistry registry;
	registry.Load();

	for (const CookedSceneBuild* build : builds)
	{
		registry.Upsert(build->identity.assetId, ResolveManifestRelativePath(*build));
	}

	const std::filesystem::path& registryPath = Filesystem::GetSceneAssetRegistryPath();
	const std::filesystem::path stagedPath = Files::BuildTemporaryPath(registryPath, ".cook-generation");

	Files::CleanupTemporaryFile(stagedPath);
	outPublication.push_back({stagedPath, registryPath});

	registry.Save(stagedPath);
}

void StageCookedSceneManifestsAndRegistry(std::span<const CookedSceneBuild* const> builds, std::vector<Files::FilePublication>& outPublication)
{
	for (const CookedSceneBuild* build : builds)
	{
		StageManifest(*build, outPublication);
	}

	StageRegistry(builds, outPublication);
}
