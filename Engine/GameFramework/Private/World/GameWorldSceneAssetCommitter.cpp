#include "PCH.h"

#include "World/GameWorldSceneAssetCommitter.h"

#include "Assets/SceneAssetPayload.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Scene/Camera/SceneCameraEntry.h"
#include "Scene/Materials/SceneMaterialVariantTranslator.h"
#include "Scene/Meshes/SceneAssetMeshInstanceBuilder.h"
#include "World/GameWorldState.h"
#include "World/Resources/GameWorldResourceStores.h"

#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_sceneAssetCommitterLogger, "GameFramework.SceneAssetCommitter");

static void ValidatePayloadContents(const SceneAssetPayload& sceneAssetPayload)
{
	if (!sceneAssetPayload.HasMeshes() && sceneAssetPayload.cameras.empty() && sceneAssetPayload.lights.empty() && sceneAssetPayload.skeletons.empty() && sceneAssetPayload.animations.empty()
	    && sceneAssetPayload.materials.empty() && sceneAssetPayload.materialVariants.empty())
	{
		Diagnostics::Fatal(g_sceneAssetCommitterLogger, __FILE__, __LINE__, "A validated scene asset payload contains no resources or entities.");
	}
}

static void AppendAnimationResources(ECS::GameWorldState& state, GameWorldResourceStores& resources, SceneAssetPayload& sceneAssetPayload)
{
	if (!sceneAssetPayload.skeletons.empty())
	{
		resources.Skeletons.Append(std::move(sceneAssetPayload.skeletons));
	}
	if (!sceneAssetPayload.animations.empty())
	{
		state.AppendAnimationClips(std::move(sceneAssetPayload.animations), resources.AnimationClips, sceneAssetPayload.authoredInstanceId);
	}
}

static MaterialHandle AppendMaterials(GameWorldResourceStores& resources, SceneAssetPayload& sceneAssetPayload)
{
	if (!sceneAssetPayload.materials.empty())
	{
		resources.Textures.AppendMaterialReferences(sceneAssetPayload.materials);
	}

	return sceneAssetPayload.materials.empty() ? MaterialHandle::Invalid() : resources.Materials.Append(std::move(sceneAssetPayload.materials));
}

static void AppendMeshes(
    ECS::GameWorldState& state,
    MaterialResourceStore& materials,
    SceneAssetPayload& sceneAssetPayload,
    MaterialHandle materialBaseHandle,
    SceneMeshInstanceIndex sceneMeshBaseIndex)
{
	const auto sceneGroupBaseIndex = static_cast<SceneMeshInstanceGroupIndex>(state.GetMeshInstanceGroupCount());

	std::vector<ECS::SceneMeshInstanceData> meshInstances = SceneAssetMeshInstanceBuilder::BuildInstances(sceneAssetPayload, materials, materialBaseHandle, sceneGroupBaseIndex);
	for (const ECS::SceneMeshInstanceData& meshInstance : meshInstances)
	{
		if (!materials.Contains(meshInstance.Material))
		{
			Diagnostics::Fatal(g_sceneAssetCommitterLogger, __FILE__, __LINE__, "A validated mesh instance resolved an absent material.");
		}
	}

	for (ECS::SceneMeshInstanceData& meshInstance : meshInstances)
	{
		if (!state.AddMesh(std::move(meshInstance)).IsValid())
		{
			Diagnostics::Fatal(g_sceneAssetCommitterLogger, __FILE__, __LINE__, "The staged world rejected a mesh instance.");
		}
	}

	state.AppendMeshInstanceGroups(SceneAssetMeshInstanceBuilder::BuildGroups(sceneAssetPayload, materialBaseHandle, sceneMeshBaseIndex));
}

static void AppendMaterialVariants(
    const ECS::GameWorldState& state,
    MaterialVariantResourceStore& variants,
    const SceneAssetPayload& sceneAssetPayload,
    MaterialHandle materialBaseHandle,
    SceneMeshInstanceIndex sceneMeshBaseIndex)
{
	variants.Append(
	    SceneMaterialVariantTranslator::BuildDescriptions(sceneAssetPayload),
	    SceneMaterialVariantTranslator::BuildBindings(sceneAssetPayload, materialBaseHandle, sceneMeshBaseIndex, state));
}

static void AppendCameras(ECS::GameWorldState& state, SceneAssetPayload& sceneAssetPayload)
{
	for (SceneAssetPayload::Camera& camera : sceneAssetPayload.cameras)
	{
		SceneCameraEntry sceneCamera;
		sceneCamera.name = std::move(camera.name);
		sceneCamera.desc = camera.desc;
		if (!state.AddCamera(std::move(sceneCamera)).IsValid())
		{
			Diagnostics::Fatal(g_sceneAssetCommitterLogger, __FILE__, __LINE__, "The staged world rejected a scene camera.");
		}
	}
}

static void AppendLights(ECS::GameWorldState& state, SceneAssetPayload& sceneAssetPayload)
{
	for (SceneLightDesc& light : sceneAssetPayload.lights)
	{
		if (!state.AddLight(std::move(light)).IsValid())
		{
			Diagnostics::Fatal(g_sceneAssetCommitterLogger, __FILE__, __LINE__, "The staged world rejected a scene light.");
		}
	}
}

void CommitSceneAssetPayload(ECS::GameWorldState& state, GameWorldResourceStores& resources, SceneAssetPayload&& sceneAssetPayload)
{
	ValidatePayloadContents(sceneAssetPayload);
	const bool hasSkeletons = !sceneAssetPayload.skeletons.empty();
	const bool hasMaterials = !sceneAssetPayload.materials.empty();

	AppendAnimationResources(state, resources, sceneAssetPayload);
	const MaterialHandle materialBaseHandle = AppendMaterials(resources, sceneAssetPayload);
	const auto sceneMeshBaseIndex = static_cast<SceneMeshInstanceIndex>(state.GetMeshCount());
	AppendMeshes(state, resources.Materials, sceneAssetPayload, materialBaseHandle, sceneMeshBaseIndex);
	AppendMaterialVariants(state, resources.MaterialVariants, sceneAssetPayload, materialBaseHandle, sceneMeshBaseIndex);
	AppendCameras(state, sceneAssetPayload);
	AppendLights(state, sceneAssetPayload);

	if (hasSkeletons)
	{
		state.NotifyResourceChanged(WorldDataKind::Skeleton);
	}
	if (hasMaterials)
	{
		state.NotifyResourceChanged(WorldDataKind::Material);
		state.NotifyResourceChanged(WorldDataKind::Texture);
	}
}
