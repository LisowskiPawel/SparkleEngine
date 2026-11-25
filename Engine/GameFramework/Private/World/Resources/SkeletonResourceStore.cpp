#include "PCH.h"

#include "World/Resources/SkeletonResourceStore.h"

#include <utility>

void SkeletonResourceStore::Append(std::vector<SkeletonResource> skeletons)
{
	for (SkeletonResource& skeleton : skeletons)
	{
		if (skeleton.assetId != Assets::InvalidCookedAssetId)
		{
			m_resources.try_emplace(skeleton.assetId, std::move(skeleton));
		}
	}
}

const SkeletonResource* SkeletonResourceStore::Find(Assets::CookedAssetId skeletonAssetId) const noexcept
{
	const auto found = m_resources.find(skeletonAssetId);
	return found == m_resources.end() ? nullptr : &found->second;
}
