#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Device/VulkanRhi.h"

#include "Vulkan/Commands/VulkanCommandQueue.h"

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_vulkanRhiLogger, "RHI.Vulkan");

VkQueue VulkanRhi::GetGraphicsQueue() const noexcept
{
	return GetQueue(ERhiQueueType::Graphics);
}

VkQueue VulkanRhi::GetQueue(ERhiQueueType queueType) const noexcept
{
	const std::unique_ptr<VulkanCommandQueue>& queue = m_queues[RhiQueueTypeToIndex(queueType)];
	return queue != nullptr ? queue->GetNativeQueue() : VK_NULL_HANDLE;
}

VulkanCommandQueue& VulkanRhi::GetCommandQueue(ERhiQueueType queueType) noexcept
{
	return *m_queues[RhiQueueTypeToIndex(queueType)];
}

const VulkanCommandQueue& VulkanRhi::GetCommandQueue(ERhiQueueType queueType) const noexcept
{
	return *m_queues[RhiQueueTypeToIndex(queueType)];
}

std::uint32_t VulkanRhi::GetGraphicsQueueFamilyIndex() const noexcept
{
	return GetQueueFamilyIndex(ERhiQueueType::Graphics);
}

std::uint32_t VulkanRhi::GetQueueFamilyIndex(ERhiQueueType queueType) const noexcept
{
	return m_queueTopology.Get(queueType).FamilyIndex;
}

std::uint32_t VulkanRhi::GetQueueIndex(ERhiQueueType queueType) const noexcept
{
	return m_queueTopology.Get(queueType).QueueIndex;
}

bool VulkanRhi::HasIndependentQueue(ERhiQueueType queueType) const noexcept
{
	return m_queueTopology.HasIndependentQueue(queueType);
}

void VulkanRhi::ConfigureResourceQueueSharing(VkBufferCreateInfo& createInfo) const noexcept
{
	const std::span<const std::uint32_t> familyIndices = m_queueTopology.GetFamilyIndices();
	if (familyIndices.size() <= 1)
	{
		return;
	}
	createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	createInfo.queueFamilyIndexCount = static_cast<std::uint32_t>(familyIndices.size());
	createInfo.pQueueFamilyIndices = familyIndices.data();
}

void VulkanRhi::ConfigureResourceQueueSharing(VkImageCreateInfo& createInfo) const noexcept
{
	const std::span<const std::uint32_t> familyIndices = m_queueTopology.GetFamilyIndices();
	if (familyIndices.size() <= 1)
	{
		return;
	}
	createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	createInfo.queueFamilyIndexCount = static_cast<std::uint32_t>(familyIndices.size());
	createInfo.pQueueFamilyIndices = familyIndices.data();
}

void VulkanRhi::ConfigureResourceQueueSharing(VkSwapchainCreateInfoKHR& createInfo) const noexcept
{
	const std::span<const std::uint32_t> familyIndices = m_queueTopology.GetFamilyIndices();
	if (familyIndices.size() <= 1)
	{
		return;
	}
	createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	createInfo.queueFamilyIndexCount = static_cast<std::uint32_t>(familyIndices.size());
	createInfo.pQueueFamilyIndices = familyIndices.data();
}

void VulkanRhi::CreateCommandQueues() noexcept
{
	std::array<std::shared_ptr<VulkanNativeQueue>, RhiQueueTypeCount> nativeQueues{};
	for (std::size_t queueIndex = 0; queueIndex < RhiQueueTypeCount; ++queueIndex)
	{
		const ERhiQueueType queueType = static_cast<ERhiQueueType>(queueIndex);
		const VulkanQueueLocation& queueLocation = m_queueTopology.Get(queueType);
		for (std::size_t previousQueueIndex = 0; previousQueueIndex < queueIndex; ++previousQueueIndex)
		{
			if (m_queueTopology.Get(static_cast<ERhiQueueType>(previousQueueIndex)) == queueLocation)
			{
				nativeQueues[queueIndex] = nativeQueues[previousQueueIndex];
				break;
			}
		}
		if (nativeQueues[queueIndex] == nullptr)
		{
			VkQueue nativeQueue = VK_NULL_HANDLE;
			vkGetDeviceQueue(m_device, queueLocation.FamilyIndex, queueLocation.QueueIndex, &nativeQueue);
			nativeQueues[queueIndex] = std::make_shared<VulkanNativeQueue>();
			*nativeQueues[queueIndex] = nativeQueue;
		}
		m_queues[queueIndex] = std::make_unique<VulkanCommandQueue>(*this, queueType, nativeQueues[queueIndex]);
	}
	if (GetGraphicsQueue() == VK_NULL_HANDLE)
	{
		Diagnostics::Fatal(g_vulkanRhiLogger, __FILE__, __LINE__, "vkGetDeviceQueue returned a null graphics queue.");
	}
}
