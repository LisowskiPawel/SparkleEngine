#pragma once

#include "Commands/RhiCommandRecordingLease.h"
#include "Vulkan/VulkanIncludes.h"

#include <array>
#include <cstdint>
#include <span>

class VulkanCommandRecordingContext;
class VulkanRhi;
class VulkanSwapChain;

// Coordinator-thread ownership of frame waits and the acquired image's binary semaphore.
// Device, swapchain, recording context and frame index outlive this owner.
class VulkanFrameSubmission final
{
public:
	VulkanFrameSubmission(VulkanRhi& rhi, VulkanSwapChain& swapChain, VulkanCommandRecordingContext& commandContext, const std::uint32_t& frameIndex) noexcept;

	VulkanFrameSubmission(const VulkanFrameSubmission&) = delete;
	VulkanFrameSubmission& operator=(const VulkanFrameSubmission&) = delete;
	VulkanFrameSubmission(VulkanFrameSubmission&&) = delete;
	VulkanFrameSubmission& operator=(VulkanFrameSubmission&&) = delete;

	void BeginFrame() noexcept;
	bool AcquireBackBuffer() noexcept;

	bool HasAcquiredBackBuffer() const noexcept { return m_hasAcquiredBackBuffer; }

	void ResetPresentation() noexcept;
	void DrainForSwapChainRecreation() noexcept;

	void QueueWait(ERhiQueueType waitQueue, RhiSubmissionToken executionToken) noexcept;
	void WaitForSubmission(RhiSubmissionToken token) noexcept;
	bool IsSubmissionComplete(RhiSubmissionToken token) const noexcept;
	RhiSubmissionToken GetLastSubmittedToken(ERhiQueueType queueType) const noexcept;
	RhiSubmissionToken Submit(RhiCommandRecordingLease&& lease, std::span<const RhiSubmissionToken> waitTokens) noexcept;
	RhiSubmissionToken SubmitBatch(std::span<RhiCommandRecordingLease> leases, std::span<const RhiSubmissionToken> waitTokens) noexcept;
	RhiSubmissionToken SubmitCurrentGraphicsCommandList(std::span<const RhiSubmissionToken> waitTokens) noexcept;
	// Returns whether presentation requested swapchain recreation.
	bool SubmitAndPresent() noexcept;

private:
	RhiSubmissionState ConsumeQueueWaits(ERhiQueueType queueType, std::span<const RhiSubmissionToken> waitTokens) noexcept;
	RhiSubmissionState ConsumePresentationWaits() noexcept;
	VkSemaphore ConsumeAcquireSemaphore(ERhiQueueType queueType) noexcept;
	bool CompletePresentation(RhiSubmissionToken frameToken, VkSemaphore renderFinishedSemaphore) noexcept;

	VulkanRhi& m_rhi;
	VulkanSwapChain& m_swapChain;
	VulkanCommandRecordingContext& m_commandContext;
	const std::uint32_t& m_currentFrameIndex;
	std::array<RhiSubmissionState, RhiQueueTypeCount> m_pendingQueueWaits;
	bool m_hasAcquiredBackBuffer = false;
	bool m_hasConsumedAcquireSemaphore = false;
};
