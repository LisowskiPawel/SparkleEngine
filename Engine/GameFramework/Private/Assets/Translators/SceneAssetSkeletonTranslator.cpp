#include "PCH.h"

#include "Assets/Translators/SceneAssetSkeletonTranslator.h"

#include "Assets/Translators/OzzConstructionBudget.h"
#include "Core/Public/Diagnostics/Error.h"

#include <ozz/animation/offline/raw_skeleton.h>
#include <ozz/animation/offline/skeleton_builder.h>

#include <array>
#include <utility>

namespace Assets
{
	SkeletonResource BuildSceneAssetSkeleton(const LoadedSkeletonAsset& asset, CookedAssetId skeletonAssetId, std::uint32_t sourceSkinIndex, CookedAssetAllocationBudget& budget)
	{
		ReserveOzzConstruction(budget, asset.evaluationJoints.size(), 0);
		ozz::animation::offline::RawSkeleton raw;
		std::array<std::uint32_t, kMaxAnimationEvaluationJoints + 1> childCounts{};
		std::array<std::uint32_t, kMaxAnimationEvaluationJoints + 1> nextChild{};
		std::array<ozz::animation::offline::RawSkeleton::Joint*, kMaxAnimationEvaluationJoints> nodes{};
		for (const auto& joint : asset.evaluationJoints)
		{
			const auto parent = joint.parentJointIndex == kInvalidCookedSkeletonJointIndex ? kMaxAnimationEvaluationJoints : joint.parentJointIndex;
			++childCounts[parent];
		}
		raw.roots.resize(childCounts[kMaxAnimationEvaluationJoints]);
		for (std::uint32_t index = 0; index < asset.evaluationJoints.size(); ++index)
		{
			const auto& joint = asset.evaluationJoints[index];
			const auto parent = joint.parentJointIndex == kInvalidCookedSkeletonJointIndex ? kMaxAnimationEvaluationJoints : joint.parentJointIndex;
			auto& siblings = parent == kMaxAnimationEvaluationJoints ? raw.roots : nodes[parent]->children;
			auto& node = siblings[nextChild[parent]++];
			nodes[index] = &node;
			node.children.resize(childCounts[index]);
			node.transform.translation = {joint.translation.x, joint.translation.y, joint.translation.z};
			node.transform.rotation = {joint.rotation.x, joint.rotation.y, joint.rotation.z, joint.rotation.w};
			node.transform.scale = {joint.scale.x, joint.scale.y, joint.scale.z};
		}
		ozz::animation::offline::SkeletonBuilder builder;
		auto evaluation = builder(raw);
		if (!evaluation || evaluation->num_joints() != asset.evaluationJoints.size())
		{
			throw Diagnostics::Error("Could not construct the animation-preview skeleton");
		}

		SkeletonResource skeleton;
		skeleton.assetId = skeletonAssetId;
		skeleton.sourceSkinIndex = sourceSkinIndex;
		skeleton.evaluation = std::move(*evaluation);
		skeleton.joints.reserve(asset.joints.size());
		for (const auto& record : asset.joints)
		{
			skeleton.joints.push_back({record.name, record.sourceNodeIndex, record.evaluationJointIndex, record.inverseBindMatrix});
		}
		return skeleton;
	}
}
