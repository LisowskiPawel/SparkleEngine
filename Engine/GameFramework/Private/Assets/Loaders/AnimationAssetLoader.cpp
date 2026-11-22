#include "PCH.h"

#include "Assets/Loaders/AnimationAssetLoader.h"

#include "GameFramework/Public/Assets/Cooked/CookedAnimationAsset.h"
#include "Animation/AnimationClipResource.h"
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

#include "GameFramework/Public/Assets/Cooked/CookedSkeletonAsset.h"
#include "Assets/Loaders/OzzConstructionBudget.h"
#include "Assets/Loaders/CookedAssetAllocationBudget.h"
#include <ozz/animation/offline/raw_animation.h>
#include <ozz/animation/offline/raw_track.h>
#include <ozz/animation/offline/animation_builder.h>
#include <ozz/animation/offline/track_builder.h>
#include <ozz/animation/runtime/skeleton_utils.h>
#include <array>
#include <algorithm>
#include <unordered_set>
#include <utility>
#include "Animation/SkeletonResource.h"

namespace Assets
{

	struct AnimationRecords final
	{
		CookedAnimationAssetHeader header;
		std::vector<CookedAnimationChannelRecord> channels;
		std::vector<CookedAnimationKeyframeRecord> keyframes;
	};

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

	static void ValidateChannelKeys(const AnimationRecords& asset, const CookedAnimationChannelRecord& channel, std::size_t channelIndex)
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

	static void ValidateAnimationChannels(const AnimationRecords& asset)
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

	template <typename TKeys, typename TConvert> static void CopyJointKeys(std::span<const CookedAnimationKeyframeRecord> keys, TKeys& output, TConvert convert)
	{
		output.resize(keys.size());
		for (std::size_t index = 0; index < keys.size(); ++index)
		{
			const auto& key = keys[index];
			const auto value = convert(key.value);
			output[index] = {key.timeSeconds, value};
		}
	}

	static AnimationMorphTrack BuildMorphTrack(const AnimationRecords& asset, const CookedAnimationChannelRecord& channel)
	{
		ozz::animation::offline::RawFloat4Track raw;
		raw.keyframes.reserve(channel.keyframeCount);
		for (std::uint32_t offset = 0; offset < channel.keyframeCount; ++offset)
		{
			const auto& key = asset.keyframes[channel.firstKeyframe + offset];
			const float ratio = asset.header.durationSeconds > 0.0f ? key.timeSeconds / asset.header.durationSeconds : 0.0f;
			const ozz::math::Float4 value{key.value.x, key.value.y, key.value.z, key.value.w};
			raw.keyframes.push_back({ozz::animation::offline::RawTrackInterpolation::kLinear, ratio, value});
		}
		ozz::animation::offline::TrackBuilder builder;
		auto track = builder(raw);
		if (!track)
		{
			throw Diagnostics::Error("Could not construct an animation-preview morph track");
		}
		return {channel.targetNodeIndex, std::move(*track)};
	}

	static void ValidateTargets(const AnimationRecords& asset, const SkeletonResource& skeleton, CookedAssetAllocationBudget& budget)
	{
		if (asset.header.targetSkeletonAssetId != skeleton.assetId)
		{
			throw Diagnostics::Error("Animation preview target identity does not match the loaded skeleton");
		}
		std::array<std::array<bool, 3>, kMaxAnimationEvaluationJoints> bound{};
		budget.Reserve(asset.channels.size(), 64);
		std::unordered_set<std::uint32_t> morphNodes;
		for (const auto& channel : asset.channels)
		{
			if (channel.targetPath == CookedAnimationTargetPath::Weights)
			{
				if (!morphNodes.insert(channel.targetNodeIndex).second)
				{
					throw Diagnostics::Error("Animation preview repeats a morph-node binding");
				}
				continue;
			}
			if (channel.targetJointIndex >= skeleton.joints.size())
			{
				throw Diagnostics::Error("Animation preview refers to an invalid skin joint");
			}
			const auto& joint = skeleton.joints[channel.targetJointIndex];
			const auto component = static_cast<std::uint32_t>(channel.targetPath);
			if (channel.targetNodeIndex != joint.sourceNodeIndex || bound[channel.targetJointIndex][component])
			{
				throw Diagnostics::Error("Animation preview has a mismatched or duplicate joint binding");
			}
			bound[channel.targetJointIndex][component] = true;
		}
	}

	static AnimationClipResource BuildAnimation(const AnimationRecords& asset, CookedAssetId animationAssetId, const SkeletonResource& skeleton, CookedAssetAllocationBudget& budget)
	{
		ValidateTargets(asset, skeleton, budget);
		ReserveOzzConstruction(budget, skeleton.evaluation.num_joints(), asset.keyframes.size());
		const auto morphCount = std::count_if(asset.channels.begin(), asset.channels.end(), [](const auto& channel) { return channel.targetPath == CookedAnimationTargetPath::Weights; });
		budget.ReserveArray<AnimationMorphTrack>(morphCount);
		ozz::animation::offline::RawAnimation raw;
		raw.name = asset.header.name;
		raw.duration = asset.header.durationSeconds > 0.0f ? asset.header.durationSeconds : 1.0f;
		raw.tracks.resize(skeleton.evaluation.num_joints());
		const auto vectorValue = [](const DirectX::XMFLOAT4& value)
		{
			return ozz::math::Float3{value.x, value.y, value.z};
		};
		const auto rotationValue = [](const DirectX::XMFLOAT4& value)
		{
			return ozz::math::Quaternion{value.x, value.y, value.z, value.w};
		};

		AnimationClipResource clip;
		clip.animationAssetId = animationAssetId;
		clip.targetSkeletonAssetId = asset.header.targetSkeletonAssetId;
		clip.sourceAnimationIndex = asset.header.sourceAnimationIndex;
		clip.durationSeconds = asset.header.durationSeconds;
		clip.morphTracks.reserve(morphCount);
		for (const auto& channel : asset.channels)
		{
			if (channel.targetPath == CookedAnimationTargetPath::Weights)
			{
				ReserveOzzConstruction(budget, 0, channel.keyframeCount);
				clip.morphTracks.push_back(BuildMorphTrack(asset, channel));
				continue;
			}
			const auto evaluationIndex = skeleton.joints[channel.targetJointIndex].evaluationJointIndex;
			auto& track = raw.tracks[evaluationIndex];
			const auto keys = std::span(asset.keyframes).subspan(channel.firstKeyframe, channel.keyframeCount);
			switch (channel.targetPath)
			{
				case CookedAnimationTargetPath::Translation:
					CopyJointKeys(keys, track.translations, vectorValue);
					break;
				case CookedAnimationTargetPath::Rotation:
					CopyJointKeys(keys, track.rotations, rotationValue);
					break;
				case CookedAnimationTargetPath::Scale:
					CopyJointKeys(keys, track.scales, vectorValue);
					break;
				default:
					throw Diagnostics::Error("Invalid animation preview component");
			}
		}
		for (int joint = 0; joint < skeleton.evaluation.num_joints(); ++joint)
		{
			const auto rest = ozz::animation::GetJointRestPoseLocalSpace(skeleton.evaluation, joint);
			auto& track = raw.tracks[joint];
			if (track.translations.empty())
			{
				track.translations.push_back({0.0f, rest.translation});
			}
			if (track.rotations.empty())
			{
				track.rotations.push_back({0.0f, rest.rotation});
			}
			if (track.scales.empty())
			{
				track.scales.push_back({0.0f, rest.scale});
			}
		}
		ozz::animation::offline::AnimationBuilder builder;
		builder.iframe_interval = 0.0f;
		auto animation = builder(raw);
		if (!animation)
		{
			throw Diagnostics::Error("Could not construct the animation preview");
		}
		clip.animation = std::move(*animation);
		return clip;
	}

	AnimationClipResource AnimationAssetLoader::Load(
	    std::span<const std::uint8_t> bytes,
	    CookedAssetId animationAssetId,
	    std::span<const SkeletonResource> skeletons,
	    CookedAssetAllocationBudget& budget)
	{
		CookedAssetByteReader reader(bytes, budget);
		AnimationRecords asset;
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
		const auto skeleton = std::find_if(skeletons.begin(), skeletons.end(), [&](const auto& entry) { return entry.assetId == asset.header.targetSkeletonAssetId; });
		if (skeleton == skeletons.end())
		{
			throw Diagnostics::Error("Animation preview target skeleton was not loaded in the same scene generation");
		}
		budget.Reserve(sizeof(asset.header.name));
		return BuildAnimation(asset, animationAssetId, *skeleton, budget);
	}
}
