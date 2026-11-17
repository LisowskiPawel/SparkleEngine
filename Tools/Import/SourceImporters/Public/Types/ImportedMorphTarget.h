#pragma once

#include <DirectXMath.h>

#include <cstdint>
#include <string>
#include <vector>

struct ImportedMorphTargetDelta
{
	DirectX::XMFLOAT3 position = {0.0f, 0.0f, 0.0f};
	DirectX::XMFLOAT3 normal = {0.0f, 0.0f, 0.0f};
	DirectX::XMFLOAT3 tangent = {0.0f, 0.0f, 0.0f};
};

struct ImportedMorphTarget
{
	std::string name;
	float defaultWeight = 0.0f;
	std::vector<ImportedMorphTargetDelta> deltas;
};
