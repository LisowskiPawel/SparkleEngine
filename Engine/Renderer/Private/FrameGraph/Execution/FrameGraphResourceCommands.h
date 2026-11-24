#pragma once

#include "FrameGraph/FrameGraphBufferHandle.h"
#include "FrameGraph/FrameGraphAccelerationStructureHandle.h"
#include "FrameGraph/FrameGraphTextureHandle.h"
#include "FrameGraph/FrameGraphRasterPass.h"
#include "RHI/Public/Descriptors/RhiDescriptorHandles.h"
#include "RHI/Public/Resources/RhiResourceDesc.h"
#include "RHI/Public/Resources/RhiResourceHandles.h"

#include <span>
#include <cstdint>

class FrameGraph;
class PassBindingOverrides;
struct CompiledBinding;
struct PassParameterBinding;
enum class BindingDomain : std::uint8_t;
class RenderCommandList;
class RenderHardwareInterface;
struct NativeTextureViewInfo;
struct RhiNativeInteropRequest;

class FrameGraphResourceCommands final
{
public:
	explicit FrameGraphResourceCommands(const FrameGraph& frameGraph) noexcept;
	void BeginRasterPass(RenderCommandList& commandList, const FrameGraphRasterPass& rasterPass) const noexcept;
	void EndRasterPass(RenderCommandList& commandList) const noexcept;

	void BindRenderTargets(
	    RenderCommandList& commandList,
	    std::span<const FrameGraphTextureHandle> renderTargetHandles,
	    FrameGraphTextureHandle depthStencilHandle = FrameGraphTextureHandle::Invalid()) const noexcept;
	void CopyTexture(RenderCommandList& commandList, FrameGraphTextureHandle destinationHandle, FrameGraphTextureHandle sourceHandle) const noexcept;
	void CopyBuffer(RenderCommandList& commandList, FrameGraphBufferHandle destinationHandle, FrameGraphBufferHandle sourceHandle) const noexcept;
	void ClearRenderTarget(RenderCommandList& commandList, FrameGraphTextureHandle handle) const noexcept;
	void ClearDepthStencil(RenderCommandList& commandList, FrameGraphTextureHandle handle) const noexcept;
	RhiResourceHandle ResolveResource(FrameGraphTextureHandle handle) const noexcept;
	NativeTextureViewInfo ResolveNativeTextureView(FrameGraphTextureHandle handle, ResourceState state, const RhiNativeInteropRequest& request) const noexcept;
	RhiGpuDescriptorHandle ResolveShaderResourceView(FrameGraphTextureHandle handle) const noexcept;
	RhiGpuDescriptorHandle ResolveShaderResourceView(FrameGraphBufferHandle handle) const noexcept;
	RhiGpuDescriptorHandle ResolveUnorderedAccessView(FrameGraphTextureHandle handle) const noexcept;
	RhiGpuDescriptorHandle ResolveUnorderedAccessView(FrameGraphBufferHandle handle) const noexcept;
	RhiResourceHandle ResolveAccelerationStructure(FrameGraphAccelerationStructureHandle handle) const noexcept;
	void BindGlobalDescriptorState(RenderCommandList& commandList) const noexcept;

private:
	friend void BindCompiledPassParameter(
	    RenderCommandList& commandList,
	    const FrameGraphResourceCommands& resources,
	    const CompiledBinding& compiledBinding,
	    const PassParameterBinding* parameterBinding,
	    const PassBindingOverrides* overrides,
	    BindingDomain domain);
	RenderHardwareInterface& GetRenderHardwareInterface() const noexcept;
	RhiCpuDescriptorHandle ResolveRenderTargetView(FrameGraphResourceHandle handle) const noexcept;
	RhiCpuDescriptorHandle ResolveDepthStencilView(FrameGraphResourceHandle handle) const noexcept;
	RhiGpuDescriptorHandle ResolveUnorderedAccessView(FrameGraphResourceHandle handle) const noexcept;
	void CopyResource(RenderCommandList& commandList, FrameGraphResourceHandle destinationHandle, FrameGraphResourceHandle sourceHandle) const noexcept;
	const FrameGraph& m_frameGraph;
};
