#pragma once

#include "TextureCookRequestList.h"
#include <exception>
#include "Cooking/TextureCookMemoryLimiter.h"
#include "Core/Public/Files/FileSetPublication.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

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

	const std::vector<std::exception_ptr>& Execute();

private:
	static void CookRange(std::uint32_t begin, std::uint32_t end, std::uint32_t thread, void* argument) noexcept;
	void CookRequest(std::uint32_t index);
	static std::uint32_t ResolveBackgroundWorkerCount();

	const std::vector<TextureCookRequest>& m_requests;
	std::span<const Files::FilePublication> m_publication;
	TextureCookMemoryLimiter m_memoryLimiter;
	TextureCookMemoryLimiter m_sourceMemoryLimiter;
	std::function<void(std::size_t, std::size_t)> m_progress;
	std::atomic<std::size_t> m_completedRequests{0};
	std::vector<std::exception_ptr> m_results;
};
