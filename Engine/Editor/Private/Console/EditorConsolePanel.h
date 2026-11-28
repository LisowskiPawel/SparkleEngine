#pragma once

#include <array>
#include <string>
#include <string_view>

class ConsoleSession;
struct ImGuiInputTextCallbackData;

class EditorConsolePanel final
{
public:
	explicit EditorConsolePanel(ConsoleSession& session) noexcept;

	void ToggleOpen() noexcept;
	float GetDockHeight(float availableHeight) noexcept;
	void BuildDockedUI(float left, float bottom, float width, float availableHeight, bool bDisableInteraction);

	void RequestFocus() noexcept;

private:
	void BuildContent(bool bDisableInteraction);
	bool BeginDockWindow(float left, float bottom, float width, float availableHeight, float height);
	void EndDockWindow();
	void DrawDockHeader(bool bDisableInteraction);
	void UpdateDockInteraction(float availableHeight, bool bDisableInteraction);
	void Clear() noexcept;
	static int HandleInputHistory(ImGuiInputTextCallbackData* data);
	static void ReplaceInputText(ImGuiInputTextCallbackData& data, const std::string& text);

	void SubmitInput();
	void DrawToolbar(bool bDisableInteraction);
	void DrawScrollback();
	void DrawInputLine(bool bDisableInteraction);
	void BuildOutputTextBuffer(std::string_view filter);

	ConsoleSession& m_session;
	std::string m_outputTextBuffer;
	std::array<char, 512> m_inputBuffer{};
	std::array<char, 128> m_filterBuffer{};
	float m_dockHeight = 300.0f;
	bool m_bIsOpen = false;
	bool m_bFocusInput = false;
};
