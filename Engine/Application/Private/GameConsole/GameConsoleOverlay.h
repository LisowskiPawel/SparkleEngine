#pragma once

#include "Core/Public/Console/ConsoleCommandRegistry.h"
#include "Core/Public/Console/CVarControl.h"

#include <cstddef>
#include <array>
#include <memory>
#include <string>
#include <vector>

class ConsoleSession;
struct ImGuiInputTextCallbackData;
struct ImVec4;

class GameConsoleOverlay final
{
public:
	explicit GameConsoleOverlay(CVarControlExecutor executor);
	~GameConsoleOverlay() noexcept;

	GameConsoleOverlay(const GameConsoleOverlay&) = delete;
	GameConsoleOverlay& operator=(const GameConsoleOverlay&) = delete;
	GameConsoleOverlay(GameConsoleOverlay&&) = delete;
	GameConsoleOverlay& operator=(GameConsoleOverlay&&) = delete;

	void Update();
	void ToggleVisibility() noexcept;

	bool IsVisible() const noexcept { return m_isVisible; }

private:
	void BuildUI();
	void DrawOutputRecords();
	void DrawInputLine();
	void DrawAutocompletePreview();
	void SubmitInput();
	std::vector<std::string> GetCurrentCompletions() const;
	std::string BuildCompletionList(const std::vector<std::string>& completions) const;

	int HandleHistoryCallback(ImGuiInputTextCallbackData& data);
	int HandleCompletionCallback(ImGuiInputTextCallbackData& data);

	static int HandleInputTextCallback(ImGuiInputTextCallbackData* data);
	static ImVec4 GetSeverityColor(ConsoleCommandSeverity severity) noexcept;
	static std::size_t FindCompletionTokenStart(const std::string& input) noexcept;
	static void ReplaceInputText(ImGuiInputTextCallbackData& data, const std::string& text);

	ConsoleCommandRegistry m_commandRegistry;
	std::unique_ptr<ConsoleSession> m_consoleSession;
	std::array<char, 512> m_inputBuffer{};
	std::array<char, 128> m_filterBuffer{};
	std::size_t m_seenOutputCount = 0;
	bool m_isVisible = false;
	bool m_scrollToBottom = true;
	bool m_focusInput = false;
};
