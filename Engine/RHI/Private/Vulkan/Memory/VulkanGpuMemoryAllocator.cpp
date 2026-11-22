#include "Vulkan/VulkanPCH.h"

#include <vk_mem_alloc.h>

#include "Vulkan/Memory/VulkanGpuMemoryAllocator.h"

#include "Core/Public/Strings/StringUtils.h"
#include "Vulkan/Memory/VulkanMemoryUsage.h"
#include "Vulkan/Core/VulkanResult.h"
#include "Vulkan/Device/VulkanRhi.h"
#include "Vulkan/Diagnostics/VulkanDebugNames.h"
#include "Vulkan/Memory/VulkanRecordingResourceTable.h"

#include <algorithm>
#include <array>
#include <mutex>
#include <string>
#include <vector>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_vulkanMemoryLogger, "RHI.Vulkan.Memory");

struct VulkanGpuMemoryAllocator::Impl final
{
	VmaAllocator Allocator = nullptr;
	mutable std::mutex RecordsMutex;
	std::vector<VulkanGpuAllocationRecord*> LiveRecords;
	std::vector<VulkanGpuMemoryBlockRecord*> LiveMemoryBlockRecords;
	std::vector<PendingAllocationRelease> PendingReleases;
	std::vector<PendingMemoryBlockRelease> PendingMemoryBlockReleases;

	~Impl() noexcept
	{
		PendingReleases.clear();
		PendingMemoryBlockReleases.clear();
		if (Allocator != nullptr)
		{
			vmaDestroyAllocator(Allocator);
			Allocator = nullptr;
		}
	}
};

static VmaAllocationCreateInfo ResolveAllocationCreateInfo(RhiMemoryResidencyClass residencyClass) noexcept
{
	switch (residencyClass)
	{
		case RhiMemoryResidencyClass::HostUpload:
			return {.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, .usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST};
		case RhiMemoryResidencyClass::HostReadback:
			return {.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT, .usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST};
		case RhiMemoryResidencyClass::DeviceLocal:
		case RhiMemoryResidencyClass::Transient:
		default:
			return {.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE};
	}
}

VulkanGpuMemoryAllocator::VulkanGpuMemoryAllocator(VulkanRhi& rhi) noexcept :
    m_rhi(rhi),
    m_impl(std::make_unique<Impl>()),
    m_recordingResources(std::make_unique<VulkanRecordingResourceTable>())
{
	const bool bHandlesUnavailablePhysicalDevice = m_rhi.GetInstance() == VK_NULL_HANDLE || m_rhi.GetPhysicalDevice() == VK_NULL_HANDLE;
	const bool bDeviceHandlesUnavailable = bHandlesUnavailablePhysicalDevice || (m_rhi.GetDevice() == VK_NULL_HANDLE);

	if (bDeviceHandlesUnavailable)
	{
		Diagnostics::Fatal(g_vulkanMemoryLogger, __FILE__, __LINE__, "VulkanGpuMemoryAllocator requires a valid instance, physical device, and device");
	}

	const bool bMemoryBudgetExtensionEnabled = std::any_of(
	    m_rhi.GetEnabledDeviceExtensions().begin(),
	    m_rhi.GetEnabledDeviceExtensions().end(),
	    [](const std::string& extension) noexcept { return extension == VK_EXT_MEMORY_BUDGET_EXTENSION_NAME; });

	VmaAllocatorCreateFlags allocatorFlags = bMemoryBudgetExtensionEnabled ? VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT : 0;
	if (m_rhi.GetRayTracingCapabilities().bSupportsAccelerationStructure)
	{
		allocatorFlags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	}
	const std::uint32_t vulkanApiVersion = std::min(m_rhi.GetAdapterInfo().ApiVersion, static_cast<std::uint32_t>(VK_API_VERSION_1_3));

	const VmaAllocatorCreateInfo allocatorCreateInfo{
	    .flags = allocatorFlags,
	    .physicalDevice = m_rhi.GetPhysicalDevice(),
	    .device = m_rhi.GetDevice(),
	    .preferredLargeHeapBlockSize = 0,
	    .pAllocationCallbacks = nullptr,
	    .pDeviceMemoryCallbacks = nullptr,
	    .pHeapSizeLimit = nullptr,
	    .pVulkanFunctions = nullptr,
	    .instance = m_rhi.GetInstance(),
	    .vulkanApiVersion = vulkanApiVersion,
	    .pTypeExternalMemoryHandleTypes = nullptr};

	const VkResult result = vmaCreateAllocator(&allocatorCreateInfo, &m_impl->Allocator);
	if (!VulkanResult::Succeeded(result))
	{
		const auto failureMessage = VulkanResult::FormatFailure("vmaCreateAllocator", result);
		Diagnostics::Fatal(g_vulkanMemoryLogger, __FILE__, __LINE__, failureMessage);
	}
}

VulkanGpuMemoryAllocator::~VulkanGpuMemoryAllocator() noexcept = default;

void VulkanGpuMemoryAllocator::SetCurrentFrameIndex(std::uint32_t frameIndex) noexcept
{
	m_owner.AssertAccess();
	vmaSetCurrentFrameIndex(m_impl->Allocator, frameIndex);
}

static void AccumulatePendingMemoryUsage(const auto& pendingReleases, RhiMemoryUsageSnapshot& snapshot)
{
	for (const auto& pendingRelease : pendingReleases)
	{
		if (pendingRelease != nullptr)
		{
			snapshot.DelayedDestructionBytes += pendingRelease->AllocatedBytes;
			++snapshot.DelayedDestructionAllocationCount;
		}
	}
}

RhiMemoryUsageSnapshot VulkanGpuMemoryAllocator::CreateMemoryUsageSnapshot() const
{
	m_owner.AssertAccess();
	RhiMemoryUsageSnapshot snapshot;
	std::array<VmaBudget, VK_MAX_MEMORY_HEAPS> heapBudgets = {};
	VulkanMemoryUsage::QueryBudget(m_impl->Allocator, heapBudgets, snapshot);
	std::vector<VulkanMemoryCategoryAggregation> aggregations;
	{
		std::scoped_lock lock(m_impl->RecordsMutex);
		aggregations.reserve(m_impl->LiveRecords.size());
		VulkanMemoryUsage::AccumulateAllocations(m_impl->Allocator, m_impl->LiveRecords, heapBudgets, aggregations, snapshot);
		VulkanMemoryUsage::AccumulateBlocks(m_impl->Allocator, m_impl->LiveMemoryBlockRecords, heapBudgets, aggregations, snapshot);
		AccumulatePendingMemoryUsage(m_impl->PendingReleases, snapshot);
		AccumulatePendingMemoryUsage(m_impl->PendingMemoryBlockReleases, snapshot);
	}
	VulkanMemoryUsage::AppendCategories(aggregations, snapshot);
	return snapshot;
}

std::unique_ptr<VulkanGpuAllocationRecord> VulkanGpuMemoryAllocator::CreateBuffer(
    const VkBufferCreateInfo& bufferCreateInfo,
    RhiMemoryCategory category,
    RhiMemoryResidencyClass residencyClass,
    std::wstring_view debugName,
    VkDeviceSize minimumAlignment) noexcept
{
	const VmaAllocationCreateInfo allocationCreateInfo = ResolveAllocationCreateInfo(residencyClass);

	VkBufferCreateInfo nativeCreateInfo = bufferCreateInfo;
	m_rhi.ConfigureResourceQueueSharing(nativeCreateInfo);
	VkBuffer buffer = VK_NULL_HANDLE;
	VmaAllocation allocation = nullptr;
	const VkResult result = vmaCreateBufferWithAlignment(m_impl->Allocator, &nativeCreateInfo, &allocationCreateInfo, minimumAlignment, &buffer, &allocation, nullptr);
	if (!VulkanResult::Succeeded(result) || buffer == VK_NULL_HANDLE || allocation == nullptr)
	{
		return {};
	}

	std::unique_ptr<VulkanGpuAllocationRecord> record = CreateAllocationRecord(VulkanGpuAllocationResourceKind::Buffer, buffer, VK_NULL_HANDLE, allocation, category, residencyClass, debugName);
	record->ResourceSizeInBytes = bufferCreateInfo.size;
	record->Usage = bufferCreateInfo.usage;
	const bool bBufferDeviceAddressEnabled = (bufferCreateInfo.usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) != 0 && m_rhi.GetGetBufferDeviceAddress() != nullptr;

	if (bBufferDeviceAddressEnabled)
	{
		const VkBufferDeviceAddressInfo addressInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .pNext = nullptr, .buffer = buffer};
		record->BufferDeviceAddress = m_rhi.GetGetBufferDeviceAddress()(m_rhi.GetDevice(), &addressInfo);
		record->DeviceAddress = record->BufferDeviceAddress;
	}
	return record;
}

std::unique_ptr<VulkanGpuAllocationRecord> VulkanGpuMemoryAllocator::CreateImage(
    const VkImageCreateInfo& imageCreateInfo,
    RhiMemoryCategory category,
    RhiMemoryResidencyClass residencyClass,
    std::wstring_view debugName) noexcept
{
	const VmaAllocationCreateInfo allocationCreateInfo = ResolveAllocationCreateInfo(residencyClass);

	VkImageCreateInfo nativeCreateInfo = imageCreateInfo;
	m_rhi.ConfigureResourceQueueSharing(nativeCreateInfo);
	VkImage image = VK_NULL_HANDLE;
	VmaAllocation allocation = nullptr;
	const VkResult result = vmaCreateImage(m_impl->Allocator, &nativeCreateInfo, &allocationCreateInfo, &image, &allocation, nullptr);
	if (!VulkanResult::Succeeded(result) || image == VK_NULL_HANDLE || allocation == nullptr)
	{
		return {};
	}

	std::unique_ptr<VulkanGpuAllocationRecord> record = CreateAllocationRecord(VulkanGpuAllocationResourceKind::Image, VK_NULL_HANDLE, image, allocation, category, residencyClass, debugName);
	record->Format = imageCreateInfo.format;
	record->Extent = imageCreateInfo.extent;
	record->AspectMask = ResolveImageAspectMask(imageCreateInfo.format);
	record->ImageFlags = imageCreateInfo.flags;
	record->Usage = imageCreateInfo.usage;
	return record;
}

std::unique_ptr<VulkanGpuMemoryBlockRecord> VulkanGpuMemoryAllocator::CreateTransientMemoryBlock(
    RhiTransientAllocationPool,
    std::uint64_t sizeInBytes,
    std::uint64_t alignment,
    std::wstring_view debugName) noexcept
{
	if (sizeInBytes == 0)
	{
		return {};
	}

	auto record = std::make_unique<VulkanGpuMemoryBlockRecord>();
	record->Category = RhiMemoryCategory::TransientResource;
	record->ResidencyClass = RhiMemoryResidencyClass::Transient;
	record->RequestedSizeInBytes = sizeInBytes;
	record->UsedBytes = sizeInBytes;
	record->Alignment = alignment;
	record->Owner = this;
	record->DebugName = std::wstring(debugName);
	RegisterMemoryBlockRecord(*record);
	return record;
}

std::unique_ptr<VulkanGpuAllocationRecord> VulkanGpuMemoryAllocator::CreateAliasingImage(
    VulkanGpuMemoryBlockRecord& memoryBlock,
    std::uint64_t memoryBlockOffset,
    const VkImageCreateInfo& imageCreateInfo,
    std::wstring_view debugName) noexcept
{
	VkImageCreateInfo nativeCreateInfo = imageCreateInfo;
	m_rhi.ConfigureResourceQueueSharing(nativeCreateInfo);
	const bool bMemoryBlockPrepared = EnsureMemoryBlockAllocationForImage(memoryBlock, nativeCreateInfo);
	const bool bMemoryBlockAllocated = bMemoryBlockPrepared && memoryBlock.Allocation != nullptr;

	if (!bMemoryBlockAllocated)
	{
		return {};
	}

	VkImage image = VK_NULL_HANDLE;
	const VkResult result = vmaCreateAliasingImage2(m_impl->Allocator, memoryBlock.Allocation, static_cast<VkDeviceSize>(memoryBlockOffset), &nativeCreateInfo, &image);
	if (!VulkanResult::Succeeded(result) || image == VK_NULL_HANDLE)
	{
		return {};
	}

	std::unique_ptr<VulkanGpuAllocationRecord> record = CreateAliasingAllocationRecord(VulkanGpuAllocationResourceKind::Image, VK_NULL_HANDLE, image, memoryBlock, debugName);
	record->Format = imageCreateInfo.format;
	record->Extent = imageCreateInfo.extent;
	record->AspectMask = ResolveImageAspectMask(imageCreateInfo.format);
	record->ImageFlags = imageCreateInfo.flags;
	record->Usage = imageCreateInfo.usage;
	return record;
}

std::unique_ptr<VulkanGpuAllocationRecord> VulkanGpuMemoryAllocator::CreateAliasingBuffer(
    VulkanGpuMemoryBlockRecord& memoryBlock,
    std::uint64_t memoryBlockOffset,
    const VkBufferCreateInfo& bufferCreateInfo,
    std::wstring_view debugName) noexcept
{
	VkBufferCreateInfo nativeCreateInfo = bufferCreateInfo;
	m_rhi.ConfigureResourceQueueSharing(nativeCreateInfo);
	const bool bMemoryBlockPrepared = EnsureMemoryBlockAllocationForBuffer(memoryBlock, nativeCreateInfo);
	const bool bMemoryBlockAllocated = bMemoryBlockPrepared && memoryBlock.Allocation != nullptr;

	if (!bMemoryBlockAllocated)
	{
		return {};
	}

	VkBuffer buffer = VK_NULL_HANDLE;
	const VkResult result = vmaCreateAliasingBuffer2(m_impl->Allocator, memoryBlock.Allocation, static_cast<VkDeviceSize>(memoryBlockOffset), &nativeCreateInfo, &buffer);
	if (!VulkanResult::Succeeded(result) || buffer == VK_NULL_HANDLE)
	{
		return {};
	}

	std::unique_ptr<VulkanGpuAllocationRecord> record = CreateAliasingAllocationRecord(VulkanGpuAllocationResourceKind::Buffer, buffer, VK_NULL_HANDLE, memoryBlock, debugName);
	record->Usage = bufferCreateInfo.usage;
	record->ResourceSizeInBytes = bufferCreateInfo.size;
	return record;
}

bool VulkanGpuMemoryAllocator::WriteAllocation(VulkanGpuAllocationRecord& record, const void* data, std::size_t sizeInBytes, std::size_t destinationOffsetInBytes) noexcept
{
	if (record.Allocation == nullptr)
	{
		return false;
	}
	if (data == nullptr || sizeInBytes == 0)
	{
		return false;
	}
	if (destinationOffsetInBytes > record.ResourceSizeInBytes || sizeInBytes > record.ResourceSizeInBytes - destinationOffsetInBytes)
	{
		return false;
	}

	const VkResult result = vmaCopyMemoryToAllocation(m_impl->Allocator, data, record.Allocation, destinationOffsetInBytes, sizeInBytes);
	return VulkanResult::Succeeded(result);
}

bool VulkanGpuMemoryAllocator::ReadAllocation(const VulkanGpuAllocationRecord& record, void* data, std::size_t sizeInBytes, std::size_t sourceOffsetInBytes) noexcept
{
	if (record.Allocation == nullptr)
	{
		return false;
	}
	if (data == nullptr || sizeInBytes == 0 || record.ResidencyClass != RhiMemoryResidencyClass::HostReadback)
	{
		return false;
	}
	if (sourceOffsetInBytes > record.ResourceSizeInBytes || sizeInBytes > record.ResourceSizeInBytes - sourceOffsetInBytes)
	{
		return false;
	}

	const VkResult result = vmaCopyAllocationToMemory(m_impl->Allocator, record.Allocation, sourceOffsetInBytes, data, sizeInBytes);
	return VulkanResult::Succeeded(result);
}

bool VulkanGpuMemoryAllocator::FlushUploadPage(VulkanGpuAllocationRecord& record, std::size_t offsetInBytes, std::size_t sizeInBytes) noexcept
{
	if (record.Allocation == nullptr || record.CpuMappedAddress == nullptr || offsetInBytes > record.ResourceSizeInBytes || sizeInBytes > record.ResourceSizeInBytes - offsetInBytes)
	{
		return false;
	}

	const VkResult result = vmaFlushAllocation(m_impl->Allocator, record.Allocation, offsetInBytes, sizeInBytes);
	return VulkanResult::Succeeded(result);
}

void* VulkanGpuMemoryAllocator::MapUploadPage(VulkanGpuAllocationRecord& record) noexcept
{
	if (record.Allocation == nullptr || record.ResidencyClass != RhiMemoryResidencyClass::HostUpload)
	{
		return nullptr;
	}
	if (record.CpuMappedAddress != nullptr)
	{
		return record.CpuMappedAddress;
	}

	void* mappedData = nullptr;
	const VkResult mapResult = vmaMapMemory(m_impl->Allocator, record.Allocation, &mappedData);
	if (!VulkanResult::Succeeded(mapResult) || mappedData == nullptr)
	{
		return nullptr;
	}

	record.CpuMappedAddress = mappedData;
	return mappedData;
}

void VulkanGpuMemoryAllocator::PublishRecordingReadView() noexcept
{
	m_owner.AssertAccess();

	m_recordingResources->Publish(m_impl->LiveRecords);
}

bool VulkanGpuMemoryAllocator::ResolveRecordingResource(RhiResourceHandle resource, VulkanRecordingResource& outResource) const noexcept
{
	return m_recordingResources->Resolve(resource, outResource);
}

bool VulkanGpuMemoryAllocator::ResolveRecordingAddress(RhiGpuVirtualAddress address, VulkanRecordingResource& outResource) const noexcept
{
	return m_recordingResources->Resolve(address, outResource);
}

bool VulkanGpuMemoryAllocator::ResolveCoordinatorRecordingResource(RhiResourceHandle resource, VulkanRecordingResource& outResource) const noexcept
{
	VulkanGpuAllocationRecord* const record = FindAllocationRecord(resource);
	if (record == nullptr)
	{
		return false;
	}

	outResource = VulkanRecordingResourceTable::BuildResource(*record);
	return true;
}

bool VulkanGpuMemoryAllocator::ResolveCoordinatorRecordingAddress(RhiGpuVirtualAddress address, VulkanRecordingResource& outResource) const noexcept
{
	VulkanGpuAllocationRecord* const record = FindAllocationRecordByDeviceAddress(address);
	if (record == nullptr)
	{
		return false;
	}

	outResource = VulkanRecordingResourceTable::BuildResource(*record);
	return true;
}

VulkanRecordingResourceUseToken VulkanGpuMemoryAllocator::RetainRecordingResource(RhiResourceHandle resource) const noexcept
{
	return m_recordingResources->Retain(resource);
}

VulkanRecordingResourceUseToken VulkanGpuMemoryAllocator::RetainCoordinatorRecordingResource(RhiResourceHandle resource) const noexcept
{
	VulkanGpuAllocationRecord* const record = FindAllocationRecord(resource);
	return record != nullptr ? m_recordingResources->Retain(*record) : VulkanRecordingResourceUseToken{};
}

void VulkanGpuMemoryAllocator::ReleaseRecordingResource(VulkanRecordingResourceUseToken use, RhiSubmissionToken submissionToken) const noexcept
{
	m_recordingResources->Release(use, submissionToken);
}

VulkanGpuAllocationRecord* VulkanGpuMemoryAllocator::FindAllocationRecord(RhiResourceHandle resource) const noexcept
{
	m_owner.AssertAccess();
	if (!resource)
	{
		return nullptr;
	}

	std::scoped_lock lock(m_impl->RecordsMutex);
	for (VulkanGpuAllocationRecord* record : m_impl->LiveRecords)
	{
		if (record == nullptr || record->bPendingRelease)
		{
			continue;
		}

		if (GetVulkanResourceHandle(*record).Value == resource.Value)
		{
			return record;
		}
	}

	return nullptr;
}

VulkanGpuAllocationRecord* VulkanGpuMemoryAllocator::FindAllocationRecordByDeviceAddress(VkDeviceAddress deviceAddress) const noexcept
{
	m_owner.AssertAccess();
	if (deviceAddress == 0)
	{
		return nullptr;
	}

	std::scoped_lock lock(m_impl->RecordsMutex);
	for (VulkanGpuAllocationRecord* record : m_impl->LiveRecords)
	{
		if (record == nullptr || record->bPendingRelease)
		{
			continue;
		}

		const bool bMatchesDeviceAddress = record->DeviceAddress == deviceAddress;
		const bool bFallsWithinBufferDeviceAddress = record->BufferDeviceAddress != 0 && deviceAddress >= record->BufferDeviceAddress;
		const bool bFallsWithinBuffer = bFallsWithinBufferDeviceAddress && (deviceAddress - record->BufferDeviceAddress < record->ResourceSizeInBytes);
		if (bMatchesDeviceAddress || bFallsWithinBuffer)
		{
			return record;
		}
	}

	return nullptr;
}

void VulkanGpuMemoryAllocator::QueueDestroyResource(std::unique_ptr<VulkanGpuAllocationRecord> record) noexcept
{
	m_owner.AssertAccess();
	if (record == nullptr)
	{
		return;
	}

	std::scoped_lock lock(m_impl->RecordsMutex);
	record->bPendingRelease = true;
	m_impl->PendingReleases.push_back(std::move(record));
}

void VulkanGpuMemoryAllocator::QueueDestroyMemoryBlock(std::unique_ptr<VulkanGpuMemoryBlockRecord> record) noexcept
{
	m_owner.AssertAccess();
	if (record == nullptr)
	{
		return;
	}

	std::scoped_lock lock(m_impl->RecordsMutex);
	m_impl->PendingMemoryBlockReleases.push_back(std::move(record));
}

void VulkanGpuMemoryAllocator::DrainCompletedReleases(const std::array<std::uint64_t, RhiQueueTypeCount>& completedValues) noexcept
{
	m_owner.AssertAccess();

	std::vector<std::unique_ptr<VulkanGpuMemoryBlockRecord>> readyMemoryBlockReleases;
	std::vector<std::unique_ptr<VulkanGpuAllocationRecord>> readyReleases;
	{
		std::scoped_lock lock(m_impl->RecordsMutex);
		auto pending = m_impl->PendingReleases.begin();
		while (pending != m_impl->PendingReleases.end())
		{
			const VulkanGpuAllocationRecord* const record = pending->get();
			const bool bUnreferenced = record != nullptr && record->RecordingReferenceCount.load(std::memory_order_relaxed) == 0;
			const bool bRetired = bUnreferenced && record->LastUse.IsComplete(completedValues);

			if (record == nullptr || bRetired)
			{
				readyReleases.push_back(std::move(*pending));
				pending = m_impl->PendingReleases.erase(pending);
			}
			else
			{
				++pending;
			}
		}

		auto pendingMemoryBlock = m_impl->PendingMemoryBlockReleases.begin();
		while (pendingMemoryBlock != m_impl->PendingMemoryBlockReleases.end())
		{
			const VulkanGpuMemoryBlockRecord* const record = pendingMemoryBlock->get();
			const bool bNoAliasedResources = record != nullptr && record->AliasingResourceCount == 0;
			const bool bUnreferenced = bNoAliasedResources && record->RecordingReferenceCount.load(std::memory_order_relaxed) == 0;
			const bool bRetired = bUnreferenced && record->LastUse.IsComplete(completedValues);

			if (record == nullptr || bRetired)
			{
				readyMemoryBlockReleases.push_back(std::move(*pendingMemoryBlock));
				pendingMemoryBlock = m_impl->PendingMemoryBlockReleases.erase(pendingMemoryBlock);
			}
			else
			{
				++pendingMemoryBlock;
			}
		}
	}

	readyReleases.clear();
	readyMemoryBlockReleases.clear();
}

void VulkanGpuMemoryAllocator::FlushPendingReleases() noexcept
{
	std::array<std::uint64_t, RhiQueueTypeCount> completedValues{};
	completedValues.fill(UINT64_MAX);
	DrainCompletedReleases(completedValues);
}

void VulkanGpuMemoryAllocator::DestroyAllocation(VulkanGpuAllocationRecord& record) noexcept
{
	if (record.CpuMappedAddress != nullptr)
	{
		vmaUnmapMemory(m_impl->Allocator, record.Allocation);
	}
	if (record.AccelerationStructure != VK_NULL_HANDLE && m_rhi.GetDestroyAccelerationStructure() != nullptr)
	{
		m_rhi.GetDestroyAccelerationStructure()(m_rhi.GetDevice(), record.AccelerationStructure, nullptr);
	}

	if (record.ParentMemoryBlock != nullptr)
	{
		if (record.Buffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(m_rhi.GetDevice(), record.Buffer, nullptr);
		}
		if (record.Image != VK_NULL_HANDLE)
		{
			vkDestroyImage(m_rhi.GetDevice(), record.Image, nullptr);
		}
		return;
	}

	switch (record.ResourceKind)
	{
		case VulkanGpuAllocationResourceKind::Buffer:
			vmaDestroyBuffer(m_impl->Allocator, record.Buffer, record.Allocation);
			break;
		case VulkanGpuAllocationResourceKind::Image:
			vmaDestroyImage(m_impl->Allocator, record.Image, record.Allocation);
			break;
		case VulkanGpuAllocationResourceKind::Unknown:
		default:
			vmaFreeMemory(m_impl->Allocator, record.Allocation);
			break;
	}
}

void VulkanGpuMemoryAllocator::DestroyMemoryBlock(VulkanGpuMemoryBlockRecord& record) noexcept
{
	if (record.Allocation != nullptr)
	{
		vmaFreeMemory(m_impl->Allocator, record.Allocation);
	}
}

void VulkanGpuMemoryAllocator::SetAllocationDebugName(VulkanGpuAllocationRecord& record, std::wstring_view debugName) noexcept
{
	const std::string narrowName = Strings::ToNarrow(debugName);
	if (record.ParentMemoryBlock == nullptr && record.Allocation != nullptr)
	{
		vmaSetAllocationName(m_impl->Allocator, record.Allocation, narrowName.empty() ? nullptr : narrowName.c_str());
	}

	PFN_vkSetDebugUtilsObjectNameEXT setObjectName = m_rhi.GetSetDebugUtilsObjectName();
	if (setObjectName == nullptr || narrowName.empty())
	{
		return;
	}

	if (record.Buffer != VK_NULL_HANDLE)
	{
		(void) VulkanDebugNames::SetObjectName(setObjectName, m_rhi.GetDevice(), VK_OBJECT_TYPE_BUFFER, reinterpret_cast<std::uint64_t>(record.Buffer), narrowName);
	}
	if (record.Image != VK_NULL_HANDLE)
	{
		(void) VulkanDebugNames::SetObjectName(setObjectName, m_rhi.GetDevice(), VK_OBJECT_TYPE_IMAGE, reinterpret_cast<std::uint64_t>(record.Image), narrowName);
	}
	if (record.AccelerationStructure != VK_NULL_HANDLE)
	{
		(void) VulkanDebugNames::SetObjectName(setObjectName, m_rhi.GetDevice(), VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR, reinterpret_cast<std::uint64_t>(record.AccelerationStructure), narrowName);
	}
}

void VulkanGpuMemoryAllocator::SetMemoryBlockDebugName(VulkanGpuMemoryBlockRecord& record, std::wstring_view debugName) noexcept
{
	if (record.Allocation == nullptr)
	{
		return;
	}

	const std::string narrowName = Strings::ToNarrow(debugName);
	vmaSetAllocationName(m_impl->Allocator, record.Allocation, narrowName.empty() ? nullptr : narrowName.c_str());
}

void VulkanGpuMemoryAllocator::RegisterAllocationRecord(VulkanGpuAllocationRecord& record) noexcept
{
	m_owner.AssertAccess();

	std::scoped_lock lock(m_impl->RecordsMutex);
	if (std::find(m_impl->LiveRecords.begin(), m_impl->LiveRecords.end(), &record) == m_impl->LiveRecords.end())
	{
		m_impl->LiveRecords.push_back(&record);
	}
}

void VulkanGpuMemoryAllocator::UnregisterAllocationRecord(VulkanGpuAllocationRecord& record) noexcept
{
	m_owner.AssertAccess();

	std::scoped_lock lock(m_impl->RecordsMutex);
	auto eraseBegin = std::remove(m_impl->LiveRecords.begin(), m_impl->LiveRecords.end(), &record);
	m_impl->LiveRecords.erase(eraseBegin, m_impl->LiveRecords.end());
}

void VulkanGpuMemoryAllocator::RegisterMemoryBlockRecord(VulkanGpuMemoryBlockRecord& record) noexcept
{
	m_owner.AssertAccess();

	std::scoped_lock lock(m_impl->RecordsMutex);
	const bool bMemoryBlockNotRegistered = std::find(m_impl->LiveMemoryBlockRecords.begin(), m_impl->LiveMemoryBlockRecords.end(), &record) == m_impl->LiveMemoryBlockRecords.end();

	if (bMemoryBlockNotRegistered)
	{
		m_impl->LiveMemoryBlockRecords.push_back(&record);
	}
}

void VulkanGpuMemoryAllocator::UnregisterMemoryBlockRecord(VulkanGpuMemoryBlockRecord& record) noexcept
{
	m_owner.AssertAccess();

	std::scoped_lock lock(m_impl->RecordsMutex);
	auto eraseBegin = std::remove(m_impl->LiveMemoryBlockRecords.begin(), m_impl->LiveMemoryBlockRecords.end(), &record);
	m_impl->LiveMemoryBlockRecords.erase(eraseBegin, m_impl->LiveMemoryBlockRecords.end());
}

VkResult VulkanGpuMemoryAllocator::AllocateMemoryBlock(VulkanGpuMemoryBlockRecord& memoryBlock, VkMemoryRequirements& memoryRequirements) noexcept
{
	memoryRequirements.size = std::max(memoryRequirements.size, static_cast<VkDeviceSize>(memoryBlock.RequestedSizeInBytes));
	memoryRequirements.alignment = std::max(memoryRequirements.alignment, static_cast<VkDeviceSize>(memoryBlock.Alignment));
	const VmaAllocationCreateInfo allocationCreateInfo{.flags = VMA_ALLOCATION_CREATE_CAN_ALIAS_BIT, .preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT};
	return vmaAllocateMemory(m_impl->Allocator, &memoryRequirements, &allocationCreateInfo, &memoryBlock.Allocation, nullptr);
}

bool VulkanGpuMemoryAllocator::CompleteMemoryBlockAllocation(VulkanGpuMemoryBlockRecord& memoryBlock, VkResult allocationResult) noexcept
{
	if (!VulkanResult::Succeeded(allocationResult) || memoryBlock.Allocation == nullptr)
	{
		memoryBlock.Allocation = nullptr;
		return false;
	}

	VmaAllocationInfo allocationInfo = {};
	vmaGetAllocationInfo(m_impl->Allocator, memoryBlock.Allocation, &allocationInfo);
	memoryBlock.AllocatedBytes = allocationInfo.size;
	SetMemoryBlockDebugName(memoryBlock, memoryBlock.DebugName);
	return true;
}

bool VulkanGpuMemoryAllocator::EnsureMemoryBlockAllocationForImage(VulkanGpuMemoryBlockRecord& memoryBlock, const VkImageCreateInfo& imageCreateInfo) noexcept
{
	if (memoryBlock.Allocation != nullptr)
	{
		return true;
	}

	VkImage image = VK_NULL_HANDLE;
	const VkResult imageResult = vkCreateImage(m_rhi.GetDevice(), &imageCreateInfo, nullptr, &image);
	if (!VulkanResult::Succeeded(imageResult) || image == VK_NULL_HANDLE)
	{
		return false;
	}

	VkMemoryRequirements memoryRequirements = {};
	vkGetImageMemoryRequirements(m_rhi.GetDevice(), image, &memoryRequirements);
	const VkResult allocationResult = AllocateMemoryBlock(memoryBlock, memoryRequirements);
	vkDestroyImage(m_rhi.GetDevice(), image, nullptr);
	return CompleteMemoryBlockAllocation(memoryBlock, allocationResult);
}

bool VulkanGpuMemoryAllocator::EnsureMemoryBlockAllocationForBuffer(VulkanGpuMemoryBlockRecord& memoryBlock, const VkBufferCreateInfo& bufferCreateInfo) noexcept
{
	if (memoryBlock.Allocation != nullptr)
	{
		return true;
	}

	VkBuffer buffer = VK_NULL_HANDLE;
	const VkResult bufferResult = vkCreateBuffer(m_rhi.GetDevice(), &bufferCreateInfo, nullptr, &buffer);
	if (!VulkanResult::Succeeded(bufferResult) || buffer == VK_NULL_HANDLE)
	{
		return false;
	}

	VkMemoryRequirements memoryRequirements = {};
	vkGetBufferMemoryRequirements(m_rhi.GetDevice(), buffer, &memoryRequirements);
	const VkResult allocationResult = AllocateMemoryBlock(memoryBlock, memoryRequirements);
	vkDestroyBuffer(m_rhi.GetDevice(), buffer, nullptr);
	return CompleteMemoryBlockAllocation(memoryBlock, allocationResult);
}

std::unique_ptr<VulkanGpuAllocationRecord> VulkanGpuMemoryAllocator::CreateAllocationRecord(
    VulkanGpuAllocationResourceKind resourceKind,
    VkBuffer buffer,
    VkImage image,
    VmaAllocation allocation,
    RhiMemoryCategory category,
    RhiMemoryResidencyClass residencyClass,
    std::wstring_view debugName) noexcept
{
	VmaAllocationInfo allocationInfo = {};
	vmaGetAllocationInfo(m_impl->Allocator, allocation, &allocationInfo);

	auto record = std::make_unique<VulkanGpuAllocationRecord>();
	record->Buffer = buffer;
	record->Image = image;
	record->Allocation = allocation;
	record->ResourceKind = resourceKind;
	record->Category = category;
	record->ResidencyClass = residencyClass;
	record->UsedBytes = allocationInfo.size;
	record->AllocatedBytes = allocationInfo.size;
	record->ResourceSizeInBytes = allocationInfo.size;
	record->Owner = this;
	record->DebugName = std::wstring(debugName);
	SetAllocationDebugName(*record, debugName);
	RegisterAllocationRecord(*record);
	return record;
}

std::unique_ptr<VulkanGpuAllocationRecord> VulkanGpuMemoryAllocator::CreateAliasingAllocationRecord(
    VulkanGpuAllocationResourceKind resourceKind,
    VkBuffer buffer,
    VkImage image,
    VulkanGpuMemoryBlockRecord& memoryBlock,
    std::wstring_view debugName) noexcept
{
	auto record = std::make_unique<VulkanGpuAllocationRecord>();
	record->Buffer = buffer;
	record->Image = image;
	record->Allocation = memoryBlock.Allocation;
	record->ParentMemoryBlock = &memoryBlock;
	record->ResourceKind = resourceKind;
	record->Category = memoryBlock.Category;
	record->ResidencyClass = memoryBlock.ResidencyClass;
	record->AllocatedBytes = 0;

	record->Owner = this;
	record->DebugName = std::wstring(debugName);
	++memoryBlock.AliasingResourceCount;
	SetAllocationDebugName(*record, debugName);
	RegisterAllocationRecord(*record);
	return record;
}

VkImageAspectFlags VulkanGpuMemoryAllocator::ResolveImageAspectMask(VkFormat format) noexcept
{
	switch (format)
	{
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_D32_SFLOAT:
			return VK_IMAGE_ASPECT_DEPTH_BIT;
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		default:
			return VK_IMAGE_ASPECT_COLOR_BIT;
	}
}
