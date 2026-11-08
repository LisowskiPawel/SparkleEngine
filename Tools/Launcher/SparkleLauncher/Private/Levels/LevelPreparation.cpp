#include "Levels/LevelPreparation.h"

#include "Levels/AssetPackSyncPlanner.h"
#include "Cook/CookedContentReadiness.h"
#include "ProjectLevelCatalog.h"

#include <filesystem>
#include <system_error>

namespace SparkleLauncher
{
	LevelPreparation InspectLevelPreparation(const ProjectLevelCatalog& catalog, const ProjectLevelCatalogEntry& level, const CookedContentReadiness& cooked, bool bHasSourceTools)
	{
		const auto pack = catalog.assetPacks.find(level.assetPackId);
		const ProjectAssetPack* assetPack = pack == catalog.assetPacks.end() ? nullptr : &pack->second;
		std::error_code error;
		const bool bLevelPresent = std::filesystem::is_regular_file(level.sourcePath, error) && !error;
		const bool bPackPresent = catalog.IsAssetPackSourceReady(level.assetPackId);
		const std::string acquisitionBlocker = FindAssetPackAcquisitionBlocker(catalog, level.assetPackId);

		LevelPreparation result;
		result.bSourceReady = bLevelPresent && bPackPresent;
		result.bGameSupported = assetPack == nullptr || assetPack->bGameSupported;
		const bool bMeshesReady = level.id == "Empty" || cooked.bMeshesReady;
		const bool bAssetsReady = bMeshesReady && cooked.bTexturesReady;
		const bool bCookedReady = bAssetsReady && cooked.Shaders == CookedShaderPublicationState::Ready;
		result.bReady = bLevelPresent && bCookedReady;
		const bool bCanUseTools = bHasSourceTools && bLevelPresent;
		result.bCanPrepare = bCanUseTools && acquisitionBlocker.empty();
		const bool bAvailable = result.bReady || result.bCanPrepare;
		result.bCanSelect = result.bGameSupported && bAvailable;
		result.bCanSync = result.bCanPrepare && !bPackPresent;
		if (!bLevelPresent)
		{
			result.UnavailableReason = "Level definition is missing: " + level.sourcePath.string();
		}
		else if (!result.bGameSupported)
		{
			result.UnavailableReason = assetPack->gameBlocker;
		}
		else if (!result.bCanSelect)
		{
			result.UnavailableReason = acquisitionBlocker.empty() ? "Source acquisition tools are unavailable in this workspace." : acquisitionBlocker;
		}
		return result;
	}
}
