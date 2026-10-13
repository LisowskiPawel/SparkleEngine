#include "PCH.h"

#include "D3D12/Commands/D3D12RecordingUploadPage.h"

#include "Core/Public/Math/MathUtils.h"
#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/Memory/D3D12GpuAllocation.h"
#include "D3D12/Memory/D3D12GpuMemoryAllocator.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <stdexcept>

struct D3D12RecordingUploadPage::Page final
{
	std::unique_ptr<D3D12GpuAllocationRecord> Allocation;
	RhiGpuVirtualAddress GpuBase = 0;
	std::uint64_t CapacityInBytes = 0;
};

D3D12RecordingUploadPage::D3D12RecordingUploadPage() noexcept = default;

D3D12RecordingUploadPage::~D3D12RecordingUploadPage() noexcept = default;

void D3D12RecordingUploadPage::Initialize(D3D12Rhi& rhi, std::uint64_t capacityInBytes, const wchar_t* debugName)
{
	assert(capacityInBytes != 0);
	assert(m_pages.empty());
	m_memoryAllocator = &rhi.GetMemoryAllocator();
	m_capacityInBytes = capacityInBytes;
	AddPage(capacityInBytes, debugName);
}

void D3D12RecordingUploadPage::AddPage(std::uint64_t capacityInBytes, const wchar_t* debugName)
{
	auto page = std::make_unique<Page>();
	const D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(capacityInBytes);
	page->Allocation = m_memoryAllocator->CreateBuffer(resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, RhiMemoryCategory::ConstantBuffer, RhiMemoryResidencyClass::HostUpload, debugName);
	if (page->Allocation == nullptr || page->Allocation->Resource == nullptr)
	{
		throw std::runtime_error("Failed to allocate a D3D12 command-recording upload page.");
	}

	D3D12_RANGE readRange{0, 0};
	const HRESULT mapResult = page->Allocation->Resource->Map(0, &readRange, &page->Allocation->CpuMappedAddress);
	if (FAILED(mapResult))
	{
		throw std::runtime_error("Failed to map a D3D12 command-recording upload page.");
	}

	page->Allocation->IsMapped = true;
	page->GpuBase = page->Allocation->Resource->GetGPUVirtualAddress();
	page->CapacityInBytes = capacityInBytes;
	m_pages.push_back(std::move(page));
}

void D3D12RecordingUploadPage::BeginRecording() noexcept
{
	const std::thread::id thread = std::this_thread::get_id();
	assert(m_recordingThread == std::thread::id{} || m_recordingThread == thread);
	m_recordingThread = thread;
}

void D3D12RecordingUploadPage::EndRecording() noexcept
{
	assert(m_recordingThread == std::this_thread::get_id());
	m_recordingThread = {};
}

void D3D12RecordingUploadPage::Reset() noexcept
{
	m_recordingThread = {};
	m_pageIndex = 0;
	m_offset = 0;
}

RhiGpuVirtualAddress D3D12RecordingUploadPage::AllocateAndCopy(const void* data, std::uint32_t sizeInBytes)
{
	assert(data != nullptr);
	assert(sizeInBytes != 0);
	assert(m_recordingThread == std::this_thread::get_id());

	std::uint64_t alignedOffset = MathUtils::AlignUp(m_offset, 256ull);
	const std::uint64_t alignedSize = MathUtils::AlignUp(static_cast<std::uint64_t>(sizeInBytes), 256ull);
	if (alignedOffset > m_pages[m_pageIndex]->CapacityInBytes || alignedSize > m_pages[m_pageIndex]->CapacityInBytes - alignedOffset)
	{
		// Earlier pages stay mapped and owned until the recording slot retires.
		do
		{
			++m_pageIndex;
		} while (m_pageIndex < m_pages.size() && m_pages[m_pageIndex]->CapacityInBytes < alignedSize);
		if (m_pageIndex == m_pages.size())
		{
			AddPage((std::max) (m_capacityInBytes, alignedSize), m_pages.front()->Allocation->DebugName.c_str());
		}
		alignedOffset = 0;
	}

	Page& page = *m_pages[m_pageIndex];
	std::memcpy(static_cast<std::uint8_t*>(page.Allocation->CpuMappedAddress) + alignedOffset, data, sizeInBytes);
	m_offset = alignedOffset + alignedSize;
	return page.GpuBase + alignedOffset;
}
