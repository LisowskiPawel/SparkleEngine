#pragma once

#include "World/Systems/Descriptors/GameWorldSystemContract.h"

#include <cstdint>
#include <span>

class SkeletonResourceStore;

namespace ECS
{
	class AnimationClipResourceStore;
	class AnimationOutputStorage;
	class MorphWeightStorage;

	class AnimationSystemExecution final
	{
	public:
		AnimationSystemExecution(
		    PlaybackAdvanceQuery& playback,
		    PoseEvaluationQuery& poses,
		    MorphEvaluationQuery& morphs,
		    const AnimationClipResourceStore& clips,
		    const SkeletonResourceStore& skeletons,
		    AnimationOutputStorage& output,
		    MorphWeightStorage& weights,
		    std::span<EntityId> animationChanges,
		    std::span<EntityId> morphChanges,
		    float deltaSeconds);

		std::uint32_t GetPlaybackCount() const noexcept;
		std::uint32_t GetPoseCount() const noexcept;
		std::uint32_t GetMorphSampleCount() const noexcept;
		std::uint32_t GetSkinningCount() const noexcept;
		bool RunPlayback(std::uint32_t begin, std::uint32_t end);
		bool RunPose(std::uint32_t begin, std::uint32_t end);
		bool RunMorphSamples(std::uint32_t begin, std::uint32_t end);
		bool RunSkinning(std::uint32_t begin, std::uint32_t end);
		bool CommitMorphOutputs(std::uint32_t begin, std::uint32_t end);

	private:
		const AnimationClipResourceStore& m_clips;
		const SkeletonResourceStore& m_skeletons;
		AnimationOutputStorage& m_output;
		MorphWeightStorage& m_weights;
		std::span<EntityId> m_animationChanges;
		std::span<EntityId> m_morphChanges;
		float m_deltaSeconds = 0.0f;
		PlaybackAdvanceQuery& m_playbackQuery;
		PoseEvaluationQuery& m_poseQuery;
		MorphEvaluationQuery& m_morphQuery;
	};
}
