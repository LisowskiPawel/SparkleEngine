#pragma once

#include "Core/RhiCapabilities.h"
#include "D3D12/Memory/D3D12RecordingResourceUseToken.h"
#include "Resources/RhiResourceService.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

class D3D12GpuMemoryAllocator;
class D3D12RenderHardwareInterface;
class D3D12Rhi;
struct D3D12GpuAllocationRecord;
struct D3D12GpuHeapRecord;

class D3D12ResourceService final : public RhiResourceService
{
public:
	D3D12ResourceService(D3D12Rhi& rhi, D3D12GpuMemoryAllocator& memoryAllocator, const RhiCapabilities& capabilities) noexcept;

	RhiOwnedResourceHandle CreateTextureResource(
	    const RhiTextureResourceDesc& desc,
	    ResourceState initialState,
	    RhiMemoryCategory category,
	    RhiMemoryResidencyClass residencyClass,
	    std::wstring_view debugName) override;
	RhiOwnedResourceHandle CreateBufferResource(
	    const RhiBufferResourceDesc& desc,
	    ResourceState initialState,
	    RhiMemoryCategory category,
	    RhiMemoryResidencyClass residencyClass,
	    std::wstring_view debugName) override;
	bool WriteBufferResource(RhiOwnedResourceHandle resource, std::size_t destinationOffsetInBytes, const void* data, std::size_t sizeInBytes) noexcept override;
	void ReleaseOwnedResource(RhiOwnedResourceHandle resource) noexcept override;
	RhiResourceHandle GetResourceHandle(RhiOwnedResourceHandle resource) const noexcept override;
	RhiGpuVirtualAddress GetResourceGpuVirtualAddress(RhiOwnedResourceHandle resource) const noexcept override;
	RhiResourceAllocationInfo GetTextureAllocationInfo(const RhiTextureResourceDesc& desc) const noexcept override;
	RhiResourceAllocationInfo GetBufferAllocationInfo(const RhiBufferResourceDesc& desc) const noexcept override;
	RhiOwnedMemoryBlockHandle CreateTransientMemoryBlock(RhiTransientAllocationPool pool, std::uint64_t sizeInBytes, std::uint64_t alignment, std::wstring_view debugName) override;
	void ReleaseTransientMemoryBlock(RhiOwnedMemoryBlockHandle memoryBlock) noexcept override;
	RhiOwnedResourceHandle CreateAliasingTextureResource(
	    RhiOwnedMemoryBlockHandle memoryBlock,
	    std::uint64_t memoryBlockOffset,
	    const RhiTransientTextureAllocationDesc& desc,
	    std::wstring_view debugName) override;
	RhiOwnedResourceHandle CreateAliasingBufferResource(
	    RhiOwnedMemoryBlockHandle memoryBlock,
	    std::uint64_t memoryBlockOffset,
	    const RhiTransientBufferAllocationDesc& desc,
	    std::wstring_view debugName) override;
	bool SupportsUnorderedAccess(RhiResourceHandle resource) const noexcept override;

private:
	friend class D3D12RenderHardwareInterface;
	friend class D3D12BackBufferPresentation;
	void DrainCompletedResourceReleases() noexcept;
	void FlushDeferredResourceReleases() noexcept;
	D3D12RecordingResourceUseToken BeginResourceTracking(RhiResourceHandle resource, bool bCoordinatorRecording) noexcept;
	void EndResourceTracking(D3D12RecordingResourceUseToken use, RhiSubmissionToken submissionToken) noexcept;

	void CollectCrashDiagnosticsOnce() noexcept;

	D3D12Rhi& m_rhi;
	D3D12GpuMemoryAllocator& m_memoryAllocator;
	const RhiCapabilities& m_capabilities;
	std::vector<std::unique_ptr<D3D12GpuAllocationRecord>> m_pendingOwnedResourceReleases;
	std::vector<std::unique_ptr<D3D12GpuHeapRecord>> m_pendingOwnedMemoryBlockReleases;
	bool m_bCrashDiagnosticsCollected = false;
};
