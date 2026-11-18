#pragma once

#include "Interop/RhiInteropService.h"
#include "RHI/Public/Interop/ResourceState.h"

class VulkanRenderHardwareInterface;

class VulkanInteropService final : public RhiInteropService
{
public:
	explicit VulkanInteropService(VulkanRenderHardwareInterface& owner) noexcept;

	RhiNativeDeviceQueueInterop GetDeviceQueueInterop(RhiNativeInteropRequest request) const noexcept override;
	NativeTextureViewInfo GetNativeTextureViewInfo(RhiResourceViewHandle view, RhiResourceHandle resource, ResourceState state, const RhiNativeInteropRequest& request) const noexcept override;

private:
	VulkanRenderHardwareInterface& m_owner;
};
