#include "PCH.h"

#include "RayTracing/RhiRayTracingTransformPacking.h"

std::array<float, 12> RhiRayTracingTransformPacking::PackCanonicalObjectToWorld(const DirectX::XMFLOAT4X4& worldMatrix) noexcept
{
	return {
	    worldMatrix._11,
	    worldMatrix._21,
	    worldMatrix._31,
	    worldMatrix._41,
	    worldMatrix._12,
	    worldMatrix._22,
	    worldMatrix._32,
	    worldMatrix._42,
	    worldMatrix._13,
	    worldMatrix._23,
	    worldMatrix._33,
	    worldMatrix._43};
}
