#include "PCH.h"

#include "AnimationSystemExecution.h"

#include "Animation/AnimationOutputStorage.h"
#include "Animation/AnimationPoseEvaluator.h"
#include "Animation/MorphWeightEvaluator.h"
#include "Animation/SkinningMatrixEvaluator.h"
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
	    AnimationOutputStorage& output,
	    MorphWeightStorage& weights,
	    float deltaSeconds) :
	    m_registry(registry),
	    m_clips(clips),
	    m_skeletons(skeletons),
	    m_output(output),
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
		return static_cast<std::uint32_t>(m_output.GetPoseWork().size());
	}

	std::uint32_t AnimationSystemExecution::GetMorphSampleCount() const noexcept
	{
		return static_cast<std::uint32_t>(m_output.GetMorphSamples().size());
	}

	bool AnimationSystemExecution::RunPlayback(std::uint32_t begin, std::uint32_t end)
	{
		const auto entities = m_registry.GetStorage<AnimationState>().GetEntities();
		if (begin > end || end > entities.size())
		{
			return false;
		}
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
		const auto work = m_output.GetPoseWork();
		AnimationOutput& output = m_output.GetMutableOutput();
		if (begin > end || end > work.size())
		{
			return false;
		}
		for (std::uint32_t index = begin; index < end; ++index)
		{
			AnimationOutputStorage::PoseWorkSlot& slot = work[index];
			const AnimationState* state = m_registry.Get<AnimationState>(slot.Entity);
			if (state == nullptr || slot.PoseOutputIndex >= output.poses.size())
			{
				continue;
			}
			const ResolvedAnimationClip clip = m_clips.Resolve(slot.Clip);
			const SkeletonEvaluationData skeleton = m_skeletons.Resolve(slot.Skeleton);
			const bool bClipAvailable = clip.IsValid();
			const bool bPoseEvaluated = bClipAvailable && AnimationPoseEvaluator::Evaluate(*clip.Resource, skeleton, state->TimeSeconds, slot.LocalTransforms, slot.ModelSpaceTransforms);
			if (bPoseEvaluated)
			{
				output.poses[slot.PoseOutputIndex].playbackTimeSeconds = state->TimeSeconds;
			}
		}
		return true;
	}

	bool AnimationSystemExecution::RunMorphSamples(std::uint32_t begin, std::uint32_t end)
	{
		const auto samples = m_output.GetMorphSamples();
		AnimationOutput& output = m_output.GetMutableOutput();
		if (begin > end || end > samples.size())
		{
			return false;
		}
		for (std::uint32_t index = begin; index < end; ++index)
		{
			const AnimationOutputStorage::MorphSampleSlot& sample = samples[index];
			const AnimationState* state = m_registry.Get<AnimationState>(sample.AnimationEntity);
			if (state == nullptr)
			{
				continue;
			}
			const ResolvedAnimationClip clip = m_clips.Resolve(sample.Clip);
			if (!clip.IsValid() || sample.OutputIndex >= output.morphWeights.size())
			{
				continue;
			}
			MorphWeightEvaluator::Evaluate(*clip.Resource, sample.ChannelIndex, state->TimeSeconds, output.morphWeights[sample.OutputIndex].weights);
		}
		return true;
	}

	bool AnimationSystemExecution::RunSkinning(std::uint32_t begin, std::uint32_t end)
	{
		std::span<AnimationOutputStorage::PoseWorkSlot> work = m_output.GetPoseWork();
		AnimationOutput& output = m_output.GetMutableOutput();
		if (begin > end || end > work.size())
		{
			return false;
		}
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

	bool AnimationSystemExecution::CommitMorphOutputs()
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
		}
		return true;
	}
}
