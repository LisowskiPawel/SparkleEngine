#include "SparkleLauncher/LevelOperations.h"

#include "Levels/LevelOperationProcessRequests.h"
#include "Levels/LevelPreparation.h"
#include "Cook/CookedContentReadiness.h"
#include "ProjectLevelCatalog.h"

#include "Core/Public/Diagnostics/Error.h"
#include "SparkleLauncher/ToolResolver.h"
#include "Workspace/LauncherProductPaths.h"

#include <algorithm>

namespace SparkleLauncher
{
	const std::vector<LevelOperationDefinition>& GetLevelOperationDefinitions()
	{
		static const std::vector<LevelOperationDefinition> definitions = {
		    {"levels.sync", "Sync Levels", "Select levels and acquire their asset packs without changing code, workspace, or SDK dependencies."},
		};

		return definitions;
	}

	const LevelOperationDefinition* FindLevelOperationDefinition(std::string_view operationId)
	{
		const std::vector<LevelOperationDefinition>& definitions = GetLevelOperationDefinitions();
		const auto found = std::find_if(definitions.begin(), definitions.end(), [operationId](const LevelOperationDefinition& definition) { return definition.Id == operationId; });
		return found == definitions.end() ? nullptr : &*found;
	}

	static void ResolveAvailableLevels(LevelOperationPlan& plan)
	{
		if (!plan.Request.bSelectAvailableLevels)
		{
			return;
		}

		const ProjectLevelCatalog catalog = LoadProjectLevelCatalog(ResolveProjectRoot(plan.Request.WorkspaceRoot, plan.Request.ContentId));
		plan.Request.RequestedLevelIds.clear();
		for (const ProjectLevelCatalogEntry& level : catalog.levels)
		{
			const LevelPreparation preparation = InspectLevelPreparation(catalog, level, CookedContentReadiness{}, true);
			if (level.id != "Empty" && preparation.bGameSupported && preparation.bCanPrepare)
			{
				plan.Request.RequestedLevelIds.push_back(level.id);
			}
		}
		if (plan.Request.RequestedLevelIds.empty())
		{
			throw Diagnostics::Error("No levels have a supported synchronization path.");
		}
	}

	static bool InspectLevelSyncTools(LevelOperationPlan& plan)
	{
		std::error_code error;
		const bool bScriptPresent = std::filesystem::is_regular_file(plan.Request.WorkspaceRoot / "Tools/Launcher/SparkleLauncher/Scripts/SyncAssetPack.cmake", error);
		if (!bScriptPresent || error)
		{
			plan.ReadinessMessages.push_back("Asset sync requires the source acquisition script. Open the matching source checkout to use this developer tool.");
			return false;
		}
		const ToolResolveResult cmake = ResolveKnownTool(KnownTool::CMake);
		if (!cmake.bFound)
		{
			plan.ReadinessMessages.push_back(cmake.FailureReason);
			return false;
		}
		if (plan.Request.ContentId.empty())
		{
			plan.ReadinessMessages.push_back("Workspace content is unavailable for level synchronization.");
			return false;
		}
		plan.CMakePath = cmake.Path;
		plan.ReadinessMessages.push_back("CMake is available for level asset acquisition.");
		return true;
	}

	LevelOperationPlan PlanLauncherOperation(std::string_view operationId, const LevelOperationRequest& request)
	{
		LevelOperationPlan plan;
		const auto definition = FindLevelOperationDefinition(operationId);
		if (definition == nullptr)
		{
			plan.Operation = MakeOperationRecord(std::string(operationId), "Unknown level operation");
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, "Unknown level operation id.", "Choose a registered level Sync operation, then retry.");
			plan.ReadinessMessages.push_back(plan.Operation.Failure->Summary);
			return plan;
		}
		plan.Request = request;
		plan.Operation = MakeOperationRecord(definition->Id, definition->DisplayName);
		if (!InspectLevelSyncTools(plan))
		{
			return plan;
		}
		try
		{
			ResolveAvailableLevels(plan);
			plan.Steps = BuildLevelOperationSteps(plan);
			plan.bCanRun = true;
		}
		catch (const Diagnostics::Error& error)
		{
			plan.ReadinessMessages.push_back(error.what());
		}
		return plan;
	}
}
