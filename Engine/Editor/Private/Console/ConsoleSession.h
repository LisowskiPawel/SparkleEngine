#pragma once

#include "Console/ConsoleCommandRegistry.h"
#include "Console/ConsoleHistoryBuffer.h"
#include "Core/Public/Console/ConsoleOutput.h"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class ConsoleSession final
{
public:
	explicit ConsoleSession(ConsoleCommandRegistry& commandRegistry);

	ConsoleSession(const ConsoleSession&) = delete;
	ConsoleSession& operator=(const ConsoleSession&) = delete;
	ConsoleSession(ConsoleSession&&) = delete;
	ConsoleSession& operator=(ConsoleSession&&) = delete;

	void SubmitLine(std::string_view line);
	std::vector<std::string> CompleteLine(std::string_view line) const;

	void Append(ConsoleOutputRecord record);
	void AddOutput(ConsoleCommandSeverity severity, std::string text);
	void ClearOutput() noexcept;

	std::optional<std::string> NavigateHistoryPrevious(std::string_view currentLine);
	std::optional<std::string> NavigateHistoryNext();

	const std::vector<ConsoleOutputRecord>& GetOutputRecords() const noexcept { return m_outputRecords; }

private:
	static constexpr std::size_t kMaxOutputRecords = 512;

	ConsoleCommandRegistry& m_commandRegistry;
	ConsoleHistoryBuffer m_history;
	std::vector<ConsoleOutputRecord> m_outputRecords;
};
