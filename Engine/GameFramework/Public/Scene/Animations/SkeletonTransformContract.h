#pragma once

#include "GameFramework/Public/GameFrameworkAPI.h"

#include <DirectXMath.h>

#include <cstdint>
#include <limits>
#include <vector>

namespace SkeletonTransformContract
{
	template <typename JointRange> bool BuildEvaluationOrder(const JointRange& joints, std::vector<std::uint32_t>& order)
	{
		order.clear();
		order.reserve(joints.size());
		std::vector<std::uint8_t> states(joints.size(), 0u);
		std::vector<std::uint32_t> path;
		path.reserve(joints.size());

		for (std::uint32_t jointIndex = 0; jointIndex < joints.size(); ++jointIndex)
		{
			if (states[jointIndex] == 2u)
			{
				continue;
			}

			path.clear();
			std::uint32_t current = jointIndex;
			while (current < joints.size() && states[current] == 0u)
			{
				states[current] = 1u;
				path.push_back(current);
				const std::uint32_t parent = joints[current].parentJointIndex;
				if (parent != (std::numeric_limits<std::uint32_t>::max)() && parent >= joints.size())
				{
					return false;
				}
				current = parent;
			}
			if (current < joints.size() && states[current] == 1u)
			{
				return false;
			}

			while (!path.empty())
			{
				const std::uint32_t resolvedJoint = path.back();
				path.pop_back();
				states[resolvedJoint] = 2u;
				order.push_back(resolvedJoint);
			}
		}
		return order.size() == joints.size();
	}

	SPARKLE_ENGINE_API bool IsFinite(const DirectX::XMFLOAT4X4& matrix) noexcept;
	SPARKLE_ENGINE_API bool IsInvertible(const DirectX::XMFLOAT4X4& matrix, float epsilon = 1.0e-8f) noexcept;
	SPARKLE_ENGINE_API bool IsTrsDecomposable(const DirectX::XMFLOAT4X4& matrix) noexcept;
	SPARKLE_ENGINE_API bool MatricesNear(DirectX::FXMMATRIX lhs, DirectX::CXMMATRIX rhs, float epsilon = 2.0e-3f) noexcept;
	SPARKLE_ENGINE_API bool SatisfiesBindInvariant(
	    const DirectX::XMFLOAT4X4& bindLocal,
	    const DirectX::XMFLOAT4X4& parentSpace,
	    const DirectX::XMFLOAT4X4* parentBindModel,
	    const DirectX::XMFLOAT4X4& bindModel) noexcept;
}
