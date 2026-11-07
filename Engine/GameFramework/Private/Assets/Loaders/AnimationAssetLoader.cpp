#include "PCH.h"

#include "Assets/Loaders/AnimationAssetLoader.h"

#include "Assets/Cooked/LoadedAnimationAsset.h"
#include "Assets/Loaders/CookedAssetByteReader.h"
#include "Assets/Loaders/CookedAssetLoaderDiagnostics.h"
#include "Core/Public/Strings/StringUtils.h"

#include <DirectXMath.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <format>
#include <limits>

static bool IsKnownTargetPath(Assets::CookedAnimationTargetPath targetPath) noexcept
{
	switch (targetPath)
	{
		case Assets::CookedAnimationTargetPath::Translation:
		case Assets::CookedAnimationTargetPath::Rotation:
		case Assets::CookedAnimationTargetPath::Scale:
		case Assets::CookedAnimationTargetPath::Weights:
			return true;
		case Assets::CookedAnimationTargetPath::Unknown:
		default:
			return false;
	}
}

static bool IsKnownInterpolation(Assets::CookedAnimationInterpolation interpolation) noexcept
{
	switch (interpolation)
	{
		case Assets::CookedAnimationInterpolation::Linear:
		case Assets::CookedAnimationInterpolation::Step:
		case Assets::CookedAnimationInterpolation::CubicSpline:
			return true;
		default:
			return false;
	}
}

static bool IsFinite(const DirectX::XMFLOAT4& value) noexcept
{
	return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z) && std::isfinite(value.w);
}

namespace Assets
{
	LoadedAnimationAsset AnimationAssetLoader::Decode(const std::filesystem::path& path, std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget) const
	{
		const CookedAssetLoaderDiagnostics diagnostics(path, "CookedAnimationAsset");

		CookedAssetByteReader reader(bytes, budget);
		LoadedAnimationAsset animationAsset;
		animationAsset.header = reader.Read<CookedAnimationAssetHeader>();

		const bool magicValid = animationAsset.header.fileHeader.HasMagic(kCookedAnimationAssetMagic);

		const bool bStridesValidheader = magicValid && animationAsset.header.channelStride == sizeof(CookedAnimationChannelRecord);
		const bool stridesValid = bStridesValidheader && (animationAsset.header.keyframeStride == sizeof(CookedAnimationKeyframeRecord));

		const bool nameTerminated = stridesValid && Strings::IsNullTerminated(std::span(animationAsset.header.name));

		if (!nameTerminated)
		{
			throw diagnostics.MakeError("header", "animation magic and current channel/keyframe strides", "Invalid cooked animation asset header; recook the asset");
		}

		animationAsset.channels = reader.ReadArray<CookedAnimationChannelRecord>(animationAsset.header.channelCount);
		animationAsset.keyframes = reader.ReadArray<CookedAnimationKeyframeRecord>(animationAsset.header.keyframeCount);
		if (!std::isfinite(animationAsset.header.durationSeconds) || animationAsset.header.durationSeconds < 0.0f)
		{
			throw diagnostics.MakeError("payload", "a finite non-negative clip duration", "Cooked animation asset has an invalid duration");
		}

		for (std::size_t channelIndex = 0; channelIndex < animationAsset.channels.size(); ++channelIndex)
		{
			const CookedAnimationChannelRecord& channel = animationAsset.channels[channelIndex];
			const bool targetKnown = IsKnownTargetPath(channel.targetPath);
			const bool interpolationKnown = targetKnown && IsKnownInterpolation(channel.interpolation);
			const bool targetNodeValid = interpolationKnown && channel.targetNodeIndex != (std::numeric_limits<std::uint32_t>::max)();
			const bool bKeyframeOffsetValidkeyframeCount = targetNodeValid && channel.keyframeCount != 0u;
			const bool keyframeOffsetValid = bKeyframeOffsetValidkeyframeCount && (channel.firstKeyframe <= animationAsset.keyframes.size());
			const bool keyframeCountValid = keyframeOffsetValid && channel.keyframeCount <= animationAsset.keyframes.size() - channel.firstKeyframe;

			if (!keyframeCountValid)
			{
				throw diagnostics.MakeError("payload", "known channel semantics and an in-range non-empty keyframe span", std::format("Cooked animation channel {} is invalid", channelIndex));
			}

			float previousTime = -1.0f;
			DirectX::XMFLOAT4 previousRotation{};
			bool hasPreviousRotation = false;
			for (std::uint32_t keyframeOffset = 0; keyframeOffset < channel.keyframeCount; ++keyframeOffset)
			{
				const CookedAnimationKeyframeRecord& keyframe = animationAsset.keyframes[channel.firstKeyframe + keyframeOffset];
				const bool cubicSpline = channel.interpolation == CookedAnimationInterpolation::CubicSpline;
				const bool timeFinite = std::isfinite(keyframe.timeSeconds);
				const bool bTimeOrderedtimeSeconds = keyframe.timeSeconds < 0.0f || keyframe.timeSeconds <= previousTime;
				const bool bTimeOrderedheader = bTimeOrderedtimeSeconds || (keyframe.timeSeconds > animationAsset.header.durationSeconds);
				const bool bTimeOrderedheader2 = !bTimeOrderedheader;
				const bool timeOrdered = timeFinite && bTimeOrderedheader2;
				const bool valueFinite = timeOrdered && IsFinite(keyframe.value);
				const bool bInTangentValidcubicSpline = valueFinite && (!cubicSpline);
				const bool bInTangentValidGuardinTangent = valueFinite && !bInTangentValidcubicSpline;
				const bool bInTangentValidinTangent = bInTangentValidGuardinTangent && (IsFinite(keyframe.inTangent));
				const bool bInTangentValidinTangent2 = bInTangentValidcubicSpline || bInTangentValidinTangent;
				const bool inTangentValid = valueFinite && bInTangentValidinTangent2;
				const bool bOutTangentValidcubicSpline = inTangentValid && (!cubicSpline);
				const bool bOutTangentValidGuardoutTangent = inTangentValid && !bOutTangentValidcubicSpline;
				const bool bOutTangentValidoutTangent = bOutTangentValidGuardoutTangent && (IsFinite(keyframe.outTangent));
				const bool bOutTangentValidoutTangent2 = bOutTangentValidcubicSpline || bOutTangentValidoutTangent;
				const bool outTangentValid = inTangentValid && bOutTangentValidoutTangent2;

				if (!outTangentValid)
				{
					throw diagnostics.MakeError(
					    "payload",
					    "finite strictly ordered keyframes within the clip duration",
					    std::format("Cooked animation channel {} keyframe {} is invalid", channelIndex, keyframeOffset));
				}

				if (channel.targetPath == CookedAnimationTargetPath::Rotation)
				{
					const DirectX::XMVECTOR rotation = DirectX::XMLoadFloat4(&keyframe.value);
					const float lengthSquared = DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(rotation));
					const bool rotationFinite = std::isfinite(lengthSquared);
					const bool rotationNormalized = rotationFinite && !(std::abs(lengthSquared - 1.0f) > 1.0e-3f);

					const bool bRotationContinuousRotation = rotationNormalized && (hasPreviousRotation);
					const bool bRotationContinuous0e = bRotationContinuousRotation && (DirectX::XMVectorGetX(DirectX::XMVector4Dot(DirectX::XMLoadFloat4(&previousRotation), rotation)) < -1.0e-5f);
					const bool bRotationContinuous0e2 = rotationNormalized && !bRotationContinuous0e;
					const bool rotationContinuous = rotationNormalized && bRotationContinuous0e2;

					if (!rotationContinuous)
					{
						throw diagnostics.MakeError(
						    "payload",
						    "normalized sign-continuous rotation keys",
						    std::format("Cooked animation channel {} keyframe {} has an invalid rotation", channelIndex, keyframeOffset));
					}
					previousRotation = keyframe.value;
					hasPreviousRotation = true;
				}
				previousTime = keyframe.timeSeconds;
			}
		}

		if (reader.GetRemainingByteCount() != 0)
		{
			throw diagnostics.MakeError("payload", "no trailing bytes after declared animation records", "Cooked animation asset contains unexpected trailing bytes");
		}

		return animationAsset;
	}
}
