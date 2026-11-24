#pragma once

#include "RHI/Public/Descriptors/RhiDescriptorHandles.h"
#include "RHI/Public/Resources/RhiResourceDesc.h"
#include "RHI/Public/Resources/RhiResourceHandles.h"
#include "RHI/Public/Resources/RhiResourceView.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

class RenderHardwareInterface;
class RenderCommandList;

class GpuMeshBuffer final
{
public:
	GpuMeshBuffer() noexcept;
	~GpuMeshBuffer() noexcept;

	GpuMeshBuffer(const GpuMeshBuffer&) = delete;
	GpuMeshBuffer& operator=(const GpuMeshBuffer&) = delete;
	GpuMeshBuffer(GpuMeshBuffer&&) = delete;
	GpuMeshBuffer& operator=(GpuMeshBuffer&&) = delete;

	bool Upload(
	    RenderHardwareInterface& renderHardwareInterface,
	    RenderCommandList& commandList,
	    std::span<const std::byte> payload,
	    RhiBufferKind kind,
	    std::uint32_t strideInBytes,
	    std::wstring_view debugName,
	    std::wstring_view uploadDebugName);

	RhiOwnedResourceHandle GetResource() const noexcept { return m_resource; }

	RhiGpuDescriptorHandle GetShaderResourceView() const noexcept { return m_shaderResourceView; }

private:
	void Release() noexcept;

	RenderHardwareInterface* m_renderHardwareInterface = nullptr;
	RhiOwnedResourceHandle m_resource = {};
	RhiResourceViewHandle m_view = {};
	RhiGpuDescriptorHandle m_shaderResourceView = {};
};
