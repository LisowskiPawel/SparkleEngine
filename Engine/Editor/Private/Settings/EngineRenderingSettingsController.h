#pragma once

#include "Renderer/Public/Settings/EngineRenderingSettings.h"

#include <functional>
#include <string>

class EngineRenderingSettingsController final
{
public:
	using CommitHandler = std::function<void(EngineRenderingSettingsState)>;

	using RefreshHandler = std::function<EngineRenderingSettingsState()>;

	EngineRenderingSettingsController(EngineRenderingSettingsState state, CommitHandler commitHandler, RefreshHandler refreshHandler);

	const EngineRenderingSettingsState& GetState() const noexcept { return m_state; }

	void RefreshFromRuntimeState() noexcept;

	bool HasPendingRestart() const noexcept;

	const std::string& GetLastError() const noexcept { return m_lastError; }

	std::string BuildPendingRestartMessage() const;

	void ResetDefaults();

	template <typename TValue> void Set(TValue EngineRenderingSettingsState::* field, TValue value)
	{
		TValue& destination = m_state.*field;
		if (destination == value)
		{
			return;
		}
		const TValue previous = destination;
		destination = value;
		if (!CommitState())
		{
			destination = previous;
		}
	}

private:
	bool CommitState();

	EngineRenderingSettingsState m_state{};
	std::string m_lastError;
	PixelFormat m_sessionBackBufferFormat = RhiPresentationDefaults::DefaultBackBufferFormat;
	bool m_bSessionPreferHighPerformanceAdapter = true;
#if SPARKLE_TARGET_EDITOR && !SPARKLE_BUILD_SHIPPING
	ExternalCaptureProvider m_sessionStartupCaptureProvider = ExternalCaptureProvider::None;
#endif
	CommitHandler m_commitHandler;
	RefreshHandler m_refreshHandler;
};
