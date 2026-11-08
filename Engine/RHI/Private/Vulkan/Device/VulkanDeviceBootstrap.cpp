#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Device/VulkanRhi.h"

#include "Vulkan/Device/VulkanDeviceCreation.h"

#include "CVars/RHICVars.h"
#include "Vulkan/Core/VulkanResult.h"
#include "Core/Public/Diagnostics/Verify.h"

#include <algorithm>
#include <cstring>
#include <utility>
#include <span>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_vulkanRhiLogger, "RHI.Vulkan");

void VulkanRhi::CreateInstance() noexcept
{
	std::uint32_t loaderApiVersion = VK_API_VERSION_1_0;
	if (vkEnumerateInstanceVersion != nullptr)
	{
		(void) vkEnumerateInstanceVersion(&loaderApiVersion);
	}
	if (loaderApiVersion < VK_API_VERSION_1_3)
	{
		Diagnostics::Fatal(g_vulkanRhiLogger, __FILE__, __LINE__, "Vulkan 1.3 is required by the engine's SPIR-V 1.6 runtime shader contract.");
	}

	std::vector<const char*> instanceExtensions;
	if (IsInstanceExtensionAvailable(VK_KHR_SURFACE_EXTENSION_NAME))
	{
		instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	}
	if (IsInstanceExtensionAvailable(VK_KHR_WIN32_SURFACE_EXTENSION_NAME))
	{
		instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	}

#if ENGINE_GPU_VALIDATION
	if (IsLayerAvailable("VK_LAYER_KHRONOS_validation"))
	{
		m_enabledLayers.emplace_back("VK_LAYER_KHRONOS_validation");
		m_bValidationEnabled = true;
	}
	else
	{
		SPDLOG_LOGGER_WARN(g_vulkanRhiLogger, "Vulkan validation requested, but VK_LAYER_KHRONOS_validation is unavailable.");
	}

	if (IsInstanceExtensionAvailable(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
	{
		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	else
	{
		SPDLOG_LOGGER_WARN(g_vulkanRhiLogger, "Vulkan debug utils extension is unavailable.");
	}
#endif

	for (const char* extension : instanceExtensions)
	{
		m_enabledInstanceExtensions.emplace_back(extension);
	}

	std::vector<const char*> layerNames;
	layerNames.reserve(m_enabledLayers.size());
	for (const std::string& layer : m_enabledLayers)
	{
		layerNames.push_back(layer.c_str());
	}

	const VkApplicationInfo applicationInfo{
	    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
	    .pNext = nullptr,
	    .pApplicationName = "Sparkle",
	    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
	    .pEngineName = "SparkleEngine",
	    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
	    .apiVersion = VK_API_VERSION_1_3};

	const VkInstanceCreateInfo createInfo{
	    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	    .pNext = nullptr,
	    .flags = 0,
	    .pApplicationInfo = &applicationInfo,
	    .enabledLayerCount = static_cast<std::uint32_t>(layerNames.size()),
	    .ppEnabledLayerNames = layerNames.empty() ? nullptr : layerNames.data(),
	    .enabledExtensionCount = static_cast<std::uint32_t>(instanceExtensions.size()),
	    .ppEnabledExtensionNames = instanceExtensions.empty() ? nullptr : instanceExtensions.data()};

	const VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
	if (!VulkanResult::Succeeded(result))
	{
		Diagnostics::Fatal(g_vulkanRhiLogger, __FILE__, __LINE__, VulkanResult::FormatFailure("vkCreateInstance", result));
	}
}

void VulkanRhi::CreateDebugMessenger() noexcept
{
	m_debugLayer.Initialize(m_instance, &VulkanRhi::DebugUtilsCallback, this);
}

struct PhysicalDeviceCandidate final
{
	VkPhysicalDevice Device = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties Properties = {};
	VkPhysicalDeviceFeatures2 Features = {};
	VkPhysicalDeviceVulkan12Features Features12 = {};
	VkPhysicalDeviceVulkan13Features Features13 = {};
	VulkanQueueTopology QueueTopology;
	std::uint32_t Score = 0;
};

static std::uint32_t ScorePhysicalDevice(const VkPhysicalDeviceProperties& properties) noexcept
{
	if (CVarPreferHighPerformanceAdapter.Get())
	{
		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			return 300;
		}
		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{
			return 200;
		}
	}
	else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
	{
		return 300;
	}

	return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU ? 10u : 100u;
}

static std::vector<VkPhysicalDevice> EnumeratePhysicalDevices(VkInstance instance)
{
	std::uint32_t deviceCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (!VulkanResult::Succeeded(result) || deviceCount == 0)
	{
		Diagnostics::Fatal(g_vulkanRhiLogger, __FILE__, __LINE__, "No Vulkan physical devices were found.");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	result = vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	if (!VulkanResult::Succeeded(result))
	{
		Diagnostics::Fatal(g_vulkanRhiLogger, __FILE__, __LINE__, VulkanResult::FormatFailure("vkEnumeratePhysicalDevices", result));
	}

	return devices;
}

static bool InspectPhysicalDevice(VkPhysicalDevice device, PhysicalDeviceCandidate& candidate)
{
	candidate.Device = device;
	candidate.Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	candidate.Features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	candidate.Features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	vkGetPhysicalDeviceProperties(device, &candidate.Properties);
	if (candidate.Properties.apiVersion < VK_API_VERSION_1_3)
	{
		return false;
	}
	candidate.Features.pNext = &candidate.Features12;
	candidate.Features12.pNext = &candidate.Features13;
	vkGetPhysicalDeviceFeatures2(device, &candidate.Features);
	if (candidate.Features.features.shaderStorageImageReadWithoutFormat != VK_TRUE || candidate.Features.features.shaderStorageImageWriteWithoutFormat != VK_TRUE
	    || candidate.Features12.scalarBlockLayout != VK_TRUE)
	{
		return false;
	}
	if (candidate.Features12.timelineSemaphore != VK_TRUE)
	{
		return false;
	}
	candidate.Features.pNext = nullptr;
	candidate.Features12.pNext = nullptr;
	candidate.QueueTopology = VulkanQueueTopology::Select(device);
	const bool bRequiredQueueMissingQueueTopology = !candidate.QueueTopology.Supports(ERhiQueueType::Graphics) || !candidate.QueueTopology.Supports(ERhiQueueType::Compute);
	const bool bRequiredQueueMissing = bRequiredQueueMissingQueueTopology || (!candidate.QueueTopology.Supports(ERhiQueueType::Copy));

	if (bRequiredQueueMissing)
	{
		return false;
	}
	candidate.Score = ScorePhysicalDevice(candidate.Properties);
	return true;
}

static std::vector<PhysicalDeviceCandidate> CollectEligibleDevices(std::span<const VkPhysicalDevice> devices)
{
	std::vector<PhysicalDeviceCandidate> candidates;
	for (VkPhysicalDevice device : devices)
	{
		PhysicalDeviceCandidate candidate;
		if (InspectPhysicalDevice(device, candidate))
		{
			candidates.push_back(std::move(candidate));
		}
	}
	return candidates;
}

static void ApplySupportedFeatures(const PhysicalDeviceCandidate& selected, VulkanFeatureStatus& status) noexcept
{
	status.bSupportsSynchronization2 = selected.Features13.synchronization2 == VK_TRUE;
	status.bSupportsTimelineSemaphore = selected.Features12.timelineSemaphore == VK_TRUE;
	status.bSupportsDynamicRendering = selected.Features13.dynamicRendering == VK_TRUE;
	status.bSupportsSamplerAnisotropy = selected.Features.features.samplerAnisotropy == VK_TRUE;
	status.bSupportsFillModeNonSolid = selected.Features.features.fillModeNonSolid == VK_TRUE;
	status.bSupportsShaderInt64 = selected.Features.features.shaderInt64 == VK_TRUE;
	status.bSupportsShaderFloat64 = selected.Features.features.shaderFloat64 == VK_TRUE;
	status.bSupportsScalarBlockLayout = selected.Features12.scalarBlockLayout == VK_TRUE;
	status.bSupportsHostQueryReset = selected.Features12.hostQueryReset == VK_TRUE;
	status.bSupportsStorageImageReadWithoutFormat = selected.Features.features.shaderStorageImageReadWithoutFormat == VK_TRUE;
	status.bSupportsStorageImageWriteWithoutFormat = selected.Features.features.shaderStorageImageWriteWithoutFormat == VK_TRUE;
	status.bNonUniformSampledImages = selected.Features12.shaderSampledImageArrayNonUniformIndexing == VK_TRUE;
	status.bSupportsPartiallyBoundDescriptorArrays = selected.Features12.descriptorBindingPartiallyBound == VK_TRUE;
	status.bSupportsShaderDemoteToHelperInvocation = selected.Features13.shaderDemoteToHelperInvocation == VK_TRUE;
}

void VulkanRhi::SelectPhysicalDevice() noexcept
{
	const std::vector<VkPhysicalDevice> devices = EnumeratePhysicalDevices(m_instance);
	std::vector<PhysicalDeviceCandidate> candidates = CollectEligibleDevices(devices);
	if (candidates.empty())
	{
		Diagnostics::Fatal(g_vulkanRhiLogger, __FILE__, __LINE__, "No Vulkan 1.3 physical device exposes a graphics queue, scalar block layout and formatless storage-image reads and writes.");
	}

	std::sort(candidates.begin(), candidates.end(), [](const PhysicalDeviceCandidate& lhs, const PhysicalDeviceCandidate& rhs) noexcept { return lhs.Score > rhs.Score; });

	PhysicalDeviceCandidate& selected = candidates.front();
	m_physicalDevice = selected.Device;
	m_queueTopology = std::move(selected.QueueTopology);
	m_adapterInfo = BuildAdapterInfo(selected.Properties);
	ApplySupportedFeatures(selected, m_featureStatus);
	m_featureStatus.RayTracing = VulkanRayTracingFeatureQuery::Query(m_physicalDevice);
}

void VulkanRhi::CreateLogicalDevice() noexcept
{
	m_device = CreateVulkanLogicalDevice(m_physicalDevice, m_queueTopology, m_adapterInfo.ApiVersion, m_adapterInfo.VendorId == NvidiaVendorId, m_featureStatus, m_enabledDeviceExtensions);
	CreateCommandQueues();
}

bool VulkanRhi::IsLayerAvailable(const char* layerName) noexcept
{
	std::uint32_t layerCount = 0;
	if (!VulkanResult::Succeeded(vkEnumerateInstanceLayerProperties(&layerCount, nullptr)))
	{
		return false;
	}

	std::vector<VkLayerProperties> layers(layerCount);
	if (!VulkanResult::Succeeded(vkEnumerateInstanceLayerProperties(&layerCount, layers.data())))
	{
		return false;
	}

	return std::any_of(layers.begin(), layers.end(), [layerName](const VkLayerProperties& layer) noexcept { return std::string_view(layer.layerName) == layerName; });
}

bool VulkanRhi::IsInstanceExtensionAvailable(const char* extensionName) noexcept
{
	std::uint32_t extensionCount = 0;
	if (!VulkanResult::Succeeded(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr)))
	{
		return false;
	}

	std::vector<VkExtensionProperties> extensions(extensionCount);
	if (!VulkanResult::Succeeded(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data())))
	{
		return false;
	}

	return std::any_of(extensions.begin(), extensions.end(), [extensionName](const VkExtensionProperties& extension) noexcept { return std::string_view(extension.extensionName) == extensionName; });
}
