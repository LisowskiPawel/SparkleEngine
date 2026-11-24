#include "PCH.h"

#include "FrameGraph/Diagnostics/FrameGraphExecutionDiagnostics.h"

#include "Diagnostics/PassExecutionDiagnostics.h"
#include "Debug/RendererCVars.h"
#include "RHI/Public/Commands/RenderCommandList.h"
#include "FrameGraph/Compiler/FrameGraphPlan.h"

#include <string>

static constexpr RhiDiagnosticLabelColor kFrameGraphDetailMarkerColor{.Red = 120, .Green = 160, .Blue = 220, .Alpha = 255};

FrameGraphExecutionDiagnostics::FrameGraphExecutionDiagnostics(const RenderDiagnostics& backendDiagnostics, RenderCommandList& commands) noexcept :
    m_backendDiagnostics(backendDiagnostics),
    m_commands(commands)
{
}

bool FrameGraphExecutionDiagnostics::ShouldEmitFramePassMarkers() const noexcept
{
	const RendererDiagnosticMarkerVerbosity verbosity = CVarRendererDiagnosticMarkerVerbosity.Get();

	return m_backendDiagnostics.GetCapabilities().bRequiresBaselineScopes || verbosity == RendererDiagnosticMarkerVerbosity::FramePass || verbosity == RendererDiagnosticMarkerVerbosity::Detailed;
}

bool FrameGraphExecutionDiagnostics::ShouldEmitDetailedMarkers() const noexcept
{
	return CVarRendererDiagnosticMarkerVerbosity.Get() == RendererDiagnosticMarkerVerbosity::Detailed && m_commands.SupportsDiagnosticScopes();
}

ScopedGpuScope FrameGraphExecutionDiagnostics::BeginPassScope(PassExecutionDiagnostics& passDiagnostics) const noexcept
{
	return ShouldEmitFramePassMarkers() ? passDiagnostics.BeginPassGpuScope() : ScopedGpuScope{};
}

void FrameGraphExecutionDiagnostics::InsertPassAliasingBarrierMarker(const FrameGraphPassNode& passRecord) const
{
	if (!ShouldEmitDetailedMarkers() || passRecord.transientAliasingBarriers.empty())
	{
		return;
	}

	std::string marker = passRecord.diagnosticName;
	marker += ".AliasingBarriers";
	InsertDetailedMarker(marker);
}

void FrameGraphExecutionDiagnostics::InsertPassResourceBarrierMarker(const FrameGraphPassNode& passRecord) const
{
	if (!ShouldEmitDetailedMarkers())
	{
		return;
	}
	if (passRecord.compiledBarriers.empty() && passRecord.compiledReleaseBarriers.empty())
	{
		return;
	}

	std::string marker = passRecord.diagnosticName;
	marker += ".ResourceBarriers";
	InsertDetailedMarker(marker);
}

void FrameGraphExecutionDiagnostics::InsertFrameBeginAliasingBarrierMarker() const
{
	if (ShouldEmitDetailedMarkers())
	{
		InsertDetailedMarker("Renderer.FrameGraph.FrameBegin.AliasingBarriers");
	}
}

void FrameGraphExecutionDiagnostics::InsertFrameEndResourceBarrierMarker() const
{
	if (ShouldEmitDetailedMarkers())
	{
		InsertDetailedMarker("Renderer.FrameGraph.FrameEnd.ResourceBarriers");
	}
}

void FrameGraphExecutionDiagnostics::InsertDetailedMarker(std::string_view label) const
{
	m_commands.InsertDiagnosticMarker(label, kFrameGraphDetailMarkerColor);
}
