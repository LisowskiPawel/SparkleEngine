#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Memory/VulkanGpuAllocation.h"

#include "Vulkan/Memory/VulkanGpuMemoryAllocator.h"

VulkanGpuAllocationRecord::~VulkanGpuAllocationRecord() noexcept
{
	if (Owner != nullptr)
	{
		Owner->UnregisterAllocationRecord(*this);
		Owner->DestroyAllocation(*this);
	}
	if (ParentMemoryBlock != nullptr && ParentMemoryBlock->AliasingResourceCount > 0)
	{
		--ParentMemoryBlock->AliasingResourceCount;
	}
}

VulkanGpuMemoryBlockRecord::~VulkanGpuMemoryBlockRecord() noexcept
{
	if (Owner != nullptr)
	{
		Owner->UnregisterMemoryBlockRecord(*this);
		Owner->DestroyMemoryBlock(*this);
	}
}

RhiOwnedResourceHandle MakeVulkanOwnedResourceHandle(std::unique_ptr<VulkanGpuAllocationRecord> record) noexcept
{
	return RhiOwnedResourceHandle{record.release()};
}

std::unique_ptr<VulkanGpuAllocationRecord> TakeVulkanOwnedResourceHandle(RhiOwnedResourceHandle handle) noexcept
{
	return std::unique_ptr<VulkanGpuAllocationRecord>(static_cast<VulkanGpuAllocationRecord*>(handle.Value));
}

VulkanGpuAllocationRecord* GetVulkanGpuAllocationRecord(RhiOwnedResourceHandle handle) noexcept
{
	return static_cast<VulkanGpuAllocationRecord*>(handle.Value);
}

RhiResourceHandle GetVulkanResourceHandle(const VulkanGpuAllocationRecord& record) noexcept
{
	switch (record.ResourceKind)
	{
		case VulkanGpuAllocationResourceKind::Buffer:
			return RhiResourceHandle{record.Buffer};

		case VulkanGpuAllocationResourceKind::Image:
			return RhiResourceHandle{record.Image};

		case VulkanGpuAllocationResourceKind::Unknown:
		default:
			return {};
	}
}

void SetVulkanAllocationRecordDebugName(VulkanGpuAllocationRecord& record, std::wstring_view debugName) noexcept
{
	record.DebugName = debugName;
	if (record.Owner != nullptr)
	{
		record.Owner->SetAllocationDebugName(record, debugName);
	}
}

RhiOwnedMemoryBlockHandle MakeVulkanOwnedMemoryBlockHandle(std::unique_ptr<VulkanGpuMemoryBlockRecord> record) noexcept
{
	return RhiOwnedMemoryBlockHandle{record.release()};
}

std::unique_ptr<VulkanGpuMemoryBlockRecord> TakeVulkanOwnedMemoryBlockHandle(RhiOwnedMemoryBlockHandle handle) noexcept
{
	return std::unique_ptr<VulkanGpuMemoryBlockRecord>(static_cast<VulkanGpuMemoryBlockRecord*>(handle.Value));
}

VulkanGpuMemoryBlockRecord* GetVulkanGpuMemoryBlockRecord(RhiOwnedMemoryBlockHandle handle) noexcept
{
	return static_cast<VulkanGpuMemoryBlockRecord*>(handle.Value);
}

void SetVulkanMemoryBlockRecordDebugName(VulkanGpuMemoryBlockRecord& record, std::wstring_view debugName) noexcept
{
	record.DebugName = debugName;
	if (record.Owner != nullptr)
	{
		record.Owner->SetMemoryBlockDebugName(record, debugName);
	}
}
