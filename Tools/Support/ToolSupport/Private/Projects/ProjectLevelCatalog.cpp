#include "ProjectLevelCatalog.h"

#include <cstdint>
#include <system_error>
#include <unordered_set>

enum class AssetPackRequirement : std::uint8_t
{
	Payload,
	GameSource
};

bool ProjectLevelCatalog::IsAssetPackPayloadPresent(const ProjectAssetPack& pack) const
{
	if (pack.rootPath.empty())
	{
		return false;
	}

	const std::filesystem::path requiredPath = pack.requiredRelativePath.empty() ? pack.rootPath : pack.rootPath / pack.requiredRelativePath;
	std::error_code error;
	return std::filesystem::exists(requiredPath, error) && !error;
}

static bool IsAssetPackChainReady(const ProjectLevelCatalog& catalog, std::string_view packId, AssetPackRequirement requirement)
{
	std::unordered_set<std::string_view> visitedPackIds;
	while (!packId.empty())
	{
		if (!visitedPackIds.insert(packId).second)
		{
			return false;
		}

		const auto pack = catalog.assetPacks.find(packId);
		if (pack == catalog.assetPacks.end() || !catalog.IsAssetPackPayloadPresent(pack->second))
		{
			return false;
		}
		if (requirement == AssetPackRequirement::GameSource && !pack->second.bGameSupported)
		{
			return false;
		}

		packId = pack->second.parentPackId;
	}

	return true;
}

bool ProjectLevelCatalog::IsAssetPackSourceReady(std::string_view packId) const
{
	return IsAssetPackChainReady(*this, packId, AssetPackRequirement::Payload);
}

bool ProjectLevelCatalog::HasLevelSourceInputs(const ProjectLevelCatalogEntry& level) const
{
	if (level.id.empty() || level.sourcePath.empty())
	{
		return false;
	}

	std::error_code error;
	if (!std::filesystem::is_regular_file(level.sourcePath, error))
	{
		return false;
	}

	return IsAssetPackChainReady(*this, level.assetPackId, AssetPackRequirement::GameSource);
}
