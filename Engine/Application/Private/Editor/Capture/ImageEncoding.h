#pragma once

#include "RHI/Public/Formats/PixelFormat.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

struct ImageBufferView final
{
	std::span<const std::byte> Pixels;
	std::uint32_t Width = 0u;
	std::uint32_t Height = 0u;
	std::uint32_t RowPitch = 0u;
	PixelFormat Format = PixelFormat::Unknown;
};

struct LinearRgbImage final
{
	std::uint32_t Width = 0u;
	std::uint32_t Height = 0u;
	std::vector<float> Pixels;
};

namespace ImageEncoding
{
	bool DecodeLinearRgb(const ImageBufferView& source, LinearRgbImage& image, std::string& errorMessage);
	bool EncodeBmp(const ImageBufferView& image, std::vector<std::byte>& encodedBytes, std::string& errorMessage);
	bool EncodeExr(const LinearRgbImage& image, std::vector<std::byte>& encodedBytes, std::string& errorMessage);
}
