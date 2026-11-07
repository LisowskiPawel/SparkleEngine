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
}

static void ValidateAssetPackMetadata(const ProjectLevelCatalog& catalog, std::string_view packId, const ProjectAssetPack& pack)
{
	if (packId.empty() || pack.id != packId)
	{
		throw Diagnostics::Error("Catalog contains an invalid asset pack identity.");
	}
	if (!IsSafeCatalogIdentifier(pack.id))
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' has an unsafe identity.", pack.id));
	}
	if (pack.displayName.empty())
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' has no display name.", pack.id));
	}
	if (!pack.parentPackId.empty() && !catalog.assetPacks.contains(pack.parentPackId))
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' references unknown parent '{}'.", pack.id, pack.parentPackId));
	}
	if (pack.parentPackId == pack.id)
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' cannot be its own parent.", pack.id));
	}
	if (pack.rootPath.empty())
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' has no content root.", pack.id));
	}
	const bool bInvalidPackPathRelativePath = pack.requiredRelativePath.empty() || pack.requiredRelativePath == ".";
	const bool bPackNameInvalid = bInvalidPackPathRelativePath || (pack.requiredRelativePath.has_root_name());
	const bool bPackRootInvalid = bPackNameInvalid || (pack.requiredRelativePath.has_root_directory());
	const bool bAbsolutePathInvalid = bPackRootInvalid || (pack.requiredRelativePath.is_absolute());
	const bool bInvalidRequiredPackPath = bAbsolutePathInvalid || (pack.requiredRelativePath.generic_string().starts_with(".."));

	if (bInvalidRequiredPackPath)
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' has an unsafe required path.", pack.id));
	}
	if (pack.bDownloadSupported && !pack.bExternal)
	{
		throw Diagnostics::Error(std::format("Downloadable asset pack '{}' must be declared external.", pack.id));
	}
	const bool bFilesPayload = !pack.sourceFilesManifestPath.empty();
	const bool bDescriptionIncompleteSupported = pack.bDownloadSupported;
	const bool bDescriptionIncompleteSourceUrl = bDescriptionIncompleteSupported && (pack.sourceUrl.empty());
	const bool bDescriptionIncompleteGuardPath = bDescriptionIncompleteSupported && !bDescriptionIncompleteSourceUrl;
	const bool bDescriptionIncompletePath = bDescriptionIncompleteGuardPath && (pack.extractionPath.empty());
	const bool bDownloadLocationMissing = bDescriptionIncompleteSourceUrl || bDescriptionIncompletePath;
	const bool bCheckArchive = bDescriptionIncompleteSupported && !bDownloadLocationMissing;
	const bool bDescriptionIncompleteFilesPayload = bCheckArchive && (!bFilesPayload);
	const bool bDescriptionIncompleteArchiveName = bDescriptionIncompleteFilesPayload && (pack.archiveName.empty());
	const bool bDescriptionIncompleteGuardBytes = bDescriptionIncompleteFilesPayload && !bDescriptionIncompleteArchiveName;
	const bool bDescriptionIncompleteArchiveBytes = bDescriptionIncompleteGuardBytes && (pack.archiveBytes == 0);
	const bool bArchiveMetadataMissing = bDescriptionIncompleteArchiveName || bDescriptionIncompleteArchiveBytes;
	const bool bCheckArchiveDigest = bDescriptionIncompleteFilesPayload && !bArchiveMetadataMissing;
	const bool bArchiveDigestMissing = bCheckArchiveDigest && (pack.archiveSha256.empty());
	const bool bArchiveIncomplete = bArchiveMetadataMissing || bArchiveDigestMissing;
	const bool bDownloadArchiveIncomplete = bDescriptionIncompleteFilesPayload && bArchiveIncomplete;
	const bool bDownloadIncomplete = bDownloadLocationMissing || bDownloadArchiveIncomplete;
	const bool bDownloadDescriptionIncomplete = bDescriptionIncompleteSupported && bDownloadIncomplete;

	if (bDownloadDescriptionIncomplete)
	{
		throw Diagnostics::Error(std::format("Downloadable asset pack '{}' has incomplete acquisition metadata.", pack.id));
	}
	const bool bDescriptionInvalidSupported = bFilesPayload && (!pack.bDownloadSupported);
	const bool bDescriptionInvalidGuardName = bFilesPayload && !bDescriptionInvalidSupported;
	const bool bDescriptionInvalidArchiveName = bDescriptionInvalidGuardName && (!pack.archiveName.empty());
	const bool bArchiveNameUnexpected = bDescriptionInvalidSupported || bDescriptionInvalidArchiveName;
	const bool bDescriptionInvalidGuardBytes = bFilesPayload && !bArchiveNameUnexpected;
	const bool bDescriptionInvalidArchiveBytes = bDescriptionInvalidGuardBytes && (pack.archiveBytes != 0);
	const bool bArchiveBytesUnexpected = bArchiveNameUnexpected || bDescriptionInvalidArchiveBytes;
	const bool bCheckUnexpectedDigest = bFilesPayload && !bArchiveBytesUnexpected;
	const bool bArchiveDigestUnexpected = bCheckUnexpectedDigest && (!pack.archiveSha256.empty());
	const bool bArchiveMetadataUnexpected = bArchiveBytesUnexpected || bArchiveDigestUnexpected;
	const bool bInvalidFilesPayload = bFilesPayload && bArchiveMetadataUnexpected;

	if (bInvalidFilesPayload)
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' must choose either a file manifest or an archive payload.", pack.id));
	}
	if (bFilesPayload && (!pack.sourceUrl.ends_with('/') || pack.requiredRelativePath != ".sparkle-acquisition.txt"))
	{
		throw Diagnostics::Error(std::format("Loose-file asset pack '{}' needs a source directory URL and acquisition receipt.", pack.id));
	}
	if (pack.bDownloadSupported && !pack.sourceUrl.starts_with("https://"))
	{
		throw Diagnostics::Error(std::format("Downloadable asset pack '{}' must use an HTTPS source URL.", pack.id));
	}
	if (pack.bDownloadSupported && !bFilesPayload && !IsSha256(pack.archiveSha256))
	{
		throw Diagnostics::Error(std::format("Downloadable asset pack '{}' has an invalid SHA-256 digest.", pack.id));
	}
	if (!pack.bGameSupported && pack.gameBlocker.empty())
	{
		throw Diagnostics::Error(std::format("Game-unsupported asset pack '{}' must declare a game blocker.", pack.id));
	}
	if (pack.bGameSupported && !pack.gameBlocker.empty())
	{
		throw Diagnostics::Error(std::format("Game-supported asset pack '{}' declares a contradictory blocker.", pack.id));
	}
	if (pack.bExternal && !pack.bDownloadSupported && pack.downloadBlocker.empty())
	{
		throw Diagnostics::Error(std::format("External asset pack '{}' without acquisition support must declare its blocker.", pack.id));
	}
	if (pack.bDownloadSupported && !pack.downloadBlocker.empty())
	{
		throw Diagnostics::Error(std::format("Downloadable asset pack '{}' declares a contradictory blocker.", pack.id));
	}
	if (pack.bExternal && (pack.sourcePageUrl.empty() || pack.version.empty() || pack.license.empty()))
	{
		throw Diagnostics::Error(std::format("External asset pack '{}' has incomplete provenance metadata.", pack.id));
	}
	if (pack.bExternal && !pack.sourcePageUrl.starts_with("https://"))
	{
		throw Diagnostics::Error(std::format("External asset pack '{}' must use an HTTPS source page URL.", pack.id));
	}
	if (!pack.extractionPath.empty() && !Paths::IsUnderRoot(pack.rootPath, pack.extractionPath))
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' root must remain within its extraction root.", pack.id));
	}
	const std::filesystem::path archiveNamePath(pack.archiveName);
	const bool bInvalidArchiveNameFilesPayload = pack.bDownloadSupported && !bFilesPayload;
	const bool bInvalidArchiveNameNamePath = bInvalidArchiveNameFilesPayload && (archiveNamePath == ".");
	const bool bInvalidArchiveNameGuardNamePath = bInvalidArchiveNameFilesPayload && !bInvalidArchiveNameNamePath;
	const bool bParentDirectoryName = bInvalidArchiveNameGuardNamePath && (archiveNamePath == "..");
	const bool bArchiveNameInvalid = bInvalidArchiveNameNamePath || bParentDirectoryName;
	const bool bInvalidArchiveNameGuardrootname = bInvalidArchiveNameFilesPayload && !bArchiveNameInvalid;
	const bool bInvalidArchiveNameRootname = bInvalidArchiveNameGuardrootname && (archiveNamePath.has_root_name());
	const bool bNameOrRootInvalid = bArchiveNameInvalid || bInvalidArchiveNameRootname;
	const bool bArchiveNameGuardrootdirectory = bInvalidArchiveNameFilesPayload && !bNameOrRootInvalid;
	const bool bInvalidArchiveNameRootdirectory = bArchiveNameGuardrootdirectory && (archiveNamePath.has_root_directory());
	const bool bArchiveRootInvalid = bNameOrRootInvalid || bInvalidArchiveNameRootdirectory;
	const bool bInvalidArchiveNameGuardfilename = bInvalidArchiveNameFilesPayload && !bArchiveRootInvalid;
	const bool bInvalidArchiveNameFilename = bInvalidArchiveNameGuardfilename && (archiveNamePath.filename() != archiveNamePath);
	const bool bNestedNameInvalid = bArchiveRootInvalid || bInvalidArchiveNameFilename;
	const bool bInvalidArchiveName = bInvalidArchiveNameFilesPayload && bNestedNameInvalid;

	if (bInvalidArchiveName)
	{
		throw Diagnostics::Error(std::format("Asset pack '{}' archive name must not contain a path.", pack.id));
	}
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
		ValidateAssetPackMetadata(catalog, packId, pack);
		if (pack.bDownloadSupported && !pack.archiveName.empty() && !archiveNames.insert(pack.archiveName).second)
		{
			throw Diagnostics::Error(std::format("Downloadable asset pack archive name '{}' is duplicated.", pack.archiveName));
		}
		if (pack.bDownloadSupported)
		{
			downloadablePacks.push_back(&pack);
		}
		ValidateParentChain(catalog, pack);
	}

	ValidateExtractionRoots(downloadablePacks);
}
