#pragma once

#include <cstdint>

class SkeletonResourceStore;

namespace ECS
{
	class AnimationClipResourceStore;
	class AnimationOutputStorage;
	class MorphWeightStorage;
	class EntityRegistry;

	class AnimationSystemExecution final
	{
	public:
		AnimationSystemExecution(
		    EntityRegistry& registry,
		    const AnimationClipResourceStore& clips,
		    const SkeletonResourceStore& skeletons,
		    AnimationOutputStorage& output,
		    MorphWeightStorage& weights,
		    float deltaSeconds);

		std::uint32_t GetPlaybackCount() const noexcept;
		std::uint32_t GetPoseCount() const noexcept;
		std::uint32_t GetMorphSampleCount() const noexcept;
		bool RunPlayback(std::uint32_t begin, std::uint32_t end);
		bool RunPose(std::uint32_t begin, std::uint32_t end);
		bool RunMorphSamples(std::uint32_t begin, std::uint32_t end);
		bool RunSkinning(std::uint32_t begin, std::uint32_t end);
		bool CommitMorphOutputs();

	private:
		EntityRegistry& m_registry;
		const AnimationClipResourceStore& m_clips;
		const SkeletonResourceStore& m_skeletons;
		AnimationOutputStorage& m_output;
		MorphWeightStorage& m_weights;
		float m_deltaSeconds = 0.0f;
	};
}
