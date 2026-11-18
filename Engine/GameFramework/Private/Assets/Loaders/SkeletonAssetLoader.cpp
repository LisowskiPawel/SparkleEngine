#include "PCH.h"

#include "Assets/Loaders/SkeletonAssetLoader.h"

#include "Assets/Cooked/LoadedSkeletonAsset.h"
#include "Assets/Loaders/CookedAssetByteReader.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Strings/StringUtils.h"
#include "GameFramework/Public/Scene/Animations/SkeletonTransformContract.h"

#include <cstdint>
#include <limits>
#include <span>
#include <unordered_set>
#include <vector>

namespace Assets
{
	static void ValidateSkeletonHeader(const CookedSkeletonAssetHeader& header)
	{
		if (!header.fileHeader.HasMagic(kCookedSkeletonAssetMagic) || header.jointStride != sizeof(CookedSkeletonJointRecord))
		{
			throw Diagnostics::Error("Invalid cooked skeleton asset header; recook the asset");
		}
	}

	static void ValidateSkeletonJoint(std::span<const CookedSkeletonJointRecord> joints, std::size_t index, std::unordered_set<std::uint32_t>& nodes)
	{
		const auto& joint = joints[index];
		const bool bValidName = Strings::IsNullTerminated(std::span(joint.name));
		const bool bValidNode = joint.sourceNodeIndex != (std::numeric_limits<std::uint32_t>::max)();
		const bool bUniqueNode = nodes.insert(joint.sourceNodeIndex).second;
		const bool bParentInRange = joint.parentJointIndex < joints.size() && joint.parentJointIndex != index;
		const bool bValidParent = joint.parentJointIndex == kInvalidCookedSkeletonJointIndex || bParentInRange;
		if (!bValidName || !bValidNode)
		{
			throw Diagnostics::Error("Cooked skeleton joint identity is invalid");
		}
		if (!bUniqueNode || !bValidParent)
		{
			throw Diagnostics::Error("Cooked skeleton joint hierarchy is invalid");
		}

		const bool bFiniteBind = SkeletonTransformContract::IsFinite(joint.inverseBindMatrix) && SkeletonTransformContract::IsFinite(joint.bindLocalTransform);
		const bool bFiniteHierarchy = SkeletonTransformContract::IsFinite(joint.parentSpaceTransform) && SkeletonTransformContract::IsFinite(joint.bindModelTransform);
		const bool bInvertible = SkeletonTransformContract::IsInvertible(joint.inverseBindMatrix) && SkeletonTransformContract::IsInvertible(joint.bindLocalTransform);
		const auto* parent = bParentInRange ? &joints[joint.parentJointIndex].bindModelTransform : nullptr;
		const bool bBindInvariant = SkeletonTransformContract::SatisfiesBindInvariant(joint.bindLocalTransform, joint.parentSpaceTransform, parent, joint.bindModelTransform);
		if (!bFiniteBind || !bFiniteHierarchy)
		{
			throw Diagnostics::Error("Cooked skeleton contains non-finite transforms");
		}
		if (!bInvertible || !SkeletonTransformContract::IsTrsDecomposable(joint.bindLocalTransform))
		{
			throw Diagnostics::Error("Cooked skeleton violates the current transform contract; recook the spatial asset");
		}
		if (!bBindInvariant)
		{
			throw Diagnostics::Error("Cooked skeleton violates the current bind invariant; recook the spatial asset");
		}
	}

	static void ValidateSkeletonJoints(const LoadedSkeletonAsset& asset)
	{
		std::unordered_set<std::uint32_t> nodes;
		for (std::size_t index = 0; index < asset.joints.size(); ++index)
		{
			ValidateSkeletonJoint(asset.joints, index, nodes);
		}
		std::vector<std::uint32_t> order;
		if (asset.joints.empty() || !SkeletonTransformContract::BuildEvaluationOrder(asset.joints, order))
		{
			throw Diagnostics::Error("Cooked skeleton has an empty or cyclic hierarchy; recook the spatial asset");
		}
	}

	LoadedSkeletonAsset SkeletonAssetLoader::Decode(std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget)
	{
		CookedAssetByteReader reader(bytes, budget);
		LoadedSkeletonAsset asset;
		asset.header = reader.Read<CookedSkeletonAssetHeader>();
		ValidateSkeletonHeader(asset.header);
		asset.joints = reader.ReadArray<CookedSkeletonJointRecord>(asset.header.jointCount);
		ValidateSkeletonJoints(asset);
		if (reader.GetRemainingByteCount() != 0)
		{
			throw Diagnostics::Error("Cooked skeleton asset contains unexpected trailing bytes");
		}
		return asset;
	}
}
