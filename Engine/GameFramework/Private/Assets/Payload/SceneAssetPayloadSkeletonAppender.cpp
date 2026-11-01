#include "PCH.h"

#include "Assets/Payload/SceneAssetPayloadSkeletonAppender.h"

#include "Assets/Cooked/LoadedSkeletonAsset.h"
#include "Assets/Loaders/SkeletonAssetLoader.h"
#include "Assets/Loaders/CookedAssetFileSet.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "Core/Public/Paths/ProductFilePaths.h"
#include "Assets/Translators/SceneAssetSkeletonTranslator.h"

#include <filesystem>
#include <format>

namespace Assets
{
	void SceneAssetPayloadSkeletonAppender::AppendSkeletons(const LoadedSceneManifest& sceneManifest, CookedAssetFileSet& files, SceneAssetPayload& sceneAssetPayload)
	{
		SkeletonAssetLoader skeletonAssetLoader;
		sceneAssetPayload.skeletons.reserve(sceneAssetPayload.skeletons.size() + sceneManifest.skeletonRefs.size());

		for (const CookedSceneSkeletonRef& skeletonRef : sceneManifest.skeletonRefs)
		{
			const std::filesystem::path skeletonAssetPath = Paths::CookedSkeletonAsset(skeletonRef.skeletonAssetId);
			const LoadedSkeletonAsset skeletonAsset = skeletonAssetLoader.Decode(skeletonAssetPath, files.Get(skeletonAssetPath), files.GetAllocationBudget());
			files.GetAllocationBudget().ReserveArray<SkeletonJoint>(skeletonAsset.joints.size());
			files.GetAllocationBudget().Reserve(skeletonAsset.joints.size(), sizeof(CookedSkeletonJointRecord{}.name));
			sceneAssetPayload.skeletons.push_back(BuildSceneAssetSkeleton(skeletonAsset, skeletonRef.skeletonAssetId, skeletonRef.sourceSkinIndex));
			files.Release(skeletonAssetPath);
		}
	}
}
