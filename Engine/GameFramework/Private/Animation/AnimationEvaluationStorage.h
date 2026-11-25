#pragma once

#include "GameFramework/Public/World/EntityId.h"
#include "World/ECS/Components/AnimationComponents.h"
#include "World/Resources/AnimationClipResourceStore.h"

#include <cstdint>
#include <array>
#include <limits>
#include <span>
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include <ozz/animation/runtime/sampling_job.h>
#include <ozz/base/maths/soa_transform.h>
#include <ozz/base/maths/simd_math.h>

class SkeletonResourceStore;
struct SkeletonResource;

namespace ECS
{
	class EntityRegistry;
	class MorphWeightStorage;

	class AnimationEvaluationStorage final
	{
	public:
		struct PoseWorkSlot final
		{
			EntityId Entity;
			AnimationResourceHandle Clip;
			const SkeletonResource* Skeleton = nullptr;
			std::uint64_t SourceInstanceId = 0;
			std::unique_ptr<ozz::animation::SamplingJob::Context> Context;
			std::vector<ozz::math::SoaTransform> LocalTransforms;
			std::vector<ozz::math::Float4x4> ModelSpaceTransforms;
			std::vector<DirectX::XMFLOAT4X4> JointMatrices;
		};

		struct MorphSampleSlot final
		{
			EntityId AnimationEntity;
			AnimationResourceHandle Clip;
			std::uint64_t SourceInstanceId = 0;
			std::uint32_t TrackIndex = 0;
			std::uint32_t TargetNodeIndex = (std::numeric_limits<std::uint32_t>::max)();
			std::array<float, 4> Weights{};
		};

		struct MorphTargetBinding final
		{
			std::uint32_t SampleIndex = 0;
			EntityId TargetEntity;
			AnimationOutputSlotHandle TargetWeights;
		};

		bool Prepare(EntityRegistry& registry, const AnimationClipResourceStore& clips, const SkeletonResourceStore& skeletons, MorphWeightStorage& morphWeights, std::uint32_t targetGeneration);

		std::span<PoseWorkSlot> GetPoseWork() noexcept { return m_poseWork; }

		std::span<const PoseWorkSlot> GetPoseWork() const noexcept { return m_poseWork; }

		std::span<MorphSampleSlot> GetMorphSamples() noexcept { return m_morphSamples; }

		std::span<const MorphSampleSlot> GetMorphSamples() const noexcept { return m_morphSamples; }

		std::span<const MorphTargetBinding> GetMorphBindings() const noexcept { return m_morphBindings; }

		std::uint32_t GetTargetGeneration() const noexcept { return m_targetGeneration; }

	private:
		void BuildAnimationWork(const EntityRegistry& registry, const AnimationClipResourceStore& clips, std::uint32_t targetGeneration);
		void AppendPoseWork(EntityId entity, AnimationResourceHandle clipHandle, const BoundAnimationClip& clip, std::uint64_t sourceInstanceId);
		void AppendMorphSamples(EntityId entity, AnimationResourceHandle clipHandle, const BoundAnimationClip& clip, std::uint64_t sourceInstanceId);
		void BindMorphTargets(const EntityRegistry& registry, MorphWeightStorage& morphWeights);
		void BindSkinningOutputs(EntityRegistry& registry, const SkeletonResourceStore& skeletons, std::uint32_t targetGeneration);

		std::vector<PoseWorkSlot> m_poseWork;
		std::vector<MorphSampleSlot> m_morphSamples;
		std::vector<MorphTargetBinding> m_morphBindings;
		std::uint64_t m_structureVersion = 0;
		std::uint32_t m_targetGeneration = 0;
	};
}
