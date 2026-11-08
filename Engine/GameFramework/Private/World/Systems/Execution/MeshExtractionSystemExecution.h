#pragma once

#include "World/Systems/Descriptors/GameWorldSystemContract.h"
#include "World/Extraction/WorldExtractionStorage.h"

#include <cstdint>
#include <span>

class SkeletonResourceStore;

namespace ECS
{
	class MeshResourceStore;

	class MeshExtractionSystemExecution final
	{
	public:
		MeshExtractionSystemExecution(MeshExtractionQuery& query, const MeshResourceStore& meshes, const SkeletonResourceStore& skeletons, std::span<WorldExtractionStorage::MeshSlot> slots);

		std::uint32_t GetMeshCount() const noexcept;
		bool Run(std::uint32_t begin, std::uint32_t end);

	private:
		MeshExtractionQuery& m_query;
		const MeshResourceStore& m_meshes;
		const SkeletonResourceStore& m_skeletons;
		std::span<WorldExtractionStorage::MeshSlot> m_slots;
	};
}
