#pragma once

#include "GameFramework/Public/Rendering/RenderSceneDelta.h"
#include "GameFramework/Public/World/EntityId.h"

#include <cstddef>
#include <DirectXMath.h>

#include <span>
#include <vector>

namespace ECS
{
	class WorldExtractionStorage final
	{
	public:
		struct MeshSlot final
		{
			EntityId Entity;
			ImmutableRenderMeshHandle Mesh;
			DirectX::XMFLOAT4X4 WorldMatrix{};
			DirectX::XMFLOAT3X4 WorldInverseTranspose{};
			MaterialHandle Material = MaterialHandle::Invalid();
			RenderSkeletonAssetHandle Skeleton;
			SceneMeshKind Kind = SceneMeshKind::Static;
			SceneMeshAssetIndex MeshAssetIndex = kInvalidSceneMeshAssetIndex;
			SceneMeshInstanceGroupIndex InstanceGroupIndex = kInvalidSceneMeshInstanceGroupIndex;
			bool bVisible = true;
			bool bIncluded = false;
		};

		void Prepare(std::size_t meshCount);

		std::span<MeshSlot> GetMeshSlots() noexcept { return m_meshSlots; }

		void CommitMeshes(std::span<const SceneMeshInstanceGroupData> groups);

		std::span<const MeshSlot> GetExtractedMeshes() const noexcept { return m_extractedMeshes; }

		std::span<const SceneMeshInstanceGroupData> GetMeshGroups() const noexcept { return m_meshGroups; }

	private:
		std::vector<MeshSlot> m_meshSlots;
		std::vector<MeshSlot> m_extractedMeshes;
		std::vector<SceneMeshInstanceGroupData> m_meshGroups;
	};
}
