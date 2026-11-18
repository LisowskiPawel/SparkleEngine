#include "PCH.h"

#include "Diagnostics/ScopedGpuScope.h"

#include "Commands/RenderCommandContext.h"

#include <utility>

ScopedGpuScope::ScopedGpuScope(RenderCommandContext& commands, std::string_view label, RhiDiagnosticLabelColor color) noexcept
{
	if (label.empty() || !commands.SupportsDiagnosticScopes())
	{
		return;
	}

	commands.BeginDiagnosticScope(label, color);
	m_commands = &commands;
}

ScopedGpuScope::~ScopedGpuScope() noexcept
{
	Reset();
}

ScopedGpuScope::ScopedGpuScope(ScopedGpuScope&& other) noexcept :
    m_commands(std::exchange(other.m_commands, nullptr))
{
}

ScopedGpuScope& ScopedGpuScope::operator=(ScopedGpuScope&& other) noexcept
{
	if (this != &other)
	{
		Reset();
		m_commands = std::exchange(other.m_commands, nullptr);
	}

	return *this;
}

void ScopedGpuScope::Reset() noexcept
{
	if (m_commands != nullptr)
	{
		m_commands->EndDiagnosticScope();
		m_commands = nullptr;
	}
}
