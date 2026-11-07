#include "PCH.h"
#include "World/GameWorldState.h"

#include <algorithm>
#include <iterator>

#include "World/ECS/Components/EditorComponents.h"
#include "World/ECS/Components/RenderingComponents.h"
#include "World/ECS/Components/TransformComponents.h"
#include "World/WorldTransformConversion.h"

namespace ECS
{
	EntityId GameWorldState::AddMesh(SceneMeshInstanceData&& instance)
	{
		const MeshResourceHandle resource = m_meshResources.Add(std::move(instance.Resource));
		if (!resource.IsValid())
		{
			return EntityId::Invalid();
		}
		const EntityId entity = m_registry.Create();
		if (!entity.IsValid())
		{
			m_meshResources.Remove(resource);
			return entity;
		}
		const LocalTransform local = WorldTransformConversion::ToLocal(instance.LocalTransform);
		const AnimationOutputSlotHandle morphState = instance.Kind == SceneMeshKind::Skeletal ? m_morphWeights.Add(instance.InitialMorphWeights) : AnimationOutputSlotHandle{};

		const MeshInstance mesh{
		    .Resource = resource,
		    .MeshAssetId = instance.MeshAssetId,
		    .SkeletonAssetId = instance.SkeletonAssetId,
		    .Material = instance.Material,
		    .Kind = instance.Kind,
		    .MeshAssetIndex = instance.MeshAssetIndex,
		    .InstanceGroupIndex = instance.InstanceGroupIndex,
		    .SourceNodeIndex = instance.SourceNodeIndex};

		const bool bTransformsAdded = m_registry.Add(entity, local) && m_registry.Add(entity, WorldTransform{});
		const bool bMeshStored = bTransformsAdded && (m_registry.Add(entity, mesh));
		const bool bVisibilityAdded = bMeshStored && (m_registry.Add(entity, Visibility{}));

		const bool bIdentityAdded = bVisibilityAdded
		    && (m_registry.Add(
		        entity,
		        AuthoredIdentity{
		            .SourceAssetId = instance.MeshAssetId,
		            .SourceInstanceId = instance.SourceInstanceId,
		            .SourceObjectId = static_cast<std::uint64_t>(instance.SourceNodeIndex),
		            .Kind = AuthoredObjectKind::MeshInstance}));

		bool bAdded = bIdentityAdded && (m_registry.Add(entity, EditorMetadata{}));

		const bool bSkeletalMesh = instance.Kind == SceneMeshKind::Skeletal;
		const bool bMorphRequired = bAdded && bSkeletalMesh;
		const bool bMorphValid = bMorphRequired && morphState.IsValid();
		const bool bMorphStored = bMorphValid && m_registry.Add(entity, MorphState{.Weights = morphState});
		const bool bSkinningStored = bMorphStored && m_registry.Add(entity, SkinningState{.SkeletonAssetId = instance.SkeletonAssetId});
		const bool bSkinningComplete = !bSkeletalMesh || bSkinningStored;
		const bool bMeshAdded = bAdded && bSkinningComplete;

		if (!bMeshAdded)
		{
			m_registry.Destroy(entity);
			m_meshResources.Remove(resource);
			if (morphState.IsValid())
			{
				m_morphWeights.Remove(morphState);
			}
			return EntityId::Invalid();
		}
		MarkTransformDirty(entity);
		RecordChange(entity);
		return entity;
	}

	std::size_t GameWorldState::GetMeshCount() const noexcept
	{
		return Count<MeshInstance>();
	}

	EntityId GameWorldState::GetMeshEntity(std::size_t index) const noexcept
	{
		return EntityAt<MeshInstance>(index);
	}

	bool GameWorldState::WriteMeshMaterial(EntityId entity, MaterialHandle material) noexcept
	{
		const MeshInstance* mesh = m_registry.Get<MeshInstance>(entity);
		if (mesh == nullptr)
		{
			return false;
		}
		MeshInstance updated = *mesh;
		updated.Material = material;
		const bool bWritten = m_registry.Replace(entity, updated);
		if (bWritten)
		{
			RecordChange(entity);
		}
		return bWritten;
	}

	void GameWorldState::AppendMeshInstanceGroups(std::vector<SceneMeshInstanceGroupData> groups)
	{
		m_meshInstanceGroups.reserve(m_meshInstanceGroups.size() + groups.size());
		std::ranges::move(groups, std::back_inserter(m_meshInstanceGroups));
		RecordChange(EntityId::Invalid());
	}

}
