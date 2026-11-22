#pragma once

#include "Core/Public/Events/ScopedEventHandle.h"
#include "Core/Public/Input/Keyboard/Key.h"

#include <functional>
#include <string>

class Window;
class InputSystem;
enum class InputCancellation : std::uint8_t;
struct SdlWindowEvent;
struct ImGuiContext;

class EditorImGuiContext final
{
public:
	using ShortcutHandler = std::function<bool(Key)>;

	explicit EditorImGuiContext(Window& window, InputSystem& inputSystem) noexcept;
	~EditorImGuiContext() noexcept;
	EditorImGuiContext(const EditorImGuiContext&) = delete;
	EditorImGuiContext& operator=(const EditorImGuiContext&) = delete;
	EditorImGuiContext(EditorImGuiContext&&) = delete;
	EditorImGuiContext& operator=(EditorImGuiContext&&) = delete;

	void Initialize();
	bool InitializePlatformBackend();
	void ApplyDpiScale(float dpiScale) noexcept;
	void SubscribeToWindowEvents(ShortcutHandler shortcutHandler);
	void UnsubscribeFromWindowEvents() noexcept;
	void BeginFrame(float deltaSeconds);

private:
	void HandleWindowEvent(const SdlWindowEvent& event);
	void CancelInput(InputCancellation cancellation) noexcept;

	Window& m_window;
	InputSystem& m_inputSystem;
	ImGuiContext* m_context = nullptr;
	bool m_bPlatformInitialized = false;
	bool m_bSuppressShortcutText = false;
	std::string m_iniPath;
	ShortcutHandler m_shortcutHandler;
	ScopedEventHandle m_windowEventHandle;
	ScopedEventHandle m_windowDpiScaleHandle;
	ScopedEventHandle m_windowCancellationHandle;
};
