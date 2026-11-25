#include "PCH.h"

#include "World/Resources/AnimationClipResourceStore.h"
#include "World/Resources/SkeletonResourceStore.h"

#include <utility>

namespace ECS
{
	AnimationResourceHandle AnimationClipResourceStore::Add(AnimationClipResource&& clip)
	{
		if (m_entries.size() >= AnimationResourceHandle{}.Slot)
		{
			return {};
		}
		const auto slot = static_cast<std::uint32_t>(m_entries.size());
		m_entries.push_back(BoundAnimationClip{.Resource = std::move(clip)});
		return AnimationResourceHandle{slot};
	}

	const BoundAnimationClip* AnimationClipResourceStore::Resolve(AnimationResourceHandle handle) const noexcept
	{
		if (handle.Slot >= m_entries.size())
		{
			return nullptr;
		}
		const BoundAnimationClip& entry = m_entries[handle.Slot];
		return entry.TargetGeneration != 0 ? &entry : nullptr;
	}

	bool AnimationClipResourceStore::ResolveTargets(const SkeletonResourceStore& skeletons, std::uint32_t targetGeneration) noexcept
	{
		if (targetGeneration == 0)
		{
			return false;
		}
		for (BoundAnimationClip& entry : m_entries)
		{
			entry.Skeleton = skeletons.Find(entry.Resource.targetSkeletonAssetId);
			entry.TargetGeneration = targetGeneration;
		}
		return true;
	}

}
