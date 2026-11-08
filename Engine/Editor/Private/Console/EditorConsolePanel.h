#pragma once

#include <cstddef>
#include <array>
#include <string>
#include <string_view>
#include <vector>

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
	static int HandleInputTextCallback(ImGuiInputTextCallbackData* data);
	static std::size_t FindCompletionTokenStart(const std::string& input) noexcept;
	static void ReplaceInputText(ImGuiInputTextCallbackData& data, const std::string& text);

	int HandleHistoryCallback(ImGuiInputTextCallbackData& data);
	int HandleCompletionCallback(ImGuiInputTextCallbackData& data);
	void SubmitInput();
	void DrawToolbar(bool bDisableInteraction);
	void DrawScrollback();
	void DrawInputLine(bool bDisableInteraction);
	void DrawAutocompletePreview();
	void BuildOutputTextBuffer(std::string_view filter);
	std::vector<std::string> GetCurrentCompletions() const;
	std::string BuildCompletionList(const std::vector<std::string>& completions) const;

	ConsoleSession& m_session;
	std::string m_outputTextBuffer;
	std::array<char, 512> m_inputBuffer{};
	std::array<char, 128> m_filterBuffer{};
	float m_dockHeight = 300.0f;
	bool m_bIsOpen = false;
	bool m_bFocusInput = false;
};
