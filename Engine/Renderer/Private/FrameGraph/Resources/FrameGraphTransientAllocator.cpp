#include "PCH.h"
#include "Renderer/Private/FrameGraph/Resources/FrameGraphTransientAllocator.h"

#include "RHI/Public/Device/RenderHardwareInterface.h"

#include <algorithm>
#include <cassert>
#include <string>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogFrameGraphTransientAllocator, "Renderer.FrameGraph");

FrameGraphTransientAllocator::FrameGraphTransientAllocator(RenderHardwareInterface& renderHardwareInterface) noexcept :
    m_renderHardwareInterface(&renderHardwareInterface)
{
}

void FrameGraphTransientAllocator::Reset() noexcept
{
	ReleaseAllocations();
	if (m_renderHardwareInterface != nullptr)
	{
		for (MemoryBlockRecord& block : m_memoryBlocks)
		{
			m_renderHardwareInterface->GetResourceService().ReleaseTransientMemoryBlock(block.memoryBlock);
		}
	}
	m_allocations.clear();
	m_memoryBlocks.clear();
	m_planEntries.clear();
}

bool FrameGraphTransientAllocator::Prepare(const FrameGraphTransientPlan& plan) noexcept
{
	std::vector<PlanEntry> planEntries;
	planEntries.reserve(plan.resources.size());
	for (const FrameGraphTransientResourcePlan& resource : plan.resources)
	{
		planEntries.push_back(
		    PlanEntry{
		        .handle = resource.handle,
		        .physicalBlockIndex = resource.physicalAllocation.physicalBlockIndex,
		        .textureDesc = resource.physicalAllocation.textureResourceDesc,
		        .bufferDesc = resource.physicalAllocation.bufferResourceDesc,
		        .bRequiresShaderResourceView = RequiresShaderResourceView(resource),
		        .bRequiresUnorderedAccessView = RequiresUnorderedAccessView(resource)});
	}

	if (planEntries == m_planEntries)
	{
		return false;
	}

	Reset();
	m_planEntries = std::move(planEntries);
	return true;
}

void FrameGraphTransientAllocator::ReleaseAllocations() noexcept
{
	if (m_renderHardwareInterface == nullptr)
	{
		return;
	}

	for (AllocationRecord& allocation : m_allocations)
	{
		if (allocation.Access.renderTargetView)
		{
			m_renderHardwareInterface->GetDescriptorService().ReleaseResourceView(allocation.Access.renderTargetView);
			allocation.Access.renderTargetView = {};
		}

		if (allocation.Access.depthStencilView)
		{
			m_renderHardwareInterface->GetDescriptorService().ReleaseResourceView(allocation.Access.depthStencilView);
			allocation.Access.depthStencilView = {};
		}

		if (allocation.Access.shaderResourceView)
		{
			m_renderHardwareInterface->GetDescriptorService().ReleaseResourceView(allocation.Access.shaderResourceView);
			allocation.Access.shaderResourceView = {};
		}

		if (allocation.Access.unorderedAccessView)
		{
			m_renderHardwareInterface->GetDescriptorService().ReleaseResourceView(allocation.Access.unorderedAccessView);
			allocation.Access.unorderedAccessView = {};
		}

		if (allocation.ownedResource)
		{
			m_renderHardwareInterface->GetResourceService().ReleaseOwnedResource(allocation.ownedResource);
			allocation.ownedResource = {};
		}
		allocation.Access.resource = {};
	}
}

const FrameGraphResourceAccess& FrameGraphTransientAllocator::Materialize(const FrameGraphTransientResourcePlan& transientPlan)
{
	if (AllocationRecord* existingAllocation = const_cast<AllocationRecord*>(FindAllocation(transientPlan.handle)))
	{
		return existingAllocation->Access;
	}

	m_allocations.push_back(CreateAllocationRecord(transientPlan));
	return m_allocations.back().Access;
}

const FrameGraphTransientAllocator::AllocationRecord* FrameGraphTransientAllocator::FindAllocation(FrameGraphResourceHandle handle) const noexcept
{
	const auto it = std::find_if(m_allocations.begin(), m_allocations.end(), [handle](const AllocationRecord& allocation) { return allocation.handle == handle; });

	return it != m_allocations.end() ? &(*it) : nullptr;
}

RhiOwnedMemoryBlockHandle FrameGraphTransientAllocator::GetOrCreateMemoryBlock(const FrameGraphTransientResourcePlan& transientPlan)
{
	const std::uint32_t blockIndex = transientPlan.physicalAllocation.physicalBlockIndex;
	const auto blockIt = std::find_if(m_memoryBlocks.begin(), m_memoryBlocks.end(), [blockIndex](const MemoryBlockRecord& block) { return block.physicalBlockIndex == blockIndex; });
	if (blockIt != m_memoryBlocks.end())
	{
		return blockIt->memoryBlock;
	}

	const std::wstring debugName = L"FG_TransientBlock_" + std::to_wstring(blockIndex);

	const RhiOwnedMemoryBlockHandle memoryBlock = m_renderHardwareInterface->GetResourceService().CreateTransientMemoryBlock(
	    transientPlan.physicalAllocation.pool,
	    transientPlan.physicalAllocation.sizeInBytes,
	    transientPlan.physicalAllocation.alignment,
	    debugName);

	m_memoryBlocks.push_back(MemoryBlockRecord{.physicalBlockIndex = blockIndex, .memoryBlock = memoryBlock});
	return memoryBlock;
}

FrameGraphTransientAllocator::AllocationRecord FrameGraphTransientAllocator::CreateAllocationRecord(const FrameGraphTransientResourcePlan& transientPlan)
{
	assert(m_renderHardwareInterface != nullptr);
	assert(transientPlan.handle.IsValid());
	assert(transientPlan.physicalAllocation.sizeInBytes > 0);
	assert(transientPlan.physicalAllocation.alignment > 0);

	AllocationRecord allocation;
	allocation.handle = transientPlan.handle;
	const RhiOwnedMemoryBlockHandle memoryBlock = GetOrCreateMemoryBlock(transientPlan);

	switch (transientPlan.kind)
	{
		case FrameGraphResourceKind::DepthStencil:
			CreateDepthStencilAllocation(allocation, transientPlan, memoryBlock);
			break;
		case FrameGraphResourceKind::ColorRenderTarget:
			CreateColorAllocation(allocation, transientPlan, memoryBlock);
			break;
		case FrameGraphResourceKind::Buffer:
			CreateBufferAllocation(allocation, transientPlan, memoryBlock);
			break;
		default:
			Diagnostics::Fatal(LogFrameGraphTransientAllocator, __FILE__, __LINE__, "FrameGraphTransientAllocator: unsupported transient resource kind for heap-backed allocation");
			break;
	}

	return allocation;
}
