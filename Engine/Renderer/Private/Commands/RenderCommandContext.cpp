#include "PCH.h"
#include "Commands/RenderCommandContext.h"
#include "RHI/Public/RayTracing/RhiRayTracingPipelineDesc.h"

#include <cstdio>

RenderCommandContext::RenderCommandContext(RenderCommandList& commandList) noexcept :
    m_commandList(commandList)
{
}

void RenderCommandContext::EnableDrawDispatchDiagnostics() noexcept
{
	m_bDrawDispatchDiagnosticsEnabled = m_commandList.SupportsDiagnosticScopes();
}

void RenderCommandContext::EmitDrawMarker() noexcept
{
	char label[32];
	std::snprintf(label, sizeof(label), "Draw.%u", m_drawCount++);
	m_commandList.InsertDiagnosticMarker(label, {});
}

void RenderCommandContext::EmitDispatchMarker(std::uint32_t groupCountX, std::uint32_t groupCountY, std::uint32_t groupCountZ) noexcept
{
	char label[64];
	std::snprintf(label, sizeof(label), "Dispatch.%u.%ux%ux%u", m_dispatchCount++, groupCountX, groupCountY, groupCountZ);
	m_commandList.InsertDiagnosticMarker(label, {});
}

void RenderCommandContext::DrawIndexedInstanced(
    std::uint32_t indexCountPerInstance,
    std::uint32_t instanceCount,
    std::uint32_t startIndexLocation,
    std::int32_t baseVertexLocation,
    std::uint32_t startInstanceLocation) noexcept
{
	if (m_bDrawDispatchDiagnosticsEnabled)
	{
		EmitDrawMarker();
	}
	m_commandList.DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void RenderCommandContext::DrawInstanced(std::uint32_t vertexCountPerInstance, std::uint32_t instanceCount, std::uint32_t startVertexLocation, std::uint32_t startInstanceLocation) noexcept
{
	if (m_bDrawDispatchDiagnosticsEnabled)
	{
		EmitDrawMarker();
	}
	m_commandList.DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void RenderCommandContext::Dispatch(std::uint32_t groupCountX, std::uint32_t groupCountY, std::uint32_t groupCountZ) noexcept
{
	if (m_bDrawDispatchDiagnosticsEnabled)
	{
		EmitDispatchMarker(groupCountX, groupCountY, groupCountZ);
	}
	m_commandList.Dispatch(groupCountX, groupCountY, groupCountZ);
}

void RenderCommandContext::TraceRays(const TraceRaysDesc& desc) noexcept
{
	if (m_bDrawDispatchDiagnosticsEnabled)
	{
		EmitDispatchMarker(desc.Width, desc.Height, desc.Depth);
	}
	m_commandList.TraceRays(desc);
}
