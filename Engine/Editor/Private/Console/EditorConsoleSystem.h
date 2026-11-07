#pragma once

#include <cstdint>
#include <memory>

struct ConsoleCommandActions;
class ConsoleSession;
class EditorConsolePanel;
struct ConsoleOutputRecord;

class EditorConsoleSystem final
{
public:
	explicit EditorConsoleSystem(ConsoleCommandActions actions);
	~EditorConsoleSystem() noexcept;

	EditorConsoleSystem(const EditorConsoleSystem&) = delete;
	EditorConsoleSystem& operator=(const EditorConsoleSystem&) = delete;
	EditorConsoleSystem(EditorConsoleSystem&&) = delete;
	EditorConsoleSystem& operator=(EditorConsoleSystem&&) = delete;

	void AppendOutput(ConsoleOutputRecord record);
	void OpenConsole() noexcept;
	bool HandleShortcut(std::uint32_t message, std::uintptr_t key, bool bWantsTextInput) noexcept;
	float GetDockHeight(float availableHeight) noexcept;
	void BuildDockedUI(float left, float bottom, float width, float availableHeight, bool bDisableInteraction);

private:
	std::unique_ptr<ConsoleSession> m_session;
	std::unique_ptr<EditorConsolePanel> m_consolePanel;
	float m_dockHeight = 300.0f;
};
