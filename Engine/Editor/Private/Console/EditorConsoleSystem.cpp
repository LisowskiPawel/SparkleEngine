#include "PCH.h"

#include "Console/EditorConsoleSystem.h"

#include "Console/ConsoleCommands.h"
#include "Console/ConsoleSession.h"
#include "Console/EditorConsolePanel.h"
#include "Core/Public/Console/ConsoleOutput.h"

#include <utility>

EditorConsoleSystem::EditorConsoleSystem(ConsoleCommandActions actions) :
    m_session(std::make_unique<ConsoleSession>(std::move(actions))),
    m_consolePanel(std::make_unique<EditorConsolePanel>(*m_session))
{
}

EditorConsoleSystem::~EditorConsoleSystem() noexcept = default;

void EditorConsoleSystem::AppendOutput(ConsoleOutputRecord record)
{
	m_session->Append(std::move(record));
}

void EditorConsoleSystem::SetShaderCompileHandler(std::function<ConsoleCommandResult(bool)> handler)
{
	m_session->SetShaderCompileHandler(std::move(handler));
}

void EditorConsoleSystem::OpenConsole() noexcept
{
	m_consolePanel->RequestFocus();
}

bool EditorConsoleSystem::HandleShortcut(Key key) noexcept
{
	if (key != Key::Grave)
	{
		return false;
	}
	m_consolePanel->ToggleOpen();
	return true;
}

float EditorConsoleSystem::GetDockHeight(float availableHeight) noexcept
{
	return m_consolePanel->GetDockHeight(availableHeight);
}

void EditorConsoleSystem::BuildDockedUI(float left, float bottom, float width, float availableHeight, bool bDisableInteraction)
{
	m_consolePanel->BuildDockedUI(left, bottom, width, availableHeight, bDisableInteraction);
}
