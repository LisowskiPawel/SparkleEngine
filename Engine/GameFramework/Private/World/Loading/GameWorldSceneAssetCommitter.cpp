#include "PCH.h"

#include "World/Loading/GameWorldSceneAssetCommitter.h"

#include "Assets/SceneAssetPayload.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Scene/Camera/SceneCameraEntry.h"
#include "Core/Public/Diagnostics/Error.h"
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
	for (ECS::SceneMeshInstanceData& meshInstance : sceneAssetPayload.meshes)
	{
		meshInstance.Material = meshInstance.Material.IsValid() ? MaterialHandle(materialBaseHandle.GetIndex() + meshInstance.Material.GetIndex(), materialBaseHandle.GetGeneration())
		                                                        : materials.GetOrCreateDefault();
		meshInstance.SourceInstanceId = sceneAssetPayload.authoredInstanceId;
		if (meshInstance.InstanceGroupIndex != kInvalidSceneMeshInstanceGroupIndex)
		{
			meshInstance.InstanceGroupIndex += sceneGroupBaseIndex;
		}
		const EntityId entity = state.AddMesh(std::move(meshInstance));
		if (!entity.IsValid())
		{
			Diagnostics::Fatal(g_sceneAssetCommitterLogger, __FILE__, __LINE__, "The staged world rejected a mesh instance.");
		}
	}

	for (SceneMeshInstanceGroupData& group : sceneAssetPayload.meshInstanceGroups)
	{
		if (group.materialHandle.IsValid())
		{
			group.materialHandle = MaterialHandle(materialBaseHandle.GetIndex() + group.materialHandle.GetIndex(), materialBaseHandle.GetGeneration());
		}
		if (group.firstInstance != kInvalidSceneMeshInstanceIndex)
		{
			group.firstInstance += sceneMeshBaseIndex;
		}
	}
	state.AppendMeshInstanceGroups(std::move(sceneAssetPayload.meshInstanceGroups));
}

static void AppendMaterialVariants(
    const ECS::GameWorldState& state,
    MaterialVariantResourceStore& variants,
    SceneAssetPayload& sceneAssetPayload,
    MaterialHandle materialBaseHandle,
    SceneMeshInstanceIndex sceneMeshBaseIndex)
{
	const auto meshEntities = state.GetMeshEntities().subspan(sceneMeshBaseIndex);
	if (meshEntities.size() != sceneAssetPayload.meshes.size())
	{
		throw Diagnostics::Error("Material-variant bindings require the complete staged mesh entity range.");
	}
	std::vector<MaterialVariantBinding> bindings;
	bindings.reserve(sceneAssetPayload.materialVariantMappings.size());
	for (const auto& mapping : sceneAssetPayload.materialVariantMappings)
	{
		if (!mapping.material.IsValid() || !materialBaseHandle.IsValid())
		{
			continue;
		}
		const MaterialHandle material(materialBaseHandle.GetIndex() + mapping.material.GetIndex(), materialBaseHandle.GetGeneration());
		for (std::size_t index = 0; index < sceneAssetPayload.meshes.size(); ++index)
		{
			const auto& mesh = sceneAssetPayload.meshes[index];
			if (mesh.Kind == mapping.meshKind && mesh.MeshAssetIndex == mapping.meshAssetIndex)
			{
				bindings.push_back({.Variant = mapping.variantIndex, .Entity = meshEntities[index], .Material = material});
			}
		}
	}
	variants.Append(std::move(sceneAssetPayload.materialVariants), std::move(bindings));
}

static void AppendCameras(ECS::GameWorldState& state, SceneAssetPayload& sceneAssetPayload)
{
	for (SceneCameraEntry& camera : sceneAssetPayload.cameras)
	{
		const EntityId entity = state.AddCamera(std::move(camera));
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
