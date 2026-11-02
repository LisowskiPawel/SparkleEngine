#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class ConsoleHistoryBuffer final
{
public:
	void Add(std::string_view line);

	std::optional<std::string> NavigatePrevious(std::string_view currentLine);
	std::optional<std::string> NavigateNext();

private:
	void ResetNavigation() noexcept;

	static constexpr std::size_t kMaxEntries = 128;

	std::vector<std::string> m_entries;
	std::string m_pendingLine;
	std::size_t m_navigationIndex = 0;
	bool m_isNavigating = false;
};
