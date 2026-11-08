#include "PCH.h"

#include "MeshExtractionSystemExecution.h"

#include "World/Extraction/WorldExtractionStorage.h"
#include "World/Resources/MeshResourceStore.h"
#include "World/Resources/SkeletonResourceStore.h"

namespace ECS
{
	MeshExtractionSystemExecution::MeshExtractionSystemExecution(
	    MeshExtractionQuery& query,
	    const MeshResourceStore& meshes,
	    const SkeletonResourceStore& skeletons,
	    std::span<WorldExtractionStorage::MeshSlot> slots) :
	    m_query(query),
	    m_meshes(meshes),
	    m_skeletons(skeletons),
	    m_slots(slots)
	{
	}

	std::uint32_t MeshExtractionSystemExecution::GetMeshCount() const noexcept
	{
		return static_cast<std::uint32_t>(m_query.GetEstimatedEntityCount());
	}

	bool MeshExtractionSystemExecution::Run(std::uint32_t begin, std::uint32_t end)
	{
		return m_query
		    .ForEachRange(
		        begin,
		        end,
		        [this](std::size_t index, EntityId entity, const MeshInstance& mesh, const Visibility& visibility, const WorldTransform& world)
		        {
			        WorldExtractionStorage::MeshSlot& slot = m_slots[index];
			        slot.Entity = entity;
			        const std::shared_ptr<const Mesh> resource = m_meshes.ResolveImmutable(mesh.Resource);
			        slot.bIncluded = resource != nullptr;
			        if (!slot.bIncluded)
			        {
				        return;
			        }
			        slot.Mesh = ImmutableRenderMeshHandle(mesh.MeshAssetId, mesh.Resource.Generation, resource);
			        slot.WorldMatrix = world.Matrix;
			        DirectX::XMStoreFloat3x4(&slot.WorldInverseTranspose, DirectX::XMLoadFloat4x4(&world.InverseTranspose));
			        slot.Material = mesh.Material;
			        const SkeletonResourceHandle skeleton = m_skeletons.Find(mesh.SkeletonAssetId);
			        slot.Skeleton = skeleton.IsValid() ? RenderSkeletonAssetHandle(mesh.SkeletonAssetId) : RenderSkeletonAssetHandle{};
			        slot.Kind = mesh.Kind;
			        slot.MeshAssetIndex = mesh.MeshAssetIndex;
			        slot.InstanceGroupIndex = mesh.InstanceGroupIndex;
			        slot.bVisible = visibility.bVisible;
		        })
		    .Succeeded();
	}
}
