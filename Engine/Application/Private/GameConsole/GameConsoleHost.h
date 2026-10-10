#pragma once

#include "Core/Public/Console/CVarControl.h"
#include "Renderer/Public/UI/UiRenderPacket.h"

#include <memory>
#include <optional>

class GameConsoleOverlay;
class Timer;
class Window;

class GameConsoleHost final
{
public:
	GameConsoleHost(Timer& timer, Window& window, CVarControlExecutor executor);
	~GameConsoleHost() noexcept;

	GameConsoleHost(const GameConsoleHost&) = delete;
	GameConsoleHost& operator=(const GameConsoleHost&) = delete;
	GameConsoleHost(GameConsoleHost&&) = delete;
	GameConsoleHost& operator=(GameConsoleHost&&) = delete;

	std::optional<UiRenderPacket> Update();

private:
	std::unique_ptr<GameConsoleOverlay> m_overlay;
};
