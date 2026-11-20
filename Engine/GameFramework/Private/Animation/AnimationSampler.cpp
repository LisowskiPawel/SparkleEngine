#include "PCH.h"

#include "Animation/AnimationSampler.h"

#include <algorithm>
#include <limits>

static inline std::uint32_t FindKeyframeSegment(const AnimationClipResource& clip, const AnimationChannel& channel, float timeSeconds) noexcept
{
	if (channel.keyframeCount <= 1u)
	{
		return 0;
	}

	const std::uint32_t first = channel.firstKeyframe;
	const std::uint32_t lastSegment = channel.keyframeCount - 2u;
	for (std::uint32_t segment = 0; segment <= lastSegment; ++segment)
	{
		if (timeSeconds <= clip.keyframes[first + segment + 1u].timeSeconds)
		{
			return segment;
		}
	}

	return lastSegment;
}

static inline float ComputeSegmentAlpha(const AnimationKeyframe& lhs, const AnimationKeyframe& rhs, float timeSeconds) noexcept
{
	const float duration = rhs.timeSeconds - lhs.timeSeconds;
	return duration <= (std::numeric_limits<float>::epsilon)() ? 0.0f : std::clamp((timeSeconds - lhs.timeSeconds) / duration, 0.0f, 1.0f);
}

static inline std::uint32_t FindStepKeyframe(const AnimationClipResource& clip, const AnimationChannel& channel, float timeSeconds) noexcept
{
	const std::uint32_t first = channel.firstKeyframe;
	std::uint32_t selected = 0u;
	for (std::uint32_t keyframe = 1u; keyframe < channel.keyframeCount; ++keyframe)
	{
		if (timeSeconds < clip.keyframes[first + keyframe].timeSeconds)
		{
			break;
		}
		selected = keyframe;
	}
	return selected;
}

static inline DirectX::XMVECTOR CubicSpline(const AnimationKeyframe& lhs, const AnimationKeyframe& rhs, float alpha) noexcept
{
	const float alphaSquared = alpha * alpha;
	const float alphaCubed = alphaSquared * alpha;
	const float duration = rhs.timeSeconds - lhs.timeSeconds;
	const float h00 = 2.0f * alphaCubed - 3.0f * alphaSquared + 1.0f;
	const float h10 = alphaCubed - 2.0f * alphaSquared + alpha;
	const float h01 = -2.0f * alphaCubed + 3.0f * alphaSquared;
	const float h11 = alphaCubed - alphaSquared;
	const DirectX::XMVECTOR lhsValueTerm = DirectX::XMVectorScale(DirectX::XMLoadFloat4(&lhs.value), h00);
	const DirectX::XMVECTOR rhsValueTerm = DirectX::XMVectorScale(DirectX::XMLoadFloat4(&rhs.value), h01);
	const DirectX::XMVECTOR valueTerms = DirectX::XMVectorAdd(lhsValueTerm, rhsValueTerm);
	const DirectX::XMVECTOR lhsTangentTerm = DirectX::XMVectorScale(DirectX::XMLoadFloat4(&lhs.outTangent), h10 * duration);
	const DirectX::XMVECTOR rhsTangentTerm = DirectX::XMVectorScale(DirectX::XMLoadFloat4(&rhs.inTangent), h11 * duration);
	const DirectX::XMVECTOR tangentTerms = DirectX::XMVectorAdd(lhsTangentTerm, rhsTangentTerm);
	return DirectX::XMVectorAdd(valueTerms, tangentTerms);
}

namespace AnimationSampler
{
	DirectX::XMVECTOR SampleVectorChannel(const AnimationClipResource& clip, const AnimationChannel& channel, float timeSeconds) noexcept
	{
		const std::uint32_t first = channel.firstKeyframe;
		if (channel.keyframeCount == 1u)
		{
			return DirectX::XMLoadFloat4(&clip.keyframes[first].value);
		}
		if (channel.interpolation == Assets::CookedAnimationInterpolation::Step)
		{
			const std::uint32_t keyframe = first + FindStepKeyframe(clip, channel, timeSeconds);
			return DirectX::XMLoadFloat4(&clip.keyframes[keyframe].value);
		}

		const std::uint32_t segment = FindKeyframeSegment(clip, channel, timeSeconds);
		const AnimationKeyframe& lhs = clip.keyframes[first + segment];
		const AnimationKeyframe& rhs = clip.keyframes[first + segment + 1u];
		const float alpha = ComputeSegmentAlpha(lhs, rhs, timeSeconds);
		if (channel.interpolation == Assets::CookedAnimationInterpolation::CubicSpline)
		{
			return CubicSpline(lhs, rhs, alpha);
		}
		const DirectX::XMVECTOR lhsValue = DirectX::XMLoadFloat4(&lhs.value);
		const DirectX::XMVECTOR rhsValue = DirectX::XMLoadFloat4(&rhs.value);
		return DirectX::XMVectorLerp(lhsValue, rhsValue, alpha);
	}

	DirectX::XMVECTOR SampleRotationChannel(const AnimationClipResource& clip, const AnimationChannel& channel, float timeSeconds) noexcept
	{
		const std::uint32_t first = channel.firstKeyframe;
		if (channel.keyframeCount == 1u)
		{
			const DirectX::XMVECTOR value = DirectX::XMLoadFloat4(&clip.keyframes[first].value);
			return DirectX::XMQuaternionNormalize(value);
		}
		if (channel.interpolation == Assets::CookedAnimationInterpolation::Step)
		{
			const std::uint32_t keyframe = first + FindStepKeyframe(clip, channel, timeSeconds);
			const DirectX::XMVECTOR value = DirectX::XMLoadFloat4(&clip.keyframes[keyframe].value);
			return DirectX::XMQuaternionNormalize(value);
		}

		const std::uint32_t segment = FindKeyframeSegment(clip, channel, timeSeconds);
		const AnimationKeyframe& lhs = clip.keyframes[first + segment];
		const AnimationKeyframe& rhs = clip.keyframes[first + segment + 1u];
		const float alpha = ComputeSegmentAlpha(lhs, rhs, timeSeconds);
		if (channel.interpolation == Assets::CookedAnimationInterpolation::CubicSpline)
		{
			const DirectX::XMVECTOR sampled = CubicSpline(lhs, rhs, alpha);
			return DirectX::XMQuaternionNormalize(sampled);
		}
		const DirectX::XMVECTOR lhsValue = DirectX::XMLoadFloat4(&lhs.value);
		const DirectX::XMVECTOR rhsValue = DirectX::XMLoadFloat4(&rhs.value);
		const DirectX::XMVECTOR sampled = DirectX::XMQuaternionSlerp(lhsValue, rhsValue, alpha);
		return DirectX::XMQuaternionNormalize(sampled);
	}
}
