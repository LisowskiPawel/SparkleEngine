#pragma once

#include "RHI/Public/Diagnostics/RhiDiagnostics.h"

#include <string_view>

class RenderCommandContext;

class ScopedGpuScope final
{
public:
	ScopedGpuScope() noexcept = default;
	ScopedGpuScope(RenderCommandContext& commands, std::string_view label, RhiDiagnosticLabelColor color = {}) noexcept;
	~ScopedGpuScope() noexcept;

	ScopedGpuScope(const ScopedGpuScope&) = delete;
	ScopedGpuScope& operator=(const ScopedGpuScope&) = delete;
	ScopedGpuScope(ScopedGpuScope&& other) noexcept;
	ScopedGpuScope& operator=(ScopedGpuScope&& other) noexcept;

private:
	void Reset() noexcept;

	RenderCommandContext* m_commands = nullptr;
};
