#pragma once

#include "SparkleLauncher/LauncherContentDefaults.h"
#include "SparkleLauncher/OperationModel.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace SparkleLauncher
{
	enum class CleanScope : std::uint8_t
	{
		CookedOutputs,
		BuildTree,
		ArtifactOutputs,
		WorkspaceState,
		ThirdPartyDependencyCache,
		Logs,
		PristineGeneratedWorkspace
	};

	struct MaintenanceOperationDefinition
	{
		std::string Id;
		std::string Group;
		std::string DisplayName;
		std::string Description;
	};

	struct MaintenanceCleanTarget
	{
		std::string DisplayName;
		std::filesystem::path Path;
		std::string Detail;
		std::uintmax_t FileCount = 0;
		std::uintmax_t DirectoryCount = 0;
		std::uintmax_t ByteCount = 0;
		bool bExists = false;
	};

	struct MaintenanceCleanPathSpec
	{
		std::string DisplayName;
		std::filesystem::path Path;
		std::string Detail;
	};

	std::vector<MaintenanceCleanPathSpec> BuildLevelCleanPaths(const std::filesystem::path& contentRoot, std::string_view levelId);

	struct ActionCleanTargetRequest
	{
		std::filesystem::path WorkspaceRoot;
		std::filesystem::path RunningLauncherPath;
		std::string OperationId;
		std::string ContentId;
		std::string EditorProfile;
		std::string GameProfile;
		std::vector<std::string> BuildScopes;
		std::vector<std::string> CookScopes;
	};

	bool SupportsActionSpecificClean(std::string_view operationId);
	std::vector<MaintenanceCleanPathSpec> BuildActionCleanPaths(const ActionCleanTargetRequest& request);

	struct MaintenanceOperationRequest
	{
		std::filesystem::path WorkspaceRoot;
		std::string ContentId = kDefaultContentId;
		std::string EditorProfile = "DevelopmentEditor";
		CleanScope RequestedCleanScope = CleanScope::CookedOutputs;
		std::vector<CleanScope> RequestedCleanScopes;
		std::vector<MaintenanceCleanPathSpec> RequestedCleanTargets;
		std::vector<std::filesystem::path> PreservedPaths;
		bool bCleanLevelContent = false;
		bool bDestructiveActionConfirmed = false;
	};

	struct MaintenanceOperationStep
	{
		std::string Id;
		std::string DisplayName;
		std::string DisplayCommandLine;
		std::filesystem::path LogPath;
		std::filesystem::path DestructivePath;
		bool bDestructive = false;
	};

	struct MaintenanceOperationPlan
	{
		OperationRecord Operation;
		MaintenanceOperationRequest Request;
		std::vector<MaintenanceCleanTarget> CleanTargets;
		std::vector<MaintenanceOperationStep> Steps;
		std::vector<std::string> PlannedEffects;
		std::vector<std::string> ReadinessMessages;
		bool bCanRun = false;
	};

	std::string ToString(CleanScope scope);
	std::string_view CleanScopeId(CleanScope scope) noexcept;
	bool CleanScopeRequiresContent(CleanScope scope) noexcept;
	bool TryParseCleanScope(std::string_view text, CleanScope& outScope) noexcept;
	const std::vector<MaintenanceOperationDefinition>& GetMaintenanceOperationDefinitions();
	std::optional<MaintenanceOperationDefinition> FindMaintenanceOperationDefinition(std::string_view operationId);
	MaintenanceOperationPlan PlanMaintenanceOperation(std::string_view operationId, const MaintenanceOperationRequest& request);
	OperationRecord RunMaintenanceOperationPlan(MaintenanceOperationPlan plan);
}
