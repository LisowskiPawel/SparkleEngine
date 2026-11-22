#include "PCH.h"

#include "Assets/Loaders/AnimationAssetLoader.h"

#include "Assets/Cooked/LoadedAnimationAsset.h"
#include "Assets/Loaders/CookedAssetByteReader.h"
#include "Core/Public/Diagnostics/Error.h"
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

static bool IsFinite(const DirectX::XMFLOAT4& value) noexcept
{
	return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z) && std::isfinite(value.w);
}

namespace Assets
{
	static void ValidateAnimationHeader(const CookedAnimationAssetHeader& header)
	{
		const bool bCurrentChannelLayout = header.fileHeader.HasMagic(kCookedAnimationAssetMagic) && header.channelStride == sizeof(CookedAnimationChannelRecord);
		const bool bCurrentLayout = bCurrentChannelLayout && header.keyframeStride == sizeof(CookedAnimationKeyframeRecord);
		if (!bCurrentLayout || !Strings::IsNullTerminated(std::span(header.name)))
		{
			throw Diagnostics::Error("Invalid cooked animation asset header; recook the asset");
		}
		if (!std::isfinite(header.durationSeconds) || header.durationSeconds < 0.0f)
		{
			throw Diagnostics::Error("Cooked animation asset has an invalid duration");
		}
		const float runtimeDuration = header.durationSeconds > 0.0f ? header.durationSeconds : 1.0f;
		if (!std::isfinite(1.0f / runtimeDuration))
		{
			throw Diagnostics::Error("Cooked animation duration is too small for preview time normalization");
		}
	}

	static void ValidateChannelRange(const CookedAnimationChannelRecord& channel, std::size_t keyframeCount)
	{
		const bool bKnownSemantics = IsKnownTargetPath(channel.targetPath);
		const bool bValidTarget = channel.targetNodeIndex != (std::numeric_limits<std::uint32_t>::max)();
		const bool bValidOffset = channel.firstKeyframe <= keyframeCount && channel.keyframeCount != 0u;
		const bool bValidRange = bValidOffset && channel.keyframeCount <= keyframeCount - channel.firstKeyframe;
		if (!bKnownSemantics || !bValidTarget)
		{
			throw Diagnostics::Error("Cooked animation channel semantics are invalid");
		}
		if (!bValidRange)
		{
			throw Diagnostics::Error("Cooked animation channel range is invalid");
		}
	}

	static bool IsValidKeyframe(const CookedAnimationKeyframeRecord& key, float previousTime, float duration) noexcept
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
		return true;
	}

	static bool IsContinuousRotation(const DirectX::XMFLOAT4& value, const DirectX::XMFLOAT4* previous) noexcept
	{
		const DirectX::XMVECTOR rotation = DirectX::XMLoadFloat4(&value);
		const float lengthSquared = DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(rotation));
		if (!std::isfinite(lengthSquared) || std::abs(lengthSquared - 1.0f) > 1.0e-3f)
		{
			return false;
		}
		if (previous == nullptr)
		{
			return true;
		}
		const DirectX::XMVECTOR previousRotation = DirectX::XMLoadFloat4(previous);
		const DirectX::XMVECTOR alignment = DirectX::XMVector4Dot(previousRotation, rotation);
		return DirectX::XMVectorGetX(alignment) >= -1.0e-5f;
	}

	static void ValidateChannelKeys(const LoadedAnimationAsset& asset, const CookedAnimationChannelRecord& channel, std::size_t channelIndex)
	{
		float previousTime = -1.0f;
		float previousRatio = -1.0f;
		const float duration = asset.header.durationSeconds > 0.0f ? asset.header.durationSeconds : 1.0f;
		const float inverseDuration = 1.0f / duration;
		const DirectX::XMFLOAT4* previousRotation = nullptr;
		for (std::uint32_t offset = 0; offset < channel.keyframeCount; ++offset)
		{
			const auto& key = asset.keyframes[channel.firstKeyframe + offset];
			if (!IsValidKeyframe(key, previousTime, asset.header.durationSeconds))
			{
				throw Diagnostics::Error(std::format("Cooked animation channel {} keyframe {} is invalid", channelIndex, offset));
			}
			// Match the pinned skeletal builder's multiplication and user-track division.
			const float ratio = channel.targetPath == CookedAnimationTargetPath::Weights ? key.timeSeconds / duration : key.timeSeconds * inverseDuration;
			const bool bCollapsedStart = key.timeSeconds > 0.0f && ratio <= 0.0f;
			const bool bCollapsedEnd = key.timeSeconds < duration && ratio >= 1.0f;
			const bool bCollapsedBoundary = bCollapsedStart || bCollapsedEnd;
			if (ratio <= previousRatio || bCollapsedBoundary)
			{
				throw Diagnostics::Error("Cooked animation keys collapse during preview time normalization; shorten or rebake the clip");
			}
			previousRatio = ratio;
			const bool bSkeletal = channel.targetPath != CookedAnimationTargetPath::Weights;
			const bool bOutsideRange = std::abs(key.value.x) > 65504.0f || std::abs(key.value.y) > 65504.0f;
			const bool bOutsideZ = std::abs(key.value.z) > 65504.0f;
			if (bSkeletal && (bOutsideRange || bOutsideZ))
			{
				throw Diagnostics::Error("Cooked skeletal key exceeds the supported numeric range");
			}
			if (channel.targetPath == CookedAnimationTargetPath::Rotation)
			{
				if (!IsContinuousRotation(key.value, previousRotation))
				{
					throw Diagnostics::Error(std::format("Cooked animation channel {} keyframe {} has an invalid rotation", channelIndex, offset));
				}
				previousRotation = &key.value;
			}
			previousTime = key.timeSeconds;
		}
	}

	static void ValidateAnimationChannels(const LoadedAnimationAsset& asset)
	{
		std::size_t nextKey = 0;
		for (std::size_t index = 0; index < asset.channels.size(); ++index)
		{
			ValidateChannelRange(asset.channels[index], asset.keyframes.size());
			if (asset.channels[index].firstKeyframe != nextKey)
			{
				throw Diagnostics::Error("Cooked animation channels must own contiguous, distinct keys");
			}
			nextKey += asset.channels[index].keyframeCount;
			ValidateChannelKeys(asset, asset.channels[index], index);
		}
		if (nextKey != asset.keyframes.size())
		{
			throw Diagnostics::Error("Cooked animation contains unreferenced keys");
		}
	}

	LoadedAnimationAsset AnimationAssetLoader::Decode(std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget)
	{
		CookedAssetByteReader reader(bytes, budget);
		LoadedAnimationAsset asset;
		asset.header = reader.Read<CookedAnimationAssetHeader>();
		ValidateAnimationHeader(asset.header);
		if (asset.header.keyframeCount > 4000000 || asset.header.channelCount > 1000000 || asset.header.flags != 0)
		{
			throw Diagnostics::Error("Cooked animation preview exceeds supported sample/channel capacity");
		}
		asset.channels = reader.ReadArray<CookedAnimationChannelRecord>(asset.header.channelCount);
		asset.keyframes = reader.ReadArray<CookedAnimationKeyframeRecord>(asset.header.keyframeCount);
		ValidateAnimationChannels(asset);
		if (reader.GetRemainingByteCount() != 0)
		{
			throw Diagnostics::Error("Cooked animation asset contains unexpected trailing bytes");
		}
		return asset;
	}
}
