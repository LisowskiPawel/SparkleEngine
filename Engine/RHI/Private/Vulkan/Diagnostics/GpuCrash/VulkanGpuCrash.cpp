#include "Vulkan/VulkanPCH.h"
#include "Vulkan/Diagnostics/GpuCrash/VulkanGpuCrash.h"
#include "Vulkan/Device/VulkanRhi.h"

#include "Core/Public/Json/JsonWriter.h"

#include <algorithm>
#include <array>
#include <cstring>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(LogVulkanGpuCrash, "RHI.Vulkan.GpuCrash");

namespace VulkanGpuCrash
{
	PFN_vkCmdSetCheckpointNV SetCheckpoint = nullptr;
	PFN_vkGetQueueCheckpointDataNV GetCheckpoints = nullptr;
}

VulkanGpuCrashConfiguration::VulkanGpuCrashConfiguration(VkPhysicalDevice physicalDevice, std::vector<const char*>& extensions)
{
	if (GpuCrashSession::Current() == nullptr)
	{
		return;
	}
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);
	if (properties.vendorID != 0x10de)
	{
		Diagnostics::Fatal(LogVulkanGpuCrash, __FILE__, __LINE__, "Aftermath requires an NVIDIA Vulkan device.");
	}
	std::uint32_t count = 0;
	if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr) != VK_SUCCESS)
	{
		Diagnostics::Fatal(LogVulkanGpuCrash, __FILE__, __LINE__, "Cannot query Aftermath Vulkan extensions.");
	}
	std::vector<VkExtensionProperties> available(count);
	if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, available.data()) != VK_SUCCESS)
	{
		Diagnostics::Fatal(LogVulkanGpuCrash, __FILE__, __LINE__, "Cannot read Aftermath Vulkan extensions.");
	}
	const std::array required{VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME, VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME};
	for (const char* name : required)
	{
		if (!std::ranges::any_of(available, [name](const auto& extension) { return std::strcmp(name, extension.extensionName) == 0; }))
		{
			Diagnostics::Fatal(LogVulkanGpuCrash, __FILE__, __LINE__, "Required Aftermath Vulkan extension is unavailable.");
		}
		extensions.push_back(name);
	}
	VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &m_features};
	vkGetPhysicalDeviceFeatures2(physicalDevice, &features);
	if (m_features.diagnosticsConfig != VK_TRUE)
	{
		Diagnostics::Fatal(LogVulkanGpuCrash, __FILE__, __LINE__, "Vulkan diagnosticsConfig is unavailable.");
	}

	m_config.flags = VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_SHADER_DEBUG_INFO_BIT_NV | VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_RESOURCE_TRACKING_BIT_NV
	    | VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_SHADER_ERROR_REPORTING_BIT_NV;

	m_enabled = true;
}

void VulkanGpuCrashConfiguration::Attach(VkDeviceCreateInfo& device, VkPhysicalDeviceFeatures2& features) noexcept
{
	if (m_enabled)
	{
		m_features.pNext = features.pNext;
		features.pNext = &m_features;
		m_config.pNext = device.pNext;
		device.pNext = &m_config;
	}
}

void VulkanGpuCrash::Initialize(VkDevice device)
{
	if (auto* session = GpuCrashSession::Current())
	{
		SetCheckpoint = reinterpret_cast<PFN_vkCmdSetCheckpointNV>(vkGetDeviceProcAddr(device, "vkCmdSetCheckpointNV"));
		GetCheckpoints = reinterpret_cast<PFN_vkGetQueueCheckpointDataNV>(vkGetDeviceProcAddr(device, "vkGetQueueCheckpointDataNV"));
		if (SetCheckpoint == nullptr || GetCheckpoints == nullptr)
		{
			Diagnostics::Fatal(LogVulkanGpuCrash, __FILE__, __LINE__, "Requested Vulkan checkpoint functions are unavailable.");
		}
		session->Ready();
	}
}

void VulkanGpuCrash::Checkpoint(VkCommandBuffer commands, const void* token) noexcept
{
	if (SetCheckpoint != nullptr && commands != VK_NULL_HANDLE && token != nullptr)
	{
		SetCheckpoint(commands, token);
	}
}

void VulkanGpuCrash::Shader(std::span<const std::byte> code) noexcept
{
	if (auto* session = GpuCrashSession::Current())
	{
		const GFSDK_Aftermath_SpirvCode spirv{code.data(), static_cast<std::uint32_t>(code.size())};
		const auto hash = reinterpret_cast<PFN_GFSDK_Aftermath_GetShaderHashSpirv>(session->RequireExport("GFSDK_Aftermath_GetShaderHashSpirv"));
		GFSDK_Aftermath_ShaderBinaryHash id{};
		const bool hasHash = hash(GFSDK_Aftermath_Version_API, &spirv, &id) == GFSDK_Aftermath_Result_Success;
		// The SDK debug-name API requires a qualified full/stripped pair, which this cook does not emit.
		session->ShaderIdentity(code, id.hash, hasHash, nullptr);
	}
}

void VulkanGpuCrash::ObserveLoss(VulkanRhi& rhi, ERhiQueueType observedQueue, VkResult result) noexcept
{
	auto* session = GpuCrashSession::Current();
	if (session == nullptr || result != VK_ERROR_DEVICE_LOST || !session->BeginLoss(result))
	{
		return;
	}
	try
	{
		Json::ObjectWriter json;
		json.WriteUInt64("cpuObservedQueue", static_cast<std::uint64_t>(observedQueue));
		json.WriteRaw("frame", "null");
		json.WriteRaw("recording", "null");
		json.WriteRaw("resourceAliases", "null");
		std::string queues = "[";
		std::array<VkCheckpointDataNV, 64> data{};
		for (auto& item : data)
		{
			item.sType = VK_STRUCTURE_TYPE_CHECKPOINT_DATA_NV;
		}
		for (std::size_t queueIndex = 0; queueIndex < RhiQueueTypeCount; ++queueIndex)
		{
			const auto queueType = static_cast<ERhiQueueType>(queueIndex);
			const auto queue = rhi.GetQueue(queueType);
			if (queue == VK_NULL_HANDLE || GetCheckpoints == nullptr)
			{
				continue;
			}
			Json::ObjectWriter observation;
			observation.WriteUInt64("logicalQueue", queueIndex);
			observation.WriteHexUInt64("nativeQueue", reinterpret_cast<std::uint64_t>(queue));
			std::string checkpoints = "[";
			std::uint32_t available = 0;
			GetCheckpoints(queue, &available, nullptr);
			std::uint32_t count = std::min(available, static_cast<std::uint32_t>(data.size()));
			GetCheckpoints(queue, &count, data.data());
			if (available > data.size())
			{
				session->Incomplete();
			}
			for (std::size_t index = 0; index < std::min<std::size_t>(count, data.size()); ++index)
			{
				Json::ObjectWriter checkpoint;
				checkpoint.WriteUInt64("stage", data[index].stage);
				checkpoint.WriteString("semanticScope", session->MarkerText(data[index].pCheckpointMarker));
				if (checkpoints.size() > 1)
				{
					checkpoints += ',';
				}
				checkpoints += checkpoint.Finish();
			}
			checkpoints += ']';
			observation.WriteUInt64("reportedCount", available);
			observation.WriteRaw("truncated", available > data.size() ? "true" : "false");
			observation.WriteRaw("checkpoints", checkpoints);
			if (queues.size() > 1)
			{
				queues += ',';
			}
			queues += observation.Finish();
		}
		queues += ']';
		json.WriteRaw("queues", queues);
		const auto text = json.Finish();
		session->Enqueue({RhiGpuCrashArtifactKind::NativeDiagnostics, 1, 0}, std::as_bytes(std::span(text)));
	}
	catch (...)
	{
		session->Incomplete();
	}
	session->FinishLoss(result);
}
