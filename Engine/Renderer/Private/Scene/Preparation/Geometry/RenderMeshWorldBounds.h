#pragma once

#include <DirectXMath.h>

struct MeshDrawGeometry;

struct RenderMeshWorldBounds final
{
	DirectX::XMFLOAT3 Min = {};
	DirectX::XMFLOAT3 Max = {};
	bool bValid = false;

	static RenderMeshWorldBounds Transform(const MeshDrawGeometry& geometry, const DirectX::XMFLOAT4X4& worldMatrix) noexcept;
	void Expand(const DirectX::XMFLOAT3& point) noexcept;
	DirectX::XMFLOAT3 GetCenter() const noexcept;
};
