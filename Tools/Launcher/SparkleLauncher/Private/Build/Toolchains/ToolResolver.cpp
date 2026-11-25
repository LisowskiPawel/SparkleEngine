#include "SparkleLauncher/ToolResolver.h"

#include "Core/Public/Strings/StringUtils.h"
#include "Core/Public/Environment/EnvironmentVariables.h"

#include <algorithm>
#include <array>
#include <optional>
#include <sstream>
#include <system_error>

namespace SparkleLauncher
{
	struct KnownToolDescription final
	{
		KnownTool Tool = KnownTool::CMake;
		const char* DisplayName = "Unknown";
		const char* OverrideEnvironment = nullptr;
		std::array<const char*, 4> ExecutableNames{};
	};

	static constexpr KnownToolDescription kKnownTools[] = {
	    {KnownTool::CMake, "CMake", "SPARKLE_CMAKE_EXE", {"cmake.exe", "cmake"}},
	    {KnownTool::MSBuild, "MSBuild", "SPARKLE_MSBUILD_EXE", {"MSBuild.exe", "msbuild.exe", "MSBuild"}},
	    {KnownTool::Ninja, "Ninja", "SPARKLE_NINJA_EXE", {"ninja.exe", "ninja"}},
	    {KnownTool::Rider, "Rider", "SPARKLE_RIDER_EXE", {"rider64.exe", "rider.exe", "rider.bat", "rider"}},
	    {KnownTool::Git, "Git", "SPARKLE_GIT_EXE", {"git.exe", "git"}},
	};

	static const KnownToolDescription& DescribeTool(KnownTool tool) noexcept
	{
		for (const KnownToolDescription& description : kKnownTools)
		{
			if (description.Tool == tool)
			{
				return description;
			}
		}

		static constexpr KnownToolDescription unknown{};
		return unknown;
	}

	static void AddEnvironmentPath(std::vector<std::filesystem::path>& candidates, const char* environmentName, const std::filesystem::path& relativePath = {})
	{
		std::string root;
		const bool bHasRoot = Environment::TryGetVariable(environmentName, root);
		if (bHasRoot)
		{
			candidates.push_back(std::filesystem::path(root) / relativePath);
		}
	}

	static std::string BuildPathSortKey(const std::filesystem::path& path);

	static void AddProgramFilesDirectoryMatches(
	    std::vector<std::filesystem::path>& candidates,
	    const char* environmentName,
	    std::string_view directoryPrefix,
	    const std::filesystem::path& relativeExecutablePath)
	{
		std::string root;
		const bool bHasRoot = Environment::TryGetVariable(environmentName, root);
		if (!bHasRoot)
		{
			return;
		}

		const std::filesystem::path parentDirectory = std::filesystem::path(root) / "JetBrains";
		std::error_code errorCode;
		if (!std::filesystem::is_directory(parentDirectory, errorCode))
		{
			return;
		}

		std::vector<std::filesystem::path> matches;
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(parentDirectory, errorCode))
		{
			if (errorCode)
			{
				errorCode.clear();
				continue;
			}

			if (!entry.is_directory(errorCode))
			{
				errorCode.clear();
				continue;
			}

			const std::string directoryName = entry.path().filename().string();
			if (directoryName.rfind(std::string(directoryPrefix), 0) == 0)
			{
				matches.push_back(entry.path() / relativeExecutablePath);
			}
			errorCode.clear();
		}

		std::ranges::sort(matches, [](const std::filesystem::path& left, const std::filesystem::path& right) { return BuildPathSortKey(left) < BuildPathSortKey(right); });
		candidates.insert(candidates.end(), matches.begin(), matches.end());
	}

	static std::string BuildPathSortKey(const std::filesystem::path& path)
	{
		return Strings::ToLowerCopy(path.lexically_normal().generic_string());
	}

	static void AddRecursiveRiderCandidates(std::vector<std::filesystem::path>& candidates, const std::filesystem::path& root)
	{
		std::error_code errorCode;
		if (!std::filesystem::is_directory(root, errorCode))
		{
			return;
		}

		std::vector<std::filesystem::path> rider64Matches;
		std::vector<std::filesystem::path> riderMatches;
		std::vector<std::filesystem::path> scriptMatches;
		std::filesystem::recursive_directory_iterator iterator(root, std::filesystem::directory_options::skip_permission_denied, errorCode);
		const std::filesystem::recursive_directory_iterator end;
		while (iterator != end)
		{
			if (errorCode)
			{
				errorCode.clear();
				iterator.increment(errorCode);
				continue;
			}
			if (iterator.depth() > 5)
			{
				iterator.disable_recursion_pending();
			}

			const std::filesystem::directory_entry entry = *iterator;
			if (entry.is_regular_file(errorCode))
			{
				const std::string filename = entry.path().filename().string();
				// Match executables through iterator depth 5 and scripts through depth 6.
				const bool bExecutableDepth = iterator.depth() <= 5;
				if (bExecutableDepth && Strings::EqualsIgnoreCase(filename, "rider64.exe"))
				{
					rider64Matches.push_back(entry.path());
				}
				else if (bExecutableDepth && Strings::EqualsIgnoreCase(filename, "rider.exe"))
				{
					riderMatches.push_back(entry.path());
				}
				else if (Strings::EqualsIgnoreCase(filename, "rider.cmd"))
				{
					scriptMatches.push_back(entry.path());
				}
			}
			errorCode.clear();
			iterator.increment(errorCode);
		}

		for (auto* matches : {&rider64Matches, &riderMatches, &scriptMatches})
		{
			std::ranges::sort(*matches, [](const std::filesystem::path& left, const std::filesystem::path& right) { return BuildPathSortKey(left) < BuildPathSortKey(right); });
			candidates.insert(candidates.end(), matches->begin(), matches->end());
		}
	}

	static void AddVisualStudioMSBuildCandidates(std::vector<std::filesystem::path>& candidates)
	{
		const std::vector<std::string> versions = {"18", "2026", "17", "2022"};
		const std::vector<std::string> editions = {"Community", "Professional", "Enterprise", "BuildTools"};
		for (const std::string& version : versions)
		{
			for (const std::string& edition : editions)
			{
				AddEnvironmentPath(candidates, "ProgramFiles", std::filesystem::path("Microsoft Visual Studio") / version / edition / "MSBuild" / "Current" / "Bin" / "MSBuild.exe");
			}
		}
	}

	static void AddJetBrainsRiderCandidates(std::vector<std::filesystem::path>& candidates)
	{
		AddEnvironmentPath(candidates, "LocalAppData", std::filesystem::path("Programs") / "Rider" / "bin" / "rider64.exe");
		AddEnvironmentPath(candidates, "LocalAppData", std::filesystem::path("JetBrains") / "Toolbox" / "scripts" / "rider.cmd");
		AddEnvironmentPath(candidates, "ProgramFiles", std::filesystem::path("JetBrains") / "JetBrains Rider" / "bin" / "rider64.exe");
		AddEnvironmentPath(candidates, "ProgramFiles(x86)", std::filesystem::path("JetBrains") / "JetBrains Rider" / "bin" / "rider64.exe");
		AddProgramFilesDirectoryMatches(candidates, "ProgramFiles", "JetBrains Rider", std::filesystem::path("bin") / "rider64.exe");
		AddProgramFilesDirectoryMatches(candidates, "ProgramFiles(x86)", "JetBrains Rider", std::filesystem::path("bin") / "rider64.exe");

		std::vector<std::filesystem::path> searchRoots;
		AddEnvironmentPath(searchRoots, "ProgramFiles", std::filesystem::path("JetBrains"));
		AddEnvironmentPath(searchRoots, "ProgramFiles(x86)", std::filesystem::path("JetBrains"));
		AddEnvironmentPath(searchRoots, "LocalAppData", std::filesystem::path("Programs"));
		AddEnvironmentPath(searchRoots, "LocalAppData", std::filesystem::path("JetBrains"));
		AddEnvironmentPath(searchRoots, "LocalAppData", std::filesystem::path("JetBrains") / "Toolbox");

		for (const std::filesystem::path& root : searchRoots)
		{
			AddRecursiveRiderCandidates(candidates, root);
		}
	}

	static std::vector<std::filesystem::path> GetKnownInstallCandidates(KnownTool tool)
	{
		std::vector<std::filesystem::path> candidates;
		std::string overridePath;
		if (Environment::TryGetVariable(DescribeTool(tool).OverrideEnvironment, overridePath))
		{
			candidates.emplace_back(overridePath);
		}

		switch (tool)
		{
			case KnownTool::CMake:
				AddEnvironmentPath(candidates, "ProgramFiles", std::filesystem::path("CMake") / "bin" / "cmake.exe");
				break;
			case KnownTool::MSBuild:
				AddVisualStudioMSBuildCandidates(candidates);
				break;
			case KnownTool::Ninja:
				AddEnvironmentPath(candidates, "ProgramFiles", std::filesystem::path("CMake") / "bin" / "ninja.exe");
				break;
			case KnownTool::Rider:
				AddJetBrainsRiderCandidates(candidates);
				break;
			case KnownTool::Git:
				AddEnvironmentPath(candidates, "ProgramFiles", std::filesystem::path("Git") / "cmd" / "git.exe");
				break;
		}

		return candidates;
	}

	static std::optional<std::filesystem::path> FindExistingExecutable(const std::vector<std::filesystem::path>& candidates)
	{
		std::error_code errorCode;
		for (const std::filesystem::path& candidate : candidates)
		{
			if (std::filesystem::exists(candidate, errorCode) && std::filesystem::is_regular_file(candidate, errorCode))
			{
				return candidate;
			}
			errorCode.clear();
		}
		return std::nullopt;
	}

	std::string ToString(KnownTool tool)
	{
		return DescribeTool(tool).DisplayName;
	}

	std::vector<std::filesystem::path> GetExecutableSearchPath()
	{
		std::vector<std::filesystem::path> searchPaths;
		std::string pathValue;
		if (!Environment::TryGetVariable("PATH", pathValue))
		{
			return searchPaths;
		}

#if defined(_WIN32)
		constexpr char separator = ';';
#else
		constexpr char separator = ':';
#endif
		std::stringstream stream(pathValue);
		std::string segment;
		while (std::getline(stream, segment, separator))
		{
			if (!segment.empty())
			{
				searchPaths.emplace_back(segment);
			}
		}

		return searchPaths;
	}

	std::optional<std::filesystem::path> FindExecutableOnPath(std::string_view executableName)
	{
		std::error_code errorCode;
		for (const std::filesystem::path& directory : GetExecutableSearchPath())
		{
			std::filesystem::path candidate = directory / std::string(executableName);
			if (std::filesystem::exists(candidate, errorCode) && std::filesystem::is_regular_file(candidate, errorCode))
			{
				return candidate;
			}
		}

		return std::nullopt;
	}

	ToolResolveResult ResolveKnownTool(KnownTool tool)
	{
		ToolResolveResult result;
		result.Tool = tool;
		result.DisplayName = ToString(tool);

		for (const char* candidateName : DescribeTool(tool).ExecutableNames)
		{
			if (candidateName == nullptr)
			{
				break;
			}
			if (const std::optional<std::filesystem::path> path = FindExecutableOnPath(candidateName))
			{
				result.bFound = true;
				result.Path = *path;
				return result;
			}
		}

		if (const std::optional<std::filesystem::path> path = FindExistingExecutable(GetKnownInstallCandidates(tool)))
		{
			result.bFound = true;
			result.Path = *path;
			return result;
		}

		result.FailureReason = result.DisplayName + " executable was not found on PATH or in known install locations.";
		return result;
	}
}
