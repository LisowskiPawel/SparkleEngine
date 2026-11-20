#include "PCH.h"

#include "Cooking/TextureCookBatchExecutor.h"

#include "Cooking/TextureAssetCooker.h"
#include "SourceLoading/TextureSource.h"
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

TextureCookBatchExecutor::TextureCookBatchExecutor(
    const std::vector<TextureCookRequest>& requests,
    std::span<const Files::FilePublication> publication,
    std::size_t memoryBudgetBytes,
    std::function<void(std::size_t, std::size_t)> progress) :
    m_requests(requests),
    m_publication(publication),
    m_memoryLimiter(memoryBudgetBytes),
    m_sourceMemoryLimiter(2 * MaximumTextureSourceBytes * ResolveBackgroundWorkerCount()),
    m_progress(std::move(progress))
{
}

TextureCookBatchExecutor::~TextureCookBatchExecutor()
{
	for (const Files::FilePublication& file : m_publication)
	{
		Files::CleanupTemporaryFile(file.StagedPath);
	}
}

const std::vector<TaskResult>& TextureCookBatchExecutor::Execute()
{
	if (m_requests.size() > (std::numeric_limits<std::uint32_t>::max)())
	{
		throw Diagnostics::Error("Texture cook request count exceeds task capacity.");
	}
	if (m_publication.size() != m_requests.size())
	{
		throw Diagnostics::Error("Texture publication does not match its cook requests.");
	}
	m_results.resize(m_requests.size(), TaskResult::Cancelled("Texture cook did not execute."));

	TaskExecutor executor(BuildExecutorConfig());
	TaskExecutionContext context;
	std::vector<TaskNodeHandle> taskHandles;
	const auto graph = BuildTaskGraph(taskHandles);
	const TaskExecution execution = executor.Submit(graph, context);

	for (std::size_t index = 0; index < taskHandles.size(); ++index)
	{
		const std::optional<TaskResult> taskResult = execution.GetTaskResult(taskHandles[index]);
		if (!taskResult)
		{
			Diagnostics::Fatal(g_textureCookBatchExecutorLogger, __FILE__, __LINE__, "Texture cook execution settled without a result for one of its tasks.");
		}
		m_results[index] = *taskResult;
	}

	return m_results;
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
		const auto task = graph.Add(TaskDesc{.Name = "Cook texture request", .Lane = TaskLane::Background}, [this, index](TaskExecutionContext&) { return CookRequest(index); });
		outTaskHandles.push_back(task);
	}

	return graph.Compile();
}

TaskResult TextureCookBatchExecutor::CookRequest(std::uint32_t index)
{
	TextureCookRequest stagedRequest = m_requests[index];
	stagedRequest.outputPath = m_publication[index].StagedPath;
	Files::CleanupTemporaryFile(stagedRequest.outputPath);

	CookTextureAsset(stagedRequest, m_memoryLimiter, m_sourceMemoryLimiter);
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
