#include "SparkleLauncher/MaintenanceOperations.h"

#include "ProjectLevelCatalog.h"

#include <set>
#include <system_error>

namespace SparkleLauncher
{
	static void RetainPackAndParents(const ProjectLevelCatalog& catalog, std::string_view packId, std::set<std::string, std::less<>>& retained)
	{
		while (!packId.empty() && retained.insert(std::string(packId)).second)
		{
			const auto pack = catalog.assetPacks.find(packId);
			if (pack == catalog.assetPacks.end())
			{
				return;
			}
			packId = pack->second.parentPackId;
		}
	}

	static void AppendExtractedPack(const ProjectAssetPack& pack, std::vector<MaintenanceCleanPathSpec>& targets)
	{
		std::error_code error;
		if (pack.bExternal && !pack.extractionPath.empty() && std::filesystem::exists(pack.extractionPath, error) && !error)
		{
			targets.push_back({pack.displayName + " level content", pack.extractionPath, "Extracted level content. The cached source archive is preserved for fast re-sync."});
		}
	}

	static void AppendPackAndParents(
	    const ProjectLevelCatalog& catalog,
	    std::string_view packId,
	    const std::set<std::string, std::less<>>& retained,
	    std::set<std::string, std::less<>>& appended,
	    std::vector<MaintenanceCleanPathSpec>& targets)
	{
		while (!packId.empty() && !retained.contains(packId) && appended.insert(std::string(packId)).second)
		{
			const auto pack = catalog.assetPacks.find(packId);
			if (pack == catalog.assetPacks.end())
			{
				return;
			}
			AppendExtractedPack(pack->second, targets);
			packId = pack->second.parentPackId;
		}
	}

	std::vector<MaintenanceCleanPathSpec> BuildLevelCleanPaths(const std::filesystem::path& contentRoot, std::string_view levelId)
	{
		const ProjectLevelCatalog catalog = ProjectLevelCatalogFile::Load(contentRoot);
		std::set<std::string, std::less<>> retained;
		if (!levelId.empty())
		{
			for (const ProjectLevelCatalogEntry& level : catalog.levels)
			{
				if (level.bSelected && level.id != levelId)
				{
					RetainPackAndParents(catalog, level.assetPackId, retained);
				}
			}
		}
		std::vector<MaintenanceCleanPathSpec> targets;
		std::set<std::string, std::less<>> appended;
		for (const ProjectLevelCatalogEntry& level : catalog.levels)
		{
			if (levelId.empty() ? level.bSelected : level.id == levelId)
			{
				AppendPackAndParents(catalog, level.assetPackId, retained, appended, targets);
			}
		}
		return targets;
	}
}
