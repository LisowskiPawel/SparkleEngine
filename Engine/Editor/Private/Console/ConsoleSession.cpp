#include "PCH.h"

#include "Console/ConsoleSession.h"

#include "Core/Public/Strings/StringUtils.h"

ConsoleSession::ConsoleSession(ConsoleCommandActions actions) :
    m_actions(std::move(actions))
{
}

void ConsoleSession::SubmitLine(std::string_view line)
{
	const std::string_view trimmedLine = Strings::TrimAsciiWhitespace(line);
	if (trimmedLine.empty())
	{
		return;
	}

	m_history.Add(trimmedLine);
	AddOutput(ConsoleCommandSeverity::Info, "> " + std::string(trimmedLine));

	const ConsoleCommandResult result = ExecuteConsoleLine(m_actions, trimmedLine);
	if (!result.Message.empty())
	{
		AddOutput(result.Severity, result.Message);
	}
	else if (!result.bSucceeded)
	{
		AddOutput(ConsoleCommandSeverity::Error, "command failed");
	}
}

std::vector<std::string> ConsoleSession::CompleteLine(std::string_view line) const
{
	return CompleteConsoleLine(line);
}

void ConsoleSession::Append(ConsoleOutputRecord record)
{
	m_outputRecords.push_back(std::move(record));
	while (m_outputRecords.size() > kMaxOutputRecords)
	{
		m_outputRecords.erase(m_outputRecords.begin());
	}
}

void ConsoleSession::AddOutput(ConsoleCommandSeverity severity, std::string text)
{
	Append(ConsoleOutputRecord{.Severity = severity, .Text = std::move(text)});
}

void ConsoleSession::ClearOutput() noexcept
{
	m_outputRecords.clear();
}

std::optional<std::string> ConsoleSession::NavigateHistoryPrevious(std::string_view currentLine)
{
	return m_history.NavigatePrevious(currentLine);
}

std::optional<std::string> ConsoleSession::NavigateHistoryNext()
{
	return m_history.NavigateNext();
}
