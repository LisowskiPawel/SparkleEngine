#include "PCH.h"

#include "Level/Loading/SceneLoadPackageBuilder.h"
#include "Level/Loading/SceneLoadWorkState.h"
#include "Core/Public/Diagnostics/Error.h"

#include <cstdint>
#include <format>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>

static std::uint64_t MakeAuthoredInstanceId(std::string_view identity)
{
	constexpr std::uint64_t OffsetBasis = 14695981039346656037ull;
	constexpr std::uint64_t Prime = 1099511628211ull;
	std::uint64_t hash = OffsetBasis;
	for (char character : identity)
	{
		hash ^= static_cast<std::uint8_t>(character);
		hash *= Prime;
	}
	if (hash == 0)
	{
		throw Diagnostics::Error(std::format("Scene asset identity '{}' hashes to the reserved null identity.", identity));
	}
	return hash;
}


namespace Assets
{
	void PrepareSceneAssetForCommit(SceneAssetLoadWork& work)
	{
		work.Payload.authoredInstanceId = MakeAuthoredInstanceId(work.Id.value);
	}

	void FinalizeSceneLoadPackage(SceneLoadWorkState& state)
	{
		std::unordered_map<std::uint64_t, std::string_view> instanceIdentities;
		for (const SceneAssetLoadWork& work : state.Assets)
		{
			const auto [existing, inserted] = instanceIdentities.emplace(work.Payload.authoredInstanceId, work.Id.value);
			if (work.Payload.authoredInstanceId == 0 || (!inserted && existing->second != work.Id.value))
			{
				throw Diagnostics::Error("Scene load package contains an invalid or colliding authored instance identity.");
			}
		}
		std::unordered_set<Assets::CookedAssetId> skeletonAssets;
		for (const SceneAssetLoadWork& work : state.Assets)
		{
			for (const SkeletonResource& skeleton : work.Payload.skeletons)
			{
				skeletonAssets.insert(skeleton.assetId);
			}
		}
		for (const SceneAssetLoadWork& work : state.Assets)
		{
			for (const SceneAssetPayload::SkeletalMeshInstance& mesh : work.Payload.skeletalMeshInstances)
			{
				if (!skeletonAssets.contains(mesh.skeletonAssetId))
				{
					throw Diagnostics::Error("Scene load package contains an unresolved cross-asset skeleton reference.");
				}
			}
		}

		state.Package->AssetPayloads.reserve(state.Assets.size());
		for (SceneAssetLoadWork& work : state.Assets)
		{
			state.Package->AssetPayloads.push_back(std::move(work.Payload));
		}
	}
}
