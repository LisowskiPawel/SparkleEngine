#include "PCH.h"

#include "World/Extraction/WorldExtractionStorage.h"

#include <algorithm>

namespace ECS
{
	void WorldExtractionStorage::Prepare(std::size_t meshCount)
	{
		m_meshSlots.resize(meshCount);
		m_extractedMeshes.reserve(meshCount);
		for (MeshSlot& slot : m_meshSlots)
		{
			slot.bIncluded = false;
		}
	}

	void WorldExtractionStorage::CommitMeshes(std::span<const SceneMeshInstanceGroupData> groups)
	{
		std::sort(m_meshSlots.begin(), m_meshSlots.end(), [](const MeshSlot& lhs, const MeshSlot& rhs) { return lhs.Entity < rhs.Entity; });
		m_extractedMeshes.clear();
		m_meshGroups.assign(groups.begin(), groups.end());
		for (SceneMeshInstanceGroupData& group : m_meshGroups)
		{
			group.firstInstance = kInvalidSceneMeshInstanceIndex;
			group.instanceCount = 0;
		}
		for (const MeshSlot& slot : m_meshSlots)
		{
			if (!slot.bIncluded)
			{
				continue;
			}
			if (slot.InstanceGroupIndex < m_meshGroups.size())
			{
				SceneMeshInstanceGroupData& group = m_meshGroups[slot.InstanceGroupIndex];
				if (group.instanceCount == 0)
				{
					group.firstInstance = static_cast<SceneMeshInstanceIndex>(m_extractedMeshes.size());
				}
				++group.instanceCount;
			}
			m_extractedMeshes.push_back(slot);
		}
	}
}
