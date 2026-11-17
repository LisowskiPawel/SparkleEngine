#include "ToolWorkProgress.h"

#include "Core/Public/Diagnostics/Error.h"

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

static constexpr std::string_view kWorkProgressPrefix = "[PROGRESS] ";
static constexpr std::string_view kIndeterminateProgressPrefix = "? ";

static std::optional<std::size_t> ParseProgressCount(std::string_view text);

std::string FormatToolWorkProgress(std::string_view phase, std::size_t completed, std::size_t total)
{
	if (phase.empty())
	{
		throw Diagnostics::Error("Tool progress phase is empty.");
	}
	if (total != 0 && completed > total)
	{
		throw Diagnostics::Error("Tool progress count exceeds its total.");
	}

	std::string output(kWorkProgressPrefix);
	if (total == 0)
	{
		output += kIndeterminateProgressPrefix;
	}
	else
	{
		output += std::to_string(completed) + '/' + std::to_string(total) + ' ';
	}

	std::string label(phase);
	std::ranges::replace(label, '\r', ' ');
	std::ranges::replace(label, '\n', ' ');
	output += label;
	output += '\n';
	return output;
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
	const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
	if (error != std::errc{} || end != text.data() + text.size())
	{
		return std::nullopt;
	}

	return value;
}
