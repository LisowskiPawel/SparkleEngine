#pragma once

#include "World/Extraction/WorldExtractionStorage.h"

#include <cstdint>
#include <span>

class SkeletonResourceStore;

namespace ECS
{
	class MeshResourceStore;
	class EntityRegistry;
	struct MeshInstance;

	class MeshExtractionSystemExecution final
	{
	public:
		MeshExtractionSystemExecution(const EntityRegistry& registry, const MeshResourceStore& meshes, const SkeletonResourceStore& skeletons, std::span<WorldExtractionStorage::MeshSlot> slots);

		std::uint32_t GetMeshCount() const noexcept;
		bool Run(std::uint32_t begin, std::uint32_t end);

	private:
		void ExtractMesh(EntityId entity, const MeshInstance& mesh, WorldExtractionStorage::MeshSlot& slot) const;

		const EntityRegistry& m_registry;
		const MeshResourceStore& m_meshes;
		const SkeletonResourceStore& m_skeletons;
		std::span<WorldExtractionStorage::MeshSlot> m_slots;
	};
}
