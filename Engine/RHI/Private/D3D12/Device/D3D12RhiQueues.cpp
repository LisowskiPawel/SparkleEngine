#include "PCH.h"

#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/Commands/D3D12CommandQueue.h"

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_d3d12RhiLogger, "RHI.D3D12");

void D3D12Rhi::CreateCommandQueues()
{
	ComPtr<ID3D12CommandQueue> nativeGraphicsQueue = CreateInterposerGraphicsQueue();

	for (std::size_t queueIndex = 0; queueIndex < RhiQueueTypeCount; ++queueIndex)
	{
		const ERhiQueueType queueType = static_cast<ERhiQueueType>(queueIndex);
		ComPtr<ID3D12CommandQueue> nativeQueue = queueType == ERhiQueueType::Graphics ? std::move(nativeGraphicsQueue) : ComPtr<ID3D12CommandQueue>{};
		m_queues[queueIndex] = std::make_unique<D3D12CommandQueue>(*m_device.Get(), queueType, std::move(nativeQueue));
	}
}

const ComPtr<ID3D12CommandQueue>& D3D12Rhi::GetCommandQueue() const noexcept
{
	return GetCommandQueue(ERhiQueueType::Graphics);
}

const ComPtr<ID3D12CommandQueue>& D3D12Rhi::GetCommandQueue(ERhiQueueType queueType) const noexcept
{
	return m_queues[RhiQueueTypeToIndex(queueType)]->GetNativeQueue();
}

ID3D12CommandQueue* D3D12Rhi::GetPresentationCommandQueue() const noexcept
{
	return m_externalCommandQueue != nullptr ? m_externalCommandQueue.Get() : GetCommandQueue().Get();
}

const ComPtr<ID3D12Fence1>& D3D12Rhi::GetFence() const noexcept
{
	return m_queues[RhiQueueTypeToIndex(ERhiQueueType::Graphics)]->GetFence();
}

HANDLE D3D12Rhi::GetFenceEvent() const noexcept
{
	return m_queues[RhiQueueTypeToIndex(ERhiQueueType::Graphics)]->GetFenceEvent();
}

RhiSubmissionToken D3D12Rhi::SubmitCommandLists(ERhiQueueType queueType, std::span<ID3D12CommandList* const> commandLists, std::span<const RhiSubmissionToken> waitTokens) noexcept
{
	if (!IsRhiQueueTypeValid(queueType))
	{
		Diagnostics::Fatal(g_d3d12RhiLogger, __FILE__, __LINE__, "Command submission rejected an invalid queue type");
		return {};
	}

	RhiSubmissionState waitState;
	for (const RhiSubmissionToken token : waitTokens)
	{
		if (token.Queue != queueType)
		{
			waitState.MarkUsed(token);
		}
	}

	std::array<D3D12QueueWait, RhiQueueTypeCount> waits{};
	std::size_t waitCount = 0;
	for (std::size_t waitQueueIndex = 0; waitQueueIndex < RhiQueueTypeCount; ++waitQueueIndex)
	{
		const ERhiQueueType waitQueue = static_cast<ERhiQueueType>(waitQueueIndex);
		const RhiSubmissionToken token = waitState.GetToken(waitQueue);
		if (token.IsValid())
		{
			waits[waitCount++] = D3D12QueueWait{.ProducerQueue = m_queues[waitQueueIndex].get(), .SubmissionValue = token.Value};
		}
	}
	return m_queues[RhiQueueTypeToIndex(queueType)]->Submit(commandLists, std::span<const D3D12QueueWait>(waits.data(), waitCount));
}

RhiSubmissionToken D3D12Rhi::SignalQueue(ERhiQueueType queueType) noexcept
{
	if (!IsRhiQueueTypeValid(queueType))
	{
		Diagnostics::Fatal(g_d3d12RhiLogger, __FILE__, __LINE__, "Queue signal rejected an invalid queue type");
		return {};
	}

	return m_queues[RhiQueueTypeToIndex(queueType)]->Signal();
}

void D3D12Rhi::QueueWait(ERhiQueueType waitQueue, RhiSubmissionToken executionToken) noexcept
{
	if (!IsRhiQueueTypeValid(waitQueue) || !executionToken.IsValid() || waitQueue == executionToken.Queue)
	{
		return;
	}

	m_queues[RhiQueueTypeToIndex(waitQueue)]->WaitFor(*m_queues[RhiQueueTypeToIndex(executionToken.Queue)], executionToken.Value);
}

void D3D12Rhi::WaitForSubmission(RhiSubmissionToken token) noexcept
{
	if (!token.IsValid())
	{
		return;
	}

	m_queues[RhiQueueTypeToIndex(token.Queue)]->WaitForSubmission(token.Value);
}

bool D3D12Rhi::IsSubmissionComplete(RhiSubmissionToken token) const noexcept
{
	if (!token.IsValid())
	{
		return true;
	}

	return m_queues[RhiQueueTypeToIndex(token.Queue)]->IsSubmissionComplete(token.Value);
}

RhiSubmissionToken D3D12Rhi::GetLastSubmittedToken(ERhiQueueType queueType) const noexcept
{
	if (!IsRhiQueueTypeValid(queueType))
	{
		return {};
	}
	return m_queues[RhiQueueTypeToIndex(queueType)]->GetLastSubmittedToken();
}

std::uint64_t D3D12Rhi::GetCompletedSubmissionValue(ERhiQueueType queueType) const noexcept
{
	if (!IsRhiQueueTypeValid(queueType))
	{
		return 0;
	}
	return m_queues[RhiQueueTypeToIndex(queueType)]->GetCompletedSubmissionValue();
}

void D3D12Rhi::WaitForIdle() noexcept
{
	for (std::size_t queueIndex = 0; queueIndex < RhiQueueTypeCount; ++queueIndex)
	{
		m_queues[queueIndex]->WaitForIdle();
	}
}
