#include "PCH.h"

#include "Animation/AnimationEvaluationStorage.h"

#include "World/ECS/Components/EditorComponents.h"
#include "World/ECS/Components/RenderingComponents.h"
#include "World/ECS/EntityRegistry.h"
#include "World/Resources/MorphWeightStorage.h"

#include <algorithm>
#include <tuple>

namespace ECS
{
	bool AnimationEvaluationStorage::Prepare(
	    EntityRegistry& registry,
	    const AnimationClipResourceStore& clips,
	    const SkeletonResourceStore& skeletons,
	    MorphWeightStorage& morphWeights,
	    std::uint32_t targetGeneration)
	{
		if (targetGeneration != 0 && m_targetGeneration == targetGeneration && m_structureVersion == registry.GetStructureVersion())
		{
			return true;
		}
		if (targetGeneration == 0 || registry.IsStructureFrozen())
		{
			return false;
		}

		m_poseWork.clear();
		m_morphSamples.clear();
		m_morphBindings.clear();
		BuildAnimationWork(registry, clips, skeletons, targetGeneration);
		BindMorphTargets(registry, morphWeights);
		BindSkinningOutputs(registry, skeletons, targetGeneration);

		m_targetGeneration = targetGeneration;
		m_structureVersion = registry.GetStructureVersion();
		return true;
	}

	void AnimationEvaluationStorage::BuildAnimationWork(const EntityRegistry& registry, const AnimationClipResourceStore& clips, const SkeletonResourceStore& skeletons, std::uint32_t targetGeneration)
	{
		const ComponentStorage<AnimationState>& animations = registry.GetStorage<AnimationState>();
		std::vector<std::pair<EntityId, AnimationResourceHandle>> ordered;
		ordered.reserve(animations.GetEntities().size());
		for (std::size_t index = 0; index < animations.GetEntities().size(); ++index)
		{
			ordered.emplace_back(animations.GetEntities()[index], animations.GetComponents()[index].Resource);
		}
		std::sort(ordered.begin(), ordered.end(), [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });

		for (const auto& [entity, clipHandle] : ordered)
		{
			const ResolvedAnimationClip clip = clips.Resolve(clipHandle);
			if (!clip.IsValid() || clip.TargetGeneration != targetGeneration)
			{
				continue;
			}

			const AuthoredIdentity* authored = registry.Get<AuthoredIdentity>(entity);
			const std::uint64_t sourceInstanceId = authored == nullptr ? 0 : authored->SourceInstanceId;
			AppendPoseWork(entity, clipHandle, clip, skeletons, sourceInstanceId);
			AppendMorphSamples(entity, clipHandle, clip, sourceInstanceId);
		}
	}

	void AnimationEvaluationStorage::AppendPoseWork(
	    EntityId entity,
	    AnimationResourceHandle clipHandle,
	    const ResolvedAnimationClip& clip,
	    const SkeletonResourceStore& skeletons,
	    std::uint64_t sourceInstanceId)
	{
		const SkeletonEvaluationData skeleton = skeletons.Resolve(clip.Skeleton);
		if (!skeleton.IsValid())
		{
			return;
		}

		PoseWorkSlot work{.Entity = entity, .Clip = clipHandle, .Skeleton = clip.Skeleton, .SourceInstanceId = sourceInstanceId};
		const std::size_t jointCount = skeleton.Resource->joints.size();
		work.LocalTransforms.resize(jointCount);
		work.ModelSpaceTransforms.resize(jointCount);
		work.JointMatrices.resize(jointCount);
		m_poseWork.push_back(std::move(work));
	}

	void AnimationEvaluationStorage::AppendMorphSamples(EntityId entity, AnimationResourceHandle clipHandle, const ResolvedAnimationClip& clip, std::uint64_t sourceInstanceId)
	{
		for (std::uint32_t channelIndex : clip.MorphChannelIndices)
		{
			const AnimationChannel& channel = clip.Resource->channels[channelIndex];
			MorphSampleSlot sample{.AnimationEntity = entity, .Clip = clipHandle, .SourceInstanceId = sourceInstanceId, .ChannelIndex = channelIndex, .TargetNodeIndex = channel.targetNodeIndex};
			sample.Weights.resize(4);
			m_morphSamples.push_back(std::move(sample));
		}
	}

	void AnimationEvaluationStorage::BindMorphTargets(const EntityRegistry& registry, MorphWeightStorage& morphWeights)
	{
		const ComponentStorage<MeshInstance>& meshes = registry.GetStorage<MeshInstance>();
		for (std::uint32_t sampleIndex = 0; sampleIndex < m_morphSamples.size(); ++sampleIndex)
		{
			const MorphSampleSlot& sample = m_morphSamples[sampleIndex];
			for (std::size_t meshIndex = 0; meshIndex < meshes.GetEntities().size(); ++meshIndex)
			{
				const MeshInstance& mesh = meshes.GetComponents()[meshIndex];
				if (mesh.Kind != SceneMeshKind::Skeletal || mesh.SourceNodeIndex != sample.TargetNodeIndex)
				{
					continue;
				}

				const EntityId targetEntity = meshes.GetEntities()[meshIndex];
				const AuthoredIdentity* targetIdentity = registry.Get<AuthoredIdentity>(targetEntity);
				if (targetIdentity == nullptr || targetIdentity->SourceInstanceId != sample.SourceInstanceId)
				{
					continue;
				}

				const MorphState* morph = registry.Get<MorphState>(targetEntity);
				if (morph == nullptr || !morphWeights.PrepareWriteSize(morph->Weights, 4))
				{
					continue;
				}

				m_morphBindings.push_back(MorphTargetBinding{sampleIndex, targetEntity, morph->Weights});
			}
		}

		std::sort(
		    m_morphBindings.begin(),
		    m_morphBindings.end(),
		    [](const MorphTargetBinding& lhs, const MorphTargetBinding& rhs) { return std::tie(lhs.SampleIndex, lhs.TargetEntity) < std::tie(rhs.SampleIndex, rhs.TargetEntity); });
	}

	void AnimationEvaluationStorage::BindSkinningOutputs(EntityRegistry& registry, const SkeletonResourceStore& skeletons, std::uint32_t targetGeneration)
	{
		const ComponentStorage<SkinningState>& skinnedMeshes = registry.GetStorage<SkinningState>();
		for (std::size_t index = 0; index < skinnedMeshes.GetEntities().size(); ++index)
		{
			SkinningState updated = skinnedMeshes.GetComponents()[index];
			updated.Pose = {};
			const SkeletonResourceHandle skeleton = skeletons.Find(updated.SkeletonAssetId);
			const AuthoredIdentity* targetIdentity = registry.Get<AuthoredIdentity>(skinnedMeshes.GetEntities()[index]);
			for (std::uint32_t poseIndex = 0; poseIndex < m_poseWork.size(); ++poseIndex)
			{
				const PoseWorkSlot& work = m_poseWork[poseIndex];
				const bool bMatchingInstance = targetIdentity != nullptr && work.SourceInstanceId == targetIdentity->SourceInstanceId;
				const bool bMatchingTarget = bMatchingInstance && work.Skeleton == skeleton;

				if (bMatchingTarget)
				{
					updated.Pose = AnimationOutputSlotHandle{poseIndex, targetGeneration};
					break;
				}
			}
			registry.Replace(skinnedMeshes.GetEntities()[index], updated);
		}
	}
}
