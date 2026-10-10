#include "PCH.h"

#include "GameConsole/GameConsoleHost.h"

#include "GameConsole/GameConsoleOverlay.h"

#include <utility>

GameConsoleHost::GameConsoleHost(Timer& timer, Window& window, CVarControlExecutor executor)
{
	m_overlay = std::make_unique<GameConsoleOverlay>(timer, window, std::move(executor));
}

GameConsoleHost::~GameConsoleHost() noexcept = default;

std::optional<UiRenderPacket> GameConsoleHost::Update()
{
	m_overlay->Update();
	if (m_overlay->IsVisible())
	{
		return m_overlay->ConsumeRenderPacket();
	}
	return std::nullopt;
}
