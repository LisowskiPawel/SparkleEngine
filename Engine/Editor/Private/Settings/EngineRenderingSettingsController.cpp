#include "PCH.h"

#include "Settings/EngineRenderingSettingsController.h"

#include <sstream>
#include <utility>
#include <vector>

EngineRenderingSettingsController::EngineRenderingSettingsController(EngineRenderingSettingsState state, CommitHandler commitHandler, RefreshHandler refreshHandler) :
    m_state(std::move(state)),
    m_sessionBackBufferFormat(m_state.BackBufferFormat),
    m_bSessionPreferHighPerformanceAdapter(m_state.bPreferHighPerformanceAdapter),
    m_commitHandler(std::move(commitHandler)),
    m_refreshHandler(std::move(refreshHandler))
{
#if SPARKLE_TARGET_EDITOR && !SPARKLE_BUILD_SHIPPING
	m_sessionStartupCaptureProvider = m_state.StartupCaptureProvider;
#endif
}

void EngineRenderingSettingsController::RefreshFromRuntimeState() noexcept
{
	if (!m_refreshHandler)
	{
		return;
	}
	m_state = m_refreshHandler();
}

bool EngineRenderingSettingsController::CommitState()
{
	try
	{
		if (!m_commitHandler)
		{
			m_lastError = "Settings persistence is unavailable.";
			return false;
		}
		m_commitHandler(m_state);
		m_lastError.clear();
		return true;
	}
	catch (const std::exception& error)
	{
		m_lastError = error.what();
		return false;
	}
}

void EngineRenderingSettingsController::ResetDefaults()
{
	const auto previous = m_state;
	m_state = EngineRenderingSettingsState{};
	if (!CommitState())
	{
		m_state = previous;
	}
}

bool EngineRenderingSettingsController::HasPendingRestart() const noexcept
{
#if SPARKLE_TARGET_EDITOR && !SPARKLE_BUILD_SHIPPING
	if (m_sessionStartupCaptureProvider != m_state.StartupCaptureProvider)
	{
		return true;
	}
#endif
	return m_bSessionPreferHighPerformanceAdapter != m_state.bPreferHighPerformanceAdapter || m_sessionBackBufferFormat != m_state.BackBufferFormat;
}

std::string EngineRenderingSettingsController::BuildPendingRestartMessage() const
{
	std::vector<std::string> reasons;
#if SPARKLE_TARGET_EDITOR && !SPARKLE_BUILD_SHIPPING
	if (m_sessionStartupCaptureProvider != m_state.StartupCaptureProvider)
	{
		reasons.emplace_back("capture startup provider");
	}
#endif
	if (m_bSessionPreferHighPerformanceAdapter != m_state.bPreferHighPerformanceAdapter)
	{
		reasons.emplace_back("GPU adapter preference");
	}
	if (m_sessionBackBufferFormat != m_state.BackBufferFormat)
	{
		reasons.emplace_back("back buffer format");
	}
	if (reasons.empty())
	{
		return {};
	}

	std::ostringstream stream;
	stream << "Restart the application to apply ";
	for (std::size_t index = 0; index < reasons.size(); ++index)
	{
		if (index > 0)
		{
			stream << (index + 1 == reasons.size() ? " and " : ", ");
		}
		stream << reasons[index];
	}
	stream << ".";
	return stream.str();
}
