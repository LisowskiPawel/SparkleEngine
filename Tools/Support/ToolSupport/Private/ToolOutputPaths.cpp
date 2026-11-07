#include "ToolOutputPaths.h"

#include "Core/Public/Paths/PathUtils.h"

#include "Core/Public/Diagnostics/Error.h"

#include <string>
#include <algorithm>
#include <cctype>

namespace SparkleTools
{
	std::filesystem::path ShaderDependencyManifestPath(const std::filesystem::path& cookedShaderRoot)
	{
		return Paths::Normalize(cookedShaderRoot / "ShaderDependencies.sdep");
	}

	ToolOutputPaths ResolveToolOutputPaths(const std::filesystem::path& workspaceRoot)
	{
		return ResolveToolOutputPaths(workspaceRoot, SPARKLE_ARTIFACT_VARIANT_NAME);
	}

	ToolOutputPaths ResolveToolOutputPaths(const std::filesystem::path& workspaceRoot, std::string_view variant)
	{
		const bool bReservedVariantName = variant == "." || variant == "..";

		const bool bInvalidVariantCharacter = !bReservedVariantName
		    && std::ranges::any_of(variant, [](unsigned char character) { return !std::isalnum(character) && character != '-' && character != '_' && character != '.'; });

		if (bReservedVariantName || bInvalidVariantCharacter)
		{
			throw Diagnostics::Error("Invalid artifact variant.");
		}
		const std::filesystem::path normalizedRoot = Paths::Normalize(workspaceRoot);
		const std::filesystem::path canonicalBuildRoot = normalizedRoot / "build";
		const auto buildRoot = variant.empty() ? canonicalBuildRoot : canonicalBuildRoot / "variants" / variant;
		const auto artifactRoot = variant.empty() ? normalizedRoot / "artifacts" : normalizedRoot / "artifacts" / variant;
		const std::filesystem::path productRoot = artifactRoot / "dev";

		return ToolOutputPaths{
		    .BuildRoot = buildRoot,
		    .DependencyCacheRoot = canonicalBuildRoot / "_deps",
		    .ArtifactRoot = artifactRoot,
		    .ProductRoot = productRoot,
		    .ProjectArtifactRoot = productRoot / "Projects",
		    .DiagnosticsRoot = artifactRoot / "diagnostics",
		    .SymbolsRoot = artifactRoot / "symbols"};
	}

	ToolTargetOutputPaths ToolOutputPaths::RuntimeSupportTargetOutputs(std::string_view targetName, std::string_view profileName) const
	{
		const std::string target(targetName);
		const std::string profile(profileName);

		return ToolTargetOutputPaths{
		    .BinaryDirectory = ProductRoot / "Engine" / "Binaries" / target / profile,
		    .LibraryDirectory = ProductRoot / "libraries" / "runtime-support" / target / profile,
		    .SymbolDirectory = SymbolsRoot / "runtime-support" / target / profile};
	}

	ToolTargetOutputPaths ToolOutputPaths::LauncherTargetOutputs(std::string_view profileName) const
	{
		const std::string profile(profileName);

		return ToolTargetOutputPaths{
		    .BinaryDirectory = ProductRoot / "Tools" / "SparkleLauncher" / "Binaries" / profile,
		    .LibraryDirectory = ProductRoot / "libraries" / "launcher" / profile,
		    .SymbolDirectory = SymbolsRoot / "launcher" / profile};
	}

	ToolTargetOutputPaths ToolOutputPaths::ToolTargetOutputs(std::string_view toolName, std::string_view profileName) const
	{
		const std::string tool(toolName);
		const std::string profile(profileName);

		return ToolTargetOutputPaths{
		    .BinaryDirectory = ProductRoot / "Tools" / tool / "Binaries" / profile,
		    .LibraryDirectory = ProductRoot / "libraries" / "tools" / tool / profile,
		    .SymbolDirectory = SymbolsRoot / "tools" / tool / profile};
	}

	std::filesystem::path ToolOutputPaths::ToolScratchDirectory(std::string_view toolName, std::string_view profileName) const
	{
		return BuildRoot / "private" / "tools" / std::string(toolName) / std::string(profileName);
	}

	std::filesystem::path ToolOutputPaths::SourceDependencySyncDirectory(std::string_view dependencyId) const
	{
		return BuildRoot / "_dependency-sync" / std::string(dependencyId);
	}

	std::filesystem::path ToolOutputPaths::LauncherBuildDirectory() const
	{
		return BuildRoot / "private" / "tools" / "SparkleLauncher";
	}

	ToolTargetOutputPaths ToolOutputPaths::ProjectTargetOutputs(std::string_view projectName, std::string_view productRole, std::string_view profileName) const
	{
		const std::string project(projectName);
		const std::string role(productRole);
		const std::string profile(profileName);

		return ToolTargetOutputPaths{
		    .BinaryDirectory = ProjectArtifactRoot / project / "Binaries" / profile,
		    .LibraryDirectory = ProductRoot / "libraries" / "projects" / project / role / profile,
		    .SymbolDirectory = SymbolsRoot / "projects" / project / role / profile};
	}

	std::filesystem::path ToolOutputPaths::CookedProjectDirectory(std::string_view projectName) const
	{
		return ProjectArtifactRoot / std::string(projectName) / "Cooked";
	}

	std::filesystem::path ToolOutputPaths::SharedCookedProjectDirectory() const
	{
		return CookedProjectDirectory("Shared");
	}
}
