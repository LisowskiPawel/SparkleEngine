#include "PCH.h"

#include "Commands/RhiQueue.h"

bool IsRhiQueueTypeValid(ERhiQueueType queue) noexcept
{
	return static_cast<std::size_t>(queue) < RhiQueueTypeCount;
}

std::size_t RhiQueueTypeToIndex(ERhiQueueType queue) noexcept
{
	return static_cast<std::size_t>(queue);
}

const char* RhiQueueTypeToString(ERhiQueueType queue) noexcept
{
	switch (queue)
	{
		case ERhiQueueType::Graphics:
			return "Graphics";
		case ERhiQueueType::Compute:
			return "Compute";
		case ERhiQueueType::Copy:
			return "Copy";
		case ERhiQueueType::Count:
		default:
			return "Unknown";
	}
}

bool RhiSubmissionToken::IsValid() const noexcept
{
	return IsRhiQueueTypeValid(Queue) && Value != 0;
}

void RhiSubmissionState::Clear() noexcept
{
	Values.fill(0);
}

void RhiSubmissionState::MarkUsed(RhiSubmissionToken token) noexcept
{
	if (token.IsValid())
	{
		std::uint64_t& value = Values[RhiQueueTypeToIndex(token.Queue)];
		value = value < token.Value ? token.Value : value;
	}
}

RhiSubmissionToken RhiSubmissionState::GetToken(ERhiQueueType queue) const noexcept
{
	return IsRhiQueueTypeValid(queue) ? RhiSubmissionToken{.Queue = queue, .Value = Values[RhiQueueTypeToIndex(queue)]} : RhiSubmissionToken{};
}

std::size_t RhiSubmissionState::CopyTokens(std::span<RhiSubmissionToken> destination) const noexcept
{
	std::size_t count = 0;
	for (std::size_t queueIndex = 0; queueIndex < RhiQueueTypeCount && count < destination.size(); ++queueIndex)
	{
		const std::uint64_t value = Values[queueIndex];
		if (value != 0)
		{
			destination[count++] = RhiSubmissionToken{.Queue = static_cast<ERhiQueueType>(queueIndex), .Value = value};
		}
	}
	return count;
}

bool RhiSubmissionState::IsComplete(const std::array<std::uint64_t, RhiQueueTypeCount>& completedValues) const noexcept
{
	for (std::size_t queueIndex = 0; queueIndex < RhiQueueTypeCount; ++queueIndex)
	{
		if (completedValues[queueIndex] < Values[queueIndex])
		{
			return false;
		}
	}
	return true;
}
