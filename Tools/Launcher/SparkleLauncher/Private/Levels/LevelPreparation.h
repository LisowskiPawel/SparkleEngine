#pragma once

#include <string>

struct ProjectLevelCatalog;
struct ProjectLevelCatalogEntry;

namespace SparkleLauncher
{
	struct CookedContentReadiness;

	// A transient projection of catalog and file readiness for one planning/UI read.
	struct LevelPreparation final
	{
		bool bSourceReady = false;
		bool bGameSupported = false;
		bool bReady = false;
		bool bCanPrepare = false;
		bool bCanSelect = false;
		bool bCanSync = false;
		std::string UnavailableReason;
	};

	LevelPreparation InspectLevelPreparation(const ProjectLevelCatalog& catalog, const ProjectLevelCatalogEntry& level, const CookedContentReadiness& cooked, bool bHasSourceTools);
}
