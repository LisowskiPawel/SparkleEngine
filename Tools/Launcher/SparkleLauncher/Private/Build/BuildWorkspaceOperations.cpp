#include "SparkleLauncher/BuildWorkspaceOperations.h"

#include "Build/Toolchains/HostGraphicsCapabilities.h"
#include "SparkleLauncher/BuildProfileCatalog.h"

#include <algorithm>

namespace SparkleLauncher
{
	WorkspaceFeatureSettings GetLauncherWorkspaceFeatureSettings()
	{
		WorkspaceFeatureSettings settings;
		const HostGraphicsCapabilities& hostGraphics = GetHostGraphicsCapabilities();
#if SPARKLE_ENABLE_CONTENT_PIPELINE
		settings.bContentPipelineEnabled = true;
#endif
#if SPARKLE_ENABLE_SHADER_COMPILER
		settings.bShaderCompilerEnabled = true;
#endif
#if SPARKLE_ENABLE_NVIDIA_STREAMLINE
		settings.bNvidiaStreamlineEnabled = hostGraphics.bHasNvidiaAdapter;
#endif
		return settings;
	}

	std::vector<std::string> ResolveWorkspaceBuildTargets(const BuildWorkspaceOperationPlan& plan, BuildWorkspaceScope scope)
	{
		switch (scope)
		{
			case BuildWorkspaceScope::CookTools:
			{
				std::vector<std::string> targets;
#if SPARKLE_ENABLE_CONTENT_PIPELINE
				targets.push_back("AssetCooker");
				targets.push_back("TextureCooker");
#endif
#if SPARKLE_ENABLE_SHADER_COMPILER
				targets.push_back("ShaderCompiler");
#endif
				return targets;
			}
			case BuildWorkspaceScope::Editor:
			case BuildWorkspaceScope::Game:
			{
				const std::string& profileName = scope == BuildWorkspaceScope::Editor ? plan.Request.EditorProfile : plan.Request.GameProfile;
				const std::optional<BuildProfile> profile = FindBuildProfile(profileName);
				if (!profile.has_value())
				{
					return {};
				}
				return {BuildProjectTargetName(plan.Request.ContentId, *profile)};
			}
		}
		return {};
	}

	bool HasSelectedBuildScope(const BuildWorkspaceOperationRequest& request, BuildWorkspaceScope scope)
	{
		return std::find(request.SelectedScopes.begin(), request.SelectedScopes.end(), scope) != request.SelectedScopes.end();
	}

	bool TryParseBuildWorkspaceScope(std::string_view text, BuildWorkspaceScope& outScope)
	{
		if (text == "editor")
		{
			outScope = BuildWorkspaceScope::Editor;
			return true;
		}
		if (text == "game")
		{
			outScope = BuildWorkspaceScope::Game;
			return true;
		}
		if (text == "cook-tools")
		{
			outScope = BuildWorkspaceScope::CookTools;
			return true;
		}
		return false;
	}

	std::string DisplayName(WorkspaceIde ide)
	{
		switch (ide)
		{
			case WorkspaceIde::VisualStudio:
				return "Visual Studio";
			case WorkspaceIde::Rider:
				return "Rider";
		}

		return "Unknown";
	}

	bool TryParseWorkspaceIde(std::string_view text, WorkspaceIde& outIde)
	{
		if (text == "visual-studio")
		{
			outIde = WorkspaceIde::VisualStudio;
			return true;
		}
		if (text == "rider")
		{
			outIde = WorkspaceIde::Rider;
			return true;
		}
		return false;
	}

	std::string DisplayName(WorkspaceCompiler compiler)
	{
		switch (compiler)
		{
			case WorkspaceCompiler::Msvc:
				return "MSVC";
			case WorkspaceCompiler::ClangCl:
				return "clang-cl";
		}

		return "Unknown";
	}

	std::string WorkspaceCompilerCommandLineValue(WorkspaceCompiler compiler)
	{
		switch (compiler)
		{
			case WorkspaceCompiler::Msvc:
				return "msvc";
			case WorkspaceCompiler::ClangCl:
				return "clang-cl";
		}

		return "unknown";
	}

	bool TryParseWorkspaceCompiler(std::string_view text, WorkspaceCompiler& outCompiler)
	{
		if (text == "msvc")
		{
			outCompiler = WorkspaceCompiler::Msvc;
			return true;
		}
		if (text == "clang-cl")
		{
			outCompiler = WorkspaceCompiler::ClangCl;
			return true;
		}
		return false;
	}

	bool HasIncompleteEnabledSourceDependencies(const BuildWorkspaceOperationPlan& plan)
	{
		return plan.SourceDependencies.ReadyDependencyCount < plan.SourceDependencies.EnabledDependencyCount;
	}

	bool BuildWorkspaceOperationRequiresConfigureStep(const BuildWorkspaceOperationPlan& plan)
	{
		switch (plan.Kind)
		{
			case BuildWorkspaceOperationKind::SyncCode:
			case BuildWorkspaceOperationKind::GenerateBuildFiles:
				return true;
			case BuildWorkspaceOperationKind::BuildWorkspace:
				return plan.Request.bForceConfigure || !plan.Freshness.bCurrent;
			default:
				return false;
		}
	}
}
