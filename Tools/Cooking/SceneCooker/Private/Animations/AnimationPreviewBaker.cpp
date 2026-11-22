#include "PCH.h"

#include "Animations/AnimationPreviewBaker.h"

#include "Core/Public/Diagnostics/Error.h"

#include <algorithm>
#include <cmath>

using namespace DirectX;

namespace AnimationPreviewBaker
{
	static DirectX::XMVECTOR EvaluateSegment(
	    const ImportedAnimationSampler& sampler,
	    ImportedAnimationTargetPath path,
	    const ImportedAnimationKeyframe& left,
	    const ImportedAnimationKeyframe& right,
	    float ratio)
	{
		const auto a = DirectX::XMLoadFloat4(&left.value);
		const auto b = DirectX::XMLoadFloat4(&right.value);
		if (sampler.interpolation == ImportedAnimationInterpolation::Step)
		{
			return ratio < 1.0f ? a : b;
		}
		if (sampler.interpolation == ImportedAnimationInterpolation::Linear)
		{
			return path == ImportedAnimationTargetPath::Rotation ? DirectX::XMQuaternionSlerp(a, b, ratio) : DirectX::XMVectorLerp(a, b, ratio);
		}

		const float interval = right.timeSeconds - left.timeSeconds;
		const auto outgoing = DirectX::XMLoadFloat4(&left.outTangent) * interval;
		const auto incoming = DirectX::XMLoadFloat4(&right.inTangent) * interval;
		return DirectX::XMVectorHermite(a, outgoing, b, incoming, ratio);
	}

	static void AppendKey(std::vector<Assets::CookedAnimationKeyframeRecord>& output, float time, DirectX::XMVECTOR value, ImportedAnimationTargetPath path, float duration)
	{
		const float runtimeDuration = duration > 0.0f ? duration : 1.0f;
		const float ratio = path == ImportedAnimationTargetPath::Weights ? time / runtimeDuration : time * (1.0f / runtimeDuration);
		const bool bCollapsedStart = time > 0.0f && ratio <= 0.0f;
		const bool bCollapsedEnd = time < duration && ratio >= 1.0f;
		if (bCollapsedStart || bCollapsedEnd)
		{
			throw Diagnostics::Error("Animation preview time normalization loses a clip boundary; shorten or rebake the clip.");
		}
		if (!output.empty())
		{
			const float previousTime = output.back().timeSeconds;
			const float previousRatio = path == ImportedAnimationTargetPath::Weights ? previousTime / runtimeDuration : previousTime * (1.0f / runtimeDuration);
			if (ratio <= previousRatio)
			{
				throw Diagnostics::Error("Animation preview time normalization merges distinct samples; shorten or rebake the clip.");
			}
		}
		if (path == ImportedAnimationTargetPath::Rotation)
		{
			const float lengthSquared = DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(value));
			if (!std::isfinite(lengthSquared) || lengthSquared < 1.0e-12f)
			{
				throw Diagnostics::Error("Animation preview has a degenerate rotation; bake the source animation before export.");
			}
			value = DirectX::XMQuaternionNormalize(value);
			if (!output.empty())
			{
				const auto previous = DirectX::XMLoadFloat4(&output.back().value);
				if (DirectX::XMVectorGetX(DirectX::XMVector4Dot(previous, value)) < 0.0f)
				{
					value = DirectX::XMVectorNegate(value);
				}
			}
		}

		Assets::CookedAnimationKeyframeRecord key{.timeSeconds = time};
		DirectX::XMStoreFloat4(&key.value, value);
		const float components[] = {key.value.x, key.value.y, key.value.z, key.value.w};
		for (float component : components)
		{
			const bool bOutsideSkeletalRange = path != ImportedAnimationTargetPath::Weights && std::abs(component) > 65504.0f;
			if (!std::isfinite(component) || bOutsideSkeletalRange)
			{
				throw Diagnostics::Error("Animation preview contains non-finite or out-of-range skeletal values; rescale or bake the source asset.");
			}
		}
		if (!output.empty() && time <= output.back().timeSeconds)
		{
			throw Diagnostics::Error("Animation preview sample times exceed float precision; shorten or bake the source clip.");
		}
		output.push_back(key);
	}

	std::vector<Assets::CookedAnimationKeyframeRecord> Bake(const ImportedAnimationSampler& sampler, ImportedAnimationTargetPath path, float durationSeconds)
	{
		if (!sampler.IsValid() || !std::isfinite(durationSeconds) || durationSeconds < 0.0f)
		{
			throw Diagnostics::Error("Animation preview has invalid source samples or duration.");
		}
		const float runtimeDuration = durationSeconds > 0.0f ? durationSeconds : 1.0f;
		if (!std::isfinite(1.0f / runtimeDuration))
		{
			throw Diagnostics::Error("Animation preview duration is too small for normalized sampling; rebake the clip.");
		}
		const bool bLinearOrStep = sampler.interpolation == ImportedAnimationInterpolation::Linear || sampler.interpolation == ImportedAnimationInterpolation::Step;
		if (!bLinearOrStep && sampler.interpolation != ImportedAnimationInterpolation::CubicSpline)
		{
			throw Diagnostics::Error("Animation preview source interpolation is unsupported.");
		}
		float previousTime = -1.0f;
		for (const auto& key : sampler.keyframes)
		{
			const bool bInvalidTime = !std::isfinite(key.timeSeconds) || key.timeSeconds <= previousTime;
			if (bInvalidTime || key.timeSeconds < 0.0f || key.timeSeconds > durationSeconds)
			{
				throw Diagnostics::Error("Animation preview source times must increase within the clip duration.");
			}
			previousTime = key.timeSeconds;
		}

		std::vector<Assets::CookedAnimationKeyframeRecord> output;
		AppendKey(output, sampler.keyframes.front().timeSeconds, DirectX::XMLoadFloat4(&sampler.keyframes.front().value), path, durationSeconds);
		for (std::size_t index = 1; index < sampler.keyframes.size(); ++index)
		{
			const auto& left = sampler.keyframes[index - 1];
			const auto& right = sampler.keyframes[index];
			const bool bBakeCurve = sampler.interpolation != ImportedAnimationInterpolation::Linear || path == ImportedAnimationTargetPath::Rotation;
			double steps = bBakeCurve ? std::ceil(double(right.timeSeconds - left.timeSeconds) * 60.0) : 1.0;
			if (path == ImportedAnimationTargetPath::Rotation && sampler.interpolation == ImportedAnimationInterpolation::Linear)
			{
				const auto a = DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&left.value));
				const auto b = DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&right.value));
				const float dot = std::clamp(std::abs(DirectX::XMVectorGetX(DirectX::XMVector4Dot(a, b))), 0.0f, 1.0f);
				steps = (std::max) (steps, std::ceil(double(2.0f * std::acos(dot)) / (DirectX::XM_PI / 18.0)));
			}
			steps = (std::max) (steps, 1.0);
			if (steps > 1000000.0 - double(output.size()))
			{
				throw Diagnostics::Error("Animation preview exceeds one million samples per channel; shorten or bake the source clip.");
			}
			const auto sampleCount = static_cast<std::uint32_t>(steps);
			for (std::uint32_t sample = 1; sample <= sampleCount; ++sample)
			{
				const float ratio = float(sample) / float(sampleCount);
				const float time = sample == sampleCount ? right.timeSeconds : left.timeSeconds + ratio * (right.timeSeconds - left.timeSeconds);
				const auto value = EvaluateSegment(sampler, path, left, right, ratio);
				AppendKey(output, time, value, path, durationSeconds);
			}
		}
		return output;
	}
}
