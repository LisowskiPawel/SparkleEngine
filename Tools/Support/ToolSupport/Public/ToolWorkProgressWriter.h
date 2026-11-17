#pragma once

#include "ToolWorkProgressEvent.h"

#include <cstddef>
#include <iosfwd>
#include <limits>
#include <mutex>
#include <string>
#include <string_view>

class ToolWorkProgressWriter final
{
public:
	explicit ToolWorkProgressWriter(std::ostream& output) noexcept;
	void Report(std::string_view phase, std::size_t completed = 0, std::size_t total = 0);
	void Report(std::string_view action, std::string_view item, std::size_t completed, std::size_t total);
	void Report(const ToolWorkProgressEvent& progress);

private:
	std::ostream& m_output;
	std::mutex m_mutex;
	std::string m_lastAction;
	std::size_t m_lastTotal = 0;
	std::size_t m_lastPercentage = std::numeric_limits<std::size_t>::max();
};
