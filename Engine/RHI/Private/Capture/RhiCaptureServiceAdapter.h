#pragma once

#include "Capture/RhiCaptureService.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

template <typename Implementation> class RhiCaptureServiceAdapter final : public RhiCaptureService
{
public:
	template <typename... Args> explicit RhiCaptureServiceAdapter(Args&&... args) noexcept :
	    m_implementation(std::forward<Args>(args)...)
	{
	}

	~RhiCaptureServiceAdapter() noexcept override
	{
		for (const auto& pending : m_pendingReadbacks)
		{
			m_implementation.WaitForReadback(*pending);
		}
	}

	RhiCaptureTicket BeginTextureReadback(const RhiTextureCaptureRequest& request) noexcept override
	{
		DrainCancelledReadbacks();
		auto pending = m_implementation.PrepareReadback(request);
		const bool bBufferAllocated = pending && m_implementation.AllocateReadbackBuffer(*pending);
		const bool bCommandsStarted = bBufferAllocated && m_implementation.BeginReadbackCommands(*pending);
		const bool bCopyRecorded = bCommandsStarted && m_implementation.RecordReadbackCopy(*pending);
		const bool bReadbackSubmitted = bCopyRecorded && m_implementation.SubmitReadback(*pending);
		if (!bReadbackSubmitted)
		{
			return {};
		}

		const RhiCaptureTicket ticket = pending->Ticket;
		m_pendingReadbacks.push_back(std::move(pending));
		return ticket;
	}

	bool TryTakeTextureReadback(RhiCaptureTicket ticket, RhiCaptureReadback& readback) noexcept override
	{
		DrainCancelledReadbacks();
		const auto pending = FindReadback(ticket);
		if (pending == m_pendingReadbacks.end() || !m_implementation.IsReadbackComplete(**pending))
		{
			return false;
		}

		m_implementation.ReadPixels(**pending, readback);
		m_pendingReadbacks.erase(pending);
		return true;
	}

	void CancelTextureReadback(RhiCaptureTicket ticket) noexcept override
	{
		const auto pending = FindReadback(ticket);
		if (pending == m_pendingReadbacks.end())
		{
			return;
		}
		(*pending)->bCancelled = true;
		DrainCancelledReadbacks();
	}

private:
	using PendingReadback = typename Implementation::PendingReadback;

	auto FindReadback(RhiCaptureTicket ticket) noexcept
	{
		return std::ranges::find_if(m_pendingReadbacks, [ticket](const auto& pending) { return pending->Ticket.Value == ticket.Value; });
	}

	void DrainCancelledReadbacks() noexcept
	{
		std::erase_if(m_pendingReadbacks, [this](const auto& pending) { return pending->bCancelled && m_implementation.IsReadbackComplete(*pending); });
	}

	Implementation m_implementation;
	// Only successfully submitted readbacks enter the queue; retain them until GPU completion.
	std::vector<std::unique_ptr<PendingReadback>> m_pendingReadbacks;
};
