#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

struct ToolWorkProgress final
{
	std::size_t completed = 0;
	std::size_t total = 0;
	std::string phase;
};

std::string FormatToolWorkProgress(std::string_view phase, std::size_t completed = 0, std::size_t total = 0);
std::optional<ToolWorkProgress> ParseToolWorkProgress(std::string_view line);
