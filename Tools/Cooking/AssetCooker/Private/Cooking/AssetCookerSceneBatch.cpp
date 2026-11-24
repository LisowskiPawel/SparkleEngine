#include "AssetCookerSceneBatch.h"

#include "CookedSceneGenerationWriter.h"
#include "Core/Public/Diagnostics/Error.h"
#include "CookImportedScene.h"
#include <TaskScheduler_c.h>
#include <memory>
#include <new>

#include <algorithm>
#include <atomic>
#include <limits>
#include <string>
#include <thread>

struct AssetCookerSceneBatch::Item final
{
	CookedSceneBuild Build;
	AssetCookerDiagnostics Diagnostics;
	std::exception_ptr Failure;
};

bool AssetCookerSceneBatch::Execute(const std::vector<AssetCookerSceneEntry>& sceneEntries, AssetCookerDiagnostics& diagnostics, const ToolWorkProgressCallback& progress)
{
	if (sceneEntries.size() > (std::numeric_limits<std::uint32_t>::max)())
	{
		diagnostics.AddError("Scene cook count exceeds task capacity.");
		return false;
	}

	const std::size_t totalWork = sceneEntries.size() + 1;
	if (progress)
	{
		progress({.Action = "Cooking scene assets", .Total = totalWork});
	}
	std::vector<Item> items(sceneEntries.size());
	const auto buildFailure = BuildProducts(sceneEntries, items, progress, totalWork);

	MergeDiagnostics(items, diagnostics);
	if (buildFailure)
	{
		try
		{
			std::rethrow_exception(buildFailure);
		}
		catch (const std::exception& error)
		{
			diagnostics.AddError("Scene asset generation failed before publication: " + std::string(error.what()));
		}
		catch (...)
		{
			diagnostics.AddError("Scene asset generation failed before publication: unknown failure.");
		}
		return false;
	}

	if (progress)
	{
		progress({.Action = "Publishing cooked scene assets", .Completed = sceneEntries.size(), .Total = totalWork});
	}
	const bool bPublished = PublishProducts(items, diagnostics);
	if (bPublished && progress)
	{
		progress({.Action = "Cooked scene assets published", .Completed = totalWork, .Total = totalWork});
	}
	return bPublished;
}

std::uint32_t AssetCookerSceneBatch::ResolveWorkerCount() noexcept
{
	const std::uint32_t hardwareThreads = std::max(1u, std::thread::hardware_concurrency());
	return std::clamp(hardwareThreads > 1u ? hardwareThreads - 1u : 1u, 1u, 4u);
}

struct AssetCookerSceneBatch::BuildRun final
{
	const std::vector<AssetCookerSceneEntry>& Entries;
	std::vector<Item>& Items;
	const ToolWorkProgressCallback& Progress;
	std::size_t TotalWork;
	std::atomic<std::size_t> Completed{0};
};

void AssetCookerSceneBatch::BuildRange(std::uint32_t begin, std::uint32_t end, std::uint32_t, void* argument) noexcept
{
	auto& run = *static_cast<BuildRun*>(argument);
	for (auto index = begin; index < end; ++index)
	{
		auto& item = run.Items[index];
		try
		{
			item.Build = CookImportedScene(run.Entries[index].sourcePath, item.Diagnostics);
			if (run.Progress)
			{
				const auto completed = ++run.Completed;
				run.Progress({.Action = "Cooked scene", .Item = run.Entries[index].relativePath, .Completed = completed, .Total = run.TotalWork});
			}
		}
		catch (...)
		{
			item.Failure = std::current_exception();
		}
	}
}

std::exception_ptr AssetCookerSceneBatch::BuildProducts(
    const std::vector<AssetCookerSceneEntry>& sceneEntries,
    std::vector<Item>& items,
    const ToolWorkProgressCallback& progress,
    std::size_t totalWork)
{
	if (sceneEntries.empty())
	{
		return {};
	}
	BuildRun run{sceneEntries, items, progress, totalWork};
	const auto workers = ResolveWorkerCount();
	if (workers == 1)
	{
		BuildRange(0, static_cast<std::uint32_t>(sceneEntries.size()), 0, &run);
	}
	else
	{
		try
		{
			auto scheduler = std::unique_ptr<enkiTaskScheduler, decltype(&enkiDeleteTaskScheduler)>(enkiNewTaskScheduler(), enkiDeleteTaskScheduler);
			if (!scheduler)
			{
				throw std::bad_alloc();
			}
			auto config = enkiGetTaskSchedulerConfig(scheduler.get());
			config.numTaskThreadsToCreate = workers - 1;
			config.numExternalTaskThreads = 0;
			char error[256]{};
			if (!enkiTryInitTaskSchedulerWithConfig(scheduler.get(), config, error, sizeof(error)))
			{
				throw Diagnostics::Error(error);
			}

			auto deleteRange = [&scheduler](enkiTaskSet* task)
			{
				enkiDeleteTaskSet(scheduler.get(), task);
			};
			std::unique_ptr<enkiTaskSet, decltype(deleteRange)> range(enkiCreateTaskSet(scheduler.get(), BuildRange), deleteRange);
			if (!range)
			{
				throw std::bad_alloc();
			}
			enkiSetParamsTaskSet(range.get(), {&run, static_cast<std::uint32_t>(sceneEntries.size()), 1, 0});
			enkiAddTaskSet(scheduler.get(), range.get());
			enkiWaitForTaskSet(scheduler.get(), range.get());
		}
		catch (...)
		{
			return std::current_exception();
		}
	}
	for (const auto& item : items)
	{
		if (item.Failure)
		{
			return item.Failure;
		}
	}
	return {};
}

void AssetCookerSceneBatch::MergeDiagnostics(std::vector<Item>& items, AssetCookerDiagnostics& diagnostics)
{
	for (Item& item : items)
	{
		diagnostics.Append(item.Diagnostics.ReleaseRecords());
	}
}

bool AssetCookerSceneBatch::PublishProducts(std::vector<Item>& items, AssetCookerDiagnostics& diagnostics)
{
	std::vector<const CookedSceneBuild*> builds;
	builds.reserve(items.size());
	for (const Item& item : items)
	{
		builds.push_back(&item.Build);
	}

	try
	{
		CookedSceneGenerationWriter::Publish(builds);
		return true;
	}
	catch (const Diagnostics::Error& error)
	{
		diagnostics.AddError(error.what());
		return false;
	}
}
