#pragma once

namespace MaterialTextureFlags
{
	// Shared C++/HLSL material flag ABI.
#ifdef __cplusplus
	using FlagBits = unsigned int;
#else
	typedef uint FlagBits;
#endif
	static const FlagBits RgbNormal = 1u << 16u;
}
