#include "Maintenance/MaintenanceCleanPreview.h"
#include "Maintenance/MaintenanceCleanSteps.h"

#include <system_error>
#include <utility>

namespace SparkleLauncher
{
	void InspectMaintenanceCleanTarget(MaintenanceCleanTarget& target)
	{
		target.FileCount = 0;
		target.DirectoryCount = 0;
		target.ByteCount = 0;
		std::error_code errorCode;
		target.bExists = std::filesystem::exists(target.Path, errorCode);
		if (!target.bExists || errorCode)
		{
			return;
		}

		if (std::filesystem::is_regular_file(target.Path, errorCode))
		{
			target.FileCount = 1;
			target.ByteCount = std::filesystem::file_size(target.Path, errorCode);
			if (errorCode)
			{
				target.ByteCount = 0;
			}
			return;
		}

		if (!std::filesystem::is_directory(target.Path, errorCode))
		{
			return;
		}

		std::filesystem::recursive_directory_iterator iterator(target.Path, std::filesystem::directory_options::skip_permission_denied, errorCode);
		const std::filesystem::recursive_directory_iterator end;
		while (iterator != end)
		{
			const std::filesystem::directory_entry entry = *iterator;
			if (entry.is_directory(errorCode))
			{
				++target.DirectoryCount;
			}
			else if (entry.is_regular_file(errorCode))
			{
				++target.FileCount;
				const auto byteCount = entry.file_size(errorCode);
				if (!errorCode)
				{
					target.ByteCount += byteCount;
				}
			}
			errorCode.clear();
			iterator.increment(errorCode);
			errorCode.clear();
		}
	}

	static void AddCleanTarget(MaintenanceOperationPlan& plan, std::string displayName, std::filesystem::path path, std::string detail)
	{
		MaintenanceCleanTarget target;
		target.DisplayName = std::move(displayName);
		target.Path = std::move(path);
		target.Detail = std::move(detail);
		InspectMaintenanceCleanTarget(target);
		plan.CleanTargets.push_back(std::move(target));
	}

	static std::string CleanTargetDisplayName(std::string_view displayName)
	{
		static constexpr std::string_view cleanPrefix = "Clean ";
		return std::string(displayName.starts_with(cleanPrefix) ? displayName.substr(cleanPrefix.size()) : displayName);
	}

	static std::string CleanTargetDetail(const MaintenanceCleanStep& step)
	{
		if (!step.PreviewDetail.empty())
		{
			return step.PreviewDetail;
		}

		switch (step.CleanBehavior)
		{
			case MaintenanceCleanBehavior::RemoveDirectoryContentsPreservingPath:
				return "Directory contents are removed except the preserved path: " + step.PreservedPath.string();
			case MaintenanceCleanBehavior::RemoveRootGeneratedFiles:
				return "Only recognized root CMake and IDE generated files are removed.";
			case MaintenanceCleanBehavior::RemovePath:
				return "The exact generated or user-local path selected by this clean scope is removed.";
		}
		return "Generated output selected by this clean scope.";
	}

	void InspectMaintenanceCleanTargets(MaintenanceOperationPlan& plan)
	{
		for (const MaintenanceCleanStep& step : BuildMaintenanceCleanSteps(plan))
		{
			AddCleanTarget(plan, CleanTargetDisplayName(step.DisplayName), step.DestructivePath, CleanTargetDetail(step));
		}
	}

}
