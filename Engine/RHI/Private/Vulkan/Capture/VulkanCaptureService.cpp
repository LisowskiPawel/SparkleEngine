#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Capture/VulkanCaptureService.h"
#include "Vulkan/Capture/VulkanCaptureReadback.h"

#include <algorithm>
#include <utility>

VulkanCaptureService::VulkanCaptureService(VulkanRhi& rhi) noexcept :
    m_rhi(rhi)
{
}

VulkanCaptureService::~VulkanCaptureService() noexcept
{
	for (const std::unique_ptr<PendingReadback>& pending : m_pendingReadbacks)
	{
		if (pending && pending->Submission.IsValid())
		{
			WaitForReadback(*pending);
		}
	}
	while (!m_pendingReadbacks.empty())
	{
		m_pendingReadbacks.pop_back();
	}
}

RhiCaptureTicket VulkanCaptureService::BeginTextureReadback(const RhiTextureCaptureRequest& request) noexcept
{
	DrainCancelledReadbacks();
	auto pending = PrepareReadback(request);
	if (!pending || !AllocateReadbackBuffer(*pending) || !BeginReadbackCommands(*pending) || !RecordReadbackCopy(*pending) || !SubmitReadback(*pending))
	{
		return {};
	}

	const RhiCaptureTicket ticket = pending->Ticket;
	m_pendingReadbacks.push_back(std::move(pending));
	return ticket;
}

bool VulkanCaptureService::TryTakeTextureReadback(RhiCaptureTicket ticket, RhiCaptureReadback& readback) noexcept
{
	DrainCancelledReadbacks();
	std::size_t pendingIndex = m_pendingReadbacks.size();
	for (std::size_t index = 0; index < m_pendingReadbacks.size(); ++index)
	{
		if (m_pendingReadbacks[index] && m_pendingReadbacks[index]->Ticket.Value == ticket.Value)
		{
			pendingIndex = index;
			break;
		}
	}
	if (pendingIndex == m_pendingReadbacks.size())
	{
		return false;
	}
	PendingReadback& pending = *m_pendingReadbacks[pendingIndex];
	if (!IsReadbackComplete(pending))
	{
		return false;
	}

	ReadPixels(pending, readback);
	m_pendingReadbacks.erase(m_pendingReadbacks.begin() + pendingIndex);
	return true;
}

void VulkanCaptureService::CancelTextureReadback(RhiCaptureTicket ticket) noexcept
{
	for (std::size_t index = 0; index < m_pendingReadbacks.size(); ++index)
	{
		const std::unique_ptr<PendingReadback>& pending = m_pendingReadbacks[index];
		if (!pending || pending->Ticket.Value != ticket.Value)
		{
			continue;
		}
		pending->Cancelled = true;
		DrainCancelledReadbacks();
		return;
	}
}

void VulkanCaptureService::DrainCancelledReadbacks() noexcept
{
	for (std::size_t index = 0; index < m_pendingReadbacks.size();)
	{
		const std::unique_ptr<PendingReadback>& pending = m_pendingReadbacks[index];
		if (!pending || !pending->Cancelled || !IsReadbackComplete(*pending))
		{
			++index;
			continue;
		}
		m_pendingReadbacks.erase(m_pendingReadbacks.begin() + index);
	}
}
