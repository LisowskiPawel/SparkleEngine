#include "PCH.h"

#include "Assets/Loaders/AnimationAssetLoader.h"

#include "Assets/Cooked/LoadedAnimationAsset.h"
#include "Assets/Loaders/CookedAssetByteReader.h"
#include "Assets/Loaders/CookedAssetLoaderDiagnostics.h"
#include "Core/Public/Strings/StringUtils.h"

#include <span>
#include <vector>
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
	static void ValidateAnimationHeader(const CookedAnimationAssetHeader& header, const CookedAssetLoaderDiagnostics& diagnostics)
	{
		const bool bCurrentChannelLayout = header.fileHeader.HasMagic(kCookedAnimationAssetMagic) && header.channelStride == sizeof(CookedAnimationChannelRecord);
		const bool bCurrentLayout = bCurrentChannelLayout && header.keyframeStride == sizeof(CookedAnimationKeyframeRecord);
		if (!bCurrentLayout || !Strings::IsNullTerminated(std::span(header.name)))
		{
			throw diagnostics.MakeError("header", "animation magic and current channel/keyframe strides", "Invalid cooked animation asset header; recook the asset");
		}
		if (!std::isfinite(header.durationSeconds) || header.durationSeconds < 0.0f)
		{
			throw diagnostics.MakeError("payload", "a finite non-negative clip duration", "Cooked animation asset has an invalid duration");
		}
	}

	static void ValidateChannelRange(const CookedAnimationChannelRecord& channel, std::size_t keyframeCount, const CookedAssetLoaderDiagnostics& diagnostics)
	{
		const bool bKnownSemantics = IsKnownTargetPath(channel.targetPath) && IsKnownInterpolation(channel.interpolation);
		const bool bValidTarget = channel.targetNodeIndex != (std::numeric_limits<std::uint32_t>::max)();
		const bool bValidOffset = channel.firstKeyframe <= keyframeCount && channel.keyframeCount != 0u;
		const bool bValidRange = bValidOffset && channel.keyframeCount <= keyframeCount - channel.firstKeyframe;
		if (!bKnownSemantics || !bValidTarget)
		{
			throw diagnostics.MakeError("channels", "known target and interpolation", "Cooked animation channel semantics are invalid");
		}
		if (!bValidRange)
		{
			throw diagnostics.MakeError("channels", "an in-range non-empty keyframe span", "Cooked animation channel range is invalid");
		}
	}

	static bool IsValidKeyframe(const CookedAnimationKeyframeRecord& key, float previousTime, float duration, bool bCubicSpline) noexcept
	{
		const bool bOrdered = std::isfinite(key.timeSeconds) && key.timeSeconds > previousTime;
		const bool bInRange = key.timeSeconds >= 0.0f && key.timeSeconds <= duration;
		if (!bOrdered || !bInRange)
		{
			return false;
		}
		if (!IsFinite(key.value))
		{
			return false;
		}
		return !bCubicSpline || (IsFinite(key.inTangent) && IsFinite(key.outTangent));
	}

	static bool IsContinuousRotation(const DirectX::XMFLOAT4& value, const DirectX::XMFLOAT4* previous) noexcept
	{
		const DirectX::XMVECTOR rotation = DirectX::XMLoadFloat4(&value);
		const float lengthSquared = DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(rotation));
		if (!std::isfinite(lengthSquared) || std::abs(lengthSquared - 1.0f) > 1.0e-3f)
		{
			return false;
		}
		return previous == nullptr || DirectX::XMVectorGetX(DirectX::XMVector4Dot(DirectX::XMLoadFloat4(previous), rotation)) >= -1.0e-5f;
	}

	static void ValidateChannelKeys(const LoadedAnimationAsset& asset, const CookedAnimationChannelRecord& channel, std::size_t channelIndex, const CookedAssetLoaderDiagnostics& diagnostics)
	{
		float previousTime = -1.0f;
		const DirectX::XMFLOAT4* previousRotation = nullptr;
		const bool bCubicSpline = channel.interpolation == CookedAnimationInterpolation::CubicSpline;
		for (std::uint32_t offset = 0; offset < channel.keyframeCount; ++offset)
		{
			const auto& key = asset.keyframes[channel.firstKeyframe + offset];
			if (!IsValidKeyframe(key, previousTime, asset.header.durationSeconds, bCubicSpline))
			{
				throw diagnostics.MakeError(
				    "payload",
				    "finite strictly ordered keyframes within clip duration",
				    std::format("Cooked animation channel {} keyframe {} is invalid", channelIndex, offset));
			}
			if (channel.targetPath == CookedAnimationTargetPath::Rotation)
			{
				if (!IsContinuousRotation(key.value, previousRotation))
				{
					throw diagnostics.MakeError(
					    "payload",
					    "normalized sign-continuous rotation keys",
					    std::format("Cooked animation channel {} keyframe {} has an invalid rotation", channelIndex, offset));
				}
				previousRotation = &key.value;
			}
			previousTime = key.timeSeconds;
		}
	}

	static void ValidateAnimationChannels(const LoadedAnimationAsset& asset, const CookedAssetLoaderDiagnostics& diagnostics)
	{
		for (std::size_t index = 0; index < asset.channels.size(); ++index)
		{
			ValidateChannelRange(asset.channels[index], asset.keyframes.size(), diagnostics);
			ValidateChannelKeys(asset, asset.channels[index], index, diagnostics);
		}
	}

	LoadedAnimationAsset AnimationAssetLoader::Decode(const std::filesystem::path& path, std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget)
	{
		const CookedAssetLoaderDiagnostics diagnostics(path, "CookedAnimationAsset");
		CookedAssetByteReader reader(bytes, budget);
		LoadedAnimationAsset asset;
		asset.header = reader.Read<CookedAnimationAssetHeader>();
		ValidateAnimationHeader(asset.header, diagnostics);
		asset.channels = reader.ReadArray<CookedAnimationChannelRecord>(asset.header.channelCount);
		asset.keyframes = reader.ReadArray<CookedAnimationKeyframeRecord>(asset.header.keyframeCount);
		ValidateAnimationChannels(asset, diagnostics);
		if (reader.GetRemainingByteCount() != 0)
		{
			throw diagnostics.MakeError("payload", "no trailing bytes after declared animation records", "Cooked animation asset contains unexpected trailing bytes");
		}
		return asset;
	}
}
