#include "Maintenance/MaintenanceCleanExecution.h"
#include "Maintenance/MaintenanceCleanSteps.h"
#include "Core/Public/Strings/StringUtils.h"

#include <system_error>

#if defined(_WIN32)
  #include <Windows.h>
#endif

namespace SparkleLauncher
{
	static std::filesystem::path NormalizePathForCompare(const std::filesystem::path& path)
	{
		std::error_code errorCode;
		const std::filesystem::path absolutePath = std::filesystem::absolute(path, errorCode);
		return (errorCode ? path : absolutePath).lexically_normal();
	}

	static bool PathsEqual(const std::filesystem::path& left, const std::filesystem::path& right)
	{
#if defined(_WIN32)
		return Strings::ToLowerCopy(left.generic_string()) == Strings::ToLowerCopy(right.generic_string());
#else
		return left == right;
#endif
	}

	static bool IsSameOrDescendantPath(const std::filesystem::path& candidate, const std::filesystem::path& ancestor)
	{
		const std::filesystem::path normalizedCandidate = NormalizePathForCompare(candidate);
		const std::filesystem::path normalizedAncestor = NormalizePathForCompare(ancestor);
		auto candidateIt = normalizedCandidate.begin();
		auto ancestorIt = normalizedAncestor.begin();
		for (; ancestorIt != normalizedAncestor.end(); ++ancestorIt, ++candidateIt)
		{
			if (candidateIt == normalizedCandidate.end() || !PathsEqual(*candidateIt, *ancestorIt))
			{
				return false;
			}
		}
		return true;
	}

	static std::filesystem::path MakePlatformDeletePath(const std::filesystem::path& path)
	{
#if defined(_WIN32)
		std::error_code errorCode;
		const std::filesystem::path absolutePath = std::filesystem::absolute(path, errorCode);
		std::filesystem::path candidate = errorCode ? path : absolutePath;
		const std::wstring& nativePath = candidate.native();
		if (nativePath.rfind(LR"(\\?\)", 0) == 0)
		{
			return candidate;
		}
		if (nativePath.rfind(LR"(\\)", 0) == 0)
		{
			return std::filesystem::path(std::wstring(LR"(\\?\UNC\)") + nativePath.substr(2));
		}
		return std::filesystem::path(std::wstring(LR"(\\?\)") + nativePath);
#else
		return path;
#endif
	}

	static bool FailCleanPath(const std::filesystem::path& path, const std::error_code& error, std::string& outErrorMessage)
	{
		outErrorMessage = "Clean blocked while inspecting or removing '" + path.string() + "': " + error.message();
		return false;
	}

	static bool InspectCleanPath(const std::filesystem::path& path, std::filesystem::file_status& status, std::string& outErrorMessage)
	{
		std::error_code error;
		status = std::filesystem::status(path, error);

#if defined(_WIN32)
		// Win32 invalid-name errors also map to the generic missing-file condition.
		const bool bMissing = error == std::error_code(ERROR_FILE_NOT_FOUND, std::system_category()) || error == std::error_code(ERROR_PATH_NOT_FOUND, std::system_category());
#else
		const bool bMissing = error == std::errc::no_such_file_or_directory || error == std::errc::not_a_directory;
#endif
		if (error && !bMissing)
		{
			return FailCleanPath(path, error, outErrorMessage);
		}
		return true;
	}

	static bool RemovePath(const std::filesystem::path& path, std::string& outErrorMessage)
	{
		std::filesystem::file_status status;
		if (!InspectCleanPath(path, status, outErrorMessage))
		{
			return false;
		}
		if (!std::filesystem::exists(status))
		{
			outErrorMessage.clear();
			return true;
		}

		std::error_code error;
		std::filesystem::remove_all(path, error);
		if (error)
		{
			std::filesystem::remove_all(MakePlatformDeletePath(path), error);
		}
		if (error)
		{
			return FailCleanPath(path, error, outErrorMessage);
		}
		if (!InspectCleanPath(path, status, outErrorMessage))
		{
			return false;
		}
		if (std::filesystem::exists(status))
		{
			return FailCleanPath(path, std::make_error_code(std::errc::device_or_resource_busy), outErrorMessage);
		}
		outErrorMessage.clear();
		return true;
	}

	template <typename Action> static bool VisitCleanDirectory(const std::filesystem::path& directory, Action&& action, std::string& outErrorMessage)
	{
		std::error_code error;
		std::filesystem::directory_iterator entry(directory, error);
		const std::filesystem::directory_iterator end;
		while (!error && entry != end)
		{
			if (!action(*entry))
			{
				return false;
			}
			entry.increment(error);
		}
		if (error)
		{
			return FailCleanPath(directory, error, outErrorMessage);
		}
		outErrorMessage.clear();
		return true;
	}

	static bool RemovePathPreservingChildren(const std::filesystem::path& path, const std::vector<std::filesystem::path>& preservedPaths, std::string& outErrorMessage)
	{
		bool bHasPreservedDescendant = false;
		for (const std::filesystem::path& preservedPath : preservedPaths)
		{
			if (PathsEqual(NormalizePathForCompare(path), NormalizePathForCompare(preservedPath)))
			{
				outErrorMessage.clear();
				return true;
			}
			bHasPreservedDescendant = bHasPreservedDescendant || IsSameOrDescendantPath(preservedPath, path);
		}
		if (!bHasPreservedDescendant)
		{
			return RemovePath(path, outErrorMessage);
		}
		std::filesystem::file_status status;
		if (!InspectCleanPath(path, status, outErrorMessage))
		{
			return false;
		}
		if (!std::filesystem::is_directory(status))
		{
			return RemovePath(path, outErrorMessage);
		}

		const bool bChildrenRemoved = VisitCleanDirectory(
		    path,
		    [&](const std::filesystem::directory_entry& entry) { return RemovePathPreservingChildren(entry.path(), preservedPaths, outErrorMessage); },
		    outErrorMessage);

		if (!bChildrenRemoved)
		{
			return false;
		}
		std::error_code error;
		const bool bEmpty = std::filesystem::is_empty(path, error);
		if (error)
		{
			return FailCleanPath(path, error, outErrorMessage);
		}
		return !bEmpty || RemovePath(path, outErrorMessage);
	}

	static bool InspectCleanDirectory(const std::filesystem::path& directory, bool& bPresent, std::string& outErrorMessage)
	{
		std::filesystem::file_status status;
		if (!InspectCleanPath(directory, status, outErrorMessage))
		{
			return false;
		}
		bPresent = std::filesystem::exists(status);
		if (bPresent && !std::filesystem::is_directory(status))
		{
			return FailCleanPath(directory, std::make_error_code(std::errc::not_a_directory), outErrorMessage);
		}
		return true;
	}

	static bool RemoveDirectoryContentsPreservingPath(const std::filesystem::path& directory, const std::filesystem::path& preservedPath, std::string& outErrorMessage)
	{
		bool bPresent = false;
		if (!InspectCleanDirectory(directory, bPresent, outErrorMessage))
		{
			return false;
		}
		if (!bPresent)
		{
			outErrorMessage.clear();
			return true;
		}

		return VisitCleanDirectory(
		    directory,
		    [&](const std::filesystem::directory_entry& entry)
		    {
			    const bool bPreserved = PathsEqual(NormalizePathForCompare(entry.path()), NormalizePathForCompare(preservedPath));
			    return bPreserved || RemovePath(entry.path(), outErrorMessage);
		    },
		    outErrorMessage);
	}

	static bool IsRootGeneratedFile(const std::filesystem::path& path)
	{
		const std::string extension = path.extension().string();
		const std::string filename = path.filename().string();
		static constexpr std::string_view kRiderSettingsSuffix = ".DotSettings.user";

		const bool bIsRiderSettingsFile = filename.size() >= kRiderSettingsSuffix.size()
		    && filename.compare(filename.size() - kRiderSettingsSuffix.size(), kRiderSettingsSuffix.size(), kRiderSettingsSuffix) == 0;

		return extension == ".sln" || extension == ".slnx" || extension == ".vcxproj" || extension == ".filters" || extension == ".user" || filename == "CMakeCache.txt"
		    || filename == "cmake_install.cmake" || filename == "Makefile" || bIsRiderSettingsFile;
	}

	static bool RemoveRootGeneratedFiles(const std::filesystem::path& workspaceRoot, std::string& outErrorMessage)
	{
		bool bPresent = false;
		if (!InspectCleanDirectory(workspaceRoot, bPresent, outErrorMessage))
		{
			return false;
		}
		if (!bPresent)
		{
			outErrorMessage.clear();
			return true;
		}

		return VisitCleanDirectory(
		    workspaceRoot,
		    [&](const std::filesystem::directory_entry& entry)
		    {
			    std::filesystem::file_status status;
			    if (!InspectCleanPath(entry.path(), status, outErrorMessage))
			    {
				    return false;
			    }
			    const bool bGeneratedDirectory = std::filesystem::is_directory(status) && entry.path().filename() == "CMakeFiles";
			    const bool bGeneratedFile = std::filesystem::is_regular_file(status) && IsRootGeneratedFile(entry.path());
			    const bool bGenerated = bGeneratedDirectory || bGeneratedFile;
			    return !bGenerated || RemovePath(entry.path(), outErrorMessage);
		    },
		    outErrorMessage);
	}

	bool RunCleanStep(const MaintenanceCleanStep& step, const std::vector<std::filesystem::path>& preservedPaths, std::string& outErrorMessage)
	{
		switch (step.CleanBehavior)
		{
			case MaintenanceCleanBehavior::RemovePath:
				return preservedPaths.empty() ? RemovePath(step.DestructivePath, outErrorMessage) : RemovePathPreservingChildren(step.DestructivePath, preservedPaths, outErrorMessage);
			case MaintenanceCleanBehavior::RemoveDirectoryContentsPreservingPath:
				return RemoveDirectoryContentsPreservingPath(step.DestructivePath, step.PreservedPath, outErrorMessage);
			case MaintenanceCleanBehavior::RemoveRootGeneratedFiles:
				return RemoveRootGeneratedFiles(step.DestructivePath, outErrorMessage);
		}

		outErrorMessage = "Unknown clean behavior.";
		return false;
	}

}
