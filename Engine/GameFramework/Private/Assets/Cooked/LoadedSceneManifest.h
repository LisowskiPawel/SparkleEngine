#pragma once

#include "Assets/Cooked/CookedSceneManifest.h"
#include "GameFramework/Public/Assets/Cooked/CookedSceneCameraRecord.h"
#include "GameFramework/Public/Assets/Cooked/CookedSceneLightRecord.h"
#include "GameFramework/Public/Assets/Cooked/CookedSceneMetadataRecords.h"

#include <vector>

namespace Assets
{
	struct LoadedSceneManifest
	{
		CookedSceneManifestHeader header;
		std::vector<CookedSceneMeshAssetRef> meshAssetReferences;
		std::vector<CookedSceneMaterialAssetRef> materialAssetReferences;
		std::vector<CookedSceneInstanceRecord> instances;
		std::vector<CookedSceneInstanceGroupRecord> instanceGroups;
		std::vector<CookedSceneCameraRecord> cameras;
		std::vector<CookedSceneLightRecord> lights;
		std::vector<CookedSceneSkeletonRef> skeletonRefs;
		std::vector<CookedAnimationReference> animationReferences;
		std::vector<float> morphWeights;
		std::vector<CookedSceneMaterialVariantRecord> materialVariants;
		std::vector<CookedSceneMaterialVariantMappingRecord> materialVariantMappings;
	};
}
