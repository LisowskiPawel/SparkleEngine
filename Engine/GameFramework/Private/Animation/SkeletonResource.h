#pragma once

#include "GameFramework/Public/Assets/Cooked/CookedAssetCommon.h"
#include "Core/Public/Math/MathUtils.h"

#include <ozz/animation/runtime/skeleton.h>

#include <cstdint>
#include <string>
#include <vector>

struct SkeletonJoint final
{
	std::string name;
	std::uint32_t sourceNodeIndex = 0;
	std::uint32_t evaluationJointIndex = 0;
	DirectX::XMFLOAT4X4 inverseBindMatrix = MathUtils::IdentityFloat4x4();
};

struct SkeletonResource final
{
	Assets::CookedAssetId assetId = Assets::InvalidCookedAssetId;
	std::uint32_t sourceSkinIndex = 0;
	std::vector<SkeletonJoint> joints;
	ozz::animation::Skeleton evaluation;
};
