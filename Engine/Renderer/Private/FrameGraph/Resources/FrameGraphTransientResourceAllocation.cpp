#include "PCH.h"
#include "FrameGraph/Resources/FrameGraphTransientAllocator.h"
#include "RHI/Public/Descriptors/RhiDescriptorService.h"
#include "RHI/Public/Resources/RhiResourceService.h"

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
	const auto& allocationPlan = transientPlan.physicalAllocation;

	const RhiTransientTextureAllocationDesc allocationDesc{
	    .ResourceDesc = allocationPlan.textureResourceDesc,
	    .ClearValue = allocationPlan.bHasOptimizedClearValue ? allocationPlan.optimizedClearValue : RhiOptimizedClearValue{},
	    .InitialState = allocationPlan.initialState};

	allocation.ownedResource = m_renderHardwareInterface.GetResourceService().CreateAliasingTextureResource(memoryBlock, allocationPlan.memoryBlockOffset, allocationDesc, debugName);

	allocation.Access.resource = m_renderHardwareInterface.GetResourceService().GetResourceHandle(allocation.ownedResource);
}

void FrameGraphTransientAllocator::CreateDepthStencilAllocation(AllocationRecord& allocation, const FrameGraphTransientResourcePlan& transientPlan, RhiOwnedMemoryBlockHandle memoryBlock)
{
	CreateTextureAllocation(allocation, transientPlan, memoryBlock, L"FG_DepthTransient");

	const RhiResourceViewDesc depthStencilViewDesc = RhiResourceViewDesc::DepthStencil(allocation.Access.resource, transientPlan.textureDesc.format);
	allocation.Access.depthStencilView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(depthStencilViewDesc);

	if (RequiresShaderResourceView(transientPlan))
	{
		const RhiResourceViewDesc shaderResourceViewDesc = RhiResourceViewDesc::TextureShaderResource(allocation.Access.resource, transientPlan.textureDesc.format);
		allocation.Access.shaderResourceView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(shaderResourceViewDesc);
	}
}

void FrameGraphTransientAllocator::CreateColorAllocation(AllocationRecord& allocation, const FrameGraphTransientResourcePlan& transientPlan, RhiOwnedMemoryBlockHandle memoryBlock)
{
	CreateTextureAllocation(allocation, transientPlan, memoryBlock, L"FG_ColorTransient");

	if (transientPlan.physicalAllocation.textureResourceDesc.bAllowRenderTarget)
	{
		const RhiResourceViewDesc renderTargetViewDesc = RhiResourceViewDesc::RenderTarget(allocation.Access.resource, transientPlan.textureDesc.format);
		allocation.Access.renderTargetView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(renderTargetViewDesc);
	}

	if (RequiresShaderResourceView(transientPlan))
	{
		const RhiResourceViewDesc shaderResourceViewDesc = RhiResourceViewDesc::TextureShaderResource(allocation.Access.resource, transientPlan.textureDesc.format);
		allocation.Access.shaderResourceView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(shaderResourceViewDesc);
	}

	if (RequiresUnorderedAccessView(transientPlan))
	{
		const RhiResourceViewDesc unorderedAccessViewDesc = RhiResourceViewDesc::TextureUnorderedAccess(allocation.Access.resource, transientPlan.textureDesc.format);
		allocation.Access.unorderedAccessView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(unorderedAccessViewDesc);
	}
}

void FrameGraphTransientAllocator::CreateBufferAllocation(AllocationRecord& allocation, const FrameGraphTransientResourcePlan& transientPlan, RhiOwnedMemoryBlockHandle memoryBlock)
{
	const std::wstring debugName = BuildWideDebugName(transientPlan.bufferDesc.name, L"FG_BufferTransient");
	const auto& allocationPlan = transientPlan.physicalAllocation;
	const RhiTransientBufferAllocationDesc allocationDesc{.ResourceDesc = allocationPlan.bufferResourceDesc, .InitialState = allocationPlan.initialState};

	allocation.ownedResource = m_renderHardwareInterface.GetResourceService().CreateAliasingBufferResource(memoryBlock, allocationPlan.memoryBlockOffset, allocationDesc, debugName);

	allocation.Access.resource = m_renderHardwareInterface.GetResourceService().GetResourceHandle(allocation.ownedResource);

	if (RequiresShaderResourceView(transientPlan))
	{
		const RhiResourceViewDesc shaderResourceViewDesc = RhiResourceViewDesc::BufferShaderResource(
		    allocation.Access.resource,
		    transientPlan.bufferDesc.sizeInBytes,
		    transientPlan.bufferDesc.strideInBytes);

		allocation.Access.shaderResourceView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(shaderResourceViewDesc);
	}

	if (RequiresUnorderedAccessView(transientPlan))
	{
		const RhiResourceViewDesc unorderedAccessViewDesc = RhiResourceViewDesc::BufferUnorderedAccess(
		    allocation.Access.resource,
		    transientPlan.bufferDesc.sizeInBytes,
		    transientPlan.bufferDesc.strideInBytes);

		allocation.Access.unorderedAccessView = m_renderHardwareInterface.GetDescriptorService().CreateResourceView(unorderedAccessViewDesc);
	}
}
