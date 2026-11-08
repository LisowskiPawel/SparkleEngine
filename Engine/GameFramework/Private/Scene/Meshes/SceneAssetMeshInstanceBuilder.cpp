#include "PCH.h"
#include "Scene/Meshes/SceneAssetMeshInstanceBuilder.h"

#include "Core/Public/Diagnostics/Verify.h"
#include "World/Resources/MaterialResourceStore.h"
#include "Scene/Meshes/CookedMesh.h"
#include "Scene/Meshes/SkeletalCookedMesh.h"

#include <vector>
#include <utility>
#include <memory>
#include <span>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_sceneAssetMeshInstanceBuilderLogger, "GameFramework.SceneAssetMeshInstanceBuilder");

static MaterialHandle ResolveMaterial(MaterialHandle payloadMaterial, MaterialHandle materialBaseHandle, MaterialResourceStore& materials)
{
	if (!payloadMaterial.IsValid())
	{
		return materials.GetOrCreateDefault();
	}
	if (!materialBaseHandle.IsValid())
	{
		Diagnostics::Fatal(g_sceneAssetMeshInstanceBuilderLogger, __FILE__, __LINE__, "A mesh instance references a material in a payload with no material block.");
	}
	return MaterialHandle(materialBaseHandle.GetIndex() + payloadMaterial.GetIndex(), materialBaseHandle.GetGeneration());
}

static MaterialHandle ResolveOptionalMaterial(MaterialHandle payloadMaterial, MaterialHandle materialBaseHandle)
{
	if (!payloadMaterial.IsValid())
	{
		return MaterialHandle::Invalid();
	}
	if (!materialBaseHandle.IsValid())
	{
		Diagnostics::Fatal(g_sceneAssetMeshInstanceBuilderLogger, __FILE__, __LINE__, "A mesh instance group references a material in a payload with no material block.");
	}
	return MaterialHandle(materialBaseHandle.GetIndex() + payloadMaterial.GetIndex(), materialBaseHandle.GetGeneration());
}

static Assets::CookedAssetId ResolveGroupAsset(const SceneAssetPayload& payload, const SceneAssetPayload::MeshInstanceGroup& group)
{
	if (group.meshAssetKind == Assets::CookedMeshAssetKind::Skeletal)
	{
		if (group.meshAssetIndex >= payload.skeletalMeshAssets.size())
		{
			Diagnostics::Fatal(g_sceneAssetMeshInstanceBuilderLogger, __FILE__, __LINE__, "A skeletal mesh instance group references an absent mesh asset.");
		}
		return payload.skeletalMeshAssets[group.meshAssetIndex].assetId;
	}
	if (group.meshAssetKind != Assets::CookedMeshAssetKind::Static)
	{
		Diagnostics::Fatal(g_sceneAssetMeshInstanceBuilderLogger, __FILE__, __LINE__, "A mesh instance group has an unsupported mesh kind.");
	}
	if (group.meshAssetIndex >= payload.staticMeshAssets.size())
	{
		Diagnostics::Fatal(g_sceneAssetMeshInstanceBuilderLogger, __FILE__, __LINE__, "A static mesh instance group references an absent mesh asset.");
	}
	return payload.staticMeshAssets[group.meshAssetIndex].assetId;
}

namespace SceneAssetMeshInstanceBuilder
{
	static std::vector<std::shared_ptr<const Mesh>> BuildStaticResources(std::vector<SceneAssetPayload::StaticMeshAsset>& assets)
	{
		std::vector<std::shared_ptr<const Mesh>> resources;
		resources.reserve(assets.size());
		for (SceneAssetPayload::StaticMeshAsset& asset : assets)
		{
			resources.push_back(std::make_shared<CookedMesh>(std::move(asset.mesh), asset.assetId));
		}
		return resources;
	}

	static std::vector<std::shared_ptr<const Mesh>> BuildSkeletalResources(std::vector<SceneAssetPayload::SkeletalMeshAsset>& assets)
	{
		std::vector<std::shared_ptr<const Mesh>> resources;
		resources.reserve(assets.size());
		for (SceneAssetPayload::SkeletalMeshAsset& asset : assets)
		{
			resources.push_back(std::make_shared<SkeletalCookedMesh>(std::move(asset.mesh), asset.assetId));
		}
		return resources;
	}

	static void AppendStaticInstances(
	    const SceneAssetPayload& payload,
	    std::span<const std::shared_ptr<const Mesh>> resources,
	    MaterialResourceStore& materials,
	    MaterialHandle materialBaseHandle,
	    SceneMeshInstanceGroupIndex groupBaseIndex,
	    std::vector<ECS::SceneMeshInstanceData>& instances)
	{
		for (const SceneAssetPayload::StaticMeshInstance& instance : payload.staticMeshInstances)
		{
			if (instance.meshAssetIndex >= payload.staticMeshAssets.size())
			{
				Diagnostics::Fatal(g_sceneAssetMeshInstanceBuilderLogger, __FILE__, __LINE__, "A static mesh instance references an absent mesh asset.");
			}
			const SceneAssetPayload::StaticMeshAsset& asset = payload.staticMeshAssets[instance.meshAssetIndex];
			const SceneMeshInstanceGroupIndex groupIndex = instance.groupIndex == kInvalidSceneMeshInstanceGroupIndex ? kInvalidSceneMeshInstanceGroupIndex : groupBaseIndex + instance.groupIndex;

			instances.push_back(
			    ECS::SceneMeshInstanceData{
			        .Resource = resources[instance.meshAssetIndex],
			        .LocalTransform = instance.transform,
			        .Material = ResolveMaterial(instance.material, materialBaseHandle, materials),
			        .MeshAssetId = asset.assetId,
			        .SourceInstanceId = payload.authoredInstanceId,
			        .MeshAssetIndex = instance.meshAssetIndex,
			        .InstanceGroupIndex = groupIndex,
			        .SourceNodeIndex = instance.sourceNodeIndex,
			        .Kind = SceneMeshKind::Static});
		}
	}

	static void AppendSkeletalInstances(
	    const SceneAssetPayload& payload,
	    std::span<const std::shared_ptr<const Mesh>> resources,
	    MaterialResourceStore& materials,
	    MaterialHandle materialBaseHandle,
	    std::vector<ECS::SceneMeshInstanceData>& instances)
	{
		for (const SceneAssetPayload::SkeletalMeshInstance& instance : payload.skeletalMeshInstances)
		{
			if (instance.meshAssetIndex >= payload.skeletalMeshAssets.size() || instance.skeletonAssetId == Assets::InvalidCookedAssetId)
			{
				Diagnostics::Fatal(g_sceneAssetMeshInstanceBuilderLogger, __FILE__, __LINE__, "A skeletal mesh instance has an invalid mesh or skeleton asset identity.");
			}
			const SceneAssetPayload::SkeletalMeshAsset& asset = payload.skeletalMeshAssets[instance.meshAssetIndex];

			instances.push_back(
			    ECS::SceneMeshInstanceData{
			        .Resource = resources[instance.meshAssetIndex],
			        .LocalTransform = instance.transform,
			        .Material = ResolveMaterial(instance.material, materialBaseHandle, materials),
			        .MeshAssetId = asset.assetId,
			        .SkeletonAssetId = instance.skeletonAssetId,
			        .SourceInstanceId = payload.authoredInstanceId,
			        .MeshAssetIndex = instance.meshAssetIndex,
			        .SourceNodeIndex = instance.sourceNodeIndex,
			        .Kind = SceneMeshKind::Skeletal,
			        .InitialMorphWeights = instance.morphWeights});
		}
	}

	std::vector<ECS::SceneMeshInstanceData> BuildInstances(SceneAssetPayload& payload, MaterialResourceStore& materials, MaterialHandle materialBaseHandle, SceneMeshInstanceGroupIndex groupBaseIndex)
	{
		const auto staticResources = BuildStaticResources(payload.staticMeshAssets);
		const auto skeletalResources = BuildSkeletalResources(payload.skeletalMeshAssets);
		std::vector<ECS::SceneMeshInstanceData> instances;
		instances.reserve(payload.staticMeshInstances.size() + payload.skeletalMeshInstances.size());
		AppendStaticInstances(payload, staticResources, materials, materialBaseHandle, groupBaseIndex, instances);
		AppendSkeletalInstances(payload, skeletalResources, materials, materialBaseHandle, instances);
		return instances;
	}

	std::vector<SceneMeshInstanceGroupData> BuildGroups(const SceneAssetPayload& payload, MaterialHandle materialBaseHandle, SceneMeshInstanceIndex meshBaseIndex)
	{
		std::vector<SceneMeshInstanceGroupData> groups;
		groups.reserve(payload.meshInstanceGroups.size());
		for (const SceneAssetPayload::MeshInstanceGroup& source : payload.meshInstanceGroups)
		{
			groups.push_back(
			    SceneMeshInstanceGroupData{
			        .meshAssetId = ResolveGroupAsset(payload, source),
			        .meshAssetIndex = source.meshAssetIndex,
			        .materialHandle = ResolveOptionalMaterial(source.material, materialBaseHandle),
			        .firstInstance = source.firstInstance == kInvalidSceneMeshInstanceIndex ? kInvalidSceneMeshInstanceIndex : meshBaseIndex + source.firstInstance,
			        .instanceCount = source.instanceCount,
			        .groupKind = source.groupKind,
			        .flags = source.flags});
		}
		return groups;
	}
}
