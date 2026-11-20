#include "PCH.h"

#include "World/Loading/GameWorldSceneAssetCommitter.h"

#include "Assets/SceneAssetPayload.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Scene/Camera/SceneCameraEntry.h"
#include "Scene/Materials/SceneMaterialVariantTranslator.h"
#include "Scene/Meshes/SceneAssetMeshInstanceBuilder.h"
#include "World/GameWorldState.h"
#include "World/Resources/GameWorldResourceStores.h"

#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_sceneAssetCommitterLogger, "GameFramework.SceneAssetCommitter");

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
	for (ECS::SceneMeshInstanceData& meshInstance : meshInstances)
	{
		const EntityId entity = state.AddMesh(std::move(meshInstance));
		if (!entity.IsValid())
		{
			Diagnostics::Fatal(g_sceneAssetCommitterLogger, __FILE__, __LINE__, "The staged world rejected a mesh instance.");
		}
	}

	std::vector<SceneMeshInstanceGroupData> groups = SceneAssetMeshInstanceBuilder::BuildGroups(sceneAssetPayload, materialBaseHandle, sceneMeshBaseIndex);
	state.AppendMeshInstanceGroups(std::move(groups));
}

static void AppendMaterialVariants(
    const ECS::GameWorldState& state,
    MaterialVariantResourceStore& variants,
    const SceneAssetPayload& sceneAssetPayload,
    MaterialHandle materialBaseHandle,
    SceneMeshInstanceIndex sceneMeshBaseIndex)
{
	std::vector<MaterialVariantDesc> descriptions = SceneMaterialVariantTranslator::BuildDescriptions(sceneAssetPayload);
	const auto meshEntities = state.GetMeshEntities().subspan(sceneMeshBaseIndex);
	std::vector<MaterialVariantBinding> bindings = SceneMaterialVariantTranslator::BuildBindings(sceneAssetPayload, materialBaseHandle, meshEntities);
	variants.Append(std::move(descriptions), std::move(bindings));
}

static void AppendCameras(ECS::GameWorldState& state, SceneAssetPayload& sceneAssetPayload)
{
	for (SceneAssetPayload::Camera& camera : sceneAssetPayload.cameras)
	{
		SceneCameraEntry sceneCamera;
		sceneCamera.name = std::move(camera.name);
		sceneCamera.desc = camera.desc;
		const EntityId entity = state.AddCamera(std::move(sceneCamera));
		if (!entity.IsValid())
		{
			Diagnostics::Fatal(g_sceneAssetCommitterLogger, __FILE__, __LINE__, "The staged world rejected a scene camera.");
		}
	}
}

static void AppendLights(ECS::GameWorldState& state, SceneAssetPayload& sceneAssetPayload)
{
	for (SceneLightDesc& light : sceneAssetPayload.lights)
	{
		const EntityId entity = state.AddLight(std::move(light));
		if (!entity.IsValid())
		{
			Diagnostics::Fatal(g_sceneAssetCommitterLogger, __FILE__, __LINE__, "The staged world rejected a scene light.");
		}
	}
}

void CommitSceneAssetPayload(ECS::GameWorldState& state, GameWorldResourceStores& resources, SceneAssetPayload&& sceneAssetPayload)
{
	const bool bHasSkeletons = !sceneAssetPayload.skeletons.empty();
	const bool bHasMaterials = !sceneAssetPayload.materials.empty();

	AppendAnimationResources(state, resources, sceneAssetPayload);
	const MaterialHandle materialBaseHandle = AppendMaterials(resources, sceneAssetPayload);
	const auto sceneMeshBaseIndex = static_cast<SceneMeshInstanceIndex>(state.GetMeshEntities().size());
	AppendMeshes(state, resources.Materials, sceneAssetPayload, materialBaseHandle, sceneMeshBaseIndex);
	AppendMaterialVariants(state, resources.MaterialVariants, sceneAssetPayload, materialBaseHandle, sceneMeshBaseIndex);
	AppendCameras(state, sceneAssetPayload);
	AppendLights(state, sceneAssetPayload);

	if (bHasSkeletons)
	{
		state.NotifyResourceChanged();
	}
	if (bHasMaterials)
	{
		state.NotifyResourceChanged();
	}
}
