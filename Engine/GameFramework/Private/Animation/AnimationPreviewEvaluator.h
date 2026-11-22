#pragma once

#include "Animation/AnimationEvaluationStorage.h"

namespace AnimationPreviewEvaluator
{
	bool EvaluatePose(const AnimationClipResource& clip, const SkeletonResource& skeleton, float timeSeconds, ECS::AnimationEvaluationStorage::PoseWorkSlot& output);
	bool EvaluateMorph(const AnimationClipResource& clip, std::uint32_t trackIndex, float timeSeconds, std::array<float, 4>& output);
}
