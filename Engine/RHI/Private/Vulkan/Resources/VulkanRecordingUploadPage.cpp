#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Resources/VulkanRecordingUploadPage.h"

#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/Memory/VulkanGpuAllocation.h"
#include "Vulkan/Memory/VulkanGpuMemoryAllocator.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <utility>

struct VulkanRecordingUploadPage::Page final
{
	struct Allocation final
	{
		VkDeviceSize Offset = 0;
		VkDeviceSize Size = 0;
	};

	std::unique_ptr<VulkanGpuAllocationRecord> Buffer;
	std::array<Allocation, 4096> Allocations;
	std::size_t AllocationCount = 0;
};

VulkanRecordingUploadPage::VulkanRecordingUploadPage(VulkanRhi& rhi, VulkanGpuMemoryAllocator& memoryAllocator, std::uint64_t capacityInBytes) noexcept :
    m_memoryAllocator(&memoryAllocator),
    m_capacityInBytes(capacityInBytes)
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(rhi.GetPhysicalDevice(), &properties);
	m_alignment = std::max<VkDeviceSize>(properties.limits.minUniformBufferOffsetAlignment, 1);
	AddPage(capacityInBytes);
}

void VulkanRecordingUploadPage::AddPage(std::uint64_t capacityInBytes)
{
	auto page = std::make_unique<Page>();

	const VkBufferCreateInfo createInfo{
	    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	    .pNext = nullptr,
	    .flags = 0,
	    .size = capacityInBytes,
	    .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
	    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	    .queueFamilyIndexCount = 0,
	    .pQueueFamilyIndices = nullptr};

	page->Buffer = m_memoryAllocator->CreateBuffer(createInfo, RhiMemoryCategory::ConstantBuffer, RhiMemoryResidencyClass::HostUpload, L"Vulkan Recording Upload Page");
	if (page->Buffer != nullptr)
	{
		(void) m_memoryAllocator->MapUploadPage(*page->Buffer);
	}
	m_pages.push_back(std::move(page));
}

VulkanRecordingUploadPage::~VulkanRecordingUploadPage() noexcept
{
	for (auto& page : m_pages)
	{
		if (page->Buffer != nullptr)
		{
			m_memoryAllocator->QueueDestroyResource(std::move(page->Buffer));
		}
	}
}

void VulkanRecordingUploadPage::Reset() noexcept
{
	for (auto& page : m_pages)
	{
		page->AllocationCount = 0;
	}
	m_pageIndex = 0;
	m_offset = 0;
}

RhiGpuVirtualAddress VulkanRecordingUploadPage::AllocateAndCopy(const void* data, std::uint32_t sizeInBytes) noexcept
{
	if (data == nullptr || sizeInBytes == 0)
	{
		return {};
	}

	std::uint64_t alignedOffset = (m_offset + m_alignment - 1) & ~(m_alignment - 1);
	const Page& current = *m_pages[m_pageIndex];
	if (current.Buffer != nullptr
	    && (current.AllocationCount == current.Allocations.size() || alignedOffset > current.Buffer->ResourceSizeInBytes || sizeInBytes > current.Buffer->ResourceSizeInBytes - alignedOffset))
	{
		// Allocation addresses stay stable while the recording spans more pages.
		do
		{
			++m_pageIndex;
		} while (m_pageIndex < m_pages.size() && m_pages[m_pageIndex]->Buffer != nullptr && m_pages[m_pageIndex]->Buffer->ResourceSizeInBytes < sizeInBytes);
		if (m_pageIndex == m_pages.size())
		{
			AddPage((std::max) (m_capacityInBytes, static_cast<std::uint64_t>(sizeInBytes)));
		}
		alignedOffset = 0;
	}
	Page& page = *m_pages[m_pageIndex];
	if (page.Buffer == nullptr || page.Buffer->Buffer == VK_NULL_HANDLE || page.Buffer->CpuMappedAddress == nullptr)
	{
		return {};
	}
	std::memcpy(static_cast<std::byte*>(page.Buffer->CpuMappedAddress) + alignedOffset, data, sizeInBytes);
	if (!m_memoryAllocator->FlushUploadPage(*page.Buffer, alignedOffset, sizeInBytes))
	{
		return {};
	}

	const std::size_t allocationIndex = page.AllocationCount++;
	page.Allocations[allocationIndex] = Page::Allocation{.Offset = alignedOffset, .Size = sizeInBytes};
	m_offset = alignedOffset + sizeInBytes;
	return reinterpret_cast<RhiGpuVirtualAddress>(&page.Allocations[allocationIndex]);
}

bool VulkanRecordingUploadPage::Resolve(RhiGpuVirtualAddress address, VkBuffer& buffer, VkDeviceSize& offset, VkDeviceSize& range) const noexcept
{
	if (address == 0)
	{
		return false;
	}

	const std::uintptr_t allocationAddress = static_cast<std::uintptr_t>(address);
	for (std::size_t pageIndex = 0; pageIndex <= m_pageIndex; ++pageIndex)
	{
		const Page& page = *m_pages[pageIndex];
		const std::uintptr_t allocationBegin = reinterpret_cast<std::uintptr_t>(page.Allocations.data());
		const std::uintptr_t allocationEnd = allocationBegin + page.AllocationCount * sizeof(Page::Allocation);
		if (allocationAddress >= allocationBegin && allocationAddress < allocationEnd && (allocationAddress - allocationBegin) % sizeof(Page::Allocation) == 0)
		{
			const Page::Allocation& allocation = page.Allocations[(allocationAddress - allocationBegin) / sizeof(Page::Allocation)];
			buffer = page.Buffer->Buffer;
			offset = allocation.Offset;
			range = allocation.Size;
			return true;
		}
	}
	return false;
}
