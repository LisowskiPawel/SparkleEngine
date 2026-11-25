#include "SparkleLauncher/LevelRunOperations.h"

#include "Core/Public/Strings/StringUtils.h"
#include "Workspace/LauncherProductPaths.h"
#include "ProjectLevelCatalog.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Cook/CookedContentReadiness.h"
#include "Levels/LevelRunOperationProcessRequests.h"
#include "ExternalCapture/ExternalCaptureDiscovery.h"
#include "GpuCrash/GpuCrashLaunch.h"
#include "Core/Public/Paths/FileSystemPaths.h"

#include <algorithm>
#include <optional>
#include <sstream>
#include <system_error>

namespace SparkleLauncher
{
	static bool InspectRequestedLevel(LevelRunOperationPlan& plan)
	{
		try
		{
			const auto catalog = LoadProjectLevelCatalog(plan.Step.Request.WorkingDirectory);
			const auto level = std::find_if(catalog.levels.begin(), catalog.levels.end(), [&plan](const ProjectLevelCatalogEntry& entry) { return entry.id == plan.Request.LevelId; });
			if (level == catalog.levels.end())
			{
				plan.ReadinessMessages.push_back("Requested level is not present in the catalog: " + plan.Request.LevelId);
				return false;
			}
			std::error_code error;
			if (!std::filesystem::is_regular_file(level->sourcePath, error) || error)
			{
				plan.ReadinessMessages.push_back("Requested level definition is missing: " + level->id);
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

		plan.Step.Request.ExecutablePath = ResolveProjectExecutable(
		    plan.Request.WorkspaceRoot,
		    plan.Request.ContentId,
		    plan.Request.RunMode == LevelRunMode::Editor ? "editor" : "game",
		    plan.Request.ProductProfile,
		    plan.Request.GpuCrashMode == RhiGpuCrashMode::Off ? "" : "gpu-crash");

		plan.Step.Request.WorkingDirectory = ResolveProjectRoot(plan.Request.WorkspaceRoot, plan.Request.ContentId);
		return true;
	}

	static bool InspectRunReadiness(LevelRunOperationPlan& plan)
	{
		std::error_code errorCode;
		const bool bExecutableReady = std::filesystem::is_regular_file(plan.Step.Request.ExecutablePath, errorCode);
		errorCode.clear();
		const bool bContentDirectoryReady = std::filesystem::exists(plan.Step.Request.WorkingDirectory / std::string(Filesystem::kProjectMarker), errorCode);

		const CookedContentReadiness cookedContent = InspectCookedContentReadiness(
		    plan.Request.WorkspaceRoot,
		    plan.Request.ContentId,
		    plan.Request.GpuCrashMode == RhiGpuCrashMode::Off ? "" : "gpu-crash");

		// GameFramework always registers built-in Empty without imported scene assets.
		const bool bCookedMeshesReady = plan.Request.LevelId == "Empty" || cookedContent.bMeshesReady;
		const bool bCookedTexturesReady = cookedContent.bTexturesReady;
		const bool bCookedShadersReady = cookedContent.Shaders == CookedShaderPublicationState::Ready;

		if (!bExecutableReady)
		{
			plan.ReadinessMessages.push_back("Selected product executable is missing; compile " + plan.TargetName + " first.");
		}
		if (!bContentDirectoryReady)
		{
			plan.ReadinessMessages.push_back("Content working directory is missing or invalid: " + Strings::ToNarrow(plan.Step.Request.WorkingDirectory.wstring()));
		}
		if (!bCookedMeshesReady)
		{
			plan.ReadinessMessages.push_back("Cooked scenes and meshes are missing.");
		}
		if (!bCookedTexturesReady)
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

		const bool bProductReady = bExecutableReady && bContentDirectoryReady;
		const bool bSceneAssetsReady = bCookedMeshesReady && bCookedTexturesReady;
		const bool bCookedContentReady = bSceneAssetsReady && bCookedShadersReady;
		return bProductReady && bCookedContentReady;
	}

	static std::string BuildRunDryRunText(const LevelRunOperationPlan& plan, std::string_view displayName)
	{
		std::ostringstream dryRun;
		dryRun << "Dry-run plan for " << displayName << ":";
		if (plan.bCanRun)
		{
			const LevelRunOperationStep& step = plan.Step;
			dryRun << "\n  " << step.DisplayName << ": " << BuildDisplayCommandLine(step.Request.ExecutablePath, step.Request.Arguments);
			dryRun << "\n    Working directory: " << Strings::ToNarrow(plan.Step.Request.WorkingDirectory.wstring());
			dryRun << "\n    Env: SPARKLE_STARTUP_LEVEL=" << plan.Request.LevelId;
			if (!step.Request.LogPath.empty())
			{
				dryRun << "\n    Log: " << Strings::ToNarrow(step.Request.LogPath.wstring());
			}
		}
		else
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

	const LevelRunOperationDefinition* FindLevelRunOperationDefinition(std::string_view operationId)
	{
		const std::vector<LevelRunOperationDefinition>& definitions = GetLevelRunOperationDefinitions();
		const auto found = std::find_if(definitions.begin(), definitions.end(), [operationId](const LevelRunOperationDefinition& definition) { return definition.Id == operationId; });
		return found == definitions.end() ? nullptr : &*found;
	}

	LevelRunOperationPlan PlanLauncherOperation(std::string_view operationId, const LevelRunOperationRequest& request)
	{
		LevelRunOperationPlan plan;
		const LevelRunOperationDefinition* definition = FindLevelRunOperationDefinition(operationId);
		if (definition == nullptr)
		{
			plan.Operation = MakeOperationRecord(std::string(operationId), "Unknown level run operation");
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, "Unknown level run operation id.", "Choose the registered Run Level operation, then retry.");
			plan.ReadinessMessages.push_back(plan.Operation.Failure->Summary);
			return plan;
		}

		plan.Request = request;
		plan.Operation = MakeOperationRecord(definition->Id, definition->DisplayName);

		plan.Operation.Inputs = {
		    {"content", plan.Request.ContentId},
		    {"runMode", std::string(ToString(plan.Request.RunMode))},
		    {"profile", plan.Request.ProductProfile},
		    {"level", plan.Request.LevelId},
		    {"graphicsApi", plan.Request.GraphicsApi},
		    {"captureProvider", std::string(ExternalCaptureProviderToString(plan.Request.CaptureProvider))}};

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

		plan.Step.Request.Environment.push_back({"SPARKLE_STARTUP_LEVEL", plan.Request.LevelId});

		plan.bCanRun = InspectRunReadiness(plan);

		plan.PlannedEffects.push_back(
		    "Run level " + plan.Request.LevelId + " in " + Strings::ToNarrow(plan.Step.Request.ExecutablePath.wstring()) + " from " + Strings::ToNarrow(plan.Step.Request.WorkingDirectory.wstring())
		    + ".");

		plan.PlannedEffects.push_back("Use graphics API: " + plan.Request.GraphicsApi + ".");

		if (plan.bCanRun)
		{
			ConfigureLevelRunStep(plan);
		}

		plan.Operation.DryRunText = BuildRunDryRunText(plan, definition->DisplayName);
		return plan;
	}
}
