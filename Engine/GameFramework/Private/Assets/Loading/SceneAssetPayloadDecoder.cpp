#include "PCH.h"

#include "Assets/Loading/SceneAssetPayloadDecoder.h"

#include "Assets/Cooked/LoadedSceneManifest.h"
#include "Assets/Cooked/LoadedMaterialAsset.h"
#include "Assets/CookedAssembly/CookedMaterialTranslator.h"
#include "Assets/Loaders/CookedAssetAllocationBudget.h"
#include "Assets/Loaders/AnimationAssetLoader.h"
#include "Assets/Loaders/SkeletonAssetLoader.h"
#include "Assets/Loaders/MaterialAssetLoader.h"
#include "Assets/Loaders/CookedAssetFileSet.h"
#include "Assets/Payload/SceneAssetPayloadCameraLightAppender.h"
#include "Assets/Payload/SceneAssetPayloadMaterialVariantAppender.h"
#include "Assets/Payload/SceneAssetPayloadMeshAppender.h"
#include "Assets/Payload/SceneAssetPayloadMeshBindings.h"
#include "Core/Public/Paths/CookedAssetPaths.h"

#include <filesystem>
#include <utility>

namespace Assets
{
	static void DecodeMaterials(const LoadedSceneManifest& manifest, CookedAssetFileSet& files, SceneAssetPayload& payload)
	{
		CookedAssetAllocationBudget& budget = files.GetAllocationBudget();
		budget.ReserveArray<MaterialDesc>(manifest.materialAssetReferences.size());
		payload.materials.reserve(payload.materials.size() + manifest.materialAssetReferences.size());

		for (const CookedSceneMaterialAssetRef& materialReference : manifest.materialAssetReferences)
		{
			const std::filesystem::path materialAssetPath = Paths::CookedMaterialAsset(materialReference);
			const auto bytes = files.Get(materialAssetPath);
			const LoadedMaterialAsset materialAsset = MaterialAssetLoader::Decode(bytes, budget);
			budget.Reserve(bytes.size());
			MaterialDesc material = CookedMaterialTranslator::Translate(materialAsset);
			payload.materials.push_back(std::move(material));
			files.Release(materialAssetPath);
		}
	}

	static void DecodeSkeletons(const LoadedSceneManifest& manifest, CookedAssetFileSet& files, SceneAssetPayload& payload)
	{
		CookedAssetAllocationBudget& budget = files.GetAllocationBudget();
		budget.ReserveArray<SkeletonResource>(manifest.skeletonRefs.size());
		payload.skeletons.reserve(payload.skeletons.size() + manifest.skeletonRefs.size());

		for (const CookedSceneSkeletonRef& skeletonRef : manifest.skeletonRefs)
		{
			const std::filesystem::path skeletonAssetPath = Paths::CookedSkeletonAsset(skeletonRef.skeletonAssetId);
			const auto bytes = files.Get(skeletonAssetPath);
			SkeletonResource skeleton = SkeletonAssetLoader::Load(bytes, skeletonRef.skeletonAssetId, budget);
			payload.skeletons.push_back(std::move(skeleton));
			files.Release(skeletonAssetPath);
		}
	}

	static void DecodeAnimations(const LoadedSceneManifest& manifest, CookedAssetFileSet& files, SceneAssetPayload& payload)
	{
		CookedAssetAllocationBudget& budget = files.GetAllocationBudget();
		budget.ReserveArray<AnimationClipResource>(manifest.animationReferences.size());
		payload.animations.reserve(payload.animations.size() + manifest.animationReferences.size());

		for (const CookedAnimationReference& animationRef : manifest.animationReferences)
		{
			const std::filesystem::path animationAssetPath = Paths::CookedAnimationAsset(animationRef.animationAssetId);
			const auto bytes = files.Get(animationAssetPath);
			AnimationClipResource animation = AnimationAssetLoader::Load(bytes, animationRef.animationAssetId, payload.skeletons, budget);
			payload.animations.push_back(std::move(animation));
			files.Release(animationAssetPath);
		}
	}

	SceneAssetPayload SceneAssetPayloadDecoder::Decode(const LoadedSceneManifest& manifest, CookedAssetFileSet& files)
	{
		SceneAssetPayload payload;
		CookedAssetAllocationBudget& budget = files.GetAllocationBudget();
		const std::vector<SceneAssetPayloadMeshBinding> meshAssetBindings = BuildSceneAssetPayloadMeshBindings(manifest, budget);
		SceneAssetPayloadMeshAppender::AppendMeshAssets(manifest, files, payload);
		DecodeMaterials(manifest, files, payload);
		SceneAssetPayloadMeshAppender::AppendMeshInstances(manifest, meshAssetBindings, payload, budget);
		SceneAssetPayloadMeshAppender::AppendMeshInstanceGroups(manifest, meshAssetBindings, payload, budget);
		SceneAssetPayloadMaterialVariantAppender::AppendMaterialVariants(manifest, meshAssetBindings, payload, budget);
		DecodeSkeletons(manifest, files, payload);
		DecodeAnimations(manifest, files, payload);
		SceneAssetPayloadCameraLightAppender::Append(manifest, payload, budget);
		return payload;
	}
}
