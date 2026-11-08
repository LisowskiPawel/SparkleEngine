#include "PCH.h"

#include "AnimationSystemExecution.h"

#include "Animation/AnimationOutputStorage.h"
#include "Animation/AnimationPoseEvaluator.h"
#include "Animation/MorphWeightEvaluator.h"
#include "Animation/SkinningMatrixEvaluator.h"
#include "World/Resources/AnimationClipResourceStore.h"
#include "World/Resources/SkeletonResourceStore.h"
#include "World/Resources/MorphWeightStorage.h"

#include <algorithm>
#include <cmath>

namespace ECS
{
	AnimationSystemExecution::AnimationSystemExecution(
	    PlaybackAdvanceQuery& playback,
	    PoseEvaluationQuery& poses,
	    MorphEvaluationQuery& morphs,
	    const AnimationClipResourceStore& clips,
	    const SkeletonResourceStore& skeletons,
	    AnimationOutputStorage& output,
	    MorphWeightStorage& weights,
	    std::span<EntityId> animationChanges,
	    std::span<EntityId> morphChanges,
	    float deltaSeconds) :
	    m_clips(clips),
	    m_skeletons(skeletons),
	    m_output(output),
	    m_weights(weights),
	    m_animationChanges(animationChanges),
	    m_morphChanges(morphChanges),
	    m_deltaSeconds((std::max) (0.0f, deltaSeconds)),
	    m_playbackQuery(playback),
	    m_poseQuery(poses),
	    m_morphQuery(morphs)
	{
		m_playbackQuery.PrepareWriteTraversal();
	}

	std::uint32_t AnimationSystemExecution::GetPlaybackCount() const noexcept
	{
		return static_cast<std::uint32_t>(m_playbackQuery.GetEstimatedEntityCount());
	}

	std::uint32_t AnimationSystemExecution::GetPoseCount() const noexcept
	{
		return static_cast<std::uint32_t>(m_poseQuery.GetEstimatedEntityCount());
	}

	std::uint32_t AnimationSystemExecution::GetMorphSampleCount() const noexcept
	{
		return static_cast<std::uint32_t>(m_output.GetMorphSamples().size());
	}

	std::uint32_t AnimationSystemExecution::GetSkinningCount() const noexcept
	{
		return static_cast<std::uint32_t>(m_output.GetPoseWork().size());
	}

	bool AnimationSystemExecution::RunPlayback(std::uint32_t begin, std::uint32_t end)
	{
		return m_playbackQuery
		    .ForEachRange(
		        begin,
		        end,
		        [this](std::size_t index, EntityId entity, AnimationState& state)
		        {
			        const ResolvedAnimationClip clip = m_clips.Resolve(state.Resource);
			        if (!clip.IsValid() || !state.bPlaying || clip.Resource->durationSeconds <= 0.0f)
			        {
				        return;
			        }
			        state.TimeSeconds += m_deltaSeconds * state.PlaybackRate;
			        state.TimeSeconds = state.bLooping ? std::fmod(state.TimeSeconds, clip.Resource->durationSeconds) : (std::min) (state.TimeSeconds, clip.Resource->durationSeconds);
			        m_animationChanges[index] = entity;
		        })
		    .Succeeded();
	}

	bool AnimationSystemExecution::RunPose(std::uint32_t begin, std::uint32_t end)
	{
		return m_poseQuery
		    .ForEachRange(
		        begin,
		        end,
		        [this](std::size_t, EntityId entity, const AnimationState& state)
		        {
			        AnimationOutputStorage::PoseWorkSlot* work = m_output.FindPoseWork(entity);
			        if (work == nullptr || work->PoseOutputIndex >= m_output.GetMutableOutput().poses.size())
			        {
				        return;
			        }

			        const ResolvedAnimationClip clip = m_clips.Resolve(state.Resource);
			        const SkeletonEvaluationData skeleton = m_skeletons.Resolve(work->Skeleton);
			        const bool bClipAvailable = clip.IsValid();
			        const bool bPoseEvaluated = bClipAvailable && AnimationPoseEvaluator::Evaluate(*clip.Resource, skeleton, state.TimeSeconds, work->LocalTransforms, work->ModelSpaceTransforms);

			        if (!bPoseEvaluated)
			        {
				        return;
			        }
			        m_output.GetMutableOutput().poses[work->PoseOutputIndex].playbackTimeSeconds = state.TimeSeconds;
		        })
		    .Succeeded();
	}

	bool AnimationSystemExecution::RunMorphSamples(std::uint32_t begin, std::uint32_t end)
	{
		std::span<AnimationOutputStorage::MorphSampleSlot> samples = m_output.GetMorphSamples();
		AnimationOutput& output = m_output.GetMutableOutput();

		return m_morphQuery
		    .ForEachEntityRange(
		        m_output.GetMorphEntities(),
		        begin,
		        end,
		        [this, samples, &output](std::size_t index, EntityId, const AnimationState& state)
		        {
			        const AnimationOutputStorage::MorphSampleSlot& sample = samples[index];
			        const ResolvedAnimationClip clip = m_clips.Resolve(sample.Clip);
			        if (!clip.IsValid() || sample.OutputIndex >= output.morphWeights.size())
			        {
				        return;
			        }
			        MorphWeightEvaluator::Evaluate(*clip.Resource, sample.ChannelIndex, state.TimeSeconds, output.morphWeights[sample.OutputIndex].weights);
		        })
		    .Succeeded();
	}

	bool AnimationSystemExecution::RunSkinning(std::uint32_t begin, std::uint32_t end)
	{
		std::span<AnimationOutputStorage::PoseWorkSlot> work = m_output.GetPoseWork();
		AnimationOutput& output = m_output.GetMutableOutput();
		for (std::uint32_t index = begin; index < end; ++index)
		{
			AnimationOutputStorage::PoseWorkSlot& slot = work[index];
			if (slot.PoseOutputIndex >= output.poses.size())
			{
				continue;
			}

			const auto& skeleton = m_skeletons.Resolve(slot.Skeleton);
			const bool bMatricesEvaluated = SkinningMatrixEvaluator::Evaluate(skeleton, slot.ModelSpaceTransforms, output.poses[slot.PoseOutputIndex].jointMatrices);

			if (!bMatricesEvaluated)
			{
				return false;
			}
		}
		return true;
	}

	bool AnimationSystemExecution::CommitMorphOutputs(std::uint32_t, std::uint32_t)
	{
		const auto bindings = m_output.GetMorphBindings();
		const auto samples = m_output.GetMorphSamples();
		const AnimationOutput& output = m_output.GetOutput();
		for (std::size_t index = 0; index < bindings.size(); ++index)
		{
			const AnimationOutputStorage::MorphTargetBinding& binding = bindings[index];
			if (binding.SampleIndex >= samples.size())
			{
				return false;
			}

			const std::uint32_t outputIndex = samples[binding.SampleIndex].OutputIndex;
			const bool bMorphOutputAvailable = outputIndex < output.morphWeights.size();
			const bool bMorphWeightsWritten = bMorphOutputAvailable && m_weights.Write(binding.TargetWeights, output.morphWeights[outputIndex].weights);

			if (!bMorphWeightsWritten)
			{
				return false;
			}
			m_morphChanges[index] = binding.TargetEntity;
		}
		return true;
	}
}
