#include "PCH.h"

#include "Assets/Loaders/SkeletonAssetLoader.h"

#include "Assets/Cooked/LoadedSkeletonAsset.h"
#include "Assets/Loaders/CookedAssetByteReader.h"
#include "Assets/Loaders/CookedAssetLoaderDiagnostics.h"
#include "Core/Public/Strings/StringUtils.h"
#include "GameFramework/Public/Scene/Animations/SkeletonTransformContract.h"

#include <cstdint>
#include <limits>
#include <span>
#include <unordered_set>
#include <vector>

namespace Assets
{
	LoadedSkeletonAsset SkeletonAssetLoader::Decode(const std::filesystem::path& path, std::span<const std::uint8_t> bytes, CookedAssetAllocationBudget& budget) const
	{
		const CookedAssetLoaderDiagnostics diagnostics(path, "CookedSkeletonAsset");

		CookedAssetByteReader reader(bytes, budget);
		LoadedSkeletonAsset skeletonAsset;
		skeletonAsset.header = reader.Read<CookedSkeletonAssetHeader>();

		const bool bSkeletonHeaderInvalid = !skeletonAsset.header.fileHeader.HasMagic(kCookedSkeletonAssetMagic) || skeletonAsset.header.jointStride != sizeof(CookedSkeletonJointRecord);
		
		if (bSkeletonHeaderInvalid)
		{
			throw diagnostics.MakeError("header", "skeleton magic and current joint stride", "Invalid cooked skeleton asset header; recook the asset");
		}

		skeletonAsset.joints = reader.ReadArray<CookedSkeletonJointRecord>(skeletonAsset.header.jointCount);

		std::unordered_set<std::uint32_t> sourceNodeIndices;
		for (std::size_t jointIndex = 0; jointIndex < skeletonAsset.joints.size(); ++jointIndex)
		{
			const CookedSkeletonJointRecord& joint = skeletonAsset.joints[jointIndex];
			const DirectX::XMFLOAT4X4* parentBindModel = joint.parentJointIndex < skeletonAsset.joints.size() ? &skeletonAsset.joints[joint.parentJointIndex].bindModelTransform : nullptr;
			const bool bNameTerminated = Strings::IsNullTerminated(std::span(joint.name));
			const bool bSourceNodeValid = bNameTerminated && joint.sourceNodeIndex != (std::numeric_limits<std::uint32_t>::max)();
			const bool bSourceNodeUnique = bSourceNodeValid && sourceNodeIndices.insert(joint.sourceNodeIndex).second;

			const bool bParentValidJointIndex = bSourceNodeUnique && (joint.parentJointIndex == kInvalidCookedSkeletonJointIndex);
			const bool bParentValidGuardJointIndex = bSourceNodeUnique && !bParentValidJointIndex;
			const bool bParentValidjoints = bParentValidGuardJointIndex && (joint.parentJointIndex < skeletonAsset.joints.size());
			const bool bParentValidJointIndex2 = bParentValidjoints && (joint.parentJointIndex != jointIndex);
			const bool bParentValidJointIndex3 = bParentValidJointIndex || bParentValidJointIndex2;
			const bool bParentValid = bSourceNodeUnique && bParentValidJointIndex3;

			const bool bInverseBindFinite = bParentValid && SkeletonTransformContract::IsFinite(joint.inverseBindMatrix);
			const bool bLocalBindFinite = bInverseBindFinite && SkeletonTransformContract::IsFinite(joint.bindLocalTransform);
			const bool bParentSpaceFinite = bLocalBindFinite && SkeletonTransformContract::IsFinite(joint.parentSpaceTransform);
			const bool bModelBindFinite = bParentSpaceFinite && SkeletonTransformContract::IsFinite(joint.bindModelTransform);
			const bool bInverseBindInvertible = bModelBindFinite && SkeletonTransformContract::IsInvertible(joint.inverseBindMatrix);
			const bool bLocalBindInvertible = bInverseBindInvertible && SkeletonTransformContract::IsInvertible(joint.bindLocalTransform);
			const bool bLocalBindDecomposable = bLocalBindInvertible && SkeletonTransformContract::IsTrsDecomposable(joint.bindLocalTransform);

			const bool bIndInvariantSatisfied = bLocalBindDecomposable
			    && SkeletonTransformContract::SatisfiesBindInvariant(joint.bindLocalTransform, joint.parentSpaceTransform, parentBindModel, joint.bindModelTransform);

			if (!bIndInvariantSatisfied)
			{
				throw diagnostics.MakeError(
				    "joints",
				    "unique nodes, valid parents, finite invertible TRS, and the canonical bind invariant",
				    "Cooked skeleton violates the current transform contract; recook the spatial asset");
			}
		}

		std::vector<std::uint32_t> evaluationOrder;
		if (skeletonAsset.joints.empty() || !SkeletonTransformContract::BuildEvaluationOrder(skeletonAsset.joints, evaluationOrder))
		{
			throw diagnostics.MakeError("joints", "one or more joints in an acyclic hierarchy", "Cooked skeleton has an empty or cyclic hierarchy; recook the spatial asset");
		}

		if (reader.GetRemainingByteCount() != 0)
		{
			throw diagnostics.MakeError("payload", "no trailing bytes after declared skeleton records", "Cooked skeleton asset contains unexpected trailing bytes");
		}

		return skeletonAsset;
	}
}
