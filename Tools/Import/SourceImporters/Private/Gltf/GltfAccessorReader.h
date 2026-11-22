#pragma once

#include <DirectXMath.h>

#include <cstddef>
#include <cstdint>
#include <vector>

struct cgltf_accessor;

class GltfAccessorReader final
{
public:
	static std::vector<std::uint32_t> ReadIndices(const cgltf_accessor* accessor);
	static DirectX::XMFLOAT2 ReadFloat2(const cgltf_accessor* accessor, std::size_t index);
	static DirectX::XMFLOAT3 ReadFloat3(const cgltf_accessor* accessor, std::size_t index);
	static DirectX::XMFLOAT4 ReadFloat4(const cgltf_accessor* accessor, std::size_t index);
	static DirectX::XMMATRIX ReadFloat4x4(const cgltf_accessor* accessor, std::size_t index);
};
