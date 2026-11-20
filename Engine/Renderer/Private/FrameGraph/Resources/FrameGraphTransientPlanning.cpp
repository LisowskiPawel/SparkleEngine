#include "PCH.h"
#include "FrameGraph/FrameGraph.h"
#include "RHI/Public/Resources/RhiResourceService.h"

#include "Renderer/Private/FrameGraph/Resources/FrameGraphTransientAllocator.h"

#include "Config/DepthConvention.h"
#include "RHI/Public/Device/RenderHardwareInterface.h"

#include <cassert>
#include <utility>

static bool RequiresUnorderedAccess(const FrameGraphPlan& plan, FrameGraphResourceHandle handle) noexcept
{
	for (const FrameGraphPassNode& passRecord : plan.passes)
	{
		for (const PassResourceDeclaration& declaration : passRecord.declarations)
		{
			if (declaration.handle == handle && UsesUnorderedAccess(declaration.usage))
			{
				return true;
			}
		}
	}

	return false;
}

static bool RequiresRenderTarget(const FrameGraphPlan& plan, FrameGraphResourceHandle handle) noexcept
{
	for (const FrameGraphPassNode& passRecord : plan.passes)
	{
		for (const PassResourceDeclaration& declaration : passRecord.declarations)
		{
			if (declaration.handle == handle && declaration.usage == ResourceUsage::RenderTarget)
			{
				return true;
			}
		}
	}

	return false;
}

static RhiBufferResourceDesc BuildTransientBufferDesc(const FrameGraphBufferDesc& desc, bool bRequiresUnorderedAccess) noexcept
{
	return RhiBufferResourceDesc{.SizeInBytes = desc.sizeInBytes, .StrideInBytes = desc.strideInBytes, .bAllowUnorderedAccess = bRequiresUnorderedAccess};
}

static RhiTextureResourceDesc BuildTransientResourceDesc(const FrameGraphTextureDesc& desc, FrameGraphResourceKind kind, bool bRequiresRenderTarget, bool bRequiresUnorderedAccess) noexcept
{
	RhiTextureResourceDesc resourceDesc{};
	resourceDesc.Width = desc.width;
	resourceDesc.Height = desc.height;
	resourceDesc.Format = desc.format;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleCount = desc.sampleCount;
	resourceDesc.bAllowRenderTarget = kind == FrameGraphResourceKind::ColorRenderTarget && bRequiresRenderTarget;
	resourceDesc.bAllowDepthStencil = kind == FrameGraphResourceKind::DepthStencil;
	resourceDesc.bAllowUnorderedAccess = bRequiresUnorderedAccess;
	return resourceDesc;
}

static RhiOptimizedClearValue BuildTransientOptimizedClearValue(const FrameGraphTextureDesc& desc, FrameGraphResourceKind kind) noexcept
{
	RhiOptimizedClearValue clearValue{};
	clearValue.Format = desc.format;

	if (kind == FrameGraphResourceKind::DepthStencil)
	{
		clearValue.ValueType = RhiOptimizedClearValue::Type::DepthStencil;
		clearValue.Depth = DepthConvention::GetClearDepth();
		clearValue.Stencil = 0;
		return clearValue;
	}

	clearValue.ValueType = RhiOptimizedClearValue::Type::Color;
	clearValue.Color = desc.clearColor;
	return clearValue;
}

static RhiTransientAllocationPool ResolveTransientAllocationPool(FrameGraphResourceKind kind, bool bRequiresRenderTarget) noexcept
{
	if (kind == FrameGraphResourceKind::DepthStencil)
	{
		return RhiTransientAllocationPool::DepthStencilTexture;
	}

	if (kind == FrameGraphResourceKind::Buffer)
	{
		return RhiTransientAllocationPool::Buffer;
	}

	return bRequiresRenderTarget ? RhiTransientAllocationPool::RenderTargetTexture : RhiTransientAllocationPool::Texture;
}

void FrameGraph::BuildTransientMaterializationPlan(FrameGraphPlan& plan) const noexcept
{
	plan.transients.resources.clear();
	plan.transients.resources.reserve(m_virtualTransientResources.size());

	for (const VirtualTransientResource& transientResource : m_virtualTransientResources)
	{
		const FrameGraphResourceMetadata& resourceMetadata = m_resourceRegistry.GetMetadata(transientResource.handle);
		if (resourceMetadata.ownership != FrameGraphResourceOwnership::Transient)
		{
			continue;
		}

		FrameGraphTransientResourcePlan resourcePlan = BuildTransientResourcePlan(transientResource, resourceMetadata, plan);
		plan.transients.resources.push_back(std::move(resourcePlan));
	}
}

FrameGraphTransientResourcePlan FrameGraph::BuildTransientResourcePlan(
    const VirtualTransientResource& transientResource,
    const FrameGraphResourceMetadata& resourceMetadata,
    const FrameGraphPlan& plan) const noexcept
{
	const bool bRequiresUnorderedAccess = RequiresUnorderedAccess(plan, transientResource.handle);
	const bool bRequiresRenderTarget = RequiresRenderTarget(plan, transientResource.handle);
	const bool bIsBuffer = resourceMetadata.resourceClass == FrameGraphResourceClass::Buffer;
	const bool bHasClearValueRenderTarget = resourceMetadata.kind == FrameGraphResourceKind::DepthStencil || bRequiresRenderTarget;
	const bool bHasOptimizedClearValue = !bIsBuffer && bHasClearValueRenderTarget;

	const RhiBufferResourceDesc bufferResourceDesc = bIsBuffer ? BuildTransientBufferDesc(transientResource.bufferDesc, bRequiresUnorderedAccess) : RhiBufferResourceDesc{};

	const RhiTextureResourceDesc textureResourceDesc = bIsBuffer ? RhiTextureResourceDesc{}
	                                                             : BuildTransientResourceDesc(transientResource.textureDesc, resourceMetadata.kind, bRequiresRenderTarget, bRequiresUnorderedAccess);

	RhiResourceService& resources = m_renderHardwareInterface.GetResourceService();
	const RhiResourceAllocationInfo allocationInfo = bIsBuffer ? resources.GetBufferAllocationInfo(bufferResourceDesc) : resources.GetTextureAllocationInfo(textureResourceDesc);

	const RhiOptimizedClearValue clearValue = bHasOptimizedClearValue ? BuildTransientOptimizedClearValue(transientResource.textureDesc, resourceMetadata.kind) : RhiOptimizedClearValue{};

	return FrameGraphTransientResourcePlan{
	    .handle = transientResource.handle,
	    .resourceClass = resourceMetadata.resourceClass,
	    .textureDesc = transientResource.textureDesc,
	    .bufferDesc = transientResource.bufferDesc,
	    .kind = resourceMetadata.kind,
	    .physicalAllocation = FrameGraphTransientResourcePlan::PhysicalAllocationPlan{
	        .physicalBlockIndex = INVALID_FRAME_GRAPH_RESOURCE_INDEX,
	        .pool = ResolveTransientAllocationPool(resourceMetadata.kind, bRequiresRenderTarget),
	        .sizeInBytes = allocationInfo.SizeInBytes,
	        .alignment = allocationInfo.Alignment,
	        .memoryBlockOffset = 0,
	        .textureResourceDesc = textureResourceDesc,
	        .bufferResourceDesc = bufferResourceDesc,
	        .optimizedClearValue = clearValue,
	        .bHasOptimizedClearValue = bHasOptimizedClearValue,
	        .initialState = resourceMetadata.initialState}};
}

void FrameGraph::PrepareTransientAllocations(const FrameGraphTransientPlan& plan) noexcept
{
	if (!m_transientAllocator->Prepare(plan))
	{
		return;
	}

	// Barrier planning must use the initial states of replacement GPU resources.
	for (const FrameGraphTransientResourcePlan& transient : plan.resources)
	{
		m_resourceStateTracker.UpdateCurrentState(transient.handle, transient.physicalAllocation.initialState);
	}
}

void FrameGraph::EnsureTransientResourcesMaterialized(const FrameGraphPlan& plan) const noexcept
{
	for (const FrameGraphTransientResourcePlan& transientPlan : plan.transients.resources)
	{
		const FrameGraphResourceAccess& access = m_transientAllocator->Materialize(transientPlan);
		m_resourceResolver.SetResolvedAccess(transientPlan.handle, access);
	}
}
