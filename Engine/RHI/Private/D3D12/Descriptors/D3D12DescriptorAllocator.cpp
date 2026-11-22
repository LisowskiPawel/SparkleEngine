#include "PCH.h"
#include "D3D12/Descriptors/D3D12DescriptorAllocator.h"

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogD3D12DescriptorAllocator, "RHI.D3D12.Descriptors");

D3D12DescriptorAllocator::D3D12DescriptorAllocator(D3D12DescriptorHeap* heap) noexcept :
    m_heap(heap)
{
	D3D12MA::VIRTUAL_BLOCK_DESC blockDesc{};
	blockDesc.Size = m_heap->GetNumDescriptors();
	CHECK(D3D12MA::CreateVirtualBlock(&blockDesc, m_block.ReleaseAndGetAddressOf()));
}

D3D12DescriptorAllocator::~D3D12DescriptorAllocator() noexcept
{
	m_block->Clear();
}

D3D12DescriptorHandle D3D12DescriptorAllocator::Allocate()
{
	return AllocateContiguous(1);
}

D3D12DescriptorHandle D3D12DescriptorAllocator::AllocateContiguous(uint32_t count)
{
	if (count == 0)
	{
		return {};
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	D3D12MA::VIRTUAL_ALLOCATION_DESC allocationDesc{};
	allocationDesc.Size = count;
	D3D12MA::VirtualAllocation allocation{};
	UINT64 offset = 0;
	const HRESULT result = m_block->Allocate(&allocationDesc, &allocation, &offset);
	if (FAILED(result))
	{
		Diagnostics::Fatal(LogD3D12DescriptorAllocator, __FILE__, __LINE__, "Descriptor heap cannot allocate the requested contiguous range.");
	}

	const UINT index = static_cast<UINT>(offset);
	m_allocations.emplace(index, allocation);
	return m_heap->GetHandleAt(index);
}

void D3D12DescriptorAllocator::Free(const D3D12DescriptorHandle& handle) noexcept
{
	FreeContiguous(handle, 1);
}

void D3D12DescriptorAllocator::FreeContiguous(const D3D12DescriptorHandle& firstHandle, uint32_t count) noexcept
{
	if (!firstHandle.IsValid() || count == 0)
	{
		return;
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	const auto allocation = m_allocations.find(firstHandle.GetIndex());
	if (allocation == m_allocations.end())
	{
		Diagnostics::Fatal(LogD3D12DescriptorAllocator, __FILE__, __LINE__, "Descriptor range is not allocated.");
	}

	D3D12MA::VIRTUAL_ALLOCATION_INFO allocationInfo{};
	m_block->GetAllocationInfo(allocation->second, &allocationInfo);
	if (allocationInfo.Size != count)
	{
		Diagnostics::Fatal(LogD3D12DescriptorAllocator, __FILE__, __LINE__, "Descriptor release must cover the complete allocated range.");
	}
	m_block->FreeAllocation(allocation->second);
	m_allocations.erase(allocation);
}
