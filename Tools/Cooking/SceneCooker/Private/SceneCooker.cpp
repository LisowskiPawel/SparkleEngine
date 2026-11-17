#include "PCH.h"
#include "SceneCooker.h"
#include "CookedSceneBuild.h"
#include "Features/Animations/CookedAnimationAssetBuilder.h"
#include "Features/Cameras/CookedSceneCameraBuilder.h"
#include "Features/Instances/CookedSceneInstanceBuilder.h"
#include "Features/Lights/CookedSceneLightBuilder.h"
#include "Features/MaterialVariants/CookedSceneMaterialVariantBuilder.h"
#include "Features/Metadata/CookedSceneMetadataBuilder.h"
#include "Features/Skeletons/CookedSceneSkeletonBuilder.h"

void SceneCooker::BuildManifest(const SourceImportOutput& importOutput, CookedSceneBuild& outBuild)
{
	outBuild.manifest = {};
	outBuild.manifest.meshAssetReferences.reserve(outBuild.outputs.meshAssets.size());
	for (const CookedMeshAssetBuild& meshAsset : outBuild.outputs.meshAssets)
	{
		outBuild.manifest.meshAssetReferences.push_back({meshAsset.assetId, meshAsset.assetKind});
	}
	outBuild.manifest.materialAssetReferences.reserve(outBuild.outputs.materialAssets.size());
	for (const CookedMaterialAssetBuild& materialAsset : outBuild.outputs.materialAssets)
	{
		outBuild.manifest.materialAssetReferences.push_back(materialAsset.assetId);
	}

	CookedSceneSkeletonBuilder::BuildSkeletons(importOutput, outBuild.identity.assetId, outBuild);
	CookedAnimationAssetBuilder::Build(importOutput, outBuild.identity.assetId, outBuild);
	CookedSceneInstanceBuilder::BuildInstances(importOutput, outBuild);
	CookedSceneMaterialVariantBuilder::BuildMaterialVariants(importOutput, outBuild);
	CookedSceneCameraBuilder::BuildCameras(importOutput, outBuild);
	CookedSceneLightBuilder::BuildLights(importOutput, outBuild);
	CookedSceneMetadataBuilder::BuildMetadata(importOutput, outBuild);
}
