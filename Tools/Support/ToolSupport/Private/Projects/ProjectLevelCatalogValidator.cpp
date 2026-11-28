#include "Projects/ProjectLevelCatalogValidator.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Paths/PathUtils.h"

#include <algorithm>
#include <format>
#include <unordered_set>
#include <vector>

static bool IsSafeCatalogIdentifier(std::string_view value) noexcept
{
	return !value.empty()
	    && std::all_of(
	        value.begin(),
	        value.end(),
	        [](unsigned char character)
	        { return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || (character >= '0' && character <= '9') || character == '-' || character == '_'; });
}

static bool IsSha256(std::string_view value) noexcept
{
	return value.size() == 64 && std::all_of(value.begin(), value.end(), [](unsigned char character) { return (character >= '0' && character <= '9') || (character >= 'a' && character <= 'f'); });
}

static void ValidateLevel(const ProjectLevelCatalogEntry& level, const std::filesystem::path& projectRoot)
{
	if (!IsSafeCatalogIdentifier(level.id))
	{
		throw Diagnostics::Error(std::format("Catalog level '{}' has an unsafe identity.", level.id));
	}
	if (level.sourcePath != (projectRoot / "Levels" / (level.id + ".level")).lexically_normal())
	{
		throw Diagnostics::Error(std::format("Catalog level '{}' must use Levels/<id>.level.", level.id));
	}
	if (!level.sceneSourcePath.empty())
	{
		const auto meshRelativePath = level.sceneSourcePath.lexically_relative(projectRoot / "Assets" / "Meshes");
		if (meshRelativePath.empty() || *meshRelativePath.begin() == "..")
		{
			throw Diagnostics::Error(std::format("Catalog level '{}' SceneSource must remain under Assets/Meshes.", level.id));
		}
	}
}

static void ValidateAssetPackIdentity(const ProjectLevelCatalog& catalog, const ProjectAssetPack& pack)
{
	if (!IsSafeCatalogIdentifier(pack.id))
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' has an unsafe identity.", pack.id));
	}
	if (!pack.parentPackId.empty() && !catalog.assetPacks.contains(pack.parentPackId))
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' references unknown parent '{}'.", pack.id, pack.parentPackId));
	}
}

static void ValidateAssetPackPaths(const ProjectAssetPack& pack)
{
	if (pack.rootPath.empty())
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' has no content root.", pack.id));
	}
	const bool bRequiredEmpty = pack.requiredRelativePath.empty() || pack.requiredRelativePath == ".";
	const bool bRooted = pack.requiredRelativePath.has_root_name() || pack.requiredRelativePath.has_root_directory();
	const bool bInvalidRoot = bRequiredEmpty || bRooted;
	const bool bAbsoluteOrParent = pack.requiredRelativePath.is_absolute() || pack.requiredRelativePath.generic_string().starts_with("..");
	if (bInvalidRoot || bAbsoluteOrParent)
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' has an unsafe required path.", pack.id));
	}
	if (!pack.extractionPath.empty() && !Paths::IsUnderRoot(pack.rootPath, pack.extractionPath))
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' root must remain within its extraction root.", pack.id));
	}
}

static void ValidateArchivePayload(const ProjectAssetPack& pack)
{
	if (!pack.bDownloadSupported)
	{
		return;
	}
	const bool bNameOrSizeMissing = pack.archiveName.empty() || pack.archiveBytes == 0;
	if (bNameOrSizeMissing || pack.archiveSha256.empty())
	{
		throw Diagnostics::Error(std::format("Downloadable asset pack '{}' has incomplete acquisition metadata.", pack.id));
	}
	if (!IsSha256(pack.archiveSha256))
	{
		throw Diagnostics::Error(std::format("Downloadable asset pack '{}' has an invalid SHA-256 digest.", pack.id));
	}
	const std::filesystem::path name(pack.archiveName);
	const bool bDot = name == "." || name == "..";
	const bool bRooted = name.has_root_name() || name.has_root_directory();
	const bool bInvalidRoot = bDot || bRooted;
	if (bInvalidRoot || name.filename() != name)
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' archive name must not contain a path.", pack.id));
	}
}

static void ValidateLooseFilePayload(const ProjectAssetPack& pack)
{
	const bool bArchiveSpecified = !pack.archiveName.empty() || pack.archiveBytes != 0;
	const bool bArchiveMetadata = bArchiveSpecified || !pack.archiveSha256.empty();
	if (!pack.bDownloadSupported || bArchiveMetadata)
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' must choose either a file manifest or an archive payload.", pack.id));
	}
	const bool bDirectoryUrl = pack.sourceUrl.ends_with('/');
	if (!bDirectoryUrl || pack.requiredRelativePath != ".sparkle-acquisition.txt")
	{
		throw Diagnostics::Error(std::format("Loose-file asset pack '{}' needs a source directory URL and acquisition receipt.", pack.id));
	}
}

static void ValidateAssetPackAcquisition(const ProjectAssetPack& pack)
{
	if (pack.bDownloadSupported)
	{
		if (!pack.bExternal)
		{
			throw Diagnostics::Error(std::format("Downloadable asset pack '{}' must be declared external.", pack.id));
		}
		if (pack.sourceUrl.empty() || pack.extractionPath.empty())
		{
			throw Diagnostics::Error(std::format("Downloadable asset pack '{}' has incomplete acquisition metadata.", pack.id));
		}
		if (!pack.sourceUrl.starts_with("https://"))
		{
			throw Diagnostics::Error(std::format("Downloadable asset pack '{}' must use an HTTPS source URL.", pack.id));
		}
	}
	if (pack.sourceFilesManifestPath.empty())
	{
		ValidateArchivePayload(pack);
	}
	else
	{
		ValidateLooseFilePayload(pack);
	}
}

static void ValidateAssetPackMetadata(const ProjectLevelCatalog& catalog, const ProjectAssetPack& pack)
{
	ValidateAssetPackIdentity(catalog, pack);
	ValidateAssetPackPaths(pack);
	ValidateAssetPackAcquisition(pack);
}

static void ValidateParentChain(const ProjectLevelCatalog& catalog, const ProjectAssetPack& pack)
{
	std::unordered_set<std::string_view> ancestors;
	const ProjectAssetPack* ancestor = &pack;
	while (!ancestor->parentPackId.empty())
	{
		if (!ancestors.insert(ancestor->id).second)
		{
			throw Diagnostics::Error(std::format("Asset pack '{}' has a cyclic parent chain.", pack.id));
		}
		ancestor = &catalog.assetPacks.at(ancestor->parentPackId);
		if (pack.bGameSupported && !ancestor->bGameSupported)
		{
			throw Diagnostics::Error(std::format("Game-supported asset pack '{}' depends on game-unsupported parent '{}'.", pack.id, ancestor->id));
		}
	}
}

static void ValidateExtractionRoots(const std::vector<const ProjectAssetPack*>& downloadablePacks)
{
	for (std::size_t leftIndex = 0; leftIndex < downloadablePacks.size(); ++leftIndex)
	{
		for (std::size_t rightIndex = leftIndex + 1; rightIndex < downloadablePacks.size(); ++rightIndex)
		{
			const ProjectAssetPack& left = *downloadablePacks[leftIndex];
			const ProjectAssetPack& right = *downloadablePacks[rightIndex];
			const bool bExtractionRootsOverlap = Paths::IsUnderRoot(left.extractionPath, right.extractionPath) || Paths::IsUnderRoot(right.extractionPath, left.extractionPath);

			if (bExtractionRootsOverlap)
			{
				throw Diagnostics::Error(std::format("Downloadable asset packs '{}' and '{}' have overlapping extraction roots.", left.id, right.id));
			}
		}
	}
}

void ValidateProjectLevelCatalog(const ProjectLevelCatalog& catalog, const std::filesystem::path& projectRoot)
{
	if (catalog.levels.empty())
	{
		throw Diagnostics::Error("Catalog contains no levels.");
	}

	std::unordered_set<std::string_view> levelIds;
	for (const ProjectLevelCatalogEntry& level : catalog.levels)
	{
		ValidateLevel(level, projectRoot);
		if (!levelIds.insert(level.id).second)
		{
			throw Diagnostics::Error(std::format("Catalog level identity '{}' is duplicated.", level.id));
		}
		if (!level.assetPackId.empty() && !catalog.assetPacks.contains(level.assetPackId))
		{
			throw Diagnostics::Error(std::format("Catalog level '{}' references unknown asset pack '{}'.", level.id, level.assetPackId));
		}
	}

	std::unordered_set<std::string_view> archiveNames;
	std::vector<const ProjectAssetPack*> downloadablePacks;
	for (const auto& [packId, pack] : catalog.assetPacks)
	{
		ValidateAssetPackMetadata(catalog, pack);
		if (pack.bDownloadSupported && !pack.archiveName.empty() && !archiveNames.insert(pack.archiveName).second)
		{
			throw Diagnostics::Error(std::format("Downloadable asset pack archive name '{}' is duplicated.", pack.archiveName));
		}
		if (pack.bDownloadSupported)
		{
			downloadablePacks.push_back(&pack);
		}
	}

	for (const auto& [packId, pack] : catalog.assetPacks)
	{
		ValidateParentChain(catalog, pack);
	}

	ValidateExtractionRoots(downloadablePacks);
}
