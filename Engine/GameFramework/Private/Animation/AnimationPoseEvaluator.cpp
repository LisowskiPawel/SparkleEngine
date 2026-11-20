#include "PCH.h"

#include "Animation/AnimationPoseEvaluator.h"

#include "Animation/AnimationSampler.h"

#include <algorithm>

static inline DirectX::XMMATRIX ComposeJointTransform(const ECS::AnimationJointTransform& transform) noexcept
{
	const DirectX::XMMATRIX scale = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&transform.Scale));
	const DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&transform.Rotation));
	const DirectX::XMMATRIX translation = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&transform.Translation));
	return scale * rotation * translation;
}

namespace AnimationPoseEvaluator
{
	bool Evaluate(
	    const AnimationClipResource& clip,
	    const ECS::SkeletonEvaluationData& skeleton,
	    float playbackTimeSeconds,
	    std::span<ECS::AnimationJointTransform> localTransforms,
	    std::span<DirectX::XMFLOAT4X4> modelSpaceTransforms) noexcept
	{
		const bool bInvalidAnimationPoseJointsSize = !skeleton.IsValid() || localTransforms.size() != skeleton.Resource->joints.size();
		const bool bInvalidAnimationPose = bInvalidAnimationPoseJointsSize || (modelSpaceTransforms.size() != skeleton.Resource->joints.size());

		if (bInvalidAnimationPose)
		{
			return false;
		}
		std::copy(skeleton.BindLocalTransforms.begin(), skeleton.BindLocalTransforms.end(), localTransforms.begin());
		for (const AnimationChannel& channel : clip.channels)
		{
			if (channel.targetJointIndex >= localTransforms.size())
			{
				continue;
			}
			ECS::AnimationJointTransform& transform = localTransforms[channel.targetJointIndex];
			switch (channel.targetPath)
			{
				case Assets::CookedAnimationTargetPath::Translation:
				{
					const DirectX::XMVECTOR translation = AnimationSampler::SampleVectorChannel(clip, channel, playbackTimeSeconds);
					DirectX::XMStoreFloat3(&transform.Translation, translation);
					break;
				}
				case Assets::CookedAnimationTargetPath::Rotation:
				{
					const DirectX::XMVECTOR rotation = AnimationSampler::SampleRotationChannel(clip, channel, playbackTimeSeconds);
					DirectX::XMStoreFloat4(&transform.Rotation, rotation);
					break;
				}
				case Assets::CookedAnimationTargetPath::Scale:
				{
					const DirectX::XMVECTOR scale = AnimationSampler::SampleVectorChannel(clip, channel, playbackTimeSeconds);
					DirectX::XMStoreFloat3(&transform.Scale, scale);
					break;
				}
				case Assets::CookedAnimationTargetPath::Weights:
				case Assets::CookedAnimationTargetPath::Unknown:
				default:
					break;
			}
		}
		for (const std::uint32_t jointIndex : skeleton.EvaluationOrder)
		{
			const DirectX::XMMATRIX local = ComposeJointTransform(localTransforms[jointIndex]);
			const DirectX::XMMATRIX parentSpace = DirectX::XMLoadFloat4x4(&skeleton.Resource->joints[jointIndex].parentSpaceTransform);
			DirectX::XMMATRIX model = local * parentSpace;
			const std::uint32_t parent = skeleton.Resource->joints[jointIndex].parentJointIndex;
			if (parent < modelSpaceTransforms.size())
			{
				model *= DirectX::XMLoadFloat4x4(&modelSpaceTransforms[parent]);
			}
			DirectX::XMStoreFloat4x4(&modelSpaceTransforms[jointIndex], model);
		}
		return true;
	}
}
