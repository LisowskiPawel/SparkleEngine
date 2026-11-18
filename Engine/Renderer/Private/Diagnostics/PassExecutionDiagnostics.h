#pragma once

#include "Diagnostics/ScopedGpuScope.h"

#include "FrameGraph/FrameGraphPassKind.h"

#include <string>
#include <string_view>

class PassExecutionDiagnostics final
{
public:
	PassExecutionDiagnostics(RenderCommandContext& commands, std::string_view passScopeLabel, EFrameGraphPassKind passKind) noexcept;

	ScopedGpuScope BeginPassGpuScope() noexcept;
	ScopedGpuScope BeginGpuScope(std::string_view label) noexcept;

private:
	static RhiDiagnosticLabelColor GetPassEventColor(EFrameGraphPassKind passKind) noexcept;
	std::string FormatEventScopeLabel(std::string_view label) const;

	RenderCommandContext& m_commands;
	// The compiled plan owns the label until recording has joined.
	std::string_view m_passScopeLabel;
	RhiDiagnosticLabelColor m_passColor = {};
};
