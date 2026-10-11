#include "AssetPackSyncPlanner.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Projects/ProjectLevelCatalog.h"

#include <algorithm>
#include <set>
#include <utility>

namespace SparkleLauncher
{
	class AssetPackSyncPlanner final
	{
	public:
		AssetPackSyncPlanner(const ProjectLevelCatalog& catalog, std::span<const std::string> requestedLevelIds, bool releaseOnly) noexcept :
		    m_catalog(catalog),
		    m_requestedLevelIds(requestedLevelIds),
		    m_releaseOnly(releaseOnly)
		{
		}

		std::vector<std::string> Build()
		{
			AppendSelectedLevelPacks();
			return std::move(m_packIds);
		}

	private:
		void AppendSelectedLevelPacks()
		{
			if (!m_requestedLevelIds.empty())
			{
				for (const std::string& levelId : m_requestedLevelIds)
				{
					const auto level = std::find_if(m_catalog.levels.begin(), m_catalog.levels.end(), [&levelId](const ProjectLevelCatalogEntry& candidate) { return candidate.id == levelId; });
					if (level == m_catalog.levels.end())
					{
						throw Diagnostics::Error("Requested level '" + levelId + "' is not present in the level catalog.");
					}
					AppendLevelPack(*level, false);
				}
				return;
			}

			for (const ProjectLevelCatalogEntry& level : m_catalog.levels)
			{
				if (!level.selected || level.assetPackId.empty())
				{
					continue;
				}
				AppendLevelPack(level, true);
			}
		}

		void AppendLevelPack(const ProjectLevelCatalogEntry& level, bool selected)
		{
			if (m_releaseOnly && !level.releaseSupported)
			{
				throw Diagnostics::Error("Level '" + level.id + "' is development-only; use a source checkout for this map.");
			}
			if (level.assetPackId.empty())
			{
				return;
			}

			const ProjectAssetPack& pack = m_catalog.assetPacks.at(level.assetPackId);
			if (selected && !pack.gameSupported)
			{
				throw Diagnostics::Error("Selected level '" + level.id + "' requires game-unsupported asset pack '" + pack.id + "': " + pack.gameBlocker);
			}
			AppendPackAndParents(pack);
		}

		void AppendPackAndParents(const ProjectAssetPack& pack)
		{
			if (m_appendedIds.contains(pack.id))
			{
				return;
			}
			if (!pack.downloadSupported && !m_catalog.IsAssetPackPayloadPresent(pack))
			{
				throw Diagnostics::Error("Asset pack '" + pack.id + "' is required but has no supported acquisition path.");
			}
			if (!pack.parentPackId.empty())
			{
				AppendPackAndParents(m_catalog.assetPacks.at(pack.parentPackId));
			}

			m_appendedIds.insert(pack.id);
			m_packIds.push_back(pack.id);
		}

		const ProjectLevelCatalog& m_catalog;
		std::span<const std::string> m_requestedLevelIds;
		bool m_releaseOnly;
		std::set<std::string, std::less<>> m_appendedIds;
		std::vector<std::string> m_packIds;
	};

	std::vector<std::string> BuildAssetPackSyncPlan(const ProjectLevelCatalog& catalog, std::span<const std::string> requestedLevelIds, bool releaseOnly)
	{
		return AssetPackSyncPlanner(catalog, requestedLevelIds, releaseOnly).Build();
	}
}
