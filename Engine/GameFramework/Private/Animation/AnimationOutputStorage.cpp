#include "PCH.h"

#include "Animation/AnimationOutputStorage.h"

#include "World/ECS/Components/EditorComponents.h"
#include "World/ECS/Components/RenderingComponents.h"
#include "World/ECS/EntityRegistry.h"
#include "World/Resources/MorphWeightStorage.h"

#include <algorithm>
#include <tuple>

namespace ECS
{
	bool AnimationOutputStorage::Prepare(
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

		ResetOutputs();
		BuildAnimationWork(registry, clips, skeletons, targetGeneration);
		BuildPoseWorkIndex();
		BindMorphTargets(registry, morphWeights);
		BindSkinningOutputs(registry, targetGeneration);

		m_targetGeneration = targetGeneration;
		m_structureVersion = registry.GetStructureVersion();
		return true;
	}

	void AnimationOutputStorage::ResetOutputs()
	{
		m_poseWork.clear();
		m_morphSamples.clear();
		m_morphEntities.clear();
		m_morphBindings.clear();
		m_workIndexByEntitySlot.clear();
		m_output.Reset();
	}

	void AnimationOutputStorage::BuildAnimationWork(const EntityRegistry& registry, const AnimationClipResourceStore& clips, const SkeletonResourceStore& skeletons, std::uint32_t targetGeneration)
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

	void AnimationOutputStorage::AppendPoseWork(
	    EntityId entity,
	    AnimationResourceHandle clipHandle,
	    const ResolvedAnimationClip& clip,
	    const SkeletonResourceStore& skeletons,
	    std::uint64_t sourceInstanceId)
	{
		PoseWorkSlot work{.Entity = entity, .Clip = clipHandle, .Skeleton = clip.Skeleton, .SourceInstanceId = sourceInstanceId};
		const SkeletonEvaluationData skeleton = skeletons.Resolve(clip.Skeleton);
		if (skeleton.IsValid())
		{
			work.PoseOutputIndex = static_cast<std::uint32_t>(m_output.poses.size());
			work.LocalTransforms.resize(skeleton.Resource->joints.size());
			work.ModelSpaceTransforms.resize(skeleton.Resource->joints.size());
			AnimationPoseOutput pose;
			pose.animationEntity = entity;
			pose.skeletonAssetId = skeleton.Resource->assetId;
			pose.animationAssetId = clip.Resource->animationAssetId;
			pose.clipName = clip.Resource->name;
			pose.jointCount = static_cast<std::uint32_t>(skeleton.Resource->joints.size());
			pose.jointMatrices.resize(skeleton.Resource->joints.size());
			m_output.poses.push_back(std::move(pose));
		}
		m_poseWork.push_back(std::move(work));
	}

	void AnimationOutputStorage::AppendMorphSamples(EntityId entity, AnimationResourceHandle clipHandle, const ResolvedAnimationClip& clip, std::uint64_t sourceInstanceId)
	{
		for (std::uint32_t channelIndex : clip.MorphChannelIndices)
		{
			const AnimationChannel& channel = clip.Resource->channels[channelIndex];
			const std::uint32_t outputIndex = static_cast<std::uint32_t>(m_output.morphWeights.size());
			MorphWeightOutput output;
			output.animationEntity = entity;
			output.targetNodeIndex = channel.targetNodeIndex;
			output.weights.resize(4);
			m_output.morphWeights.push_back(std::move(output));
			m_morphSamples.push_back(MorphSampleSlot{entity, clipHandle, sourceInstanceId, channelIndex, outputIndex});
			m_morphEntities.push_back(entity);
		}
	}

	void AnimationOutputStorage::BuildPoseWorkIndex()
	{
		EntityId::Slot largestSlot = 0;
		for (const PoseWorkSlot& work : m_poseWork)
		{
			largestSlot = (std::max) (largestSlot, work.Entity.GetSlot());
		}
		if (!m_poseWork.empty())
		{
			m_workIndexByEntitySlot.resize(static_cast<std::size_t>(largestSlot) + 1u);
		}
		for (std::uint32_t index = 0; index < m_poseWork.size(); ++index)
		{
			m_workIndexByEntitySlot[m_poseWork[index].Entity.GetSlot()] = EntityWorkIndex{m_poseWork[index].Entity, index};
		}
	}

	void AnimationOutputStorage::BindMorphTargets(const EntityRegistry& registry, MorphWeightStorage& morphWeights)
	{
		const ComponentStorage<MeshInstance>& meshes = registry.GetStorage<MeshInstance>();
		for (std::uint32_t sampleIndex = 0; sampleIndex < m_morphSamples.size(); ++sampleIndex)
		{
			const MorphSampleSlot& sample = m_morphSamples[sampleIndex];
			const std::uint32_t targetNode = m_output.morphWeights[sample.OutputIndex].targetNodeIndex;
			for (std::size_t meshIndex = 0; meshIndex < meshes.GetEntities().size(); ++meshIndex)
			{
				const MeshInstance& mesh = meshes.GetComponents()[meshIndex];
				if (mesh.Kind != SceneMeshKind::Skeletal || mesh.SourceNodeIndex != targetNode)
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

	void AnimationOutputStorage::BindSkinningOutputs(EntityRegistry& registry, std::uint32_t targetGeneration)
	{
		const ComponentStorage<SkinningState>& skinnedMeshes = registry.GetStorage<SkinningState>();
		for (std::size_t index = 0; index < skinnedMeshes.GetEntities().size(); ++index)
		{
			SkinningState updated = skinnedMeshes.GetComponents()[index];
			updated.Pose = {};
			const AuthoredIdentity* targetIdentity = registry.Get<AuthoredIdentity>(skinnedMeshes.GetEntities()[index]);
			for (const PoseWorkSlot& work : m_poseWork)
			{
				const bool bMatchingAnimationTargetInstanceId = targetIdentity != nullptr && work.SourceInstanceId == targetIdentity->SourceInstanceId;
				const bool bMatchingAnimationTargetposes = bMatchingAnimationTargetInstanceId && (work.PoseOutputIndex < m_output.poses.size());
				const bool matchingAnimationTarget = bMatchingAnimationTargetposes && (m_output.poses[work.PoseOutputIndex].skeletonAssetId == updated.SkeletonAssetId);
				
				if (matchingAnimationTarget)
				{
					updated.Pose = AnimationOutputSlotHandle{work.PoseOutputIndex, targetGeneration};
					break;
				}
			}
			registry.Replace(skinnedMeshes.GetEntities()[index], updated);
		}
	}

	AnimationOutputStorage::PoseWorkSlot* AnimationOutputStorage::FindPoseWork(EntityId entity) noexcept
	{
		if (!entity.IsValid() || entity.GetSlot() >= m_workIndexByEntitySlot.size())
		{
			return nullptr;
		}
		const EntityWorkIndex& mapping = m_workIndexByEntitySlot[entity.GetSlot()];
		return mapping.Entity == entity && mapping.Index < m_poseWork.size() ? &m_poseWork[mapping.Index] : nullptr;
	}
}
