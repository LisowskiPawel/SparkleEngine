#include "PCH.h"

#include "Core/Public/Paths/PackagePaths.h"

#include "Core/Public/Paths/PathUtils.h"
#include "PackageManifest.h"
#include "Paths/UserStatePaths.h"

namespace Filesystem
{
	std::optional<std::filesystem::path> DiscoverPackageProjectRoot(const std::filesystem::path& packageRoot)
	{
		return Private::ResolveContainedPackageDirectory(packageRoot, "ProjectPath");
	}


	std::optional<std::filesystem::path> DiscoverPackageEngineRoot(const std::filesystem::path& packageRoot)
	{
		return Private::ResolveContainedPackageDirectory(packageRoot, "EnginePath");
	}


	std::optional<std::filesystem::path> ResolvePackageContentRoot(const std::filesystem::path& packageRoot)
	{
		const auto relative = Private::ReadPackageRelativePath(packageRoot, "ContentStore");
		if (!relative)
		{
			return std::nullopt;
		}

		const std::filesystem::path userStateRoot = Private::ResolveSparkleUserStateRoot();
		if (Private::HasReparsePath(userStateRoot, *relative))
		{
			return std::nullopt;
		}

		const std::filesystem::path result = Paths::Normalize(userStateRoot / *relative);
		return Paths::IsUnderRoot(result, userStateRoot) ? std::optional(result) : std::nullopt;
	}


	std::optional<std::filesystem::path> ResolvePackageSourceRoot(const std::filesystem::path& packageRoot)
	{
		const auto content = ResolvePackageContentRoot(packageRoot);
		const auto project = DiscoverPackageProjectRoot(packageRoot);
		if (!content || !project)
		{
			return std::nullopt;
		}
		const auto relative = std::filesystem::path("Source") / project->filename();
		return Private::HasReparsePath(*content, relative) ? std::nullopt : std::optional(*content / relative);
	}

}
