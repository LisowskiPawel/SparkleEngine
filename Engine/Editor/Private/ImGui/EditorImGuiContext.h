#pragma once

#include "Core/Public/Events/ScopedEventHandle.h"
#include "Core/Public/Input/Keyboard/Key.h"

#include <functional>
#include <string>

class Window;
struct WindowMessageEvent;
struct ImGuiContext;

class EditorImGuiContext final
{
public:
	using ShortcutHandler = std::function<bool(Key)>;

	explicit EditorImGuiContext(Window& window) noexcept;
	~EditorImGuiContext() noexcept;
	EditorImGuiContext(const EditorImGuiContext&) = delete;
	EditorImGuiContext& operator=(const EditorImGuiContext&) = delete;
	EditorImGuiContext(EditorImGuiContext&&) = delete;
	EditorImGuiContext& operator=(EditorImGuiContext&&) = delete;

	void Initialize();
	bool InitializePlatformBackend();
	bool IsReady() const noexcept;
	void ApplyDpiScale(float dpiScale) noexcept;
	void SubscribeToWindowEvents(ShortcutHandler shortcutHandler);
	void UnsubscribeFromWindowEvents() noexcept;
	void BeginFrame(float deltaSeconds);

private:
	void HandleWindowMessage(WindowMessageEvent& event);

	Window& m_window;
	ImGuiContext* m_context = nullptr;
	bool m_bPlatformInitialized = false;
	std::string m_iniPath;
	ShortcutHandler m_shortcutHandler;
	ScopedEventHandle m_windowMessageHandle;
	ScopedEventHandle m_windowDpiScaleHandle;
};
