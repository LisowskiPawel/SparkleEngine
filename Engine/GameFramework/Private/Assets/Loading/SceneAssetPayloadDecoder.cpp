#include "PCH.h"

#include "Assets/Loading/SceneAssetPayloadDecoder.h"

#include "Assets/Cooked/LoadedSceneManifest.h"
#include "Assets/Loaders/CookedAssetFileSet.h"
#include "Assets/Payload/SceneAssetPayloadAnimationAppender.h"
#include "Assets/Payload/SceneAssetPayloadCameraLightAppender.h"
#include "Assets/Payload/SceneAssetPayloadMaterialAppender.h"
#include "Assets/Payload/SceneAssetPayloadMaterialVariantAppender.h"
#include "Assets/Payload/SceneAssetPayloadMeshAppender.h"
#include "Assets/Payload/SceneAssetPayloadMeshBindings.h"
#include "Assets/Payload/SceneAssetPayloadSkeletonAppender.h"

namespace Assets
{
	SceneAssetPayload SceneAssetPayloadDecoder::Decode(const LoadedSceneManifest& manifest, CookedAssetFileSet& files)
	{
		SceneAssetPayload payload;
		CookedAssetAllocationBudget& budget = files.GetAllocationBudget();
		budget.ReserveArray<SceneAssetPayload::StaticMeshAsset>(manifest.meshAssetReferences.size());
		budget.ReserveArray<SceneAssetPayload::SkeletalMeshAsset>(manifest.meshAssetReferences.size());
		budget.ReserveArray<SceneAssetPayloadMeshBinding>(manifest.meshAssetReferences.size());
		budget.ReserveArray<SceneAssetPayload::StaticMeshInstance>(manifest.instances.size());
		budget.ReserveArray<SceneAssetPayload::SkeletalMeshInstance>(manifest.instances.size());
		budget.ReserveArray<SceneAssetPayload::MeshInstanceGroup>(manifest.instanceGroups.size());
		budget.ReserveArray<MaterialDesc>(manifest.materialAssetReferences.size());
		budget.ReserveArray<SceneAssetPayload::MaterialVariant>(manifest.materialVariants.size());
		budget.ReserveArray<SceneAssetPayload::MaterialVariantMapping>(manifest.materialVariantMappings.size());
		budget.ReserveArray<SceneAssetPayload::Camera>(manifest.cameras.size());
		budget.ReserveArray<SceneLightDesc>(manifest.lights.size());
		budget.ReserveArray<SkeletonResource>(manifest.skeletonRefs.size());
		budget.ReserveArray<AnimationClipResource>(manifest.animationReferences.size());
		for (const auto& instance : manifest.instances)
		{
			budget.ReserveArray<float>(instance.morphWeightCount);
		}
		budget.Reserve(manifest.cameras.size(), sizeof(CookedSceneCameraRecord));
		budget.Reserve(manifest.lights.size(), sizeof(CookedSceneLightRecord));

		const std::vector<SceneAssetPayloadMeshBinding> meshAssetBindings = BuildSceneAssetPayloadMeshBindings(manifest);
		SceneAssetPayloadMeshAppender::AppendMeshAssets(manifest, files, payload);
		SceneAssetPayloadMaterialAppender::AppendMaterials(manifest, files, payload);
		SceneAssetPayloadMeshAppender::AppendMeshInstances(manifest, meshAssetBindings, payload);
		SceneAssetPayloadMeshAppender::AppendMeshInstanceGroups(manifest, meshAssetBindings, payload);
		SceneAssetPayloadMaterialVariantAppender::AppendMaterialVariants(manifest, meshAssetBindings, payload);
		SceneAssetPayloadSkeletonAppender::AppendSkeletons(manifest, files, payload);
		SceneAssetPayloadAnimationAppender::AppendAnimations(manifest, files, payload);
		SceneAssetPayloadCameraLightAppender::Append(manifest, payload);
		return payload;
	}
}
