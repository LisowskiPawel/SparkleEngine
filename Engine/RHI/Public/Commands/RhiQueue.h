#pragma once

#include "../RHIAPI.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

enum class ERhiQueueType : std::uint8_t
{
	Graphics = 0,
	Compute,
	Copy,
	Count,
};

constexpr std::size_t RhiQueueTypeCount = static_cast<std::size_t>(ERhiQueueType::Count);

SPARKLE_RHI_API bool IsRhiQueueTypeValid(ERhiQueueType queue) noexcept;

SPARKLE_RHI_API std::size_t RhiQueueTypeToIndex(ERhiQueueType queue) noexcept;

SPARKLE_RHI_API const char* RhiQueueTypeToString(ERhiQueueType queue) noexcept;

struct RhiSubmissionToken final
{
	ERhiQueueType Queue = ERhiQueueType::Graphics;
	std::uint64_t Value = 0;

	SPARKLE_RHI_API bool IsValid() const noexcept;

	explicit operator bool() const noexcept { return IsValid(); }

	constexpr bool operator==(const RhiSubmissionToken&) const noexcept = default;
};

struct RhiSubmissionState final
{
	std::array<std::uint64_t, RhiQueueTypeCount> Values{};

	SPARKLE_RHI_API void Clear() noexcept;

	SPARKLE_RHI_API void MarkUsed(RhiSubmissionToken token) noexcept;

	SPARKLE_RHI_API RhiSubmissionToken GetToken(ERhiQueueType queue) const noexcept;

	SPARKLE_RHI_API std::size_t CopyTokens(std::span<RhiSubmissionToken> destination) const noexcept;

	SPARKLE_RHI_API bool IsComplete(const std::array<std::uint64_t, RhiQueueTypeCount>& completedValues) const noexcept;
};
