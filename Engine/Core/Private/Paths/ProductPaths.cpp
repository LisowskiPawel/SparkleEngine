#include "PCH.h"

#include "Core/Public/Paths/ProductPaths.h"

#include "Core/Public/FileSystemUtils.h"
#include "Core/Public/Paths/PackagePaths.h"
#include "Core/Public/Paths/WorkspaceOutputPaths.h"
#include "Core/Public/Strings/StringUtils.h"

namespace Filesystem
{
	bool IsSourceWorkspace(const std::filesystem::path& root)
	{
		std::error_code errorCode;
		return std::filesystem::is_regular_file(root / std::string(kWorkspaceMarker), errorCode) && !errorCode;
	}

	static std::filesystem::path ResolveExecutable(
	    const std::filesystem::path& root,
	    const WorkspaceOutputPaths& outputs,
	    const std::filesystem::path& directory,
	    std::string_view target,
	    std::string_view profile)
	{
		std::string fileName(target);
#ifdef _WIN32
		fileName += ".exe";
#endif
		if (IsSourceWorkspace(root))
		{
			return directory / fileName;
		}
		const auto executable = ResolvePackageProductPath(root, target, profile);
		const auto relative = directory.lexically_relative(outputs.DevelopmentArtifactRoot) / fileName;
		return executable && executable->lexically_relative(root) == relative ? *executable : std::filesystem::path{};
	}

	std::filesystem::path ResolveProductToolPath(const std::filesystem::path& root, std::string_view target, std::string_view profile)
	{
		const auto outputs = ResolveWorkspaceOutputPaths(root);
		const auto product = outputs.ToolTargetOutputs(target, profile);
		return ResolveExecutable(root, outputs, product.BinaryDirectory, target, profile);
	}

	std::filesystem::path ResolveProjectProductPath(const std::filesystem::path& root, std::string_view project, std::string_view role, std::string_view profile, std::string_view variant)
	{
		if ((!variant.empty() && !IsSourceWorkspace(root)) || (role != "editor" && role != "game"))
		{
			return {};
		}
		const auto outputs = ResolveWorkspaceOutputPaths(root, variant);
		const auto product = outputs.ProjectTargetOutputs(project, role, profile);
		return ResolveExecutable(root, outputs, product.BinaryDirectory, std::string(project) + (role == "editor" ? "Editor" : "Game"), profile);
	}

	std::filesystem::path ResolveProductProjectRoot(const std::filesystem::path& root, std::string_view project)
	{
		if (IsSourceWorkspace(root))
		{
			return root / "Projects" / project;
		}
		const auto source = ResolvePackageSourceRoot(root);
		return source && Strings::ToNarrow(source->filename().wstring()) == project ? *source : std::filesystem::path{};
	}

	std::filesystem::path ResolveProductCookedRoot(const std::filesystem::path& root, std::string_view project)
	{
		if (IsSourceWorkspace(root))
		{
			return ResolveWorkspaceOutputPaths(root).CookedProjectDirectory(project);
		}
		const auto content = ResolvePackageContentRoot(root);
		return content && !ResolveProductProjectRoot(root, project).empty() ? *content / "Cooked" : std::filesystem::path{};
	}

	std::filesystem::path ResolveProductToolScratch(const std::filesystem::path& root, std::string_view tool, std::string_view profile)
	{
		if (IsSourceWorkspace(root))
		{
			return ResolveWorkspaceOutputPaths(root).ToolScratchDirectory(tool, profile);
		}
		const auto content = ResolvePackageContentRoot(root);
		return content ? *content / "Scratch" / tool : std::filesystem::path{};
	}
}
