#pragma once

#include "Rendering/RenderObjectId.h"

#include <DirectXMath.h>

#include <cstdint>
#include <span>
#include <vector>

struct ResolvedRenderPrimitive;
class RenderScene;

struct RenderJointMatrixCopyRange final
{
	RenderObjectId Object;
	std::uint32_t OutputOffset = 0u;
	std::span<const DirectX::XMFLOAT4X4> Current;
	std::span<const DirectX::XMFLOAT4X4> Previous;
};

struct RenderMorphWeightCopyRange final
{
	RenderObjectId Object;
	std::uint32_t OutputOffset = 0u;
	std::uint32_t TargetCount = 0u;
	std::span<const float> Current;
	std::span<const float> Previous;
};

struct RenderDeformationWork final
{
	std::vector<RenderJointMatrixCopyRange> JointMatrixCopyRanges;
	std::vector<RenderMorphWeightCopyRange> MorphWeightCopyRanges;
	std::vector<DirectX::XMFLOAT4X4> JointMatrices;
	std::vector<DirectX::XMFLOAT4X4> PreviousJointMatrices;
	std::vector<float> MorphWeights;
	std::vector<float> PreviousMorphWeights;
};

void PrepareRenderDeformation(const RenderScene& scene, std::span<ResolvedRenderPrimitive> primitives, RenderDeformationWork& work);
void CopyRenderJointMatrixRanges(std::span<const RenderJointMatrixCopyRange> ranges, std::span<DirectX::XMFLOAT4X4> current, std::span<DirectX::XMFLOAT4X4> previous) noexcept;
void CopyRenderMorphWeightRanges(std::span<const RenderMorphWeightCopyRange> ranges, std::span<float> current, std::span<float> previous) noexcept;
