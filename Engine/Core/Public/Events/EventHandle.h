#pragma once

#include <cstdint>

struct EventHandle
{
	std::uint32_t Id = 0;

	bool IsValid() const noexcept { return Id != 0; }

	void Invalidate() noexcept { Id = 0; }

	bool operator==(const EventHandle&) const noexcept = default;
};
