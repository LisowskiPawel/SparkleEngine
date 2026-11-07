#pragma once

#include "SparkleLauncher/BuildWorkspaceOperations.h"
#include "SparkleLauncher/LauncherContentDefaults.h"
#include "SparkleLauncher/OperationModel.h"
#include "SparkleLauncher/ProcessRunner.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace SparkleLauncher
{
	enum class MaintenanceOperationKind : std::uint8_t
	{
		CleanWorkspace
	};

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
		MaintenanceOperationKind Kind = MaintenanceOperationKind::CleanWorkspace;
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
		MaintenanceOperationKind Kind = MaintenanceOperationKind::CleanWorkspace;
		BuildToolchainStatus Toolchain;
		BuildFilesFreshnessStatus Freshness;
		std::vector<MaintenanceCleanTarget> CleanTargets;
		std::vector<MaintenanceOperationStep> Steps;
		std::vector<std::string> PlannedEffects;
		std::vector<std::string> ReadinessMessages;
		bool bCanRun = false;
	};

	std::string ToString(MaintenanceOperationKind kind);
	std::string ToString(CleanScope scope);
	std::string_view CleanScopeId(CleanScope scope) noexcept;
	bool CleanScopeRequiresContent(CleanScope scope) noexcept;
	bool TryParseCleanScope(std::string_view text, CleanScope& outScope) noexcept;
	const std::vector<MaintenanceOperationDefinition>& GetMaintenanceOperationDefinitions();
	std::optional<MaintenanceOperationDefinition> FindMaintenanceOperationDefinition(std::string_view operationId);
	MaintenanceOperationPlan PlanMaintenanceOperation(std::string_view operationId, const MaintenanceOperationRequest& request);
	OperationRecord RunMaintenanceOperationPlan(MaintenanceOperationPlan plan, IProcessRunner& processRunner);
}
