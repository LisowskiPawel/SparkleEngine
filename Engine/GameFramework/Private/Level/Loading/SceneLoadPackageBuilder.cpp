#include "PCH.h"

#include "Level/Loading/SceneLoadPackageBuilder.h"
#include "Level/Loading/SceneLoadWorkState.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Hash/HashUtils.h"

#include <cstdint>
#include <format>
#include <string_view>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <utility>

static std::uint64_t MakeAuthoredInstanceId(std::string_view identity)
{
	const std::uint64_t hash = Hash::Fnv1a64(identity);
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

	static void ValidateInstanceIdentities(std::span<const SceneAssetLoadWork> assets)
	{
		std::unordered_map<std::uint64_t, std::string_view> instanceIdentities;
		for (const SceneAssetLoadWork& work : assets)
		{
			const auto [existing, bInserted] = instanceIdentities.emplace(work.Payload.authoredInstanceId, work.Id.value);
			if (work.Payload.authoredInstanceId == 0 || (!bInserted && existing->second != work.Id.value))
			{
				throw Diagnostics::Error("Scene load package contains an invalid or colliding authored instance identity.");
			}
		}
	}

	static void ValidateSkeletonBindings(std::span<const SceneAssetLoadWork> assets)
	{
		std::unordered_set<Assets::CookedAssetId> skeletonAssets;
		for (const SceneAssetLoadWork& work : assets)
		{
			for (const SkeletonResource& skeleton : work.Payload.skeletons)
			{
				skeletonAssets.insert(skeleton.assetId);
			}
		}
		for (const SceneAssetLoadWork& work : assets)
		{
			for (const ECS::SceneMeshInstanceData& mesh : work.Payload.meshes)
			{
				const bool bUnresolvedSkeleton = mesh.Kind == SceneMeshKind::Skeletal && !skeletonAssets.contains(mesh.SkeletonAssetId);
				if (bUnresolvedSkeleton)
				{
					throw Diagnostics::Error("Scene load package contains an unresolved cross-asset skeleton reference.");
				}
			}
		}
	}

	void FinalizeSceneLoadPackage(SceneLoadWorkState& state)
	{
		ValidateInstanceIdentities(state.Assets);
		ValidateSkeletonBindings(state.Assets);

		state.Package->AssetPayloads.reserve(state.Assets.size());
		for (SceneAssetLoadWork& work : state.Assets)
		{
			state.Package->AssetPayloads.push_back(std::move(work.Payload));
		}
	}
}
