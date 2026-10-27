#include "PCH.h"

#include "D3D12/Capture/D3D12CaptureService.h"
#include "D3D12/Capture/D3D12CaptureReadback.h"

#include <algorithm>
#include <utility>

D3D12CaptureService::D3D12CaptureService(D3D12Rhi& rhi) noexcept :
    m_rhi(rhi)
{
}

D3D12CaptureService::~D3D12CaptureService() noexcept
{
	for (const std::unique_ptr<PendingReadback>& pending : m_pendingReadbacks)
	{
		if (pending && pending->Submission.IsValid())
		{
			WaitForReadback(*pending);
		}
	}
	m_pendingReadbacks.clear();
}

RhiCaptureTicket D3D12CaptureService::BeginTextureReadback(const RhiTextureCaptureRequest& request) noexcept
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

bool D3D12CaptureService::TryTakeTextureReadback(RhiCaptureTicket ticket, RhiCaptureReadback& readback) noexcept
{
	DrainCancelledReadbacks();

	const auto pendingIterator = std::find_if(
	    m_pendingReadbacks.begin(),
	    m_pendingReadbacks.end(),
	    [ticket](const std::unique_ptr<PendingReadback>& candidate) { return candidate && candidate->Ticket.Value == ticket.Value; });

	if (pendingIterator == m_pendingReadbacks.end() || !IsReadbackComplete(**pendingIterator))
	{
		return false;
	}
	PendingReadback& pending = **pendingIterator;

	ReadPixels(pending, readback);
	m_pendingReadbacks.erase(pendingIterator);
	return true;
}

void D3D12CaptureService::CancelTextureReadback(RhiCaptureTicket ticket) noexcept
{
	const auto iterator = std::find_if(
	    m_pendingReadbacks.begin(),
	    m_pendingReadbacks.end(),
	    [ticket](const std::unique_ptr<PendingReadback>& candidate) { return candidate && candidate->Ticket.Value == ticket.Value; });

	if (iterator == m_pendingReadbacks.end())
	{
		return;
	}
	(*iterator)->Cancelled = true;
	DrainCancelledReadbacks();
}

void D3D12CaptureService::DrainCancelledReadbacks() noexcept
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
