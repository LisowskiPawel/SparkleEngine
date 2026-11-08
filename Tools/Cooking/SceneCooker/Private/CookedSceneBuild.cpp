#include "PCH.h"

#include "CookedSceneBuild.h"
#include "CookedMaterialAssetBuild.h"
#include "CookedMeshAssetBuild.h"

#include <utility>

void CookedSceneBuild::ApplyMeshOutput(MeshCookOutput&& meshOutput)
{
	manifest.meshAssetReferences = std::move(meshOutput.assetReferences);
	outputs.meshAssets = std::move(meshOutput.assets);
}

void CookedSceneBuild::ApplyMaterialOutput(MaterialCookOutput&& materialOutput)
{
	manifest.materialAssetReferences = std::move(materialOutput.assetReferences);
	outputs.materialAssets = std::move(materialOutput.assets);
}

void CookedSceneBuild::ResetManifest()
{
	manifest.instances.clear();
	manifest.instanceGroups.clear();
	manifest.morphWeights.clear();
	manifest.materialVariants.clear();
	manifest.materialVariantMappings.clear();
}

void CookedSceneBuild::FinalizeManifestHeader() noexcept
{
	Assets::CookedSceneManifestHeader& header = manifest.header;
	header.meshAssetReferenceCount = static_cast<std::uint32_t>(manifest.meshAssetReferences.size());
	header.materialAssetReferenceCount = static_cast<std::uint32_t>(manifest.materialAssetReferences.size());
	header.instanceCount = static_cast<std::uint32_t>(manifest.instances.size());
	header.instanceGroupCount = static_cast<std::uint32_t>(manifest.instanceGroups.size());
	header.cameraCount = static_cast<std::uint32_t>(manifest.cameras.size());
	header.lightCount = static_cast<std::uint32_t>(manifest.lights.size());
	header.skeletonRefCount = static_cast<std::uint32_t>(manifest.skeletonRefs.size());
	header.animationRefCount = static_cast<std::uint32_t>(manifest.animationReferences.size());
	header.morphWeightCount = static_cast<std::uint32_t>(manifest.morphWeights.size());
	header.materialVariantCount = static_cast<std::uint32_t>(manifest.materialVariants.size());
	header.materialVariantMappingCount = static_cast<std::uint32_t>(manifest.materialVariantMappings.size());
}
