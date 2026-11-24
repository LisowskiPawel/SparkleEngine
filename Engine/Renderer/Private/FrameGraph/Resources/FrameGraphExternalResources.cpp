#include "PCH.h"
#include "FrameGraph/FrameGraph.h"
#include "RHI/Public/Descriptors/RhiDescriptorService.h"
#include "RHI/Public/Resources/RhiResourceService.h"

#include "Core/Public/Diagnostics/Verify.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"

#include <cassert>
#include <format>
#include <string>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_frameGraphExternalLogger, "Renderer.FrameGraph");

static std::string FormatResourceName(const FrameGraphResourceMetadata& metadata)
{
	return metadata.debugName.empty() ? std::format("handle {}", metadata.handle.index) : metadata.debugName;
}

static void FailMissingUnorderedAccessSupport(const FrameGraphResourceMetadata& metadata) noexcept
{
	const std::string message = std::format(
	    "FrameGraph external resource validation failed: resource='{}' handle={} ownership={} requiredUsage=UnorderedAccess "
	    "remediation='create/import the resource with unordered-access support or remove UAV declarations for this pass path'",
	    FormatResourceName(metadata),
	    metadata.handle.index,
	    IsExternalFrameGraphResource(metadata.ownership) ? "External" : "Internal");

	Diagnostics::Fatal(g_frameGraphExternalLogger, __FILE__, __LINE__, message);
}

static bool RequiresUsage(const FrameGraphPlan& plan, FrameGraphResourceHandle handle, ResourceUsage usage) noexcept
{
	for (const FrameGraphPassNode& passRecord : plan.passes)
	{
		for (const PassResourceDeclaration& declaration : passRecord.declarations)
		{
			if (declaration.handle == handle && declaration.usage == usage)
			{
				return true;
			}
		}
	}

	return false;
}

static bool RequiresUnorderedAccessView(const FrameGraphPlan& plan, FrameGraphResourceHandle handle) noexcept
{
	return RequiresUsage(plan, handle, ResourceUsage::UnorderedAccess);
}

void FrameGraph::SyncImportedResourceAccesses() const noexcept
{
	for (const FrameGraphResourceHandle handle : m_resourceRegistry.GetRegisteredHandles())
	{
		const FrameGraphResourceMetadata& metadata = m_resourceRegistry.GetMetadata(handle);
		FrameGraphResourceAccess& access = m_resourceResolver.GetResolvedAccess(handle);
		if (!IsExternalFrameGraphResource(metadata.ownership) || metadata.kind == FrameGraphResourceKind::BackBuffer)
		{
			continue;
		}

		if (!access.resource)
		{
			continue;
		}

		if (metadata.kind == FrameGraphResourceKind::AccelerationStructure)
		{
			continue;
		}

		if (metadata.kind == FrameGraphResourceKind::ColorRenderTarget)
		{
			SyncImportedColorViews(handle, metadata, access);
		}
		else if (metadata.kind == FrameGraphResourceKind::DepthStencil)
		{
			SyncImportedDepthViews(handle, metadata, access);
		}
		else if (metadata.kind == FrameGraphResourceKind::Buffer)
		{
			SyncImportedBufferViews(handle, metadata, access);
		}
	}
}

void FrameGraph::SyncImportedColorViews(FrameGraphResourceHandle handle, const FrameGraphResourceMetadata& metadata, FrameGraphResourceAccess& access) const noexcept
{
	if (RequiresUsage(m_compiledPlan, handle, ResourceUsage::RenderTarget) && !access.renderTargetView)
	{
		const RhiResourceViewDesc renderTargetViewDesc = RhiResourceViewDesc::RenderTarget(access.resource, metadata.textureDesc.format);
		access.renderTargetView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(renderTargetViewDesc);
	}

	if (RequiresUsage(m_compiledPlan, handle, ResourceUsage::ShaderRead) && !access.shaderResourceView)
	{
		const RhiResourceViewDesc shaderResourceViewDesc = RhiResourceViewDesc::TextureShaderResource(access.resource, metadata.textureDesc.format);
		access.shaderResourceView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(shaderResourceViewDesc);

		access.bOwnsShaderResourceView = static_cast<bool>(access.shaderResourceView);
	}

	if (RequiresUnorderedAccessView(m_compiledPlan, handle))
	{
		if (!m_renderHardwareInterface.GetResourceService().SupportsUnorderedAccess(access.resource))
		{
			FailMissingUnorderedAccessSupport(metadata);
		}

		if (!access.unorderedAccessView)
		{
			const RhiResourceViewDesc unorderedAccessViewDesc = RhiResourceViewDesc::TextureUnorderedAccess(access.resource, metadata.textureDesc.format);
			access.unorderedAccessView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(unorderedAccessViewDesc);
		}
	}
}

void FrameGraph::SyncImportedDepthViews(FrameGraphResourceHandle handle, const FrameGraphResourceMetadata& metadata, FrameGraphResourceAccess& access) const noexcept
{
	if (!access.depthStencilView)
	{
		const RhiResourceViewDesc depthStencilViewDesc = RhiResourceViewDesc::DepthStencil(access.resource, metadata.textureDesc.format);
		access.depthStencilView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(depthStencilViewDesc);
	}

	if (RequiresUsage(m_compiledPlan, handle, ResourceUsage::ShaderRead) && !access.shaderResourceView)
	{
		const RhiResourceViewDesc shaderResourceViewDesc = RhiResourceViewDesc::TextureShaderResource(access.resource, metadata.textureDesc.format);
		access.shaderResourceView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(shaderResourceViewDesc);

		access.bOwnsShaderResourceView = static_cast<bool>(access.shaderResourceView);
	}
}

void FrameGraph::SyncImportedBufferViews(FrameGraphResourceHandle handle, const FrameGraphResourceMetadata& metadata, FrameGraphResourceAccess& access) const noexcept
{
	if (RequiresUsage(m_compiledPlan, handle, ResourceUsage::ShaderRead) && !access.shaderResourceView)
	{
		const RhiResourceViewDesc shaderResourceViewDesc = RhiResourceViewDesc::BufferShaderResource(access.resource, metadata.bufferDesc.sizeInBytes, metadata.bufferDesc.strideInBytes);
		access.shaderResourceView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(shaderResourceViewDesc);

		access.bOwnsShaderResourceView = static_cast<bool>(access.shaderResourceView);
	}

	if (RequiresUnorderedAccessView(m_compiledPlan, handle))
	{
		if (!m_renderHardwareInterface.GetResourceService().SupportsUnorderedAccess(access.resource))
		{
			FailMissingUnorderedAccessSupport(metadata);
		}

		if (!access.unorderedAccessView)
		{
			const RhiResourceViewDesc unorderedAccessViewDesc = RhiResourceViewDesc::BufferUnorderedAccess(access.resource, metadata.bufferDesc.sizeInBytes, metadata.bufferDesc.strideInBytes);
			access.unorderedAccessView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(unorderedAccessViewDesc);
		}
	}
}

void FrameGraph::ReleaseExternalResourceViews() noexcept
{
	for (const FrameGraphResourceHandle handle : m_resourceRegistry.GetRegisteredHandles())
	{
		const FrameGraphResourceMetadata& metadata = m_resourceRegistry.GetMetadata(handle);
		if (!IsExternalFrameGraphResource(metadata.ownership) || metadata.kind == FrameGraphResourceKind::BackBuffer)
		{
			continue;
		}

		ReleaseExternalResourceViews(handle);
	}
}

void FrameGraph::ReleaseExternalResourceViews(FrameGraphResourceHandle handle) noexcept
{
	if (!m_resourceRegistry.IsRegistered(handle))
	{
		return;
	}

	FrameGraphResourceAccess& access = m_resourceResolver.GetResolvedAccess(handle);
	if (access.renderTargetView)
	{
		m_renderHardwareInterface.GetDescriptorService().ReleaseResourceView(access.renderTargetView);
		access.renderTargetView = {};
	}

	if (access.depthStencilView)
	{
		m_renderHardwareInterface.GetDescriptorService().ReleaseResourceView(access.depthStencilView);
		access.depthStencilView = {};
	}

	if (access.shaderResourceView)
	{
		if (access.bOwnsShaderResourceView)
		{
			m_renderHardwareInterface.GetDescriptorService().ReleaseResourceView(access.shaderResourceView);
		}
		access.shaderResourceView = {};
		access.bOwnsShaderResourceView = false;
	}

	if (access.unorderedAccessView)
	{
		m_renderHardwareInterface.GetDescriptorService().ReleaseResourceView(access.unorderedAccessView);
		access.unorderedAccessView = {};
	}
}
