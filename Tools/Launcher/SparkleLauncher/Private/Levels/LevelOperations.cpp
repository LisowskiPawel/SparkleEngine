#include "SparkleLauncher/LevelOperations.h"

#include "Levels/LevelOperationProcessRequests.h"
#include "Levels/LevelPreparation.h"
#include "Cook/CookedContentReadiness.h"
#include "ProjectLevelCatalog.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Workspace/LauncherStatePaths.h"
#include "SparkleLauncher/ToolResolver.h"
#include "Workspace/LauncherProductPaths.h"

#include <algorithm>
#include <sstream>
#include <utility>

namespace SparkleLauncher
{
	static void AddReadiness(LevelOperationPlan& plan, std::string message)
	{
		plan.ReadinessMessages.push_back(std::move(message));
	}

	static void PopulateLevelOperationSteps(LevelOperationPlan& plan)
	{
		for (const LevelOperationProcessStep& processStep : BuildLevelOperationProcessSteps(plan))
		{
			LevelOperationStep step;
			step.Id = processStep.Id;
			step.DisplayName = processStep.DisplayName;
			step.DisplayCommandLine = BuildDisplayCommandLine(processStep.Request.ExecutablePath, processStep.Request.Arguments);
			step.LogPath = processStep.Request.LogPath;
			plan.Steps.push_back(std::move(step));
		}
	}

	const std::vector<LevelOperationDefinition>& GetLevelOperationDefinitions()
	{
		static const std::vector<LevelOperationDefinition> definitions = {
		    {"levels.sync", "Levels", "Sync Levels", "Select levels and acquire their asset packs without changing code, workspace, or SDK dependencies."},
		};

		return definitions;
	}

	std::optional<LevelOperationDefinition> FindLevelOperationDefinition(std::string_view operationId)
	{
		const std::vector<LevelOperationDefinition>& definitions = GetLevelOperationDefinitions();
		const auto found = std::find_if(definitions.begin(), definitions.end(), [operationId](const LevelOperationDefinition& definition) { return definition.Id == operationId; });
		return found == definitions.end() ? std::nullopt : std::optional<LevelOperationDefinition>(*found);
	}

	static void ResolveAvailableLevels(LevelOperationPlan& plan)
	{
		if (!plan.Request.bSelectAvailableLevels)
		{
			return;
		}
		const ProjectLevelCatalog catalog = ProjectLevelCatalogFile::Load(ResolveProjectRoot(plan.Request.WorkspaceRoot, plan.Request.ContentId));
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
			AddReadiness(plan, "Asset sync requires the source acquisition script. Open the matching source checkout to use this developer tool.");
			return false;
		}
		const ToolResolveResult cmake = ResolveKnownTool(KnownTool::CMake);
		if (!cmake.bFound)
		{
			AddReadiness(plan, cmake.FailureReason);
			return false;
		}
		if (plan.Request.ContentId.empty())
		{
			AddReadiness(plan, "Workspace content is unavailable for level synchronization.");
			return false;
		}
		plan.CMakePath = cmake.Path;
		AddReadiness(plan, "CMake is available for level asset acquisition.");
		return true;
	}

	static void RecordLevelSyncInputs(LevelOperationPlan& plan)
	{
		plan.Operation.Inputs.push_back({"content", plan.Request.ContentId});
		std::string levels;
		for (const std::string& levelId : plan.Request.RequestedLevelIds)
		{
			if (!levels.empty())
			{
				levels += ", ";
			}
			levels += levelId;
		}
		if (!levels.empty())
		{
			plan.Operation.Inputs.push_back({"levels", std::move(levels)});
		}
	}

	static void DescribeLevelSync(LevelOperationPlan& plan)
	{
		plan.PlannedEffects.push_back(
		    plan.Request.bSelectAvailableLevels          ? "Acquire all available supported levels and select them only after acquisition succeeds."
		        : plan.Request.RequestedLevelIds.empty() ? "Acquire asset packs referenced by selected maps; unselected packs remain untouched."
		                                                 : "Acquire asset packs referenced by the requested maps; map selection and Game support remain unchanged.");

		std::ostringstream dryRun;
		dryRun << "Dry-run plan for " << plan.Operation.DisplayName << ":";
		for (const LevelOperationStep& step : plan.Steps)
		{
			dryRun << "\n  " << step.DisplayName << ": " << step.DisplayCommandLine;
			dryRun << "\n    Log: " << step.LogPath.string();
		}
		if (plan.Steps.empty())
		{
			dryRun << (plan.bCanRun ? "\n  All requested level asset packs are already present." : "\n  No acquisition step available until readiness issues are resolved.");
		}
		plan.Operation.DryRunText = dryRun.str();
	}

	LevelOperationPlan PlanLevelOperation(std::string_view operationId, const LevelOperationRequest& request)
	{
		LevelOperationPlan plan;
		const auto definition = FindLevelOperationDefinition(operationId);
		if (!definition.has_value())
		{
			plan.Operation = MakeOperationRecord(std::string(operationId), "Unknown level operation");
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, "Unknown level operation id.", "Choose a registered level Sync operation, then retry.");
			AddReadiness(plan, plan.Operation.Failure->Summary);
			return plan;
		}
		plan.Request = request;
		plan.Operation = MakeOperationRecord(definition->Id, definition->DisplayName);
		plan.Operation.LogPath = ResolveLauncherOperationLogPath(request.WorkspaceRoot, definition->Id, "Latest.txt");
		if (!InspectLevelSyncTools(plan))
		{
			return plan;
		}
		try
		{
			ResolveAvailableLevels(plan);
			RecordLevelSyncInputs(plan);
			PopulateLevelOperationSteps(plan);
			plan.bCanRun = true;
		}
		catch (const Diagnostics::Error& error)
		{
			AddReadiness(plan, error.what());
		}
		DescribeLevelSync(plan);
		return plan;
	}
}
