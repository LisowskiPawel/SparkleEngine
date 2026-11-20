#include "PCH.h"
#include "World/Extraction/Dynamic/RenderSceneDynamicDataExtractor.h"

#include "World/ECS/Components/AnimationComponents.h"
#include "World/ECS/ComponentStorage.h"
#include "World/Extraction/Identity/RenderObjectIdentityMap.h"
#include "World/Extraction/Structural/RenderObjectDeltaExtractor.h"
#include "World/Extraction/WorldExtractionStorage.h"
#include "Animation/AnimationEvaluationStorage.h"
#include "World/Resources/MorphWeightStorage.h"
#include "World/Resources/SkeletonResourceStore.h"
#include "World/WorldReadView.h"

#include <algorithm>
#include <cstring>
#include <limits>
#include <utility>

namespace ECS
{
	void RenderSceneDynamicDataExtractor::BeginScene() noexcept
	{
		m_publishedObjects.clear();
		m_currentObjects.clear();
		m_morphMetadata.clear();
	}

	void RenderSceneDynamicDataExtractor::ExtractObjects(std::span<const WorldExtractionStorage::MeshSlot> meshes, const RenderObjectDeltaExtractor& objects, RenderSceneDynamicData& dynamic)
	{
		m_currentObjects.clear();
		m_currentObjects.reserve(meshes.size());
		for (const WorldExtractionStorage::MeshSlot& mesh : meshes)
		{
			m_currentObjects.push_back({objects.FindObject(mesh.Entity), mesh.WorldMatrix, mesh.WorldInverseTranspose, mesh.bVisible});
		}

		std::sort(m_currentObjects.begin(), m_currentObjects.end(), [](const RenderObjectDynamicData& left, const RenderObjectDynamicData& right) { return left.Object < right.Object; });

		dynamic.Objects.reserve(m_currentObjects.size());
		std::size_t publishedIndex = 0u;
		for (const RenderObjectDynamicData& object : m_currentObjects)
		{
			while (publishedIndex < m_publishedObjects.size() && m_publishedObjects[publishedIndex].Object < object.Object)
			{
				++publishedIndex;
			}

			const bool bUnchangedObject = publishedIndex < m_publishedObjects.size() && m_publishedObjects[publishedIndex].Object == object.Object;
			const bool bUnchanged = bUnchangedObject && (HasSameObjectData(m_publishedObjects[publishedIndex], object));

			if (!bUnchanged)
			{
				dynamic.Objects.push_back(object);
			}
		}

		m_publishedObjects.swap(m_currentObjects);
	}

	bool RenderSceneDynamicDataExtractor::HasSameObjectData(const RenderObjectDynamicData& left, const RenderObjectDynamicData& right) noexcept
	{
		if (left.bVisible != right.bVisible)
		{
			return false;
		}
		const bool bSameWorldMatrix = std::memcmp(&left.WorldMatrix, &right.WorldMatrix, sizeof(left.WorldMatrix)) == 0;
		return bSameWorldMatrix && std::memcmp(&left.WorldInverseTranspose, &right.WorldInverseTranspose, sizeof(left.WorldInverseTranspose)) == 0;
	}

	void RenderSceneDynamicDataExtractor::ExtractSkinning(
	    const ComponentStorage<SkinningState>& skinningStates,
	    const ComponentStorage<AnimationState>& animationStates,
	    const AnimationEvaluationStorage& animation,
	    const SkeletonResourceStore& skeletons,
	    std::span<const WorldExtractionStorage::MeshSlot> meshes,
	    const RenderObjectDeltaExtractor& objects,
	    RenderSceneDynamicData& dynamic)
	{
		const auto poses = animation.GetPoseWork();
		for (const WorldExtractionStorage::MeshSlot& mesh : meshes)
		{
			const SkinningState* skinning = skinningStates.Get(mesh.Entity);
			if (skinning == nullptr || !skinning->Pose.IsValid())
			{
				continue;
			}
			if (skinning->Pose.Generation != animation.GetTargetGeneration() || skinning->Pose.Slot >= poses.size())
			{
				continue;
			}

			const AnimationEvaluationStorage::PoseWorkSlot& pose = poses[skinning->Pose.Slot];
			const SkeletonResourceHandle skeleton = skeletons.Find(skinning->SkeletonAssetId);
			const AnimationState* playback = animationStates.Get(pose.Entity);
			const std::uint32_t jointMatrixOffset = static_cast<std::uint32_t>(dynamic.JointMatrices.size());
			dynamic.JointMatrices.insert(dynamic.JointMatrices.end(), pose.JointMatrices.begin(), pose.JointMatrices.end());

			dynamic.JointMatrixRanges.push_back(
			    RenderJointMatrixRange{
			        .Object = objects.FindObject(mesh.Entity),
			        .Skeleton = skeleton.IsValid() ? RenderSkeletonAssetHandle(skinning->SkeletonAssetId) : RenderSkeletonAssetHandle{},
			        .Animation = playback != nullptr ? RenderAnimationAssetHandle(playback->AnimationAssetId) : RenderAnimationAssetHandle{},
			        .JointMatrixOffset = jointMatrixOffset,
			        .JointMatrixCount = static_cast<std::uint32_t>(pose.JointMatrices.size())});
		}
		std::sort(dynamic.JointMatrixRanges.begin(), dynamic.JointMatrixRanges.end(), [](const RenderJointMatrixRange& lhs, const RenderJointMatrixRange& rhs) { return lhs.Object < rhs.Object; });
	}

	void RenderSceneDynamicDataExtractor::ExtractMorphWeights(
	    const ComponentStorage<MorphState>& morphStates,
	    const ComponentStorage<AnimationState>& animationStates,
	    const AnimationEvaluationStorage& animation,
	    const MorphWeightStorage& weights,
	    const RenderObjectDeltaExtractor& objects,
	    RenderSceneDynamicData& dynamic)
	{
		BuildMorphMetadata(animation, animationStates);
		ExtractMorphWeightRanges(morphStates, weights, objects, dynamic);
	}

	void RenderSceneDynamicDataExtractor::BuildMorphMetadata(const AnimationEvaluationStorage& animation, const ComponentStorage<AnimationState>& animationStates)
	{
		const auto samples = animation.GetMorphSamples();

		m_morphMetadata.clear();
		m_morphMetadata.reserve(animation.GetMorphBindings().size());
		for (const AnimationEvaluationStorage::MorphTargetBinding& binding : animation.GetMorphBindings())
		{
			if (binding.SampleIndex >= samples.size())
			{
				continue;
			}

			const AnimationEvaluationStorage::MorphSampleSlot& sample = samples[binding.SampleIndex];
			const AnimationState* playback = animationStates.Get(sample.AnimationEntity);

			m_morphMetadata.push_back(
			    MorphMetadata{
			        .Entity = binding.TargetEntity,
			        .Animation = playback != nullptr ? RenderAnimationAssetHandle(playback->AnimationAssetId) : RenderAnimationAssetHandle{},
			        .TargetNodeIndex = sample.TargetNodeIndex});
		}
		std::stable_sort(m_morphMetadata.begin(), m_morphMetadata.end(), [](const MorphMetadata& left, const MorphMetadata& right) { return left.Entity < right.Entity; });
	}

	void RenderSceneDynamicDataExtractor::ExtractMorphWeightRanges(
	    const ComponentStorage<MorphState>& morphStates,
	    const MorphWeightStorage& weights,
	    const RenderObjectDeltaExtractor& objects,
	    RenderSceneDynamicData& dynamic)
	{
		const std::span<const EntityId> entities = morphStates.GetEntities();
		const std::span<const MorphState> components = morphStates.GetComponents();
		dynamic.MorphWeightRanges.reserve(entities.size());
		for (std::size_t index = 0; index < entities.size(); ++index)
		{
			const RenderObjectId object = objects.FindObject(entities[index]);
			const std::span<const float> values = weights.Read(components[index].Weights);
			if (!object.IsValid() || values.empty())
			{
				continue;
			}

			const auto metadataEnd = std::upper_bound(
			    m_morphMetadata.begin(),
			    m_morphMetadata.end(),
			    entities[index],
			    [](EntityId entity, const MorphMetadata& metadata) { return entity < metadata.Entity; });

			const MorphMetadata* candidate = metadataEnd != m_morphMetadata.begin() ? &*(metadataEnd - 1) : nullptr;
			const MorphMetadata* metadata = candidate != nullptr && candidate->Entity == entities[index] ? candidate : nullptr;
			const std::uint32_t weightOffset = static_cast<std::uint32_t>(dynamic.MorphWeights.size());
			dynamic.MorphWeights.insert(dynamic.MorphWeights.end(), values.begin(), values.end());

			dynamic.MorphWeightRanges.push_back(
			    RenderMorphWeightRange{
			        .Object = object,
			        .Animation = metadata != nullptr ? metadata->Animation : RenderAnimationAssetHandle{},
			        .TargetNodeIndex = metadata != nullptr ? metadata->TargetNodeIndex : (std::numeric_limits<std::uint32_t>::max)(),
			        .WeightOffset = weightOffset,
			        .WeightCount = static_cast<std::uint32_t>(values.size())});
		}

		std::sort(
		    dynamic.MorphWeightRanges.begin(),
		    dynamic.MorphWeightRanges.end(),
		    [](const RenderMorphWeightRange& left, const RenderMorphWeightRange& right) { return left.Object < right.Object; });
	}

	void RenderSceneDynamicDataExtractor::ExtractLights(const WorldReadView& readView, RenderObjectIdentityMap& identities, RenderSceneDynamicData& dynamic)
	{
		dynamic.Lights.reserve(readView.GetLights().size());
		for (const WorldLightReadData& light : readView.GetLights())
		{
			dynamic.Lights.push_back({identities.Resolve(light.Entity), light.Description});
		}
		std::sort(dynamic.Lights.begin(), dynamic.Lights.end(), [](const RenderLightData& left, const RenderLightData& right) { return left.Object < right.Object; });
	}
}
