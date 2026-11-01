#include "LauncherProductPaths.h"

#include "ToolOutputPaths.h"

#include <string>
#include <system_error>

namespace SparkleLauncher
{
	bool HasBuildSources(const std::filesystem::path& root)
	{
		std::error_code error;
		return std::filesystem::is_regular_file(root / "CMakeLists.txt", error) && !error;
	}

	static std::filesystem::path ResolveExecutable(const std::filesystem::path& directory, std::string_view target)
	{
		std::string fileName(target);
#ifdef _WIN32
		fileName += ".exe";
#endif
		return directory / fileName;
	}

	std::filesystem::path ResolveToolExecutable(const std::filesystem::path& root, std::string_view target, std::string_view profile)
	{
		const auto outputs = SparkleTools::ResolveToolOutputPaths(root);
		const auto product = outputs.ToolTargetOutputs(target, profile);
		return ResolveExecutable(product.BinaryDirectory, target);
	}

	std::filesystem::path ResolveProjectExecutable(const std::filesystem::path& root, std::string_view project, std::string_view role, std::string_view profile, std::string_view variant)
	{
		if (role != "editor" && role != "game")
		{
			return {};
		}
		const auto outputs = variant.empty() ? SparkleTools::ResolveToolOutputPaths(root) : SparkleTools::ResolveToolOutputPaths(root, variant);
		const auto product = outputs.ProjectTargetOutputs(project, role, profile);
		const std::string targetName = std::string(project) + (role == "editor" ? "Editor" : "Game");
		return ResolveExecutable(product.BinaryDirectory, targetName);
	}

	std::filesystem::path ResolveProjectRoot(const std::filesystem::path& root, std::string_view project)
	{
		if (project.empty() || project == "." || project == ".." || project.find_first_of("/\\:") != std::string_view::npos)
		{
			return {};
		}
		return root / "Projects" / project;
	}

	std::filesystem::path ResolveCookedRoot(const std::filesystem::path& root, std::string_view project)
	{
		const auto outputs = SparkleTools::ResolveToolOutputPaths(root);
		return outputs.CookedProjectDirectory(project);
	}

}
