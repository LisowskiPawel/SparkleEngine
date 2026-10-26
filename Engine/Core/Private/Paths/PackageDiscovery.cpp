#include "PCH.h"

#include "Core/Public/Paths/PackagePaths.h"

#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Paths/PathUtils.h"
#include "Core/Public/Paths/WorkspaceOutputPaths.h"

namespace Filesystem
{
	static bool IsDevelopmentExecutableDirectory(const std::filesystem::path& executableDirectory)
	{
		const auto source = FindAncestorWithMarker(executableDirectory, kWorkspaceMarker);
		if (!source)
		{
			return false;
		}
		const auto outputs = ResolveWorkspaceOutputPaths(*source);
		const auto relative = executableDirectory.lexically_relative(outputs.ArtifactRoot);
		auto component = relative.begin();
		if (component == relative.end() || *component == "..")
		{
			return false;
		}
		const bool canonical = *component == "dev";
		++component;
		return canonical || (component != relative.end() && *component == "dev");
	}

	static std::optional<std::filesystem::path> DiscoverShippingPackageRoot(const std::filesystem::path& executableDirectory)
	{
		const auto profile = executableDirectory.filename();
		if (profile != "ShippingEditor" && profile != "ShippingGame")
		{
			return std::nullopt;
		}
		const auto productDirectory = executableDirectory.parent_path();
		if (productDirectory.filename() == "Binaries" && (productDirectory.parent_path().parent_path().filename() == "Projects" || productDirectory.parent_path().parent_path().filename() == "Tools"))
		{
			return Paths::Normalize(productDirectory.parent_path().parent_path().parent_path());
		}
		return std::nullopt;
	}

	std::optional<std::filesystem::path> DiscoverPackageRoot(const std::filesystem::path& startPath)
	{
		const auto executableDirectory = Filesystem::GetExecutableDirectory();
		if (startPath.empty() || Paths::Normalize(startPath) == Paths::Normalize(executableDirectory))
		{
			if (IsDevelopmentExecutableDirectory(executableDirectory))
			{
				return std::nullopt;
			}
			if (const auto packageRoot = DiscoverShippingPackageRoot(executableDirectory))
			{
				return packageRoot;
			}
		}
		const auto packageRoot = FindAncestorWithMarker(startPath.empty() ? executableDirectory : startPath, "manifests/sparkle-package-manifest.json", 8);
		return packageRoot ? std::make_optional(Paths::Normalize(*packageRoot)) : std::nullopt;
	}
}
