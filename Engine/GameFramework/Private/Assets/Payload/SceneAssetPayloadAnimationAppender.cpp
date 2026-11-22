#include "PCH.h"
#include "GameFramework/Public/Assets/Cooked/CookedSceneMetadataRecords.h"

#include "Assets/Payload/SceneAssetPayloadAnimationAppender.h"

#include "Assets/Loaders/AnimationAssetLoader.h"
#include "Assets/Loaders/CookedAssetAllocationBudget.h"
#include "Assets/Loaders/CookedAssetFileSet.h"
#include "Core/Public/Paths/CookedAssetPaths.h"

#include <filesystem>
#include <utility>

namespace Assets
{
	void SceneAssetPayloadAnimationAppender::AppendAnimations(const LoadedSceneManifest& sceneManifest, CookedAssetFileSet& files, SceneAssetPayload& sceneAssetPayload)
	{
		CookedAssetAllocationBudget& budget = files.GetAllocationBudget();
		budget.ReserveArray<AnimationClipResource>(sceneManifest.animationReferences.size());
		sceneAssetPayload.animations.reserve(sceneAssetPayload.animations.size() + sceneManifest.animationReferences.size());

		for (const CookedAnimationReference& animationRef : sceneManifest.animationReferences)
		{
			const std::filesystem::path animationAssetPath = Paths::CookedAnimationAsset(animationRef.animationAssetId);
			const auto bytes = files.Get(animationAssetPath);
			AnimationClipResource animation = AnimationAssetLoader::Load(bytes, animationRef.animationAssetId, sceneAssetPayload.skeletons, budget);
			sceneAssetPayload.animations.push_back(std::move(animation));
			files.Release(animationAssetPath);
		}
	}
}
