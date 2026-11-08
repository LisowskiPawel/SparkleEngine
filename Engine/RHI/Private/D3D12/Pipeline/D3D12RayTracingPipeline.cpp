#include "PCH.h"

#include "D3D12/Pipeline/D3D12RayTracingPipeline.h"

#include "Core/Public/Diagnostics/Error.h"
#include "D3D12/Device/D3D12Rhi.h"
#include "D3D12/Pipeline/D3D12RayTracingPipelineCreation.h"
#include "Core/Public/Strings/StringUtils.h"

#include <string>

D3D12RayTracingPipeline::D3D12RayTracingPipeline(D3D12Rhi& rhi, const RayTracingPipelineDesc& desc) :
    RayTracingPipeline(desc)
{
	const RhiRayTracingCapabilities capabilities = rhi.GetRayTracingCapabilities();
	if (!capabilities.bSupportsRayTracingPipeline || desc.MaxPayloadSizeInBytes > capabilities.MaxRayPayloadSizeInBytes || desc.MaxAttributeSizeInBytes > capabilities.MaxRayAttributeSizeInBytes
	    || desc.MaxRecursionDepth > capabilities.MaxTraceRecursionDepth)
	{
		throw Diagnostics::Error("D3D12 ray-tracing pipeline exceeds device readiness or limits.");
	}
	CreateD3D12RayTracingPipeline(rhi, desc, m_stateObject, m_properties);
	if (desc.DebugName != nullptr)
	{
		m_stateObject->SetName(desc.DebugName);
	}
}

const void* D3D12RayTracingPipeline::FindShaderIdentifier(std::string_view exportName) const noexcept
{
	const std::wstring wideName = Strings::ToWide(exportName);
	return m_properties != nullptr ? m_properties->GetShaderIdentifier(wideName.c_str()) : nullptr;
}
