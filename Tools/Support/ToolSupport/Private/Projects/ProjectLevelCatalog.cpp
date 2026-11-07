#include "ProjectLevelCatalog.h"

#include "Projects/ProjectLevelSelectionWriter.h"
#include "Projects/ProjectLevelCatalogReader.h"

#include <system_error>
#include <unordered_set>

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

bool ProjectLevelCatalog::IsAssetPackSourceReady(std::string_view packId) const
{
	std::unordered_set<std::string_view> visitedPackIds;
	while (!packId.empty())
	{
		if (!visitedPackIds.insert(packId).second)
		{
			return false;
		}

		const auto pack = assetPacks.find(packId);
		if (pack == assetPacks.end() || !IsAssetPackPayloadPresent(pack->second))
		{
			return false;
		}
		packId = pack->second.parentPackId;
	}

	return true;
}

bool ProjectLevelCatalog::HasLevelSourceInputs(const ProjectLevelCatalogEntry& level) const
{
	std::error_code error;
	const bool bSourceSpecified = !level.id.empty() && !level.sourcePath.empty();
	const bool bSourcePresentSourcePath = bSourceSpecified && std::filesystem::is_regular_file(level.sourcePath, error);
	const bool bSourcePresent = bSourcePresentSourcePath && (!error);
	const bool bSourceReady = bSourcePresent && IsAssetPackSourceReady(level.assetPackId);

	if (!bSourceReady)
	{
		return false;
	}

	std::string_view packId = level.assetPackId;
	while (!packId.empty())
	{
		const auto pack = assetPacks.find(packId);
		if (pack == assetPacks.end() || !pack->second.bGameSupported)
		{
			return false;
		}
		packId = pack->second.parentPackId;
	}
	return true;
}

ProjectLevelCatalog ProjectLevelCatalogFile::Load(const std::filesystem::path& projectRoot)
{
	return ProjectLevelCatalogReader::Read(projectRoot);
}

bool ProjectLevelCatalogFile::SetLevelSelected(const std::filesystem::path& projectRoot, std::string_view levelId, bool bSelected, std::string& outErrorMessage)
{
	return ProjectLevelSelectionWriter::SetLevelSelected(projectRoot, levelId, bSelected, outErrorMessage);
}

bool ProjectLevelCatalogFile::SetLevelsSelected(const std::filesystem::path& projectRoot, const std::vector<std::string>& levelIds, bool bSelected, std::string& outErrorMessage)
{
	return ProjectLevelSelectionWriter::SetLevelsSelected(projectRoot, levelIds, bSelected, outErrorMessage);
}
