#pragma once

#include "Diagnostics/ScopedGpuScope.h"

#include <string_view>

class PassExecutionDiagnostics;
struct FrameGraphPassNode;
class RenderCommandList;

class FrameGraphExecutionDiagnostics final
{
public:
	FrameGraphExecutionDiagnostics(const RenderDiagnostics& backendDiagnostics, RenderCommandList& commands) noexcept;

	bool ShouldEmitFramePassMarkers() const noexcept;
	bool ShouldEmitDetailedMarkers() const noexcept;

	ScopedGpuScope BeginPassScope(PassExecutionDiagnostics& passDiagnostics) const noexcept;
	void InsertPassAliasingBarrierMarker(const FrameGraphPassNode& passRecord) const;
	void InsertPassResourceBarrierMarker(const FrameGraphPassNode& passRecord) const;
	void InsertFrameBeginAliasingBarrierMarker() const;
	void InsertFrameEndResourceBarrierMarker() const;

private:
	void InsertDetailedMarker(std::string_view label) const;

	const RenderDiagnostics& m_backendDiagnostics;
	RenderCommandList& m_commands;
};
