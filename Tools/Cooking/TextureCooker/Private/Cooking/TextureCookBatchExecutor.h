#pragma once

#include "TextureCookRequestList.h"
#include "TaskTypes.h"
#include "Cooking/TextureCookMemoryLimiter.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <vector>

class CompiledTaskGraph;
class TaskNodeHandle;
struct TaskExecutorConfig;

struct TextureCookBatchItemResult final
{
	std::filesystem::path StagedOutputPath;
	TaskResult CookResult = TaskResult::Cancelled("Texture cook did not execute.");
};

class TextureCookBatchExecutor final
{
public:
	TextureCookBatchExecutor(const std::vector<TextureCookRequest>& requests, std::size_t memoryBudgetBytes, std::function<void(std::size_t, std::size_t)> progress = {});
	~TextureCookBatchExecutor();

	TextureCookBatchExecutor(const TextureCookBatchExecutor&) = delete;
	TextureCookBatchExecutor& operator=(const TextureCookBatchExecutor&) = delete;

	const std::vector<TextureCookBatchItemResult>& Execute();

private:
	TaskExecutorConfig BuildExecutorConfig() const;
	CompiledTaskGraph BuildTaskGraph(std::vector<TaskNodeHandle>& outTaskHandles);
	TaskResult CookRequest(std::uint32_t index);
	static std::uint32_t ResolveBackgroundWorkerCount();

	const std::vector<TextureCookRequest>& m_requests;
	TextureCookMemoryLimiter m_memoryLimiter;
	std::function<void(std::size_t, std::size_t)> m_progress;
	std::atomic<std::size_t> m_completedRequests{0};
	std::vector<TextureCookBatchItemResult> m_items;
};
