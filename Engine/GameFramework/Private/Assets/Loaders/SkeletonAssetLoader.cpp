#include "PCH.h"

#include "Assets/Loaders/SkeletonAssetLoader.h"
#include "GameFramework/Public/Assets/Cooked/CookedSkeletonAsset.h"
#include "Animation/SkeletonResource.h"

#include "Assets/Loaders/CookedAssetByteReader.h"
#include "Assets/Loaders/OzzConstructionBudget.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"
#include "GameFramework/Public/Scene/Animations/SkeletonTransformContract.h"

#include <array>
#include <cmath>
#include <vector>
#include <ozz/animation/offline/raw_skeleton.h>
#include <ozz/animation/offline/skeleton_builder.h>
#include <utility>

namespace Assets
{
	struct SkeletonRecords final
	{
		CookedSkeletonAssetHeader header;
		std::vector<CookedSkeletonJointRecord> joints;
		std::vector<CookedSkeletonEvaluationJointRecord> evaluationJoints;
	};

	static void ValidateEvaluationJoints(const SkeletonRecords& asset)
	{
		std::array<std::uint32_t, kMaxAnimationEvaluationJoints> ancestors;
		std::size_t depth = 0;
		for (std::uint32_t index = 0; index < asset.evaluationJoints.size(); ++index)
		{
			const auto& joint = asset.evaluationJoints[index];
			if (joint.parentJointIndex == kInvalidCookedSkeletonJointIndex)
			{
				depth = 0;
			}
			else
			{
				while (depth > 0 && ancestors[depth - 1] != joint.parentJointIndex)
				{
					--depth;
				}
				if (depth == 0)
				{
					throw Diagnostics::Error("Cooked skeleton must use depth-first parent-before-child evaluation order; recook the asset");
				}
			}
			ancestors[depth++] = index;
			const float components[] = {joint.translation.x, joint.translation.y, joint.translation.z, joint.scale.x, joint.scale.y, joint.scale.z};
			for (float component : components)
			{
				if (!std::isfinite(component) || std::abs(component) > 65504.0f)
				{
					throw Diagnostics::Error("Cooked skeleton contains invalid evaluation transforms");
				}
			}
			const auto rotation = DirectX::XMLoadFloat4(&joint.rotation);
			const float lengthSquared = DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(rotation));
			if (!std::isfinite(lengthSquared) || std::abs(lengthSquared - 1.0f) > 1.0e-3f)
			{
				throw Diagnostics::Error("Cooked skeleton contains an invalid evaluation rotation");
			}
		}
	}

	static void ValidateSkinJoints(const SkeletonRecords& asset)
	{
		std::array<bool, kMaxAnimationEvaluationJoints> mapped{};
		for (std::size_t index = 0; index < asset.joints.size(); ++index)
		{
			const auto& joint = asset.joints[index];
			const bool bValidName = Strings::IsNullTerminated(std::span(joint.name));
			const bool bValidNode = joint.sourceNodeIndex != kInvalidCookedSkeletonJointIndex;
			const bool bValidMapping = joint.evaluationJointIndex < asset.evaluationJoints.size();
			if (!bValidName || !bValidNode || !bValidMapping)
			{
				throw Diagnostics::Error("Cooked skeleton has invalid skin identity or evaluation mapping");
			}
			if (mapped[joint.evaluationJointIndex])
			{
				throw Diagnostics::Error("Cooked skeleton maps multiple skin joints to one evaluation joint");
			}
			mapped[joint.evaluationJointIndex] = true;
			for (std::size_t previous = 0; previous < index; ++previous)
			{
				if (asset.joints[previous].sourceNodeIndex == joint.sourceNodeIndex)
				{
					throw Diagnostics::Error("Cooked skeleton repeats a source node identity");
				}
			}
			const bool bFinite = SkeletonTransformContract::IsFinite(joint.inverseBindMatrix);
			const bool bInvertible = bFinite && SkeletonTransformContract::IsInvertible(joint.inverseBindMatrix);
			if (!bInvertible)
			{
				throw Diagnostics::Error("Cooked skeleton contains an invalid inverse bind matrix");
			}
		}
	}

	static SkeletonResource BuildSkeleton(const SkeletonRecords& asset, CookedAssetId skeletonAssetId, CookedAssetAllocationBudget& budget)
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
		skeleton.evaluation = std::move(*evaluation);
		skeleton.joints.reserve(asset.joints.size());
		for (const auto& record : asset.joints)
		{
			skeleton.joints.push_back({record.sourceNodeIndex, record.evaluationJointIndex, record.inverseBindMatrix});
		}
		return skeleton;
	}

	SkeletonResource SkeletonAssetLoader::Load(std::span<const std::uint8_t> bytes, CookedAssetId skeletonAssetId, CookedAssetAllocationBudget& budget)
	{
		CookedAssetByteReader reader(bytes, budget);
		SkeletonRecords asset;
		asset.header = reader.Read<CookedSkeletonAssetHeader>();
		const auto& header = asset.header;
		const bool bSkinLayout = header.fileHeader.HasMagic(kCookedSkeletonAssetMagic) && header.jointStride == sizeof(CookedSkeletonJointRecord);
		const bool bCurrentLayout = bSkinLayout && header.evaluationJointStride == sizeof(CookedSkeletonEvaluationJointRecord);
		const bool bSkinCount = header.jointCount > 0 && header.jointCount <= kMaxAnimationEvaluationJoints;
		const bool bEvaluationCount = header.evaluationJointCount >= header.jointCount && header.evaluationJointCount <= kMaxAnimationEvaluationJoints;
		if (!bCurrentLayout || header.flags != 0 || !bSkinCount || !bEvaluationCount)
		{
			throw Diagnostics::Error("Invalid cooked animation-preview skeleton header or capacity; recook the asset");
		}
		asset.joints = reader.ReadArray<CookedSkeletonJointRecord>(header.jointCount);
		asset.evaluationJoints = reader.ReadArray<CookedSkeletonEvaluationJointRecord>(header.evaluationJointCount);
		ValidateEvaluationJoints(asset);
		ValidateSkinJoints(asset);
		if (reader.GetRemainingByteCount() != 0)
		{
			throw Diagnostics::Error("Cooked skeleton asset contains unexpected trailing bytes");
		}
		budget.ReserveArray<SkeletonJoint>(asset.joints.size());
		return BuildSkeleton(asset, skeletonAssetId, budget);
	}
}
