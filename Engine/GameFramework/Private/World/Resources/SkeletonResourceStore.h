#pragma once

#include "Animation/SkeletonResource.h"

#include <cstddef>
#include <unordered_map>

#include <vector>

class SkeletonResourceStore final
{
public:
	void Append(std::vector<SkeletonResource> skeletons);
	const SkeletonResource* Find(Assets::CookedAssetId skeletonAssetId) const noexcept;

	std::size_t GetCount() const noexcept { return m_resources.size(); }

private:
	// Entries are append-only. Node addresses survive insertion and rehash until this world is retired.
	std::unordered_map<Assets::CookedAssetId, SkeletonResource> m_resources;
};
