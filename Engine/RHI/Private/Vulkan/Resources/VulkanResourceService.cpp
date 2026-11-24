#include "Vulkan/Resources/VulkanResourceService.h"

#include "Validation/RhiContract.h"
#include "Vulkan/Commands/VulkanCommandQueue.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/Memory/VulkanGpuAllocation.h"
#include "Vulkan/Memory/VulkanGpuMemoryAllocator.h"
#include "Vulkan/VulkanTypeConversions.h"

VulkanResourceService::VulkanResourceService(VulkanRhi& rhi, VulkanGpuMemoryAllocator& memoryAllocator, const RhiCapabilities& capabilities) noexcept :
    m_rhi(rhi),
    m_memoryAllocator(memoryAllocator),
    m_capabilities(capabilities)
{
}

VulkanResourceService::~VulkanResourceService() noexcept
{
	FlushDeferredResourceReleases();
}

RhiOwnedResourceHandle VulkanResourceService::CreateTextureResource(
    const RhiTextureResourceDesc& desc,
    ResourceState initialState,
    RhiMemoryCategory category,
    RhiMemoryResidencyClass residencyClass,
    std::wstring_view debugName)
{
	(void) initialState;
	if (!RhiContract::IsTextureResourceDescUsable(m_capabilities, desc))
	{
		return {};
	}

	const VkImageCreateInfo imageCreateInfo = VulkanTypeConversions::BuildTextureCreateInfo(desc);
	std::unique_ptr<VulkanGpuAllocationRecord> record = m_memoryAllocator.CreateImage(imageCreateInfo, category, residencyClass, debugName);
	return MakeVulkanOwnedResourceHandle(std::move(record));
}

RhiOwnedResourceHandle VulkanResourceService::CreateBufferResource(
    const RhiBufferResourceDesc& desc,
    ResourceState initialState,
    RhiMemoryCategory category,
    RhiMemoryResidencyClass residencyClass,
    std::wstring_view debugName)
{
	(void) initialState;
	if (desc.SizeInBytes == 0)
	{
		return {};
	}

	const VkBufferCreateInfo bufferCreateInfo = VulkanTypeConversions::BuildBufferCreateInfo(desc);
	std::unique_ptr<VulkanGpuAllocationRecord> record = m_memoryAllocator.CreateBuffer(bufferCreateInfo, category, residencyClass, debugName);
	return MakeVulkanOwnedResourceHandle(std::move(record));
}

bool VulkanResourceService::WriteBufferResource(RhiOwnedResourceHandle resource, std::size_t destinationOffsetInBytes, const void* data, std::size_t sizeInBytes) noexcept
{
	VulkanGpuAllocationRecord* const record = GetVulkanGpuAllocationRecord(resource);
	return record != nullptr && m_memoryAllocator.WriteAllocation(*record, data, sizeInBytes, destinationOffsetInBytes);
}

void VulkanResourceService::ReleaseOwnedResource(RhiOwnedResourceHandle resource) noexcept
{
	std::unique_ptr<VulkanGpuAllocationRecord> record = TakeVulkanOwnedResourceHandle(resource);
	if (record == nullptr)
	{
		return;
	}

	m_memoryAllocator.QueueDestroyResource(std::move(record));
	DrainCompletedResourceReleases();
}

void VulkanResourceService::FlushDeferredResourceReleases() noexcept
{
	m_memoryAllocator.FlushPendingReleases();
}

void VulkanResourceService::DrainCompletedResourceReleases() noexcept
{
	std::array<std::uint64_t, RhiQueueTypeCount> completedValues{};
	for (std::size_t queueIndex = 0; queueIndex < RhiQueueTypeCount; ++queueIndex)
	{
		completedValues[queueIndex] = m_rhi.GetCommandQueue(static_cast<ERhiQueueType>(queueIndex)).GetCompletedSubmissionValue();
	}
	m_memoryAllocator.DrainCompletedReleases(completedValues);
}

RhiResourceHandle VulkanResourceService::GetResourceHandle(RhiOwnedResourceHandle resource) const noexcept
{
	VulkanGpuAllocationRecord* const record = GetVulkanGpuAllocationRecord(resource);
	return record != nullptr ? GetVulkanResourceHandle(*record) : RhiResourceHandle{};
}

RhiGpuVirtualAddress VulkanResourceService::GetResourceGpuVirtualAddress(RhiOwnedResourceHandle resource) const noexcept
{
	VulkanGpuAllocationRecord* const record = GetVulkanGpuAllocationRecord(resource);
	if (record == nullptr)
	{
		return 0;
	}
	return record->DeviceAddress != 0 ? record->DeviceAddress : reinterpret_cast<std::uint64_t>(record->Buffer);
}

RhiResourceAllocationInfo VulkanResourceService::GetTextureAllocationInfo(const RhiTextureResourceDesc& desc) const noexcept
{
	if (desc.Width == 0 || desc.Height == 0 || desc.Format == PixelFormat::Unknown)
	{
		return {};
	}

	const VkImageCreateInfo imageCreateInfo = VulkanTypeConversions::BuildTextureCreateInfo(desc);

	const VkDeviceImageMemoryRequirements requirementsInfo{
	    .sType = VK_STRUCTURE_TYPE_DEVICE_IMAGE_MEMORY_REQUIREMENTS,
	    .pNext = nullptr,
	    .pCreateInfo = &imageCreateInfo,
	    .planeAspect = static_cast<VkImageAspectFlagBits>(0)};

	VkMemoryRequirements2 memoryRequirements{.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2};
	vkGetDeviceImageMemoryRequirements(m_rhi.GetDevice(), &requirementsInfo, &memoryRequirements);
	return RhiResourceAllocationInfo{.SizeInBytes = memoryRequirements.memoryRequirements.size, .Alignment = memoryRequirements.memoryRequirements.alignment};
}

RhiResourceAllocationInfo VulkanResourceService::GetBufferAllocationInfo(const RhiBufferResourceDesc& desc) const noexcept
{
	if (desc.SizeInBytes == 0)
	{
		return {};
	}

	const VkBufferCreateInfo bufferCreateInfo = VulkanTypeConversions::BuildBufferCreateInfo(desc);
	const VkDeviceBufferMemoryRequirements requirementsInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS, .pNext = nullptr, .pCreateInfo = &bufferCreateInfo};
	VkMemoryRequirements2 memoryRequirements{.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2};
	vkGetDeviceBufferMemoryRequirements(m_rhi.GetDevice(), &requirementsInfo, &memoryRequirements);
	return RhiResourceAllocationInfo{.SizeInBytes = memoryRequirements.memoryRequirements.size, .Alignment = memoryRequirements.memoryRequirements.alignment};
}

RhiOwnedMemoryBlockHandle VulkanResourceService::CreateTransientMemoryBlock(RhiTransientAllocationPool pool, std::uint64_t sizeInBytes, std::uint64_t alignment, std::wstring_view debugName)
{
	if (sizeInBytes == 0)
	{
		return {};
	}

	std::unique_ptr<VulkanGpuMemoryBlockRecord> record = m_memoryAllocator.CreateTransientMemoryBlock(pool, sizeInBytes, alignment, debugName);
	return MakeVulkanOwnedMemoryBlockHandle(std::move(record));
}

void VulkanResourceService::ReleaseTransientMemoryBlock(RhiOwnedMemoryBlockHandle memoryBlock) noexcept
{
	std::unique_ptr<VulkanGpuMemoryBlockRecord> record = TakeVulkanOwnedMemoryBlockHandle(memoryBlock);
	if (record == nullptr)
	{
		return;
	}

	m_memoryAllocator.QueueDestroyMemoryBlock(std::move(record));
	DrainCompletedResourceReleases();
}

RhiOwnedResourceHandle VulkanResourceService::CreateAliasingTextureResource(
    RhiOwnedMemoryBlockHandle memoryBlock,
    std::uint64_t memoryBlockOffset,
    const RhiTransientTextureAllocationDesc& desc,
    std::wstring_view debugName)
{
	(void) desc.InitialState;
	(void) desc.ClearValue;
	const bool bUsableTexture = RhiContract::IsTextureResourceDescUsable(m_capabilities, desc.ResourceDesc);
	if (!bUsableTexture)
	{
		return {};
	}

	VulkanGpuMemoryBlockRecord* const memoryBlockRecord = GetVulkanGpuMemoryBlockRecord(memoryBlock);
	if (memoryBlockRecord == nullptr)
	{
		return {};
	}

	const VkImageCreateInfo imageCreateInfo = VulkanTypeConversions::BuildTextureCreateInfo(desc.ResourceDesc);
	std::unique_ptr<VulkanGpuAllocationRecord> record = m_memoryAllocator.CreateAliasingImage(*memoryBlockRecord, memoryBlockOffset, imageCreateInfo, debugName);
	return MakeVulkanOwnedResourceHandle(std::move(record));
}

RhiOwnedResourceHandle VulkanResourceService::CreateAliasingBufferResource(
    RhiOwnedMemoryBlockHandle memoryBlock,
    std::uint64_t memoryBlockOffset,
    const RhiTransientBufferAllocationDesc& desc,
    std::wstring_view debugName)
{
	(void) desc.InitialState;
	if (desc.ResourceDesc.SizeInBytes == 0)
	{
		return {};
	}

	VulkanGpuMemoryBlockRecord* const memoryBlockRecord = GetVulkanGpuMemoryBlockRecord(memoryBlock);
	if (memoryBlockRecord == nullptr)
	{
		return {};
	}

	const VkBufferCreateInfo bufferCreateInfo = VulkanTypeConversions::BuildBufferCreateInfo(desc.ResourceDesc);
	std::unique_ptr<VulkanGpuAllocationRecord> record = m_memoryAllocator.CreateAliasingBuffer(*memoryBlockRecord, memoryBlockOffset, bufferCreateInfo, debugName);
	return MakeVulkanOwnedResourceHandle(std::move(record));
}

bool VulkanResourceService::SupportsUnorderedAccess(RhiResourceHandle resource) const noexcept
{
	const VulkanGpuAllocationRecord* const record = m_memoryAllocator.FindAllocationRecord(resource);
	if (record == nullptr)
	{
		return false;
	}

	switch (record->ResourceKind)
	{
		case VulkanGpuAllocationResourceKind::Image:
			return (record->Usage & VK_IMAGE_USAGE_STORAGE_BIT) != 0;
		case VulkanGpuAllocationResourceKind::Buffer:
			return (record->Usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) != 0;
		case VulkanGpuAllocationResourceKind::Unknown:
		default:
			return false;
	}
}
