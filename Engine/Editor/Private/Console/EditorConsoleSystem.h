#pragma once

#include "Core/Public/Input/Keyboard/Key.h"
#include <functional>
#include <memory>

struct ConsoleCommandActions;
class ConsoleSession;
class EditorConsolePanel;
struct ConsoleOutputRecord;
struct ConsoleCommandResult;

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
	void SetShaderCompileHandler(std::function<ConsoleCommandResult(bool)> handler);
	void OpenConsole() noexcept;
	bool HandleShortcut(Key key) noexcept;
	float GetDockHeight(float availableHeight) noexcept;
	void BuildDockedUI(float left, float bottom, float width, float availableHeight, bool bDisableInteraction);

private:
	std::unique_ptr<ConsoleSession> m_session;
	std::unique_ptr<EditorConsolePanel> m_consolePanel;
};
