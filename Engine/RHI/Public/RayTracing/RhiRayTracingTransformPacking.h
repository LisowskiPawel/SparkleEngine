#pragma once

#include "../RHIAPI.h"

#include <DirectXMath.h>

#include <array>

namespace RhiRayTracingTransformPacking
{
	// D3D12 and Vulkan acceleration-structure instances use a row-major 3x4
	// column-vector affine transform. Sparkle stores canonical transforms for
	// row-vector multiplication, so the native payload is the affine transpose.
	SPARKLE_RHI_API std::array<float, 12> PackCanonicalObjectToWorld(const DirectX::XMFLOAT4X4& worldMatrix) noexcept;
}
