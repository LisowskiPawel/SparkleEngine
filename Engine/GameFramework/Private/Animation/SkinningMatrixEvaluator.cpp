#include "PCH.h"

#include "Animation/SkinningMatrixEvaluator.h"

namespace SkinningMatrixEvaluator
{
	bool Evaluate(const ECS::SkeletonEvaluationData& skeleton, std::span<const DirectX::XMFLOAT4X4> modelSpaceTransforms, std::span<DirectX::XMFLOAT4X4> jointMatrices) noexcept
	{
		const bool bInvalidSkinningPoseJointsSize = !skeleton.IsValid() || modelSpaceTransforms.size() != skeleton.Resource->joints.size();
		const bool bInvalidSkinningPose = bInvalidSkinningPoseJointsSize || (jointMatrices.size() != skeleton.Resource->joints.size());

		if (bInvalidSkinningPose)
		{
			return false;
		}
		for (std::size_t jointIndex = 0; jointIndex < skeleton.Resource->joints.size(); ++jointIndex)
		{
			const DirectX::XMMATRIX inverseBind = DirectX::XMLoadFloat4x4(&skeleton.Resource->joints[jointIndex].inverseBindMatrix);
			const DirectX::XMMATRIX model = DirectX::XMLoadFloat4x4(&modelSpaceTransforms[jointIndex]);
			const DirectX::XMMATRIX skinningMatrix = inverseBind * model;
			DirectX::XMStoreFloat4x4(&jointMatrices[jointIndex], skinningMatrix);
		}
		return true;
	}
}
