#pragma once

#include "World/ECS/Components/RenderingComponents.h"
#include "GameFramework/Public/Scene/Meshes/Mesh.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace ECS
{
	class MeshResourceStore final
	{
	public:
		MeshResourceHandle Add(std::shared_ptr<const Mesh> mesh);
		std::shared_ptr<const Mesh> ResolveImmutable(MeshResourceHandle handle) const noexcept;
		bool Remove(MeshResourceHandle handle) noexcept;

	private:
		struct Entry final
		{
			std::shared_ptr<const Mesh> Resource;
			std::uint32_t Generation = 1;
		};

		std::vector<Entry> m_entries;
		std::vector<std::uint32_t> m_freeSlots;
	};
}
