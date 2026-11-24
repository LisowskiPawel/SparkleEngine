#pragma once

#include "Animation/AnimationClipResource.h"
#include "Scene/Camera/SceneCameraEntry.h"
#include "Scene/Lighting/SceneLightDesc.h"
#include "Scene/Materials/MaterialDesc.h"
#include "Animation/SkeletonResource.h"
#include "Scene/Materials/MaterialVariant.h"
#include "World/SceneMeshInstanceData.h"

#include <cstdint>
#include <vector>

struct SceneAssetMaterialVariantMapping final
{
	SceneMeshAssetIndex meshAssetIndex = kInvalidSceneMeshAssetIndex;
	SceneMeshKind meshKind = SceneMeshKind::Static;
	MaterialVariantIndex variantIndex = kInvalidMaterialVariantIndex;
	MaterialHandle material;
};

struct SceneAssetPayload final
{
	std::uint64_t authoredInstanceId = 0;

	std::vector<ECS::SceneMeshInstanceData> meshes;
	std::vector<SceneMeshInstanceGroupData> meshInstanceGroups;
	std::vector<SceneCameraEntry> cameras;
	std::vector<SceneLightDesc> lights;
	std::vector<SkeletonResource> skeletons;
	std::vector<AnimationClipResource> animations;
	std::vector<MaterialDesc> materials;
	std::vector<MaterialVariantDesc> materialVariants;
	std::vector<SceneAssetMaterialVariantMapping> materialVariantMappings;
};
