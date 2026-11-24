#pragma once

#include "RHI/Public/Commands/RenderCommandList.h"

#include <cstdint>

class RenderCommandContext final
{
public:
	explicit RenderCommandContext(RenderCommandList& commandList) noexcept;
	~RenderCommandContext() noexcept = default;

	RenderCommandContext(const RenderCommandContext&) = delete;
	RenderCommandContext& operator=(const RenderCommandContext&) = delete;
	RenderCommandContext(RenderCommandContext&&) = delete;
	RenderCommandContext& operator=(RenderCommandContext&&) = delete;

	void EnableDrawDispatchDiagnostics() noexcept;

	bool IsDrawDispatchDiagnosticsEnabled() const noexcept { return m_bDrawDispatchDiagnosticsEnabled; }

	void DrawIndexedInstanced(
	    std::uint32_t indexCountPerInstance,
	    std::uint32_t instanceCount,
	    std::uint32_t startIndexLocation,
	    std::int32_t baseVertexLocation,
	    std::uint32_t startInstanceLocation) noexcept;

	void DrawInstanced(std::uint32_t vertexCountPerInstance, std::uint32_t instanceCount, std::uint32_t startVertexLocation, std::uint32_t startInstanceLocation) noexcept;

	void Dispatch(std::uint32_t groupCountX, std::uint32_t groupCountY, std::uint32_t groupCountZ) noexcept;
	void TraceRays(const TraceRaysDesc& desc) noexcept;

	RenderCommandList& GetRenderCommandList() const noexcept { return m_commandList; }

private:
	void EmitDrawMarker() noexcept;
	void EmitDispatchMarker(std::uint32_t groupCountX, std::uint32_t groupCountY, std::uint32_t groupCountZ) noexcept;

	RenderCommandList& m_commandList;
	bool m_bDrawDispatchDiagnosticsEnabled = false;
	std::uint32_t m_drawCount = 0;
	std::uint32_t m_dispatchCount = 0;
};
