#include "PCH.h"

#include "MeshExtractionSystemExecution.h"

#include "World/Extraction/WorldExtractionStorage.h"
#include "World/Resources/MeshResourceStore.h"
#include "World/Resources/SkeletonResourceStore.h"
#include "World/ECS/EntityRegistry.h"

namespace ECS
{
	MeshExtractionSystemExecution::MeshExtractionSystemExecution(
	    const EntityRegistry& registry,
	    const MeshResourceStore& meshes,
	    const SkeletonResourceStore& skeletons,
	    std::span<WorldExtractionStorage::MeshSlot> slots) :
	    m_registry(registry),
	    m_meshes(meshes),
	    m_skeletons(skeletons),
	    m_slots(slots)
	{
	}

	std::uint32_t MeshExtractionSystemExecution::GetMeshCount() const noexcept
	{
		return static_cast<std::uint32_t>(m_registry.GetStorage<MeshInstance>().GetEntities().size());
	}

	bool MeshExtractionSystemExecution::Run(std::uint32_t begin, std::uint32_t end)
	{
		const auto& meshes = m_registry.GetStorage<MeshInstance>();
		const auto entities = meshes.GetEntities();
		const auto components = meshes.GetComponents();
		const bool bInvalidRange = begin > end || end > entities.size();
		if (bInvalidRange || end > m_slots.size())
		{
			return false;
		}
		for (std::uint32_t index = begin; index < end; ++index)
		{
			ExtractMesh(entities[index], components[index], m_slots[index]);
		}
		return true;
	}

	void MeshExtractionSystemExecution::ExtractMesh(EntityId entity, const MeshInstance& mesh, WorldExtractionStorage::MeshSlot& slot) const
	{
		const Visibility* visibility = m_registry.Get<Visibility>(entity);
		const WorldTransform* world = m_registry.Get<WorldTransform>(entity);
		if (visibility == nullptr || world == nullptr)
		{
			return;
		}
		slot.Entity = entity;
		const std::shared_ptr<const Mesh> resource = m_meshes.ResolveImmutable(mesh.Resource);
		slot.bIncluded = resource != nullptr;
		if (!slot.bIncluded)
		{
			return;
		}
		slot.Mesh = ImmutableRenderMeshHandle(mesh.MeshAssetId, mesh.Resource.Generation, resource);
		slot.WorldMatrix = world->Matrix;
		DirectX::XMStoreFloat3x4(&slot.WorldInverseTranspose, DirectX::XMLoadFloat4x4(&world->InverseTranspose));
		slot.Material = mesh.Material;
		const SkeletonResourceHandle skeleton = m_skeletons.Find(mesh.SkeletonAssetId);
		slot.Skeleton = skeleton.IsValid() ? RenderSkeletonAssetHandle(mesh.SkeletonAssetId) : RenderSkeletonAssetHandle{};
		slot.Kind = mesh.Kind;
		slot.MeshAssetIndex = mesh.MeshAssetIndex;
		slot.InstanceGroupIndex = mesh.InstanceGroupIndex;
		slot.bVisible = visibility->bVisible;
	}
}
