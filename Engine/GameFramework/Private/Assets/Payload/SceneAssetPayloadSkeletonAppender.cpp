#include "PCH.h"
#include "GameFramework/Public/Assets/Cooked/CookedSceneMetadataRecords.h"

#include "Assets/Payload/SceneAssetPayloadSkeletonAppender.h"

#include "Assets/Cooked/LoadedSkeletonAsset.h"
#include "Assets/Loaders/SkeletonAssetLoader.h"
#include "Assets/Loaders/CookedAssetAllocationBudget.h"
#include "Assets/Loaders/CookedAssetFileSet.h"
#include "Core/Public/Formatting/HexFormat.h"
#include "Core/Public/Paths/CookedAssetPaths.h"
#include "Assets/Translators/SceneAssetSkeletonTranslator.h"

#include <filesystem>
#include <format>
#include <utility>

namespace Assets
{
	void SceneAssetPayloadSkeletonAppender::AppendSkeletons(const LoadedSceneManifest& sceneManifest, CookedAssetFileSet& files, SceneAssetPayload& sceneAssetPayload)
	{
		CookedAssetAllocationBudget& budget = files.GetAllocationBudget();
		budget.ReserveArray<SkeletonResource>(sceneManifest.skeletonRefs.size());
		sceneAssetPayload.skeletons.reserve(sceneAssetPayload.skeletons.size() + sceneManifest.skeletonRefs.size());

		for (const CookedSceneSkeletonRef& skeletonRef : sceneManifest.skeletonRefs)
		{
			const std::filesystem::path skeletonAssetPath = Paths::CookedSkeletonAsset(skeletonRef.skeletonAssetId);
			const auto bytes = files.Get(skeletonAssetPath);
			const LoadedSkeletonAsset skeletonAsset = SkeletonAssetLoader::Decode(bytes, budget);
			budget.ReserveArray<SkeletonJoint>(skeletonAsset.joints.size());
			SkeletonResource skeleton = BuildSceneAssetSkeleton(skeletonAsset, skeletonRef.skeletonAssetId, budget);
			sceneAssetPayload.skeletons.push_back(std::move(skeleton));
			files.Release(skeletonAssetPath);
		}
	}
}
