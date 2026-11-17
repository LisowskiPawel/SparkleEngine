#include "ToolWorkProgressWriter.h"
#include "ToolWorkProgress.h"

#include "Core/Public/Diagnostics/Error.h"

#include <cstddef>
#include <limits>
#include <mutex>
#include <ostream>
#include <string>
#include <string_view>

static std::size_t ProgressPercentage(std::size_t completed, std::size_t total)
{
	if (total == 0)
	{
		return std::numeric_limits<std::size_t>::max();
	}
	if (completed > total)
	{
		throw Diagnostics::Error("Tool progress count exceeds its total.");
	}

	std::size_t lower = 0;
	std::size_t upper = 100;
	while (lower < upper)
	{
		const std::size_t percentage = (lower + upper + 1) / 2;
		// Divide before multiplying to keep the threshold exact over the full size_t range.
		const std::size_t threshold = (total / 100) * percentage + ((total % 100) * percentage + 99) / 100;
		if (completed >= threshold)
		{
			lower = percentage;
		}
		else
		{
			upper = percentage - 1;
		}
	}
	return lower;
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
	const std::size_t percentage = ProgressPercentage(completed, total);
	std::scoped_lock lock(m_mutex);
	const bool bSamePhase = m_lastAction == action && m_lastTotal == total;
	if (bSamePhase && (total == 0 || percentage <= m_lastPercentage))
	{
		return;
	}

	std::string phase(action);
	if (!item.empty())
	{
		phase += ": ";
		phase += item;
	}
	const std::string record = FormatToolWorkProgress(phase, completed, total);
	m_output << record << std::flush;
	m_lastAction = action;
	m_lastTotal = total;
	m_lastPercentage = percentage;
}
