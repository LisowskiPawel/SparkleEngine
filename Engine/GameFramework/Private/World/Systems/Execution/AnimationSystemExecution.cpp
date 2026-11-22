#include "PCH.h"

#include "AnimationSystemExecution.h"

#include "Animation/AnimationEvaluationStorage.h"
#include "Animation/AnimationPreviewEvaluator.h"
#include "World/Resources/AnimationClipResourceStore.h"
#include "World/Resources/SkeletonResourceStore.h"
#include "World/Resources/MorphWeightStorage.h"
#include "World/ECS/EntityRegistry.h"

#include <algorithm>
#include <cmath>

namespace ECS
{
	AnimationSystemExecution::AnimationSystemExecution(
	    EntityRegistry& registry,
	    const AnimationClipResourceStore& clips,
	    const SkeletonResourceStore& skeletons,
	    AnimationEvaluationStorage& output,
	    MorphWeightStorage& weights,
	    float deltaSeconds) :
	    m_registry(registry),
	    m_clips(clips),
	    m_skeletons(skeletons),
	    m_evaluation(output),
	    m_weights(weights),
	    m_deltaSeconds((std::max) (0.0f, deltaSeconds))
	{
	}

	std::uint32_t AnimationSystemExecution::GetPlaybackCount() const noexcept
	{
		return static_cast<std::uint32_t>(m_registry.GetStorage<AnimationState>().GetEntities().size());
	}

	std::uint32_t AnimationSystemExecution::GetPoseCount() const noexcept
	{
		return static_cast<std::uint32_t>(m_evaluation.GetPoseWork().size());
	}

	std::uint32_t AnimationSystemExecution::GetMorphSampleCount() const noexcept
	{
		return static_cast<std::uint32_t>(m_evaluation.GetMorphSamples().size());
	}

	bool AnimationSystemExecution::RunPlayback(std::uint32_t begin, std::uint32_t end)
	{
		const auto entities = m_registry.GetStorage<AnimationState>().GetEntities();
		for (std::uint32_t index = begin; index < end; ++index)
		{
			const EntityId entity = entities[index];
			AnimationState& state = *m_registry.GetMutable<AnimationState>(entity);
			const ResolvedAnimationClip clip = m_clips.Resolve(state.Resource);
			if (!clip.IsValid() || !state.bPlaying || clip.Resource->durationSeconds <= 0.0f)
			{
				continue;
			}
			state.TimeSeconds += m_deltaSeconds * state.PlaybackRate;
			state.TimeSeconds = state.bLooping ? std::fmod(state.TimeSeconds, clip.Resource->durationSeconds) : (std::min) (state.TimeSeconds, clip.Resource->durationSeconds);
		}
		return true;
	}

	bool AnimationSystemExecution::RunPose(std::uint32_t begin, std::uint32_t end)
	{
		const auto work = m_evaluation.GetPoseWork();
		for (std::uint32_t index = begin; index < end; ++index)
		{
			AnimationEvaluationStorage::PoseWorkSlot& slot = work[index];
			const AnimationState* state = m_registry.Get<AnimationState>(slot.Entity);
			if (state == nullptr)
			{
				continue;
			}

			const ResolvedAnimationClip clip = m_clips.Resolve(slot.Clip);
			const SkeletonEvaluationData skeleton = m_skeletons.Resolve(slot.Skeleton);
			const bool bInputsAvailable = clip.IsValid() && skeleton.IsValid();
			const bool bPoseEvaluated = bInputsAvailable && AnimationPreviewEvaluator::EvaluatePose(*clip.Resource, *skeleton.Resource, state->TimeSeconds, slot);
			if (!bPoseEvaluated)
			{
				return false;
			}
		}
		return true;
	}

	bool AnimationSystemExecution::RunMorphSamples(std::uint32_t begin, std::uint32_t end)
	{
		const auto samples = m_evaluation.GetMorphSamples();
		for (std::uint32_t index = begin; index < end; ++index)
		{
			AnimationEvaluationStorage::MorphSampleSlot& sample = samples[index];
			const AnimationState* state = m_registry.Get<AnimationState>(sample.AnimationEntity);
			if (state == nullptr)
			{
				continue;
			}

			const ResolvedAnimationClip clip = m_clips.Resolve(sample.Clip);
			if (!clip.IsValid())
			{
				continue;
			}

			const bool bWeightsEvaluated = AnimationPreviewEvaluator::EvaluateMorph(*clip.Resource, sample.TrackIndex, state->TimeSeconds, sample.Weights);
			if (!bWeightsEvaluated)
			{
				return false;
			}
		}
		return true;
	}

	bool AnimationSystemExecution::CommitMorphOutputs()
	{
		const auto bindings = m_evaluation.GetMorphBindings();
		const auto samples = m_evaluation.GetMorphSamples();
		for (std::size_t index = 0; index < bindings.size(); ++index)
		{
			const AnimationEvaluationStorage::MorphTargetBinding& binding = bindings[index];
			const bool bMorphWeightsWritten = m_weights.Write(binding.TargetWeights, samples[binding.SampleIndex].Weights);

			if (!bMorphWeightsWritten)
			{
				return false;
			}
		}
		return true;
	}
}
