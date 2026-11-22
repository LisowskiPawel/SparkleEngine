#include "PCH.h"

#include "World/Resources/SkeletonResourceStore.h"

void SkeletonResourceStore::Append(std::vector<SkeletonResource> skeletons)
{
	m_entries.reserve(m_entries.size() + skeletons.size());
	for (SkeletonResource& skeleton : skeletons)
	{
		if (skeleton.assetId == Assets::InvalidCookedAssetId || m_byAssetId.contains(skeleton.assetId))
		{
			continue;
		}
		Entry entry;
		entry.Resource = std::move(skeleton);
		const auto slot = static_cast<std::uint32_t>(m_entries.size());
		m_entries.push_back(std::move(entry));
		m_byAssetId.emplace(m_entries.back().Resource.assetId, SkeletonResourceHandle{slot, m_entries.back().Generation});
	}
}

SkeletonResourceHandle SkeletonResourceStore::Find(Assets::CookedAssetId skeletonAssetId) const noexcept
{
	const auto found = m_byAssetId.find(skeletonAssetId);
	return found == m_byAssetId.end() ? SkeletonResourceHandle{} : found->second;
}

const SkeletonResource* SkeletonResourceStore::Resolve(SkeletonResourceHandle handle) const noexcept
{
	if (!handle.IsValid() || handle.Slot >= m_entries.size())
	{
		return nullptr;
	}
	const Entry& entry = m_entries[handle.Slot];
	return entry.Generation == handle.Generation ? &entry.Resource : nullptr;
}
