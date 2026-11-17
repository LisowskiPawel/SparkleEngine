#include "PCH.h"

#include "Cooking/TextureCookBatchExecutor.h"

#include "Cooking/TextureAssetCooker.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Core/Public/Files/FileUtils.h"
#include "TaskExecution.h"
#include "TaskExecutionContext.h"
#include "TaskExecutor.h"
#include "TaskGraph.h"
#include "TaskTypes.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <thread>
#include <utility>
#include <vector>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_textureCookBatchExecutorLogger, "TextureCooker.BatchExecutor");

TextureCookBatchExecutor::TextureCookBatchExecutor(const std::vector<TextureCookRequest>& requests, std::size_t memoryBudgetBytes, std::function<void(std::size_t, std::size_t)> progress) :
    m_requests(requests),
    m_memoryLimiter(memoryBudgetBytes),
    m_progress(std::move(progress))
{
}

TextureCookBatchExecutor::~TextureCookBatchExecutor()
{
	for (const TextureCookBatchItemResult& item : m_items)
	{
		if (!item.StagedOutputPath.empty())
		{
			Files::CleanupTemporaryFile(item.StagedOutputPath);
		}
	}
}

const std::vector<TextureCookBatchItemResult>& TextureCookBatchExecutor::Execute()
{
	if (m_requests.size() > (std::numeric_limits<std::uint32_t>::max)())
	{
		throw Diagnostics::Error("Texture cook request count exceeds task capacity.");
	}

	m_items.resize(m_requests.size());
	for (std::size_t index = 0; index < m_requests.size(); ++index)
	{
		m_items[index].StagedOutputPath = Files::BuildTemporaryPath(m_requests[index].outputPath, ".cook-generation");
	}

	TaskExecutor executor(BuildExecutorConfig());
	TaskExecutionContext context;
	std::vector<TaskNodeHandle> taskHandles;
	const TaskExecution execution = executor.Submit(BuildTaskGraph(taskHandles), context);

	for (std::size_t index = 0; index < taskHandles.size(); ++index)
	{
		const std::optional<TaskResult> taskResult = execution.GetTaskResult(taskHandles[index]);
		if (!taskResult)
		{
			Diagnostics::Fatal(g_textureCookBatchExecutorLogger, __FILE__, __LINE__, "Texture cook execution settled without a result for one of its tasks.");
		}
		m_items[index].CookResult = *taskResult;
	}

	return m_items;
}

TaskExecutorConfig TextureCookBatchExecutor::BuildExecutorConfig() const
{
	const std::uint32_t maximumTasks = static_cast<std::uint32_t>(std::max<std::size_t>(m_requests.size(), 1));

	return TaskExecutorConfig{
	    .FrameCriticalWorkerCount = 1,
	    .BackgroundWorkerCount = ResolveBackgroundWorkerCount(),
	    .MaximumTasksPerExecution = maximumTasks,
	    .MaximumEdgesPerExecution = 1,
	    .MaximumActiveExecutions = 1};
}

CompiledTaskGraph TextureCookBatchExecutor::BuildTaskGraph(std::vector<TaskNodeHandle>& outTaskHandles)
{
	const std::uint32_t maximumTasks = static_cast<std::uint32_t>(std::max<std::size_t>(m_requests.size(), 1));
	TaskGraphBuilder graph(TaskGraphLimits{.MaximumTasks = maximumTasks, .MaximumEdges = 1});
	outTaskHandles.clear();
	outTaskHandles.reserve(m_requests.size());

	for (std::uint32_t index = 0; index < m_requests.size(); ++index)
	{
		outTaskHandles.push_back(graph.Add(TaskDesc{.Name = TaskName("Cook texture request"), .Lane = TaskLane::Background}, [this, index](TaskExecutionContext&) { return CookRequest(index); }));
	}

	return graph.Compile();
}

TaskResult TextureCookBatchExecutor::CookRequest(std::uint32_t index)
{
	TextureCookRequest stagedRequest = m_requests[index];
	stagedRequest.outputPath = m_items[index].StagedOutputPath;
	Files::CleanupTemporaryFile(stagedRequest.outputPath);

	CookTextureAsset(stagedRequest, m_memoryLimiter);

	if (m_progress)
	{
		const std::size_t completed = ++m_completedRequests;
		m_progress(index, completed);
	}
	return TaskResult::Success();
}

std::uint32_t TextureCookBatchExecutor::ResolveBackgroundWorkerCount()
{
	const std::uint32_t hardwareThreads = std::max(1u, std::thread::hardware_concurrency());
	return std::clamp(hardwareThreads > 1 ? hardwareThreads - 1 : 1u, 1u, 4u);
}
