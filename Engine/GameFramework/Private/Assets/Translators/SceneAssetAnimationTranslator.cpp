#include "PCH.h"

#include "Assets/Translators/SceneAssetAnimationTranslator.h"
#include "GameFramework/Public/Assets/Cooked/CookedSkeletonAsset.h"

#include "Assets/Translators/OzzConstructionBudget.h"
#include "Assets/Loaders/CookedAssetAllocationBudget.h"
#include "Core/Public/Diagnostics/Error.h"

#include <ozz/animation/offline/raw_animation.h>
#include <ozz/animation/offline/raw_track.h>
#include <ozz/animation/offline/animation_builder.h>
#include <ozz/animation/offline/track_builder.h>
#include <ozz/animation/runtime/skeleton_utils.h>

#include <array>
#include <algorithm>
#include <span>
#include <type_traits>
#include <unordered_set>
#include <utility>

namespace Assets
{
	template <typename TKey> static void CopyJointKeys(std::span<const CookedAnimationKeyframeRecord> keys, ozz::vector<TKey>& output)
	{
		output.resize(keys.size());
		for (std::size_t index = 0; index < keys.size(); ++index)
		{
			const auto& key = keys[index];
			if constexpr (std::is_same_v<TKey, ozz::animation::offline::RawAnimation::RotationKey>)
			{
				output[index] = {key.timeSeconds, {key.value.x, key.value.y, key.value.z, key.value.w}};
			}
			else
			{
				output[index] = {key.timeSeconds, {key.value.x, key.value.y, key.value.z}};
			}
		}
	}

	static AnimationMorphTrack BuildMorphTrack(const LoadedAnimationAsset& asset, const CookedAnimationChannelRecord& channel)
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

	static void ValidateTargets(const LoadedAnimationAsset& asset, const SkeletonResource& skeleton, CookedAssetAllocationBudget& budget)
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

	AnimationClipResource BuildSceneAssetAnimation(const LoadedAnimationAsset& asset, CookedAssetId animationAssetId, const SkeletonResource& skeleton, CookedAssetAllocationBudget& budget)
	{
		ValidateTargets(asset, skeleton, budget);
		ReserveOzzConstruction(budget, skeleton.evaluation.num_joints(), asset.keyframes.size());
		const auto morphCount = std::count_if(asset.channels.begin(), asset.channels.end(), [](const auto& channel) { return channel.targetPath == CookedAnimationTargetPath::Weights; });
		budget.ReserveArray<AnimationMorphTrack>(morphCount);
		ozz::animation::offline::RawAnimation raw;
		raw.name = asset.header.name;
		raw.duration = asset.header.durationSeconds > 0.0f ? asset.header.durationSeconds : 1.0f;
		raw.tracks.resize(skeleton.evaluation.num_joints());

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
					CopyJointKeys(keys, track.translations);
					break;
				case CookedAnimationTargetPath::Rotation:
					CopyJointKeys(keys, track.rotations);
					break;
				case CookedAnimationTargetPath::Scale:
					CopyJointKeys(keys, track.scales);
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
}
