#include "SparkleLauncher/LevelRunOperations.h"

#include "Core/Public/Strings/StringUtils.h"
#include "Core/Public/Paths/ProductPaths.h"
#include "Core/Public/Projects/ProjectLevelCatalog.h"
#include "Core/Public/Diagnostics/Error.h"
#include "CookedContentReadiness.h"
#include "LevelRunOperationProcessRequests.h"
#include "ExternalCaptureDiscovery.h"
#include "GpuCrashLaunch.h"
#include "Core/Public/FileSystemUtils.h"
#include "LauncherStatePaths.h"
#include "SparkleLauncher/ToolResolver.h"

#include <algorithm>
#include <optional>
#include <sstream>
#include <system_error>
#include <utility>

namespace SparkleLauncher
{
	static bool InspectRequestedLevel(LevelRunOperationPlan& plan)
	{
		try
		{
			const auto catalog = ProjectLevelCatalogFile::Load(plan.WorkingDirectory);
			const auto level = std::find_if(catalog.levels.begin(), catalog.levels.end(), [&plan](const ProjectLevelCatalogEntry& entry) { return entry.id == plan.Request.LevelId; });
			if (level == catalog.levels.end())
			{
				plan.ReadinessMessages.push_back("Requested level is not present in the catalog: " + plan.Request.LevelId);
				return false;
			}
			if (!Filesystem::IsSourceWorkspace(plan.RepositoryRoot) && !level->releaseSupported)
			{
				plan.ReadinessMessages.push_back("Requested level is development-only; use a source checkout: " + level->id);
				return false;
			}
			if (level->id != "Empty" && (!level->selected || !catalog.IsLevelReady(*level)))
			{
				plan.ReadinessMessages.push_back("Requested level must be selected with its complete source content ready: " + level->id);
				return false;
			}
			return true;
		}
		catch (const Diagnostics::Error& error)
		{
			plan.ReadinessMessages.push_back(error.what());
			return false;
		}
	}

	static void PopulateRunStep(LevelRunOperationPlan& plan)
	{
		if (!plan.CanRun)
		{
			return;
		}
		for (const LevelRunOperationProcessStep& processStep : BuildLevelRunProcessStepsForPlan(plan))
		{
			LevelRunOperationStep step;
			step.Id = processStep.Id;
			step.DisplayName = processStep.DisplayName;
			step.DisplayCommandLine = BuildDisplayCommandLine(processStep.Request.ExecutablePath, processStep.Request.Arguments);
			step.LogPath = processStep.Request.LogPath;
			plan.Steps.push_back(std::move(step));
		}
	}

	static bool ResolveRunProduct(LevelRunOperationPlan& plan)
	{
		const BuildProfileTarget expectedTarget = plan.Request.RunMode == LevelRunMode::Editor ? BuildProfileTarget::Editor : BuildProfileTarget::Game;
		const std::optional<BuildProfile> profile = FindBuildProfile(plan.Request.ProductProfile);
		if (!profile.has_value() || profile->Target != expectedTarget)
		{
			plan.ReadinessMessages.push_back("Profile does not match the selected " + std::string(ToString(plan.Request.RunMode)) + " run mode: " + plan.Request.ProductProfile);
			return false;
		}

		plan.TargetName = BuildProjectTargetName(plan.Request.ContentId, *profile);
		if (plan.Request.GpuCrashMode == RhiGpuCrashMode::Aftermath)
		{
			plan.TargetName += "GpuCrashReceipt";
		}

		plan.ExecutablePath = Filesystem::ResolveProjectProductPath(
		    plan.RepositoryRoot,
		    plan.Request.ContentId,
		    plan.Request.RunMode == LevelRunMode::Editor ? "editor" : "game",
		    plan.Request.ProductProfile,
		    plan.Request.GpuCrashMode == RhiGpuCrashMode::Off ? "" : "gpu-crash");

		plan.WorkingDirectory = Filesystem::ResolveProductProjectRoot(plan.Request.RepositoryRoot, plan.Request.ContentId);
		return true;
	}

	static void InspectRunReadiness(LevelRunOperationPlan& plan)
	{
		std::error_code errorCode;
		plan.Readiness.ExecutableReady = std::filesystem::is_regular_file(plan.ExecutablePath, errorCode);
		errorCode.clear();
		plan.Readiness.ContentDirectoryReady = std::filesystem::exists(plan.WorkingDirectory / std::string(Filesystem::kProjectMarker), errorCode);

		const CookedContentReadiness cookedContent = InspectCookedContentReadiness(
		    plan.Request.RepositoryRoot,
		    plan.Request.ContentId,
		    plan.Request.GpuCrashMode == RhiGpuCrashMode::Off ? "" : "gpu-crash");

		// GameFramework always registers built-in Empty without imported scene assets.
		plan.Readiness.CookedMeshesReady = plan.Request.LevelId == "Empty" || cookedContent.MeshesReady;
		plan.Readiness.CookedTexturesReady = cookedContent.TexturesReady;
		plan.Readiness.CookedShadersReady = cookedContent.Shaders == CookedShaderPublicationState::Ready;

		if (!plan.Readiness.ExecutableReady)
		{
			plan.ReadinessMessages.push_back("Selected product executable is missing; compile " + plan.TargetName + " first.");
		}
		if (!plan.Readiness.ContentDirectoryReady)
		{
			plan.ReadinessMessages.push_back("Content working directory is missing or invalid: " + Strings::ToNarrow(plan.WorkingDirectory.wstring()));
		}
		if (!plan.Readiness.CookedMeshesReady)
		{
			plan.ReadinessMessages.push_back("Cooked scenes and meshes are missing.");
		}
		if (!plan.Readiness.CookedTexturesReady)
		{
			plan.ReadinessMessages.push_back("Cooked textures are missing.");
		}
		switch (cookedContent.Shaders)
		{
			case CookedShaderPublicationState::Ready:
				break;
			case CookedShaderPublicationState::Invalid:
				plan.ReadinessMessages.push_back("Cooked shader publication is incomplete or internally inconsistent.");
				break;
			case CookedShaderPublicationState::Missing:
				plan.ReadinessMessages.push_back("The complete cooked shader generation is missing.");
				break;
		}
	}

	static std::string BuildRunDryRunText(const LevelRunOperationPlan& plan, std::string_view displayName)
	{
		std::ostringstream dryRun;
		dryRun << "Dry-run plan for " << displayName << ":";
		for (const LevelRunOperationStep& step : plan.Steps)
		{
			dryRun << "\n  " << step.DisplayName << ": " << step.DisplayCommandLine;
			dryRun << "\n    Working directory: " << Strings::ToNarrow(plan.WorkingDirectory.wstring());
			dryRun << "\n    Env: SPARKLE_STARTUP_LEVEL=" << plan.Request.LevelId;
			if (!step.LogPath.empty())
			{
				dryRun << "\n    Log: " << Strings::ToNarrow(step.LogPath.wstring());
			}
		}
		if (plan.Steps.empty())
		{
			dryRun << "\n  No command step is available until readiness issues are resolved.";
		}
		return dryRun.str();
	}

	std::string_view ToString(LevelRunMode mode) noexcept
	{
		switch (mode)
		{
			case LevelRunMode::Editor:
				return "editor";
			case LevelRunMode::Game:
				return "game";
		}

		return "unknown";
	}

	const std::vector<LevelRunOperationDefinition>& GetLevelRunOperationDefinitions()
	{
		static const std::vector<LevelRunOperationDefinition> definitions = {
		    {"levels.run", "Levels", "Run Level", "Open a catalog level in the selected product after its prerequisites are ready."},
		};

		return definitions;
	}

	std::optional<LevelRunOperationDefinition> FindLevelRunOperationDefinition(std::string_view operationId)
	{
		const std::vector<LevelRunOperationDefinition>& definitions = GetLevelRunOperationDefinitions();
		const auto found = std::find_if(definitions.begin(), definitions.end(), [operationId](const LevelRunOperationDefinition& definition) { return definition.Id == operationId; });
		return found == definitions.end() ? std::nullopt : std::optional<LevelRunOperationDefinition>(*found);
	}

	LevelRunOperationPlan PlanLevelRunOperation(std::string_view operationId, const LevelRunOperationRequest& request)
	{
		LevelRunOperationPlan plan;
		const std::optional<LevelRunOperationDefinition> definition = FindLevelRunOperationDefinition(operationId);
		if (!definition.has_value())
		{
			plan.Operation = MakeOperationRecord(std::string(operationId), "Unknown level run operation");
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, "Unknown level run operation id.", "Choose the registered Run Level operation, then retry.");
			plan.ReadinessMessages.push_back(plan.Operation.Failure->Summary);
			return plan;
		}

		plan.RepositoryRoot = request.RepositoryRoot;
		plan.Request = request;
		plan.Operation = MakeOperationRecord(definition->Id, definition->DisplayName);

		plan.Operation.Inputs = {
		    {"content", plan.Request.ContentId},
		    {"runMode", std::string(ToString(plan.Request.RunMode))},
		    {"profile", plan.Request.ProductProfile},
		    {"level", plan.Request.LevelId},
		    {"graphicsApi", plan.Request.GraphicsApi},
		    {"captureProvider", std::string(ExternalCaptureProviderToString(plan.Request.CaptureProvider))}};

		plan.Operation.LogPath = ResolveLauncherOperationLogPath(plan.Request.RepositoryRoot, definition->Id, "Latest.txt");
		if (plan.Request.LevelId.empty())
		{
			plan.ReadinessMessages.push_back("A catalog level id is required.");
			return plan;
		}
		if (plan.Request.GraphicsApi != "d3d12" && plan.Request.GraphicsApi != "vulkan")
		{
			plan.ReadinessMessages.push_back("Unknown graphics API: " + plan.Request.GraphicsApi);
			return plan;
		}

		if (!PrepareGpuCrashLaunch(plan))
		{
			return plan;
		}

		const auto capture = InspectExternalCaptureProvider(plan.Request.CaptureProvider, plan.Request.GraphicsApi, plan.Request.ProductProfile);
		if (!capture.Available())
		{
			plan.ReadinessMessages.push_back(capture.Detail);
			return plan;
		}

		if (!ResolveRunProduct(plan))
		{
			return plan;
		}
		if (!InspectRequestedLevel(plan))
		{
			return plan;
		}

		plan.Environment.push_back({"SPARKLE_STARTUP_LEVEL", plan.Request.LevelId});

		InspectRunReadiness(plan);

		plan.PlannedEffects.push_back(
		    "Run level " + plan.Request.LevelId + " in " + Strings::ToNarrow(plan.ExecutablePath.wstring()) + " from " + Strings::ToNarrow(plan.WorkingDirectory.wstring()) + ".");

		plan.PlannedEffects.push_back("Use graphics API: " + plan.Request.GraphicsApi + ".");

		plan.CanRun = plan.Readiness.ExecutableReady && plan.Readiness.ContentDirectoryReady && plan.Readiness.CookedMeshesReady && plan.Readiness.CookedTexturesReady
		    && plan.Readiness.CookedShadersReady;

		PopulateRunStep(plan);

		plan.Operation.DryRunText = BuildRunDryRunText(plan, definition->DisplayName);
		return plan;
	}
}
