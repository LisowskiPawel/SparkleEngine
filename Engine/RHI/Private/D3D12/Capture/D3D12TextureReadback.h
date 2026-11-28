#pragma once

#include "Capture/RhiCaptureService.h"
#include "Commands/RhiQueue.h"
#include "D3D12/Memory/D3D12GpuAllocation.h"
#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>

class D3D12Rhi;
class D3D12GpuMemoryAllocator;
class D3D12ResourceService;

template <typename Implementation> class RhiCaptureServiceAdapter;

class D3D12TextureReadback final
{
public:
	explicit D3D12TextureReadback(D3D12Rhi& rhi, D3D12GpuMemoryAllocator& memoryAllocator, D3D12ResourceService& resourceService) noexcept;
	~D3D12TextureReadback() noexcept;

private:
	template <typename Implementation> friend class RhiCaptureServiceAdapter;
	struct PendingReadback;

	std::unique_ptr<PendingReadback> PrepareReadback(const RhiTextureCaptureRequest& request) noexcept;
	bool AllocateReadbackBuffer(PendingReadback& pending) noexcept;
	bool BeginReadbackCommands(PendingReadback& pending) noexcept;
	bool RecordReadbackCopy(PendingReadback& pending) noexcept;
	bool SubmitReadback(PendingReadback& pending) noexcept;
	void ReadPixels(PendingReadback& pending, RhiCaptureReadback& readback) noexcept;
	bool IsReadbackComplete(const PendingReadback& pending) const noexcept;
	void WaitForReadback(const PendingReadback& pending) noexcept;

	D3D12Rhi& m_rhi;
	D3D12GpuMemoryAllocator& m_memoryAllocator;
	D3D12ResourceService& m_resourceService;
	std::uint64_t m_nextTicket = 1;
};

struct D3D12TextureReadback::PendingReadback final
{
	~PendingReadback() noexcept;

	D3D12ResourceService* ResourceService = nullptr;
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
