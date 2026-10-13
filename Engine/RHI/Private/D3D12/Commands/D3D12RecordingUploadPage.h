#pragma once

#include "Resources/RhiResourceDesc.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <thread>
#include <vector>

class D3D12Rhi;
class D3D12GpuMemoryAllocator;

class D3D12RecordingUploadPage final
{
public:
	D3D12RecordingUploadPage() noexcept;
	~D3D12RecordingUploadPage() noexcept;

	D3D12RecordingUploadPage(const D3D12RecordingUploadPage&) = delete;
	D3D12RecordingUploadPage& operator=(const D3D12RecordingUploadPage&) = delete;
	D3D12RecordingUploadPage(D3D12RecordingUploadPage&&) = delete;
	D3D12RecordingUploadPage& operator=(D3D12RecordingUploadPage&&) = delete;

	void Initialize(D3D12Rhi& rhi, std::uint64_t capacityInBytes, const wchar_t* debugName);
	void BeginRecording() noexcept;
	void EndRecording() noexcept;
	void Reset() noexcept;
	RhiGpuVirtualAddress AllocateAndCopy(const void* data, std::uint32_t sizeInBytes);

private:
	struct Page;
	void AddPage(std::uint64_t capacityInBytes, const wchar_t* debugName);

	std::vector<std::unique_ptr<Page>> m_pages;
	D3D12GpuMemoryAllocator* m_memoryAllocator = nullptr;
	std::size_t m_pageIndex = 0;
	std::thread::id m_recordingThread;
	std::uint64_t m_capacityInBytes = 0;
	std::uint64_t m_offset = 0;
};
