#include "PCH.h"
#include "FrameGraph/Builder/FrameGraphBuilder.h"

#include <algorithm>
#include <limits>

FrameGraphBuilder::FrameGraphBuilder(FrameGraph& frameGraph, const RenderPassRuntimeCache& renderPassRuntimeCache) noexcept :
    m_frameGraph(frameGraph),
    m_renderPassRuntimeCache(renderPassRuntimeCache)
{
}

FrameGraphTextureHandle FrameGraphBuilder::ImportBackBuffer(const FrameGraphTextureDesc& desc, ResourceState initialState) noexcept
{
	return m_frameGraph.ImportBackBuffer(desc, initialState);
}

FrameGraphTextureHandle FrameGraphBuilder::ReservePersistentTexture(const FrameGraphTextureDesc& desc, ResourceState initialState) noexcept
{
	return m_frameGraph.ReservePersistentTexture(desc, initialState);
}

FrameGraphTextureHandle FrameGraphBuilder::CreateTexture(const FrameGraphTextureDesc& desc) noexcept
{
	return m_frameGraph.CreateTexture(desc);
}

FrameGraphTextureHistory FrameGraphBuilder::CreateTextureHistory(const FrameGraphTextureDesc& desc) noexcept
{
	return m_frameGraph.CreateTextureHistory(desc);
}

FrameGraphBufferHandle FrameGraphBuilder::ReservePersistentBuffer(const FrameGraphBufferDesc& desc, ResourceState initialState) noexcept
{
	return m_frameGraph.ReservePersistentBuffer(desc, initialState);
}

FrameGraphBufferHandle FrameGraphBuilder::CreateBuffer(const FrameGraphBufferDesc& desc) noexcept
{
	return m_frameGraph.CreateBuffer(desc);
}

FrameGraphAccelerationStructureHandle FrameGraphBuilder::ReservePersistentAccelerationStructure(std::string_view name, ResourceState initialState) noexcept
{
	return m_frameGraph.ReservePersistentAccelerationStructure(name, initialState);
}

void FrameGraphBuilder::ExportTexture(FrameGraphTextureHandle handle, std::string_view name) noexcept
{
	m_frameGraph.ExportTexture(handle, name);
}

ShaderAccelerationStructure FrameGraphBuilder::CreateAccelerationStructureBinding(FrameGraphAccelerationStructureHandle handle) const noexcept
{
	return m_frameGraph.CreateAccelerationStructureBinding(handle);
}

ShaderRenderTarget FrameGraphBuilder::CreateRenderTarget(FrameGraphTextureHandle handle, FrameGraphAttachmentLoadAction load, FrameGraphAttachmentStoreAction store) const noexcept
{
	ShaderRenderTarget result;
	result = FrameGraphAttachmentBinding{.Handle = handle, .Load = load, .Store = store};
	return result;
}

ShaderDepthTarget FrameGraphBuilder::CreateDepthTarget(FrameGraphTextureHandle handle, FrameGraphAttachmentLoadAction load, FrameGraphAttachmentStoreAction store, FrameGraphDepthStencilAccess access)
    const noexcept
{
	ShaderDepthTarget result;
	result = FrameGraphAttachmentBinding{.Handle = handle, .Load = load, .Store = store, .DepthStencilAccess = access};
	return result;
}

std::string FrameGraphBuilder::BuildRayTracingShaderTableKey(std::string_view label, const RayTracingPassPipelineRuntime& runtime, std::uint64_t recordLayoutGeneration)
{
	const std::string pipelineIdentity = std::to_string(reinterpret_cast<std::uintptr_t>(&runtime.Pipeline));
	const std::string shaderGeneration = std::to_string(runtime.Generation);
	const std::string layoutGeneration = std::to_string(recordLayoutGeneration);
	return std::string(label) + "." + pipelineIdentity + "." + shaderGeneration + "." + layoutGeneration;
}

std::uint64_t FrameGraphBuilder::GetRayTracingShaderTableSize(const RayTracingShaderTable& shaderTable)
{
	const auto regionEnd = [](const RhiRayTracingShaderTableRegion& region)
	{
		if (region.OffsetInBytes > std::numeric_limits<std::uint64_t>::max() - region.SizeInBytes)
		{
			throw Diagnostics::Error("Ray-tracing shader-table region arithmetic overflowed before graph dispatch.");
		}

		return region.OffsetInBytes + region.SizeInBytes;
	};

	const std::uint64_t rayGenerationEnd = regionEnd(shaderTable.GetRayGenerationRegion());
	const std::uint64_t missEnd = regionEnd(shaderTable.GetMissRegion());
	const std::uint64_t hitGroupEnd = regionEnd(shaderTable.GetHitGroupRegion());
	const std::uint64_t callableEnd = regionEnd(shaderTable.GetCallableRegion());
	return std::max({rayGenerationEnd, missEnd, hitGroupEnd, callableEnd});
}
