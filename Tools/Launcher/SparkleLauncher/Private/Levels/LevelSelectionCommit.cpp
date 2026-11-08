#include "Levels/LevelSelectionCommit.h"
#include "Levels/AssetPackSyncPlanner.h"

#include "ProjectLevelCatalog.h"
#include "SparkleLauncher/OperationModel.h"

#include <exception>
#include <utility>

namespace SparkleLauncher
{
	static bool FailLevelSelection(OperationRecord& operation, std::string error, OperationProblemKind problem = OperationProblemKind::Filesystem)
	{
		SetOperationFailure(
		    operation,
		    problem,
		    "Level selection could not be committed: " + error,
		    problem == OperationProblemKind::Planning ? "Refresh the level workflow, review its preview, then retry." : "Verify that Levels.catalog is writable, then retry the operation.");

		MarkOperationFinished(operation, OperationStatus::Failed, std::nullopt);
		return false;
	}

	static bool AdmitLevelSelection(const std::filesystem::path& contentRoot, const std::vector<std::string>& levelIds, OperationRecord& operation)
	{
		try
		{
			const ProjectLevelCatalog catalog = ProjectLevelCatalogFile::Load(contentRoot);
			(void) BuildAssetPackSyncPlan(catalog, levelIds, true);
			return true;
		}
		catch (const std::exception& error)
		{
			return FailLevelSelection(operation, error.what(), OperationProblemKind::Planning);
		}
	}

	bool CommitLevelSelection(const std::filesystem::path& contentRoot, const std::vector<std::string>& levelIds, bool bSelected, OperationRecord& operation)
	{
		if (levelIds.empty())
		{
			return true;
		}
		if (bSelected && !AdmitLevelSelection(contentRoot, levelIds, operation))
		{
			return false;
		}
		try
		{
			std::string error;
			if (!ProjectLevelCatalogFile::SetLevelsSelected(contentRoot, levelIds, bSelected, error))
			{
				return FailLevelSelection(operation, std::move(error));
			}
			return true;
		}
		catch (const std::exception& error)
		{
			return FailLevelSelection(operation, error.what());
		}
	}

	bool ClearLevelSelection(const std::filesystem::path& contentRoot, OperationRecord& operation)
	{
		try
		{
			const ProjectLevelCatalog catalog = ProjectLevelCatalogFile::Load(contentRoot);
			std::vector<std::string> levelIds;
			for (const ProjectLevelCatalogEntry& level : catalog.levels)
			{
				if (level.id != "Empty" && level.bSelected)
				{
					levelIds.push_back(level.id);
				}
			}
			return CommitLevelSelection(contentRoot, levelIds, false, operation);
		}
		catch (const std::exception& error)
		{
			return FailLevelSelection(operation, error.what());
		}
	}
}
