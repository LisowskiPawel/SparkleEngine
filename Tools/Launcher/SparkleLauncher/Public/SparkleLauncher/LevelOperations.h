#pragma once

#include "SparkleLauncher/LauncherContentDefaults.h"
#include "SparkleLauncher/OperationModel.h"
#include "SparkleLauncher/LauncherProcess.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace SparkleLauncher
{
	struct LevelOperationDefinition
	{
		std::string Id;
		std::string Group;
		std::string DisplayName;
		std::string Description;
	};

	struct LevelOperationRequest
	{
		std::filesystem::path WorkspaceRoot;
		std::string ContentId = kDefaultContentId;
		std::vector<std::string> RequestedLevelIds;
		bool bSelectAvailableLevels = false;
	};

	struct LevelOperationStep
	{
		std::string DisplayName;
		Process::ChildProcessRequest Request;
	};

	struct LevelOperationPlan
	{
		OperationRecord Operation;
		LevelOperationRequest Request;
		std::filesystem::path CMakePath;
		std::vector<LevelOperationStep> Steps;
		std::vector<std::string> PlannedEffects;
		std::vector<std::string> ReadinessMessages;
		bool bCanRun = false;
	};

	const std::vector<LevelOperationDefinition>& GetLevelOperationDefinitions();
	const LevelOperationDefinition* FindLevelOperationDefinition(std::string_view operationId);
	LevelOperationPlan PlanLevelOperation(std::string_view operationId, const LevelOperationRequest& request);
	OperationRecord RunLevelOperationPlan(LevelOperationPlan plan, const Process::ChildProcessOutputCallback& outputCallback = {}, std::stop_token cancellation = {});
}
