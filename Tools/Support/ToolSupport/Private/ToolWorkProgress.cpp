#include "ToolWorkProgress.h"
#include "ToolWorkProgressEvent.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <mutex>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

static constexpr std::string_view kWorkProgressPrefix = "[PROGRESS] ";
static constexpr std::string_view kIndeterminateProgressPrefix = "? ";
static constexpr std::size_t kDecimalRadix = 10;

static std::optional<std::size_t> ParseProgressCount(std::string_view text);

static std::string FormatProgressItem(std::string_view action, std::string_view item)
{
	std::string phase(action);
	if (!item.empty())
	{
		phase += ": ";
		phase += item;
	}
	std::ranges::replace(phase, '\r', ' ');
	std::ranges::replace(phase, '\n', ' ');
	return phase;
}

std::string FormatToolWorkProgress(std::string_view phase, std::size_t completed, std::size_t total)
{
	std::ostringstream output;
	output << kWorkProgressPrefix;
	if (total == 0)
	{
		output << kIndeterminateProgressPrefix;
	}
	else
	{
		output << completed << '/' << total << ' ';
	}
	output << phase << '\n';
	return output.str();
}

std::optional<ToolWorkProgress> ParseToolWorkProgress(std::string_view line)
{
	if (line.ends_with('\r'))
	{
		line.remove_suffix(1);
	}
	if (!line.starts_with(kWorkProgressPrefix))
	{
		return std::nullopt;
	}

	line.remove_prefix(kWorkProgressPrefix.size());
	if (line.starts_with(kIndeterminateProgressPrefix))
	{
		line.remove_prefix(kIndeterminateProgressPrefix.size());
		return line.empty() ? std::nullopt : std::optional<ToolWorkProgress>(ToolWorkProgress{.phase = std::string(line)});
	}

	const std::size_t slash = line.find('/');
	const std::size_t space = line.find(' ', slash == std::string_view::npos ? 0 : slash + 1);
	if (slash == std::string_view::npos || space == std::string_view::npos || space + 1 >= line.size())
	{
		return std::nullopt;
	}

	const std::string_view completedText = line.substr(0, slash);
	const std::string_view totalText = line.substr(slash + 1, space - slash - 1);
	const std::optional<std::size_t> completed = ParseProgressCount(completedText);
	const std::optional<std::size_t> total = ParseProgressCount(totalText);
	if (!completed || !total || *total == 0 || *completed > *total)
	{
		return std::nullopt;
	}

	return ToolWorkProgress{.completed = *completed, .total = *total, .phase = std::string(line.substr(space + 1))};
}

static std::optional<std::size_t> ParseProgressCount(std::string_view text)
{
	if (text.empty())
	{
		return std::nullopt;
	}

	std::size_t value = 0;
	for (const char character : text)
	{
		if (character < '0' || character > '9')
		{
			return std::nullopt;
		}
		const std::size_t digit = static_cast<std::size_t>(character - '0');
		if (value > (std::numeric_limits<std::size_t>::max() - digit) / kDecimalRadix)
		{
			return std::nullopt;
		}
		value = (value * kDecimalRadix) + digit;
	}
	return value;
}

ToolWorkProgressWriter::ToolWorkProgressWriter(std::ostream& output) noexcept :
    m_output(output)
{
}

void ToolWorkProgressWriter::Report(std::string_view phase, std::size_t completed, std::size_t total)
{
	Report(phase, {}, completed, total);
}

void ToolWorkProgressWriter::Report(const ToolWorkProgressEvent& progress)
{
	Report(progress.Action, progress.Item, progress.Completed, progress.Total);
}

void ToolWorkProgressWriter::Report(std::string_view action, std::string_view item, std::size_t completed, std::size_t total)
{
	std::scoped_lock lock(m_mutex);
	const std::size_t percentage = total != 0 ? static_cast<std::size_t>(static_cast<double>(completed) * 100.0 / static_cast<double>(total)) : std::numeric_limits<std::size_t>::max();
	const bool samePhase = m_lastAction == action && m_lastTotal == total;
	if (samePhase && (total == 0 || percentage <= m_lastPercentage))
	{
		return;
	}

	const std::string phase = FormatProgressItem(action, item);
	const std::string record = FormatToolWorkProgress(phase, completed, total);
	m_output << record << std::flush;
	m_lastAction = action;
	m_lastTotal = total;
	m_lastPercentage = percentage;
}
