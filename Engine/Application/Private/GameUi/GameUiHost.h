#pragma once

#include "Core/Public/Events/ScopedEventHandle.h"
#include "Core/Public/Console/CVarControl.h"
#include "Renderer/Public/UI/UiRenderPacket.h"

#include <functional>
#include <memory>
#include <string>

class Timer;
class Window;
class LevelSession;
class GameConsoleOverlay;
class ImGuiRenderPacketBuilder;
struct WindowMessageEvent;

class GameUiHost final
{
public:
	GameUiHost(Timer& timer, Window& window, LevelSession& levels, std::function<void()> releaseMouse, CVarControlExecutor consoleExecutor);
	~GameUiHost() noexcept;
	UiRenderPacket Update();
	bool WantsInputCapture() const noexcept;

private:
	void HandleWindowMessage(WindowMessageEvent& event);
	void DrawMenu();
	void DrawLevelSelection();
	void DrawControls();
	void Shutdown() noexcept;
	void OpenMenu();

	Timer& m_timer;
	Window& m_window;
	LevelSession& m_levels;
	std::function<void()> m_releaseMouse;
	ScopedEventHandle m_windowMessageHandle;
	ScopedEventHandle m_dpiHandle;
	ScopedEventHandle m_levelFailureHandle;
	std::unique_ptr<ImGuiRenderPacketBuilder> m_packetBuilder;
#if !SPARKLE_BUILD_SHIPPING
	std::unique_ptr<GameConsoleOverlay> m_console;
#endif
	bool m_menuOpen = true;
	bool m_contextReady = false;
	bool m_backendReady = false;
};
