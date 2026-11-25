#pragma once

#include "Animation/AnimationClipResource.h"
#include "World/ECS/Components/AnimationComponents.h"

#include <cstdint>
#include <span>
#include <vector>

class SkeletonResourceStore;
struct SkeletonResource;

namespace ECS
{
	struct BoundAnimationClip final
	{
		AnimationClipResource Resource;
		const SkeletonResource* Skeleton = nullptr;
		std::uint32_t TargetGeneration = 0;
	};

	class AnimationClipResourceStore final
	{
	public:
		AnimationResourceHandle Add(AnimationClipResource&& clip);
		const BoundAnimationClip* Resolve(AnimationResourceHandle handle) const noexcept;
		bool ResolveTargets(const SkeletonResourceStore& skeletons, std::uint32_t targetGeneration) noexcept;

	private:
		std::vector<BoundAnimationClip> m_entries;
	};
}
