#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Commands/VulkanFrameSubmission.h"

#include "Vulkan/Commands/VulkanCommandRecordingContext.h"
#include "Vulkan/Commands/VulkanCommandQueue.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/SwapChain/VulkanSwapChain.h"

#include <utility>

VulkanFrameSubmission::VulkanFrameSubmission(VulkanRhi& rhi, VulkanSwapChain& swapChain, VulkanCommandRecordingContext& commandContext, const std::uint32_t& frameIndex) noexcept :
    m_rhi(rhi),
    m_swapChain(swapChain),
    m_commandContext(commandContext),
    m_currentFrameIndex(frameIndex)
{
}

void VulkanFrameSubmission::BeginFrame() noexcept
{
	m_hasConsumedAcquireSemaphore = false;
	for (RhiSubmissionState& waits : m_pendingQueueWaits)
	{
		waits.Clear();
	}
}

bool VulkanFrameSubmission::AcquireBackBuffer() noexcept
{
	m_hasAcquiredBackBuffer = m_swapChain.AcquireNextImage(m_currentFrameIndex);
	return m_hasAcquiredBackBuffer;
}

void VulkanFrameSubmission::ResetPresentation() noexcept
{
	m_hasAcquiredBackBuffer = false;
	m_hasConsumedAcquireSemaphore = false;
}

RhiSubmissionToken VulkanFrameSubmission::Submit(RhiCommandRecordingLease&& lease, std::span<const RhiSubmissionToken> waitTokens) noexcept
{
	const ERhiQueueType queueType = lease.GetQueueType();
	const RhiSubmissionState resolvedWaits = ConsumeQueueWaits(queueType, waitTokens);
	std::array<RhiSubmissionToken, RhiQueueTypeCount> resolvedWaitTokens{};
	const std::size_t resolvedWaitCount = resolvedWaits.CopyTokens(resolvedWaitTokens);

	return m_commandContext
	    .Submit(std::move(lease), std::span<const RhiSubmissionToken>(resolvedWaitTokens.data(), resolvedWaitCount), ConsumeAcquireSemaphore(queueType), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
}

RhiSubmissionToken VulkanFrameSubmission::SubmitBatch(std::span<RhiCommandRecordingLease> leases, std::span<const RhiSubmissionToken> waitTokens) noexcept
{
	if (leases.empty())
	{
		return {};
	}

	const ERhiQueueType queueType = leases.front().GetQueueType();
	const RhiSubmissionState resolvedWaits = ConsumeQueueWaits(queueType, waitTokens);
	std::array<RhiSubmissionToken, RhiQueueTypeCount> resolvedWaitTokens{};
	const std::size_t resolvedWaitCount = resolvedWaits.CopyTokens(resolvedWaitTokens);

	return m_commandContext
	    .SubmitBatch(leases, std::span<const RhiSubmissionToken>(resolvedWaitTokens.data(), resolvedWaitCount), ConsumeAcquireSemaphore(queueType), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
}

RhiSubmissionState VulkanFrameSubmission::ConsumeQueueWaits(ERhiQueueType queueType, std::span<const RhiSubmissionToken> waitTokens) noexcept
{
	const std::size_t queueIndex = RhiQueueTypeToIndex(queueType);
	RhiSubmissionState resolvedWaits = m_pendingQueueWaits[queueIndex];
	for (const RhiSubmissionToken waitToken : waitTokens)
	{
		resolvedWaits.MarkUsed(waitToken);
	}

	m_pendingQueueWaits[queueIndex].Clear();
	return resolvedWaits;
}

VkSemaphore VulkanFrameSubmission::ConsumeAcquireSemaphore(ERhiQueueType queueType) noexcept
{
	if (queueType == ERhiQueueType::Graphics && m_hasAcquiredBackBuffer && !m_hasConsumedAcquireSemaphore)
	{
		m_hasConsumedAcquireSemaphore = true;
		return m_swapChain.GetImageAvailableSemaphore(m_currentFrameIndex);
	}

	return VK_NULL_HANDLE;
}

void VulkanFrameSubmission::QueueWait(ERhiQueueType waitQueue, RhiSubmissionToken executionToken) noexcept
{
	if (!IsRhiQueueTypeValid(waitQueue) || !executionToken.IsValid() || waitQueue == executionToken.Queue)
	{
		return;
	}
	m_pendingQueueWaits[RhiQueueTypeToIndex(waitQueue)].MarkUsed(executionToken);
}

void VulkanFrameSubmission::WaitForSubmission(RhiSubmissionToken token) noexcept
{
	if (token.IsValid())
	{
		m_rhi.GetCommandQueue(token.Queue).WaitForSubmission(token.Value);
	}
}

bool VulkanFrameSubmission::IsSubmissionComplete(RhiSubmissionToken token) const noexcept
{
	return !token.IsValid() || m_rhi.GetCommandQueue(token.Queue).IsSubmissionComplete(token.Value);
}

RhiSubmissionToken VulkanFrameSubmission::GetLastSubmittedToken(ERhiQueueType queueType) const noexcept
{
	return IsRhiQueueTypeValid(queueType) ? m_rhi.GetCommandQueue(queueType).GetLastSubmittedToken() : RhiSubmissionToken{};
}

void VulkanFrameSubmission::DrainForSwapChainRecreation() noexcept
{
	VulkanCommandQueue& graphicsQueue = m_rhi.GetCommandQueue(ERhiQueueType::Graphics);
	const RhiSubmissionToken lastSubmission = graphicsQueue.GetLastSubmittedToken();
	if (lastSubmission.IsValid())
	{
		graphicsQueue.WaitForSubmission(lastSubmission.Value);
	}

	graphicsQueue.DrainForSwapChainRecreation();
}

RhiSubmissionState VulkanFrameSubmission::ConsumePresentationWaits() noexcept
{
	RhiSubmissionState waits = m_pendingQueueWaits[RhiQueueTypeToIndex(ERhiQueueType::Graphics)];
	m_pendingQueueWaits[RhiQueueTypeToIndex(ERhiQueueType::Graphics)].Clear();
	for (const ERhiQueueType queueType : {ERhiQueueType::Compute, ERhiQueueType::Copy})
	{
		waits.MarkUsed(m_rhi.GetCommandQueue(queueType).GetLastSubmittedToken());
	}

	return waits;
}

RhiSubmissionToken VulkanFrameSubmission::SubmitCurrentGraphicsCommandList(std::span<const RhiSubmissionToken> waitTokens) noexcept
{
	const RhiSubmissionState resolvedWaits = ConsumeQueueWaits(ERhiQueueType::Graphics, waitTokens);
	std::array<RhiSubmissionToken, RhiQueueTypeCount> resolvedWaitTokens{};
	const std::size_t resolvedWaitCount = resolvedWaits.CopyTokens(resolvedWaitTokens);

	return m_commandContext.SubmitCurrentGraphicsCommandList(
	    m_currentFrameIndex,
	    std::span<const RhiSubmissionToken>(resolvedWaitTokens.data(), resolvedWaitCount),
	    ConsumeAcquireSemaphore(ERhiQueueType::Graphics));
}

bool VulkanFrameSubmission::SubmitAndPresent() noexcept
{
	const VkSemaphore renderFinishedSemaphore = m_swapChain.GetCurrentRenderFinishedSemaphore();
	const RhiSubmissionState waits = ConsumePresentationWaits();
	std::array<RhiSubmissionToken, RhiQueueTypeCount> waitTokens{};
	const std::size_t waitTokenCount = waits.CopyTokens(waitTokens);
	const VkSemaphore acquireSemaphore = ConsumeAcquireSemaphore(ERhiQueueType::Graphics);

	const RhiSubmissionToken frameToken = m_commandContext.SubmitCurrentGraphicsCommandList(
	    m_currentFrameIndex,
	    std::span<const RhiSubmissionToken>(waitTokens.data(), waitTokenCount),
	    acquireSemaphore,
	    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	    renderFinishedSemaphore);

	return CompletePresentation(frameToken, renderFinishedSemaphore);
}

bool VulkanFrameSubmission::CompletePresentation(RhiSubmissionToken frameToken, VkSemaphore renderFinishedSemaphore) noexcept
{
	if (!frameToken.IsValid())
	{
		m_hasAcquiredBackBuffer = false;
		return false;
	}

	const bool resizeRequired = m_swapChain.Present(renderFinishedSemaphore);
	if (resizeRequired)
	{
		(void) m_swapChain.ConsumeResizeRequest();
	}
	ResetPresentation();
	return resizeRequired;
}
