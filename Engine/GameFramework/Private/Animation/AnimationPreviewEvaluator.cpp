#include "PCH.h"

#include "Animation/AnimationPreviewEvaluator.h"

#include <ozz/animation/runtime/local_to_model_job.h>
#include <ozz/animation/runtime/track_sampling_job.h>

#include <algorithm>
#include <cmath>

namespace AnimationPreviewEvaluator
{
	static float PlaybackRatio(const AnimationClipResource& clip, float timeSeconds) noexcept
	{
		return clip.durationSeconds > 0.0f ? std::clamp(timeSeconds / clip.durationSeconds, 0.0f, 1.0f) : 0.0f;
	}

	bool EvaluatePose(const AnimationClipResource& clip, const SkeletonResource& skeleton, float timeSeconds, ECS::AnimationEvaluationStorage::PoseWorkSlot& output)
	{
		if (!output.Context || !std::isfinite(timeSeconds))
		{
			return false;
		}
		ozz::animation::SamplingJob sampling;
		sampling.animation = &clip.animation;
		sampling.context = output.Context.get();
		sampling.ratio = PlaybackRatio(clip, timeSeconds);
		sampling.output = ozz::make_span(output.LocalTransforms);
		if (!sampling.Run())
		{
			return false;
		}
		ozz::animation::LocalToModelJob hierarchy;
		hierarchy.skeleton = &skeleton.evaluation;
		hierarchy.input = ozz::make_span(output.LocalTransforms);
		hierarchy.output = ozz::make_span(output.ModelSpaceTransforms);
		if (!hierarchy.Run())
		{
			return false;
		}
		if (output.JointMatrices.size() != skeleton.joints.size())
		{
			return false;
		}
		for (std::size_t index = 0; index < skeleton.joints.size(); ++index)
		{
			const auto& joint = skeleton.joints[index];
			const auto& model = output.ModelSpaceTransforms[joint.evaluationJointIndex];
			DirectX::XMFLOAT4X4 rowModel;
			for (int row = 0; row < 4; ++row)
			{
				ozz::math::StorePtrU(model.cols[row], rowModel.m[row]);
			}
			const auto inverseBind = DirectX::XMLoadFloat4x4(&joint.inverseBindMatrix);
			const auto modelTransform = DirectX::XMLoadFloat4x4(&rowModel);
			const auto palette = DirectX::XMMatrixMultiply(inverseBind, modelTransform);
			DirectX::XMStoreFloat4x4(&output.JointMatrices[index], palette);
		}
		return true;
	}

	bool EvaluateMorph(const AnimationClipResource& clip, std::uint32_t trackIndex, float timeSeconds, std::array<float, 4>& output)
	{
		if (!std::isfinite(timeSeconds) || trackIndex >= clip.morphTracks.size())
		{
			return false;
		}
		ozz::math::Float4 result;
		ozz::animation::Float4TrackSamplingJob sampling;
		sampling.track = &clip.morphTracks[trackIndex].track;
		sampling.ratio = PlaybackRatio(clip, timeSeconds);
		sampling.result = &result;
		if (!sampling.Run())
		{
			return false;
		}
		output = {result.x, result.y, result.z, result.w};
		return true;
	}
}
