#include "SparkleLauncher/CookOperations.h"

#include "Cook/CookOperationProcessRequests.h"
#include "Build/Toolchains/ShaderCompilerSdkDiscovery.h"
#include "Core/Public/Strings/StringUtils.h"
#include "SparkleLauncher/BuildProfileCatalog.h"
#include "Workspace/LauncherStatePaths.h"
#include "Workspace/LauncherProductPaths.h"

#include <algorithm>
#include <filesystem>
#include <optional>
#include <sstream>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace SparkleLauncher
{

	static std::vector<std::string> FindMissingCookToolSupport(const std::filesystem::path& toolPath)
	{
		std::vector<std::string> missingEntries;
		if (toolPath.stem() != "ShaderCompiler")
		{
			return missingEntries;
		}
		const std::filesystem::path toolDirectory = toolPath.parent_path();
		std::error_code errorCode;
		for (std::string_view sdkFile : GetShaderCompilerRequiredSdkFiles())
		{
			constexpr std::string_view binaryPrefix = "Bin/";
			if (!sdkFile.starts_with(binaryPrefix))
			{
				continue;
			}
			const std::string_view runtimeFileName = sdkFile.substr(binaryPrefix.size());
			errorCode.clear();
			if (std::filesystem::is_regular_file(toolDirectory / runtimeFileName, errorCode) && !errorCode)
			{
				continue;
			}

			missingEntries.emplace_back(runtimeFileName);
		}

		if (!HasShaderCompilerStandardModules(toolDirectory))
		{
			missingEntries.emplace_back("slang-standard-module-*");
		}

		return missingEntries;
	}

	static void AddReadiness(CookOperationPlan& plan, std::string message)
	{
		plan.ReadinessMessages.push_back(std::move(message));
	}

	static void AddPlannedEffect(CookOperationPlan& plan, std::string message)
	{
		plan.PlannedEffects.push_back(std::move(message));
	}

	static std::string ResolveCookToolProfile(std::string_view gameProfileName)
	{
		const std::optional<BuildProfile> gameProfile = FindBuildProfile(gameProfileName);
		if (!gameProfile.has_value())
		{
			return "DevelopmentEditor";
		}

		for (const BuildProfile& profile : GetBuildProfileCatalog())
		{
			if (profile.State == gameProfile->State && profile.Target == BuildProfileTarget::Editor)
			{
				return profile.Name;
			}
		}

		return "DevelopmentEditor";
	}

	static void ResolveRequiredCookTools(CookOperationPlan& plan)
	{
#if SPARKLE_ENABLE_SHADER_COMPILER
		if (IncludesCookScope(plan, CookWorkspaceScope::Shaders))
		{
			plan.RequiredToolPaths.push_back(ResolveToolExecutable(plan.Request.WorkspaceRoot, "ShaderCompiler", plan.ToolProfile));
		}
#endif
#if SPARKLE_ENABLE_CONTENT_PIPELINE
		if (IncludesCookScope(plan, CookWorkspaceScope::Textures) || IncludesCookScope(plan, CookWorkspaceScope::SceneAssets))
		{
			plan.RequiredToolPaths.push_back(ResolveToolExecutable(plan.Request.WorkspaceRoot, "AssetCooker", plan.ToolProfile));
		}
		if (IncludesCookScope(plan, CookWorkspaceScope::Textures))
		{
			plan.RequiredToolPaths.push_back(ResolveToolExecutable(plan.Request.WorkspaceRoot, "TextureCooker", plan.ToolProfile));
		}
#endif
	}

	static std::string JoinCookScopeNames(std::span<const CookWorkspaceScope> scopes, std::string (*describeScope)(CookWorkspaceScope))
	{
		std::string names;
		for (const CookWorkspaceScope scope : scopes)
		{
			if (!names.empty())
			{
				names += ", ";
			}
			names += describeScope(scope);
		}
		return names;
	}

	static void PopulateCookEffects(CookOperationPlan& plan)
	{
		if (plan.Request.Mode == CookMode::Force)
		{
			AddPlannedEffect(plan, "Remove cooked outputs before cooking: " + plan.CookedOutputDirectory.string());
		}

		switch (plan.Kind)
		{
			case CookOperationKind::CookWorkspace:
			{
				const std::string scopeNames = JoinCookScopeNames(plan.Request.SelectedScopes, DisplayName);
				if (!scopeNames.empty())
				{
					AddPlannedEffect(plan, "Cook selected outputs: " + scopeNames + ".");
				}
				if (IncludesCookScope(plan, CookWorkspaceScope::Shaders))
				{
					AddPlannedEffect(plan, "Shader backend: " + plan.Request.ShaderBackend + ".");
					AddPlannedEffect(plan, "Cook the global shader map and code library for the DXIL and SPIR-V targets.");
				}
				return;
			}
			case CookOperationKind::CookShaders:
				AddPlannedEffect(plan, "Shader backend: " + plan.Request.ShaderBackend + ".");
				AddPlannedEffect(plan, std::string("Shader debug info: ") + (plan.Request.bShaderEnableDebugInfo ? "enabled." : "disabled."));
				AddPlannedEffect(plan, std::string("Shader optimizations: ") + (plan.Request.bShaderEnableOptimizations ? "enabled." : "disabled."));
				AddPlannedEffect(plan, std::string("Warnings as errors: ") + (plan.Request.bShaderWarningsAsErrors ? "enabled." : "disabled."));
				AddPlannedEffect(plan, std::string("Strip debug info: ") + (plan.Request.bShaderStripDebugInfo ? "enabled." : "disabled."));
				AddPlannedEffect(plan, "Cook the global shader map and code library for the DXIL and SPIR-V targets.");
				return;
			case CookOperationKind::BuildTextures:
				AddPlannedEffect(plan, "Cook texture assets for " + plan.Request.ContentId + ".");
				return;
			case CookOperationKind::BuildSceneAssets:
				AddPlannedEffect(plan, "Cook scene, mesh, and material assets for " + plan.Request.ContentId + ".");
				return;
			case CookOperationKind::CookAllAssets:
				AddPlannedEffect(plan, "Shader phase backend: " + plan.Request.ShaderBackend + ".");
				AddPlannedEffect(plan, std::string("Shader phase debug info: ") + (plan.Request.bShaderEnableDebugInfo ? "enabled." : "disabled."));
				AddPlannedEffect(plan, std::string("Shader phase optimizations: ") + (plan.Request.bShaderEnableOptimizations ? "enabled." : "disabled."));
				AddPlannedEffect(plan, "Cook all selected level assets.");
				return;
		}
	}

	static void PopulateCookSteps(CookOperationPlan& plan)
	{
		const std::vector<CookOperationProcessStep> processSteps = BuildCookProcessStepsForPlan(plan);
		for (const CookOperationProcessStep& processStep : processSteps)
		{
			CookOperationStep step;
			step.Id = processStep.Id;
			step.DisplayName = processStep.DisplayName;
			step.bDestructive = processStep.bDeletesCookedOutputs;
			step.DestructivePath = processStep.DestructivePath;
			if (processStep.bHasProcessRequest)
			{
				step.DisplayCommandLine = BuildDisplayCommandLine(processStep.Request.ExecutablePath, processStep.Request.Arguments);
				step.LogPath = processStep.Request.LogPath;
			}
			else
			{
				step.DisplayCommandLine = "Delete " + processStep.DestructivePath.string();
			}
			plan.Steps.push_back(std::move(step));
		}
	}

	std::string ToString(CookOperationKind kind)
	{
		switch (kind)
		{
			case CookOperationKind::CookWorkspace:
				return "CookWorkspace";
			case CookOperationKind::CookShaders:
				return "CookShaders";
			case CookOperationKind::BuildTextures:
				return "BuildTextures";
			case CookOperationKind::BuildSceneAssets:
				return "BuildSceneAssets";
			case CookOperationKind::CookAllAssets:
				return "CookAllAssets";
		}

		return "Unknown";
	}

	std::string ToString(CookWorkspaceScope scope)
	{
		switch (scope)
		{
			case CookWorkspaceScope::Shaders:
				return "shaders";
			case CookWorkspaceScope::Textures:
				return "textures";
			case CookWorkspaceScope::SceneAssets:
				return "assets";
		}

		return "unknown";
	}

	std::string DisplayName(CookWorkspaceScope scope)
	{
		switch (scope)
		{
			case CookWorkspaceScope::Shaders:
				return "Shaders";
			case CookWorkspaceScope::Textures:
				return "Textures";
			case CookWorkspaceScope::SceneAssets:
				return "Scene assets";
		}

		return "Unknown";
	}

	bool TryParseCookWorkspaceScope(std::string_view value, CookWorkspaceScope& outScope)
	{
		if (value == "shaders")
		{
			outScope = CookWorkspaceScope::Shaders;
			return true;
		}
		if (value == "textures")
		{
			outScope = CookWorkspaceScope::Textures;
			return true;
		}
		if (value == "assets" || value == "scene-assets")
		{
			outScope = CookWorkspaceScope::SceneAssets;
			return true;
		}
		return false;
	}

	std::string ToString(CookMode mode)
	{
		switch (mode)
		{
			case CookMode::Incremental:
				return "incremental";
			case CookMode::Force:
				return "force";
		}

		return "unknown";
	}

	const std::vector<CookOperationDefinition>& GetCookOperationDefinitions()
	{
		static const std::vector<CookOperationDefinition> definitions = {
		    {CookOperationKind::CookWorkspace, "cook.workspace", "Cook", "Cook Workspace", "Cook the selected shader, texture, and scene outputs as one request."},
		    {CookOperationKind::CookAllAssets, "cook.all", "Cook", "Cook All", "Prepare all selected level assets."},
		    {CookOperationKind::CookShaders, "cook.shaders", "Cook", "Cook Shaders", "Validate and prepare the shader map and code library."},
		    {CookOperationKind::BuildTextures, "cook.textures", "Cook", "Cook Textures", "Prepare texture assets for Editor and Game."},
		    {CookOperationKind::BuildSceneAssets, "cook.assets", "Cook", "Cook Scenes And Meshes", "Prepare scene, mesh, and material assets for Editor and Game."},
		};

		return definitions;
	}

	std::optional<CookOperationDefinition> FindCookOperationDefinition(std::string_view operationId)
	{
		const std::vector<CookOperationDefinition>& definitions = GetCookOperationDefinitions();
		const auto found = std::find_if(definitions.begin(), definitions.end(), [operationId](const CookOperationDefinition& definition) { return definition.Id == operationId; });
		return found == definitions.end() ? std::nullopt : std::optional<CookOperationDefinition>(*found);
	}

	static bool ValidateCookScopes(CookOperationPlan& plan)
	{
		if (plan.Kind == CookOperationKind::CookWorkspace && plan.Request.SelectedScopes.empty())
		{
			AddReadiness(plan, "Select at least one output to cook.");
			return false;
		}
		if (plan.Kind == CookOperationKind::CookAllAssets)
		{
#if !SPARKLE_ENABLE_CONTENT_PIPELINE && !SPARKLE_ENABLE_SHADER_COMPILER
			AddReadiness(plan, "No cook features are enabled in this workspace configuration.");
			return false;
#else
			return true;
#endif
		}
		bool bSupported = true;
#if !SPARKLE_ENABLE_SHADER_COMPILER
		if (IncludesCookScope(plan, CookWorkspaceScope::Shaders))
		{
			supported = false;
			AddReadiness(plan, "ShaderCompiler is disabled in this workspace configuration.");
		}
#endif
#if !SPARKLE_ENABLE_CONTENT_PIPELINE
		if (IncludesCookScope(plan, CookWorkspaceScope::Textures) || IncludesCookScope(plan, CookWorkspaceScope::SceneAssets))
		{
			supported = false;
			AddReadiness(plan, "Content pipeline tools are disabled in this workspace configuration.");
		}
#endif
		return bSupported;
	}

	static bool CheckRequiredCookTools(CookOperationPlan& plan)
	{
		bool bRequiredCookToolsAvailable = true;
		for (const std::filesystem::path& toolPath : plan.RequiredToolPaths)
		{
			std::error_code errorCode;
			const bool bToolExists = std::filesystem::exists(toolPath, errorCode);
			if (!bToolExists || errorCode)
			{
				bRequiredCookToolsAvailable = false;
				AddReadiness(plan, "Cook tool is missing; sync products or build cooking tools: " + toolPath.string());
				continue;
			}

			const std::vector<std::string> missingSupportEntries = FindMissingCookToolSupport(toolPath);
			if (missingSupportEntries.empty())
			{
				continue;
			}

			bRequiredCookToolsAvailable = false;
			std::vector<std::string_view> missingSupportEntryViews;
			missingSupportEntryViews.reserve(missingSupportEntries.size());
			for (const std::string& supportEntry : missingSupportEntries)
			{
				missingSupportEntryViews.push_back(supportEntry);
			}

			AddReadiness(
			    plan,
			    "Cook tool runtime support bundle is incomplete beside " + toolPath.filename().string()
			        + "; sync the complete product bundle or rebuild cooking tools: " + Strings::Join(missingSupportEntryViews, ", "));
		}
		return bRequiredCookToolsAvailable;
	}

	static void NormalizeCookScopes(CookOperationPlan& plan)
	{
		if (plan.Kind != CookOperationKind::CookWorkspace)
		{
			return;
		}
		std::vector<CookWorkspaceScope>& scopes = plan.Request.SelectedScopes;
		for (std::size_t index = 0; index < scopes.size(); ++index)
		{
			scopes.erase(std::remove(scopes.begin() + index + 1, scopes.end(), scopes[index]), scopes.end());
		}
	}

	static void RecordCookInputs(CookOperationPlan& plan)
	{
		const CookOperationRequest& request = plan.Request;
		plan.Operation.Inputs.push_back({"content", request.ContentId});
		plan.Operation.Inputs.push_back({"gameProfile", request.GameProfile});
		plan.Operation.Inputs.push_back({"cookMode", ToString(request.Mode)});
		plan.Operation.Inputs.push_back({"toolProfile", plan.ToolProfile});
		plan.Operation.Inputs.push_back({"shaderBackend", request.ShaderBackend});
		if (plan.Kind == CookOperationKind::CookWorkspace && !plan.Request.SelectedScopes.empty())
		{
			plan.Operation.Inputs.push_back({"cookScopes", JoinCookScopeNames(plan.Request.SelectedScopes, ToString)});
		}
		plan.Operation.LogPath = ResolveLauncherOperationLogPath(request.WorkspaceRoot, plan.Operation.Id, "Latest.txt");
		if (request.Mode == CookMode::Force)
		{
			plan.Operation.DestructiveScope = OperationDestructiveScope::CookedOutputs;
			plan.Operation.bRequiresConfirmation = true;
		}
	}

	static bool CheckCookConfirmation(CookOperationPlan& plan)
	{
		if (plan.Request.Mode != CookMode::Force || plan.Request.bForceRecookConfirmed)
		{
			return true;
		}
		AddReadiness(plan, "Clean before cooking requires confirmation for: " + plan.CookedOutputDirectory.string());
		return false;
	}

	static void DescribeCookDryRun(CookOperationPlan& plan, std::string_view displayName)
	{
		std::ostringstream dryRun;
		dryRun << "Dry-run plan for " << displayName << ":";
		for (const CookOperationStep& step : plan.Steps)
		{
			dryRun << "\n  " << step.DisplayName << ": " << step.DisplayCommandLine;
			if (!step.LogPath.empty())
			{
				dryRun << "\n    Log: " << step.LogPath.string();
			}
		}
		if (plan.Steps.empty())
		{
			dryRun << "\n  No command step available until readiness issues are resolved.";
		}
		plan.Operation.DryRunText = dryRun.str();
	}

	CookOperationPlan PlanCookOperation(std::string_view operationId, const CookOperationRequest& request)
	{
		CookOperationPlan plan;
		const std::optional<CookOperationDefinition> definition = FindCookOperationDefinition(operationId);
		if (!definition.has_value())
		{
			plan.Operation = MakeOperationRecord(std::string(operationId), "Unknown cook operation");
			SetOperationFailure(plan.Operation, OperationProblemKind::Planning, "Unknown cook operation id.", "Choose a registered Cook operation, then retry.");
			AddReadiness(plan, plan.Operation.Failure->Summary);
			return plan;
		}

		plan.Kind = definition->Kind;
		plan.Request = request;
		NormalizeCookScopes(plan);
		plan.ToolProfile = ResolveCookToolProfile(request.GameProfile);
		plan.CookedOutputDirectory = ResolveCookedRoot(request.WorkspaceRoot, request.ContentId);
		plan.Operation = MakeOperationRecord(definition->Id, definition->DisplayName);
		RecordCookInputs(plan);
		ResolveRequiredCookTools(plan);

		const bool bContentAvailable = !request.ContentId.empty();
		if (!bContentAvailable)
		{
			AddReadiness(plan, "Workspace content is unavailable for cooking.");
		}
		const bool bScopesSupported = ValidateCookScopes(plan);
		const bool bToolsAvailable = CheckRequiredCookTools(plan);
		const bool bConfirmed = CheckCookConfirmation(plan);
		PopulateCookEffects(plan);

		const bool bInputsReadyscopesSupported = bContentAvailable && bScopesSupported;
		const bool bInputsReady = bInputsReadyscopesSupported && (bToolsAvailable);
		if (bInputsReady)
		{
			PopulateCookSteps(plan);
		}
		plan.bCanRun = bInputsReady && bConfirmed;
		DescribeCookDryRun(plan, definition->DisplayName);
		return plan;
	}
}
