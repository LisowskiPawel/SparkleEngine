#pragma once

#include <cstdint>

// Client-area pixel coordinates and relative pixel movement are distinct values.
struct MousePosition final
{
	std::int32_t X = 0;
	std::int32_t Y = 0;
};

struct MouseDelta final
{
	std::int32_t X = 0;
	std::int32_t Y = 0;
};
