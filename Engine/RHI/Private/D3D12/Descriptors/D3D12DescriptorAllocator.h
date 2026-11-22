#pragma once

#include "D3D12DescriptorHeap.h"

#include <cstdint>
#include <D3D12MemAlloc.h>
#include <mutex>
#include <unordered_map>
#include <wrl/client.h>

class D3D12DescriptorAllocator final
{
public:
	explicit D3D12DescriptorAllocator(D3D12DescriptorHeap* heap) noexcept;
	~D3D12DescriptorAllocator() noexcept;

	D3D12DescriptorHandle Allocate();

	D3D12DescriptorHandle AllocateContiguous(uint32_t count);

	void Free(const D3D12DescriptorHandle& handle) noexcept;

	void FreeContiguous(const D3D12DescriptorHandle& firstHandle, uint32_t count) noexcept;

private:
	D3D12DescriptorHeap* m_heap;
	Microsoft::WRL::ComPtr<D3D12MA::VirtualBlock> m_block;
	std::unordered_map<UINT, D3D12MA::VirtualAllocation> m_allocations;
	mutable std::mutex m_mutex;
};
