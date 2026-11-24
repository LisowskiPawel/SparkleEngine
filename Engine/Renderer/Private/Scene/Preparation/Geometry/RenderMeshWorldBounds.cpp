#include "PCH.h"

#include "Scene/Preparation/Geometry/RenderMeshWorldBounds.h"
#include "Scene/Geometry/MeshDraw.h"

#include <algorithm>
#include <array>

static std::array<DirectX::XMFLOAT3, 8> BuildBoundsCorners(const MeshDrawGeometry& geometry) noexcept
{
	return {
	    DirectX::XMFLOAT3{geometry.LocalBoundsMin.x, geometry.LocalBoundsMin.y, geometry.LocalBoundsMin.z},
	    DirectX::XMFLOAT3{geometry.LocalBoundsMax.x, geometry.LocalBoundsMin.y, geometry.LocalBoundsMin.z},
	    DirectX::XMFLOAT3{geometry.LocalBoundsMin.x, geometry.LocalBoundsMax.y, geometry.LocalBoundsMin.z},
	    DirectX::XMFLOAT3{geometry.LocalBoundsMax.x, geometry.LocalBoundsMax.y, geometry.LocalBoundsMin.z},
	    DirectX::XMFLOAT3{geometry.LocalBoundsMin.x, geometry.LocalBoundsMin.y, geometry.LocalBoundsMax.z},
	    DirectX::XMFLOAT3{geometry.LocalBoundsMax.x, geometry.LocalBoundsMin.y, geometry.LocalBoundsMax.z},
	    DirectX::XMFLOAT3{geometry.LocalBoundsMin.x, geometry.LocalBoundsMax.y, geometry.LocalBoundsMax.z},
	    DirectX::XMFLOAT3{geometry.LocalBoundsMax.x, geometry.LocalBoundsMax.y, geometry.LocalBoundsMax.z}};
}

static DirectX::XMFLOAT3 TransformPoint(const DirectX::XMFLOAT3& point, DirectX::FXMMATRIX worldMatrix) noexcept
{
	const DirectX::XMVECTOR localPoint = DirectX::XMLoadFloat3(&point);
	const DirectX::XMVECTOR worldPoint = DirectX::XMVector3TransformCoord(localPoint, worldMatrix);
	DirectX::XMFLOAT3 transformed;
	DirectX::XMStoreFloat3(&transformed, worldPoint);
	return transformed;
}

void RenderMeshWorldBounds::Expand(const DirectX::XMFLOAT3& point) noexcept
{
	if (!bValid)
	{
		Min = point;
		Max = point;
		bValid = true;
		return;
	}

	Min.x = (std::min) (Min.x, point.x);
	Min.y = (std::min) (Min.y, point.y);
	Min.z = (std::min) (Min.z, point.z);
	Max.x = (std::max) (Max.x, point.x);
	Max.y = (std::max) (Max.y, point.y);
	Max.z = (std::max) (Max.z, point.z);
}

RenderMeshWorldBounds RenderMeshWorldBounds::Transform(const MeshDrawGeometry& geometry, const DirectX::XMFLOAT4X4& worldMatrix) noexcept
{
	if (!geometry.bHasLocalBounds)
	{
		const DirectX::XMFLOAT3 position{worldMatrix._41, worldMatrix._42, worldMatrix._43};
		return RenderMeshWorldBounds{.Min = position, .Max = position, .bValid = true};
	}

	const DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&worldMatrix);
	const std::array<DirectX::XMFLOAT3, 8> corners = BuildBoundsCorners(geometry);

	RenderMeshWorldBounds bounds;
	for (const DirectX::XMFLOAT3& corner : corners)
	{
		const DirectX::XMFLOAT3 worldCorner = TransformPoint(corner, world);
		bounds.Expand(worldCorner);
	}
	return bounds;
}

DirectX::XMFLOAT3 RenderMeshWorldBounds::GetCenter() const noexcept
{
	return DirectX::XMFLOAT3{0.5f * (Min.x + Max.x), 0.5f * (Min.y + Max.y), 0.5f * (Min.z + Max.z)};
}
