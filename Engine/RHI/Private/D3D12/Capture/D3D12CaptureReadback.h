#pragma once

#include "D3D12/Capture/D3D12CaptureService.h"
#include "Commands/RhiQueue.h"
#include "D3D12/Memory/D3D12GpuAllocation.h"
#include <d3d12.h>
#include <wrl/client.h>

struct D3D12CaptureService::PendingReadback final
{
	RhiCaptureTicket Ticket;
	RhiTextureCaptureRequest Request;
	RhiSubmissionToken Submission;
	std::unique_ptr<D3D12GpuAllocationRecord> Buffer;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint{};
	std::uint64_t TotalBytes = 0;
	bool bCancelled = false;
};
