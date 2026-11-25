#pragma once

#include "RHI/Public/Diagnostics/ExternalCaptureProvider.h"
#include "RHI/Public/Diagnostics/RhiGpuCrash.h"

#include "SparkleLauncher/BuildProfileCatalog.h"
#include "SparkleLauncher/LauncherContentDefaults.h"
#include "SparkleLauncher/OperationModel.h"
#include "SparkleLauncher/LauncherProcess.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace SparkleLauncher
{
	enum class LevelRunMode : std::uint8_t
	{
		Editor,
		Game
	};

	struct LevelRunOperationDefinition
	{
		std::string Id;
		std::string Group;
		std::string DisplayName;
		std::string Description;
	};

	struct LevelRunOperationRequest
	{
		std::filesystem::path WorkspaceRoot;
		std::string ContentId = kDefaultContentId;
		LevelRunMode RunMode = LevelRunMode::Editor;
		std::string ProductProfile = "DevelopmentEditor";
		std::string LevelId;
		std::string GraphicsApi = "d3d12";
		ExternalCaptureProvider CaptureProvider = ExternalCaptureProvider::None;
		RhiGpuCrashMode GpuCrashMode = RhiGpuCrashMode::Off;
		std::filesystem::path ShaderExportPath;
	};

	struct LevelRunOperationStep
	{
		std::string DisplayName;
		Process::ChildProcessRequest Request;
	};

	struct LevelRunOperationPlan
	{
		OperationRecord Operation;
		LevelRunOperationRequest Request;
		std::string TargetName;
		std::string SessionId;
		LevelRunOperationStep Step;
		std::vector<std::string> PlannedEffects;
		std::vector<std::string> ReadinessMessages;
		bool bCanRun = false;
	};

	std::string_view ToString(LevelRunMode mode) noexcept;
	const std::vector<LevelRunOperationDefinition>& GetLevelRunOperationDefinitions();
	const LevelRunOperationDefinition* FindLevelRunOperationDefinition(std::string_view operationId);
	LevelRunOperationPlan PlanLauncherOperation(std::string_view operationId, const LevelRunOperationRequest& request);
	OperationRecord RunLauncherOperationPlan(LevelRunOperationPlan plan, const Process::ChildProcessOutputCallback& outputCallback = {}, std::stop_token cancellation = {});
}
