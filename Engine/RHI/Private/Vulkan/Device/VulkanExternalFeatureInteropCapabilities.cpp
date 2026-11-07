#include "Vulkan/VulkanPCH.h"

#include "Vulkan/Device/VulkanExternalFeatureInteropCapabilities.h"

#include "Vulkan/Device/VulkanRhi.h"

class VulkanExternalFeatureAdapterIdentity final
{
public:
	static RhiAdapterIdentity BuildVulkanAdapterIdentity(const VulkanRhi* rhi)
	{
		if (rhi == nullptr)
		{
			return {};
		}

		const VulkanAdapterInfo& adapter = rhi->GetAdapterInfo();
		return RhiAdapterIdentity{.Name = adapter.Name, .DriverDescription = adapter.Driver, .VendorId = adapter.VendorId, .DeviceId = adapter.DeviceId};
	}
};

RhiExternalFeatureInteropCapabilities BuildVulkanExternalFeatureInteropCapabilities(const VulkanRhi* rhi, bool bHasGraphicsCommandContext) noexcept
{
	RhiExternalFeatureInteropCapabilities capabilities{};
	capabilities.BridgeKind = ERhiExternalFeatureBridgeKind::None;
	capabilities.Adapter = VulkanExternalFeatureAdapterIdentity::BuildVulkanAdapterIdentity(rhi);
	capabilities.bExposesNativeDevice = rhi != nullptr && rhi->GetDevice() != VK_NULL_HANDLE;
	capabilities.bExposesNativeGraphicsQueue = rhi != nullptr && rhi->GetGraphicsQueue() != VK_NULL_HANDLE;
	capabilities.bExposesNativeGraphicsCommandList = bHasGraphicsCommandContext;
	capabilities.bExposesNativeResources = true;
	capabilities.bSupportsExplicitResourceStates = true;
	capabilities.bSupportsExternalProviderEvaluation = false;
	capabilities.bSupportsRuntimeProviderChecks = false;
	return capabilities;
}
