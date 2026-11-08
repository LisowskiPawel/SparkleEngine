#include "Maintenance/MaintenanceCleanPreview.h"
#include "Maintenance/MaintenanceCleanSteps.h"

#include <sstream>
#include <system_error>
#include <utility>

namespace SparkleLauncher
{
	static void CountCleanTarget(MaintenanceCleanTarget& target)
	{
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
				target.ByteCount += entry.file_size(errorCode);
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
		CountCleanTarget(target);
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

	static std::string FormatCleanTargetStats(const MaintenanceCleanTarget& target)
	{
		if (!target.bExists)
		{
			return "not present";
		}

		return std::to_string(target.FileCount) + " files, " + std::to_string(target.DirectoryCount) + " directories, " + std::to_string(target.ByteCount) + " bytes";
	}

	static void PopulatePlanSteps(MaintenanceOperationPlan& plan)
	{
		if (!plan.bCanRun)
		{
			return;
		}

		for (const MaintenanceCleanStep& cleanStep : BuildMaintenanceCleanSteps(plan))
		{
			MaintenanceOperationStep step;
			step.Id = cleanStep.Id;
			step.DisplayName = cleanStep.DisplayName;
			step.bDestructive = true;
			step.DestructivePath = cleanStep.DestructivePath;
			step.DisplayCommandLine = "Delete " + cleanStep.DestructivePath.string();
			plan.Steps.push_back(std::move(step));
		}
	}

	void DescribeMaintenancePlan(MaintenanceOperationPlan& plan)
	{
		PopulatePlanSteps(plan);
		for (const MaintenanceCleanTarget& target : plan.CleanTargets)
		{
			plan.PlannedEffects.push_back(target.DisplayName + ": " + target.Path.string() + " (" + target.Detail + "; " + FormatCleanTargetStats(target) + ")");
		}
		if (plan.Request.bCleanLevelContent)
		{
			plan.PlannedEffects.push_back("Deselect catalog levels after their extracted content is cleaned; cached archives are preserved.");
		}
		std::ostringstream dryRun;
		dryRun << "Dry-run plan for " << plan.Operation.DisplayName << ":";
		if (!plan.Operation.LogPath.empty())
		{
			dryRun << "\n  Latest log: " << plan.Operation.LogPath.string();
		}
		if (plan.Operation.bRequiresConfirmation)
		{
			dryRun << "\n  Confirmation required for clean scope: " << ToString(plan.Request.RequestedCleanScope);
		}
		for (const MaintenanceOperationStep& step : plan.Steps)
		{
			dryRun << "\n  " << step.DisplayName << ": " << step.DisplayCommandLine;
			if (!step.LogPath.empty())
			{
				dryRun << "\n    Log: " << step.LogPath.string();
			}
			if (step.bDestructive)
			{
				dryRun << "\n    Scope: " << step.DestructivePath.string();
			}
		}
		if (plan.Steps.empty())
		{
			dryRun
			    << (plan.Request.bCleanLevelContent && plan.bCanRun ? "\n  No extracted content requires deletion; catalog selection will be cleared."
			                                                        : "\n  No command step available until readiness issues are resolved.");
		}
		plan.Operation.DryRunText = dryRun.str();
	}
}
