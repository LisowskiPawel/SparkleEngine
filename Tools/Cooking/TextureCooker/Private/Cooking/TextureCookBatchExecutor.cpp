#include "PCH.h"

#include "Cooking/TextureCookBatchExecutor.h"

#include "Cooking/TextureAssetCooker.h"
#include "SourceLoading/TextureSource.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Diagnostics/Logger.h"
#include "Core/Public/Diagnostics/Verify.h"
#include "Core/Public/Files/FileUtils.h"

#include <TaskScheduler_c.h>
#include <memory>
#include <new>
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

const std::vector<std::exception_ptr>& TextureCookBatchExecutor::Execute()
{
	if (m_requests.size() > (std::numeric_limits<std::uint32_t>::max)())
	{
		throw Diagnostics::Error("Texture cook request count exceeds task capacity.");
	}
	if (m_publication.size() != m_requests.size())
	{
		throw Diagnostics::Error("Texture publication does not match its cook requests.");
	}
	m_results.assign(m_requests.size(), {});
	m_completedRequests.store(0);
	if (m_requests.empty())
	{
		return m_results;
	}

	const auto workers = ResolveBackgroundWorkerCount();
	if (workers == 1)
	{
		CookRange(0, static_cast<std::uint32_t>(m_requests.size()), 0, this);
		return m_results;
	}

	CookParallel(workers);
	return m_results;
}

void TextureCookBatchExecutor::CookParallel(std::uint32_t workers)
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
	std::unique_ptr<enkiTaskSet, decltype(deleteRange)> range(enkiCreateTaskSet(scheduler.get(), CookRange), deleteRange);
	if (!range)
	{
		throw std::bad_alloc();
	}
	enkiSetParamsTaskSet(range.get(), {this, static_cast<std::uint32_t>(m_requests.size()), 1, 0});
	enkiAddTaskSet(scheduler.get(), range.get());
	enkiWaitForTaskSet(scheduler.get(), range.get());
}

void TextureCookBatchExecutor::CookRange(std::uint32_t begin, std::uint32_t end, std::uint32_t, void* argument) noexcept
{
	auto& batch = *static_cast<TextureCookBatchExecutor*>(argument);
	for (auto index = begin; index < end; ++index)
	{
		try
		{
			batch.CookRequest(index);
		}
		catch (...)
		{
			batch.m_results[index] = std::current_exception();
		}
	}
}

void TextureCookBatchExecutor::CookRequest(std::uint32_t index)
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
}

std::uint32_t TextureCookBatchExecutor::ResolveBackgroundWorkerCount()
{
	const std::uint32_t hardwareThreads = std::max(1u, std::thread::hardware_concurrency());
	return std::clamp(hardwareThreads > 1 ? hardwareThreads - 1 : 1u, 1u, 4u);
}
