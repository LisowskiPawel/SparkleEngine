#include "Levels/AssetPackSyncPlanner.h"

#include "Core/Public/Diagnostics/Error.h"
#include "ProjectLevelCatalog.h"

#include <algorithm>
#include <set>
#include <utility>

namespace SparkleLauncher
{
	std::string FindAssetPackAcquisitionBlocker(const ProjectLevelCatalog& catalog, std::string_view packId)
	{
		while (!packId.empty())
		{
			const ProjectAssetPack& pack = catalog.assetPacks.find(packId)->second;
			if (!pack.bDownloadSupported && !catalog.IsAssetPackPayloadPresent(pack))
			{
				return "Asset pack '" + pack.id + "' is required but has no supported acquisition path.";
			}
			packId = pack.parentPackId;
		}
		return {};
	}

	class AssetPackSyncPlanner final
	{
	public:
		AssetPackSyncPlanner(const ProjectLevelCatalog& catalog, std::span<const std::string> requestedLevelIds, bool bRequireGameSupport) noexcept :
		    m_catalog(catalog),
		    m_requestedLevelIds(requestedLevelIds),
		    m_bRequireGameSupport(bRequireGameSupport)
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
					AppendLevelPack(*level, m_bRequireGameSupport);
				}
				return;
			}

			for (const ProjectLevelCatalogEntry& level : m_catalog.levels)
			{
				if (!level.bSelected || level.assetPackId.empty())
				{
					continue;
				}
				AppendLevelPack(level, true);
			}
		}

		void AppendLevelPack(const ProjectLevelCatalogEntry& level, bool bSelected)
		{
			if (level.assetPackId.empty())
			{
				return;
			}

			const ProjectAssetPack& pack = m_catalog.assetPacks.at(level.assetPackId);
			if (bSelected && !pack.bGameSupported)
			{
				throw Diagnostics::Error("Selected level '" + level.id + "' requires game-unsupported asset pack '" + pack.id + "': " + pack.gameBlocker);
			}
			const std::string blocker = FindAssetPackAcquisitionBlocker(m_catalog, pack.id);
			if (!blocker.empty())
			{
				throw Diagnostics::Error(blocker);
			}
			AppendPackAndParents(pack);
		}

		void AppendPackAndParents(const ProjectAssetPack& pack)
		{
			if (m_appendedIds.contains(pack.id))
			{
				return;
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
		bool m_bRequireGameSupport;
		std::set<std::string, std::less<>> m_appendedIds;
		std::vector<std::string> m_packIds;
	};

	std::vector<std::string> BuildAssetPackSyncPlan(const ProjectLevelCatalog& catalog, std::span<const std::string> requestedLevelIds, bool bRequireGameSupport)
	{
		return AssetPackSyncPlanner(catalog, requestedLevelIds, bRequireGameSupport).Build();
	}
}
