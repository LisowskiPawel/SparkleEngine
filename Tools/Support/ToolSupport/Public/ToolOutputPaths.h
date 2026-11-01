#pragma once

#include <filesystem>
#include <string_view>

namespace SparkleTools
{
	struct ToolTargetOutputPaths final
	{
		std::filesystem::path BinaryDirectory;
		std::filesystem::path LibraryDirectory;
		std::filesystem::path SymbolDirectory;
	};

	struct ToolOutputPaths final
	{
		std::filesystem::path BuildRoot;
		std::filesystem::path DependencyCacheRoot;
		std::filesystem::path ArtifactRoot;
		std::filesystem::path ProductRoot;
		std::filesystem::path ProjectArtifactRoot;
		std::filesystem::path DiagnosticsRoot;
		std::filesystem::path SymbolsRoot;

		ToolTargetOutputPaths RuntimeSupportTargetOutputs(std::string_view targetName, std::string_view profileName) const;
		ToolTargetOutputPaths LauncherTargetOutputs(std::string_view profileName) const;
		ToolTargetOutputPaths ToolTargetOutputs(std::string_view toolName, std::string_view profileName) const;
		std::filesystem::path ToolScratchDirectory(std::string_view toolName, std::string_view profileName) const;
		std::filesystem::path LauncherBuildDirectory() const;
		std::filesystem::path SourceDependencySyncDirectory(std::string_view dependencyId) const;
		ToolTargetOutputPaths ProjectTargetOutputs(std::string_view projectName, std::string_view productRole, std::string_view profileName) const;
		std::filesystem::path CookedProjectDirectory(std::string_view projectName) const;
		std::filesystem::path SharedCookedProjectDirectory() const;
	};

	ToolOutputPaths ResolveToolOutputPaths(const std::filesystem::path& workspaceRoot);
	ToolOutputPaths ResolveToolOutputPaths(const std::filesystem::path& workspaceRoot, std::string_view variant);
	std::filesystem::path ShaderDependencyManifestPath(const std::filesystem::path& cookedShaderRoot);
}
