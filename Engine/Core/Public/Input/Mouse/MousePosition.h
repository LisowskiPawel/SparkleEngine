#pragma once

// Fractional client coordinates and system-scaled relative movement are distinct values.
struct MousePosition final
{
	float X = 0.0f;
	float Y = 0.0f;
};

struct MouseDelta final
{
	float X = 0.0f;
	float Y = 0.0f;
};
