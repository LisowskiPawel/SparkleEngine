#include "PCH.h"

#include "Commands/RhiCommandSubmissionService.h"

#include <array>

RhiCommandSubmissionService::~RhiCommandSubmissionService() noexcept = default;

RhiSubmissionState RhiCommandSubmissionService::CaptureLastSubmittedState() const noexcept
{
	RhiSubmissionState state;
	for (std::size_t queueIndex = 0; queueIndex < RhiQueueTypeCount; ++queueIndex)
	{
		state.MarkUsed(GetLastSubmittedToken(static_cast<ERhiQueueType>(queueIndex)));
	}
	return state;
}

bool RhiCommandSubmissionService::IsSubmissionStateComplete(const RhiSubmissionState& state) const noexcept
{
	std::array<RhiSubmissionToken, RhiQueueTypeCount> tokens{};
	const std::size_t tokenCount = state.CopyTokens(tokens);
	for (std::size_t tokenIndex = 0; tokenIndex < tokenCount; ++tokenIndex)
	{
		if (!IsSubmissionComplete(tokens[tokenIndex]))
		{
			return false;
		}
	}
	return true;
}
