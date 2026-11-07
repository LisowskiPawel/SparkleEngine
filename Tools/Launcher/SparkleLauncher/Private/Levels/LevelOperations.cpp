#include "SparkleLauncher/LevelOperations.h"

#include "Levels/LevelOperationProcessRequests.h"

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

	LevelOperationPlan PlanLevelOperation(std::string_view operationId, const LevelOperationRequest& request)
	{
		LevelOperationPlan plan;
		const std::optional<LevelOperationDefinition> definition = FindLevelOperationDefinition(operationId);
		if (!definition.has_value())
		{
			plan.Operation = MakeOperationRecord(std::string(operationId), "Unknown level operation");
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, "Unknown level operation id.", "Choose a registered level Sync operation, then retry.");
			AddReadiness(plan, plan.Operation.Failure->Summary);
			return plan;
		}

		plan.Request = request;
		plan.Operation = MakeOperationRecord(definition->Id, definition->DisplayName);
		plan.Operation.Inputs.push_back({"content", request.ContentId});
		if (!request.RequestedLevelIds.empty())
		{
			std::vector<std::string_view> levelIds;
			levelIds.reserve(request.RequestedLevelIds.size());
			for (const std::string& levelId : request.RequestedLevelIds)
			{
				levelIds.push_back(levelId);
			}
			plan.Operation.Inputs.push_back({"levels", Strings::Join(levelIds, ", ")});
		}
		plan.Operation.LogPath = ResolveLauncherOperationLogPath(request.WorkspaceRoot, definition->Id, "Latest.txt");

		std::error_code sourceError;
		const bool bAssetSyncScriptMissing = !std::filesystem::is_regular_file(request.WorkspaceRoot / "Tools/Launcher/SparkleLauncher/Scripts/SyncAssetPack.cmake", sourceError) || sourceError;

		if (bAssetSyncScriptMissing)
		{
			AddReadiness(plan, "Asset sync requires the source acquisition script. Open the matching source checkout to use this developer tool.");
			return plan;
		}
		const ToolResolveResult cmake = ResolveKnownTool(KnownTool::CMake);
		if (cmake.bFound)
		{
			plan.CMakePath = cmake.Path;
			AddReadiness(plan, "CMake is available for level asset acquisition.");
		}
		else
		{
			AddReadiness(plan, cmake.FailureReason);
		}
		if (request.ContentId.empty())
		{
			AddReadiness(plan, "Workspace content is unavailable for level synchronization.");
		}

		plan.PlannedEffects.push_back(
		    request.RequestedLevelIds.empty() ? "Acquire asset packs referenced by selected maps into gitignored content roots; unselected packs remain untouched."
		                                      : "Acquire asset packs referenced by the requested maps into gitignored content roots; map selection and Game support "
		                                        "remain unchanged.");

		if (cmake.bFound && !request.ContentId.empty())
		{
			try
			{
				PopulateLevelOperationSteps(plan);
				plan.bCanRun = true;
			}
			catch (const Diagnostics::Error& error)
			{
				AddReadiness(plan, error.what());
			}
		}

		std::ostringstream dryRun;
		dryRun << "Dry-run plan for " << definition->DisplayName << ":";
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
		return plan;
	}
}
