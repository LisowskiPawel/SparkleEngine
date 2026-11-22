#pragma once

#include "Capture/RhiCaptureService.h"

#include <cstdint>
#include <memory>
#include <vector>

class VulkanRhi;
class VulkanGpuMemoryAllocator;

class VulkanCaptureService final : public RhiCaptureService
{
public:
	explicit VulkanCaptureService(VulkanRhi& rhi, VulkanGpuMemoryAllocator& memoryAllocator) noexcept;
	~VulkanCaptureService() noexcept override;

	RhiCaptureTicket BeginTextureReadback(const RhiTextureCaptureRequest& request) noexcept override;
	bool TryTakeTextureReadback(RhiCaptureTicket ticket, RhiCaptureReadback& readback) noexcept override;
	void CancelTextureReadback(RhiCaptureTicket ticket) noexcept override;

private:
	struct PendingReadback;
	std::unique_ptr<PendingReadback> PrepareReadback(const RhiTextureCaptureRequest& request) noexcept;
	bool AllocateReadbackBuffer(PendingReadback& pending) noexcept;
	bool BeginReadbackCommands(PendingReadback& pending) noexcept;
	bool RecordReadbackCopy(PendingReadback& pending) noexcept;
	bool SubmitReadback(PendingReadback& pending) noexcept;
	void ReadPixels(PendingReadback& pending, RhiCaptureReadback& readback) noexcept;
	bool IsReadbackComplete(const PendingReadback& pending) const noexcept;
	void WaitForReadback(const PendingReadback& pending) noexcept;
	void DrainCancelledReadbacks() noexcept;

	VulkanRhi& m_rhi;
	VulkanGpuMemoryAllocator& m_memoryAllocator;
	std::vector<std::unique_ptr<PendingReadback>> m_pendingReadbacks;
	std::uint64_t m_nextTicket = 1;
};
