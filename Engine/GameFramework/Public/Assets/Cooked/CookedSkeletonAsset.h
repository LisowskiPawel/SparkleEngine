#pragma once

#include "GameFramework/Public/Assets/Cooked/CookedAssetCommon.h"

#include <DirectXMath.h>

#include <cstdint>
#include <limits>
#include <type_traits>

namespace Assets
{
	inline constexpr std::uint32_t kCookedSkeletonAssetMagic = 0x4C4B5353u;
	inline constexpr std::uint32_t kInvalidCookedSkeletonJointIndex = (std::numeric_limits<std::uint32_t>::max)();
	inline constexpr std::uint32_t kMaxAnimationEvaluationJoints = 1024;

	struct SPARKLE_ENGINE_API CookedSkeletonJointRecord
	{
		char name[64] = {};
		std::uint32_t sourceNodeIndex = (std::numeric_limits<std::uint32_t>::max)();
		std::uint32_t evaluationJointIndex = kInvalidCookedSkeletonJointIndex;
		DirectX::XMFLOAT4X4 inverseBindMatrix;
	};

	struct SPARKLE_ENGINE_API CookedSkeletonEvaluationJointRecord
	{
		std::uint32_t parentJointIndex = kInvalidCookedSkeletonJointIndex;
		DirectX::XMFLOAT3 translation = {};
		DirectX::XMFLOAT4 rotation = {0.0f, 0.0f, 0.0f, 1.0f};
		DirectX::XMFLOAT3 scale = {1.0f, 1.0f, 1.0f};
	};

	struct SPARKLE_ENGINE_API CookedSkeletonAssetHeader
	{
		CookedAssetHeader fileHeader{kCookedSkeletonAssetMagic};
		std::uint32_t jointCount = 0;
		std::uint32_t jointStride = sizeof(CookedSkeletonJointRecord);
		std::uint32_t evaluationJointCount = 0;
		std::uint32_t evaluationJointStride = sizeof(CookedSkeletonEvaluationJointRecord);
		std::uint32_t flags = 0;
	};
}

static_assert(std::is_trivially_copyable_v<Assets::CookedSkeletonJointRecord>, "CookedSkeletonJointRecord must stay trivially copyable.");
static_assert(std::is_trivially_copyable_v<Assets::CookedSkeletonAssetHeader>, "CookedSkeletonAssetHeader must stay trivially copyable.");
static_assert(std::is_trivially_copyable_v<Assets::CookedSkeletonEvaluationJointRecord>);
