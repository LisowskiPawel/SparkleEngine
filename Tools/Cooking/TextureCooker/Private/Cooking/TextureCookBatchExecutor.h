#pragma once

#include "TextureCookRequestList.h"
#include "TaskTypes.h"
#include "Cooking/TextureCookMemoryLimiter.h"
#include "Core/Public/Files/FileSetPublication.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

class CompiledTaskGraph;
class TaskNodeHandle;
struct TaskExecutorConfig;

class TextureCookBatchExecutor final
{
public:
	TextureCookBatchExecutor(
	    const std::vector<TextureCookRequest>& requests,
	    std::span<const Files::FilePublication> publication,
	    std::size_t memoryBudgetBytes,
	    std::function<void(std::size_t, std::size_t)> progress = {});
	~TextureCookBatchExecutor();

	TextureCookBatchExecutor(const TextureCookBatchExecutor&) = delete;
	TextureCookBatchExecutor& operator=(const TextureCookBatchExecutor&) = delete;

	const std::vector<TaskResult>& Execute();

private:
	TaskExecutorConfig BuildExecutorConfig() const;
	CompiledTaskGraph BuildTaskGraph(std::vector<TaskNodeHandle>& outTaskHandles);
	TaskResult CookRequest(std::uint32_t index);
	static std::uint32_t ResolveBackgroundWorkerCount();

	const std::vector<TextureCookRequest>& m_requests;
	std::span<const Files::FilePublication> m_publication;
	TextureCookMemoryLimiter m_memoryLimiter;
	TextureCookMemoryLimiter m_sourceMemoryLimiter;
	std::function<void(std::size_t, std::size_t)> m_progress;
	std::atomic<std::size_t> m_completedRequests{0};
	std::vector<TaskResult> m_results;
};
