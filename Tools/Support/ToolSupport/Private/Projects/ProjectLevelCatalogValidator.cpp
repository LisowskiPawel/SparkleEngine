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
	const bool bInvalidPackPathRelativePath2 = bInvalidPackPathRelativePath || (pack.requiredRelativePath.has_root_name());
	const bool bInvalidPackPathRelativePath3 = bInvalidPackPathRelativePath2 || (pack.requiredRelativePath.has_root_directory());
	const bool bInvalidPackPathRelativePath4 = bInvalidPackPathRelativePath3 || (pack.requiredRelativePath.is_absolute());
	const bool bInvalidRequiredPackPath = bInvalidPackPathRelativePath4 || (pack.requiredRelativePath.generic_string().starts_with(".."));
	
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
	const bool bDescriptionIncompletesourceUrl = bDescriptionIncompleteSupported && (pack.sourceUrl.empty());
	const bool bDescriptionIncompleteGuardPath = bDescriptionIncompleteSupported && !bDescriptionIncompletesourceUrl;
	const bool bDescriptionIncompletePath = bDescriptionIncompleteGuardPath && (pack.extractionPath.empty());
	const bool bDescriptionIncompletePath2 = bDescriptionIncompletesourceUrl || bDescriptionIncompletePath;
	const bool bDescriptionIncompleteGuardSha256 = bDescriptionIncompleteSupported && !bDescriptionIncompletePath2;
	const bool bDescriptionIncompletefilesPayload = bDescriptionIncompleteGuardSha256 && (!bFilesPayload);
	const bool bDescriptionIncompletearchiveName = bDescriptionIncompletefilesPayload && (pack.archiveName.empty());
	const bool bDescriptionIncompleteGuardBytes = bDescriptionIncompletefilesPayload && !bDescriptionIncompletearchiveName;
	const bool bDescriptionIncompletearchiveBytes = bDescriptionIncompleteGuardBytes && (pack.archiveBytes == 0);
	const bool bDescriptionIncompletearchiveBytes2 = bDescriptionIncompletearchiveName || bDescriptionIncompletearchiveBytes;
	const bool bDescriptionIncompleteGuardSha2562 = bDescriptionIncompletefilesPayload && !bDescriptionIncompletearchiveBytes2;
	const bool bDescriptionIncompleteSha256 = bDescriptionIncompleteGuardSha2562 && (pack.archiveSha256.empty());
	const bool bDescriptionIncompleteSha2562 = bDescriptionIncompletearchiveBytes2 || bDescriptionIncompleteSha256;
	const bool bDescriptionIncompleteSha2563 = bDescriptionIncompletefilesPayload && bDescriptionIncompleteSha2562;
	const bool bDescriptionIncompleteSha2564 = bDescriptionIncompletePath2 || bDescriptionIncompleteSha2563;
	const bool bDownloadDescriptionIncomplete = bDescriptionIncompleteSupported && bDescriptionIncompleteSha2564;
	
	if (bDownloadDescriptionIncomplete)
	{
		throw Diagnostics::Error(std::format("Downloadable asset pack '{}' has incomplete acquisition metadata.", pack.id));
	}
	const bool bDescriptionInvalidSupported = bFilesPayload && (!pack.bDownloadSupported);
	const bool bDescriptionInvalidGuardName = bFilesPayload && !bDescriptionInvalidSupported;
	const bool bDescriptionInvalidarchiveName = bDescriptionInvalidGuardName && (!pack.archiveName.empty());
	const bool bDescriptionInvalidarchiveName2 = bDescriptionInvalidSupported || bDescriptionInvalidarchiveName;
	const bool bDescriptionInvalidGuardBytes = bFilesPayload && !bDescriptionInvalidarchiveName2;
	const bool bDescriptionInvalidarchiveBytes = bDescriptionInvalidGuardBytes && (pack.archiveBytes != 0);
	const bool bDescriptionInvalidarchiveBytes2 = bDescriptionInvalidarchiveName2 || bDescriptionInvalidarchiveBytes;
	const bool bDescriptionInvalidGuardSha256 = bFilesPayload && !bDescriptionInvalidarchiveBytes2;
	const bool bFilesDescriptionInvalidSha256 = bDescriptionInvalidGuardSha256 && (!pack.archiveSha256.empty());
	const bool bFilesDescriptionInvalidSha2562 = bDescriptionInvalidarchiveBytes2 || bFilesDescriptionInvalidSha256;
	const bool bInvalidFilesPayload = bFilesPayload && bFilesDescriptionInvalidSha2562;
	
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
	const bool bInvalidArchiveNamefilesPayload = pack.bDownloadSupported && !bFilesPayload;
	const bool bInvalidArchiveNameNamePath = bInvalidArchiveNamefilesPayload && (archiveNamePath == ".");
	const bool bInvalidArchiveNameGuardNamePath = bInvalidArchiveNamefilesPayload && !bInvalidArchiveNameNamePath;
	const bool bInvalidArchiveNameNamePath2 = bInvalidArchiveNameGuardNamePath && (archiveNamePath == "..");
	const bool bInvalidArchiveNameNamePath3 = bInvalidArchiveNameNamePath || bInvalidArchiveNameNamePath2;
	const bool bInvalidArchiveNameGuardrootname = bInvalidArchiveNamefilesPayload && !bInvalidArchiveNameNamePath3;
	const bool bInvalidArchiveNamerootname = bInvalidArchiveNameGuardrootname && (archiveNamePath.has_root_name());
	const bool bInvalidArchiveNamerootname2 = bInvalidArchiveNameNamePath3 || bInvalidArchiveNamerootname;
	const bool bArchiveNameGuardrootdirectory = bInvalidArchiveNamefilesPayload && !bInvalidArchiveNamerootname2;
	const bool bInvalidArchiveNamerootdirectory = bArchiveNameGuardrootdirectory && (archiveNamePath.has_root_directory());
	const bool bInvalidArchiveNamerootdirectory2 = bInvalidArchiveNamerootname2 || bInvalidArchiveNamerootdirectory;
	const bool bInvalidArchiveNameGuardfilename = bInvalidArchiveNamefilesPayload && !bInvalidArchiveNamerootdirectory2;
	const bool bInvalidArchiveNamefilename = bInvalidArchiveNameGuardfilename && (archiveNamePath.filename() != archiveNamePath);
	const bool bInvalidArchiveNamefilename2 = bInvalidArchiveNamerootdirectory2 || bInvalidArchiveNamefilename;
	const bool bInvalidArchiveName = bInvalidArchiveNamefilesPayload && bInvalidArchiveNamefilename2;
	
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
