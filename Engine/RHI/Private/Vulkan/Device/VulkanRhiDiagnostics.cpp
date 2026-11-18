#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Device/VulkanRhi.h"

#include "Vulkan/Commands/VulkanCommandQueue.h"
#include "Vulkan/Diagnostics/VulkanDebugNames.h"

#include <format>
#include <utility>

bool VulkanRhi::TryPopDiagnosticMessage(RhiDiagnosticMessage& outMessage) noexcept
{
	return m_diagnosticMessageQueue.TryPop(outMessage);
}

void VulkanRhi::ClearDiagnosticMessages() noexcept
{
	m_diagnosticMessageQueue.Clear();
}

PFN_vkSetDebugUtilsObjectNameEXT VulkanRhi::GetSetDebugUtilsObjectName() const noexcept
{
	return m_setDebugUtilsObjectName;
}

PFN_vkCmdBeginDebugUtilsLabelEXT VulkanRhi::GetCmdBeginDebugUtilsLabel() const noexcept
{
	return m_cmdBeginDebugUtilsLabel;
}

PFN_vkCmdEndDebugUtilsLabelEXT VulkanRhi::GetCmdEndDebugUtilsLabel() const noexcept
{
	return m_cmdEndDebugUtilsLabel;
}

PFN_vkCmdInsertDebugUtilsLabelEXT VulkanRhi::GetCmdInsertDebugUtilsLabel() const noexcept
{
	return m_cmdInsertDebugUtilsLabel;
}

void VulkanRhi::LoadDeviceDebugFunctions() noexcept
{
	if (m_device == VK_NULL_HANDLE)
	{
		return;
	}

	m_setDebugUtilsObjectName = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(m_device, "vkSetDebugUtilsObjectNameEXT"));
	m_cmdBeginDebugUtilsLabel = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetDeviceProcAddr(m_device, "vkCmdBeginDebugUtilsLabelEXT"));
	m_cmdEndDebugUtilsLabel = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetDeviceProcAddr(m_device, "vkCmdEndDebugUtilsLabelEXT"));
	m_cmdInsertDebugUtilsLabel = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(vkGetDeviceProcAddr(m_device, "vkCmdInsertDebugUtilsLabelEXT"));
}

void VulkanRhi::PushDiagnosticMessage(ERhiDiagnosticMessageSeverity severity, ERhiDiagnosticMessageCategory category, std::string text) noexcept
{
	m_diagnosticMessageQueue.Push(RhiDiagnosticMessage{.Severity = severity, .Category = category, .Text = std::move(text)});
}

VkBool32 VKAPI_PTR VulkanRhi::DebugUtilsCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData) noexcept
{
	auto* rhi = static_cast<VulkanRhi*>(userData);
	if (rhi == nullptr || callbackData == nullptr)
	{
		return VK_FALSE;
	}

	ERhiDiagnosticMessageSeverity sparkleSeverity = ERhiDiagnosticMessageSeverity::Info;
	if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0)
	{
		sparkleSeverity = ERhiDiagnosticMessageSeverity::Error;
	}
	else if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) != 0)
	{
		sparkleSeverity = ERhiDiagnosticMessageSeverity::Warning;
	}
	else if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) != 0)
	{
		sparkleSeverity = ERhiDiagnosticMessageSeverity::Verbose;
	}

	ERhiDiagnosticMessageCategory sparkleCategory = ERhiDiagnosticMessageCategory::General;
	if ((messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) != 0)
	{
		sparkleCategory = ERhiDiagnosticMessageCategory::Validation;
	}
	else if ((messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) != 0)
	{
		sparkleCategory = ERhiDiagnosticMessageCategory::Performance;
	}

	const char* const message = callbackData->pMessage != nullptr ? callbackData->pMessage : "";
	rhi->PushDiagnosticMessage(sparkleSeverity, sparkleCategory, message);
	return VK_FALSE;
}

void VulkanRhi::NameBootstrapObjects() noexcept
{
	if (m_setDebugUtilsObjectName == nullptr || m_device == VK_NULL_HANDLE)
	{
		return;
	}

	(void) VulkanDebugNames::SetObjectName(m_setDebugUtilsObjectName, m_device, VK_OBJECT_TYPE_DEVICE, reinterpret_cast<std::uint64_t>(m_device), "Sparkle Vulkan Device");
	for (std::size_t queueIndex = 0; queueIndex < RhiQueueTypeCount; ++queueIndex)
	{
		const ERhiQueueType queueType = static_cast<ERhiQueueType>(queueIndex);
		const VulkanCommandQueue& queue = *m_queues[queueIndex];
		if (queue.GetNativeQueue() == VK_NULL_HANDLE)
		{
			continue;
		}
		const std::string queueName = std::format("Sparkle Vulkan {} Queue", RhiQueueTypeToString(queueType));
		(void) VulkanDebugNames::SetObjectName(m_setDebugUtilsObjectName, m_device, VK_OBJECT_TYPE_QUEUE, reinterpret_cast<std::uint64_t>(queue.GetNativeQueue()), queueName);
		const std::string timelineName = std::format("Sparkle Vulkan {} Queue Timeline", RhiQueueTypeToString(queueType));
		(void) VulkanDebugNames::SetObjectName(m_setDebugUtilsObjectName, m_device, VK_OBJECT_TYPE_SEMAPHORE, reinterpret_cast<std::uint64_t>(queue.GetTimelineSemaphore()), timelineName);
	}
}

VulkanAdapterInfo VulkanRhi::BuildAdapterInfo(const VkPhysicalDeviceProperties& properties)
{
	return VulkanAdapterInfo{
	    .Name = properties.deviceName,
	    .Driver = FormatDriverVersion(properties),
	    .Vendor = std::format("{:#06x}", properties.vendorID),
	    .VendorId = properties.vendorID,
	    .DeviceId = properties.deviceID,
	    .ApiVersion = properties.apiVersion,
	    .DriverVersion = properties.driverVersion,
	    .DeviceType = properties.deviceType};
}

std::string VulkanRhi::FormatApiVersion(std::uint32_t version)
{
	return std::format("{}.{}.{}", VK_API_VERSION_MAJOR(version), VK_API_VERSION_MINOR(version), VK_API_VERSION_PATCH(version));
}

std::string VulkanRhi::FormatDriverVersion(const VkPhysicalDeviceProperties& properties)
{
	return std::format("{} ({:#010x})", FormatApiVersion(properties.driverVersion), properties.driverVersion);
}
