#pragma once

#include "RHI/Public/Formats/PixelFormat.h"

namespace RenderFrameGraphFormats
{
	inline constexpr PixelFormat SceneColor = PixelFormat::R32G32B32A32_Float;
	inline constexpr PixelFormat SceneDepth = PixelFormat::R32_Float;
}
