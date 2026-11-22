#include "PCH.h"

#include "Gltf/GltfAccessorReader.h"

#include "Core/Public/Diagnostics/Error.h"

#include <cgltf.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <format>
#include <limits>

namespace GltfAccessorReader
{
	template <std::size_t ComponentCount> static std::array<cgltf_float, ComponentCount> ReadFloatElement(const cgltf_accessor* accessor, std::size_t index)
	{
		std::array<cgltf_float, ComponentCount> element{};
		const bool bElementAvailable = accessor != nullptr && index < accessor->count;
		const bool bElementDecoded = bElementAvailable && cgltf_accessor_read_float(accessor, index, element.data(), element.size());
		if (!bElementDecoded)
		{
			throw Diagnostics::Error(std::format("Cannot decode glTF accessor element {} with {} components.", index, ComponentCount));
		}
		const bool bElementFinite = std::ranges::all_of(element, [](cgltf_float value) { return std::isfinite(value); });
		if (!bElementFinite)
		{
			throw Diagnostics::Error(std::format("glTF accessor element {} contains a non-finite value.", index));
		}
		return element;
	}

	std::vector<std::uint32_t> ReadIndices(const cgltf_accessor* accessor)
	{
		if (accessor == nullptr || accessor->count > (std::numeric_limits<std::uint32_t>::max)())
		{
			throw Diagnostics::Error("glTF index accessor is missing or exceeds the engine index range.");
		}

		std::vector<std::uint32_t> indices(accessor->count);
		if (accessor->buffer_view == nullptr && !accessor->is_sparse)
		{
			return indices;
		}
		const cgltf_size decodedCount = cgltf_accessor_unpack_indices(accessor, indices.data(), sizeof(std::uint32_t), indices.size());
		if (decodedCount != indices.size())
		{
			throw Diagnostics::Error("Cannot decode glTF index accessor data.");
		}
		return indices;
	}

	DirectX::XMFLOAT2 ReadFloat2(const cgltf_accessor* accessor, std::size_t index)
	{
		const auto element = ReadFloatElement<2>(accessor, index);
		return {element[0], element[1]};
	}

	DirectX::XMFLOAT3 ReadFloat3(const cgltf_accessor* accessor, std::size_t index)
	{
		const auto element = ReadFloatElement<3>(accessor, index);
		return {element[0], element[1], element[2]};
	}

	DirectX::XMFLOAT4 ReadFloat4(const cgltf_accessor* accessor, std::size_t index)
	{
		const auto element = ReadFloatElement<4>(accessor, index);
		return {element[0], element[1], element[2], element[3]};
	}

	DirectX::XMMATRIX ReadFloat4x4(const cgltf_accessor* accessor, std::size_t index)
	{
		const auto element = ReadFloatElement<16>(accessor, index);
		const DirectX::XMFLOAT4X4 matrix(element.data());
		return DirectX::XMLoadFloat4x4(&matrix);
	}
}
