#pragma once

#include <cstddef>
#include <functional>
#include <string_view>

struct ToolWorkProgressEvent final
{
	std::string_view Action;
	std::string_view Item;
	std::size_t Completed = 0;
	std::size_t Total = 0;
};

// Batch workers may report concurrently or out of completion order; sinks serialize/coalesce their own state.
// Action and Item are borrowed only for the duration of the callback. Progress never publishes work results.
using ToolWorkProgressCallback = std::function<void(const ToolWorkProgressEvent&)>;
