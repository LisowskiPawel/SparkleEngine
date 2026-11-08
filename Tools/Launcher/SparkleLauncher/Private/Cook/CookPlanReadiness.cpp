#include "Cook/CookPlanReadiness.h"

#include "Cook/CookOperationProcessRequests.h"
#include "Build/Toolchains/ShaderCompilerSdkDiscovery.h"
#include "SparkleLauncher/BuildProfileCatalog.h"
#include "Workspace/LauncherProductPaths.h"
#include "Core/Public/Strings/StringUtils.h"

#include <algorithm>
#include <system_error>

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

	static bool ValidateCookScopes(CookOperationPlan& plan)
	{
		if (plan.Kind == CookOperationKind::CookWorkspace && plan.Request.SelectedScopes.empty())
		{
			plan.ReadinessMessages.push_back("Select at least one output to cook.");
			return false;
		}
		if (plan.Kind == CookOperationKind::CookAllAssets)
		{
#if !SPARKLE_ENABLE_CONTENT_PIPELINE && !SPARKLE_ENABLE_SHADER_COMPILER
			plan.ReadinessMessages.push_back("No cook features are enabled in this workspace configuration.");
			return false;
#else
			return true;
#endif
		}
		bool bSupported = true;
#if !SPARKLE_ENABLE_SHADER_COMPILER
		if (IncludesCookScope(plan, CookWorkspaceScope::Shaders))
		{
			bSupported = false;
			plan.ReadinessMessages.push_back("ShaderCompiler is disabled in this workspace configuration.");
		}
#endif
#if !SPARKLE_ENABLE_CONTENT_PIPELINE
		if (IncludesCookScope(plan, CookWorkspaceScope::Textures) || IncludesCookScope(plan, CookWorkspaceScope::SceneAssets))
		{
			bSupported = false;
			plan.ReadinessMessages.push_back("Content pipeline tools are disabled in this workspace configuration.");
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
				plan.ReadinessMessages.push_back("Cook tool is missing; sync products or build cooking tools: " + toolPath.string());
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

			plan.ReadinessMessages.push_back(
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

	bool CheckCookConfirmation(CookOperationPlan& plan)
	{
		if (plan.Request.Mode != CookMode::Force || plan.Request.bForceRecookConfirmed)
		{
			return true;
		}
		plan.ReadinessMessages.push_back("Clean before cooking requires confirmation for: " + plan.CookedOutputDirectory.string());
		return false;
	}

	void PrepareCookPlanInputs(CookOperationPlan& plan)
	{
		NormalizeCookScopes(plan);
		plan.ToolProfile = ResolveCookToolProfile(plan.Request.GameProfile);
		plan.CookedOutputDirectory = ResolveCookedRoot(plan.Request.WorkspaceRoot, plan.Request.ContentId);
		ResolveRequiredCookTools(plan);
	}

	bool CheckCookInputs(CookOperationPlan& plan)
	{
		const bool bContentAvailable = !plan.Request.ContentId.empty();
		if (!bContentAvailable)
		{
			plan.ReadinessMessages.push_back("Workspace content is unavailable for cooking.");
		}
		const bool bScopesSupported = ValidateCookScopes(plan);
		const bool bToolsAvailable = CheckRequiredCookTools(plan);
		return bContentAvailable && bScopesSupported && bToolsAvailable;
	}
}
