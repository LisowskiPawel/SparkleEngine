#include "PCH.h"
#include "FrameGraph/Resources/FrameGraphTransientAllocator.h"

#include "RHI/Public/Device/RenderHardwareInterface.h"

#include <algorithm>
#include <string>

bool FrameGraphTransientAllocator::RequiresShaderResourceView(const FrameGraphTransientResourcePlan& transientPlan) noexcept
{
	return std::find(transientPlan.lifetime.requiredStates.begin(), transientPlan.lifetime.requiredStates.end(), ResourceState::ShaderResource) != transientPlan.lifetime.requiredStates.end();
}

bool FrameGraphTransientAllocator::RequiresUnorderedAccessView(const FrameGraphTransientResourcePlan& transientPlan) noexcept
{
	return transientPlan.kind != FrameGraphResourceKind::DepthStencil
	    && std::find(transientPlan.lifetime.requiredStates.begin(), transientPlan.lifetime.requiredStates.end(), ResourceState::UnorderedAccess) != transientPlan.lifetime.requiredStates.end();
}

static std::wstring BuildWideDebugName(const std::string& name, const wchar_t* defaultDebugName)
{
	if (name.empty())
	{
		return defaultDebugName;
	}

	return std::wstring(name.begin(), name.end());
}

void FrameGraphTransientAllocator::CreateTextureAllocation(
    AllocationRecord& allocation,
    const FrameGraphTransientResourcePlan& transientPlan,
    RhiOwnedMemoryBlockHandle memoryBlock,
    const wchar_t* defaultDebugName)
{
	const std::wstring debugName = BuildWideDebugName(transientPlan.textureDesc.name, defaultDebugName);

	allocation.ownedResource = m_renderHardwareInterface->GetResourceService().CreateAliasingTextureResource(
	    memoryBlock,
	    transientPlan.physicalAllocation.memoryBlockOffset,
	    RhiTransientTextureAllocationDesc{
	        .ResourceDesc = transientPlan.physicalAllocation.textureResourceDesc,
	        .ClearValue = transientPlan.physicalAllocation.hasOptimizedClearValue ? transientPlan.physicalAllocation.optimizedClearValue : RhiOptimizedClearValue{},
	        .InitialState = transientPlan.physicalAllocation.initialState},
	    debugName);

	allocation.resource = m_renderHardwareInterface->GetResourceService().GetResourceHandle(allocation.ownedResource);
}

void FrameGraphTransientAllocator::CreateDepthStencilAllocation(AllocationRecord& allocation, const FrameGraphTransientResourcePlan& transientPlan, RhiOwnedMemoryBlockHandle memoryBlock)
{
	CreateTextureAllocation(allocation, transientPlan, memoryBlock, L"FG_DepthTransient");

	allocation.depthStencilView = m_renderHardwareInterface->GetDescriptorService().CreateResourceView(RhiResourceViewDesc::DepthStencil(allocation.resource, transientPlan.textureDesc.format));

	if (RequiresShaderResourceView(transientPlan))
	{
		allocation.shaderResourceView = m_renderHardwareInterface->GetDescriptorService().CreateResourceView(
		    RhiResourceViewDesc::TextureShaderResource(allocation.resource, transientPlan.textureDesc.format));
	}
}

void FrameGraphTransientAllocator::CreateColorAllocation(AllocationRecord& allocation, const FrameGraphTransientResourcePlan& transientPlan, RhiOwnedMemoryBlockHandle memoryBlock)
{
	CreateTextureAllocation(allocation, transientPlan, memoryBlock, L"FG_ColorTransient");

	if (transientPlan.physicalAllocation.textureResourceDesc.AllowRenderTarget)
	{
		allocation.renderTargetView = m_renderHardwareInterface->GetDescriptorService().CreateResourceView(RhiResourceViewDesc::RenderTarget(allocation.resource, transientPlan.textureDesc.format));
	}

	if (RequiresShaderResourceView(transientPlan))
	{
		allocation.shaderResourceView = m_renderHardwareInterface->GetDescriptorService().CreateResourceView(
		    RhiResourceViewDesc::TextureShaderResource(allocation.resource, transientPlan.textureDesc.format));
	}

	if (RequiresUnorderedAccessView(transientPlan))
	{
		allocation.unorderedAccessView = m_renderHardwareInterface->GetDescriptorService().CreateResourceView(
		    RhiResourceViewDesc::TextureUnorderedAccess(allocation.resource, transientPlan.textureDesc.format));
	}
}

void FrameGraphTransientAllocator::CreateBufferAllocation(AllocationRecord& allocation, const FrameGraphTransientResourcePlan& transientPlan, RhiOwnedMemoryBlockHandle memoryBlock)
{
	const std::wstring debugName = BuildWideDebugName(transientPlan.bufferDesc.name, L"FG_BufferTransient");

	allocation.ownedResource = m_renderHardwareInterface->GetResourceService().CreateAliasingBufferResource(
	    memoryBlock,
	    transientPlan.physicalAllocation.memoryBlockOffset,
	    RhiTransientBufferAllocationDesc{.ResourceDesc = transientPlan.physicalAllocation.bufferResourceDesc, .InitialState = transientPlan.physicalAllocation.initialState},
	    debugName);

	allocation.resource = m_renderHardwareInterface->GetResourceService().GetResourceHandle(allocation.ownedResource);

	if (RequiresShaderResourceView(transientPlan))
	{
		allocation.shaderResourceView = m_renderHardwareInterface->GetDescriptorService().CreateResourceView(
		    RhiResourceViewDesc::BufferShaderResource(allocation.resource, transientPlan.bufferDesc.sizeInBytes, transientPlan.bufferDesc.strideInBytes));
	}

	if (RequiresUnorderedAccessView(transientPlan))
	{
		allocation.unorderedAccessView = m_renderHardwareInterface->GetDescriptorService().CreateResourceView(
		    RhiResourceViewDesc::BufferUnorderedAccess(allocation.resource, transientPlan.bufferDesc.sizeInBytes, transientPlan.bufferDesc.strideInBytes));
	}
}
