#include "PCH.h"

#include "Assets/Loading/SceneMeshDecoder.h"
#include "Assets/Cooked/LoadedSceneManifest.h"
#include "Assets/Loaders/CookedAssetFileSet.h"
#include "Assets/Loaders/MeshAssetLoader.h"
#include "Assets/SceneAssetPayload.h"
#include "Core/Public/Paths/CookedAssetPaths.h"
#include "Scene/Transform.h"

#include <DirectXMath.h>

#include <memory>
#include <span>
#include <utility>
#include <vector>

namespace Assets
{
	struct SceneMeshResource final
	{
		std::shared_ptr<const Mesh> Resource;
		SceneMeshAssetIndex Index = kInvalidSceneMeshAssetIndex;
	};

	static std::vector<SceneMeshResource> DecodeMeshResources(const LoadedSceneManifest& manifest, CookedAssetFileSet& files)
	{
		auto& budget = files.GetAllocationBudget();
		budget.ReserveArray<SceneMeshResource>(manifest.meshAssetReferences.size());
		std::vector<SceneMeshResource> resources;
		resources.reserve(manifest.meshAssetReferences.size());
		SceneMeshAssetIndex staticIndex = 0;
		SceneMeshAssetIndex skeletalIndex = 0;
		for (const auto& reference : manifest.meshAssetReferences)
		{
			const auto path = Paths::CookedMeshAsset(reference.meshAssetId);
			const auto bytes = files.Get(path);
			auto mesh = MeshAssetLoader::Decode(bytes, reference.meshAssetId, reference.meshAssetKind, budget);
			const auto index = reference.meshAssetKind == CookedMeshAssetKind::Skeletal ? skeletalIndex++ : staticIndex++;
			resources.push_back({std::move(mesh), index});
			files.Release(path);
		}
		return resources;
	}

	static std::vector<float> DecodeMorphWeights(const LoadedSceneManifest& manifest, const CookedSceneInstanceRecord& instance)
	{
		if (instance.firstMorphWeight == kInvalidCookedSceneMorphWeightIndex || instance.morphWeightCount == 0)
		{
			return {};
		}
		const auto weights = std::span(manifest.morphWeights).subspan(instance.firstMorphWeight, instance.morphWeightCount);
		return {weights.begin(), weights.end()};
	}

	static void DecodeMeshInstances(const LoadedSceneManifest& manifest, std::span<const SceneMeshResource> resources, SceneAssetPayload& payload, CookedAssetAllocationBudget& budget)
	{
		budget.ReserveArray<ECS::SceneMeshInstanceData>(manifest.instances.size());
		payload.meshes.reserve(manifest.instances.size());
		// Keep the existing world/entity order: static instances, then skeletal instances.
		for (const auto kind : {CookedMeshAssetKind::Static, CookedMeshAssetKind::Skeletal})
		{
			for (const auto& instance : manifest.instances)
			{
				const auto& reference = manifest.meshAssetReferences[instance.meshAssetIndex];
				if (reference.meshAssetKind != kind)
				{
					continue;
				}

				const auto& resource = resources[instance.meshAssetIndex];
				const Transform transform(DirectX::XMLoadFloat4x4(&instance.worldTransform));
				const auto material = instance.materialAssetIndex == kInvalidCookedMaterialAssetIndex ? MaterialHandle::Invalid() : MaterialHandle(instance.materialAssetIndex);
				auto& mesh = payload.meshes.emplace_back();
				mesh.Resource = resource.Resource;
				mesh.LocalTransform = transform;
				mesh.Material = material;
				mesh.MeshAssetId = reference.meshAssetId;
				mesh.MeshAssetIndex = resource.Index;
				mesh.SourceNodeIndex = instance.sourceNodeIndex;
				if (kind == CookedMeshAssetKind::Skeletal)
				{
					mesh.Kind = SceneMeshKind::Skeletal;
					mesh.SkeletonAssetId = manifest.skeletonRefs[instance.skeletonRefIndex].skeletonAssetId;
					budget.ReserveArray<float>(instance.morphWeightCount);
					mesh.InitialMorphWeights = DecodeMorphWeights(manifest, instance);
				}
				else
				{
					mesh.InstanceGroupIndex = instance.groupIndex;
				}
			}
		}
	}

	static SceneMeshInstanceGroupKind DecodeGroupKind(CookedSceneInstanceGroupKind kind) noexcept
	{
		switch (kind)
		{
			case CookedSceneInstanceGroupKind::SharedMeshReference:
				return SceneMeshInstanceGroupKind::SharedMeshReference;
			case CookedSceneInstanceGroupKind::AuthoredInstanceGroup:
				return SceneMeshInstanceGroupKind::AuthoredInstanceGroup;
			case CookedSceneInstanceGroupKind::None:
			default:
				return SceneMeshInstanceGroupKind::None;
		}
	}

	static void DecodeMeshGroups(const LoadedSceneManifest& manifest, std::span<const SceneMeshResource> resources, SceneAssetPayload& payload, CookedAssetAllocationBudget& budget)
	{
		budget.ReserveArray<SceneMeshInstanceGroupData>(manifest.instanceGroups.size());
		payload.meshInstanceGroups.reserve(manifest.instanceGroups.size());
		for (const auto& group : manifest.instanceGroups)
		{
			const auto material = group.materialAssetIndex == kInvalidCookedMaterialAssetIndex ? MaterialHandle::Invalid() : MaterialHandle(group.materialAssetIndex);
			payload.meshInstanceGroups.push_back(
			    {.meshAssetId = manifest.meshAssetReferences[group.meshAssetIndex].meshAssetId,
			        .meshAssetIndex = resources[group.meshAssetIndex].Index,
			        .materialHandle = material,
			        .firstInstance = group.firstInstance,
			        .instanceCount = group.instanceCount,
			        .groupKind = DecodeGroupKind(group.groupKind),
			        .flags = group.flags});
		}
	}

	static void DecodeMaterialVariants(const LoadedSceneManifest& manifest, std::span<const SceneMeshResource> resources, SceneAssetPayload& payload, CookedAssetAllocationBudget& budget)
	{
		budget.ReserveArray<MaterialVariantDesc>(manifest.materialVariants.size());
		budget.ReserveArray<SceneAssetMaterialVariantMapping>(manifest.materialVariantMappings.size());
		payload.materialVariants.reserve(manifest.materialVariants.size());
		for (const auto& variant : manifest.materialVariants)
		{
			payload.materialVariants.push_back({variant.name, variant.sourceVariantIndex});
		}

		payload.materialVariantMappings.reserve(manifest.materialVariantMappings.size());
		for (const auto& mapping : manifest.materialVariantMappings)
		{
			const auto kind = manifest.meshAssetReferences[mapping.meshAssetIndex].meshAssetKind;
			payload.materialVariantMappings.push_back(
			    {.meshAssetIndex = resources[mapping.meshAssetIndex].Index,
			        .meshKind = kind == CookedMeshAssetKind::Skeletal ? SceneMeshKind::Skeletal : SceneMeshKind::Static,
			        .variantIndex = mapping.variantIndex,
			        .material = MaterialHandle(mapping.materialAssetIndex)});
		}
	}

	void DecodeSceneMeshes(const LoadedSceneManifest& manifest, CookedAssetFileSet& files, SceneAssetPayload& payload)
	{
		const auto resources = DecodeMeshResources(manifest, files);
		auto& budget = files.GetAllocationBudget();
		DecodeMeshInstances(manifest, resources, payload, budget);
		DecodeMeshGroups(manifest, resources, payload, budget);
		DecodeMaterialVariants(manifest, resources, payload, budget);
	}
}
