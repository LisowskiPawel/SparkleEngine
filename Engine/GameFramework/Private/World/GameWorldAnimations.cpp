#include "PCH.h"
#include "World/GameWorldState.h"

#include "Animation/AnimationDiagnostics.h"
#include "World/ECS/Components/AnimationComponents.h"
#include "World/ECS/Components/EditorComponents.h"
#include "World/Resources/GameWorldResourceStores.h"

#include <utility>

namespace ECS
{
	void GameWorldState::AppendAnimationClips(std::vector<AnimationClipResource> clips, AnimationClipResourceStore& resources, std::uint64_t sourceInstanceId)
	{
		for (AnimationClipResource& clip : clips)
		{
			const std::uint32_t unsupportedCount = AnimationDiagnostics::CountUnsupportedRuntimeChannels(clip);
			AnimationDiagnostics::LogLoadedClip(clip);
			if (unsupportedCount > 0u)
			{
				AnimationDiagnostics::LogUnsupportedRuntimeChannels(clip, unsupportedCount);
			}
			const std::string name = clip.name;
			const std::uint32_t sourceAnimationIndex = clip.sourceAnimationIndex;
			const Assets::CookedAssetId animationAssetId = clip.animationAssetId;
			const AnimationResourceHandle resource = resources.Add(std::move(clip));
			if (!resource.IsValid())
			{
				continue;
			}
			const EntityId entity = m_registry.Create();
			if (!entity.IsValid())
			{
				continue;
			}

			const bool bAnimationAdded = m_registry.Add(entity, AnimationState{.Resource = resource, .AnimationAssetId = animationAssetId}) && m_registry.Add(entity, Name{name});

			const bool bIdentityAdded = bAnimationAdded
			    && (m_registry.Add(
			        entity,
			        AuthoredIdentity{.SourceAssetId = animationAssetId, .SourceInstanceId = sourceInstanceId, .SourceObjectId = sourceAnimationIndex, .Kind = AuthoredObjectKind::Animation}));

			if (!bIdentityAdded)
			{
				m_registry.Destroy(entity);
				continue;
			}
			RecordChange(entity);
		}
	}

	bool GameWorldState::PrepareSystemResources(GameWorldResourceStores& resources)
	{
		const bool bAnimationPrepared = resources.AnimationClips.ResolveTargets(resources.Skeletons, resources.Generation)
		    && m_animationOutput.Prepare(m_registry, resources.AnimationClips, resources.Skeletons, m_morphWeights, resources.Generation);

		if (!bAnimationPrepared)
		{
			return false;
		}

		m_extraction.Prepare(GetMeshEntities().size());
		return true;
	}

}
